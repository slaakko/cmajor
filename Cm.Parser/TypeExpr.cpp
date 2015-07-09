#include "TypeExpr.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Ast/Expression.hpp>
#include <Cm.Ast/Typedef.hpp>
#include <Cm.Parser/BasicType.hpp>
#include <Cm.Parser/Identifier.hpp>
#include <Cm.Parser/Template.hpp>
#include <Cm.Parser/Expression.hpp>

namespace Cm { namespace Parser {

using namespace Cm::Ast;
using namespace Cm::Parsing;

TypeExprGrammar* TypeExprGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

TypeExprGrammar* TypeExprGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    TypeExprGrammar* grammar(new TypeExprGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

TypeExprGrammar::TypeExprGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("TypeExprGrammar", parsingDomain_->GetNamespaceScope("Cm.Parser"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Ast::Node* TypeExprGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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

class TypeExprGrammar::TypeExprRule : public Cm::Parsing::Rule
{
public:
    TypeExprRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::Node*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<DerivedTypeExprNode>", "node"));
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeExprRule>(this, &TypeExprRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeExprRule>(this, &TypeExprRule::A1Action));
        a1ActionParser->SetFailureAction(new Cm::Parsing::MemberFailureAction<TypeExprRule>(this, &TypeExprRule::A1ActionFail));
        Cm::Parsing::NonterminalParser* prefixTypeExprNonterminalParser = GetNonterminal("PrefixTypeExpr");
        prefixTypeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<TypeExprRule>(this, &TypeExprRule::PrePrefixTypeExpr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ctx->BeginParsingTypeExpr();
        context.node.reset(new DerivedTypeExprNode(span));
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.node->GetSpan().SetEnd(span.End());
        context.value = MakeTypeExprNode(context.node.release());
        context.ctx->EndParsingTypeExpr();
    }
    void A1ActionFail()
    {
        context.ctx->EndParsingTypeExpr();
    }
    void PrePrefixTypeExpr(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::DerivedTypeExprNode*>(context.node.get())));
    }
private:
    struct Context
    {
        Context(): ctx(), value(), node() {}
        ParsingContext* ctx;
        Cm::Ast::Node* value;
        std::unique_ptr<DerivedTypeExprNode> node;
    };
    std::stack<Context> contextStack;
    Context context;
};

class TypeExprGrammar::PrefixTypeExprRule : public Cm::Parsing::Rule
{
public:
    PrefixTypeExprRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::DerivedTypeExprNode*", "node"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> node_value = std::move(stack.top());
        context.node = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::DerivedTypeExprNode*>*>(node_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrefixTypeExprRule>(this, &PrefixTypeExprRule::A0Action));
        Cm::Parsing::NonterminalParser* cNonterminalParser = GetNonterminal("c");
        cNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<PrefixTypeExprRule>(this, &PrefixTypeExprRule::Prec));
        Cm::Parsing::NonterminalParser* postfixTypeExprNonterminalParser = GetNonterminal("PostfixTypeExpr");
        postfixTypeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<PrefixTypeExprRule>(this, &PrefixTypeExprRule::PrePostfixTypeExpr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.node->AddConst();
    }
    void Prec(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::DerivedTypeExprNode*>(context.node)));
    }
    void PrePostfixTypeExpr(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::DerivedTypeExprNode*>(context.node)));
    }
private:
    struct Context
    {
        Context(): ctx(), node() {}
        ParsingContext* ctx;
        Cm::Ast::DerivedTypeExprNode* node;
    };
    std::stack<Context> contextStack;
    Context context;
};

class TypeExprGrammar::PostfixTypeExprRule : public Cm::Parsing::Rule
{
public:
    PostfixTypeExprRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::DerivedTypeExprNode*", "node"));
        AddLocalVariable(AttrOrVariable("Span", "s"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> node_value = std::move(stack.top());
        context.node = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::DerivedTypeExprNode*>*>(node_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A5Action));
        Cm::Parsing::ActionParser* a6ActionParser = GetAction("A6");
        a6ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A6Action));
        Cm::Parsing::NonterminalParser* primaryTypeExprNonterminalParser = GetNonterminal("PrimaryTypeExpr");
        primaryTypeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<PostfixTypeExprRule>(this, &PostfixTypeExprRule::PrePrimaryTypeExpr));
        Cm::Parsing::NonterminalParser* dotMemberIdNonterminalParser = GetNonterminal("dotMemberId");
        dotMemberIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostfixTypeExprRule>(this, &PostfixTypeExprRule::PostdotMemberId));
        Cm::Parsing::NonterminalParser* dimNonterminalParser = GetNonterminal("dim");
        dimNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<PostfixTypeExprRule>(this, &PostfixTypeExprRule::Predim));
        dimNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostfixTypeExprRule>(this, &PostfixTypeExprRule::Postdim));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.s = span;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.s.SetEnd(span.End());
        context.node->SetBaseTypeExpr(new DotNode(context.s, context.node->ReleaseBaseTypeExprNode(), context.fromdotMemberId));
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.node->AddRvalueRef();
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.node->AddReference();
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.node->AddPointer();
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.node->AddArray();
    }
    void A6Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.node->AddArrayDimensionNode(context.fromdim);
    }
    void PrePrimaryTypeExpr(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::DerivedTypeExprNode*>(context.node)));
    }
    void PostdotMemberId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromdotMemberId_value = std::move(stack.top());
            context.fromdotMemberId = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromdotMemberId_value.get());
            stack.pop();
        }
    }
    void Predim(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void Postdim(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromdim_value = std::move(stack.top());
            context.fromdim = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromdim_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), node(), s(), fromdotMemberId(), fromdim() {}
        ParsingContext* ctx;
        Cm::Ast::DerivedTypeExprNode* node;
        Span s;
        Cm::Ast::IdentifierNode* fromdotMemberId;
        Cm::Ast::Node* fromdim;
    };
    std::stack<Context> contextStack;
    Context context;
};

