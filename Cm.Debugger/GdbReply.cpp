#include "GdbReply.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>

namespace Cm { namespace Debugger {

using namespace Cm::Parsing;

BackTraceReplyGrammar* BackTraceReplyGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

BackTraceReplyGrammar* BackTraceReplyGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    BackTraceReplyGrammar* grammar(new BackTraceReplyGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

BackTraceReplyGrammar::BackTraceReplyGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("BackTraceReplyGrammar", parsingDomain_->GetNamespaceScope("Cm.Debugger"), parsingDomain_)
{
    SetOwner(0);
}

CallStack BackTraceReplyGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
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
    CallStack result = *static_cast<Cm::Parsing::ValueObject<CallStack>*>(value.get());
    stack.pop();
    return result;
}

class BackTraceReplyGrammar::BackTraceReplyRule : public Cm::Parsing::Rule
{
public:
    BackTraceReplyRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("CallStack");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<CallStack>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<BackTraceReplyRule>(this, &BackTraceReplyRule::A0Action));
        Cm::Parsing::NonterminalParser* frameNonterminalParser = GetNonterminal("Frame");
        frameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<BackTraceReplyRule>(this, &BackTraceReplyRule::PostFrame));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value.AddFrame(context.fromFrame);
    }
    void PostFrame(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFrame_value = std::move(stack.top());
            context.fromFrame = *static_cast<Cm::Parsing::ValueObject<Frame>*>(fromFrame_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromFrame() {}
        CallStack value;
        Frame fromFrame;
    };
    std::stack<Context> contextStack;
    Context context;
};

class BackTraceReplyGrammar::FrameRule : public Cm::Parsing::Rule
{
public:
    FrameRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Frame");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Frame>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<FrameRule>(this, &FrameRule::A0Action));
        Cm::Parsing::NonterminalParser* frameNumberNonterminalParser = GetNonterminal("frameNumber");
        frameNumberNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<FrameRule>(this, &FrameRule::PostframeNumber));
        Cm::Parsing::NonterminalParser* sourceFileLineNonterminalParser = GetNonterminal("SourceFileLine");
        sourceFileLineNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<FrameRule>(this, &FrameRule::PostSourceFileLine));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = Frame(context.fromframeNumber, context.fromSourceFileLine);
    }
    void PostframeNumber(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromframeNumber_value = std::move(stack.top());
            context.fromframeNumber = *static_cast<Cm::Parsing::ValueObject<int>*>(fromframeNumber_value.get());
            stack.pop();
        }
    }
    void PostSourceFileLine(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromSourceFileLine_value = std::move(stack.top());
            context.fromSourceFileLine = *static_cast<Cm::Parsing::ValueObject<Cm::Core::SourceFileLine>*>(fromSourceFileLine_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromframeNumber(), fromSourceFileLine() {}
        Frame value;
        int fromframeNumber;
        Cm::Core::SourceFileLine fromSourceFileLine;
    };
    std::stack<Context> contextStack;
    Context context;
};

class BackTraceReplyGrammar::SourceFileLineRule : public Cm::Parsing::Rule
{
public:
    SourceFileLineRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Core::SourceFileLine");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Core::SourceFileLine>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SourceFileLineRule>(this, &SourceFileLineRule::A0Action));
        Cm::Parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal("FilePath");
        filePathNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SourceFileLineRule>(this, &SourceFileLineRule::PostFilePath));
        Cm::Parsing::NonterminalParser* lineNonterminalParser = GetNonterminal("Line");
        lineNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SourceFileLineRule>(this, &SourceFileLineRule::PostLine));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = Cm::Core::SourceFileLine(context.fromFilePath, context.fromLine);
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
private:
    struct Context
    {
        Context(): value(), fromFilePath(), fromLine() {}
        Cm::Core::SourceFileLine value;
        std::string fromFilePath;
        int fromLine;
    };
    std::stack<Context> contextStack;
    Context context;
};

class BackTraceReplyGrammar::FilePathRule : public Cm::Parsing::Rule
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

class BackTraceReplyGrammar::LineRule : public Cm::Parsing::Rule
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
        Cm::Parsing::NonterminalParser* lineNonterminalParser = GetNonterminal("line");
        lineNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LineRule>(this, &LineRule::Postline));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromline;
    }
    void Postline(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromline_value = std::move(stack.top());
            context.fromline = *static_cast<Cm::Parsing::ValueObject<int>*>(fromline_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromline() {}
        int value;
        int fromline;
    };
    std::stack<Context> contextStack;
    Context context;
};

void BackTraceReplyGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void BackTraceReplyGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("int", this, "Cm.Parsing.stdlib.int"));
    AddRuleLink(new Cm::Parsing::RuleLink("newline", this, "Cm.Parsing.stdlib.newline"));
    AddRule(new BackTraceReplyRule("BackTraceReply", GetScope(),
        new Cm::Parsing::KleeneStarParser(
            new Cm::Parsing::ListParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("Frame", "Frame", 0)),
                new Cm::Parsing::NonterminalParser("newline", "newline", 0)))));
    AddRule(new FrameRule("Frame", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::CharParser('#'),
                                    new Cm::Parsing::NonterminalParser("frameNumber", "int", 0)),
                                new Cm::Parsing::KleeneStarParser(
                                    new Cm::Parsing::DifferenceParser(
                                        new Cm::Parsing::AnyCharParser(),
                                        new Cm::Parsing::StringParser(" at ")))),
                            new Cm::Parsing::StringParser(" at ")),
                        new Cm::Parsing::KleeneStarParser(
                            new Cm::Parsing::CharParser(' '))),
                    new Cm::Parsing::NonterminalParser("SourceFileLine", "SourceFileLine", 0)),
                new Cm::Parsing::KleeneStarParser(
                    new Cm::Parsing::DifferenceParser(
                        new Cm::Parsing::AnyCharParser(),
                        new Cm::Parsing::NonterminalParser("newline", "newline", 0)))))));
    AddRule(new SourceFileLineRule("SourceFileLine", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::NonterminalParser("FilePath", "FilePath", 0),
                    new Cm::Parsing::CharParser(':')),
                new Cm::Parsing::NonterminalParser("Line", "Line", 0)))));
    AddRule(new FilePathRule("FilePath", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::NonterminalParser("Drive", "Drive", 0)),
                new Cm::Parsing::PositiveParser(
                    new Cm::Parsing::DifferenceParser(
                        new Cm::Parsing::AnyCharParser(),
                        new Cm::Parsing::CharParser(':')))))));
    AddRule(new Cm::Parsing::Rule("Drive", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::LetterParser(),
            new Cm::Parsing::CharParser(':'))));
    AddRule(new LineRule("Line", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("line", "int", 0))));
}

FrameReplyGrammar* FrameReplyGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

FrameReplyGrammar* FrameReplyGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    FrameReplyGrammar* grammar(new FrameReplyGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

FrameReplyGrammar::FrameReplyGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("FrameReplyGrammar", parsingDomain_->GetNamespaceScope("Cm.Debugger"), parsingDomain_)
{
    SetOwner(0);
}

int FrameReplyGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
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
    int result = *static_cast<Cm::Parsing::ValueObject<int>*>(value.get());
    stack.pop();
    return result;
}

class FrameReplyGrammar::FrameRule : public Cm::Parsing::Rule
{
public:
    FrameRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<FrameRule>(this, &FrameRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<FrameRule>(this, &FrameRule::A1Action));
        Cm::Parsing::NonterminalParser* frameNumberNonterminalParser = GetNonterminal("frameNumber");
        frameNumberNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<FrameRule>(this, &FrameRule::PostframeNumber));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = -1;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromframeNumber;
    }
    void PostframeNumber(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromframeNumber_value = std::move(stack.top());
            context.fromframeNumber = *static_cast<Cm::Parsing::ValueObject<int>*>(fromframeNumber_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromframeNumber() {}
        int value;
        int fromframeNumber;
    };
    std::stack<Context> contextStack;
    Context context;
};

void FrameReplyGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void FrameReplyGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("int", this, "Cm.Parsing.stdlib.int"));
    AddRuleLink(new Cm::Parsing::RuleLink("spaces", this, "Cm.Parsing.stdlib.spaces"));
    AddRule(new FrameRule("Frame", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KleeneStarParser(
                        new Cm::Parsing::TokenParser(
                            new Cm::Parsing::DifferenceParser(
                                new Cm::Parsing::AnyCharParser(),
                                new Cm::Parsing::StringParser("??")))),
                    new Cm::Parsing::StringParser("??")),
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::TokenParser(
                        new Cm::Parsing::KleeneStarParser(
                            new Cm::Parsing::AnyCharParser())))),
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('#'),
                    new Cm::Parsing::NonterminalParser("frameNumber", "int", 0)),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::TokenParser(
                        new Cm::Parsing::KleeneStarParser(
                            new Cm::Parsing::AnyCharParser())))))));
    SetSkipRuleName("spaces");
}

ContinueReplyGrammar* ContinueReplyGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

