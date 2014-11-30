#include "Template.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Ast/Template.hpp>
#include <Cm.Parser/Identifier.hpp>
#include <Cm.Parser/TypeExpr.hpp>

namespace Cm { namespace Parser {

using namespace Cm::Ast;
using namespace Cm::Parsing;

TemplateGrammar* TemplateGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

TemplateGrammar* TemplateGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    TemplateGrammar* grammar(new TemplateGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

TemplateGrammar::TemplateGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("TemplateGrammar", parsingDomain_->GetNamespaceScope("Cm.Parser"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Ast::Node* TemplateGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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

class TemplateGrammar::TemplateIdRule : public Cm::Parsing::Rule
{
public:
    TemplateIdRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::Node*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<TemplateIdNode>", "templateId"));
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TemplateIdRule>(this, &TemplateIdRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TemplateIdRule>(this, &TemplateIdRule::A1Action));
        a1ActionParser->SetFailureAction(new Cm::Parsing::MemberFailureAction<TemplateIdRule>(this, &TemplateIdRule::A1ActionFail));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TemplateIdRule>(this, &TemplateIdRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TemplateIdRule>(this, &TemplateIdRule::A3Action));
        Cm::Parsing::NonterminalParser* subjectNonterminalParser = GetNonterminal("subject");
        subjectNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TemplateIdRule>(this, &TemplateIdRule::Postsubject));
        Cm::Parsing::NonterminalParser* templateArgNonterminalParser = GetNonterminal("templateArg");
        templateArgNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<TemplateIdRule>(this, &TemplateIdRule::PretemplateArg));
        templateArgNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TemplateIdRule>(this, &TemplateIdRule::PosttemplateArg));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ctx->BeginParsingTemplateId();
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ctx->EndParsingTemplateId();
        context.value = context.templateId.release();
        context.value->GetSpan().SetEnd(span.End());
    }
    void A1ActionFail()
    {
        context.ctx->EndParsingTemplateId();
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.templateId.reset(new TemplateIdNode(span, context.fromsubject));
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.templateId->AddTemplateArgument(context.fromtemplateArg);
    }
    void Postsubject(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromsubject_value = std::move(stack.top());
            context.fromsubject = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromsubject_value.get());
            stack.pop();
        }
    }
    void PretemplateArg(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PosttemplateArg(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromtemplateArg_value = std::move(stack.top());
            context.fromtemplateArg = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromtemplateArg_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), templateId(), fromsubject(), fromtemplateArg() {}
        ParsingContext* ctx;
        Cm::Ast::Node* value;
        std::unique_ptr<TemplateIdNode> templateId;
        Cm::Ast::IdentifierNode* fromsubject;
        Cm::Ast::Node* fromtemplateArg;
    };
    std::stack<Context> contextStack;
    Context context;
};

class TemplateGrammar::TemplateParameterRule : public Cm::Parsing::Rule
{
public:
    TemplateParameterRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::TemplateParameterNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::TemplateParameterNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TemplateParameterRule>(this, &TemplateParameterRule::A0Action));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TemplateParameterRule>(this, &TemplateParameterRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<TemplateParameterRule>(this, &TemplateParameterRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TemplateParameterRule>(this, &TemplateParameterRule::PostTypeExpr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new TemplateParameterNode(span, context.fromIdentifier, context.fromTypeExpr);
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
private:
    struct Context
    {
        Context(): ctx(), value(), fromIdentifier(), fromTypeExpr() {}
        ParsingContext* ctx;
        Cm::Ast::TemplateParameterNode* value;
        Cm::Ast::IdentifierNode* fromIdentifier;
        Cm::Ast::Node* fromTypeExpr;
    };
    std::stack<Context> contextStack;
    Context context;
};

class TemplateGrammar::TemplateParameterListRule : public Cm::Parsing::Rule
{
public:
    TemplateParameterListRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Node*", "owner"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> owner_value = std::move(stack.top());
        context.owner = *static_cast<Cm::Parsing::ValueObject<Node*>*>(owner_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TemplateParameterListRule>(this, &TemplateParameterListRule::A0Action));
        Cm::Parsing::NonterminalParser* templateParameterNonterminalParser = GetNonterminal("TemplateParameter");
        templateParameterNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<TemplateParameterListRule>(this, &TemplateParameterListRule::PreTemplateParameter));
        templateParameterNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TemplateParameterListRule>(this, &TemplateParameterListRule::PostTemplateParameter));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.owner->AddTemplateParameter(context.fromTemplateParameter);
    }
    void PreTemplateParameter(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostTemplateParameter(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTemplateParameter_value = std::move(stack.top());
            context.fromTemplateParameter = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::TemplateParameterNode*>*>(fromTemplateParameter_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), owner(), fromTemplateParameter() {}
        ParsingContext* ctx;
        Node* owner;
        Cm::Ast::TemplateParameterNode* fromTemplateParameter;
    };
    std::stack<Context> contextStack;
    Context context;
};

void TemplateGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parser.IdentifierGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parser::IdentifierGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parser.TypeExprGrammar");
    if (!grammar1)
    {
        grammar1 = Cm::Parser::TypeExprGrammar::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void TemplateGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("QualifiedId", this, "IdentifierGrammar.QualifiedId"));
    AddRuleLink(new Cm::Parsing::RuleLink("Identifier", this, "IdentifierGrammar.Identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("TypeExpr", this, "TypeExprGrammar.TypeExpr"));
    AddRule(new TemplateIdRule("TemplateId", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::EmptyParser()),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::ActionParser("A2",
                                new Cm::Parsing::NonterminalParser("subject", "QualifiedId", 0)),
                            new Cm::Parsing::CharParser('<')),
                        new Cm::Parsing::ListParser(
                            new Cm::Parsing::ActionParser("A3",
                                new Cm::Parsing::NonterminalParser("templateArg", "TypeExpr", 1)),
                            new Cm::Parsing::CharParser(','))),
                    new Cm::Parsing::CharParser('>'))))));
    AddRule(new TemplateParameterRule("TemplateParameter", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::CharParser('='),
                        new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 1)))))));
    AddRule(new TemplateParameterListRule("TemplateParameterList", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::CharParser('<'),
                new Cm::Parsing::ListParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::NonterminalParser("TemplateParameter", "TemplateParameter", 1)),
                    new Cm::Parsing::CharParser(','))),
            new Cm::Parsing::ExpectationParser(
                new Cm::Parsing::CharParser('>')))));
}

} } // namespace Cm.Parser
