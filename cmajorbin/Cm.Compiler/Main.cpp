/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Compiler/IdeError.hpp>
#include <Cm.Build/Build.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.Core/GlobalSettings.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Sym/InitDone.hpp>
#include <Cm.Ast/InitDone.hpp>
#include <Cm.Sym/Exception.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Parsing/InitDone.hpp>
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

const char* version = "1.0.0-beta-4";

struct InitDone
{
    InitDone()
    {
        Cm::Parsing::Init();
        Cm::Ast::Init();
        Cm::Sym::Init();
    }
    ~InitDone()
    {
        Cm::Sym::Done();
        Cm::Ast::Done();
        Cm::Parsing::Done();
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

int main(int argc, const char** argv)
{
#if defined(_MSC_VER) && !defined(NDEBUG)
    int dbgFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    dbgFlags |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(dbgFlags);
   //_CrtSetBreakAlloc(1679620);
#endif //  defined(_MSC_VER) && !defined(NDEBUG)
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    Cm::Core::GlobalSettings globalSettings;
    Cm::Core::SetGlobalSettings(&globalSettings);
    try
    {
        InitDone initDone;
        std::vector<std::string> solutionOrProjectFilePaths;
        Cm::IrIntf::BackEnd backend = Cm::IrIntf::BackEnd::llvm;
        if (argc < 2)
        {
            std::cout << "Cmajor " << CompilerMode() << " mode compiler version " << version << std::endl;
            std::cout << "Usage: " << CompilerName() << " [options] {file.cms | file.cmp}" << std::endl;
            std::cout << "Compile Cmajor solution file.cms or project file.cmp" << std::endl;
            std::cout <<
                "options:\n" <<
                "-clean          : clean project or solution\n" <<
                "-config=debug   : use debug configuration (default)\n" <<
                "-config=release : use release configuration\n" <<
                "-O=<n>          : set optimization level to <n> (default: debug:0, release:3)\n" <<
                "-backend=llvm   : use LLVM backend (default)\n" <<
                "-backend=c      : use C backend\n" <<
                "-emit-opt       : generate optimized LLVM code to <file>.opt.ll\n" <<
                "-quiet          : write no output messages for successful compiles\n" << 
                "-trace          : instrument program/library with tracing enabled\n" <<
                "-debug_heap     : instrument program/library with debug heap enabled\n" <<
                std::endl;
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
                                    if (config != "debug" && config != "release")
                                    {
                                        throw std::runtime_error("unknown configuration '" + config + "'");
                                    }
                                    Cm::Core::GetGlobalSettings()->SetConfig(config);
                                    if (config == "release")
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
                        else if (arg == "-clean")
                        {
                            Cm::Sym::SetGlobalFlag(Cm::Sym::GlobalFlags::clean);
                        }
                        else if (arg == "-emit-opt")
                        {
                            Cm::Sym::SetGlobalFlag(Cm::Sym::GlobalFlags::emitOpt);
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
                        std::string ext = Cm::Util::Path::GetExtension(arg);
                        if (ext != ".cms" && ext != ".cmp")
                        {
                            throw std::runtime_error(arg + " is not Cmajor solution or project file");
                        }
                        solutionOrProjectFilePaths.push_back(arg);
                    }
                }
            }
            Cm::IrIntf::SetBackEnd(backend);
            bool quiet = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::quiet);
            if (!quiet)
            {
                std::cout << "Cmajor " << CompilerMode() << " mode compiler version " << version << std::endl;
            }
            for (const std::string& solutionOrProjectFilePath : solutionOrProjectFilePaths)
            {
                std::string ext = Cm::Util::Path::GetExtension(solutionOrProjectFilePath);
                if (ext == ".cms")
                {
                    Cm::Build::BuildSolution(solutionOrProjectFilePath);
                }
                else if (ext == ".cmp")
                {
                    Cm::Build::BuildProject(solutionOrProjectFilePath);
                }
                else
                {
                    throw std::runtime_error(solutionOrProjectFilePath + " is not Cmajor solution or project file");
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
    catch (const Cm::Parsing::CombinedParsingError& ex)
    {
        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::ide))
        {
            Cm::Compiler::IdeErrorCollection errorCollection(ex);
            std::cerr << errorCollection << std::endl;
        }
        for (const Cm::Parsing::ExpectationFailure& exp : ex.Errors())
        {
            if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::ide))
            {
                std::cout << exp.what() << std::endl;
            }
            else
            {
                std::cerr << exp.what() << std::endl;
            }
        }
        return 4;
    }
    catch (const Cm::Sym::Exception& ex)
    {
        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::ide))
        {
            Cm::Compiler::IdeErrorCollection errorCollection(ex);
            std::cerr << errorCollection << std::endl;
            std::cout << ex.what() << std::endl;
        }
        else
        {
            std::cerr << ex.what() << std::endl;
        }
    }
    catch (const Cm::Core::Exception& ex)
    {
        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::ide))
        {
            Cm::Compiler::IdeErrorCollection errorCollection(ex);
            std::cerr << errorCollection << std::endl;
            std::cout << ex.what() << std::endl;
        }
        else
        {
            std::cerr << ex.what() << std::endl;
        }
    }
    catch (const Cm::Core::ToolErrorExcecption& ex)
    {
        const Cm::Util::ToolError& error = ex.Error();
        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::ide))
        {
            Cm::Compiler::IdeErrorCollection errorCollection(ex);
            std::cerr << errorCollection << std::endl;
            std::cout << error.ToolName() << ": " << error.Message() << " in file " << error.FilePath() << " line " << error.Line() << " column " << error.Column() << std::endl;
        }
        else
        {
            std::cerr << error.ToolName() << ": " << error.Message() << " in file " << error.FilePath() << " line " << error.Line() << " column " << error.Column() << std::endl;
        }
        return 3;
    }
    catch (const std::exception& ex)
    {
        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::ide))
        {
            Cm::Compiler::IdeErrorCollection errorCollection(ex);
            std::cerr << errorCollection << std::endl;
            std::cout << ex.what() << std::endl;
        }
        else
        {
            std::cerr << ex.what() << std::endl;
        }
        return 2;
    }
    catch (...)
    {
        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::ide))
        {
            Cm::Compiler::IdeErrorCollection errorCollection("unknown exception occurred");
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
