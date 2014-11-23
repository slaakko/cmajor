#include "Composite.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing.Syntax/Primary.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

using namespace Cm::Parsing;

CompositeGrammar* CompositeGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

CompositeGrammar* CompositeGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    CompositeGrammar* grammar(new CompositeGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

CompositeGrammar::CompositeGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("CompositeGrammar", parsingDomain_->GetNamespaceScope("Cm.Parsing.Syntax"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Parsing::Parser* CompositeGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, Cm::Parsing::Scope* enclosingScope)
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
    stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<Cm::Parsing::Scope*>(enclosingScope)));
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
    Cm::Parsing::Parser* result = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(value.get());
    stack.pop();
    return result;
}

class CompositeGrammar::AlternativeRule : public Cm::Parsing::Rule
{
public:
    AlternativeRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Scope*", "enclosingScope"));
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> enclosingScope_value = std::move(stack.top());
        context.enclosingScope = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AlternativeRule>(this, &AlternativeRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AlternativeRule>(this, &AlternativeRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<AlternativeRule>(this, &AlternativeRule::Preleft));
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AlternativeRule>(this, &AlternativeRule::Postleft));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<AlternativeRule>(this, &AlternativeRule::Preright));
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AlternativeRule>(this, &AlternativeRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new AlternativeParser(context.value, context.fromright);
    }
    void Preleft(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        Cm::Parsing::Scope* enclosingScope;
        Cm::Parsing::Parser* value;
        Cm::Parsing::Parser* fromleft;
        Cm::Parsing::Parser* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompositeGrammar::SequenceRule : public Cm::Parsing::Rule
{
public:
    SequenceRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Scope*", "enclosingScope"));
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> enclosingScope_value = std::move(stack.top());
        context.enclosingScope = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SequenceRule>(this, &SequenceRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SequenceRule>(this, &SequenceRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<SequenceRule>(this, &SequenceRule::Preleft));
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SequenceRule>(this, &SequenceRule::Postleft));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<SequenceRule>(this, &SequenceRule::Preright));
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SequenceRule>(this, &SequenceRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new SequenceParser(context.value, context.fromright);
    }
    void Preleft(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        Cm::Parsing::Scope* enclosingScope;
        Cm::Parsing::Parser* value;
        Cm::Parsing::Parser* fromleft;
        Cm::Parsing::Parser* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompositeGrammar::DifferenceRule : public Cm::Parsing::Rule
{
public:
    DifferenceRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Scope*", "enclosingScope"));
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> enclosingScope_value = std::move(stack.top());
        context.enclosingScope = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DifferenceRule>(this, &DifferenceRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DifferenceRule>(this, &DifferenceRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DifferenceRule>(this, &DifferenceRule::Preleft));
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DifferenceRule>(this, &DifferenceRule::Postleft));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DifferenceRule>(this, &DifferenceRule::Preright));
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DifferenceRule>(this, &DifferenceRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new DifferenceParser(context.value, context.fromright);
    }
    void Preleft(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        Cm::Parsing::Scope* enclosingScope;
        Cm::Parsing::Parser* value;
        Cm::Parsing::Parser* fromleft;
        Cm::Parsing::Parser* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompositeGrammar::ExclusiveOrRule : public Cm::Parsing::Rule
{
public:
    ExclusiveOrRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Scope*", "enclosingScope"));
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> enclosingScope_value = std::move(stack.top());
        context.enclosingScope = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ExclusiveOrRule>(this, &ExclusiveOrRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ExclusiveOrRule>(this, &ExclusiveOrRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ExclusiveOrRule>(this, &ExclusiveOrRule::Preleft));
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ExclusiveOrRule>(this, &ExclusiveOrRule::Postleft));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ExclusiveOrRule>(this, &ExclusiveOrRule::Preright));
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ExclusiveOrRule>(this, &ExclusiveOrRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ExclusiveOrParser(context.value, context.fromright);
    }
    void Preleft(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        Cm::Parsing::Scope* enclosingScope;
        Cm::Parsing::Parser* value;
        Cm::Parsing::Parser* fromleft;
        Cm::Parsing::Parser* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompositeGrammar::IntersectionRule : public Cm::Parsing::Rule
{
public:
    IntersectionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Scope*", "enclosingScope"));
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> enclosingScope_value = std::move(stack.top());
        context.enclosingScope = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IntersectionRule>(this, &IntersectionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IntersectionRule>(this, &IntersectionRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<IntersectionRule>(this, &IntersectionRule::Preleft));
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IntersectionRule>(this, &IntersectionRule::Postleft));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<IntersectionRule>(this, &IntersectionRule::Preright));
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IntersectionRule>(this, &IntersectionRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new IntersectionParser(context.value, context.fromright);
    }
    void Preleft(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        Cm::Parsing::Scope* enclosingScope;
        Cm::Parsing::Parser* value;
        Cm::Parsing::Parser* fromleft;
        Cm::Parsing::Parser* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompositeGrammar::ListRule : public Cm::Parsing::Rule
{
public:
    ListRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Scope*", "enclosingScope"));
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> enclosingScope_value = std::move(stack.top());
        context.enclosingScope = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ListRule>(this, &ListRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ListRule>(this, &ListRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ListRule>(this, &ListRule::Preleft));
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ListRule>(this, &ListRule::Postleft));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ListRule>(this, &ListRule::Preright));
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ListRule>(this, &ListRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ListParser(context.value, context.fromright);
    }
    void Preleft(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        Cm::Parsing::Scope* enclosingScope;
        Cm::Parsing::Parser* value;
        Cm::Parsing::Parser* fromleft;
        Cm::Parsing::Parser* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompositeGrammar::PostfixRule : public Cm::Parsing::Rule
{
public:
    PostfixRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Scope*", "enclosingScope"));
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> enclosingScope_value = std::move(stack.top());
        context.enclosingScope = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A3Action));
        Cm::Parsing::NonterminalParser* primaryNonterminalParser = GetNonterminal("Primary");
        primaryNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<PostfixRule>(this, &PostfixRule::PrePrimary));
        primaryNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostfixRule>(this, &PostfixRule::PostPrimary));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromPrimary;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new KleeneStarParser(context.value);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new PositiveParser(context.value);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new OptionalParser(context.value);
    }
    void PrePrimary(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void PostPrimary(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromPrimary_value = std::move(stack.top());
            context.fromPrimary = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromPrimary_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): enclosingScope(), value(), fromPrimary() {}
        Cm::Parsing::Scope* enclosingScope;
        Cm::Parsing::Parser* value;
        Cm::Parsing::Parser* fromPrimary;
    };
    std::stack<Context> contextStack;
    Context context;
};

void CompositeGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.Syntax.PrimaryGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::Syntax::PrimaryGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void CompositeGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("Primary", this, "PrimaryGrammar.Primary"));
    AddRule(new AlternativeRule("Alternative", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "Sequence", 1)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('|'),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("right", "Sequence", 1))))))));
    AddRule(new SequenceRule("Sequence", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "Difference", 1)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::NonterminalParser("right", "Difference", 1))))));
    AddRule(new DifferenceRule("Difference", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "ExclusiveOr", 1)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('-'),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("right", "ExclusiveOr", 1))))))));
    AddRule(new ExclusiveOrRule("ExclusiveOr", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "Intersection", 1)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('^'),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("right", "Intersection", 1))))))));
    AddRule(new IntersectionRule("Intersection", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "List", 1)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('&'),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("right", "List", 1)))))));
    AddRule(new ListRule("List", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "Postfix", 1)),
            new Cm::Parsing::OptionalParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('%'),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("right", "Postfix", 1)))))));
    AddRule(new PostfixRule("Postfix", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("Primary", "Primary", 1)),
            new Cm::Parsing::OptionalParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::CharParser('*')),
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::CharParser('+'))),
                    new Cm::Parsing::ActionParser("A3",
                        new Cm::Parsing::CharParser('?')))))));
}

} } } // namespace Cm.Parsing.Syntax
