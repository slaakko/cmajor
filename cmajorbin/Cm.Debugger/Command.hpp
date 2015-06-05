/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_COMMAND_INCLUDED
#define CM_DEBUGGER_COMMAND_INCLUDED
#include <Cm.Core/CDebugInfo.hpp>
#include <memory>

namespace Cm { namespace Debugger {

class DebugInfo;
class Gdb;
class InputReader;
class Shell;

class CommandError : public std::runtime_error
{
public:
    CommandError(int sequenceNumber_, const std::string& message_);
    int SequenceNumber() const { return sequenceNumber; }
private:
    int sequenceNumber;
};

class Command
{
public:
    Command(int sequenceNumber_);
    virtual ~Command();
    virtual void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell);
    virtual bool IsQuit() const { return false; }
    int SequenceNumber() const { return sequenceNumber; }
private:
    int sequenceNumber;
};

typedef std::shared_ptr<Command> CommandPtr;

class StartCommand : public Command
{
public:
    StartCommand(int sequenceNumber_);
    void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell) override;
};

class QuitCommand : public Command
{
public:
    QuitCommand(int sequenceNumber_);
    void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell) override;
    bool IsQuit() const override { return true; }
};

class HelpCommand : public Command
{
public:
    HelpCommand();
    void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell) override;
};

class ContinueCommand : public Command
{
public:
    ContinueCommand(int sequenceNumber_);
    void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell) override;
};

class NextCommand : public Command
{
public:
    NextCommand(int sequenceNumber_);
    void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell) override;
};

class StepCommand : public Command
{
public:
    StepCommand(int sequenceNumber_);
    void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell) override;
};

class OutCommand : public Command
{
public:
    OutCommand(int sequenceNumber_);
    void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell) override;
};

class BreakCommand : public Command
{
public:
    BreakCommand(int sequenceNumber_, const Cm::Core::SourceFileLine& sourceFileLine_);
    void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell) override;
private:
    Cm::Core::SourceFileLine sourceFileLine;
};

class ClearCommand : public Command
{
public:
    ClearCommand(int sequenceNumber_, int bpNum_);
    void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell) override;
private:
    int bpNum;
};

class ListCommand : public Command
{
public:
    ListCommand(const Cm::Core::SourceFileLine& sourceFileLine_);
    ListCommand(const std::string& command_);
    void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell) override;
private:
    std::string command;
    Cm::Core::SourceFileLine sourceFileLine;
};

class CallStackCommand : public Command
{
public:
    CallStackCommand(int sequenceNumber_);
    void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell) override;
};

class FrameCommand : public Command
{
public:
    FrameCommand(int sequenceNumber_, int frameNumber_);
    void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell) override;
private:
    int frameNumber;
};

class ShowBreakpointsCommand : public Command
{
public:
    ShowBreakpointsCommand(int sequenceNumber_);
    void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell) override;
};

class SetBreakOnThrowCommand : public Command
{
public:
    SetBreakOnThrowCommand(int sequenceNumber_, bool enabled_);
    void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell) override;
private:
    bool enabled;
};

class InspectCommand : public Command
{
public:
    InspectCommand(int sequenceNumber_, const std::string& expr_);
    void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell) override;
private:
    std::string expr;
};

} } // Cm::Debugger

#endif // CM_DEBUGGER_COMMAND_INCLUDED
