/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Compiler/IdeError.hpp>
#include <Cm.Build/Build.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Sym/InitDone.hpp>
#include <Cm.Ast/InitDone.hpp>
#include <Cm.Emit/InitDone.hpp>
#include <Cm.Parser/FileRegistry.hpp>
#include <Cm.Parsing/InitDone.hpp>
#include <Cm.Sym/Exception.hpp>
#include <Cm.Sym/Warning.hpp>
#include <Cm.Core/GlobalSettings.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <Cm.Util/Path.hpp>
#include <Cm.IrIntf/BackEnd.hpp>
#include <chrono>
#include <iostream>

#if defined(_MSC_VER) && !defined(NDEBUG)
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

const char* version = "1.2.0";

struct InitDone
{
    InitDone()
    {
        Cm::Parsing::Init();
        Cm::Ast::Init();
        Cm::Sym::Init();
        Cm::Emit::Init();
    }
    ~InitDone()
    {
        Cm::Emit::Done();
        Cm::Sym::Done();
        Cm::Ast::Done();
        Cm::Parsing::Done();
        Cm::Parser::FileRegistry::Done();
    }
};

const char* CompilerName()
{
#ifdef NDEBUG
    return "cmc";
#else
    return "cmcd";
#endif
}

const char* CompilerMode()
{
#ifdef NDEBUG
    return "release";
#else
    return "debug";
#endif
}

std::string GetTargetTriple()
{
    std::string targetTriple;
    char* cmTargetTriple = getenv("CM_TARGET_TRIPLE");
    if (cmTargetTriple)
    {
        targetTriple = cmTargetTriple;
    }
    if (targetTriple.empty())
    {

#ifdef _WIN32

    #if defined(_M_X64)
        targetTriple = "x86_64-w64-windows-gnu";
    #else
        targetTriple = "i686-pc-windows-gnu";
    #endif

#elif defined(__linux) || defined(__unix) || defined(__posix)

    #if defined(__x86_64__)
        targetTriple = "x86_64-pc-linux-gnu";
    #else
        targetTriple = "i686-pc-linux-gnu";
    #endif

#else

#error unknown platform

#endif

    }

    return targetTriple;
}

std::string GetDataLayout()
{
    std::string dataLayout;
    char* cmDataLayout = getenv("CM_TARGET_DATALAYOUT");
    if (cmDataLayout)
    {
        dataLayout = cmDataLayout;
    }
    if (dataLayout.empty())
    {

#ifdef _WIN32

#if defined(_M_X64)
        dataLayout  = "e-m:e-i64:64-f80:128-n8:16:32:64-S128";
#else
        dataLayout = "e-m:w-p:32:32-i64:64-f80:32-n8:16:32-S32";
#endif

#elif defined(__linux) || defined(__unix) || defined(__posix)

#if defined(__x86_64__)
        dataLayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128";
#else
        dataLayout = "e-m:w-p:32:32-i64:64-f80:32-n8:16:32-S32";
#endif

#else

#error unknown platform

#endif

    }

    return dataLayout;
}

