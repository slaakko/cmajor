/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

const char* version = "1.3.0";

#include <Cm.Debugger/DebugInfo.hpp>
#include <Cm.Debugger/Gdb.hpp>
#include <Cm.Debugger/Shell.hpp>
#include <Cm.Debugger/IdeOutput.hpp>
#include <Cm.Debugger/IdeInput.hpp>
#include <Cm.Debugger/StdIoLineStream.hpp>
#include <Cm.Debugger/SocketLineStream.hpp>
#include <Sockets/SocketLib.hpp>
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
        Sockets::InitSockets();
    }
    ~InitDone()
    {
        Sockets::DoneSockets();
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
        int ioPort = -1;
        int errorPort = -1;
        if (argc < 2)
        {
            std::cout << "Cmajor Debugger version " << version << "\n" <<
                "Usage: cmdb [options] <executable> [arguments...]\n" << 
                "options:\n" << 
                "-ide               : use IDE command reply format\n" <<
                "-ioport=PORT       : receive commands and send replies using socket connected to PORT\n" <<
                "-errorport=PORT    : send errors to socket connected to PORT\n" << 
                "-file=FILE         : read commands from FILE" <<
                std::endl;
            return 1;
        }
        std::string commandFileName;
        std::string executable;
        std::vector<std::string> arguments;
        bool firstNonOption = true;
        bool receiveProgramArguments = false;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (!receiveProgramArguments && Cm::Util::StartsWith(arg, "-"))
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
                        else if (components[0] == "-ioport")
                        {
                            ioPort = std::stoi(components[1]);
                        }
                        else if (components[0] == "-errorport")
                        {
                            errorPort = std::stoi(components[1]);
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
                receiveProgramArguments = true;
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
        if (executable.empty())
        {
            std::cerr << "no executable specified" << std::endl;
            return 1;
        }
#ifdef _WIN32
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
        std::unique_ptr<Cm::Debugger::LineStream> ioLineStream;
        std::unique_ptr<Cm::Debugger::LineStream> errorLineStream;
        if (ioPort != -1)
        {
            Sockets::TcpSocket listenSocket;
            listenSocket.Bind(ioPort);
            listenSocket.Listen(100);
            std::cout << "listening port " << ioPort << std::endl;
            Sockets::TcpSocket ioSocket = listenSocket.Accept();
            std::cout << "connection to " << ioPort << " accepted" << std::endl;
            ioLineStream.reset(new Cm::Debugger::SocketLineStream(std::move(ioSocket)));
        }
        else
        {
            ioLineStream.reset(new Cm::Debugger::StdIoLineStream());
        }
        Cm::Debugger::SetIoLineStream(ioLineStream.get());
        if (errorPort != -1)
        {
            Sockets::TcpSocket listenSocket;
            listenSocket.Bind(errorPort);
            listenSocket.Listen(100);
            std::cout << "listening port " << errorPort << std::endl;
            Sockets::TcpSocket errorSocket = listenSocket.Accept();
            std::cout << "connection to " << errorPort << " accepted" << std::endl;
            errorLineStream.reset(new Cm::Debugger::SocketLineStream(std::move(errorSocket)));
        }
        else
        {
            errorLineStream.reset(new Cm::Debugger::StdErrorLineStream());
        }
        Cm::Debugger::SetErrorLineStream(errorLineStream.get());
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
            Cm::Debugger::ErrorLineStream()->WriteLine(ex.what());
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
            Cm::Debugger::ErrorLineStream()->WriteLine("unknown exception occurred");
        }
        return 1;
    }
    return 0;
}