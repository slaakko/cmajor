#include "Class.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Parser/Specifier.hpp>
#include <Cm.Parser/Identifier.hpp>
#include <Cm.Parser/Template.hpp>
#include <Cm.Parser/Concept.hpp>
#include <Cm.Parser/Statement.hpp>
#include <Cm.Parser/Parameter.hpp>
#include <Cm.Parser/Enumeration.hpp>
#include <Cm.Parser/Constant.hpp>
#include <Cm.Parser/Delegate.hpp>
#include <Cm.Parser/Typedef.hpp>
#include <Cm.Parser/Expression.hpp>
#include <Cm.Parser/TypeExpr.hpp>
#include <Cm.Parser/Function.hpp>

namespace Cm { namespace Parser {

using namespace Cm::Ast;
using namespace Cm::Parsing;

ClassGrammar* ClassGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

ClassGrammar* ClassGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    ClassGrammar* grammar(new ClassGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

ClassGrammar::ClassGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("ClassGrammar", parsingDomain_->GetNamespaceScope("Cm.Parser"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Ast::ClassNode* ClassGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx, Cm::Ast::CompileUnitNode* compileUnit)
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
    stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<Cm::Ast::CompileUnitNode*>(compileUnit)));
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
    Cm::Ast::ClassNode* result = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ClassNode*>*>(value.get());
    stack.pop();
    return result;
}

class ClassGrammar::ClassRule : public Cm::Parsing::Rule
{
public:
    ClassRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::CompileUnitNode*", "compileUnit"));
        SetValueTypeName("Cm::Ast::ClassNode*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> compileUnit_value = std::move(stack.top());
        context.compileUnit = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>*>(compileUnit_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ClassNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassRule>(this, &ClassRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassRule>(this, &ClassRule::A1Action));
        Cm::Parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal("Specifiers");
        specifiersNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassRule>(this, &ClassRule::PostSpecifiers));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassRule>(this, &ClassRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* templateParameterListNonterminalParser = GetNonterminal("TemplateParameterList");
        templateParameterListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassRule>(this, &ClassRule::PreTemplateParameterList));
        Cm::Parsing::NonterminalParser* inheritanceAndImplIntfsNonterminalParser = GetNonterminal("InheritanceAndImplIntfs");
        inheritanceAndImplIntfsNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassRule>(this, &ClassRule::PreInheritanceAndImplIntfs));
        Cm::Parsing::NonterminalParser* whereConstraintNonterminalParser = GetNonterminal("WhereConstraint");
        whereConstraintNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassRule>(this, &ClassRule::PreWhereConstraint));
        whereConstraintNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassRule>(this, &ClassRule::PostWhereConstraint));
        Cm::Parsing::NonterminalParser* classContentNonterminalParser = GetNonterminal("ClassContent");
        classContentNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassRule>(this, &ClassRule::PreClassContent));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ClassNode(span, context.fromSpecifiers, context.fromIdentifier);
        context.value->SetCompileUnit(context.compileUnit);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->SetConstraint(context.fromWhereConstraint);
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
    void PostIdentifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIdentifier_value = std::move(stack.top());
            context.fromIdentifier = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
    void PreTemplateParameterList(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Node*>(context.value)));
    }
    void PreInheritanceAndImplIntfs(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ClassNode*>(context.value)));
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
    void PreClassContent(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ClassNode*>(context.value)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>(context.compileUnit)));
    }
