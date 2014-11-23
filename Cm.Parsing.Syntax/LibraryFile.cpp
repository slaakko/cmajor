#include "LibraryFile.hpp"
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
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/Namespace.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

using namespace Cm::Parsing;

LibraryFileGrammar* LibraryFileGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

LibraryFileGrammar* LibraryFileGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    LibraryFileGrammar* grammar(new LibraryFileGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

LibraryFileGrammar::LibraryFileGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("LibraryFileGrammar", parsingDomain_->GetNamespaceScope("Cm.Parsing.Syntax"), parsingDomain_)
{
    SetOwner(0);
}

void LibraryFileGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, Cm::Parsing::ParsingDomain* parsingDomain)
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
    stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<Cm::Parsing::ParsingDomain*>(parsingDomain)));
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
}

class LibraryFileGrammar::LibraryFileRule : public Cm::Parsing::Rule
{
public:
    LibraryFileRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::ParsingDomain*", "parsingDomain"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> parsingDomain_value = std::move(stack.top());
        context.parsingDomain = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::ParsingDomain*>*>(parsingDomain_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal("NamespaceContent");
        namespaceContentNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<LibraryFileRule>(this, &LibraryFileRule::PreNamespaceContent));
    }
    void PreNamespaceContent(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::ParsingDomain*>(context.parsingDomain)));
    }
private:
    struct Context
    {
        Context(): parsingDomain() {}
        Cm::Parsing::ParsingDomain* parsingDomain;
    };
    std::stack<Context> contextStack;
    Context context;
};

class LibraryFileGrammar::NamespaceContentRule : public Cm::Parsing::Rule
{
public:
    NamespaceContentRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::ParsingDomain*", "parsingDomain"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> parsingDomain_value = std::move(stack.top());
        context.parsingDomain = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::ParsingDomain*>*>(parsingDomain_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A0Action));
        Cm::Parsing::NonterminalParser* grammarNonterminalParser = GetNonterminal("Grammar");
        grammarNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreGrammar));
        grammarNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostGrammar));
        Cm::Parsing::NonterminalParser* namespaceNonterminalParser = GetNonterminal("Namespace");
        namespaceNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreNamespace));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.parsingDomain->AddGrammar(context.fromGrammar);
    }
    void PreGrammar(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.parsingDomain->CurrentNamespace()->GetScope())));
    }
    void PostGrammar(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromGrammar_value = std::move(stack.top());
            context.fromGrammar = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Grammar*>*>(fromGrammar_value.get());
            stack.pop();
        }
    }
    void PreNamespace(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::ParsingDomain*>(context.parsingDomain)));
    }
private:
    struct Context
    {
        Context(): parsingDomain(), fromGrammar() {}
        Cm::Parsing::ParsingDomain* parsingDomain;
        Cm::Parsing::Grammar* fromGrammar;
    };
    std::stack<Context> contextStack;
    Context context;
};

class LibraryFileGrammar::NamespaceRule : public Cm::Parsing::Rule
{
public:
    NamespaceRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::ParsingDomain*", "parsingDomain"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> parsingDomain_value = std::move(stack.top());
        context.parsingDomain = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::ParsingDomain*>*>(parsingDomain_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NamespaceRule>(this, &NamespaceRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NamespaceRule>(this, &NamespaceRule::A1Action));
        Cm::Parsing::NonterminalParser* nsNonterminalParser = GetNonterminal("ns");
        nsNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NamespaceRule>(this, &NamespaceRule::Postns));
        Cm::Parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal("NamespaceContent");
        namespaceContentNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<NamespaceRule>(this, &NamespaceRule::PreNamespaceContent));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.parsingDomain->BeginNamespace(context.fromns);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.parsingDomain->EndNamespace();
    }
    void Postns(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromns_value = std::move(stack.top());
            context.fromns = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromns_value.get());
            stack.pop();
        }
    }
    void PreNamespaceContent(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::ParsingDomain*>(context.parsingDomain)));
    }
private:
    struct Context
    {
        Context(): parsingDomain(), fromns() {}
        Cm::Parsing::ParsingDomain* parsingDomain;
        std::string fromns;
    };
    std::stack<Context> contextStack;
    Context context;
};

