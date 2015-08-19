#include "Rule.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Parsing.Syntax/Element.hpp>
#include <Cm.Parsing.Syntax/Composite.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

using namespace Cm::Parsing;

RuleGrammar* RuleGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

RuleGrammar* RuleGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    RuleGrammar* grammar(new RuleGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

RuleGrammar::RuleGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("RuleGrammar", parsingDomain_->GetNamespaceScope("Cm.Parsing.Syntax"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Parsing::Rule* RuleGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, Cm::Parsing::Scope* enclosingScope)
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
    Cm::Parsing::Rule* result = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Rule*>*>(value.get());
    stack.pop();
    return result;
}

class RuleGrammar::RuleRule : public Cm::Parsing::Rule
{
public:
    RuleRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Scope*", "enclosingScope"));
        SetValueTypeName("Cm::Parsing::Rule*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Rule*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RuleRule>(this, &RuleRule::A0Action));
        Cm::Parsing::NonterminalParser* ruleHeaderNonterminalParser = GetNonterminal("RuleHeader");
        ruleHeaderNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<RuleRule>(this, &RuleRule::PreRuleHeader));
        ruleHeaderNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RuleRule>(this, &RuleRule::PostRuleHeader));
        Cm::Parsing::NonterminalParser* ruleBodyNonterminalParser = GetNonterminal("RuleBody");
        ruleBodyNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<RuleRule>(this, &RuleRule::PreRuleBody));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromRuleHeader;
    }
    void PreRuleHeader(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.enclosingScope)));
    }
    void PostRuleHeader(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromRuleHeader_value = std::move(stack.top());
            context.fromRuleHeader = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Rule*>*>(fromRuleHeader_value.get());
            stack.pop();
        }
    }
    void PreRuleBody(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Rule*>(context.value)));
    }
private:
    struct Context
    {
        Context(): enclosingScope(), value(), fromRuleHeader() {}
        Cm::Parsing::Scope* enclosingScope;
        Cm::Parsing::Rule* value;
        Cm::Parsing::Rule* fromRuleHeader;
    };
    std::stack<Context> contextStack;
    Context context;
};

class RuleGrammar::RuleHeaderRule : public Cm::Parsing::Rule
{
public:
    RuleHeaderRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Scope*", "enclosingScope"));
        SetValueTypeName("Cm::Parsing::Rule*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Rule*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RuleHeaderRule>(this, &RuleHeaderRule::A0Action));
        Cm::Parsing::NonterminalParser* nameNonterminalParser = GetNonterminal("name");
        nameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RuleHeaderRule>(this, &RuleHeaderRule::Postname));
        Cm::Parsing::NonterminalParser* signatureNonterminalParser = GetNonterminal("Signature");
        signatureNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<RuleHeaderRule>(this, &RuleHeaderRule::PreSignature));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Rule(context.fromname, context.enclosingScope);
        context.value->SetSpan(span);
    }
    void Postname(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromname_value = std::move(stack.top());
            context.fromname = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromname_value.get());
            stack.pop();
        }
    }
    void PreSignature(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Rule*>(context.value)));
    }
private:
    struct Context
    {
        Context(): enclosingScope(), value(), fromname() {}
        Cm::Parsing::Scope* enclosingScope;
        Cm::Parsing::Rule* value;
        std::string fromname;
    };
    std::stack<Context> contextStack;
    Context context;
};

class RuleGrammar::RuleBodyRule : public Cm::Parsing::Rule
{
public:
    RuleBodyRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Rule*", "rule"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> rule_value = std::move(stack.top());
        context.rule = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Rule*>*>(rule_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RuleBodyRule>(this, &RuleBodyRule::A0Action));
        Cm::Parsing::NonterminalParser* definitionNonterminalParser = GetNonterminal("definition");
        definitionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<RuleBodyRule>(this, &RuleBodyRule::Predefinition));
        definitionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RuleBodyRule>(this, &RuleBodyRule::Postdefinition));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.rule->SetDefinition(context.fromdefinition);
    }
    void Predefinition(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.rule->GetScope())));
    }
    void Postdefinition(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromdefinition_value = std::move(stack.top());
            context.fromdefinition = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromdefinition_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): rule(), fromdefinition() {}
        Cm::Parsing::Rule* rule;
        Cm::Parsing::Parser* fromdefinition;
    };
    std::stack<Context> contextStack;
    Context context;
};

void RuleGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.Syntax.ElementGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::Syntax::ElementGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parsing.Syntax.CompositeGrammar");
    if (!grammar1)
    {
        grammar1 = Cm::Parsing::Syntax::CompositeGrammar::Create(pd);
    }
    AddGrammarReference(grammar1);
    Cm::Parsing::Grammar* grammar2 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar2)
    {
        grammar2 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar2);
}

void RuleGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("Signature", this, "ElementGrammar.Signature"));
    AddRuleLink(new Cm::Parsing::RuleLink("Identifier", this, "ElementGrammar.Identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("Alternative", this, "CompositeGrammar.Alternative"));
    AddRuleLink(new Cm::Parsing::RuleLink("string", this, "Cm.Parsing.stdlib.string"));
    AddRule(new RuleRule("Rule", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("RuleHeader", "RuleHeader", 1)),
                new Cm::Parsing::StringParser("::=")),
            new Cm::Parsing::NonterminalParser("RuleBody", "RuleBody", 1))));
    AddRule(new RuleHeaderRule("RuleHeader", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("name", "Identifier", 0)),
            new Cm::Parsing::NonterminalParser("Signature", "Signature", 1))));
    AddRule(new RuleBodyRule("RuleBody", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("definition", "Alternative", 1)),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
}

} } } // namespace Cm.Parsing.Syntax
