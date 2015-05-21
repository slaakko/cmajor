/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_GDB_INCLUDED
#define CM_DEBUGGER_GDB_INCLUDED
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace Cm { namespace Debugger {

class GdbCommand
{
public:
    GdbCommand(const std::string& message_);
    virtual ~GdbCommand();
    void SetReplyMessage(const std::string& replyMessage_);
    const std::string& Message() const { return message; }
    const std::string& ReplyMessage() const { return replyMessage; }
    virtual bool ReplyExpected() const { return true; }
    virtual bool IsQuitCommand() const { return false; }
private:
    std::string message;
    std::string replyMessage;
};

class GdbQuitCommand : public GdbCommand
{
public:
    GdbQuitCommand();
    bool IsQuitCommand() const override { return true; }
    bool ReplyExpected() const override { return false; }
};

class Gdb
{
public:
    Gdb(const std::string& program_, const std::vector<std::string>& args_);
    void Run();
    void DoRun();
    void Quit();
    std::string Read();
    void Write(const std::string& message);
    void ExecuteCommand(std::shared_ptr<GdbCommand>& command);
private:
    std::string program;
    std::vector<std::string> args;
    unsigned long long gdbHandle;
    std::vector<int> pipeHandles;
    std::thread gdbThread;
    std::shared_ptr<GdbCommand> commandToExecute;
    std::mutex commandReadyMtx;
    std::condition_variable commandReady;
    std::mutex replyReadyMtx;
    std::condition_variable replyReady;
    bool hasReply;
    bool hasCommand;
    bool firstStart;
    bool gdbKilled;
};

} } // Cm::Debugger

#endif // CM_DEBUGGER_GDB_INCLUDED
