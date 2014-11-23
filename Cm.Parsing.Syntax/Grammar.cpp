#include "Grammar.hpp"
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
#include <Cm.Parsing.Syntax/Rule.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

using namespace Cm::Parsing;

GrammarGrammar* GrammarGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

GrammarGrammar* GrammarGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    GrammarGrammar* grammar(new GrammarGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

GrammarGrammar::GrammarGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("GrammarGrammar", parsingDomain_->GetNamespaceScope("Cm.Parsing.Syntax"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Parsing::Grammar* GrammarGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, Cm::Parsing::Scope* enclosingScope)
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
    Cm::Parsing::Grammar* result = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Grammar*>*>(value.get());
    stack.pop();
    return result;
}

class GrammarGrammar::GrammarRule : public Cm::Parsing::Rule
{
public:
    GrammarRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Scope*", "enclosingScope"));
        SetValueTypeName("Cm::Parsing::Grammar*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Grammar*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<GrammarRule>(this, &GrammarRule::A0Action));
        Cm::Parsing::NonterminalParser* nameNonterminalParser = GetNonterminal("name");
        nameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<GrammarRule>(this, &GrammarRule::Postname));
        Cm::Parsing::NonterminalParser* grammarContentNonterminalParser = GetNonterminal("GrammarContent");
        grammarContentNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<GrammarRule>(this, &GrammarRule::PreGrammarContent));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Grammar(context.fromname, context.enclosingScope);
        context.value->SetSpan(span);
        context.value->SetNs(context.enclosingScope->Ns());
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
    void PreGrammarContent(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Grammar*>(context.value)));
    }
private:
    struct Context
    {
        Context(): enclosingScope(), value(), fromname() {}
        Cm::Parsing::Scope* enclosingScope;
        Cm::Parsing::Grammar* value;
        std::string fromname;
    };
    std::stack<Context> contextStack;
    Context context;
};

class GrammarGrammar::GrammarContentRule : public Cm::Parsing::Rule
{
public:
    GrammarContentRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Grammar*", "grammar"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> grammar_value = std::move(stack.top());
        context.grammar = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Grammar*>*>(grammar_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<GrammarContentRule>(this, &GrammarContentRule::A0Action));
        Cm::Parsing::NonterminalParser* startClauseNonterminalParser = GetNonterminal("StartClause");
        startClauseNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreStartClause));
        Cm::Parsing::NonterminalParser* skipClauseNonterminalParser = GetNonterminal("SkipClause");
        skipClauseNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreSkipClause));
        Cm::Parsing::NonterminalParser* ruleLinkNonterminalParser = GetNonterminal("RuleLink");
        ruleLinkNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreRuleLink));
        Cm::Parsing::NonterminalParser* ruleNonterminalParser = GetNonterminal("Rule");
        ruleNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreRule));
        ruleNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<GrammarContentRule>(this, &GrammarContentRule::PostRule));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.grammar->AddRule(context.fromRule);
    }
    void PreStartClause(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Grammar*>(context.grammar)));
    }
    void PreSkipClause(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Grammar*>(context.grammar)));
    }
    void PreRuleLink(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Grammar*>(context.grammar)));
    }
    void PreRule(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.grammar->GetScope())));
    }
    void PostRule(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromRule_value = std::move(stack.top());
            context.fromRule = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Rule*>*>(fromRule_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): grammar(), fromRule() {}
        Cm::Parsing::Grammar* grammar;
        Cm::Parsing::Rule* fromRule;
    };
    std::stack<Context> contextStack;
    Context context;
};

class GrammarGrammar::StartClauseRule : public Cm::Parsing::Rule
{
public:
    StartClauseRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Grammar*", "grammar"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> grammar_value = std::move(stack.top());
        context.grammar = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Grammar*>*>(grammar_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StartClauseRule>(this, &StartClauseRule::A0Action));
        Cm::Parsing::NonterminalParser* strtNonterminalParser = GetNonterminal("strt");
        strtNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StartClauseRule>(this, &StartClauseRule::Poststrt));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.grammar->SetStartRuleName(context.fromstrt);
    }
    void Poststrt(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromstrt_value = std::move(stack.top());
            context.fromstrt = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromstrt_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): grammar(), fromstrt() {}
        Cm::Parsing::Grammar* grammar;
        std::string fromstrt;
    };
    std::stack<Context> contextStack;
    Context context;
};

class GrammarGrammar::SkipClauseRule : public Cm::Parsing::Rule
{
public:
    SkipClauseRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Grammar*", "grammar"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> grammar_value = std::move(stack.top());
        context.grammar = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Grammar*>*>(grammar_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SkipClauseRule>(this, &SkipClauseRule::A0Action));
        Cm::Parsing::NonterminalParser* skpNonterminalParser = GetNonterminal("skp");
        skpNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SkipClauseRule>(this, &SkipClauseRule::Postskp));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.grammar->SetSkipRuleName(context.fromskp);
    }
    void Postskp(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromskp_value = std::move(stack.top());
            context.fromskp = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromskp_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): grammar(), fromskp() {}
        Cm::Parsing::Grammar* grammar;
        std::string fromskp;
    };
    std::stack<Context> contextStack;
    Context context;
};

void GrammarGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.Syntax.ElementGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::Syntax::ElementGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parsing.Syntax.RuleGrammar");
    if (!grammar1)
    {
        grammar1 = Cm::Parsing::Syntax::RuleGrammar::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void GrammarGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("Identifier", this, "ElementGrammar.Identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("QualifiedId", this, "ElementGrammar.QualifiedId"));
    AddRuleLink(new Cm::Parsing::RuleLink("Signature", this, "ElementGrammar.Signature"));
    AddRuleLink(new Cm::Parsing::RuleLink("RuleLink", this, "ElementGrammar.RuleLink"));
    AddRuleLink(new Cm::Parsing::RuleLink("Rule", this, "RuleGrammar.Rule"));
    AddRule(new GrammarRule("Grammar", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("grammar"),
                        new Cm::Parsing::ActionParser("A0",
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::NonterminalParser("name", "Identifier", 0)))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser('{'))),
                new Cm::Parsing::NonterminalParser("GrammarContent", "GrammarContent", 1)),
            new Cm::Parsing::ExpectationParser(
                new Cm::Parsing::CharParser('}')))));
    AddRule(new GrammarContentRule("GrammarContent", GetScope(),
        new Cm::Parsing::KleeneStarParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::NonterminalParser("StartClause", "StartClause", 1),
                        new Cm::Parsing::NonterminalParser("SkipClause", "SkipClause", 1)),
                    new Cm::Parsing::NonterminalParser("RuleLink", "RuleLink", 1)),
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("Rule", "Rule", 1))))));
    AddRule(new StartClauseRule("StartClause", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::KeywordParser("start"),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("strt", "Identifier", 0))),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::CharParser(';')))));
    AddRule(new SkipClauseRule("SkipClause", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::KeywordParser("skip"),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("skp", "QualifiedId", 0))),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::CharParser(';')))));
}

} } } // namespace Cm.Parsing.Syntax
