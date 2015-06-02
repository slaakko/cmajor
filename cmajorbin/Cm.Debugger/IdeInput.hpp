/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
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
    virtual ~IdeCommand();
    virtual void SetDataFrom(Cm::Core::JsonValue* jsonValue);
    virtual CommandPtr ToShellCommand() const;
    virtual bool IsInputCommand() const { return false; }
};

class IdeErrorCommand : public IdeCommand
{
public:
    IdeErrorCommand();
    IdeErrorCommand(const std::string& errorMessage_);
    CommandPtr ToShellCommand() const override;
private:
    std::string errorMessage;
};

class IdeInputCommand : public IdeCommand
{
public:
    void SetDataFrom(Cm::Core::JsonValue* jsonValue) override;
    bool IsInputCommand() const override { return true; }
    const std::string& Input() const { return input; }
private:
    std::string input;
};

class IdeStartCommand : public IdeCommand
{
public:
    CommandPtr ToShellCommand() const override;
};

class IdeQuitCommand : public IdeCommand
{
public:
    CommandPtr ToShellCommand() const override;
};

class IdeContinueCommand : public IdeCommand
{
public:
    CommandPtr ToShellCommand() const override;
};

class IdeStepOverCommand : public IdeCommand
{
public:
    CommandPtr ToShellCommand() const override;
};

class IdeStepIntoCommand : public IdeCommand
{
public:
    CommandPtr ToShellCommand() const override;
};

class IdeStepOutCommand : public IdeCommand
{
public:
    CommandPtr ToShellCommand() const override;
};

class IdeBreakCommand : public IdeCommand
{
public:
    CommandPtr ToShellCommand() const override;
    void SetDataFrom(Cm::Core::JsonValue* jsonValue) override;
private:
    Cm::Core::SourceFileLine sourceFileLine;
};

class IdeClearCommand : public IdeCommand
{
public:
    CommandPtr ToShellCommand() const override;
    void SetDataFrom(Cm::Core::JsonValue* jsonValue) override;
private:
    int breakpointNumber;
};

class IdeCallStackCommand : public IdeCommand
{
public:
    CommandPtr ToShellCommand() const override;
};

class IdeFrameCommand : public IdeCommand
{
public:
    CommandPtr ToShellCommand() const override;
    void SetDataFrom(Cm::Core::JsonValue* jsonValue) override;
private:
    int frameNumber;
};

class IdeShowBreakpointsCommand : public IdeCommand
{
public:
    CommandPtr ToShellCommand() const override;
};

class IdeSetBreakOnThrowCommand : public IdeCommand
{
public:
    CommandPtr ToShellCommand() const override;
    void SetDataFrom(Cm::Core::JsonValue* jsonValue) override;
private:
    bool enable;
};

std::unique_ptr<IdeCommand> ParseIdeCommand(const std::string& commandLine);

void InitIdeInput();
void DoneIdeInput();

} } // Cm::Debugger

#endif // CM_DEBUGGER_IDE_INPUT_INCLUDED
