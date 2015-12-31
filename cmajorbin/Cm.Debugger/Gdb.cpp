/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/Gdb.hpp>
#include <Cm.Debugger/GdbReply.hpp>
#include <Cm.Debugger/DebugInfo.hpp>
#include <Cm.Debugger/IdeOutput.hpp>
#include <Cm.Debugger/LineStream.hpp>
#include <Cm.Util/System.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <iostream>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif

namespace Cm { namespace Debugger {

std::mutex pipeReadMutex;
std::mutex pipeWriteMutex;
std::mutex commandMutex;

ContinueReplyData::ContinueReplyData() : exitCode(0), exitCodeSet(false), signalSet(false)
{
}

GdbCommand::GdbCommand(const std::string& message_) : message(message_)
{
}

GdbCommand::~GdbCommand()
{
}

void GdbCommand::SetReplyMessage(const std::string& replyMessage_)
{
    replyMessage = replyMessage_;
}

GdbSetWidthUnlimitedCommand::GdbSetWidthUnlimitedCommand() : GdbCommand("set width 0")
{
}

GdbSetHeightUnlimitedCommand::GdbSetHeightUnlimitedCommand() : GdbCommand("set height 0")
{
}

GdbSetPrintElementsCommand::GdbSetPrintElementsCommand() : GdbCommand("set print elements 0")
{
}

GdbStartCommand::GdbStartCommand() : GdbCommand("start")
{
}

GdbQuitCommand::GdbQuitCommand() : GdbCommand("quit")
{
}

GdbBreakCommand::GdbBreakCommand(const std::string& cFileLine_) : GdbCommand("break " + cFileLine_)
{
}

GdbContinueCommand::GdbContinueCommand() : GdbCommand("continue")
{
}

GdbClearCommand::GdbClearCommand(const std::string& cFileLine_) : GdbCommand("clear " + cFileLine_)
{
}

GdbBackTraceCommand::GdbBackTraceCommand() : GdbCommand("backtrace")
{
}

GdbFrameCommand::GdbFrameCommand(int frameNumber) : GdbCommand("frame " + std::to_string(frameNumber))
{
}

GdbPrintCommand::GdbPrintCommand(const std::string& expr) : GdbCommand("print " + expr)
{
}

unsigned long long SpawnGdb(std::vector<std::string> args)
{
    std::string gdb = "gdb";
    unsigned long long gdbHandle = Cm::Util::Spawn(gdb, args);
    return gdbHandle;
}

void RunGdb(Gdb* gdb)
{
    gdb->DoRun();
}

Gdb::Gdb(const std::string& program_, const std::vector<std::string>& args_) : 
    program(program_), args(args_), gdbHandle(-1), hasReply(false), hasCommand(false), firstStart(true), gdbKilled(false), started(false)
{
    std::vector<std::string> gdbArgs;
    if (!args.empty())
    {
        gdbArgs.push_back("--args");
    }
    gdbArgs.push_back(program);
    if (!args.empty())
    {
        gdbArgs.insert(gdbArgs.end(), args.begin(), args.end());
    }
    std::vector<int> oldHandles;
    Cm::Util::RedirectStdHandlesToPipes(oldHandles, pipeHandles);
    gdbHandle = SpawnGdb(gdbArgs);
    Cm::Util::RestoreStdHandles(oldHandles);
}

void RunErrorReader(Gdb* gdb)
{
    try
    {
        int errorPipeHandle = gdb->GetErrorPipeHandle();
        static char buffer[8192];
        int bytesRead = Cm::Util::ReadFromPipe(errorPipeHandle, buffer, sizeof(buffer) - 1);
        while (bytesRead > 0)
        {
            buffer[bytesRead] = 0;
            std::string buf(buffer);
            std::vector<std::string> lines = Cm::Util::Split(buf, '\n');
            for (const std::string& line : lines)
            {
                if (gdb->Started())
                {
                    ErrorLineStream()->WriteLine(line);
                }
            }
            bytesRead = Cm::Util::ReadFromPipe(errorPipeHandle, buffer, sizeof(buffer) - 1);
        }
    }
    catch (...)
    {
    }
}

void Gdb::Run()
{
    gdbThread = std::thread(RunGdb, this);
    errorReaderThread = std::thread(RunErrorReader, this);
}

void Gdb::DoRun()
{
    try
    {
        std::string welcome = Read();
        while (true)
        {
            std::unique_lock<std::mutex> commandLock(commandReadyMtx);
            commandReady.wait(commandLock, [this] { return hasCommand; });
            hasCommand = false;
            std::shared_ptr<GdbCommand> cmd = commandToExecute;
            commandToExecute = nullptr;
            std::lock_guard<std::mutex> replyLock(replyReadyMtx);
            std::string message = cmd->Message();
            message.append("\n");
            Write(message);
            if (cmd->ReplyExpected())
            {
                if (cmd->IsContinueCommand())
                {
                    GdbContinueCommand* continueCommand = static_cast<GdbContinueCommand*>(cmd.get());
                    ContinueReplyData continueReplyData = ReadContinueReply();
                    continueCommand->SetContinueReplyData(continueReplyData);
                }
                else
                {
                    std::string replyMessage = Read();
                    cmd->SetReplyMessage(replyMessage);
                }
            }
            hasReply = true;
            replyReady.notify_one();
            if (cmd->IsQuitCommand()) break;
        }
    }
    catch (const std::exception& ex)
    {
        if (ide)
        {
            IdePrintError(-1, std::string("gdb component stopped running: ") + ex.what());
        }
        else
        {
            ErrorLineStream()->WriteLine(std::string("stopped running: ") + ex.what());
        }
    }
}

std::shared_ptr<GdbCommand> Gdb::Start()
{
    if (firstStart)
    {
        firstStart = false;
        ExecuteCommand(std::shared_ptr<GdbCommand>(new GdbSetWidthUnlimitedCommand()));
        ExecuteCommand(std::shared_ptr<GdbCommand>(new GdbSetHeightUnlimitedCommand()));
        ExecuteCommand(std::shared_ptr<GdbCommand>(new GdbSetPrintElementsCommand()));
    }
    std::shared_ptr<GdbCommand> startCommand(new GdbStartCommand());
    ExecuteCommand(startCommand);
    started = true;
    return startCommand;
}

void Gdb::Quit()
{
    if (gdbHandle != -1)
    {
        ExecuteCommand(std::shared_ptr<GdbCommand>(new GdbQuitCommand()));
        Cm::Util::Wait(gdbHandle);
        gdbHandle = -1;
    }
    gdbThread.join();
    errorReaderThread.join();
}

std::shared_ptr<GdbCommand> Gdb::Break(const std::string& cFileLine)
{
    std::shared_ptr<GdbCommand> breakCommand(new GdbBreakCommand(cFileLine));
    ExecuteCommand(breakCommand);
    return breakCommand;
}

std::shared_ptr<GdbContinueCommand> Gdb::Continue()
{
    std::shared_ptr<GdbContinueCommand> continueCommand(new GdbContinueCommand());
    std::shared_ptr<GdbCommand> command = continueCommand;
    ExecuteCommand(command);
    return continueCommand;
}

std::shared_ptr<GdbCommand> Gdb::Clear(const std::string& cFileLine)
{
    std::shared_ptr<GdbCommand> clearCommand(new GdbClearCommand(cFileLine));
    ExecuteCommand(clearCommand);
    return clearCommand;
}

std::shared_ptr<GdbCommand> Gdb::BackTrace()
{
    std::shared_ptr<GdbCommand> backTraceCommand(new GdbBackTraceCommand());
    ExecuteCommand(backTraceCommand);
    return backTraceCommand;
}

std::shared_ptr<GdbCommand> Gdb::Frame(int frameNumber)
{
    std::shared_ptr<GdbCommand> frameCommand(new GdbFrameCommand(frameNumber));
    ExecuteCommand(frameCommand);
    return frameCommand;
}

std::shared_ptr<GdbCommand> Gdb::Print(const std::string& expr)
{
    std::shared_ptr<GdbCommand> printCommand(new GdbPrintCommand(expr));
    ExecuteCommand(printCommand);
    return printCommand;
}

std::string Gdb::Read()
{
    static char buf[8192];
    std::string gdbPrompt("(gdb) ");
    std::string message;
    while (!Cm::Util::EndsWith(message, gdbPrompt))
    {
        int bytesRead = -1;
        {
            std::lock_guard<std::mutex> lck(pipeReadMutex);
            bytesRead = Cm::Util::ReadFromPipe(pipeHandles[1], buf, sizeof(buf) - 1);
        }
        if (bytesRead == -1)
        {
            std::string errorMessage = strerror(errno);
            throw std::runtime_error(errorMessage);
        }
        if (bytesRead > 0)
        {
            message.append(Cm::Util::NarrowString(buf, bytesRead));
        }
        else
        {
            break;
        }
    }
    return Cm::Util::Trim(message.substr(0, message.length() - gdbPrompt.length()));
}

ContinueReplyGrammar* continueReplyGrammar = nullptr;

ContinueReplyData Gdb::ReadContinueReply()
{
    if (!continueReplyGrammar)
    {
        continueReplyGrammar = ContinueReplyGrammar::Create();
    }
    static char buf[8192];
    std::string gdbPrompt("(gdb) ");
    ContinueReplyData data;
    ContinueReplyState state = ContinueReplyState::start;
    std::string combinedMessage;
    while (state != ContinueReplyState::prompt)
    {
        std::string message;
        int bytesRead = -1;
        {
            std::lock_guard<std::mutex> lck(pipeReadMutex);
            bytesRead = Cm::Util::ReadFromPipe(pipeHandles[1], buf, sizeof(buf) - 1);
        }
        if (bytesRead == -1)
        {
            std::string errorMessage = strerror(errno);
            throw std::runtime_error(errorMessage);
        }
        if (bytesRead > 0)
        {
            message.append(Cm::Util::NarrowString(buf, bytesRead));
            combinedMessage.append(message);
            std::string consoleLines;
            bool consoleLinesSet = false;
            std::vector<std::string> lines = Cm::Util::Split(message, '\n');
            for (const std::string& line : lines)
            {
                switch (state)
                {
                    case ContinueReplyState::start:
                    {
                        state = continueReplyGrammar->Parse(line.c_str(), line.c_str() + line.length(), 0, "", &data);
                        if (state == ContinueReplyState::consoleLine)
                        {
                            if (consoleLinesSet)
                            {
                                consoleLines.append("\n");
                            }
                            consoleLines.append(data.ConsoleLine());
                            consoleLinesSet = true;
                            state = ContinueReplyState::start;
                        }
                        else if (state == ContinueReplyState::exit)
                        {
                            state = ContinueReplyState::start;
                        }
                        else if (state == ContinueReplyState::continuing)
                        {
                            state = ContinueReplyState::start;
                        }
                        break;
                    }
                    case ContinueReplyState::breakpoint:
                    {
                        state = ContinueReplyState::start;
                        break;
                    }
                    case ContinueReplyState::signal:
                    {
                        state = ContinueReplyState::start;
                        break;
                    }
                }
            }
            if (consoleLinesSet && !data.SignalSet())
            {
                if (ide)
                {
                    IdePrintOutput(-1, consoleLines);
                }
                else
                {
                    if (!consoleLines.empty() && consoleLines[consoleLines.size() - 1] == '\n')
                    {
                        consoleLines = consoleLines.substr(0, consoleLines.size() - 1);
                    }
                    IoLineStream()->WriteLine(consoleLines);
                }
            }
        }
    }
    return data;
}

void Gdb::Write(const std::string& message)
{
    int bytesWritten = -1;
    {
        std::lock_guard<std::mutex> lck(pipeWriteMutex);
        bytesWritten = Cm::Util::WriteToPipe(pipeHandles[0], (void*)message.c_str(), (unsigned int)message.length());
    }
    if (bytesWritten == -1)
    {
        std::string errorMessage = strerror(errno);
        throw std::runtime_error(errorMessage);
    }
}

void Gdb::ExecuteCommand(std::shared_ptr<GdbCommand> command)
{
    hasReply = false;
    hasCommand = false;
    std::lock_guard<std::mutex> lock(commandMutex);
    {
        std::lock_guard<std::mutex> lck(commandReadyMtx);
        commandToExecute = command;
        hasCommand = true;
        commandReady.notify_one();
    }
    std::unique_lock<std::mutex> lck(replyReadyMtx);
    replyReady.wait(lck, [this] { return hasReply; });
    hasReply = false;
}

} } // Cm::Debugger
