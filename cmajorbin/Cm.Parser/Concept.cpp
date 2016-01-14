#include "Concept.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Parser/Specifier.hpp>
#include <Cm.Parser/Identifier.hpp>
#include <Cm.Parser/Parameter.hpp>
#include <Cm.Parser/Expression.hpp>
#include <Cm.Parser/Function.hpp>
#include <Cm.Parser/TypeExpr.hpp>

namespace Cm { namespace Parser {

using namespace Cm::Ast;
using namespace Cm::Parsing;

ConceptGrammar* ConceptGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

ConceptGrammar* ConceptGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    ConceptGrammar* grammar(new ConceptGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

ConceptGrammar::ConceptGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("ConceptGrammar", parsingDomain_->GetNamespaceScope("Cm.Parser"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Ast::ConceptNode* ConceptGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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
    Cm::Ast::ConceptNode* result = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConceptNode*>*>(value.get());
    stack.pop();
    return result;
}

class ConceptGrammar::ConceptRule : public Cm::Parsing::Rule
{
public:
    ConceptRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::ConceptNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConceptNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConceptRule>(this, &ConceptRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConceptRule>(this, &ConceptRule::A1Action));
        a1ActionParser->SetFailureAction(new Cm::Parsing::MemberFailureAction<ConceptRule>(this, &ConceptRule::A1ActionFail));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConceptRule>(this, &ConceptRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConceptRule>(this, &ConceptRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConceptRule>(this, &ConceptRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConceptRule>(this, &ConceptRule::A5Action));
        Cm::Parsing::ActionParser* a6ActionParser = GetAction("A6");
        a6ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConceptRule>(this, &ConceptRule::A6Action));
        Cm::Parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal("Specifiers");
        specifiersNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConceptRule>(this, &ConceptRule::PostSpecifiers));
        Cm::Parsing::NonterminalParser* conceptNameNonterminalParser = GetNonterminal("conceptName");
        conceptNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConceptRule>(this, &ConceptRule::PostconceptName));
        Cm::Parsing::NonterminalParser* typeParameterNonterminalParser = GetNonterminal("typeParameter");
        typeParameterNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConceptRule>(this, &ConceptRule::PosttypeParameter));
        Cm::Parsing::NonterminalParser* refinementNonterminalParser = GetNonterminal("Refinement");
        refinementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConceptRule>(this, &ConceptRule::PostRefinement));
        Cm::Parsing::NonterminalParser* whereConstraintNonterminalParser = GetNonterminal("WhereConstraint");
        whereConstraintNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConceptRule>(this, &ConceptRule::PreWhereConstraint));
        whereConstraintNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConceptRule>(this, &ConceptRule::PostWhereConstraint));
        Cm::Parsing::NonterminalParser* conceptBodyNonterminalParser = GetNonterminal("ConceptBody");
        conceptBodyNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConceptRule>(this, &ConceptRule::PreConceptBody));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ctx->BeginParsingConcept();
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ctx->EndParsingConcept();
    }
    void A1ActionFail()
    {
        context.ctx->EndParsingConcept();
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ConceptNode(span, context.fromSpecifiers, context.fromconceptName);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->AddTypeParameter(context.fromtypeParameter);
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->GetSpan().SetEnd(span.End());
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->SetRefinement(context.fromRefinement);
    }
    void A6Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->AddConstraint(context.fromWhereConstraint);
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
    void PostconceptName(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromconceptName_value = std::move(stack.top());
            context.fromconceptName = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromconceptName_value.get());
            stack.pop();
        }
    }
    void PosttypeParameter(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromtypeParameter_value = std::move(stack.top());
            context.fromtypeParameter = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromtypeParameter_value.get());
            stack.pop();
        }
    }
    void PostRefinement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromRefinement_value = std::move(stack.top());
            context.fromRefinement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConceptIdNode*>*>(fromRefinement_value.get());
            stack.pop();
        }
    }
    void PreWhereConstraint(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostWhereConstraint(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromWhereConstraint_value = std::move(stack.top());
            context.fromWhereConstraint = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::WhereConstraintNode*>*>(fromWhereConstraint_value.get());
            stack.pop();
        }
    }
    void PreConceptBody(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConceptNode*>(context.value)));
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromSpecifiers(), fromconceptName(), fromtypeParameter(), fromRefinement(), fromWhereConstraint() {}
        ParsingContext* ctx;
        Cm::Ast::ConceptNode* value;
        Cm::Ast::Specifiers fromSpecifiers;
        Cm::Ast::IdentifierNode* fromconceptName;
        Cm::Ast::IdentifierNode* fromtypeParameter;
        Cm::Ast::ConceptIdNode* fromRefinement;
        Cm::Ast::WhereConstraintNode* fromWhereConstraint;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::RefinementRule : public Cm::Parsing::Rule
{
public:
    RefinementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::ConceptIdNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConceptIdNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RefinementRule>(this, &RefinementRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RefinementRule>(this, &RefinementRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RefinementRule>(this, &RefinementRule::A2Action));
        Cm::Parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal("QualifiedId");
        qualifiedIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RefinementRule>(this, &RefinementRule::PostQualifiedId));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RefinementRule>(this, &RefinementRule::PostIdentifier));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->GetSpan().SetEnd(span.End());
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ConceptIdNode(span, context.fromQualifiedId);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->AddTypeParameter(context.fromIdentifier);
    }
    void PostQualifiedId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromQualifiedId_value = std::move(stack.top());
            context.fromQualifiedId = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromQualifiedId_value.get());
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
private:
    struct Context
    {
        Context(): value(), fromQualifiedId(), fromIdentifier() {}
        Cm::Ast::ConceptIdNode* value;
        Cm::Ast::IdentifierNode* fromQualifiedId;
        Cm::Ast::IdentifierNode* fromIdentifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::ConceptBodyRule : public Cm::Parsing::Rule
{
public:
    ConceptBodyRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::ConceptNode*", "concept"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> concept_value = std::move(stack.top());
        context.concept = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConceptNode*>*>(concept_value.get());
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
        Cm::Parsing::NonterminalParser* conceptBodyConstraintNonterminalParser = GetNonterminal("ConceptBodyConstraint");
        conceptBodyConstraintNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConceptBodyRule>(this, &ConceptBodyRule::PreConceptBodyConstraint));
        Cm::Parsing::NonterminalParser* axiomNonterminalParser = GetNonterminal("Axiom");
        axiomNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConceptBodyRule>(this, &ConceptBodyRule::PreAxiom));
    }
    void PreConceptBodyConstraint(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConceptNode*>(context.concept)));
    }
    void PreAxiom(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConceptNode*>(context.concept)));
    }
