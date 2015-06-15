/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

const char* version = "1.0.0-beta-5";

#include <Cm.Debugger/DebugInfo.hpp>
#include <Cm.Debugger/Gdb.hpp>
#include <Cm.Debugger/Shell.hpp>
#include <Cm.Debugger/IdeOutput.hpp>
#include <Cm.Debugger/IdeInput.hpp>
#include <Cm.Sym/InitDone.hpp>
#include <Cm.Ast/InitDone.hpp>
#include <Cm.Emit/InitDone.hpp>
#include <Cm.Parsing/InitDone.hpp>
#include <Cm.Util/Path.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>

struct InitDone
{
    InitDone()
    {
        Cm::Parsing::Init();
        Cm::Ast::Init();
        Cm::Sym::Init();
        Cm::Emit::Init();
        Cm::Debugger::InitIdeInput();
    }
    ~InitDone()
    {
        Cm::Debugger::DoneIdeInput();
        Cm::Emit::Done();
        Cm::Sym::Done();
        Cm::Ast::Done();
        Cm::Parsing::Done();
    }
};

int main(int argc, const char** argv)
{
    try
    {
        InitDone initDone;
        if (argc < 2)
        {
            std::cout << "Cmajor Debugger version " << version << "\n" <<
                "Usage: cmdb [options] <executable> [arguments...]\n" << 
                "options:\n" << 
                "-ide       : use IDE command reply format\n" << 
                "-file=FILE : read commands from FILE" <<
                std::endl;
            return 1;
        }
        std::string commandFileName;
        std::string executable;
        std::vector<std::string> arguments;
        bool firstNonOption = true;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (Cm::Util::StartsWith(arg, "-"))
            {
                if (arg == "-ide")
                {
                    Cm::Debugger::ide = true;
                }
                else if (arg.find('=') != std::string::npos)
                {
                    std::vector<std::string> components = Cm::Util::Split(arg, '=');
                    if (components.size() == 2)
                    {
                        if (components[0] == "-file")
                        {
                            commandFileName = components[1];
                        }
                        else
                        {
                            throw std::runtime_error("unknown option '" + arg + "'");
                        }
                    }
                    else
                    {
                        throw std::runtime_error("unknown option '" + arg + "'");
                    }
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else if (firstNonOption)
            {
                firstNonOption = false;
                executable = arg;
            }
            else
            {
                arguments.push_back(arg);
            }
        }
        if (!Cm::Debugger::ide)
        {
            std::cout << "Cmajor Debugger version " << version << std::endl;
        }
#ifdef WIN32
        std::string ext = Cm::Util::Path::GetExtension(executable);
        if (ext.empty())
        {
            executable.append(".exe");
        }
#endif
        if (!boost::filesystem::exists(executable))
        {
            throw std::runtime_error("executable '" + executable + "' not found");
        }
        std::string cmdbFilePath = Cm::Util::Path::ChangeExtension(executable, ".cmdb");
        if (!boost::filesystem::exists(cmdbFilePath))
        {
            throw std::runtime_error("debug information file '" + cmdbFilePath + "' not found");
        }
        Cm::Debugger::DebugInfo debugInfo(cmdbFilePath);
        Cm::Debugger::Gdb gdb(executable, arguments);
        Cm::Debugger::Shell shell(debugInfo, gdb, commandFileName);
        shell.Execute();
    }
    catch (const std::exception& ex)
    {
        if (Cm::Debugger::ide)
        {
            Cm::Debugger::IdePrintError(-1, ex.what());
        }
        else
        {
            std::cerr << ex.what() << std::endl;
        }
        return 1;
    }
    catch (...)
    {
        if (Cm::Debugger::ide)
        {
            Cm::Debugger::IdePrintError(-1, "unknown exception occurred");
        }
        else
        {
            std::cerr << "unknown exception occurred" << std::endl;
        }
        return 1;
    }
    return 0;
}