ContinueReplyGrammar* ContinueReplyGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    ContinueReplyGrammar* grammar(new ContinueReplyGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

ContinueReplyGrammar::ContinueReplyGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("ContinueReplyGrammar", parsingDomain_->GetNamespaceScope("Cm.Debugger"), parsingDomain_)
{
    SetOwner(0);
}

ContinueReplyState ContinueReplyGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ContinueReplyData* data)
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
    stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<ContinueReplyData*>(data)));
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
    ContinueReplyState result = *static_cast<Cm::Parsing::ValueObject<ContinueReplyState>*>(value.get());
    stack.pop();
    return result;
}

class ContinueReplyGrammar::ReplyLineRule : public Cm::Parsing::Rule
{
public:
    ReplyLineRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ContinueReplyData*", "data"));
        SetValueTypeName("ContinueReplyState");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> data_value = std::move(stack.top());
        context.data = *static_cast<Cm::Parsing::ValueObject<ContinueReplyData*>*>(data_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ContinueReplyState>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ReplyLineRule>(this, &ReplyLineRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ReplyLineRule>(this, &ReplyLineRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ReplyLineRule>(this, &ReplyLineRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ReplyLineRule>(this, &ReplyLineRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ReplyLineRule>(this, &ReplyLineRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ReplyLineRule>(this, &ReplyLineRule::A5Action));
        Cm::Parsing::NonterminalParser* inferiorExitNonterminalParser = GetNonterminal("InferiorExit");
        inferiorExitNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ReplyLineRule>(this, &ReplyLineRule::PostInferiorExit));
        Cm::Parsing::NonterminalParser* signalNonterminalParser = GetNonterminal("Signal");
        signalNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ReplyLineRule>(this, &ReplyLineRule::PostSignal));
        Cm::Parsing::NonterminalParser* consoleLineNonterminalParser = GetNonterminal("ConsoleLine");
        consoleLineNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ReplyLineRule>(this, &ReplyLineRule::PostConsoleLine));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = ContinueReplyState::continuing;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.data->SetExitCode(context.fromInferiorExit);
        context.value = ContinueReplyState::exit;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = ContinueReplyState::breakpoint;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.data->SetSignal(context.fromSignal);
        context.value = ContinueReplyState::signal;
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = ContinueReplyState::prompt;
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.data->SetConsoleLine(context.fromConsoleLine);
        context.value = ContinueReplyState::consoleLine;
    }
    void PostInferiorExit(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromInferiorExit_value = std::move(stack.top());
            context.fromInferiorExit = *static_cast<Cm::Parsing::ValueObject<int>*>(fromInferiorExit_value.get());
            stack.pop();
        }
    }
    void PostSignal(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromSignal_value = std::move(stack.top());
            context.fromSignal = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromSignal_value.get());
            stack.pop();
        }
    }
    void PostConsoleLine(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConsoleLine_value = std::move(stack.top());
            context.fromConsoleLine = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromConsoleLine_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): data(), value(), fromInferiorExit(), fromSignal(), fromConsoleLine() {}
        ContinueReplyData* data;
        ContinueReplyState value;
        int fromInferiorExit;
        std::string fromSignal;
        std::string fromConsoleLine;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ContinueReplyGrammar::InferiorExitRule : public Cm::Parsing::Rule
{
public:
    InferiorExitRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InferiorExitRule>(this, &InferiorExitRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InferiorExitRule>(this, &InferiorExitRule::A1Action));
        Cm::Parsing::NonterminalParser* idNonterminalParser = GetNonterminal("id");
        idNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InferiorExitRule>(this, &InferiorExitRule::Postid));
        Cm::Parsing::NonterminalParser* procIdNonterminalParser = GetNonterminal("procId");
        procIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InferiorExitRule>(this, &InferiorExitRule::PostprocId));
        Cm::Parsing::NonterminalParser* exitCodeNonterminalParser = GetNonterminal("exitCode");
        exitCodeNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InferiorExitRule>(this, &InferiorExitRule::PostexitCode));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = 0;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromexitCode;
    }
    void Postid(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromid_value = std::move(stack.top());
            context.fromid = *static_cast<Cm::Parsing::ValueObject<int>*>(fromid_value.get());
            stack.pop();
        }
    }
    void PostprocId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromprocId_value = std::move(stack.top());
            context.fromprocId = *static_cast<Cm::Parsing::ValueObject<int>*>(fromprocId_value.get());
            stack.pop();
        }
    }
    void PostexitCode(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromexitCode_value = std::move(stack.top());
            context.fromexitCode = *static_cast<Cm::Parsing::ValueObject<int>*>(fromexitCode_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromid(), fromprocId(), fromexitCode() {}
        int value;
        int fromid;
        int fromprocId;
        int fromexitCode;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ContinueReplyGrammar::octalRule : public Cm::Parsing::Rule
{
public:
    octalRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<octalRule>(this, &octalRule::A0Action));
        Cm::Parsing::NonterminalParser* oNonterminalParser = GetNonterminal("o");
        oNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<octalRule>(this, &octalRule::Posto));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = 8 * context.value + context.fromo;
    }
    void Posto(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromo_value = std::move(stack.top());
            context.fromo = *static_cast<Cm::Parsing::ValueObject<int>*>(fromo_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromo() {}
        int value;
        int fromo;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ContinueReplyGrammar::octaldigitRule : public Cm::Parsing::Rule
{
public:
    octaldigitRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<octaldigitRule>(this, &octaldigitRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = *matchBegin - '0';
    }
private:
    struct Context
    {
        Context(): value() {}
        int value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ContinueReplyGrammar::SignalRule : public Cm::Parsing::Rule
{
public:
    SignalRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SignalRule>(this, &SignalRule::A0Action));
        Cm::Parsing::NonterminalParser* idNonterminalParser = GetNonterminal("id");
        idNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SignalRule>(this, &SignalRule::Postid));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromid;
    }
    void Postid(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromid_value = std::move(stack.top());
            context.fromid = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromid_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromid() {}
        std::string value;
        std::string fromid;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ContinueReplyGrammar::ConsoleLineRule : public Cm::Parsing::Rule
{
public:
    ConsoleLineRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConsoleLineRule>(this, &ConsoleLineRule::A0Action));
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

void ContinueReplyGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void ContinueReplyGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("int", this, "Cm.Parsing.stdlib.int"));
    AddRuleLink(new Cm::Parsing::RuleLink("identifier", this, "Cm.Parsing.stdlib.identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("spaces", this, "Cm.Parsing.stdlib.spaces"));
    AddRule(new ReplyLineRule("ReplyLine", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::ActionParser("A0",
                                new Cm::Parsing::NonterminalParser("ContinuingLine", "ContinuingLine", 0)),
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::NonterminalParser("InferiorExit", "InferiorExit", 0))),
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::NonterminalParser("Breakpoint", "Breakpoint", 0))),
                    new Cm::Parsing::ActionParser("A3",
                        new Cm::Parsing::NonterminalParser("Signal", "Signal", 0))),
                new Cm::Parsing::ActionParser("A4",
                    new Cm::Parsing::NonterminalParser("Prompt", "Prompt", 0))),
            new Cm::Parsing::ActionParser("A5",
                new Cm::Parsing::NonterminalParser("ConsoleLine", "ConsoleLine", 0)))));
    AddRule(new Cm::Parsing::Rule("ContinuingLine", GetScope(),
        new Cm::Parsing::StringParser("Continuing.")));
    AddRule(new InferiorExitRule("InferiorExit", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::SequenceParser(
                                            new Cm::Parsing::CharParser('['),
                                            new Cm::Parsing::KeywordParser("Inferior")),
                                        new Cm::Parsing::NonterminalParser("id", "int", 0)),
                                    new Cm::Parsing::CharParser('(')),
                                new Cm::Parsing::KeywordParser("process")),
                            new Cm::Parsing::NonterminalParser("procId", "int", 0)),
                        new Cm::Parsing::CharParser(')')),
                    new Cm::Parsing::KeywordParser("exited")),
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::KeywordParser("normally")),
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::KeywordParser("with"),
                            new Cm::Parsing::KeywordParser("code")),
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::NonterminalParser("exitCode", "octal", 0))))),
            new Cm::Parsing::CharParser(']'))));
    AddRule(new octalRule("octal", GetScope(),
        new Cm::Parsing::PositiveParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("o", "octaldigit", 0)))));
    AddRule(new octaldigitRule("octaldigit", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::CharSetParser("0-7"))));
    AddRule(new Cm::Parsing::Rule("Breakpoint", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::KeywordParser("Breakpoint"),
                new Cm::Parsing::NonterminalParser("bp", "int", 0)),
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::KleeneStarParser(
                    new Cm::Parsing::AnyCharParser())))));
    AddRule(new SignalRule("Signal", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::KeywordParser("Program"),
                            new Cm::Parsing::KeywordParser("received")),
                        new Cm::Parsing::KeywordParser("signal")),
                    new Cm::Parsing::NonterminalParser("id", "identifier", 0)),
                new Cm::Parsing::TokenParser(
                    new Cm::Parsing::KleeneStarParser(
                        new Cm::Parsing::AnyCharParser()))))));
    AddRule(new Cm::Parsing::Rule("Prompt", GetScope(),
        new Cm::Parsing::StringParser("(gdb) ")));
    AddRule(new ConsoleLineRule("ConsoleLine", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::KleeneStarParser(
                    new Cm::Parsing::AnyCharParser())))));
    SetSkipRuleName("spaces");
}

} } // namespace Cm.Debugger
