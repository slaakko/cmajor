#include "ToolError.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>

namespace Cm { namespace Parser {

using namespace Cm::Parsing;

ToolErrorGrammar* ToolErrorGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

ToolErrorGrammar* ToolErrorGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    ToolErrorGrammar* grammar(new ToolErrorGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

ToolErrorGrammar::ToolErrorGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("ToolErrorGrammar", parsingDomain_->GetNamespaceScope("Cm.Parser"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Util::ToolError ToolErrorGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
{
    Cm::Parsing::Scanner scanner(start, end, fileName, fileIndex, SkipRule());
    std::unique_ptr<Cm::Parsing::XmlLog> xmlLog;
    if (Log())
    {
        xmlLog.reset(new Cm::Parsing::XmlLog(*Log(), MaxLogLineLength()));
        scanner.SetLog(xmlLog.get());
        xmlLog->WriteBeginRule("parse");
    }
    Cm::Parsing::ObjectStack stack;
    Cm::Parsing::Match match = Cm::Parsing::Grammar::Parse(scanner, stack);
    Cm::Parsing::Span stop = scanner.GetSpan();
    if (Log())
    {
        xmlLog->WriteEndRule("parse");
    }
    if (!match.Hit() || stop.Start() != int(end - start))
    {
        if (StartRule())
        {
            throw Cm::Parsing::ExpectationFailure(StartRule()->Info(), fileName, stop, start, end);
        }
        else
        {
            throw Cm::Parsing::ParsingException("grammar '" + Name() + "' has no start rule", fileName, scanner.GetSpan(), start, end);
        }
    }
    std::unique_ptr<Cm::Parsing::Object> value = std::move(stack.top());
    Cm::Util::ToolError result = *static_cast<Cm::Parsing::ValueObject<Cm::Util::ToolError>*>(value.get());
    stack.pop();
    return result;
}

class ToolErrorGrammar::ToolErrorRule : public Cm::Parsing::Rule
{
public:
    ToolErrorRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Util::ToolError");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Util::ToolError>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ToolErrorRule>(this, &ToolErrorRule::A0Action));
        Cm::Parsing::NonterminalParser* toolNameNonterminalParser = GetNonterminal("ToolName");
        toolNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ToolErrorRule>(this, &ToolErrorRule::PostToolName));
        Cm::Parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal("FilePath");
        filePathNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ToolErrorRule>(this, &ToolErrorRule::PostFilePath));
        Cm::Parsing::NonterminalParser* lineNonterminalParser = GetNonterminal("Line");
        lineNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ToolErrorRule>(this, &ToolErrorRule::PostLine));
        Cm::Parsing::NonterminalParser* columnNonterminalParser = GetNonterminal("Column");
        columnNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ToolErrorRule>(this, &ToolErrorRule::PostColumn));
        Cm::Parsing::NonterminalParser* messageNonterminalParser = GetNonterminal("Message");
        messageNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ToolErrorRule>(this, &ToolErrorRule::PostMessage));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = Cm::Util::ToolError(context.fromToolName, context.fromFilePath, context.fromLine, context.fromColumn, context.fromMessage);
    }
    void PostToolName(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromToolName_value = std::move(stack.top());
            context.fromToolName = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromToolName_value.get());
            stack.pop();
        }
    }
    void PostFilePath(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFilePath_value = std::move(stack.top());
            context.fromFilePath = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromFilePath_value.get());
            stack.pop();
        }
    }
    void PostLine(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromLine_value = std::move(stack.top());
            context.fromLine = *static_cast<Cm::Parsing::ValueObject<int>*>(fromLine_value.get());
            stack.pop();
        }
    }
    void PostColumn(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromColumn_value = std::move(stack.top());
            context.fromColumn = *static_cast<Cm::Parsing::ValueObject<int>*>(fromColumn_value.get());
            stack.pop();
        }
    }
    void PostMessage(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromMessage_value = std::move(stack.top());
            context.fromMessage = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromMessage_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromToolName(), fromFilePath(), fromLine(), fromColumn(), fromMessage() {}
        Cm::Util::ToolError value;
        std::string fromToolName;
        std::string fromFilePath;
        int fromLine;
        int fromColumn;
        std::string fromMessage;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ToolErrorGrammar::ToolNameRule : public Cm::Parsing::Rule
{
public:
    ToolNameRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("std::string");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<std::string>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ToolNameRule>(this, &ToolNameRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = std::string(matchBegin, matchEnd);
    }
private:
    struct Context
    {
        Context(): value() {}
        std::string value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ToolErrorGrammar::FilePathRule : public Cm::Parsing::Rule
{
public:
    FilePathRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("std::string");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<std::string>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<FilePathRule>(this, &FilePathRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = std::string(matchBegin, matchEnd);
    }
private:
    struct Context
    {
        Context(): value() {}
        std::string value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ToolErrorGrammar::LineRule : public Cm::Parsing::Rule
{
public:
    LineRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("int");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<int>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LineRule>(this, &LineRule::A0Action));
        Cm::Parsing::NonterminalParser* iNonterminalParser = GetNonterminal("i");
        iNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LineRule>(this, &LineRule::Posti));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromi;
    }
    void Posti(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromi_value = std::move(stack.top());
            context.fromi = *static_cast<Cm::Parsing::ValueObject<int>*>(fromi_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromi() {}
        int value;
        int fromi;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ToolErrorGrammar::ColumnRule : public Cm::Parsing::Rule
{
public:
    ColumnRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("int");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<int>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ColumnRule>(this, &ColumnRule::A0Action));
        Cm::Parsing::NonterminalParser* iNonterminalParser = GetNonterminal("i");
        iNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ColumnRule>(this, &ColumnRule::Posti));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromi;
    }
    void Posti(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromi_value = std::move(stack.top());
            context.fromi = *static_cast<Cm::Parsing::ValueObject<int>*>(fromi_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromi() {}
        int value;
        int fromi;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ToolErrorGrammar::MessageRule : public Cm::Parsing::Rule
{
public:
    MessageRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("std::string");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<std::string>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MessageRule>(this, &MessageRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = std::string(matchBegin, matchEnd);
    }
private:
    struct Context
    {
        Context(): value() {}
        std::string value;
    };
    std::stack<Context> contextStack;
    Context context;
};

void ToolErrorGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void ToolErrorGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("int", this, "Cm.Parsing.stdlib.int"));
    AddRuleLink(new Cm::Parsing::RuleLink("spaces", this, "Cm.Parsing.stdlib.spaces"));
    AddRule(new ToolErrorRule("ToolError", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::SequenceParser(
                                            new Cm::Parsing::NonterminalParser("ToolName", "ToolName", 0),
                                            new Cm::Parsing::CharParser(':')),
                                        new Cm::Parsing::NonterminalParser("FilePath", "FilePath", 0)),
                                    new Cm::Parsing::CharParser(':')),
                                new Cm::Parsing::NonterminalParser("Line", "Line", 0)),
                            new Cm::Parsing::CharParser(':')),
                        new Cm::Parsing::NonterminalParser("Column", "Column", 0)),
                    new Cm::Parsing::CharParser(':')),
                new Cm::Parsing::NonterminalParser("Message", "Message", 0)))));
    AddRule(new ToolNameRule("ToolName", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::PositiveParser(
                    new Cm::Parsing::DifferenceParser(
                        new Cm::Parsing::AnyCharParser(),
                        new Cm::Parsing::CharParser(':')))))));
    AddRule(new FilePathRule("FilePath", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("Drive", "Drive", 0)),
                    new Cm::Parsing::PositiveParser(
                        new Cm::Parsing::DifferenceParser(
                            new Cm::Parsing::AnyCharParser(),
                            new Cm::Parsing::CharParser(':'))))))));
    AddRule(new Cm::Parsing::Rule("Drive", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::LetterParser(),
            new Cm::Parsing::CharParser(':'))));
    AddRule(new LineRule("Line", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("i", "int", 0))));
    AddRule(new ColumnRule("Column", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("i", "int", 0))));
    AddRule(new MessageRule("Message", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::KleeneStarParser(
                    new Cm::Parsing::AnyCharParser())))));
    SetSkipRuleName("spaces");
}

} } // namespace Cm.Parser