int main(int argc, const char** argv)
{
#if defined(_MSC_VER) && !defined(NDEBUG)
    int dbgFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    dbgFlags |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(dbgFlags);
    //_CrtSetBreakAlloc(20195464);
#endif //  defined(_MSC_VER) && !defined(NDEBUG)
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    InitDone initDone;
    Cm::Core::GlobalSettings globalSettings;
    Cm::Core::SetGlobalSettings(&globalSettings);
    std::unordered_set<std::string> defines;
    try
    {
        std::vector<std::string> solutionOrProjectFilePaths;
        std::vector<std::string> compileFileNames;
        Cm::IrIntf::BackEnd backend = Cm::IrIntf::BackEnd::llvm;
        bool rebuild = false;
        bool prevWasCompile = false;
        bool prevWasDefine = false;
        bool prevWasTargetTriple = false;
        bool prevWasDatalayout = false;
        bool emitNoTriple = false;
        bool emitNoLayout = false;
        std::string targetTriple = GetTargetTriple();
        std::string datalayout = GetDataLayout();
        if (argc < 2)
        {
            std::cout << "Cmajor " << CompilerMode() << " mode compiler version " << version << std::endl;
            std::cout << "Usage: " << CompilerName() << " [options] {file.cms | file.cmp}" << std::endl;
            std::cout << "Compile Cmajor solution file.cms or project file.cmp" << std::endl;
            std::cout <<
                "options:\n" <<
                "-R              : rebuild project or solution\n" <<
                "-clean          : clean project or solution\n" <<
                "-c FILENAME     : compile only FILENAME, do not link\n" <<
                "-config=debug   : use debug configuration (default)\n" <<
                "-config=release : use release configuration\n" <<
                "-config=profile : instrument program/library with profiling enabled\n" <<
                "-D SYMBOL       : define conditional compilation symbol SYMBOL\n" << 
                "-O=<n> (n=0-3)  : set optimization level to <n> (default: debug:0, release:3)\n" <<
                "-backend=llvm   : use LLVM backend (default)\n" <<
                "-backend=c      : use C backend\n" <<
                "-m TRIPLE       : override LLVM target triple to emit to .ll files\n" << 
                "-emit-no-triple : do not emit any LLVM target triple to .ll files\n" <<
                "-d DATALAYOUT   : override LLVM target datalayout to emit to .ll files\n" <<
                "-emit-no-layout : do not emit any LLVM datalayout to .ll files\n" <<
                "-emit-opt       : generate optimized LLVM code to <file>.opt.ll\n" <<
                "-quiet          : write no output messages for successful compiles\n" << 
                "-trace          : instrument program/library with tracing enabled\n" <<
                "-debug_heap     : instrument program/library with debug heap enabled\n" <<
                "-no_call_stacks : do not generate call stack information for exceptions\n" <<
                std::endl;
            std::cout << "If no -m option is given, LLVM target triple is obtained from environment variable CM_TARGET_TRIPLE. " << 
                "If there is no CM_TARGET_TRIPLE environment variable, default target triple is used unless option -emit-no-triple is given." << std::endl;
            std::cout << "If no -d option is given, LLVM target datalayout is obtained from environment variable CM_TARGET_DATALAYOUT. " << 
                "If there is no CM_TARGET_DATALAYOUT environment variable, default datalayout is used unless option -emit-no-layout is given." << std::endl; 
        }
        else
        {
            for (int i = 1; i < argc; ++i)
            {
                std::string arg = argv[i];
                if (!arg.empty())
                {
                    if (arg[0] == '-')
                    {
                        if (arg.find('=') != std::string::npos)
                        {
                            std::vector<std::string> v = Cm::Util::Split(arg, '=');
                            if (v.size() == 2)
                            {
                                if (v[0] == "-config")
                                {
                                    const std::string& config = v[1];
                                    if (config != "debug" && config != "release" && config != "profile")
                                    {
                                        throw std::runtime_error("unknown configuration '" + config + "'");
                                    }
                                    Cm::Core::GetGlobalSettings()->SetConfig(config);
                                    if (config == "release" || config == "profile")
                                    {
                                        Cm::Sym::SetGlobalFlag(Cm::Sym::GlobalFlags::optimize);
                                    }
                                }
                                else if (v[0] == "-O")
                                {
                                    std::string levelStr = v[1];
                                    int level = std::stoi(levelStr);
                                    Cm::Core::GetGlobalSettings()->SetOptimizationLevel(level);
                                }
                                else if (v[0] == "-backend")
                                {
                                    std::string backendStr = v[1];
                                    if (backendStr == "llvm")
                                    {
                                        backend = Cm::IrIntf::BackEnd::llvm;
                                    }
                                    else if (backendStr == "c")
                                    {
                                        backend = Cm::IrIntf::BackEnd::c;
                                    }
                                    else
                                    {
                                        throw std::runtime_error("unknown backend '" + backendStr + "'");
                                    }
                                }
                                else
                                {
                                    throw std::runtime_error("unknown argument '" + arg + "'");
                                }
                            }
                            else
                            {
                                throw std::runtime_error("unknown argument '" + arg + "'");
                            }
                        }
                        else if (arg == "-R")
                        {
                            rebuild = true;
                        }
                        else if (arg == "-clean")
                        {
                            Cm::Sym::SetGlobalFlag(Cm::Sym::GlobalFlags::clean);
                        }
                        else if (arg == "-c")
                        {
                            prevWasCompile = true;
                        }
                        else if (arg == "-D")
                        {
                            prevWasDefine = true;
                        }
                        else if (arg == "-m")
                        {
                            prevWasTargetTriple = true;
                        }
                        else if (arg == "-d")
                        {
                            prevWasDatalayout = true;
                        }
                        else if (arg == "-emit-opt")
                        {
                            Cm::Sym::SetGlobalFlag(Cm::Sym::GlobalFlags::emitOpt);
                        }
                        else if (arg == "-emit-no-triple")
                        {
                            emitNoTriple = true;
                        }
                        else if (arg == "-emit-no-layout")
                        {
                            emitNoLayout = true;
                        }
                        else if (arg == "-quiet")
                        {
                            Cm::Sym::SetGlobalFlag(Cm::Sym::GlobalFlags::quiet);
                        }
                        else if (arg == "-trace")
                        {
                            Cm::Sym::SetGlobalFlag(Cm::Sym::GlobalFlags::trace);
                        }
                        else if (arg == "-debug_heap")
                        {
                            Cm::Sym::SetGlobalFlag(Cm::Sym::GlobalFlags::debug_heap);
                        }
                        else if (arg == "-no_call_stacks")
                        {
                            Cm::Sym::SetGlobalFlag(Cm::Sym::GlobalFlags::no_call_stacks);
                        }
                        else if (arg == "-ide")
                        {
                            Cm::Sym::SetGlobalFlag(Cm::Sym::GlobalFlags::ide);
                        }
                        else
                        {
                            throw std::runtime_error("unknown argument '" + arg + "'");
                        }
                    }
                    else
                    {
                        if (prevWasCompile)
                        {
                            compileFileNames.push_back(arg);
                            prevWasCompile = false;
                        }
                        else if (prevWasDefine)
                        {
                            defines.insert(arg);
                            prevWasDefine = false;
                        }
                        else if (prevWasTargetTriple)
                        {
                            targetTriple = arg;
                            prevWasTargetTriple = false;
                        }
                        else if (prevWasDatalayout)
                        {
                            datalayout = arg;
                            prevWasDatalayout = false;
                        }
                        else
                        {
                            std::string ext = Cm::Util::Path::GetExtension(arg);
                            if (ext != ".cms" && ext != ".cmp")
                            {
                                throw std::runtime_error(arg + " is not Cmajor solution or project file");
                            }
                            solutionOrProjectFilePaths.push_back(arg);
                        }
                    }
                }
            }
            Cm::IrIntf::SetBackEnd(backend);
            if (Cm::Core::GetGlobalSettings()->Config() == "debug" && backend == Cm::IrIntf::BackEnd::c)
            {
                Cm::Sym::SetGlobalFlag(Cm::Sym::GlobalFlags::generate_debug_info);
            }
            bool quiet = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::quiet);
            if (!quiet)
            {
                std::cout << "Cmajor " << CompilerMode() << " mode compiler version " << version << std::endl;
            }
            if (backend == Cm::IrIntf::BackEnd::llvm)
            {
                if (!emitNoTriple)
                {
                    Cm::Core::GetGlobalSettings()->SetTargetTriple(targetTriple);
                    if (!quiet)
                    {
                        std::cout << "using LLVM target triple = \"" << targetTriple << "\"" << std::endl;;
                    }
                }
                else
                {
                    if (!quiet)
                    {
                        std::cout << "emitting no LLVM target triple" << std::endl;
                    }
                }
                if (!emitNoLayout)
                {
                    Cm::Core::GetGlobalSettings()->SetDatalayout(datalayout);
                    if (!quiet)
                    {
                        std::cout << "using LLVM target datalayout = \"" << datalayout << "\"" << std::endl;;
                    }
                }
                else
                {
                    if (!quiet)
                    {
                        std::cout << "emitting no LLVM target datalayout" << std::endl;
                    }
                }
            }
            if (!compileFileNames.empty())
            {
                if (solutionOrProjectFilePaths.empty())
                {
                    throw std::runtime_error("solution or project must be specified when compiling single files");
                }
            }
            for (const std::string& solutionOrProjectFilePath : solutionOrProjectFilePaths)
            {
                std::string ext = Cm::Util::Path::GetExtension(solutionOrProjectFilePath);
                if (ext == ".cms")
                {
                    Cm::Build::BuildSolution(solutionOrProjectFilePath, rebuild, compileFileNames, defines);
                }
                else if (ext == ".cmp")
                {
                    Cm::Build::BuildProject(solutionOrProjectFilePath, rebuild, compileFileNames, defines);
                }
                else
                {
                    throw std::runtime_error(solutionOrProjectFilePath + " is not Cmajor solution or project file");
                }
            }
            if (!Cm::Sym::CompileWarningCollection::Instance().Warnings().empty())
            {
                if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::ide))
                {
                    Cm::Compiler::IdeErrorCollection errorCollection;
                    errorCollection.AddWarnings(Cm::Sym::CompileWarningCollection::Instance().Warnings());
                    std::cerr << errorCollection << std::endl;
                }
            }
        }
        bool quiet = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::quiet);
        if (!quiet && !solutionOrProjectFilePaths.empty())
        {
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            auto dur = end - start;
            long long totalSecs = std::chrono::duration_cast<std::chrono::seconds>(dur).count() + 1;
            int hours = static_cast<int>(totalSecs / 3600);
            int mins = static_cast<int>((totalSecs / 60) % 60);
            int secs = static_cast<int>(totalSecs % 60);
            std::cout <<
                (hours > 0 ? std::to_string(hours) + " hour" + ((hours != 1) ? "s " : " ") : "") <<
                (mins > 0 ? std::to_string(mins) + " minute" + ((mins != 1) ? "s " : " ") : "") <<
                secs << " second" << ((secs != 1) ? "s" : "") << std::endl;
        }
    }
    catch (const Cm::Parsing::ExpectationFailure& ex)
    {
        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::ide))
        {
            Cm::Compiler::IdeErrorCollection errorCollection(ex);
            errorCollection.AddWarnings(Cm::Sym::CompileWarningCollection::Instance().Warnings());
            std::cerr << errorCollection << std::endl;
            std::cout << ex.what() << std::endl;
        }
        else
        {
            std::cerr << ex.what() << std::endl;
        }
        return 1;
    }
    catch (const Cm::Sym::Exception& ex)
    {
        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::ide))
        {
            Cm::Compiler::IdeErrorCollection errorCollection(ex);
            errorCollection.AddWarnings(Cm::Sym::CompileWarningCollection::Instance().Warnings());
            std::cerr << errorCollection << std::endl;
            std::cout << ex.what() << std::endl;
        }
        else
        {
            std::cerr << ex.what() << std::endl;
        }
        return 1;
    }
    catch (const Cm::Core::Exception& ex)
    {
        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::ide))
        {
            Cm::Compiler::IdeErrorCollection errorCollection(ex);
            errorCollection.AddWarnings(Cm::Sym::CompileWarningCollection::Instance().Warnings());
            std::cerr << errorCollection << std::endl;
            std::cout << ex.what() << std::endl;
        }
        else
        {
            std::cerr << ex.what() << std::endl;
        }
        return 1;
    }
    catch (const Cm::Core::ToolErrorExcecption& ex)
    {
        const Cm::Util::ToolError& error = ex.Error();
        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::ide))
        {
            Cm::Compiler::IdeErrorCollection errorCollection(ex);
            errorCollection.AddWarnings(Cm::Sym::CompileWarningCollection::Instance().Warnings());
            std::cerr << errorCollection << std::endl;
            std::cout << error.ToolName() << ": " << error.Message() << " in file " << error.FilePath() << " line " << error.Line() << " column " << error.Column() << std::endl;
        }
        else
        {
            std::cerr << error.ToolName() << ": " << error.Message() << " in file " << error.FilePath() << " line " << error.Line() << " column " << error.Column() << std::endl;
        }
        return 1;
    }
    catch (const std::exception& ex)
    {
        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::ide))
        {
            Cm::Compiler::IdeErrorCollection errorCollection(ex);
            errorCollection.AddWarnings(Cm::Sym::CompileWarningCollection::Instance().Warnings());
            std::cerr << errorCollection << std::endl;
            std::cout << ex.what() << std::endl;
        }
        else
        {
            std::cerr << ex.what() << std::endl;
        }
        return 1;
    }
    catch (...)
    {
        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::ide))
        {
            Cm::Compiler::IdeErrorCollection errorCollection("unknown exception occurred");
            errorCollection.AddWarnings(Cm::Sym::CompileWarningCollection::Instance().Warnings());
            std::cerr << errorCollection << std::endl;
            std::cout << "unknown exception occurred" << std::endl;
        }
        else
        {
            std::cerr << "unknown exception occurred" << std::endl;
        }
        return 1;
    }
    return 0;
}