class LibraryFileGrammar::GrammarRule : public Cm::Parsing::Rule
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
        Cm::Parsing::NonterminalParser* grammarNameNonterminalParser = GetNonterminal("grammarName");
        grammarNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<GrammarRule>(this, &GrammarRule::PostgrammarName));
        Cm::Parsing::NonterminalParser* grammarContentNonterminalParser = GetNonterminal("GrammarContent");
        grammarContentNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<GrammarRule>(this, &GrammarRule::PreGrammarContent));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Grammar(context.fromgrammarName, context.enclosingScope);
        context.value->SetNs(context.enclosingScope->Ns());
    }
    void PostgrammarName(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromgrammarName_value = std::move(stack.top());
            context.fromgrammarName = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromgrammarName_value.get());
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
        Context(): enclosingScope(), value(), fromgrammarName() {}
        Cm::Parsing::Scope* enclosingScope;
        Cm::Parsing::Grammar* value;
        std::string fromgrammarName;
    };
    std::stack<Context> contextStack;
    Context context;
};

class LibraryFileGrammar::GrammarContentRule : public Cm::Parsing::Rule
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
        Cm::Parsing::NonterminalParser* ruleNonterminalParser = GetNonterminal("Rule");
        ruleNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreRule));
        ruleNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<GrammarContentRule>(this, &GrammarContentRule::PostRule));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.grammar->AddRule(context.fromRule);
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

class LibraryFileGrammar::RuleRule : public Cm::Parsing::Rule
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
        Cm::Parsing::NonterminalParser* ruleNameNonterminalParser = GetNonterminal("ruleName");
        ruleNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RuleRule>(this, &RuleRule::PostruleName));
        Cm::Parsing::NonterminalParser* signatureNonterminalParser = GetNonterminal("Signature");
        signatureNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<RuleRule>(this, &RuleRule::PreSignature));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::Rule(context.fromruleName, context.enclosingScope);
    }
    void PostruleName(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromruleName_value = std::move(stack.top());
            context.fromruleName = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromruleName_value.get());
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
        Context(): enclosingScope(), value(), fromruleName() {}
        Cm::Parsing::Scope* enclosingScope;
        Cm::Parsing::Rule* value;
        std::string fromruleName;
    };
    std::stack<Context> contextStack;
    Context context;
};

void LibraryFileGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.Syntax.ElementGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::Syntax::ElementGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar1)
    {
        grammar1 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void LibraryFileGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("identifier", this, "Cm.Parsing.stdlib.identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("qualified_id", this, "Cm.Parsing.stdlib.qualified_id"));
    AddRuleLink(new Cm::Parsing::RuleLink("spaces_and_comments", this, "Cm.Parsing.stdlib.spaces_and_comments"));
    AddRuleLink(new Cm::Parsing::RuleLink("Signature", this, "ElementGrammar.Signature"));
    AddRuleLink(new Cm::Parsing::RuleLink("Identifier", this, "ElementGrammar.Identifier"));
    AddRule(new LibraryFileRule("LibraryFile", GetScope(),
        new Cm::Parsing::NonterminalParser("NamespaceContent", "NamespaceContent", 1)));
    AddRule(new NamespaceContentRule("NamespaceContent", GetScope(),
        new Cm::Parsing::KleeneStarParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("Grammar", "Grammar", 1)),
                new Cm::Parsing::NonterminalParser("Namespace", "Namespace", 1)))));
    AddRule(new NamespaceRule("Namespace", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("namespace"),
                        new Cm::Parsing::ActionParser("A0",
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::NonterminalParser("ns", "qualified_id", 0)))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser('{'))),
                new Cm::Parsing::NonterminalParser("NamespaceContent", "NamespaceContent", 1)),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser('}'))))));
    AddRule(new GrammarRule("Grammar", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("grammar"),
                        new Cm::Parsing::ActionParser("A0",
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::NonterminalParser("grammarName", "Identifier", 0)))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser('{'))),
                new Cm::Parsing::NonterminalParser("GrammarContent", "GrammarContent", 1)),
            new Cm::Parsing::ExpectationParser(
                new Cm::Parsing::CharParser('}')))));
    AddRule(new GrammarContentRule("GrammarContent", GetScope(),
        new Cm::Parsing::KleeneStarParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("Rule", "Rule", 1)))));
    AddRule(new RuleRule("Rule", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("ruleName", "Identifier", 0)),
                new Cm::Parsing::NonterminalParser("Signature", "Signature", 1)),
            new Cm::Parsing::CharParser(';'))));
    SetSkipRuleName("spaces_and_comments");
}

} } } // namespace Cm.Parsing.Syntax
