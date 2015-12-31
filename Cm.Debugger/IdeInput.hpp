/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_IDE_INPUT_INCLUDED
#define CM_DEBUGGER_IDE_INPUT_INCLUDED
#include <Cm.Core/CDebugInfo.hpp>
#include <Cm.Core/Json.hpp>
#include <string>
#include <memory>

namespace Cm { namespace Debugger {

class Command;
typedef std::shared_ptr<Command> CommandPtr;

class IdeCommand
{
public:
    IdeCommand(int sequenceNumber_);
    virtual ~IdeCommand();
    virtual void SetDataFrom(Cm::Core::JsonValue* jsonValue);
    virtual CommandPtr ToShellCommand() const;
    virtual bool IsInputCommand() const { return false; }
    int SequenceNumber() const { return sequenceNumber; }
private:
    int sequenceNumber;
};

class IdeErrorCommand : public IdeCommand
{
public:
    IdeErrorCommand(int sequenceNumber_);
    IdeErrorCommand(int sequenceNumber_, const std::string& errorMessage_);
    CommandPtr ToShellCommand() const override;
private:
    std::string errorMessage;
};

class IdeInputCommand : public IdeCommand
{
public:
    IdeInputCommand(int sequenceNumber_);
    void SetDataFrom(Cm::Core::JsonValue* jsonValue) override;
    bool IsInputCommand() const override { return true; }
    const std::string& Input() const { return input; }
private:
    std::string input;
};

class IdeStartCommand : public IdeCommand
{
public:
    IdeStartCommand(int sequenceNumber_);
    CommandPtr ToShellCommand() const override;
};

class IdeQuitCommand : public IdeCommand
{
public:
    IdeQuitCommand(int sequenceNumber_);
    CommandPtr ToShellCommand() const override;
};

class IdeContinueCommand : public IdeCommand
{
public:
    IdeContinueCommand(int sequenceNumber_);
    CommandPtr ToShellCommand() const override;
};

class IdeStepOverCommand : public IdeCommand
{
public:
    IdeStepOverCommand(int sequenceNumber_);
    CommandPtr ToShellCommand() const override;
};

class IdeStepIntoCommand : public IdeCommand
{
public:
    IdeStepIntoCommand(int sequenceNumber_);
    CommandPtr ToShellCommand() const override;
};

class IdeStepOutCommand : public IdeCommand
{
public:
    IdeStepOutCommand(int sequenceNumber_);
    CommandPtr ToShellCommand() const override;
};

class IdeBreakCommand : public IdeCommand
{
public:
    IdeBreakCommand(int sequenceNumber_);
    CommandPtr ToShellCommand() const override;
    void SetDataFrom(Cm::Core::JsonValue* jsonValue) override;
private:
    Cm::Core::SourceFileLine sourceFileLine;
};

class IdeClearCommand : public IdeCommand
{
public:
    IdeClearCommand(int sequenceNumber_);
    CommandPtr ToShellCommand() const override;
    void SetDataFrom(Cm::Core::JsonValue* jsonValue) override;
private:
    int breakpointNumber;
};

class IdeCallStackCommand : public IdeCommand
{
public:
    IdeCallStackCommand(int sequenceNumber_);
    CommandPtr ToShellCommand() const override;
};

class IdeFrameCommand : public IdeCommand
{
public:
    IdeFrameCommand(int sequenceNumber_);
    CommandPtr ToShellCommand() const override;
    void SetDataFrom(Cm::Core::JsonValue* jsonValue) override;
private:
    int frameNumber;
};

class IdeShowBreakpointsCommand : public IdeCommand
{
public:
    IdeShowBreakpointsCommand(int sequenceNumber_);
    CommandPtr ToShellCommand() const override;
};

class IdeSetBreakOnThrowCommand : public IdeCommand
{
public:
    IdeSetBreakOnThrowCommand(int sequenceNumber_);
    CommandPtr ToShellCommand() const override;
    void SetDataFrom(Cm::Core::JsonValue* jsonValue) override;
private:
    bool enable;
};

class IdeSourcesCommand : public IdeCommand
{
public:
    IdeSourcesCommand(int sequenceNumber_);
    CommandPtr ToShellCommand() const override;
};

class IdeInspectCommand : public IdeCommand
{
public:
    IdeInspectCommand(int sequenceNumber_);
    CommandPtr ToShellCommand() const override;
    void SetDataFrom(Cm::Core::JsonValue* jsonValue) override;
private:
    std::string expr;
};

std::unique_ptr<IdeCommand> ParseIdeCommand(const std::string& commandLine);

void InitIdeInput();
void DoneIdeInput();

} } // Cm::Debugger

#endif // CM_DEBUGGER_IDE_INPUT_INCLUDED
