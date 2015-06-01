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

enum class ContinueReplyState
{
    start, continuing, exit, breakpoint, signal, prompt, consoleLine
};

class ContinueReplyData
{
public:
    ContinueReplyData();
    void SetConsoleLine(const std::string& consoleLine_) { consoleLine = consoleLine_; }
    const std::string& ConsoleLine() const { return consoleLine; }
    void SetExitCode(int exitCode_) { exitCode = exitCode_; }
    int ExitCode() const { return exitCode; }
private:
    std::string consoleLine;
    int exitCode;
};

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
    virtual bool IsContinueCommand() const { return false; }
private:
    std::string message;
    std::string replyMessage;
};

class GdbSetWidthUnlimitedCommand : public GdbCommand
{
public:
    GdbSetWidthUnlimitedCommand();
};

class GdbSetHeightUnlimitedCommand : public GdbCommand
{
public:
    GdbSetHeightUnlimitedCommand();
};

class GdbStartCommand : public GdbCommand
{
public:
    GdbStartCommand();
};

class GdbQuitCommand : public GdbCommand
{
public:
    GdbQuitCommand();
    bool IsQuitCommand() const override { return true; }
    bool ReplyExpected() const override { return false; }
};

class GdbBreakCommand : public GdbCommand
{
public:
    GdbBreakCommand(const std::string& cFileLine_);
};

class GdbContinueCommand : public GdbCommand
{
public:
    GdbContinueCommand();
    bool IsContinueCommand() const override { return true; }
};

class GdbClearCommand : public GdbCommand
{
public:
    GdbClearCommand(const std::string& cFileLine_);
};

class GdbBackTraceCommand : public GdbCommand
{
public:
    GdbBackTraceCommand();
};

class Gdb
{
public:
    Gdb(const std::string& program_, const std::vector<std::string>& args_);
    void Run();
    void DoRun();
    std::shared_ptr<GdbCommand> Start();
    void Quit();
    std::shared_ptr<GdbCommand> Break(const std::string& cFileLine);
    std::shared_ptr<GdbCommand> Continue();
    std::shared_ptr<GdbCommand> Clear(const std::string& cFileLine);
    std::shared_ptr<GdbCommand> BackTrace();
    std::string Read();
    std::string ReadContinueReply();
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
