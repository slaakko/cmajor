/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Unit/Test.hpp>
#include <Cm.Build/Build.hpp>
#include <Cm.Core/GlobalSettings.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.Sym/InitDone.hpp>
#include <Cm.Ast/InitDone.hpp>
#include <Cm.Parsing/InitDone.hpp>
#include <Cm.Parser/LlvmVersion.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <Cm.Util/Path.hpp>
#include <Cm.Util/System.hpp>
#include <string>
#include <vector>
#include <chrono>
#include <iostream>

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

const char* version = "1.4.0";

const char* Name()
{
#ifdef NDEBUG
    return "cmunit";
#else
    return "cmunitd";
#endif
}

const char* Mode()
{
#ifdef NDEBUG
    return "release";
#else
    return "debug";
#endif
}

void ObtainLlvmVersion()
{
    boost::filesystem::path llvmVersionPath;
    try
    {
        std::vector<std::string> libraryDirectories;
        Cm::Build::GetLibraryDirectories(libraryDirectories);
        if (libraryDirectories.empty())
        {
            throw std::runtime_error("no library directories");
        }
        std::string command = "llc --version";
        llvmVersionPath = libraryDirectories[0];
        llvmVersionPath /= "llvmver.txt";
        Cm::Util::System(command, 1, llvmVersionPath.generic_string(), true); // ignore return value
        std::string versionString = Cm::Util::ReadFile(llvmVersionPath.generic_string());
        Cm::Parser::LlvmVersionParser* versionParser = Cm::Parser::LlvmVersionParser::Create();
        Cm::Ast::ProgramVersion llvmVersion = versionParser->Parse(versionString.c_str(), versionString.c_str() + versionString.length(), 0, llvmVersionPath.generic_string());
        bool quiet = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::quiet);
        if (!quiet)
        {
            std::cout << "LLVM version is " << llvmVersion.ToString() << " (" << llvmVersion.VersionText() << ")" << std::endl;
        }
        Cm::Core::GetGlobalSettings()->SetLlvmVersion(llvmVersion);
        boost::filesystem::remove(llvmVersionPath);
    }
    catch (const std::exception& ex)
    {
        boost::filesystem::remove(llvmVersionPath);
        throw std::runtime_error(std::string("could not obtain LLVM compiler version (llc --version): ") + ex.what());
    }
}

void PrintVersion()
{
    std::cout << "Cmajor " << Mode() << " mode unit test engine version " << version << std::endl;
}

void PrintHelp()
{
    PrintVersion();
    std::cout << "Usage: " << Name() << " [options] {file.cms | file.cmp}" << std::endl;
    std::cout << "Compile and run unit tests in solution file.cms or project file.cmp" << std::endl;
    std::cout <<
        "options:\n" <<
        "-config=debug   : use debug configuration (default)\n" <<
        "-config=release : use release configuration\n" <<
        "-backend=llvm   : use LLVM backend (default)\n" <<
        "-backend=c      : use C backend\n" <<
        "-file=FILE      : run only unit tests in file FILE\n" <<
        "-test=TEST      : run only unit test TEST\n" <<
        "-D SYMBOL       : define conditional compilation symbol SYMBOL\n" <<
        std::endl;
}

int main(int argc, const char** argv)
{
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    Cm::Core::GlobalSettings globalSettings;
    Cm::Core::SetGlobalSettings(&globalSettings);
    Cm::Sym::SetGlobalFlag(Cm::Sym::GlobalFlags::quiet);
    Cm::Sym::SetGlobalFlag(Cm::Sym::GlobalFlags::unit_test);
    try
    {
        InitDone initDone;
        std::vector<std::string> solutionOrProjectFilePaths;
        Cm::IrIntf::BackEnd backend = Cm::IrIntf::BackEnd::llvm;
        std::string fileName;
        std::string testName;
        if (argc < 2)
        {
            PrintHelp();
        }
        else
        {
            bool prevWasDefine = false;
            std::unordered_set<std::string> defines;
            for (int i = 1; i < argc; ++i)
            {
                std::string arg = argv[i];
                if (!arg.empty())
                {
                    if (arg[0] == '-')
                    {
                        if (arg == "-D")
                        { 
                            prevWasDefine = true;
                        }
                        else if (arg == "-help" || arg == "--help")
                        {
                            PrintHelp();
                            return 0;
                        }
                        else if (arg == "-version" || arg == "--version")
                        {
                            PrintVersion();
                            return 0;
                        }
                        else if (arg.find('=') != std::string::npos)
                        {
                            std::vector<std::string> v = Cm::Util::Split(arg, '=');
                            if (v.size() == 2)
                            {
                                if (v[0] == "-config")
                                {
                                    const std::string& config = v[1];
                                    if (config != "debug" && config != "release" && config != "profile" && config != "full")
                                    {
                                        throw std::runtime_error("unknown configuration '" + config + "'");
                                    }
                                    Cm::Core::GetGlobalSettings()->SetConfig(config);
                                    if (config == "release" || config == "profile" || config == "full")
                                    {
                                        Cm::Sym::SetGlobalFlag(Cm::Sym::GlobalFlags::optimize);
                                        if (config == "full")
                                        {
                                            Cm::Sym::SetGlobalFlag(Cm::Sym::GlobalFlags::fullConfig);
                                        }
                                    }
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
                                else if (v[0] == "-file")
                                {
                                    fileName = v[1];
                                }
                                else if (v[0] == "-test")
                                {
                                    testName = v[1];
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
                        else
                        {
                            throw std::runtime_error("unknown argument '" + arg + "'");
                        }
                    }
                    else if (prevWasDefine)
                    {
                        defines.insert(arg);
                        prevWasDefine = false;
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
            if (backend == Cm::IrIntf::BackEnd::llvm)
            {
                ObtainLlvmVersion();
            }
            bool passed = true;
            if (solutionOrProjectFilePaths.empty())
            {
                throw std::runtime_error("no project or solution given");
            }
            for (const std::string& solutionOrProjectFilePath : solutionOrProjectFilePaths)
            {
                std::string ext = Cm::Util::Path::GetExtension(solutionOrProjectFilePath);
                if (ext == ".cms")
                {
                    bool solutionPassed = Cm::Unit::TestSolution(solutionOrProjectFilePath, fileName, testName, defines);
                    if (!solutionPassed)
                    {
                        passed = false;
                    }
                }
                else if (ext == ".cmp")
                {
                    bool projectPassed = Cm::Unit::TestProject(solutionOrProjectFilePath, fileName, testName, defines);
                    if (!projectPassed)
                    {
                        passed = false;
                    }
                }
                else
                {
                    throw std::runtime_error(solutionOrProjectFilePath + " is not Cmajor solution or project file");
                }
            }
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
            return passed ? 0 : 1;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 3;
    }
    catch (...)
    {
        std::cerr << "unknown exception occurred" << std::endl;
        return 2;
    }
    return 0;
}