private:
    struct Context
    {
        Context(): ctx(), concept() {}
        ParsingContext* ctx;
        Cm::Ast::ConceptNode* concept;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::ConceptBodyConstraintRule : public Cm::Parsing::Rule
{
public:
    ConceptBodyConstraintRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::ConceptNode*", "concept"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> concept_value = std::move(stack.top());
        context.concept = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConceptNode*>*>(concept_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::A2Action));
        Cm::Parsing::NonterminalParser* typenameConstraintNonterminalParser = GetNonterminal("TypenameConstraint");
        typenameConstraintNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::PreTypenameConstraint));
        typenameConstraintNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::PostTypenameConstraint));
        Cm::Parsing::NonterminalParser* signatureConstraintNonterminalParser = GetNonterminal("SignatureConstraint");
        signatureConstraintNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::PreSignatureConstraint));
        signatureConstraintNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::PostSignatureConstraint));
        Cm::Parsing::NonterminalParser* embeddedConstraintNonterminalParser = GetNonterminal("EmbeddedConstraint");
        embeddedConstraintNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::PreEmbeddedConstraint));
        embeddedConstraintNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::PostEmbeddedConstraint));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.concept->AddConstraint(context.fromTypenameConstraint);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.concept->AddConstraint(context.fromSignatureConstraint);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.concept->AddConstraint(context.fromEmbeddedConstraint);
    }
    void PreTypenameConstraint(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostTypenameConstraint(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypenameConstraint_value = std::move(stack.top());
            context.fromTypenameConstraint = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>*>(fromTypenameConstraint_value.get());
            stack.pop();
        }
    }
    void PreSignatureConstraint(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<std::string>(context.concept->FirstTypeParameter())));
    }
    void PostSignatureConstraint(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromSignatureConstraint_value = std::move(stack.top());
            context.fromSignatureConstraint = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>*>(fromSignatureConstraint_value.get());
            stack.pop();
        }
    }
    void PreEmbeddedConstraint(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostEmbeddedConstraint(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromEmbeddedConstraint_value = std::move(stack.top());
            context.fromEmbeddedConstraint = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>*>(fromEmbeddedConstraint_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), concept(), fromTypenameConstraint(), fromSignatureConstraint(), fromEmbeddedConstraint() {}
        ParsingContext* ctx;
        Cm::Ast::ConceptNode* concept;
        Cm::Ast::ConstraintNode* fromTypenameConstraint;
        Cm::Ast::ConstraintNode* fromSignatureConstraint;
        Cm::Ast::ConstraintNode* fromEmbeddedConstraint;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::AxiomRule : public Cm::Parsing::Rule
{
public:
    AxiomRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::ConceptNode*", "concept"));
        AddLocalVariable(AttrOrVariable("std::unique_ptr<AxiomNode>", "axiom"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> concept_value = std::move(stack.top());
        context.concept = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConceptNode*>*>(concept_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AxiomRule>(this, &AxiomRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AxiomRule>(this, &AxiomRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AxiomRule>(this, &AxiomRule::A2Action));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AxiomRule>(this, &AxiomRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal("ParameterList");
        parameterListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<AxiomRule>(this, &AxiomRule::PreParameterList));
        Cm::Parsing::NonterminalParser* axiomBodyNonterminalParser = GetNonterminal("AxiomBody");
        axiomBodyNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<AxiomRule>(this, &AxiomRule::PreAxiomBody));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.concept->AddAxiom(context.axiom.release());
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.axiom->GetSpan().SetEnd(span.End());
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.axiom.reset(new AxiomNode(span, context.fromIdentifier));
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
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.axiom.get())));
    }
    void PreAxiomBody(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::AxiomNode*>(context.axiom.get())));
    }
