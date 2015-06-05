#include "Interpreter.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Core/CDebugInfo.hpp>

namespace Cm { namespace Debugger {

using namespace Cm::Parsing;

InterpreterGrammar* InterpreterGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

InterpreterGrammar* InterpreterGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    InterpreterGrammar* grammar(new InterpreterGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

InterpreterGrammar::InterpreterGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("InterpreterGrammar", parsingDomain_->GetNamespaceScope("Cm.Debugger"), parsingDomain_)
{
    SetOwner(0);
}

CommandPtr InterpreterGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
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
    CommandPtr result = *static_cast<Cm::Parsing::ValueObject<CommandPtr>*>(value.get());
    stack.pop();
    return result;
}

class InterpreterGrammar::CommandRule : public Cm::Parsing::Rule
{
public:
    CommandRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("CommandPtr");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<CommandPtr>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A5Action));
        Cm::Parsing::ActionParser* a6ActionParser = GetAction("A6");
        a6ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A6Action));
        Cm::Parsing::ActionParser* a7ActionParser = GetAction("A7");
        a7ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A7Action));
        Cm::Parsing::ActionParser* a8ActionParser = GetAction("A8");
        a8ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A8Action));
        Cm::Parsing::ActionParser* a9ActionParser = GetAction("A9");
        a9ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A9Action));
        Cm::Parsing::ActionParser* a10ActionParser = GetAction("A10");
        a10ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A10Action));
        Cm::Parsing::ActionParser* a11ActionParser = GetAction("A11");
        a11ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A11Action));
        Cm::Parsing::ActionParser* a12ActionParser = GetAction("A12");
        a12ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A12Action));
        Cm::Parsing::ActionParser* a13ActionParser = GetAction("A13");
        a13ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A13Action));
        Cm::Parsing::ActionParser* a14ActionParser = GetAction("A14");
        a14ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A14Action));
        Cm::Parsing::ActionParser* a15ActionParser = GetAction("A15");
        a15ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A15Action));
        Cm::Parsing::ActionParser* a16ActionParser = GetAction("A16");
        a16ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A16Action));
        Cm::Parsing::ActionParser* a17ActionParser = GetAction("A17");
        a17ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A17Action));
        Cm::Parsing::ActionParser* a18ActionParser = GetAction("A18");
        a18ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CommandRule>(this, &CommandRule::A18Action));
        Cm::Parsing::NonterminalParser* bsflNonterminalParser = GetNonterminal("bsfl");
        bsflNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CommandRule>(this, &CommandRule::Postbsfl));
        Cm::Parsing::NonterminalParser* bpNonterminalParser = GetNonterminal("bp");
        bpNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CommandRule>(this, &CommandRule::Postbp));
        Cm::Parsing::NonterminalParser* exprNonterminalParser = GetNonterminal("expr");
        exprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CommandRule>(this, &CommandRule::Postexpr));
        Cm::Parsing::NonterminalParser* lsflNonterminalParser = GetNonterminal("lsfl");
        lsflNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CommandRule>(this, &CommandRule::Postlsfl));
        Cm::Parsing::NonterminalParser* frameNonterminalParser = GetNonterminal("frame");
        frameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CommandRule>(this, &CommandRule::Postframe));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new StartCommand(-1));
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new QuitCommand(-1));
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new HelpCommand);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new ContinueCommand(-1));
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new NextCommand(-1));
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new StepCommand(-1));
    }
    void A6Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new OutCommand(-1));
    }
    void A7Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new BreakCommand(-1, context.frombsfl));
    }
    void A8Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new ClearCommand(-1, context.frombp));
    }
    void A9Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new InspectCommand(-1, context.fromexpr));
    }
    void A10Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new ListCommand(context.fromlsfl));
    }
    void A11Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new ListCommand("*"));
    }
    void A12Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new ListCommand("+"));
    }
    void A13Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new CallStackCommand(-1));
    }
    void A14Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new FrameCommand(-1, context.fromframe));
    }
    void A15Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new ShowBreakpointsCommand(-1));
    }
    void A16Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new SetBreakOnThrowCommand(-1, true));
    }
    void A17Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr(new SetBreakOnThrowCommand(-1, false));
    }
    void A18Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CommandPtr();
    }
    void Postbsfl(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> frombsfl_value = std::move(stack.top());
            context.frombsfl = *static_cast<Cm::Parsing::ValueObject<Cm::Core::SourceFileLine>*>(frombsfl_value.get());
            stack.pop();
        }
    }
    void Postbp(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> frombp_value = std::move(stack.top());
            context.frombp = *static_cast<Cm::Parsing::ValueObject<int>*>(frombp_value.get());
            stack.pop();
        }
    }
    void Postexpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromexpr_value = std::move(stack.top());
            context.fromexpr = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromexpr_value.get());
            stack.pop();
        }
    }
    void Postlsfl(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromlsfl_value = std::move(stack.top());
            context.fromlsfl = *static_cast<Cm::Parsing::ValueObject<Cm::Core::SourceFileLine>*>(fromlsfl_value.get());
            stack.pop();
        }
    }
    void Postframe(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromframe_value = std::move(stack.top());
            context.fromframe = *static_cast<Cm::Parsing::ValueObject<int>*>(fromframe_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), frombsfl(), frombp(), fromexpr(), fromlsfl(), fromframe() {}
        CommandPtr value;
        Cm::Core::SourceFileLine frombsfl;
        int frombp;
        std::string fromexpr;
        Cm::Core::SourceFileLine fromlsfl;
        int fromframe;
    };
    std::stack<Context> contextStack;
    Context context;
};

