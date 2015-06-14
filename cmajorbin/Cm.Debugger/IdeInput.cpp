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
    virtual IdeCommand* CreateCommand(int sequenceNumber) const = 0;
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
    IdeCommand* CreateCommand(int sequenceNumber, const std::string& command);
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

IdeCommand* IdeCommandFactory::CreateCommand(int sequenceNumber, const std::string& command)
{
    CreatorMapIt i = creatorMap.find(command);
    if (i != creatorMap.end())
    {
        IdeCommandCreator* creator = i->second;
        return creator->CreateCommand(sequenceNumber);
    }
    throw std::runtime_error("IDE command '" + command + "' not found");
}

void IdeCommandFactory::Register(IdeCommandCreator* creator)
{
    creatorMap[creator->CommandName()] = creator;
    creators.push_back(std::unique_ptr<IdeCommandCreator>(creator));
}

IdeCommand::IdeCommand(int sequenceNumber_) : sequenceNumber(sequenceNumber_)
{
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

IdeErrorCommand::IdeErrorCommand(int sequenceNumber_) : IdeCommand(sequenceNumber_), errorMessage("internal error: no message")
{
}

IdeErrorCommand::IdeErrorCommand(int sequenceNumber_, const std::string& errorMessage_) : IdeCommand(sequenceNumber_), errorMessage(errorMessage_)
{
}

CommandPtr IdeErrorCommand::ToShellCommand() const
{
    throw std::runtime_error(errorMessage);
}

IdeInputCommand::IdeInputCommand(int sequenceNumber_) : IdeCommand(sequenceNumber_)
{
}

void IdeInputCommand::SetDataFrom(Cm::Core::JsonValue* jsonValue)
{
    if (!jsonValue)
    {
        throw std::runtime_error("IDE input command contains no data field");
    }
    if (jsonValue->IsString())
    {
        input = static_cast<Cm::Core::JsonString*>(jsonValue)->Value();
    }
    else
    {
        throw std::runtime_error("IDE input command data is not a JSON string");
    }
}

IdeStartCommand::IdeStartCommand(int sequenceNumber_) : IdeCommand(sequenceNumber_)
{
}

CommandPtr IdeStartCommand::ToShellCommand() const
{
    return CommandPtr(new StartCommand(SequenceNumber()));
}

IdeQuitCommand::IdeQuitCommand(int sequenceNumber_) : IdeCommand(sequenceNumber_)
{
}

CommandPtr IdeQuitCommand::ToShellCommand() const
{
    return CommandPtr(new QuitCommand(SequenceNumber()));
}

IdeContinueCommand::IdeContinueCommand(int sequenceNumber_) : IdeCommand(sequenceNumber_)
{
}

CommandPtr IdeContinueCommand::ToShellCommand() const
{
    return CommandPtr(new ContinueCommand(SequenceNumber()));
}

IdeStepOverCommand::IdeStepOverCommand(int sequenceNumber_) : IdeCommand(sequenceNumber_)
{
}

CommandPtr IdeStepOverCommand::ToShellCommand() const
{
    return CommandPtr(new NextCommand(SequenceNumber()));
}

IdeStepIntoCommand::IdeStepIntoCommand(int sequenceNumber_) : IdeCommand(sequenceNumber_)
{
}

CommandPtr IdeStepIntoCommand::ToShellCommand() const
{
    return CommandPtr(new StepCommand(SequenceNumber()));
}

IdeStepOutCommand::IdeStepOutCommand(int sequenceNumber_) : IdeCommand(sequenceNumber_)
{
}

CommandPtr IdeStepOutCommand::ToShellCommand() const
{
    return CommandPtr(new OutCommand(SequenceNumber()));
}

IdeBreakCommand::IdeBreakCommand(int sequenceNumber_) : IdeCommand(sequenceNumber_)
{
}

CommandPtr IdeBreakCommand::ToShellCommand() const
{
    return CommandPtr(new BreakCommand(SequenceNumber(), sourceFileLine));
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
            sourceFileLine.SetSourceFilePath(static_cast<Cm::Core::JsonString*>(file)->Value());
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

IdeClearCommand::IdeClearCommand(int sequenceNumber_) : IdeCommand(sequenceNumber_)
{
}

CommandPtr IdeClearCommand::ToShellCommand() const
{
    return CommandPtr(new ClearCommand(SequenceNumber(), breakpointNumber));
}

void IdeClearCommand::SetDataFrom(Cm::Core::JsonValue* jsonValue)
{
    if (!jsonValue)
    {
        throw std::runtime_error("IDE clear command contains no data field");
    }
    if (jsonValue->IsNumber())
    {
        Cm::Core::JsonNumber* bpNum = static_cast<Cm::Core::JsonNumber*>(jsonValue);
        breakpointNumber = static_cast<int>(bpNum->Value());
    }
    else
    {
        throw std::runtime_error("IDE clear command data is not a JSON number");
    }
}

IdeCallStackCommand::IdeCallStackCommand(int sequenceNumber_) : IdeCommand(sequenceNumber_)
{
}

CommandPtr IdeCallStackCommand::ToShellCommand() const
{
    return CommandPtr(new CallStackCommand(SequenceNumber()));
}

IdeFrameCommand::IdeFrameCommand(int sequenceNumber_) : IdeCommand(sequenceNumber_)
{
}

CommandPtr IdeFrameCommand::ToShellCommand() const
{
    return CommandPtr(new FrameCommand(SequenceNumber(), frameNumber));
}

void IdeFrameCommand::SetDataFrom(Cm::Core::JsonValue* jsonValue)
{
    if (!jsonValue)
    {
        throw std::runtime_error("IDE frame command contains no data field");
    }
    if (jsonValue->IsNumber())
    {
        Cm::Core::JsonNumber* frameNum = static_cast<Cm::Core::JsonNumber*>(jsonValue);
        frameNumber = static_cast<int>(frameNum->Value());
    }
    else
    {
        throw std::runtime_error("IDE frame command data is not a JSON number");
    }
}

IdeShowBreakpointsCommand::IdeShowBreakpointsCommand(int sequenceNumber_) : IdeCommand(sequenceNumber_)
{
}

CommandPtr IdeShowBreakpointsCommand::ToShellCommand() const
{
    return CommandPtr(new ShowBreakpointsCommand(SequenceNumber()));
}

IdeSetBreakOnThrowCommand::IdeSetBreakOnThrowCommand(int sequenceNumber_) : IdeCommand(sequenceNumber_)
{
}

CommandPtr IdeSetBreakOnThrowCommand::ToShellCommand() const
{
    return CommandPtr(new SetBreakOnThrowCommand(SequenceNumber(), enable));
}

void IdeSetBreakOnThrowCommand::SetDataFrom(Cm::Core::JsonValue* jsonValue)
{
    if (!jsonValue)
    {
        throw std::runtime_error("IDE set break on throw command contains no data field");
    }
    if (jsonValue->IsBool())
    {
        Cm::Core::JsonBool* on = static_cast<Cm::Core::JsonBool*>(jsonValue);
        enable = on->Value();
    }
    else
    {
        throw std::runtime_error("IDE set break on throw command data is not a JSON Boolean");
    }
}

IdeSourcesCommand::IdeSourcesCommand(int sequenceNumber_) : IdeCommand(sequenceNumber_)
{
}

CommandPtr IdeSourcesCommand::ToShellCommand() const
{
    return CommandPtr(new SourcesCommand(SequenceNumber()));
}

IdeInspectCommand::IdeInspectCommand(int sequenceNumber_) : IdeCommand(sequenceNumber_)
{
}

CommandPtr IdeInspectCommand::ToShellCommand() const
{
    return CommandPtr(new InspectCommand(SequenceNumber(), expr));
}

void IdeInspectCommand::SetDataFrom(Cm::Core::JsonValue* jsonValue)
{
    if (!jsonValue)
    {
        throw std::runtime_error("IDE inspect command contains no data field");
    }
    if (jsonValue->IsString())
    {
        expr = static_cast<Cm::Core::JsonString*>(jsonValue)->Value();
    }
    else
    {
        throw std::runtime_error("IDE inspect command data is not a JSON string");
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
    Cm::Core::JsonValue* sequenceNumberField = commandObject->GetField(Cm::Core::JsonString("sequence"));
    if (!sequenceNumberField)
    {
        throw std::runtime_error("IDE command contains no sequence field");
    }
    if (!sequenceNumberField->IsNumber())
    {
        throw std::runtime_error("IDE command's sequence field is not a JSON number");
    }
    int sequenceNumber = static_cast<int>(static_cast<Cm::Core::JsonNumber*>(sequenceNumberField)->Value());
    const std::string& command = static_cast<Cm::Core::JsonString*>(commandName)->Value();
    std::unique_ptr<IdeCommand> ideCommand(IdeCommandFactory::Instance().CreateCommand(sequenceNumber, command));
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
    IdeCommand* CreateCommand(int sequenceNumber) const
    {
        return new CommandT(sequenceNumber);
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
    IdeCommandFactory::Instance().Register(new ConcreteIdeCommandCreator<IdeSourcesCommand>("sources"));
    IdeCommandFactory::Instance().Register(new ConcreteIdeCommandCreator<IdeInspectCommand>("inspect"));
}

void DoneIdeInput()
{
    IdeCommandFactory::Done();
}

} } // Cm::Debugger
