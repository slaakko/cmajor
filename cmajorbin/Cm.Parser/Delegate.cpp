#include "Delegate.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Ast/Delegate.hpp>
#include <Cm.Parser/Parameter.hpp>
#include <Cm.Parser/Specifier.hpp>
#include <Cm.Parser/TypeExpr.hpp>
#include <Cm.Parser/Identifier.hpp>

namespace Cm { namespace Parser {

using namespace Cm::Ast;
using namespace Cm::Parsing;

DelegateGrammar* DelegateGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

DelegateGrammar* DelegateGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    DelegateGrammar* grammar(new DelegateGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

DelegateGrammar::DelegateGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("DelegateGrammar", parsingDomain_->GetNamespaceScope("Cm.Parser"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Ast::Node* DelegateGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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
    stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<ParsingContext*>(ctx)));
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
    Cm::Ast::Node* result = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(value.get());
    stack.pop();
    return result;
}

class DelegateGrammar::DelegateRule : public Cm::Parsing::Rule
{
public:
    DelegateRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::Node*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DelegateRule>(this, &DelegateRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DelegateRule>(this, &DelegateRule::A1Action));
        Cm::Parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal("Specifiers");
        specifiersNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DelegateRule>(this, &DelegateRule::PostSpecifiers));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DelegateRule>(this, &DelegateRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DelegateRule>(this, &DelegateRule::PostTypeExpr));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DelegateRule>(this, &DelegateRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal("ParameterList");
        parameterListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DelegateRule>(this, &DelegateRule::PreParameterList));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new DelegateNode(span, context.fromSpecifiers, context.fromTypeExpr, context.fromIdentifier);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->GetSpan().SetEnd(span.End());
    }
    void PostSpecifiers(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromSpecifiers_value = std::move(stack.top());
            context.fromSpecifiers = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Specifiers>*>(fromSpecifiers_value.get());
            stack.pop();
        }
    }
    void PreTypeExpr(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostTypeExpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeExpr_value = std::move(stack.top());
            context.fromTypeExpr = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromTypeExpr_value.get());
            stack.pop();
        }
    }
    void PostIdentifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIdentifier_value = std::move(stack.top());
            context.fromIdentifier = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
    void PreParameterList(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromSpecifiers(), fromTypeExpr(), fromIdentifier() {}
        ParsingContext* ctx;
        Cm::Ast::Node* value;
        Cm::Ast::Specifiers fromSpecifiers;
        Cm::Ast::Node* fromTypeExpr;
        Cm::Ast::IdentifierNode* fromIdentifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DelegateGrammar::ClassDelegateRule : public Cm::Parsing::Rule
{
public:
    ClassDelegateRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::Node*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassDelegateRule>(this, &ClassDelegateRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassDelegateRule>(this, &ClassDelegateRule::A1Action));
        Cm::Parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal("Specifiers");
        specifiersNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassDelegateRule>(this, &ClassDelegateRule::PostSpecifiers));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassDelegateRule>(this, &ClassDelegateRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassDelegateRule>(this, &ClassDelegateRule::PostTypeExpr));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassDelegateRule>(this, &ClassDelegateRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal("ParameterList");
        parameterListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassDelegateRule>(this, &ClassDelegateRule::PreParameterList));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ClassDelegateNode(span, context.fromSpecifiers, context.fromTypeExpr, context.fromIdentifier);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->GetSpan().SetEnd(span.End());
    }
    void PostSpecifiers(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromSpecifiers_value = std::move(stack.top());
            context.fromSpecifiers = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Specifiers>*>(fromSpecifiers_value.get());
            stack.pop();
        }
    }
    void PreTypeExpr(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostTypeExpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeExpr_value = std::move(stack.top());
            context.fromTypeExpr = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromTypeExpr_value.get());
            stack.pop();
        }
    }
    void PostIdentifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIdentifier_value = std::move(stack.top());
            context.fromIdentifier = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
    void PreParameterList(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromSpecifiers(), fromTypeExpr(), fromIdentifier() {}
        ParsingContext* ctx;
        Cm::Ast::Node* value;
        Cm::Ast::Specifiers fromSpecifiers;
        Cm::Ast::Node* fromTypeExpr;
        Cm::Ast::IdentifierNode* fromIdentifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

void DelegateGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parser.ParameterGrammar");
    if (!grammar1)
    {
        grammar1 = Cm::Parser::ParameterGrammar::Create(pd);
    }
    AddGrammarReference(grammar1);
    Cm::Parsing::Grammar* grammar2 = pd->GetGrammar("Cm.Parser.TypeExprGrammar");
    if (!grammar2)
    {
        grammar2 = Cm::Parser::TypeExprGrammar::Create(pd);
    }
    AddGrammarReference(grammar2);
    Cm::Parsing::Grammar* grammar3 = pd->GetGrammar("Cm.Parser.SpecifierGrammar");
    if (!grammar3)
    {
        grammar3 = Cm::Parser::SpecifierGrammar::Create(pd);
    }
    AddGrammarReference(grammar3);
    Cm::Parsing::Grammar* grammar4 = pd->GetGrammar("Cm.Parser.IdentifierGrammar");
    if (!grammar4)
    {
        grammar4 = Cm::Parser::IdentifierGrammar::Create(pd);
    }
    AddGrammarReference(grammar4);
}

void DelegateGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("Specifiers", this, "SpecifierGrammar.Specifiers"));
    AddRuleLink(new Cm::Parsing::RuleLink("TypeExpr", this, "TypeExprGrammar.TypeExpr"));
    AddRuleLink(new Cm::Parsing::RuleLink("ParameterList", this, "ParameterGrammar.ParameterList"));
    AddRuleLink(new Cm::Parsing::RuleLink("Identifier", this, "IdentifierGrammar.Identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("spaces_and_comments", this, "Cm.Parsing.stdlib.spaces_and_comments"));
    AddRule(new DelegateRule("Delegate", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::NonterminalParser("Specifiers", "Specifiers", 0),
                                new Cm::Parsing::KeywordParser("delegate")),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 1))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0)))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("ParameterList", "ParameterList", 2))),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new ClassDelegateRule("ClassDelegate", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::NonterminalParser("Specifiers", "Specifiers", 0),
                                    new Cm::Parsing::KeywordParser("class")),
                                new Cm::Parsing::KeywordParser("delegate")),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 1))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0)))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("ParameterList", "ParameterList", 2))),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    SetStartRuleName("ClassDelegate");
    SetSkipRuleName("spaces_and_comments");
}

} } // namespace Cm.Parser