class InterpreterGrammar::SourceFileLineRule : public Cm::Parsing::Rule
{
public:
    SourceFileLineRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Core::SourceFileLine");
        AddLocalVariable(AttrOrVariable("std::string", "filePath"));
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
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SourceFileLineRule>(this, &SourceFileLineRule::A1Action));
        Cm::Parsing::NonterminalParser* fileNonterminalParser = GetNonterminal("file");
        fileNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SourceFileLineRule>(this, &SourceFileLineRule::Postfile));
        Cm::Parsing::NonterminalParser* lineNonterminalParser = GetNonterminal("line");
        lineNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SourceFileLineRule>(this, &SourceFileLineRule::Postline));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = Cm::Core::SourceFileLine(context.filePath, context.fromline);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.filePath = context.fromfile;
    }
    void Postfile(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromfile_value = std::move(stack.top());
            context.fromfile = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromfile_value.get());
            stack.pop();
        }
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
        Context(): value(), filePath(), fromfile(), fromline() {}
        Cm::Core::SourceFileLine value;
        std::string filePath;
        std::string fromfile;
        int fromline;
    };
    std::stack<Context> contextStack;
    Context context;
};

class InterpreterGrammar::FilePathRule : public Cm::Parsing::Rule
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

class InterpreterGrammar::InspectExprRule : public Cm::Parsing::Rule
{
public:
    InspectExprRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InspectExprRule>(this, &InspectExprRule::A0Action));
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

void InterpreterGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void InterpreterGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("spaces", this, "Cm.Parsing.stdlib.spaces"));
    AddRuleLink(new Cm::Parsing::RuleLink("int", this, "Cm.Parsing.stdlib.int"));
    AddRuleLink(new Cm::Parsing::RuleLink("qualified_id", this, "Cm.Parsing.stdlib.qualified_id"));
    AddRule(new CommandRule("Command", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::AlternativeParser(
                                new Cm::Parsing::AlternativeParser(
                                    new Cm::Parsing::AlternativeParser(
                                        new Cm::Parsing::AlternativeParser(
                                            new Cm::Parsing::AlternativeParser(
                                                new Cm::Parsing::AlternativeParser(
                                                    new Cm::Parsing::AlternativeParser(
                                                        new Cm::Parsing::AlternativeParser(
                                                            new Cm::Parsing::AlternativeParser(
                                                                new Cm::Parsing::AlternativeParser(
                                                                    new Cm::Parsing::ActionParser("A0",
                                                                        new Cm::Parsing::KeywordParser("start")),
                                                                    new Cm::Parsing::ActionParser("A1",
                                                                        new Cm::Parsing::AlternativeParser(
                                                                            new Cm::Parsing::KeywordParser("quit"),
                                                                            new Cm::Parsing::KeywordParser("q")))),
                                                                new Cm::Parsing::ActionParser("A2",
                                                                    new Cm::Parsing::AlternativeParser(
                                                                        new Cm::Parsing::KeywordParser("help"),
                                                                        new Cm::Parsing::KeywordParser("h")))),
                                                            new Cm::Parsing::ActionParser("A3",
                                                                new Cm::Parsing::AlternativeParser(
                                                                    new Cm::Parsing::KeywordParser("continue"),
                                                                    new Cm::Parsing::KeywordParser("c")))),
                                                        new Cm::Parsing::ActionParser("A4",
                                                            new Cm::Parsing::AlternativeParser(
                                                                new Cm::Parsing::KeywordParser("next"),
                                                                new Cm::Parsing::KeywordParser("n")))),
                                                    new Cm::Parsing::ActionParser("A5",
                                                        new Cm::Parsing::AlternativeParser(
                                                            new Cm::Parsing::KeywordParser("step"),
                                                            new Cm::Parsing::KeywordParser("s")))),
                                                new Cm::Parsing::ActionParser("A6",
                                                    new Cm::Parsing::AlternativeParser(
                                                        new Cm::Parsing::KeywordParser("out"),
                                                        new Cm::Parsing::KeywordParser("o")))),
                                            new Cm::Parsing::ActionParser("A7",
                                                new Cm::Parsing::SequenceParser(
                                                    new Cm::Parsing::AlternativeParser(
                                                        new Cm::Parsing::KeywordParser("break"),
                                                        new Cm::Parsing::KeywordParser("b")),
                                                    new Cm::Parsing::NonterminalParser("bsfl", "SourceFileLine", 0)))),
                                        new Cm::Parsing::ActionParser("A8",
                                            new Cm::Parsing::SequenceParser(
                                                new Cm::Parsing::AlternativeParser(
                                                    new Cm::Parsing::KeywordParser("clear"),
                                                    new Cm::Parsing::KeywordParser("cl")),
                                                new Cm::Parsing::NonterminalParser("bp", "int", 0)))),
                                    new Cm::Parsing::ActionParser("A9",
                                        new Cm::Parsing::SequenceParser(
                                            new Cm::Parsing::AlternativeParser(
                                                new Cm::Parsing::KeywordParser("inspect"),
                                                new Cm::Parsing::KeywordParser("i")),
                                            new Cm::Parsing::NonterminalParser("expr", "InspectExpr", 0)))),
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::AlternativeParser(
                                        new Cm::Parsing::KeywordParser("list"),
                                        new Cm::Parsing::KeywordParser("l")),
                                    new Cm::Parsing::AlternativeParser(
                                        new Cm::Parsing::AlternativeParser(
                                            new Cm::Parsing::ActionParser("A10",
                                                new Cm::Parsing::NonterminalParser("lsfl", "SourceFileLine", 0)),
                                            new Cm::Parsing::ActionParser("A11",
                                                new Cm::Parsing::CharParser('*'))),
                                        new Cm::Parsing::ActionParser("A12",
                                            new Cm::Parsing::AlternativeParser(
                                                new Cm::Parsing::CharParser('+'),
                                                new Cm::Parsing::EmptyParser()))))),
                            new Cm::Parsing::ActionParser("A13",
                                new Cm::Parsing::AlternativeParser(
                                    new Cm::Parsing::KeywordParser("callstack"),
                                    new Cm::Parsing::KeywordParser("ca")))),
                        new Cm::Parsing::ActionParser("A14",
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::AlternativeParser(
                                    new Cm::Parsing::KeywordParser("frame"),
                                    new Cm::Parsing::KeywordParser("f")),
                                new Cm::Parsing::NonterminalParser("frame", "int", 0)))),
                    new Cm::Parsing::ActionParser("A15",
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::KeywordParser("show"),
                            new Cm::Parsing::KeywordParser("breakpoints")))),
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::KeywordParser("set"),
                                new Cm::Parsing::KeywordParser("break")),
                            new Cm::Parsing::KeywordParser("on")),
                        new Cm::Parsing::KeywordParser("throw")),
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::ActionParser("A16",
                            new Cm::Parsing::KeywordParser("on")),
                        new Cm::Parsing::ActionParser("A17",
                            new Cm::Parsing::KeywordParser("off"))))),
            new Cm::Parsing::ActionParser("A18",
                new Cm::Parsing::EmptyParser()))));
    AddRule(new SourceFileLineRule("SourceFileLine", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::NonterminalParser("file", "FilePath", 0),
                                new Cm::Parsing::CharParser(':')))),
                    new Cm::Parsing::NonterminalParser("line", "int", 0))))));
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
    AddRule(new InspectExprRule("InspectExpr", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::PositiveParser(
                    new Cm::Parsing::AnyCharParser())))));
    SetSkipRuleName("spaces");
}

} } // namespace Cm.Debugger
