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
    void SetExitCode(int exitCode_) { exitCode = exitCode_; exitCodeSet = true; }
    int ExitCode() const { return exitCode; }
    bool ExitCodeSet() const { return exitCodeSet; }
    void SetSignal(const std::string& signal_) { signal = signal_; signalSet = true; }
    const std::string& Signal() const { return signal; }
    bool SignalSet() const { return signalSet; }
private:
    std::string consoleLine;
    int exitCode;
    bool exitCodeSet;
    std::string signal;
    bool signalSet;
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

class GdbSetPrintElementsCommand : public GdbCommand
{
public:
    GdbSetPrintElementsCommand();
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
    void SetContinueReplyData(const ContinueReplyData& continueReplyData_) { continueReplyData = continueReplyData_; }
    const ContinueReplyData& GetContinueReplyData() const { return continueReplyData; }
private:
    ContinueReplyData continueReplyData;
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

class GdbFrameCommand : public GdbCommand
{
public:
    GdbFrameCommand(int frameNumber);
};

class GdbPrintCommand : public GdbCommand
{
public:    
    GdbPrintCommand(const std::string& expr);
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
    std::shared_ptr<GdbContinueCommand> Continue();
    std::shared_ptr<GdbCommand> Clear(const std::string& cFileLine);
    std::shared_ptr<GdbCommand> BackTrace();
    std::shared_ptr<GdbCommand> Frame(int frameNumber);
    std::shared_ptr<GdbCommand> Print(const std::string& expr);
    std::string Read();
    ContinueReplyData ReadContinueReply();
    void Write(const std::string& message);
    void ExecuteCommand(std::shared_ptr<GdbCommand> command);
    int GetErrorPipeHandle() const { return pipeHandles[2]; }
    bool Started() const { return started; }
private:
    std::string program;
    std::vector<std::string> args;
    unsigned long long gdbHandle;
    std::vector<int> pipeHandles;
    std::thread gdbThread;
    std::thread errorReaderThread;
    std::shared_ptr<GdbCommand> commandToExecute;
    std::mutex commandReadyMtx;
    std::condition_variable commandReady;
    std::mutex replyReadyMtx;
    std::condition_variable replyReady;
    bool hasReply;
    bool hasCommand;
    bool firstStart;
    bool started;
    bool gdbKilled;
};

} } // Cm::Debugger

#endif // CM_DEBUGGER_GDB_INCLUDED
