/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/Gdb.hpp>
#include <Cm.Util/System.hpp>
#include <Cm.Util/TextUtils.hpp>

namespace Cm { namespace Debugger {

std::mutex pipeReadMutex;
std::mutex pipeWriteMutex;
std::mutex commandMutex;

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

GdbQuitCommand::GdbQuitCommand() : GdbCommand("quit")
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
    program(program_), args(args_), gdbHandle(-1), hasReply(false), hasCommand(false), firstStart(true), gdbKilled(false)
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

void Gdb::Run()
{
    gdbThread = std::thread(RunGdb, this);
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
                std::string replyMessage = Read();
                cmd->SetReplyMessage(replyMessage);
            }
            hasReply = true;
            replyReady.notify_one();
            if (cmd->IsQuitCommand()) break;
        }
    }
    catch (const std::exception& ex)
    {
        // todo
    }
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

void Gdb::ExecuteCommand(std::shared_ptr<GdbCommand>& command)
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
