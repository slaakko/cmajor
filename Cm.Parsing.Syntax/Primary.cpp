#include "Primary.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Parsing.Cpp/Expression.hpp>
#include <Cm.Parsing.Cpp/Statement.hpp>
#include <Cm.Parsing.Syntax/Primitive.hpp>
#include <Cm.Parsing.Syntax/Composite.hpp>
#include <Cm.Parsing.Syntax/Element.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing.CppObjectModel/Statement.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

using namespace Cm::Parsing::CppObjectModel;
using namespace Cm::Parsing;

PrimaryGrammar* PrimaryGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

PrimaryGrammar* PrimaryGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    PrimaryGrammar* grammar(new PrimaryGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

PrimaryGrammar::PrimaryGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("PrimaryGrammar", parsingDomain_->GetNamespaceScope("Cm.Parsing.Syntax"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Parsing::Parser* PrimaryGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, Cm::Parsing::Scope* enclosingScope)
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

class PrimaryGrammar::PrimaryRule : public Cm::Parsing::Rule
{
public:
    PrimaryRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A5Action));
        Cm::Parsing::NonterminalParser* ruleCallNonterminalParser = GetNonterminal("RuleCall");
        ruleCallNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostRuleCall));
        Cm::Parsing::NonterminalParser* primitiveNonterminalParser = GetNonterminal("Primitive");
        primitiveNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostPrimitive));
        Cm::Parsing::NonterminalParser* groupingNonterminalParser = GetNonterminal("Grouping");
        groupingNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreGrouping));
        groupingNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostGrouping));
        Cm::Parsing::NonterminalParser* tokenNonterminalParser = GetNonterminal("Token");
        tokenNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreToken));
        tokenNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostToken));
        Cm::Parsing::NonterminalParser* expectationNonterminalParser = GetNonterminal("Expectation");
        expectationNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreExpectation));
        expectationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostExpectation));
        Cm::Parsing::NonterminalParser* actionNonterminalParser = GetNonterminal("Action");
        actionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreAction));
        actionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostAction));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromRuleCall;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromPrimitive;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromGrouping;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromToken;
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromExpectation;
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromAction;
    }
    void PostRuleCall(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromRuleCall_value = std::move(stack.top());
            context.fromRuleCall = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromRuleCall_value.get());
            stack.pop();
        }
    }
    void PostPrimitive(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromPrimitive_value = std::move(stack.top());
            context.fromPrimitive = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromPrimitive_value.get());
            stack.pop();
        }
    }
    void PreGrouping(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void PostGrouping(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromGrouping_value = std::move(stack.top());
            context.fromGrouping = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromGrouping_value.get());
            stack.pop();
        }
    }
    void PreToken(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void PostToken(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromToken_value = std::move(stack.top());
            context.fromToken = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromToken_value.get());
            stack.pop();
        }
    }
    void PreExpectation(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
    }
    void PostExpectation(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromExpectation_value = std::move(stack.top());
            context.fromExpectation = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromExpectation_value.get());
            stack.pop();
        }
    }
    void PreAction(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
    }
    void PostAction(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAction_value = std::move(stack.top());
            context.fromAction = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromAction_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): enclosingScope(), value(), fromRuleCall(), fromPrimitive(), fromGrouping(), fromToken(), fromExpectation(), fromAction() {}
        Cm::Parsing::Scope* enclosingScope;
        Cm::Parsing::Parser* value;
        Cm::Parsing::Parser* fromRuleCall;
        Cm::Parsing::Parser* fromPrimitive;
        Cm::Parsing::Parser* fromGrouping;
        Cm::Parsing::Parser* fromToken;
        Cm::Parsing::Parser* fromExpectation;
        Cm::Parsing::Parser* fromAction;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimaryGrammar::RuleCallRule : public Cm::Parsing::Rule
{
public:
    RuleCallRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::Parser*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RuleCallRule>(this, &RuleCallRule::A0Action));
        Cm::Parsing::NonterminalParser* ntWithArgsNonterminalParser = GetNonterminal("ntWithArgs");
        ntWithArgsNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RuleCallRule>(this, &RuleCallRule::PostntWithArgs));
        Cm::Parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal("ExpressionList");
        expressionListNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RuleCallRule>(this, &RuleCallRule::PostExpressionList));
        Cm::Parsing::NonterminalParser* ntNonterminalParser = GetNonterminal("nt");
        ntNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RuleCallRule>(this, &RuleCallRule::Postnt));
        Cm::Parsing::NonterminalParser* aliasNonterminalParser = GetNonterminal("Alias");
        aliasNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RuleCallRule>(this, &RuleCallRule::PostAlias));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        std::string ruleName;
        if (!context.fromntWithArgs.empty())
        {
            ruleName = context.fromntWithArgs;
        }
        else
        {
            ruleName = context.fromnt;
        }
        std::string alias = context.fromAlias;
        std::string name = !alias.empty() ? alias : ruleName;
        NonterminalParser * parser(new NonterminalParser(name, ruleName));
        parser->SetSpan(span);
        if (!context.fromExpressionList.empty())
        {
            parser->SetArguments(context.fromExpressionList);
        }
        context.value = parser;
    }
    void PostntWithArgs(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromntWithArgs_value = std::move(stack.top());
            context.fromntWithArgs = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromntWithArgs_value.get());
            stack.pop();
        }
    }
    void PostExpressionList(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromExpressionList_value = std::move(stack.top());
            context.fromExpressionList = *static_cast<Cm::Parsing::ValueObject<std::vector<Cm::Parsing::CppObjectModel::CppObject*>>*>(fromExpressionList_value.get());
            stack.pop();
        }
    }
    void Postnt(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromnt_value = std::move(stack.top());
            context.fromnt = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromnt_value.get());
            stack.pop();
        }
    }
    void PostAlias(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAlias_value = std::move(stack.top());
            context.fromAlias = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromAlias_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromntWithArgs(), fromExpressionList(), fromnt(), fromAlias() {}
        Cm::Parsing::Parser* value;
        std::string fromntWithArgs;
        std::vector<Cm::Parsing::CppObjectModel::CppObject*> fromExpressionList;
        std::string fromnt;
        std::string fromAlias;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimaryGrammar::NonterminalRule : public Cm::Parsing::Rule
{
public:
    NonterminalRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NonterminalRule>(this, &NonterminalRule::A0Action));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NonterminalRule>(this, &NonterminalRule::PostIdentifier));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromIdentifier;
    }
    void PostIdentifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIdentifier_value = std::move(stack.top());
            context.fromIdentifier = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromIdentifier() {}
        std::string value;
        std::string fromIdentifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimaryGrammar::AliasRule : public Cm::Parsing::Rule
{
public:
    AliasRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AliasRule>(this, &AliasRule::A0Action));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AliasRule>(this, &AliasRule::PostIdentifier));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromIdentifier;
    }
    void PostIdentifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIdentifier_value = std::move(stack.top());
            context.fromIdentifier = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromIdentifier() {}
        std::string value;
        std::string fromIdentifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimaryGrammar::GroupingRule : public Cm::Parsing::Rule
{
public:
    GroupingRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<GroupingRule>(this, &GroupingRule::A0Action));
        Cm::Parsing::NonterminalParser* alternativeNonterminalParser = GetNonterminal("Alternative");
        alternativeNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<GroupingRule>(this, &GroupingRule::PreAlternative));
        alternativeNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<GroupingRule>(this, &GroupingRule::PostAlternative));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromAlternative;
    }
    void PreAlternative(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void PostAlternative(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAlternative_value = std::move(stack.top());
            context.fromAlternative = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromAlternative_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): enclosingScope(), value(), fromAlternative() {}
        Cm::Parsing::Scope* enclosingScope;
        Cm::Parsing::Parser* value;
        Cm::Parsing::Parser* fromAlternative;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimaryGrammar::TokenRule : public Cm::Parsing::Rule
{
public:
    TokenRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TokenRule>(this, &TokenRule::A0Action));
        Cm::Parsing::NonterminalParser* alternativeNonterminalParser = GetNonterminal("Alternative");
        alternativeNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<TokenRule>(this, &TokenRule::PreAlternative));
        alternativeNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TokenRule>(this, &TokenRule::PostAlternative));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new TokenParser(context.fromAlternative);
    }
    void PreAlternative(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void PostAlternative(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAlternative_value = std::move(stack.top());
            context.fromAlternative = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromAlternative_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): enclosingScope(), value(), fromAlternative() {}
        Cm::Parsing::Scope* enclosingScope;
        Cm::Parsing::Parser* value;
        Cm::Parsing::Parser* fromAlternative;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimaryGrammar::ExpectationRule : public Cm::Parsing::Rule
{
public:
    ExpectationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Parser*", "child"));
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> child_value = std::move(stack.top());
        context.child = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(child_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ExpectationRule>(this, &ExpectationRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ExpectationParser(context.child);
    }
private:
    struct Context
    {
        Context(): child(), value() {}
        Cm::Parsing::Parser* child;
        Cm::Parsing::Parser* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimaryGrammar::ActionRule : public Cm::Parsing::Rule
{
public:
    ActionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Parser*", "child"));
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> child_value = std::move(stack.top());
        context.child = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(child_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ActionRule>(this, &ActionRule::A0Action));
        Cm::Parsing::NonterminalParser* actionNonterminalParser = GetNonterminal("action");
        actionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ActionRule>(this, &ActionRule::Postaction));
        Cm::Parsing::NonterminalParser* failureActionNonterminalParser = GetNonterminal("failureAction");
        failureActionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ActionRule>(this, &ActionRule::PostfailureAction));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ActionParser("", context.fromaction, context.fromfailureAction, context.child);
    }
    void Postaction(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromaction_value = std::move(stack.top());
            context.fromaction = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CompoundStatement*>*>(fromaction_value.get());
            stack.pop();
        }
    }
    void PostfailureAction(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromfailureAction_value = std::move(stack.top());
            context.fromfailureAction = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CompoundStatement*>*>(fromfailureAction_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): child(), value(), fromaction(), fromfailureAction() {}
        Cm::Parsing::Parser* child;
        Cm::Parsing::Parser* value;
        Cm::Parsing::CppObjectModel::CompoundStatement* fromaction;
        Cm::Parsing::CppObjectModel::CompoundStatement* fromfailureAction;
    };
    std::stack<Context> contextStack;
    Context context;
};

void PrimaryGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.Syntax.PrimitiveGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::Syntax::PrimitiveGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parsing.Cpp.ExpressionGrammar");
    if (!grammar1)
    {
        grammar1 = Cm::Parsing::Cpp::ExpressionGrammar::Create(pd);
    }
    AddGrammarReference(grammar1);
    Cm::Parsing::Grammar* grammar2 = pd->GetGrammar("Cm.Parsing.Cpp.StatementGrammar");
    if (!grammar2)
    {
        grammar2 = Cm::Parsing::Cpp::StatementGrammar::Create(pd);
    }
    AddGrammarReference(grammar2);
    Cm::Parsing::Grammar* grammar3 = pd->GetGrammar("Cm.Parsing.Syntax.ElementGrammar");
    if (!grammar3)
    {
        grammar3 = Cm::Parsing::Syntax::ElementGrammar::Create(pd);
    }
    AddGrammarReference(grammar3);
    Cm::Parsing::Grammar* grammar4 = pd->GetGrammar("Cm.Parsing.Syntax.CompositeGrammar");
    if (!grammar4)
    {
        grammar4 = Cm::Parsing::Syntax::CompositeGrammar::Create(pd);
    }
    AddGrammarReference(grammar4);
}

void PrimaryGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("Primitive", this, "PrimitiveGrammar.Primitive"));
    AddRuleLink(new Cm::Parsing::RuleLink("ExpressionList", this, "Cm.Parsing.Cpp.ExpressionGrammar.ExpressionList"));
    AddRuleLink(new Cm::Parsing::RuleLink("Identifier", this, "ElementGrammar.Identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("Alternative", this, "CompositeGrammar.Alternative"));
    AddRuleLink(new Cm::Parsing::RuleLink("CompoundStatement", this, "Cm.Parsing.Cpp.StatementGrammar.CompoundStatement"));
    AddRule(new PrimaryRule("Primary", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::ActionParser("A0",
                                new Cm::Parsing::NonterminalParser("RuleCall", "RuleCall", 0)),
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::NonterminalParser("Primitive", "Primitive", 0))),
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::NonterminalParser("Grouping", "Grouping", 1))),
                    new Cm::Parsing::ActionParser("A3",
                        new Cm::Parsing::NonterminalParser("Token", "Token", 1))),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::ActionParser("A4",
                        new Cm::Parsing::NonterminalParser("Expectation", "Expectation", 1)))),
            new Cm::Parsing::OptionalParser(
                new Cm::Parsing::ActionParser("A5",
                    new Cm::Parsing::NonterminalParser("Action", "Action", 1))))));
    AddRule(new RuleCallRule("RuleCall", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::TokenParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::NonterminalParser("ntWithArgs", "Nonterminal", 0),
                                    new Cm::Parsing::CharParser('('))),
                            new Cm::Parsing::NonterminalParser("ExpressionList", "ExpressionList", 0)),
                        new Cm::Parsing::CharParser(')')),
                    new Cm::Parsing::NonterminalParser("nt", "Nonterminal", 0)),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::NonterminalParser("Alias", "Alias", 0))))));
    AddRule(new NonterminalRule("Nonterminal", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0))));
    AddRule(new AliasRule("Alias", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::CharParser(':'),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0))))));
    AddRule(new GroupingRule("Grouping", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::CharParser('('),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("Alternative", "Alternative", 1))),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(')'))))));
    AddRule(new TokenRule("Token", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("token"),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser('('))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("Alternative", "Alternative", 1))),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(')'))))));
    AddRule(new ExpectationRule("Expectation", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::CharParser('!'))));
    AddRule(new ActionRule("Action", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::NonterminalParser("action", "CompoundStatement", 0),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::CharParser('/'),
                        new Cm::Parsing::NonterminalParser("failureAction", "CompoundStatement", 0)))))));
}

} } } // namespace Cm.Parsing.Syntax