private:
    struct Context
    {
        Context(): ctx(), compileUnit(), value(), fromSpecifiers(), fromIdentifier(), fromWhereConstraint() {}
        ParsingContext* ctx;
        Cm::Ast::CompileUnitNode* compileUnit;
        Cm::Ast::ClassNode* value;
        Cm::Ast::Specifiers fromSpecifiers;
        Cm::Ast::IdentifierNode* fromIdentifier;
        Cm::Ast::WhereConstraintNode* fromWhereConstraint;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ClassGrammar::InheritanceAndImplIntfsRule : public Cm::Parsing::Rule
{
public:
    InheritanceAndImplIntfsRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::ClassNode*", "cls"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> cls_value = std::move(stack.top());
        context.cls = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ClassNode*>*>(cls_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InheritanceAndImplIntfsRule>(this, &InheritanceAndImplIntfsRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InheritanceAndImplIntfsRule>(this, &InheritanceAndImplIntfsRule::A1Action));
        a1ActionParser->SetFailureAction(new Cm::Parsing::MemberFailureAction<InheritanceAndImplIntfsRule>(this, &InheritanceAndImplIntfsRule::A1ActionFail));
        Cm::Parsing::NonterminalParser* baseClassTypeExprOrImplIntfNonterminalParser = GetNonterminal("BaseClassTypeExprOrImplIntf");
        baseClassTypeExprOrImplIntfNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<InheritanceAndImplIntfsRule>(this, &InheritanceAndImplIntfsRule::PreBaseClassTypeExprOrImplIntf));
        baseClassTypeExprOrImplIntfNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InheritanceAndImplIntfsRule>(this, &InheritanceAndImplIntfsRule::PostBaseClassTypeExprOrImplIntf));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ctx->BeginParsingTypeExpr();
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.cls->AddBaseClassOrImplIntfTypeExpr(context.fromBaseClassTypeExprOrImplIntf);
        context.ctx->EndParsingTypeExpr();
    }
    void A1ActionFail()
    {
        context.ctx->EndParsingTypeExpr();
    }
    void PreBaseClassTypeExprOrImplIntf(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostBaseClassTypeExprOrImplIntf(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromBaseClassTypeExprOrImplIntf_value = std::move(stack.top());
            context.fromBaseClassTypeExprOrImplIntf = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromBaseClassTypeExprOrImplIntf_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), cls(), fromBaseClassTypeExprOrImplIntf() {}
        ParsingContext* ctx;
        Cm::Ast::ClassNode* cls;
        Cm::Ast::Node* fromBaseClassTypeExprOrImplIntf;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ClassGrammar::BaseClassTypeExprOrImplIntfRule : public Cm::Parsing::Rule
{
public:
    BaseClassTypeExprOrImplIntfRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<BaseClassTypeExprOrImplIntfRule>(this, &BaseClassTypeExprOrImplIntfRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<BaseClassTypeExprOrImplIntfRule>(this, &BaseClassTypeExprOrImplIntfRule::A1Action));
        Cm::Parsing::NonterminalParser* templateIdNonterminalParser = GetNonterminal("TemplateId");
        templateIdNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<BaseClassTypeExprOrImplIntfRule>(this, &BaseClassTypeExprOrImplIntfRule::PreTemplateId));
        templateIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<BaseClassTypeExprOrImplIntfRule>(this, &BaseClassTypeExprOrImplIntfRule::PostTemplateId));
        Cm::Parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal("QualifiedId");
        qualifiedIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<BaseClassTypeExprOrImplIntfRule>(this, &BaseClassTypeExprOrImplIntfRule::PostQualifiedId));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromTemplateId;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromQualifiedId;
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
    void PostQualifiedId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromQualifiedId_value = std::move(stack.top());
            context.fromQualifiedId = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromQualifiedId_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromTemplateId(), fromQualifiedId() {}
        ParsingContext* ctx;
        Cm::Ast::Node* value;
        Cm::Ast::Node* fromTemplateId;
        Cm::Ast::IdentifierNode* fromQualifiedId;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ClassGrammar::ClassContentRule : public Cm::Parsing::Rule
{
public:
    ClassContentRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::ClassNode*", "cls"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::CompileUnitNode*", "compileUnit"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> compileUnit_value = std::move(stack.top());
        context.compileUnit = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>*>(compileUnit_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> cls_value = std::move(stack.top());
        context.cls = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ClassNode*>*>(cls_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassContentRule>(this, &ClassContentRule::A0Action));
        Cm::Parsing::NonterminalParser* classMemberNonterminalParser = GetNonterminal("ClassMember");
        classMemberNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassContentRule>(this, &ClassContentRule::PreClassMember));
        classMemberNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassContentRule>(this, &ClassContentRule::PostClassMember));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.cls->AddMember(context.fromClassMember);
    }
    void PreClassMember(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ClassNode*>(context.cls)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>(context.compileUnit)));
    }
    void PostClassMember(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromClassMember_value = std::move(stack.top());
            context.fromClassMember = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromClassMember_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), cls(), compileUnit(), fromClassMember() {}
        ParsingContext* ctx;
        Cm::Ast::ClassNode* cls;
        Cm::Ast::CompileUnitNode* compileUnit;
        Cm::Ast::Node* fromClassMember;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ClassGrammar::ClassMemberRule : public Cm::Parsing::Rule
{
public:
    ClassMemberRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::ClassNode*", "cls"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::CompileUnitNode*", "compileUnit"));
        SetValueTypeName("Cm::Ast::Node*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> compileUnit_value = std::move(stack.top());
        context.compileUnit = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>*>(compileUnit_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> cls_value = std::move(stack.top());
        context.cls = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ClassNode*>*>(cls_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A5Action));
        Cm::Parsing::ActionParser* a6ActionParser = GetAction("A6");
        a6ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A6Action));
        Cm::Parsing::ActionParser* a7ActionParser = GetAction("A7");
        a7ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A7Action));
        Cm::Parsing::ActionParser* a8ActionParser = GetAction("A8");
        a8ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A8Action));
        Cm::Parsing::ActionParser* a9ActionParser = GetAction("A9");
        a9ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A9Action));
        Cm::Parsing::ActionParser* a10ActionParser = GetAction("A10");
        a10ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A10Action));
        Cm::Parsing::ActionParser* a11ActionParser = GetAction("A11");
        a11ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A11Action));
        Cm::Parsing::NonterminalParser* staticConstructorNonterminalParser = GetNonterminal("StaticConstructor");
        staticConstructorNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreStaticConstructor));
        staticConstructorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostStaticConstructor));
        Cm::Parsing::NonterminalParser* constructorNonterminalParser = GetNonterminal("Constructor");
        constructorNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreConstructor));
        constructorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostConstructor));
        Cm::Parsing::NonterminalParser* destructorNonterminalParser = GetNonterminal("Destructor");
        destructorNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreDestructor));
        destructorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostDestructor));
        Cm::Parsing::NonterminalParser* memberFunctionNonterminalParser = GetNonterminal("MemberFunction");
        memberFunctionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreMemberFunction));
        memberFunctionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostMemberFunction));
        Cm::Parsing::NonterminalParser* conversionFunctionNonterminalParser = GetNonterminal("ConversionFunction");
        conversionFunctionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreConversionFunction));
        conversionFunctionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostConversionFunction));
        Cm::Parsing::NonterminalParser* enumTypeNonterminalParser = GetNonterminal("EnumType");
        enumTypeNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreEnumType));
        enumTypeNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostEnumType));
        Cm::Parsing::NonterminalParser* constantNonterminalParser = GetNonterminal("Constant");
        constantNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreConstant));
        constantNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostConstant));
        Cm::Parsing::NonterminalParser* memberVariableNonterminalParser = GetNonterminal("MemberVariable");
        memberVariableNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreMemberVariable));
        memberVariableNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostMemberVariable));
        Cm::Parsing::NonterminalParser* classNonterminalParser = GetNonterminal("Class");
        classNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreClass));
        classNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostClass));
        Cm::Parsing::NonterminalParser* delegateNonterminalParser = GetNonterminal("Delegate");
        delegateNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreDelegate));
        delegateNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostDelegate));
        Cm::Parsing::NonterminalParser* classDelegateNonterminalParser = GetNonterminal("ClassDelegate");
        classDelegateNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreClassDelegate));
        classDelegateNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostClassDelegate));
        Cm::Parsing::NonterminalParser* typedefNonterminalParser = GetNonterminal("Typedef");
        typedefNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreTypedef));
        typedefNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostTypedef));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromStaticConstructor;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromConstructor;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromDestructor;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromMemberFunction;
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromConversionFunction;
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromEnumType;
    }
    void A6Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromConstant;
    }
    void A7Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromMemberVariable;
    }
    void A8Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromClass;
    }
    void A9Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromDelegate;
    }
    void A10Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromClassDelegate;
    }
    void A11Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromTypedef;
    }
    void PreStaticConstructor(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ClassNode*>(context.cls)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>(context.compileUnit)));
    }
    void PostStaticConstructor(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStaticConstructor_value = std::move(stack.top());
            context.fromStaticConstructor = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StaticConstructorNode*>*>(fromStaticConstructor_value.get());
            stack.pop();
        }
    }
    void PreConstructor(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ClassNode*>(context.cls)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>(context.compileUnit)));
    }
    void PostConstructor(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConstructor_value = std::move(stack.top());
            context.fromConstructor = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConstructorNode*>*>(fromConstructor_value.get());
            stack.pop();
        }
    }
    void PreDestructor(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ClassNode*>(context.cls)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>(context.compileUnit)));
    }
    void PostDestructor(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDestructor_value = std::move(stack.top());
            context.fromDestructor = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::DestructorNode*>*>(fromDestructor_value.get());
            stack.pop();
        }
    }
    void PreMemberFunction(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>(context.compileUnit)));
    }
    void PostMemberFunction(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromMemberFunction_value = std::move(stack.top());
            context.fromMemberFunction = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::MemberFunctionNode*>*>(fromMemberFunction_value.get());
            stack.pop();
        }
    }
    void PreConversionFunction(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>(context.compileUnit)));
    }
    void PostConversionFunction(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConversionFunction_value = std::move(stack.top());
            context.fromConversionFunction = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConversionFunctionNode*>*>(fromConversionFunction_value.get());
            stack.pop();
        }
    }
    void PreEnumType(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostEnumType(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromEnumType_value = std::move(stack.top());
            context.fromEnumType = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::EnumTypeNode*>*>(fromEnumType_value.get());
            stack.pop();
        }
    }
    void PreConstant(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostConstant(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConstant_value = std::move(stack.top());
            context.fromConstant = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromConstant_value.get());
            stack.pop();
        }
    }
    void PreMemberVariable(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostMemberVariable(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromMemberVariable_value = std::move(stack.top());
            context.fromMemberVariable = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::MemberVariableNode*>*>(fromMemberVariable_value.get());
            stack.pop();
        }
    }
    void PreClass(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>(context.compileUnit)));
    }
    void PostClass(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromClass_value = std::move(stack.top());
            context.fromClass = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ClassNode*>*>(fromClass_value.get());
            stack.pop();
        }
    }
    void PreDelegate(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostDelegate(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDelegate_value = std::move(stack.top());
            context.fromDelegate = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromDelegate_value.get());
            stack.pop();
        }
    }
    void PreClassDelegate(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostClassDelegate(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromClassDelegate_value = std::move(stack.top());
            context.fromClassDelegate = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromClassDelegate_value.get());
            stack.pop();
        }
    }
    void PreTypedef(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostTypedef(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypedef_value = std::move(stack.top());
            context.fromTypedef = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromTypedef_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), cls(), compileUnit(), value(), fromStaticConstructor(), fromConstructor(), fromDestructor(), fromMemberFunction(), fromConversionFunction(), fromEnumType(), fromConstant(), fromMemberVariable(), fromClass(), fromDelegate(), fromClassDelegate(), fromTypedef() {}
        ParsingContext* ctx;
        Cm::Ast::ClassNode* cls;
        Cm::Ast::CompileUnitNode* compileUnit;
        Cm::Ast::Node* value;
        Cm::Ast::StaticConstructorNode* fromStaticConstructor;
        Cm::Ast::ConstructorNode* fromConstructor;
        Cm::Ast::DestructorNode* fromDestructor;
        Cm::Ast::MemberFunctionNode* fromMemberFunction;
        Cm::Ast::ConversionFunctionNode* fromConversionFunction;
        Cm::Ast::EnumTypeNode* fromEnumType;
        Cm::Ast::Node* fromConstant;
        Cm::Ast::MemberVariableNode* fromMemberVariable;
        Cm::Ast::ClassNode* fromClass;
        Cm::Ast::Node* fromDelegate;
        Cm::Ast::Node* fromClassDelegate;
        Cm::Ast::Node* fromTypedef;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ClassGrammar::InitializerListRule : public Cm::Parsing::Rule
{
public:
    InitializerListRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InitializerListRule>(this, &InitializerListRule::A0Action));
        Cm::Parsing::NonterminalParser* initializerNonterminalParser = GetNonterminal("Initializer");
        initializerNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<InitializerListRule>(this, &InitializerListRule::PreInitializer));
        initializerNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InitializerListRule>(this, &InitializerListRule::PostInitializer));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.owner->AddInitializer(context.fromInitializer);
    }
    void PreInitializer(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostInitializer(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromInitializer_value = std::move(stack.top());
            context.fromInitializer = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::InitializerNode*>*>(fromInitializer_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), owner(), fromInitializer() {}
        ParsingContext* ctx;
        Node* owner;
        Cm::Ast::InitializerNode* fromInitializer;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ClassGrammar::InitializerRule : public Cm::Parsing::Rule
{
public:
    InitializerRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::InitializerNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::InitializerNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InitializerRule>(this, &InitializerRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InitializerRule>(this, &InitializerRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InitializerRule>(this, &InitializerRule::A2Action));
        Cm::Parsing::NonterminalParser* baseInitializerNonterminalParser = GetNonterminal("BaseInitializer");
        baseInitializerNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<InitializerRule>(this, &InitializerRule::PreBaseInitializer));
        baseInitializerNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InitializerRule>(this, &InitializerRule::PostBaseInitializer));
        Cm::Parsing::NonterminalParser* thisInitializerNonterminalParser = GetNonterminal("ThisInitializer");
        thisInitializerNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<InitializerRule>(this, &InitializerRule::PreThisInitializer));
        thisInitializerNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InitializerRule>(this, &InitializerRule::PostThisInitializer));
        Cm::Parsing::NonterminalParser* memberInitializerNonterminalParser = GetNonterminal("MemberInitializer");
        memberInitializerNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<InitializerRule>(this, &InitializerRule::PreMemberInitializer));
        memberInitializerNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InitializerRule>(this, &InitializerRule::PostMemberInitializer));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromBaseInitializer;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromThisInitializer;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromMemberInitializer;
    }
    void PreBaseInitializer(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostBaseInitializer(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromBaseInitializer_value = std::move(stack.top());
            context.fromBaseInitializer = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::BaseInitializerNode*>*>(fromBaseInitializer_value.get());
            stack.pop();
        }
    }
    void PreThisInitializer(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostThisInitializer(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromThisInitializer_value = std::move(stack.top());
            context.fromThisInitializer = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ThisInitializerNode*>*>(fromThisInitializer_value.get());
            stack.pop();
        }
    }
    void PreMemberInitializer(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostMemberInitializer(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromMemberInitializer_value = std::move(stack.top());
            context.fromMemberInitializer = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::MemberInitializerNode*>*>(fromMemberInitializer_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromBaseInitializer(), fromThisInitializer(), fromMemberInitializer() {}
        ParsingContext* ctx;
        Cm::Ast::InitializerNode* value;
        Cm::Ast::BaseInitializerNode* fromBaseInitializer;
        Cm::Ast::ThisInitializerNode* fromThisInitializer;
        Cm::Ast::MemberInitializerNode* fromMemberInitializer;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ClassGrammar::MemberInitializerRule : public Cm::Parsing::Rule
{
public:
    MemberInitializerRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::MemberInitializerNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::MemberInitializerNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MemberInitializerRule>(this, &MemberInitializerRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MemberInitializerRule>(this, &MemberInitializerRule::A1Action));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MemberInitializerRule>(this, &MemberInitializerRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* argumentListNonterminalParser = GetNonterminal("ArgumentList");
        argumentListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<MemberInitializerRule>(this, &MemberInitializerRule::PreArgumentList));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->GetSpan().SetEnd(span.End());
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new MemberInitializerNode(span, context.fromIdentifier);
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
    void PreArgumentList(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromIdentifier() {}
        ParsingContext* ctx;
        Cm::Ast::MemberInitializerNode* value;
        Cm::Ast::IdentifierNode* fromIdentifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ClassGrammar::BaseInitializerRule : public Cm::Parsing::Rule
{
public:
    BaseInitializerRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::BaseInitializerNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::BaseInitializerNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<BaseInitializerRule>(this, &BaseInitializerRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<BaseInitializerRule>(this, &BaseInitializerRule::A1Action));
        Cm::Parsing::NonterminalParser* argumentListNonterminalParser = GetNonterminal("ArgumentList");
        argumentListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<BaseInitializerRule>(this, &BaseInitializerRule::PreArgumentList));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->GetSpan().SetEnd(span.End());
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new BaseInitializerNode(span);
    }
    void PreArgumentList(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
    }
private:
    struct Context
    {
        Context(): ctx(), value() {}
        ParsingContext* ctx;
        Cm::Ast::BaseInitializerNode* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ClassGrammar::ThisInitializerRule : public Cm::Parsing::Rule
{
public:
    ThisInitializerRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::ThisInitializerNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ThisInitializerNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ThisInitializerRule>(this, &ThisInitializerRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ThisInitializerRule>(this, &ThisInitializerRule::A1Action));
        Cm::Parsing::NonterminalParser* argumentListNonterminalParser = GetNonterminal("ArgumentList");
        argumentListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ThisInitializerRule>(this, &ThisInitializerRule::PreArgumentList));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->GetSpan().SetEnd(span.End());
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ThisInitializerNode(span);
    }
    void PreArgumentList(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
    }
private:
    struct Context
    {
        Context(): ctx(), value() {}
        ParsingContext* ctx;
        Cm::Ast::ThisInitializerNode* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ClassGrammar::StaticConstructorRule : public Cm::Parsing::Rule
{
public:
    StaticConstructorRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::ClassNode*", "cls"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::CompileUnitNode*", "compileUnit"));
        SetValueTypeName("Cm::Ast::StaticConstructorNode*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<IdentifierNode>", "id"));
        AddLocalVariable(AttrOrVariable("std::unique_ptr<IdentifierNode>", "refId"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> compileUnit_value = std::move(stack.top());
        context.compileUnit = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>*>(compileUnit_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> cls_value = std::move(stack.top());
        context.cls = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ClassNode*>*>(cls_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StaticConstructorNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StaticConstructorRule>(this, &StaticConstructorRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StaticConstructorRule>(this, &StaticConstructorRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StaticConstructorRule>(this, &StaticConstructorRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StaticConstructorRule>(this, &StaticConstructorRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StaticConstructorRule>(this, &StaticConstructorRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StaticConstructorRule>(this, &StaticConstructorRule::A5Action));
        Cm::Parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal("Specifiers");
        specifiersNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StaticConstructorRule>(this, &StaticConstructorRule::PostSpecifiers));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StaticConstructorRule>(this, &StaticConstructorRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* refNonterminalParser = GetNonterminal("ref");
        refNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StaticConstructorRule>(this, &StaticConstructorRule::Postref));
        Cm::Parsing::NonterminalParser* initializerListNonterminalParser = GetNonterminal("InitializerList");
        initializerListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<StaticConstructorRule>(this, &StaticConstructorRule::PreInitializerList));
        Cm::Parsing::NonterminalParser* whereConstraintNonterminalParser = GetNonterminal("WhereConstraint");
        whereConstraintNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<StaticConstructorRule>(this, &StaticConstructorRule::PreWhereConstraint));
        whereConstraintNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StaticConstructorRule>(this, &StaticConstructorRule::PostWhereConstraint));
        Cm::Parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal("CompoundStatement");
        compoundStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<StaticConstructorRule>(this, &StaticConstructorRule::PreCompoundStatement));
        compoundStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StaticConstructorRule>(this, &StaticConstructorRule::PostCompoundStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->GetSpan().SetEnd(span.End());
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        pass = context.id->Str() == context.cls->Id()->Str() && HasStaticSpecifier(context.fromSpecifiers);
        if (pass)
        {
            context.value = new StaticConstructorNode(span, context.fromSpecifiers);
            context.value->SetCompileUnit(context.compileUnit);
        }
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.id.reset(context.fromIdentifier);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.refId.reset(context.fromref);
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->SetConstraint(context.fromWhereConstraint);
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->SetBody(context.fromCompoundStatement);
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
    void PostIdentifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIdentifier_value = std::move(stack.top());
            context.fromIdentifier = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
    void Postref(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromref_value = std::move(stack.top());
            context.fromref = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromref_value.get());
            stack.pop();
        }
    }
    void PreInitializerList(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Node*>(context.value)));
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
    void PreCompoundStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostCompoundStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context.fromCompoundStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompoundStatementNode*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), cls(), compileUnit(), value(), id(), refId(), fromSpecifiers(), fromIdentifier(), fromref(), fromWhereConstraint(), fromCompoundStatement() {}
        ParsingContext* ctx;
        Cm::Ast::ClassNode* cls;
        Cm::Ast::CompileUnitNode* compileUnit;
        Cm::Ast::StaticConstructorNode* value;
        std::unique_ptr<IdentifierNode> id;
        std::unique_ptr<IdentifierNode> refId;
        Cm::Ast::Specifiers fromSpecifiers;
        Cm::Ast::IdentifierNode* fromIdentifier;
        Cm::Ast::IdentifierNode* fromref;
        Cm::Ast::WhereConstraintNode* fromWhereConstraint;
        Cm::Ast::CompoundStatementNode* fromCompoundStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ClassGrammar::ConstructorRule : public Cm::Parsing::Rule
{
public:
    ConstructorRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::ClassNode*", "cls"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::CompileUnitNode*", "compileUnit"));
        SetValueTypeName("Cm::Ast::ConstructorNode*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<IdentifierNode>", "id"));
        AddLocalVariable(AttrOrVariable("std::unique_ptr<IdentifierNode>", "refId"));
        AddLocalVariable(AttrOrVariable("std::unique_ptr<ConstructorNode>", "ctor"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> compileUnit_value = std::move(stack.top());
        context.compileUnit = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>*>(compileUnit_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> cls_value = std::move(stack.top());
        context.cls = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ClassNode*>*>(cls_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConstructorNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConstructorRule>(this, &ConstructorRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConstructorRule>(this, &ConstructorRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConstructorRule>(this, &ConstructorRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConstructorRule>(this, &ConstructorRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConstructorRule>(this, &ConstructorRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConstructorRule>(this, &ConstructorRule::A5Action));
        Cm::Parsing::ActionParser* a6ActionParser = GetAction("A6");
        a6ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConstructorRule>(this, &ConstructorRule::A6Action));
        Cm::Parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal("Specifiers");
        specifiersNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConstructorRule>(this, &ConstructorRule::PostSpecifiers));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConstructorRule>(this, &ConstructorRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* refNonterminalParser = GetNonterminal("ref");
        refNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConstructorRule>(this, &ConstructorRule::Postref));
        Cm::Parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal("ParameterList");
        parameterListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConstructorRule>(this, &ConstructorRule::PreParameterList));
        Cm::Parsing::NonterminalParser* initializerListNonterminalParser = GetNonterminal("InitializerList");
        initializerListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConstructorRule>(this, &ConstructorRule::PreInitializerList));
        Cm::Parsing::NonterminalParser* whereConstraintNonterminalParser = GetNonterminal("WhereConstraint");
        whereConstraintNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConstructorRule>(this, &ConstructorRule::PreWhereConstraint));
        whereConstraintNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConstructorRule>(this, &ConstructorRule::PostWhereConstraint));
        Cm::Parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal("CompoundStatement");
        compoundStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConstructorRule>(this, &ConstructorRule::PreCompoundStatement));
        compoundStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConstructorRule>(this, &ConstructorRule::PostCompoundStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.ctor.release();
        context.value->SetCompileUnit(context.compileUnit);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ctor->GetSpan().SetEnd(span.End());
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        pass = context.id->Str() == context.cls->Id()->Str();
        if (pass) context.ctor.reset(new ConstructorNode(span, context.fromSpecifiers));
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.id.reset(context.fromIdentifier);
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.refId.reset(context.fromref);
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ctor->SetConstraint(context.fromWhereConstraint);
    }
    void A6Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ctor->SetBody(context.fromCompoundStatement);
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
    void PostIdentifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIdentifier_value = std::move(stack.top());
            context.fromIdentifier = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
    void Postref(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromref_value = std::move(stack.top());
            context.fromref = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromref_value.get());
            stack.pop();
        }
    }
    void PreParameterList(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.ctor.get())));
    }
    void PreInitializerList(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Node*>(context.ctor.get())));
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
    void PreCompoundStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostCompoundStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context.fromCompoundStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompoundStatementNode*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), cls(), compileUnit(), value(), id(), refId(), ctor(), fromSpecifiers(), fromIdentifier(), fromref(), fromWhereConstraint(), fromCompoundStatement() {}
        ParsingContext* ctx;
        Cm::Ast::ClassNode* cls;
        Cm::Ast::CompileUnitNode* compileUnit;
        Cm::Ast::ConstructorNode* value;
        std::unique_ptr<IdentifierNode> id;
        std::unique_ptr<IdentifierNode> refId;
        std::unique_ptr<ConstructorNode> ctor;
        Cm::Ast::Specifiers fromSpecifiers;
        Cm::Ast::IdentifierNode* fromIdentifier;
        Cm::Ast::IdentifierNode* fromref;
        Cm::Ast::WhereConstraintNode* fromWhereConstraint;
        Cm::Ast::CompoundStatementNode* fromCompoundStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ClassGrammar::DestructorRule : public Cm::Parsing::Rule
{
public:
    DestructorRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::ClassNode*", "cls"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::CompileUnitNode*", "compileUnit"));
        SetValueTypeName("Cm::Ast::DestructorNode*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<IdentifierNode>", "id"));
        AddLocalVariable(AttrOrVariable("Span", "s"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> compileUnit_value = std::move(stack.top());
        context.compileUnit = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>*>(compileUnit_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> cls_value = std::move(stack.top());
        context.cls = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ClassNode*>*>(cls_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::DestructorNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DestructorRule>(this, &DestructorRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DestructorRule>(this, &DestructorRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DestructorRule>(this, &DestructorRule::A2Action));
        Cm::Parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal("Specifiers");
        specifiersNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DestructorRule>(this, &DestructorRule::PostSpecifiers));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DestructorRule>(this, &DestructorRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal("CompoundStatement");
        compoundStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DestructorRule>(this, &DestructorRule::PreCompoundStatement));
        compoundStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DestructorRule>(this, &DestructorRule::PostCompoundStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.s = span;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.id.reset(context.fromIdentifier);
        pass = context.id->Str() == context.cls->Id()->Str();
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new DestructorNode(context.s, context.fromSpecifiers, context.fromCompoundStatement);
        context.value->SetCompileUnit(context.compileUnit);
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
    void PostIdentifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIdentifier_value = std::move(stack.top());
            context.fromIdentifier = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
    void PreCompoundStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostCompoundStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context.fromCompoundStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompoundStatementNode*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), cls(), compileUnit(), value(), id(), s(), fromSpecifiers(), fromIdentifier(), fromCompoundStatement() {}
        ParsingContext* ctx;
        Cm::Ast::ClassNode* cls;
        Cm::Ast::CompileUnitNode* compileUnit;
        Cm::Ast::DestructorNode* value;
        std::unique_ptr<IdentifierNode> id;
        Span s;
        Cm::Ast::Specifiers fromSpecifiers;
        Cm::Ast::IdentifierNode* fromIdentifier;
        Cm::Ast::CompoundStatementNode* fromCompoundStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ClassGrammar::MemberFunctionRule : public Cm::Parsing::Rule
{
public:
    MemberFunctionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::CompileUnitNode*", "compileUnit"));
        SetValueTypeName("Cm::Ast::MemberFunctionNode*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<MemberFunctionNode>", "memFun"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> compileUnit_value = std::move(stack.top());
        context.compileUnit = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>*>(compileUnit_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::MemberFunctionNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MemberFunctionRule>(this, &MemberFunctionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MemberFunctionRule>(this, &MemberFunctionRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MemberFunctionRule>(this, &MemberFunctionRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MemberFunctionRule>(this, &MemberFunctionRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MemberFunctionRule>(this, &MemberFunctionRule::A4Action));
        Cm::Parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal("Specifiers");
        specifiersNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MemberFunctionRule>(this, &MemberFunctionRule::PostSpecifiers));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<MemberFunctionRule>(this, &MemberFunctionRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MemberFunctionRule>(this, &MemberFunctionRule::PostTypeExpr));
        Cm::Parsing::NonterminalParser* functionGroupIdNonterminalParser = GetNonterminal("FunctionGroupId");
        functionGroupIdNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<MemberFunctionRule>(this, &MemberFunctionRule::PreFunctionGroupId));
        functionGroupIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MemberFunctionRule>(this, &MemberFunctionRule::PostFunctionGroupId));
        Cm::Parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal("ParameterList");
        parameterListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<MemberFunctionRule>(this, &MemberFunctionRule::PreParameterList));
        Cm::Parsing::NonterminalParser* whereConstraintNonterminalParser = GetNonterminal("WhereConstraint");
        whereConstraintNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<MemberFunctionRule>(this, &MemberFunctionRule::PreWhereConstraint));
        whereConstraintNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MemberFunctionRule>(this, &MemberFunctionRule::PostWhereConstraint));
        Cm::Parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal("CompoundStatement");
        compoundStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<MemberFunctionRule>(this, &MemberFunctionRule::PreCompoundStatement));
        compoundStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MemberFunctionRule>(this, &MemberFunctionRule::PostCompoundStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.memFun->GetSpan().SetEnd(span.End());
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.memFun.reset(new MemberFunctionNode(span, context.fromSpecifiers, context.fromTypeExpr, context.fromFunctionGroupId));
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.memFun->SetConst();
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.memFun->SetConstraint(context.fromWhereConstraint);
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.memFun->SetBody(context.fromCompoundStatement);
        context.value = context.memFun.release();
        context.value->SetCompileUnit(context.compileUnit);
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
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.memFun.get())));
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
    void PreCompoundStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostCompoundStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context.fromCompoundStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompoundStatementNode*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), compileUnit(), value(), memFun(), fromSpecifiers(), fromTypeExpr(), fromFunctionGroupId(), fromWhereConstraint(), fromCompoundStatement() {}
        ParsingContext* ctx;
        Cm::Ast::CompileUnitNode* compileUnit;
        Cm::Ast::MemberFunctionNode* value;
        std::unique_ptr<MemberFunctionNode> memFun;
        Cm::Ast::Specifiers fromSpecifiers;
        Cm::Ast::Node* fromTypeExpr;
        Cm::Ast::FunctionGroupIdNode* fromFunctionGroupId;
        Cm::Ast::WhereConstraintNode* fromWhereConstraint;
        Cm::Ast::CompoundStatementNode* fromCompoundStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ClassGrammar::ConversionFunctionRule : public Cm::Parsing::Rule
{
public:
    ConversionFunctionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::CompileUnitNode*", "compileUnit"));
        SetValueTypeName("Cm::Ast::ConversionFunctionNode*");
        AddLocalVariable(AttrOrVariable("bool", "setConst"));
        AddLocalVariable(AttrOrVariable("Span", "s"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> compileUnit_value = std::move(stack.top());
        context.compileUnit = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>*>(compileUnit_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ConversionFunctionNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConversionFunctionRule>(this, &ConversionFunctionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConversionFunctionRule>(this, &ConversionFunctionRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConversionFunctionRule>(this, &ConversionFunctionRule::A2Action));
        Cm::Parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal("Specifiers");
        specifiersNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConversionFunctionRule>(this, &ConversionFunctionRule::PostSpecifiers));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConversionFunctionRule>(this, &ConversionFunctionRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConversionFunctionRule>(this, &ConversionFunctionRule::PostTypeExpr));
        Cm::Parsing::NonterminalParser* whereConstraintNonterminalParser = GetNonterminal("WhereConstraint");
        whereConstraintNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConversionFunctionRule>(this, &ConversionFunctionRule::PreWhereConstraint));
        whereConstraintNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConversionFunctionRule>(this, &ConversionFunctionRule::PostWhereConstraint));
        Cm::Parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal("CompoundStatement");
        compoundStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConversionFunctionRule>(this, &ConversionFunctionRule::PreCompoundStatement));
        compoundStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConversionFunctionRule>(this, &ConversionFunctionRule::PostCompoundStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.s = span;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.setConst = true;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ConversionFunctionNode(context.s, context.fromSpecifiers, context.fromTypeExpr, context.setConst, context.fromWhereConstraint, context.fromCompoundStatement);
        context.value->SetCompileUnit(context.compileUnit);
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
    void PreCompoundStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostCompoundStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context.fromCompoundStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompoundStatementNode*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), compileUnit(), value(), setConst(), s(), fromSpecifiers(), fromTypeExpr(), fromWhereConstraint(), fromCompoundStatement() {}
        ParsingContext* ctx;
        Cm::Ast::CompileUnitNode* compileUnit;
        Cm::Ast::ConversionFunctionNode* value;
        bool setConst;
        Span s;
        Cm::Ast::Specifiers fromSpecifiers;
        Cm::Ast::Node* fromTypeExpr;
        Cm::Ast::WhereConstraintNode* fromWhereConstraint;
        Cm::Ast::CompoundStatementNode* fromCompoundStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ClassGrammar::MemberVariableRule : public Cm::Parsing::Rule
{
public:
    MemberVariableRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::MemberVariableNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::MemberVariableNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MemberVariableRule>(this, &MemberVariableRule::A0Action));
        Cm::Parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal("Specifiers");
        specifiersNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MemberVariableRule>(this, &MemberVariableRule::PostSpecifiers));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<MemberVariableRule>(this, &MemberVariableRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MemberVariableRule>(this, &MemberVariableRule::PostTypeExpr));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MemberVariableRule>(this, &MemberVariableRule::PostIdentifier));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new MemberVariableNode(span, context.fromSpecifiers, context.fromTypeExpr, context.fromIdentifier);
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
private:
    struct Context
    {
        Context(): ctx(), value(), fromSpecifiers(), fromTypeExpr(), fromIdentifier() {}
        ParsingContext* ctx;
        Cm::Ast::MemberVariableNode* value;
        Cm::Ast::Specifiers fromSpecifiers;
        Cm::Ast::Node* fromTypeExpr;
        Cm::Ast::IdentifierNode* fromIdentifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

void ClassGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parser.SpecifierGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parser::SpecifierGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parser.IdentifierGrammar");
    if (!grammar1)
    {
        grammar1 = Cm::Parser::IdentifierGrammar::Create(pd);
    }
    AddGrammarReference(grammar1);
    Cm::Parsing::Grammar* grammar2 = pd->GetGrammar("Cm.Parser.TemplateGrammar");
    if (!grammar2)
    {
        grammar2 = Cm::Parser::TemplateGrammar::Create(pd);
    }
    AddGrammarReference(grammar2);
    Cm::Parsing::Grammar* grammar3 = pd->GetGrammar("Cm.Parser.DelegateGrammar");
    if (!grammar3)
    {
        grammar3 = Cm::Parser::DelegateGrammar::Create(pd);
    }
    AddGrammarReference(grammar3);
    Cm::Parsing::Grammar* grammar4 = pd->GetGrammar("Cm.Parser.ParameterGrammar");
    if (!grammar4)
    {
        grammar4 = Cm::Parser::ParameterGrammar::Create(pd);
    }
    AddGrammarReference(grammar4);
    Cm::Parsing::Grammar* grammar5 = pd->GetGrammar("Cm.Parser.EnumerationGrammar");
    if (!grammar5)
    {
        grammar5 = Cm::Parser::EnumerationGrammar::Create(pd);
    }
    AddGrammarReference(grammar5);
    Cm::Parsing::Grammar* grammar6 = pd->GetGrammar("Cm.Parser.ConceptGrammar");
    if (!grammar6)
    {
        grammar6 = Cm::Parser::ConceptGrammar::Create(pd);
    }
    AddGrammarReference(grammar6);
    Cm::Parsing::Grammar* grammar7 = pd->GetGrammar("Cm.Parser.StatementGrammar");
    if (!grammar7)
    {
        grammar7 = Cm::Parser::StatementGrammar::Create(pd);
    }
    AddGrammarReference(grammar7);
    Cm::Parsing::Grammar* grammar8 = pd->GetGrammar("Cm.Parser.ConstantGrammar");
    if (!grammar8)
    {
        grammar8 = Cm::Parser::ConstantGrammar::Create(pd);
    }
    AddGrammarReference(grammar8);
    Cm::Parsing::Grammar* grammar9 = pd->GetGrammar("Cm.Parser.TypedefGrammar");
    if (!grammar9)
    {
        grammar9 = Cm::Parser::TypedefGrammar::Create(pd);
    }
    AddGrammarReference(grammar9);
    Cm::Parsing::Grammar* grammar10 = pd->GetGrammar("Cm.Parser.ExpressionGrammar");
    if (!grammar10)
    {
        grammar10 = Cm::Parser::ExpressionGrammar::Create(pd);
    }
    AddGrammarReference(grammar10);
    Cm::Parsing::Grammar* grammar11 = pd->GetGrammar("Cm.Parser.TypeExprGrammar");
    if (!grammar11)
    {
        grammar11 = Cm::Parser::TypeExprGrammar::Create(pd);
    }
    AddGrammarReference(grammar11);
    Cm::Parsing::Grammar* grammar12 = pd->GetGrammar("Cm.Parser.FunctionGrammar");
    if (!grammar12)
    {
        grammar12 = Cm::Parser::FunctionGrammar::Create(pd);
    }
    AddGrammarReference(grammar12);
    Cm::Parsing::Grammar* grammar13 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar13)
    {
        grammar13 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar13);
}

void ClassGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("Specifiers", this, "SpecifierGrammar.Specifiers"));
    AddRuleLink(new Cm::Parsing::RuleLink("Identifier", this, "IdentifierGrammar.Identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("QualifiedId", this, "IdentifierGrammar.QualifiedId"));
    AddRuleLink(new Cm::Parsing::RuleLink("TemplateParameterList", this, "TemplateGrammar.TemplateParameterList"));
    AddRuleLink(new Cm::Parsing::RuleLink("EnumType", this, "EnumerationGrammar.EnumType"));
    AddRuleLink(new Cm::Parsing::RuleLink("TemplateId", this, "TemplateGrammar.TemplateId"));
    AddRuleLink(new Cm::Parsing::RuleLink("WhereConstraint", this, "ConceptGrammar.WhereConstraint"));
    AddRuleLink(new Cm::Parsing::RuleLink("CompoundStatement", this, "StatementGrammar.CompoundStatement"));
    AddRuleLink(new Cm::Parsing::RuleLink("ParameterList", this, "ParameterGrammar.ParameterList"));
    AddRuleLink(new Cm::Parsing::RuleLink("Constant", this, "ConstantGrammar.Constant"));
    AddRuleLink(new Cm::Parsing::RuleLink("Delegate", this, "DelegateGrammar.Delegate"));
    AddRuleLink(new Cm::Parsing::RuleLink("ClassDelegate", this, "DelegateGrammar.ClassDelegate"));
    AddRuleLink(new Cm::Parsing::RuleLink("Typedef", this, "TypedefGrammar.Typedef"));
    AddRuleLink(new Cm::Parsing::RuleLink("ArgumentList", this, "ExpressionGrammar.ArgumentList"));
    AddRuleLink(new Cm::Parsing::RuleLink("TypeExpr", this, "TypeExprGrammar.TypeExpr"));
    AddRuleLink(new Cm::Parsing::RuleLink("FunctionGroupId", this, "FunctionGrammar.FunctionGroupId"));
    AddRuleLink(new Cm::Parsing::RuleLink("spaces_and_comments", this, "Cm.Parsing.stdlib.spaces_and_comments"));
    AddRule(new ClassRule("Class", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::ActionParser("A0",
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::SequenceParser(
                                            new Cm::Parsing::NonterminalParser("Specifiers", "Specifiers", 0),
                                            new Cm::Parsing::KeywordParser("class")),
                                        new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0))),
                                new Cm::Parsing::OptionalParser(
                                    new Cm::Parsing::NonterminalParser("TemplateParameterList", "TemplateParameterList", 2))),
                            new Cm::Parsing::OptionalParser(
                                new Cm::Parsing::NonterminalParser("InheritanceAndImplIntfs", "InheritanceAndImplIntfs", 2))),
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::NonterminalParser("WhereConstraint", "WhereConstraint", 1)))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser('{'))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("ClassContent", "ClassContent", 3))),
            new Cm::Parsing::ExpectationParser(
                new Cm::Parsing::CharParser('}')))));
    AddRule(new InheritanceAndImplIntfsRule("InheritanceAndImplIntfs", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::CharParser(':'),
            new Cm::Parsing::ListParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::EmptyParser()),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("BaseClassTypeExprOrImplIntf", "BaseClassTypeExprOrImplIntf", 1))),
                new Cm::Parsing::CharParser(',')))));
    AddRule(new BaseClassTypeExprOrImplIntfRule("BaseClassTypeExprOrImplIntf", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("TemplateId", "TemplateId", 1)),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("QualifiedId", "QualifiedId", 0)))));
    AddRule(new ClassContentRule("ClassContent", GetScope(),
        new Cm::Parsing::KleeneStarParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("ClassMember", "ClassMember", 3)))));
    AddRule(new ClassMemberRule("ClassMember", GetScope(),
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
                                                        new Cm::Parsing::NonterminalParser("StaticConstructor", "StaticConstructor", 3)),
                                                    new Cm::Parsing::ActionParser("A1",
                                                        new Cm::Parsing::NonterminalParser("Constructor", "Constructor", 3))),
                                                new Cm::Parsing::ActionParser("A2",
                                                    new Cm::Parsing::NonterminalParser("Destructor", "Destructor", 3))),
                                            new Cm::Parsing::ActionParser("A3",
                                                new Cm::Parsing::NonterminalParser("MemberFunction", "MemberFunction", 2))),
                                        new Cm::Parsing::ActionParser("A4",
                                            new Cm::Parsing::NonterminalParser("ConversionFunction", "ConversionFunction", 2))),
                                    new Cm::Parsing::ActionParser("A5",
                                        new Cm::Parsing::NonterminalParser("EnumType", "EnumType", 1))),
                                new Cm::Parsing::ActionParser("A6",
                                    new Cm::Parsing::NonterminalParser("Constant", "Constant", 1))),
                            new Cm::Parsing::ActionParser("A7",
                                new Cm::Parsing::NonterminalParser("MemberVariable", "MemberVariable", 1))),
                        new Cm::Parsing::ActionParser("A8",
                            new Cm::Parsing::NonterminalParser("Class", "Class", 2))),
                    new Cm::Parsing::ActionParser("A9",
                        new Cm::Parsing::NonterminalParser("Delegate", "Delegate", 1))),
                new Cm::Parsing::ActionParser("A10",
                    new Cm::Parsing::NonterminalParser("ClassDelegate", "ClassDelegate", 1))),
            new Cm::Parsing::ActionParser("A11",
                new Cm::Parsing::NonterminalParser("Typedef", "Typedef", 1)))));
    AddRule(new InitializerListRule("InitializerList", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::CharParser(':'),
            new Cm::Parsing::ListParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("Initializer", "Initializer", 1)),
                new Cm::Parsing::CharParser(',')))));
    AddRule(new InitializerRule("Initializer", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("BaseInitializer", "BaseInitializer", 1)),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::NonterminalParser("ThisInitializer", "ThisInitializer", 1))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::NonterminalParser("MemberInitializer", "MemberInitializer", 1)))));
    AddRule(new MemberInitializerRule("MemberInitializer", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser('('))),
                    new Cm::Parsing::NonterminalParser("ArgumentList", "ArgumentList", 2)),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(')'))))));
    AddRule(new BaseInitializerRule("BaseInitializer", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::KeywordParser("base")),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser('('))),
                    new Cm::Parsing::NonterminalParser("ArgumentList", "ArgumentList", 2)),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(')'))))));
    AddRule(new ThisInitializerRule("ThisInitializer", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::KeywordParser("this")),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser('('))),
                    new Cm::Parsing::NonterminalParser("ArgumentList", "ArgumentList", 2)),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(')'))))));
    AddRule(new StaticConstructorRule("StaticConstructor", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::NonterminalParser("Specifiers", "Specifiers", 0),
                                        new Cm::Parsing::DifferenceParser(
                                            new Cm::Parsing::ActionParser("A2",
                                                new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0)),
                                            new Cm::Parsing::SequenceParser(
                                                new Cm::Parsing::ActionParser("A3",
                                                    new Cm::Parsing::NonterminalParser("ref", "Identifier", 0)),
                                                new Cm::Parsing::CharParser('&')))),
                                    new Cm::Parsing::CharParser('(')),
                                new Cm::Parsing::CharParser(')'))),
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::NonterminalParser("InitializerList", "InitializerList", 2))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::ActionParser("A4",
                            new Cm::Parsing::NonterminalParser("WhereConstraint", "WhereConstraint", 1))))),
            new Cm::Parsing::ActionParser("A5",
                new Cm::Parsing::NonterminalParser("CompoundStatement", "CompoundStatement", 1)))));
    AddRule(new ConstructorRule("Constructor", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::ActionParser("A2",
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::NonterminalParser("Specifiers", "Specifiers", 0),
                                        new Cm::Parsing::DifferenceParser(
                                            new Cm::Parsing::ActionParser("A3",
                                                new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0)),
                                            new Cm::Parsing::SequenceParser(
                                                new Cm::Parsing::ActionParser("A4",
                                                    new Cm::Parsing::NonterminalParser("ref", "Identifier", 0)),
                                                new Cm::Parsing::CharParser('&'))))),
                                new Cm::Parsing::NonterminalParser("ParameterList", "ParameterList", 2)),
                            new Cm::Parsing::OptionalParser(
                                new Cm::Parsing::NonterminalParser("InitializerList", "InitializerList", 2))),
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::ActionParser("A5",
                                new Cm::Parsing::NonterminalParser("WhereConstraint", "WhereConstraint", 1))))),
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::ActionParser("A6",
                        new Cm::Parsing::NonterminalParser("CompoundStatement", "CompoundStatement", 1)),
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new DestructorRule("Destructor", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::NonterminalParser("Specifiers", "Specifiers", 0),
                                new Cm::Parsing::CharParser('~')),
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser('('))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser(')')))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::NonterminalParser("CompoundStatement", "CompoundStatement", 1),
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new MemberFunctionRule("MemberFunction", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::NonterminalParser("Specifiers", "Specifiers", 0),
                                        new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 1)),
                                    new Cm::Parsing::NonterminalParser("FunctionGroupId", "FunctionGroupId", 1))),
                            new Cm::Parsing::NonterminalParser("ParameterList", "ParameterList", 2)),
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::ActionParser("A2",
                                new Cm::Parsing::KeywordParser("const")))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::ActionParser("A3",
                            new Cm::Parsing::NonterminalParser("WhereConstraint", "WhereConstraint", 1))))),
            new Cm::Parsing::ActionParser("A4",
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::NonterminalParser("CompoundStatement", "CompoundStatement", 1),
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new ConversionFunctionRule("ConversionFunction", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::NonterminalParser("Specifiers", "Specifiers", 0),
                                        new Cm::Parsing::KeywordParser("operator")),
                                    new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 1)),
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::CharParser('('))),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::CharParser(')'))),
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::KeywordParser("const")))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("WhereConstraint", "WhereConstraint", 1)))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::NonterminalParser("CompoundStatement", "CompoundStatement", 1)))));
    AddRule(new MemberVariableRule("MemberVariable", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::NonterminalParser("Specifiers", "Specifiers", 0),
                        new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 1)),
                    new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0)),
                new Cm::Parsing::CharParser(';')))));
    SetSkipRuleName("spaces_and_comments");
}

} } // namespace Cm.Parser