private:
    struct Context
    {
        Context(): ctx(), concept(), axiom(), fromIdentifier() {}
        ParsingContext* ctx;
        Cm::Ast::ConceptNode* concept;
        std::unique_ptr<AxiomNode> axiom;
        Cm::Ast::IdentifierNode* fromIdentifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::AxiomBodyRule : public Cm::Parsing::Rule
{
public:
    AxiomBodyRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::AxiomNode*", "axiom"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> axiom_value = std::move(stack.top());
        context.axiom = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::AxiomNode*>*>(axiom_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AxiomBodyRule>(this, &AxiomBodyRule::A0Action));
        Cm::Parsing::NonterminalParser* axiomStatementNonterminalParser = GetNonterminal("AxiomStatement");
        axiomStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<AxiomBodyRule>(this, &AxiomBodyRule::PreAxiomStatement));
        axiomStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AxiomBodyRule>(this, &AxiomBodyRule::PostAxiomStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.axiom->AddStatement(context.fromAxiomStatement);
    }
    void PreAxiomStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostAxiomStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAxiomStatement_value = std::move(stack.top());
            context.fromAxiomStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::AxiomStatementNode*>*>(fromAxiomStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), axiom(), fromAxiomStatement() {}
        ParsingContext* ctx;
        Cm::Ast::AxiomNode* axiom;
        Cm::Ast::AxiomStatementNode* fromAxiomStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::AxiomStatementRule : public Cm::Parsing::Rule
{
public:
    AxiomStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::AxiomStatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::AxiomStatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AxiomStatementRule>(this, &AxiomStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<AxiomStatementRule>(this, &AxiomStatementRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AxiomStatementRule>(this, &AxiomStatementRule::PostExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new AxiomStatementNode(span, context.fromExpression, std::string(matchBegin, matchEnd));
    }
    void PreExpression(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromExpression_value = std::move(stack.top());
            context.fromExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromExpression() {}
        ParsingContext* ctx;
        Cm::Ast::AxiomStatementNode* value;
        Cm::Ast::Node* fromExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::TypenameConstraintRule : public Cm::Parsing::Rule
{
public:
    TypenameConstraintRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::ConstraintNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypenameConstraintRule>(this, &TypenameConstraintRule::A0Action));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<TypenameConstraintRule>(this, &TypenameConstraintRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypenameConstraintRule>(this, &TypenameConstraintRule::PostTypeExpr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new TypenameConstraintNode(span, context.fromTypeExpr);
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
        Context(): ctx(), value(), fromTypeExpr() {}
        ParsingContext* ctx;
        Cm::Ast::ConstraintNode* value;
        Cm::Ast::Node* fromTypeExpr;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::SignatureConstraintRule : public Cm::Parsing::Rule
{
public:
    SignatureConstraintRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("std::string", "firstTypeParameter"));
        SetValueTypeName("Cm::Ast::ConstraintNode*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> firstTypeParameter_value = std::move(stack.top());
        context.firstTypeParameter = *static_cast<Cm::Parsing::ValueObject<std::string>*>(firstTypeParameter_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SignatureConstraintRule>(this, &SignatureConstraintRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SignatureConstraintRule>(this, &SignatureConstraintRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SignatureConstraintRule>(this, &SignatureConstraintRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SignatureConstraintRule>(this, &SignatureConstraintRule::A3Action));
        Cm::Parsing::NonterminalParser* constructorConstraintNonterminalParser = GetNonterminal("ConstructorConstraint");
        constructorConstraintNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<SignatureConstraintRule>(this, &SignatureConstraintRule::PreConstructorConstraint));
        constructorConstraintNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SignatureConstraintRule>(this, &SignatureConstraintRule::PostConstructorConstraint));
        Cm::Parsing::NonterminalParser* destructorConstraintNonterminalParser = GetNonterminal("DestructorConstraint");
        destructorConstraintNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<SignatureConstraintRule>(this, &SignatureConstraintRule::PreDestructorConstraint));
        destructorConstraintNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SignatureConstraintRule>(this, &SignatureConstraintRule::PostDestructorConstraint));
        Cm::Parsing::NonterminalParser* memberFunctionConstraintNonterminalParser = GetNonterminal("MemberFunctionConstraint");
        memberFunctionConstraintNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<SignatureConstraintRule>(this, &SignatureConstraintRule::PreMemberFunctionConstraint));
        memberFunctionConstraintNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SignatureConstraintRule>(this, &SignatureConstraintRule::PostMemberFunctionConstraint));
        Cm::Parsing::NonterminalParser* functionConstraintNonterminalParser = GetNonterminal("FunctionConstraint");
        functionConstraintNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<SignatureConstraintRule>(this, &SignatureConstraintRule::PreFunctionConstraint));
        functionConstraintNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SignatureConstraintRule>(this, &SignatureConstraintRule::PostFunctionConstraint));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromConstructorConstraint;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromDestructorConstraint;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromMemberFunctionConstraint;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromFunctionConstraint;
    }
    void PreConstructorConstraint(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<std::string>(context.firstTypeParameter)));
    }
    void PostConstructorConstraint(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConstructorConstraint_value = std::move(stack.top());
            context.fromConstructorConstraint = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>*>(fromConstructorConstraint_value.get());
            stack.pop();
        }
    }
    void PreDestructorConstraint(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<std::string>(context.firstTypeParameter)));
    }
    void PostDestructorConstraint(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDestructorConstraint_value = std::move(stack.top());
            context.fromDestructorConstraint = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>*>(fromDestructorConstraint_value.get());
            stack.pop();
        }
    }
    void PreMemberFunctionConstraint(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostMemberFunctionConstraint(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromMemberFunctionConstraint_value = std::move(stack.top());
            context.fromMemberFunctionConstraint = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>*>(fromMemberFunctionConstraint_value.get());
            stack.pop();
        }
    }
    void PreFunctionConstraint(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostFunctionConstraint(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFunctionConstraint_value = std::move(stack.top());
            context.fromFunctionConstraint = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>*>(fromFunctionConstraint_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), firstTypeParameter(), value(), fromConstructorConstraint(), fromDestructorConstraint(), fromMemberFunctionConstraint(), fromFunctionConstraint() {}
        ParsingContext* ctx;
        std::string firstTypeParameter;
        Cm::Ast::ConstraintNode* value;
        Cm::Ast::ConstraintNode* fromConstructorConstraint;
        Cm::Ast::ConstraintNode* fromDestructorConstraint;
        Cm::Ast::ConstraintNode* fromMemberFunctionConstraint;
        Cm::Ast::ConstraintNode* fromFunctionConstraint;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::ConstructorConstraintRule : public Cm::Parsing::Rule
{
public:
    ConstructorConstraintRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("std::string", "firstTypeParameter"));
        SetValueTypeName("Cm::Ast::ConstraintNode*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<IdentifierNode>", "id"));
        AddLocalVariable(AttrOrVariable("std::unique_ptr<ConstraintNode>", "ctorConstraint"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> firstTypeParameter_value = std::move(stack.top());
        context.firstTypeParameter = *static_cast<Cm::Parsing::ValueObject<std::string>*>(firstTypeParameter_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConstructorConstraintRule>(this, &ConstructorConstraintRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConstructorConstraintRule>(this, &ConstructorConstraintRule::A1Action));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConstructorConstraintRule>(this, &ConstructorConstraintRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal("ParameterList");
        parameterListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConstructorConstraintRule>(this, &ConstructorConstraintRule::PreParameterList));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.ctorConstraint.release();
        context.value->GetSpan().SetEnd(span.End());
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.id.reset(context.fromIdentifier);
        pass = context.fromIdentifier->Str() == context.firstTypeParameter;
        if (pass) context.ctorConstraint.reset(new ConstructorConstraintNode(span, context.id.release()));
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
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.ctorConstraint.get())));
    }