class TypeExprGrammar::PrimaryTypeExprRule : public Cm::Parsing::Rule
{
public:
    PrimaryTypeExprRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::DerivedTypeExprNode*", "node"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> node_value = std::move(stack.top());
        context.node = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::DerivedTypeExprNode*>*>(node_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::A4Action));
        Cm::Parsing::NonterminalParser* basicTypeNonterminalParser = GetNonterminal("BasicType");
        basicTypeNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::PostBasicType));
        Cm::Parsing::NonterminalParser* templateIdNonterminalParser = GetNonterminal("TemplateId");
        templateIdNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::PreTemplateId));
        templateIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::PostTemplateId));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* prefixTypeExprNonterminalParser = GetNonterminal("PrefixTypeExpr");
        prefixTypeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::PrePrefixTypeExpr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.node->SetBaseTypeExpr(context.fromBasicType);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.node->SetBaseTypeExpr(context.fromTemplateId);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.node->SetBaseTypeExpr(context.fromIdentifier);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.node->AddLeftParen();
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.node->AddRightParen();
    }
    void PostBasicType(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromBasicType_value = std::move(stack.top());
            context.fromBasicType = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromBasicType_value.get());
            stack.pop();
        }
    }
    void PreTemplateId(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostTemplateId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTemplateId_value = std::move(stack.top());
            context.fromTemplateId = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromTemplateId_value.get());
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
    void PrePrefixTypeExpr(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::DerivedTypeExprNode*>(context.node)));
    }
private:
    struct Context
    {
        Context(): ctx(), node(), fromBasicType(), fromTemplateId(), fromIdentifier() {}
        ParsingContext* ctx;
        Cm::Ast::DerivedTypeExprNode* node;
        Cm::Ast::Node* fromBasicType;
        Cm::Ast::Node* fromTemplateId;
        Cm::Ast::IdentifierNode* fromIdentifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

void TypeExprGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parser.ExpressionGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parser::ExpressionGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parser.IdentifierGrammar");
    if (!grammar1)
    {
        grammar1 = Cm::Parser::IdentifierGrammar::Create(pd);
    }
    AddGrammarReference(grammar1);
    Cm::Parsing::Grammar* grammar2 = pd->GetGrammar("Cm.Parser.BasicTypeGrammar");
    if (!grammar2)
    {
        grammar2 = Cm::Parser::BasicTypeGrammar::Create(pd);
    }
    AddGrammarReference(grammar2);
    Cm::Parsing::Grammar* grammar3 = pd->GetGrammar("Cm.Parser.TemplateGrammar");
    if (!grammar3)
    {
        grammar3 = Cm::Parser::TemplateGrammar::Create(pd);
    }
    AddGrammarReference(grammar3);
}

void TypeExprGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("Identifier", this, "IdentifierGrammar.Identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("BasicType", this, "BasicTypeGrammar.BasicType"));
    AddRuleLink(new Cm::Parsing::RuleLink("QualifiedId", this, "IdentifierGrammar.QualifiedId"));
    AddRuleLink(new Cm::Parsing::RuleLink("TemplateId", this, "TemplateGrammar.TemplateId"));
    AddRuleLink(new Cm::Parsing::RuleLink("Expression", this, "ExpressionGrammar.Expression"));
    AddRule(new TypeExprRule("TypeExpr", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::EmptyParser()),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("PrefixTypeExpr", "PrefixTypeExpr", 2)))));
    AddRule(new PrefixTypeExprRule("PrefixTypeExpr", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::KeywordParser("const")),
                new Cm::Parsing::NonterminalParser("c", "PostfixTypeExpr", 2)),
            new Cm::Parsing::NonterminalParser("PostfixTypeExpr", "PostfixTypeExpr", 2))));
    AddRule(new PostfixTypeExprRule("PostfixTypeExpr", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("PrimaryTypeExpr", "PrimaryTypeExpr", 2)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::AlternativeParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::CharParser('.'),
                                    new Cm::Parsing::ActionParser("A1",
                                        new Cm::Parsing::ExpectationParser(
                                            new Cm::Parsing::NonterminalParser("dotMemberId", "Identifier", 0)))),
                                new Cm::Parsing::ActionParser("A2",
                                    new Cm::Parsing::StringParser("&&"))),
                            new Cm::Parsing::ActionParser("A3",
                                new Cm::Parsing::CharParser('&'))),
                        new Cm::Parsing::ActionParser("A4",
                            new Cm::Parsing::CharParser('*'))),
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::ActionParser("A5",
                                new Cm::Parsing::CharParser('[')),
                            new Cm::Parsing::ActionParser("A6",
                                new Cm::Parsing::NonterminalParser("dim", "Expression", 1))),
                        new Cm::Parsing::CharParser(']')))))));
    AddRule(new PrimaryTypeExprRule("PrimaryTypeExpr", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::NonterminalParser("BasicType", "BasicType", 0)),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("TemplateId", "TemplateId", 1))),
                new Cm::Parsing::ActionParser("A2",
                    new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0))),
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::ActionParser("A3",
                        new Cm::Parsing::CharParser('(')),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("PrefixTypeExpr", "PrefixTypeExpr", 2))),
                new Cm::Parsing::ActionParser("A4",
                    new Cm::Parsing::CharParser(')'))))));
}

} } // namespace Cm.Parser
