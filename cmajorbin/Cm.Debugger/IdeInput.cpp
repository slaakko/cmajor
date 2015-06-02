/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/IdeInput.hpp>
#include <Cm.Debugger/Command.hpp>
#include <Cm.Parser/Json.hpp>
#include <unordered_map>

namespace Cm { namespace Debugger {

class IdeCommandCreator
{
public:
    IdeCommandCreator(const std::string& commandName_);
    virtual ~IdeCommandCreator();
    const std::string& CommandName() const { return commandName; }
    virtual IdeCommand* CreateCommand() const = 0;
private:
    std::string commandName;
};

IdeCommandCreator::IdeCommandCreator(const std::string& commandName_) : commandName(commandName_)
{
}

IdeCommandCreator::~IdeCommandCreator()
{
}

class IdeCommandFactory
{
public:
    static void Init();
    static void Done();
    static IdeCommandFactory& Instance();
    IdeCommand* CreateCommand(const std::string& command);
    void Register(IdeCommandCreator* creator);
private:
    static std::unique_ptr<IdeCommandFactory> instance;
    std::vector<std::unique_ptr<IdeCommandCreator>> creators;
    typedef std::unordered_map<std::string, IdeCommandCreator*> CreatorMap;
    typedef CreatorMap::const_iterator CreatorMapIt;
    CreatorMap creatorMap;
};

std::unique_ptr<IdeCommandFactory> IdeCommandFactory::instance;

void IdeCommandFactory::Init()
{
    instance.reset(new IdeCommandFactory());
}

void IdeCommandFactory::Done()
{
    instance.reset();
}

IdeCommandFactory& IdeCommandFactory::Instance()
{
    return *instance;
}

IdeCommand* IdeCommandFactory::CreateCommand(const std::string& command)
{
    CreatorMapIt i = creatorMap.find(command);
    if (i != creatorMap.end())
    {
        IdeCommandCreator* creator = i->second;
        return creator->CreateCommand();
    }
    throw std::runtime_error("IDE command '" + command + "' not found");
}

void IdeCommandFactory::Register(IdeCommandCreator* creator)
{
    creatorMap[creator->CommandName()] = creator;
    creators.push_back(std::unique_ptr<IdeCommandCreator>(creator));
}


IdeCommand::~IdeCommand()
{
}

void IdeCommand::SetDataFrom(Cm::Core::JsonValue* jsonValue)
{
}

CommandPtr IdeCommand::ToShellCommand() const
{
    throw std::runtime_error("internal error: command not implemented");
}

IdeErrorCommand::IdeErrorCommand() : errorMessage("internal error: no message")
{
}

IdeErrorCommand::IdeErrorCommand(const std::string& errorMessage_) : errorMessage(errorMessage_)
{
}

CommandPtr IdeErrorCommand::ToShellCommand() const
{
    throw std::runtime_error(errorMessage);
}

void IdeInputCommand::SetDataFrom(Cm::Core::JsonValue* jsonValue)
{
    if (!jsonValue)
    {
        throw std::runtime_error("IDE input command contains no data field");
    }
    if (jsonValue->IsString())
    {
        input = jsonValue->ToString();
    }
    else
    {
        throw std::runtime_error("IDE input command data is not a JSON string");
    }
}

CommandPtr IdeStartCommand::ToShellCommand() const
{
    return CommandPtr(new StartCommand());
}

CommandPtr IdeQuitCommand::ToShellCommand() const
{
    return CommandPtr(new QuitCommand());
}

CommandPtr IdeContinueCommand::ToShellCommand() const
{
    return CommandPtr(new ContinueCommand());
}

CommandPtr IdeStepOverCommand::ToShellCommand() const
{
    return CommandPtr(new NextCommand());
}

CommandPtr IdeStepIntoCommand::ToShellCommand() const
{
    return CommandPtr(new StepCommand());
}

CommandPtr IdeStepOutCommand::ToShellCommand() const
{
    return CommandPtr(new OutCommand());
}

CommandPtr IdeBreakCommand::ToShellCommand() const
{
    return CommandPtr(new BreakCommand(sourceFileLine));
}

void IdeBreakCommand::SetDataFrom(Cm::Core::JsonValue* jsonValue)
{
    if (!jsonValue)
    {
        throw std::runtime_error("IDE break command contains no data field");
    }
    if (jsonValue->IsObject())
    {
        Cm::Core::JsonObject* data = static_cast<Cm::Core::JsonObject*>(jsonValue);
        Cm::Core::JsonValue* file = data->GetField(Cm::Core::JsonString("file"));
        if (!file)
        {
            throw std::runtime_error("IDE break command data contains no file attribute");
        }
        if (file->IsString())
        {
            sourceFileLine.SetSourceFilePath(file->ToString());
        }
        else
        {
            throw std::runtime_error("IDE break command file attribute is not a JSON string");
        }
        Cm::Core::JsonValue* line = data->GetField(Cm::Core::JsonString("line"));
        if (!line)
        {
            throw std::runtime_error("IDE break command data contains no line attribute");
        }
        if (line->IsNumber())
        {
            Cm::Core::JsonNumber* lineNumber = static_cast<Cm::Core::JsonNumber*>(line);
            sourceFileLine.SetSourceLineNumber(static_cast<int>(lineNumber->Value()));
        }
        else
        {
            throw std::runtime_error("IDE break command line attribute is not a JSON number");
        }
    }
    else
    {
        throw std::runtime_error("IDE break command data is not a JSON object");
    }
}

CommandPtr IdeClearCommand::ToShellCommand() const
{
    return CommandPtr(new ClearCommand(breakpointNumber));
}

void IdeClearCommand::SetDataFrom(Cm::Core::JsonValue* jsonValue)
{
    if (!jsonValue)
    {
        throw std::runtime_error("IDE clear command contains no data field");
    }
    if (jsonValue->IsObject())
    {
        Cm::Core::JsonObject* data = static_cast<Cm::Core::JsonObject*>(jsonValue);
        Cm::Core::JsonValue* bpNum = data->GetField(Cm::Core::JsonString("bpNum"));
        if (!bpNum)
        {
            throw std::runtime_error("IDE clear command data contains no bpNum attribute");
        }
        if (bpNum->IsNumber())
        {
            Cm::Core::JsonNumber* bpNumber = static_cast<Cm::Core::JsonNumber*>(bpNum);
            breakpointNumber = static_cast<int>(bpNumber->Value());
        }
        else
        {
            throw std::runtime_error("IDE clear command bpNum attribute is not a JSON number");
        }
    }
    else
    {
        throw std::runtime_error("IDE clear command data is not a JSON object");
    }
}

CommandPtr IdeCallStackCommand::ToShellCommand() const
{
    return CommandPtr(new CallStackCommand());
}

CommandPtr IdeFrameCommand::ToShellCommand() const
{
    return CommandPtr(new FrameCommand(frameNumber));
}

void IdeFrameCommand::SetDataFrom(Cm::Core::JsonValue* jsonValue)
{
    if (!jsonValue)
    {
        throw std::runtime_error("IDE frame command contains no data field");
    }
    if (jsonValue->IsObject())
    {
        Cm::Core::JsonObject* data = static_cast<Cm::Core::JsonObject*>(jsonValue);
        Cm::Core::JsonValue* frameNum = data->GetField(Cm::Core::JsonString("frameNum"));
        if (!frameNum)
        {
            throw std::runtime_error("IDE frame command data contains no frameNum attribute");
        }
        if (frameNum->IsNumber())
        {
            Cm::Core::JsonNumber* frameNr = static_cast<Cm::Core::JsonNumber*>(frameNum);
            frameNumber = static_cast<int>(frameNr->Value());
        }
        else
        {
            throw std::runtime_error("IDE frame command frameNum attribute is not a JSON number");
        }
    }
    else
    {
        throw std::runtime_error("IDE frame command data is not a JSON object");
    }
}

CommandPtr IdeShowBreakpointsCommand::ToShellCommand() const
{
    return CommandPtr(new ShowBreakpointsCommand());
}

CommandPtr IdeSetBreakOnThrowCommand::ToShellCommand() const
{
    return CommandPtr(new SetBreakOnThrowCommand(enable));
}

void IdeSetBreakOnThrowCommand::SetDataFrom(Cm::Core::JsonValue* jsonValue)
{
    if (!jsonValue)
    {
        throw std::runtime_error("IDE set break on throw command contains no data field");
    }
    if (jsonValue->IsObject())
    {
        Cm::Core::JsonObject* data = static_cast<Cm::Core::JsonObject*>(jsonValue);
        Cm::Core::JsonValue* setOn = data->GetField(Cm::Core::JsonString("on"));
        if (!setOn)
        {
            throw std::runtime_error("IDE set break on throw command data contains no on attribute");
        }
        if (setOn->IsBool())
        {
            Cm::Core::JsonBool* on = static_cast<Cm::Core::JsonBool*>(setOn);
            enable = on->Value();
        }
        else
        {
            throw std::runtime_error("IDE set break on throw command on attribute is not a JSON Boolean");
        }
    }
    else
    {
        throw std::runtime_error("IDE set break on throw command data is not a JSON object");
    }
}

Cm::Parser::JsonGrammar* jsonGrammar = nullptr;

std::unique_ptr<IdeCommand> ParseIdeCommand(const std::string& commandLine)
{
    if (!jsonGrammar)
    {
        jsonGrammar = Cm::Parser::JsonGrammar::Create();
    }
    std::unique_ptr<Cm::Core::JsonValue> jsonValue(jsonGrammar->Parse(commandLine.c_str(), commandLine.c_str() + commandLine.length(), 0, ""));
    if (!jsonValue->IsObject())
    {
        throw std::runtime_error("IDE command is not a JSON object");
    }
    Cm::Core::JsonObject* commandObject = static_cast<Cm::Core::JsonObject*>(jsonValue.get());
    Cm::Core::JsonValue* commandName = commandObject->GetField(Cm::Core::JsonString("command"));
    if (!commandName)
    {
        throw std::runtime_error("IDE command contains no command field");
    }
    if (!commandName->IsString())
    {
        throw std::runtime_error("IDE command's command field is not a JSON string");
    }
    std::unique_ptr<IdeCommand> ideCommand(IdeCommandFactory::Instance().CreateCommand(commandName->ToString()));
    Cm::Core::JsonValue* dataField = commandObject->GetField(Cm::Core::JsonString("data"));
    ideCommand->SetDataFrom(dataField);
    return ideCommand;
}

template<class CommandT>
class ConcreteIdeCommandCreator : public IdeCommandCreator
{
public:
    ConcreteIdeCommandCreator(const std::string& commandName_) : IdeCommandCreator(commandName_)
    {
    }
    IdeCommand* CreateCommand() const
    {
        return new CommandT();
    }
};

void InitIdeInput()
{
    IdeCommandFactory::Init();
    IdeCommandFactory::Instance().Register(new ConcreteIdeCommandCreator<IdeInputCommand>("input"));
    IdeCommandFactory::Instance().Register(new ConcreteIdeCommandCreator<IdeStartCommand>("start"));
    IdeCommandFactory::Instance().Register(new ConcreteIdeCommandCreator<IdeQuitCommand>("quit"));
    IdeCommandFactory::Instance().Register(new ConcreteIdeCommandCreator<IdeContinueCommand>("continue"));
    IdeCommandFactory::Instance().Register(new ConcreteIdeCommandCreator<IdeStepOverCommand>("stepOver"));
    IdeCommandFactory::Instance().Register(new ConcreteIdeCommandCreator<IdeStepIntoCommand>("stepInto"));
    IdeCommandFactory::Instance().Register(new ConcreteIdeCommandCreator<IdeStepOutCommand>("stepOut"));
    IdeCommandFactory::Instance().Register(new ConcreteIdeCommandCreator<IdeBreakCommand>("break"));
    IdeCommandFactory::Instance().Register(new ConcreteIdeCommandCreator<IdeClearCommand>("clear"));
    IdeCommandFactory::Instance().Register(new ConcreteIdeCommandCreator<IdeCallStackCommand>("callStack"));
    IdeCommandFactory::Instance().Register(new ConcreteIdeCommandCreator<IdeFrameCommand>("frame"));
    IdeCommandFactory::Instance().Register(new ConcreteIdeCommandCreator<IdeShowBreakpointsCommand>("showBreakpoints"));
    IdeCommandFactory::Instance().Register(new ConcreteIdeCommandCreator<IdeSetBreakOnThrowCommand>("setBreakOnThrow"));
}

void DoneIdeInput()
{
    IdeCommandFactory::Done();
}

} } // Cm::Debugger