private:
    struct Context
    {
        Context(): ctx(), firstTypeParameter(), value(), id(), ctorConstraint(), fromIdentifier() {}
        ParsingContext* ctx;
        std::string firstTypeParameter;
        Cm::Ast::ConstraintNode* value;
        std::unique_ptr<IdentifierNode> id;
        std::unique_ptr<ConstraintNode> ctorConstraint;
        Cm::Ast::IdentifierNode* fromIdentifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::DestructorConstraintRule : public Cm::Parsing::Rule
{
public:
    DestructorConstraintRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("std::string", "firstTypeParameter"));
        SetValueTypeName("Cm::Ast::ConstraintNode*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<IdentifierNode>", "id"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> firstTypeParameter_value = std::move(stack.top());
        context.firstTypeParameter = *static_cast<Cm::Parsing::ValueObject<std::string>*>(firstTypeParameter_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DestructorConstraintRule>(this, &DestructorConstraintRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DestructorConstraintRule>(this, &DestructorConstraintRule::A1Action));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DestructorConstraintRule>(this, &DestructorConstraintRule::PostIdentifier));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new DestructorConstraintNode(span, context.id.release());
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.id.reset(context.fromIdentifier);
        pass = context.fromIdentifier->Str() == context.firstTypeParameter;
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
private:
    struct Context
    {
        Context(): ctx(), firstTypeParameter(), value(), id(), fromIdentifier() {}
        ParsingContext* ctx;
        std::string firstTypeParameter;
        Cm::Ast::ConstraintNode* value;
        std::unique_ptr<IdentifierNode> id;
        Cm::Ast::IdentifierNode* fromIdentifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::MemberFunctionConstraintRule : public Cm::Parsing::Rule
{
public:
    MemberFunctionConstraintRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::ConstraintNode*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<Node>", "returnType"));
        AddLocalVariable(AttrOrVariable("std::unique_ptr<IdentifierNode>", "typeParam"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::A3Action));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::PostTypeExpr));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* functionGroupIdNonterminalParser = GetNonterminal("FunctionGroupId");
        functionGroupIdNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::PreFunctionGroupId));
        functionGroupIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::PostFunctionGroupId));
        Cm::Parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal("ParameterList");
        parameterListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::PreParameterList));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->GetSpan().SetEnd(span.End());
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.returnType.reset(context.fromTypeExpr);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.typeParam.reset(context.fromIdentifier);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new MemberFunctionConstraintNode(span, context.returnType.release(), context.typeParam.release(), context.fromFunctionGroupId);
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
    void PreFunctionGroupId(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostFunctionGroupId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFunctionGroupId_value = std::move(stack.top());
            context.fromFunctionGroupId = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::FunctionGroupIdNode*>*>(fromFunctionGroupId_value.get());
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
        Context(): ctx(), value(), returnType(), typeParam(), fromTypeExpr(), fromIdentifier(), fromFunctionGroupId() {}
        ParsingContext* ctx;
        Cm::Ast::ConstraintNode* value;
        std::unique_ptr<Node> returnType;
        std::unique_ptr<IdentifierNode> typeParam;
        Cm::Ast::Node* fromTypeExpr;
        Cm::Ast::IdentifierNode* fromIdentifier;
        Cm::Ast::FunctionGroupIdNode* fromFunctionGroupId;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::FunctionConstraintRule : public Cm::Parsing::Rule
{
public:
    FunctionConstraintRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::ConstraintNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<FunctionConstraintRule>(this, &FunctionConstraintRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<FunctionConstraintRule>(this, &FunctionConstraintRule::A1Action));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<FunctionConstraintRule>(this, &FunctionConstraintRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<FunctionConstraintRule>(this, &FunctionConstraintRule::PostTypeExpr));
        Cm::Parsing::NonterminalParser* functionGroupIdNonterminalParser = GetNonterminal("FunctionGroupId");
        functionGroupIdNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<FunctionConstraintRule>(this, &FunctionConstraintRule::PreFunctionGroupId));
        functionGroupIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<FunctionConstraintRule>(this, &FunctionConstraintRule::PostFunctionGroupId));
        Cm::Parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal("ParameterList");
        parameterListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<FunctionConstraintRule>(this, &FunctionConstraintRule::PreParameterList));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->GetSpan().SetEnd(span.End());
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new FunctionConstraintNode(span, context.fromTypeExpr, context.fromFunctionGroupId);
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
    void PreFunctionGroupId(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostFunctionGroupId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFunctionGroupId_value = std::move(stack.top());
            context.fromFunctionGroupId = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::FunctionGroupIdNode*>*>(fromFunctionGroupId_value.get());
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
        Context(): ctx(), value(), fromTypeExpr(), fromFunctionGroupId() {}
        ParsingContext* ctx;
        Cm::Ast::ConstraintNode* value;
        Cm::Ast::Node* fromTypeExpr;
        Cm::Ast::FunctionGroupIdNode* fromFunctionGroupId;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::EmbeddedConstraintRule : public Cm::Parsing::Rule
{
public:
    EmbeddedConstraintRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::ConstraintNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<EmbeddedConstraintRule>(this, &EmbeddedConstraintRule::A0Action));
        Cm::Parsing::NonterminalParser* whereConstraintNonterminalParser = GetNonterminal("WhereConstraint");
        whereConstraintNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<EmbeddedConstraintRule>(this, &EmbeddedConstraintRule::PreWhereConstraint));
        whereConstraintNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<EmbeddedConstraintRule>(this, &EmbeddedConstraintRule::PostWhereConstraint));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromWhereConstraint;
    }
    void PreWhereConstraint(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostWhereConstraint(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromWhereConstraint_value = std::move(stack.top());
            context.fromWhereConstraint = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::WhereConstraintNode*>*>(fromWhereConstraint_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromWhereConstraint() {}
        ParsingContext* ctx;
        Cm::Ast::ConstraintNode* value;
        Cm::Ast::WhereConstraintNode* fromWhereConstraint;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::WhereConstraintRule : public Cm::Parsing::Rule
{
public:
    WhereConstraintRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::WhereConstraintNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::WhereConstraintNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<WhereConstraintRule>(this, &WhereConstraintRule::A0Action));
        Cm::Parsing::NonterminalParser* constraintExprNonterminalParser = GetNonterminal("ConstraintExpr");
        constraintExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<WhereConstraintRule>(this, &WhereConstraintRule::PreConstraintExpr));
        constraintExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<WhereConstraintRule>(this, &WhereConstraintRule::PostConstraintExpr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new WhereConstraintNode(span, context.fromConstraintExpr);
    }
    void PreConstraintExpr(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostConstraintExpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConstraintExpr_value = std::move(stack.top());
            context.fromConstraintExpr = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>*>(fromConstraintExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromConstraintExpr() {}
        ParsingContext* ctx;
        Cm::Ast::WhereConstraintNode* value;
        Cm::Ast::ConstraintNode* fromConstraintExpr;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::ConstraintExprRule : public Cm::Parsing::Rule
{
public:
    ConstraintExprRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::ConstraintNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConstraintExprRule>(this, &ConstraintExprRule::A0Action));
        Cm::Parsing::NonterminalParser* disjunctiveConstraintExprNonterminalParser = GetNonterminal("DisjunctiveConstraintExpr");
        disjunctiveConstraintExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConstraintExprRule>(this, &ConstraintExprRule::PreDisjunctiveConstraintExpr));
        disjunctiveConstraintExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConstraintExprRule>(this, &ConstraintExprRule::PostDisjunctiveConstraintExpr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromDisjunctiveConstraintExpr;
    }
    void PreDisjunctiveConstraintExpr(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostDisjunctiveConstraintExpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDisjunctiveConstraintExpr_value = std::move(stack.top());
            context.fromDisjunctiveConstraintExpr = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>*>(fromDisjunctiveConstraintExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromDisjunctiveConstraintExpr() {}
        ParsingContext* ctx;
        Cm::Ast::ConstraintNode* value;
        Cm::Ast::ConstraintNode* fromDisjunctiveConstraintExpr;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::DisjunctiveConstraintExprRule : public Cm::Parsing::Rule
{
public:
    DisjunctiveConstraintExprRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::ConstraintNode*");
        AddLocalVariable(AttrOrVariable("Span", "s"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DisjunctiveConstraintExprRule>(this, &DisjunctiveConstraintExprRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DisjunctiveConstraintExprRule>(this, &DisjunctiveConstraintExprRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DisjunctiveConstraintExprRule>(this, &DisjunctiveConstraintExprRule::Preleft));
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DisjunctiveConstraintExprRule>(this, &DisjunctiveConstraintExprRule::Postleft));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DisjunctiveConstraintExprRule>(this, &DisjunctiveConstraintExprRule::Preright));
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DisjunctiveConstraintExprRule>(this, &DisjunctiveConstraintExprRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
        context.s = span;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.s.SetEnd(span.End());
        context.value = new DisjunctiveConstraintNode(context.s, context.value, context.fromright);
    }
    void Preleft(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Cm::Ast::ConstraintNode* value;
        Span s;
        Cm::Ast::ConstraintNode* fromleft;
        Cm::Ast::ConstraintNode* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::ConjunctiveConstraintExprRule : public Cm::Parsing::Rule
{
public:
    ConjunctiveConstraintExprRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::ConstraintNode*");
        AddLocalVariable(AttrOrVariable("Span", "s"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConjunctiveConstraintExprRule>(this, &ConjunctiveConstraintExprRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConjunctiveConstraintExprRule>(this, &ConjunctiveConstraintExprRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConjunctiveConstraintExprRule>(this, &ConjunctiveConstraintExprRule::Preleft));
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConjunctiveConstraintExprRule>(this, &ConjunctiveConstraintExprRule::Postleft));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConjunctiveConstraintExprRule>(this, &ConjunctiveConstraintExprRule::Preright));
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConjunctiveConstraintExprRule>(this, &ConjunctiveConstraintExprRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
        context.s = span;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.s.SetEnd(span.End());
        context.value = new ConjunctiveConstraintNode(context.s, context.value, context.fromright);
    }
    void Preleft(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Cm::Ast::ConstraintNode* value;
        Span s;
        Cm::Ast::ConstraintNode* fromleft;
        Cm::Ast::ConstraintNode* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::PrimaryConstraintExprRule : public Cm::Parsing::Rule
{
public:
    PrimaryConstraintExprRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::ConstraintNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryConstraintExprRule>(this, &PrimaryConstraintExprRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryConstraintExprRule>(this, &PrimaryConstraintExprRule::A1Action));
        Cm::Parsing::NonterminalParser* constraintExprNonterminalParser = GetNonterminal("ConstraintExpr");
        constraintExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<PrimaryConstraintExprRule>(this, &PrimaryConstraintExprRule::PreConstraintExpr));
        constraintExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryConstraintExprRule>(this, &PrimaryConstraintExprRule::PostConstraintExpr));
        Cm::Parsing::NonterminalParser* atomicConstraintExprNonterminalParser = GetNonterminal("AtomicConstraintExpr");
        atomicConstraintExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<PrimaryConstraintExprRule>(this, &PrimaryConstraintExprRule::PreAtomicConstraintExpr));
        atomicConstraintExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryConstraintExprRule>(this, &PrimaryConstraintExprRule::PostAtomicConstraintExpr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromConstraintExpr;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromAtomicConstraintExpr;
    }
    void PreConstraintExpr(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostConstraintExpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConstraintExpr_value = std::move(stack.top());
            context.fromConstraintExpr = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>*>(fromConstraintExpr_value.get());
            stack.pop();
        }
    }
    void PreAtomicConstraintExpr(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostAtomicConstraintExpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAtomicConstraintExpr_value = std::move(stack.top());
            context.fromAtomicConstraintExpr = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>*>(fromAtomicConstraintExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromConstraintExpr(), fromAtomicConstraintExpr() {}
        ParsingContext* ctx;
        Cm::Ast::ConstraintNode* value;
        Cm::Ast::ConstraintNode* fromConstraintExpr;
        Cm::Ast::ConstraintNode* fromAtomicConstraintExpr;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::AtomicConstraintExprRule : public Cm::Parsing::Rule
{
public:
    AtomicConstraintExprRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::ConstraintNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AtomicConstraintExprRule>(this, &AtomicConstraintExprRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AtomicConstraintExprRule>(this, &AtomicConstraintExprRule::A1Action));
        Cm::Parsing::NonterminalParser* isConstraintNonterminalParser = GetNonterminal("IsConstraint");
        isConstraintNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<AtomicConstraintExprRule>(this, &AtomicConstraintExprRule::PreIsConstraint));
        isConstraintNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AtomicConstraintExprRule>(this, &AtomicConstraintExprRule::PostIsConstraint));
        Cm::Parsing::NonterminalParser* multiParamConstraintNonterminalParser = GetNonterminal("MultiParamConstraint");
        multiParamConstraintNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<AtomicConstraintExprRule>(this, &AtomicConstraintExprRule::PreMultiParamConstraint));
        multiParamConstraintNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AtomicConstraintExprRule>(this, &AtomicConstraintExprRule::PostMultiParamConstraint));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromIsConstraint;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromMultiParamConstraint;
    }
    void PreIsConstraint(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostIsConstraint(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIsConstraint_value = std::move(stack.top());
            context.fromIsConstraint = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>*>(fromIsConstraint_value.get());
            stack.pop();
        }
    }
    void PreMultiParamConstraint(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostMultiParamConstraint(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromMultiParamConstraint_value = std::move(stack.top());
            context.fromMultiParamConstraint = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>*>(fromMultiParamConstraint_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromIsConstraint(), fromMultiParamConstraint() {}
        ParsingContext* ctx;
        Cm::Ast::ConstraintNode* value;
        Cm::Ast::ConstraintNode* fromIsConstraint;
        Cm::Ast::ConstraintNode* fromMultiParamConstraint;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::IsConstraintRule : public Cm::Parsing::Rule
{
public:
    IsConstraintRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::ConstraintNode*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<Node>", "typeExpr"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IsConstraintRule>(this, &IsConstraintRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IsConstraintRule>(this, &IsConstraintRule::A1Action));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<IsConstraintRule>(this, &IsConstraintRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IsConstraintRule>(this, &IsConstraintRule::PostTypeExpr));
        Cm::Parsing::NonterminalParser* conceptOrTypeNameNonterminalParser = GetNonterminal("ConceptOrTypeName");
        conceptOrTypeNameNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<IsConstraintRule>(this, &IsConstraintRule::PreConceptOrTypeName));
        conceptOrTypeNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IsConstraintRule>(this, &IsConstraintRule::PostConceptOrTypeName));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new IsConstraintNode(span, context.typeExpr.release(), context.fromConceptOrTypeName);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.typeExpr.reset(context.fromTypeExpr);
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
    void PreConceptOrTypeName(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostConceptOrTypeName(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConceptOrTypeName_value = std::move(stack.top());
            context.fromConceptOrTypeName = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromConceptOrTypeName_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), typeExpr(), fromTypeExpr(), fromConceptOrTypeName() {}
        ParsingContext* ctx;
        Cm::Ast::ConstraintNode* value;
        std::unique_ptr<Node> typeExpr;
        Cm::Ast::Node* fromTypeExpr;
        Cm::Ast::Node* fromConceptOrTypeName;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::ConceptOrTypeNameRule : public Cm::Parsing::Rule
{
public:
    ConceptOrTypeNameRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConceptOrTypeNameRule>(this, &ConceptOrTypeNameRule::A0Action));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConceptOrTypeNameRule>(this, &ConceptOrTypeNameRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConceptOrTypeNameRule>(this, &ConceptOrTypeNameRule::PostTypeExpr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromTypeExpr;
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
        Context(): ctx(), value(), fromTypeExpr() {}
        ParsingContext* ctx;
        Cm::Ast::Node* value;
        Cm::Ast::Node* fromTypeExpr;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ConceptGrammar::MultiParamConstraintRule : public Cm::Parsing::Rule
{
public:
    MultiParamConstraintRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::ConstraintNode*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<MultiParamConstraintNode>", "constraint"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConstraintNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MultiParamConstraintRule>(this, &MultiParamConstraintRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MultiParamConstraintRule>(this, &MultiParamConstraintRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MultiParamConstraintRule>(this, &MultiParamConstraintRule::A2Action));
        Cm::Parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal("QualifiedId");
        qualifiedIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MultiParamConstraintRule>(this, &MultiParamConstraintRule::PostQualifiedId));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<MultiParamConstraintRule>(this, &MultiParamConstraintRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MultiParamConstraintRule>(this, &MultiParamConstraintRule::PostTypeExpr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.constraint.release();
        context.value->GetSpan().SetEnd(span.End());
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.constraint.reset(new MultiParamConstraintNode(span, context.fromQualifiedId));
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.constraint->AddTypeExpr(context.fromTypeExpr);
    }
    void PostQualifiedId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromQualifiedId_value = std::move(stack.top());
            context.fromQualifiedId = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromQualifiedId_value.get());
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
        Context(): ctx(), value(), constraint(), fromQualifiedId(), fromTypeExpr() {}
        ParsingContext* ctx;
        Cm::Ast::ConstraintNode* value;
        std::unique_ptr<MultiParamConstraintNode> constraint;
        Cm::Ast::IdentifierNode* fromQualifiedId;
        Cm::Ast::Node* fromTypeExpr;
    };
    std::stack<Context> contextStack;
    Context context;
};

void ConceptGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parser.SpecifierGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parser::SpecifierGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parser.ParameterGrammar");
    if (!grammar1)
    {
        grammar1 = Cm::Parser::ParameterGrammar::Create(pd);
    }
    AddGrammarReference(grammar1);
    Cm::Parsing::Grammar* grammar2 = pd->GetGrammar("Cm.Parser.IdentifierGrammar");
    if (!grammar2)
    {
        grammar2 = Cm::Parser::IdentifierGrammar::Create(pd);
    }
    AddGrammarReference(grammar2);
    Cm::Parsing::Grammar* grammar3 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar3)
    {
        grammar3 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar3);
    Cm::Parsing::Grammar* grammar4 = pd->GetGrammar("Cm.Parser.ExpressionGrammar");
    if (!grammar4)
    {
        grammar4 = Cm::Parser::ExpressionGrammar::Create(pd);
    }
    AddGrammarReference(grammar4);
    Cm::Parsing::Grammar* grammar5 = pd->GetGrammar("Cm.Parser.TypeExprGrammar");
    if (!grammar5)
    {
        grammar5 = Cm::Parser::TypeExprGrammar::Create(pd);
    }
    AddGrammarReference(grammar5);
    Cm::Parsing::Grammar* grammar6 = pd->GetGrammar("Cm.Parser.FunctionGrammar");
    if (!grammar6)
    {
        grammar6 = Cm::Parser::FunctionGrammar::Create(pd);
    }
    AddGrammarReference(grammar6);
}

void ConceptGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("Specifiers", this, "SpecifierGrammar.Specifiers"));
    AddRuleLink(new Cm::Parsing::RuleLink("spaces_and_comments", this, "Cm.Parsing.stdlib.spaces_and_comments"));
    AddRuleLink(new Cm::Parsing::RuleLink("Identifier", this, "IdentifierGrammar.Identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("QualifiedId", this, "IdentifierGrammar.QualifiedId"));
    AddRuleLink(new Cm::Parsing::RuleLink("ParameterList", this, "ParameterGrammar.ParameterList"));
    AddRuleLink(new Cm::Parsing::RuleLink("Expression", this, "ExpressionGrammar.Expression"));
    AddRuleLink(new Cm::Parsing::RuleLink("TypeExpr", this, "TypeExprGrammar.TypeExpr"));
    AddRuleLink(new Cm::Parsing::RuleLink("FunctionGroupId", this, "FunctionGrammar.FunctionGroupId"));
    AddRule(new ConceptRule("Concept", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::EmptyParser()),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::SequenceParser(
                                            new Cm::Parsing::ActionParser("A2",
                                                new Cm::Parsing::SequenceParser(
                                                    new Cm::Parsing::SequenceParser(
                                                        new Cm::Parsing::NonterminalParser("Specifiers", "Specifiers", 0),
                                                        new Cm::Parsing::KeywordParser("concept")),
                                                    new Cm::Parsing::ExpectationParser(
                                                        new Cm::Parsing::NonterminalParser("conceptName", "Identifier", 0)))),
                                            new Cm::Parsing::ExpectationParser(
                                                new Cm::Parsing::CharParser('<'))),
                                        new Cm::Parsing::ListParser(
                                            new Cm::Parsing::ActionParser("A3",
                                                new Cm::Parsing::NonterminalParser("typeParameter", "Identifier", 0)),
                                            new Cm::Parsing::CharParser(','))),
                                    new Cm::Parsing::ExpectationParser(
                                        new Cm::Parsing::CharParser('>'))),
                                new Cm::Parsing::ActionParser("A4",
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::OptionalParser(
                                            new Cm::Parsing::ActionParser("A5",
                                                new Cm::Parsing::NonterminalParser("Refinement", "Refinement", 0))),
                                        new Cm::Parsing::OptionalParser(
                                            new Cm::Parsing::ActionParser("A6",
                                                new Cm::Parsing::NonterminalParser("WhereConstraint", "WhereConstraint", 1)))))),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::CharParser('{'))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("ConceptBody", "ConceptBody", 2))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser('}')))))));
    AddRule(new RefinementRule("Refinement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::CharParser(':'),
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::NonterminalParser("QualifiedId", "QualifiedId", 0)))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser('<'))),
                    new Cm::Parsing::ListParser(
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0)),
                        new Cm::Parsing::CharParser(','))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser('>'))))));
    AddRule(new ConceptBodyRule("ConceptBody", GetScope(),
        new Cm::Parsing::KleeneStarParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::NonterminalParser("ConceptBodyConstraint", "ConceptBodyConstraint", 2),
                new Cm::Parsing::NonterminalParser("Axiom", "Axiom", 2)))));
    AddRule(new ConceptBodyConstraintRule("ConceptBodyConstraint", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("TypenameConstraint", "TypenameConstraint", 1)),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::NonterminalParser("SignatureConstraint", "SignatureConstraint", 2))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::NonterminalParser("EmbeddedConstraint", "EmbeddedConstraint", 1)))));
    AddRule(new AxiomRule("Axiom", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::KeywordParser("axiom"),
                                        new Cm::Parsing::OptionalParser(
                                            new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0))),
                                    new Cm::Parsing::ActionParser("A2",
                                        new Cm::Parsing::EmptyParser())),
                                new Cm::Parsing::OptionalParser(
                                    new Cm::Parsing::NonterminalParser("ParameterList", "ParameterList", 2)))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser('{'))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("AxiomBody", "AxiomBody", 2))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser('}'))))));
    AddRule(new AxiomBodyRule("AxiomBody", GetScope(),
        new Cm::Parsing::KleeneStarParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("AxiomStatement", "AxiomStatement", 1)))));
    AddRule(new AxiomStatementRule("AxiomStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::NonterminalParser("Expression", "Expression", 1),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new TypenameConstraintRule("TypenameConstraint", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("typename"),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 1))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new SignatureConstraintRule("SignatureConstraint", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::NonterminalParser("ConstructorConstraint", "ConstructorConstraint", 2)),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("DestructorConstraint", "DestructorConstraint", 2))),
                new Cm::Parsing::ActionParser("A2",
                    new Cm::Parsing::NonterminalParser("MemberFunctionConstraint", "MemberFunctionConstraint", 1))),
            new Cm::Parsing::ActionParser("A3",
                new Cm::Parsing::NonterminalParser("FunctionConstraint", "FunctionConstraint", 1)))));
    AddRule(new ConstructorConstraintRule("ConstructorConstraint", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::KeywordParser("explicit")),
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0))),
                    new Cm::Parsing::NonterminalParser("ParameterList", "ParameterList", 2)),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new DestructorConstraintRule("DestructorConstraint", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::CharParser('~'),
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser('('))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser(')'))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new MemberFunctionConstraintRule("MemberFunctionConstraint", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::ActionParser("A1",
                                    new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 1)),
                                new Cm::Parsing::ActionParser("A2",
                                    new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0))),
                            new Cm::Parsing::CharParser('.')),
                        new Cm::Parsing::ActionParser("A3",
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::NonterminalParser("FunctionGroupId", "FunctionGroupId", 1)))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("ParameterList", "ParameterList", 2))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new FunctionConstraintRule("FunctionConstraint", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 1),
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::NonterminalParser("FunctionGroupId", "FunctionGroupId", 1)))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("ParameterList", "ParameterList", 2))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new EmbeddedConstraintRule("EmbeddedConstraint", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::NonterminalParser("WhereConstraint", "WhereConstraint", 1),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new WhereConstraintRule("WhereConstraint", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::KeywordParser("where"),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("ConstraintExpr", "ConstraintExpr", 1))))));
    AddRule(new ConstraintExprRule("ConstraintExpr", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("DisjunctiveConstraintExpr", "DisjunctiveConstraintExpr", 1))));
    AddRule(new DisjunctiveConstraintExprRule("DisjunctiveConstraintExpr", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "ConjunctiveConstraintExpr", 1)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("or"),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("right", "ConjunctiveConstraintExpr", 1)))))));
    AddRule(new ConjunctiveConstraintExprRule("ConjunctiveConstraintExpr", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "PrimaryConstraintExpr", 1)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("and"),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("right", "PrimaryConstraintExpr", 1)))))));
    AddRule(new PrimaryConstraintExprRule("PrimaryConstraintExpr", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::CharParser('('),
                        new Cm::Parsing::NonterminalParser("ConstraintExpr", "ConstraintExpr", 1)),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser(')')))),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("AtomicConstraintExpr", "AtomicConstraintExpr", 1)))));
    AddRule(new AtomicConstraintExprRule("AtomicConstraintExpr", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("IsConstraint", "IsConstraint", 1)),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("MultiParamConstraint", "MultiParamConstraint", 1)))));
    AddRule(new IsConstraintRule("IsConstraint", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 1)),
                    new Cm::Parsing::KeywordParser("is")),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("ConceptOrTypeName", "ConceptOrTypeName", 1))))));
    AddRule(new ConceptOrTypeNameRule("ConceptOrTypeName", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 1))));
    AddRule(new MultiParamConstraintRule("MultiParamConstraint", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::NonterminalParser("QualifiedId", "QualifiedId", 0)),
                        new Cm::Parsing::CharParser('<')),
                    new Cm::Parsing::ListParser(
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 1)),
                        new Cm::Parsing::CharParser(','))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser('>'))))));
    SetSkipRuleName("spaces_and_comments");
}

} } // namespace Cm.Parser
