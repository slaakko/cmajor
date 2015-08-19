#include "Expression.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Parsing.Cpp/Declaration.hpp>
#include <Cm.Parsing.Cpp/Declarator.hpp>
#include <Cm.Parsing.Cpp/Literal.hpp>
#include <Cm.Parsing.Cpp/Identifier.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <Cm.Parsing.CppObjectModel/Type.hpp>

namespace Cm { namespace Parsing { namespace Cpp {

using namespace Cm::Parsing::CppObjectModel;
using Cm::Util::Trim;
using namespace Cm::Parsing;

ExpressionGrammar* ExpressionGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

ExpressionGrammar* ExpressionGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    ExpressionGrammar* grammar(new ExpressionGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

ExpressionGrammar::ExpressionGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("ExpressionGrammar", parsingDomain_->GetNamespaceScope("Cm.Parsing.Cpp"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Parsing::CppObjectModel::CppObject* ExpressionGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
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
    Cm::Parsing::CppObjectModel::CppObject* result = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(value.get());
    stack.pop();
    return result;
}

class ExpressionGrammar::ExpressionRule : public Cm::Parsing::Rule
{
public:
    ExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ExpressionRule>(this, &ExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ExpressionRule>(this, &ExpressionRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ExpressionRule>(this, &ExpressionRule::Postleft));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ExpressionRule>(this, &ExpressionRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new BinaryOpExpr(context.value, comma, 0, context.fromright);
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromleft(), fromright() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::CppObject* fromleft;
        Cm::Parsing::CppObjectModel::CppObject* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::ConstantExpressionRule : public Cm::Parsing::Rule
{
public:
    ConstantExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConstantExpressionRule>(this, &ConstantExpressionRule::A0Action));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConstantExpressionRule>(this, &ConstantExpressionRule::PostExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromExpression;
    }
    void PostExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromExpression_value = std::move(stack.top());
            context.fromExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromExpression() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::CppObject* fromExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::AssignmentExpressionRule : public Cm::Parsing::Rule
{
public:
    AssignmentExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<CppObject>", "lor"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A3Action));
        Cm::Parsing::NonterminalParser* logicalOrExpressionNonterminalParser = GetNonterminal("LogicalOrExpression");
        logicalOrExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostLogicalOrExpression));
        Cm::Parsing::NonterminalParser* assingmentOpNonterminalParser = GetNonterminal("AssingmentOp");
        assingmentOpNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostAssingmentOp));
        Cm::Parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal("AssignmentExpression");
        assignmentExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostAssignmentExpression));
        Cm::Parsing::NonterminalParser* conditionalExpressionNonterminalParser = GetNonterminal("ConditionalExpression");
        conditionalExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostConditionalExpression));
        Cm::Parsing::NonterminalParser* throwExpressionNonterminalParser = GetNonterminal("ThrowExpression");
        throwExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostThrowExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new BinaryOpExpr(context.lor.release(), context.fromAssingmentOp, 1, context.fromAssignmentExpression);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.lor.reset(context.fromLogicalOrExpression);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromConditionalExpression;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromThrowExpression;
    }
    void PostLogicalOrExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromLogicalOrExpression_value = std::move(stack.top());
            context.fromLogicalOrExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromLogicalOrExpression_value.get());
            stack.pop();
        }
    }
    void PostAssingmentOp(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAssingmentOp_value = std::move(stack.top());
            context.fromAssingmentOp = *static_cast<Cm::Parsing::ValueObject<Operator>*>(fromAssingmentOp_value.get());
            stack.pop();
        }
    }
    void PostAssignmentExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAssignmentExpression_value = std::move(stack.top());
            context.fromAssignmentExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromAssignmentExpression_value.get());
            stack.pop();
        }
    }
    void PostConditionalExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConditionalExpression_value = std::move(stack.top());
            context.fromConditionalExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromConditionalExpression_value.get());
            stack.pop();
        }
    }
    void PostThrowExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromThrowExpression_value = std::move(stack.top());
            context.fromThrowExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromThrowExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), lor(), fromLogicalOrExpression(), fromAssingmentOp(), fromAssignmentExpression(), fromConditionalExpression(), fromThrowExpression() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        std::unique_ptr<CppObject> lor;
        Cm::Parsing::CppObjectModel::CppObject* fromLogicalOrExpression;
        Operator fromAssingmentOp;
        Cm::Parsing::CppObjectModel::CppObject* fromAssignmentExpression;
        Cm::Parsing::CppObjectModel::CppObject* fromConditionalExpression;
        Cm::Parsing::CppObjectModel::CppObject* fromThrowExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::AssingmentOpRule : public Cm::Parsing::Rule
{
public:
    AssingmentOpRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Operator");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Operator>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AssingmentOpRule>(this, &AssingmentOpRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = GetOperator(std::string(matchBegin, matchEnd));
    }
private:
    struct Context
    {
        Context(): value() {}
        Operator value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::ThrowExpressionRule : public Cm::Parsing::Rule
{
public:
    ThrowExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ThrowExpressionRule>(this, &ThrowExpressionRule::A0Action));
        Cm::Parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal("AssignmentExpression");
        assignmentExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ThrowExpressionRule>(this, &ThrowExpressionRule::PostAssignmentExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ThrowExpr(context.fromAssignmentExpression);
    }
    void PostAssignmentExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAssignmentExpression_value = std::move(stack.top());
            context.fromAssignmentExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromAssignmentExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromAssignmentExpression() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::CppObject* fromAssignmentExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::ConditionalExpressionRule : public Cm::Parsing::Rule
{
public:
    ConditionalExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalExpressionRule>(this, &ConditionalExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalExpressionRule>(this, &ConditionalExpressionRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::Postleft));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::PostExpression));
        Cm::Parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal("AssignmentExpression");
        assignmentExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::PostAssignmentExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ConditionalExpr(context.value, context.fromExpression, context.fromAssignmentExpression);
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromExpression_value = std::move(stack.top());
            context.fromExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
    void PostAssignmentExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAssignmentExpression_value = std::move(stack.top());
            context.fromAssignmentExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromAssignmentExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromleft(), fromExpression(), fromAssignmentExpression() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::CppObject* fromleft;
        Cm::Parsing::CppObjectModel::CppObject* fromExpression;
        Cm::Parsing::CppObjectModel::CppObject* fromAssignmentExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::LogicalOrExpressionRule : public Cm::Parsing::Rule
{
public:
    LogicalOrExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::Postleft));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new BinaryOpExpr(context.value, or_, 2, context.fromright);
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromleft(), fromright() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::CppObject* fromleft;
        Cm::Parsing::CppObjectModel::CppObject* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::LogicalAndExpressionRule : public Cm::Parsing::Rule
{
public:
    LogicalAndExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::Postleft));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new BinaryOpExpr(context.value, and_, 3, context.fromright);
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromleft(), fromright() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::CppObject* fromleft;
        Cm::Parsing::CppObjectModel::CppObject* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::InclusiveOrExpressionRule : public Cm::Parsing::Rule
{
public:
    InclusiveOrExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::Postleft));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new BinaryOpExpr(context.value, bitor_, 4, context.fromright);
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromleft(), fromright() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::CppObject* fromleft;
        Cm::Parsing::CppObjectModel::CppObject* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::ExclusiveOrExpressionRule : public Cm::Parsing::Rule
{
public:
    ExclusiveOrExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::Postleft));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new BinaryOpExpr(context.value, bitxor, 5, context.fromright);
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromleft(), fromright() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::CppObject* fromleft;
        Cm::Parsing::CppObjectModel::CppObject* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::AndExpressionRule : public Cm::Parsing::Rule
{
public:
    AndExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AndExpressionRule>(this, &AndExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AndExpressionRule>(this, &AndExpressionRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AndExpressionRule>(this, &AndExpressionRule::Postleft));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AndExpressionRule>(this, &AndExpressionRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new BinaryOpExpr(context.value, bitand_, 6, context.fromright);
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromleft(), fromright() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::CppObject* fromleft;
        Cm::Parsing::CppObjectModel::CppObject* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::EqualityExpressionRule : public Cm::Parsing::Rule
{
public:
    EqualityExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<EqualityExpressionRule>(this, &EqualityExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<EqualityExpressionRule>(this, &EqualityExpressionRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<EqualityExpressionRule>(this, &EqualityExpressionRule::Postleft));
        Cm::Parsing::NonterminalParser* eqOpNonterminalParser = GetNonterminal("EqOp");
        eqOpNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<EqualityExpressionRule>(this, &EqualityExpressionRule::PostEqOp));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<EqualityExpressionRule>(this, &EqualityExpressionRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new BinaryOpExpr(context.value, context.fromEqOp, 7, context.fromright);
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostEqOp(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromEqOp_value = std::move(stack.top());
            context.fromEqOp = *static_cast<Cm::Parsing::ValueObject<Operator>*>(fromEqOp_value.get());
            stack.pop();
        }
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromleft(), fromEqOp(), fromright() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::CppObject* fromleft;
        Operator fromEqOp;
        Cm::Parsing::CppObjectModel::CppObject* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::EqOpRule : public Cm::Parsing::Rule
{
public:
    EqOpRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Operator");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Operator>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<EqOpRule>(this, &EqOpRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<EqOpRule>(this, &EqOpRule::A1Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = eq;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = notEq;
    }
private:
    struct Context
    {
        Context(): value() {}
        Operator value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::RelationalExpressionRule : public Cm::Parsing::Rule
{
public:
    RelationalExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RelationalExpressionRule>(this, &RelationalExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RelationalExpressionRule>(this, &RelationalExpressionRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RelationalExpressionRule>(this, &RelationalExpressionRule::Postleft));
        Cm::Parsing::NonterminalParser* relOpNonterminalParser = GetNonterminal("RelOp");
        relOpNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RelationalExpressionRule>(this, &RelationalExpressionRule::PostRelOp));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RelationalExpressionRule>(this, &RelationalExpressionRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new BinaryOpExpr(context.value, context.fromRelOp, 8, context.fromright);
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostRelOp(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromRelOp_value = std::move(stack.top());
            context.fromRelOp = *static_cast<Cm::Parsing::ValueObject<Operator>*>(fromRelOp_value.get());
            stack.pop();
        }
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromleft(), fromRelOp(), fromright() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::CppObject* fromleft;
        Operator fromRelOp;
        Cm::Parsing::CppObjectModel::CppObject* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::RelOpRule : public Cm::Parsing::Rule
{
public:
    RelOpRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Operator");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Operator>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A3Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = lessOrEq;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = greaterOrEq;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = less;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = greater;
    }
private:
    struct Context
    {
        Context(): value() {}
        Operator value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::ShiftExpressionRule : public Cm::Parsing::Rule
{
public:
    ShiftExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ShiftExpressionRule>(this, &ShiftExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ShiftExpressionRule>(this, &ShiftExpressionRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ShiftExpressionRule>(this, &ShiftExpressionRule::Postleft));
        Cm::Parsing::NonterminalParser* shiftOpNonterminalParser = GetNonterminal("ShiftOp");
        shiftOpNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ShiftExpressionRule>(this, &ShiftExpressionRule::PostShiftOp));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ShiftExpressionRule>(this, &ShiftExpressionRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new BinaryOpExpr(context.value, context.fromShiftOp, 9, context.fromright);
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostShiftOp(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromShiftOp_value = std::move(stack.top());
            context.fromShiftOp = *static_cast<Cm::Parsing::ValueObject<Operator>*>(fromShiftOp_value.get());
            stack.pop();
        }
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromleft(), fromShiftOp(), fromright() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::CppObject* fromleft;
        Operator fromShiftOp;
        Cm::Parsing::CppObjectModel::CppObject* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::ShiftOpRule : public Cm::Parsing::Rule
{
public:
    ShiftOpRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Operator");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Operator>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ShiftOpRule>(this, &ShiftOpRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ShiftOpRule>(this, &ShiftOpRule::A1Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = shiftLeft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = shiftRight;
    }
private:
    struct Context
    {
        Context(): value() {}
        Operator value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::AdditiveExpressionRule : public Cm::Parsing::Rule
{
public:
    AdditiveExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AdditiveExpressionRule>(this, &AdditiveExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AdditiveExpressionRule>(this, &AdditiveExpressionRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AdditiveExpressionRule>(this, &AdditiveExpressionRule::Postleft));
        Cm::Parsing::NonterminalParser* addOpNonterminalParser = GetNonterminal("AddOp");
        addOpNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AdditiveExpressionRule>(this, &AdditiveExpressionRule::PostAddOp));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AdditiveExpressionRule>(this, &AdditiveExpressionRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new BinaryOpExpr(context.value, context.fromAddOp, 10, context.fromright);
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostAddOp(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAddOp_value = std::move(stack.top());
            context.fromAddOp = *static_cast<Cm::Parsing::ValueObject<Operator>*>(fromAddOp_value.get());
            stack.pop();
        }
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromleft(), fromAddOp(), fromright() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::CppObject* fromleft;
        Operator fromAddOp;
        Cm::Parsing::CppObjectModel::CppObject* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::AddOpRule : public Cm::Parsing::Rule
{
public:
    AddOpRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Operator");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Operator>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AddOpRule>(this, &AddOpRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AddOpRule>(this, &AddOpRule::A1Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = plus;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = minus;
    }
private:
    struct Context
    {
        Context(): value() {}
        Operator value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::MultiplicativeExpressionRule : public Cm::Parsing::Rule
{
public:
    MultiplicativeExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::Postleft));
        Cm::Parsing::NonterminalParser* mulOpNonterminalParser = GetNonterminal("MulOp");
        mulOpNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::PostMulOp));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new BinaryOpExpr(context.value, context.fromMulOp, 11, context.fromright);
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostMulOp(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromMulOp_value = std::move(stack.top());
            context.fromMulOp = *static_cast<Cm::Parsing::ValueObject<Operator>*>(fromMulOp_value.get());
            stack.pop();
        }
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromleft(), fromMulOp(), fromright() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::CppObject* fromleft;
        Operator fromMulOp;
        Cm::Parsing::CppObjectModel::CppObject* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::MulOpRule : public Cm::Parsing::Rule
{
public:
    MulOpRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Operator");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Operator>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MulOpRule>(this, &MulOpRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MulOpRule>(this, &MulOpRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MulOpRule>(this, &MulOpRule::A2Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = mul;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = div;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = rem;
    }
private:
    struct Context
    {
        Context(): value() {}
        Operator value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::PmExpressionRule : public Cm::Parsing::Rule
{
public:
    PmExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PmExpressionRule>(this, &PmExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PmExpressionRule>(this, &PmExpressionRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PmExpressionRule>(this, &PmExpressionRule::Postleft));
        Cm::Parsing::NonterminalParser* pmOpNonterminalParser = GetNonterminal("PmOp");
        pmOpNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PmExpressionRule>(this, &PmExpressionRule::PostPmOp));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PmExpressionRule>(this, &PmExpressionRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new BinaryOpExpr(context.value, context.fromPmOp, 12, context.fromright);
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostPmOp(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromPmOp_value = std::move(stack.top());
            context.fromPmOp = *static_cast<Cm::Parsing::ValueObject<Operator>*>(fromPmOp_value.get());
            stack.pop();
        }
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromleft(), fromPmOp(), fromright() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::CppObject* fromleft;
        Operator fromPmOp;
        Cm::Parsing::CppObjectModel::CppObject* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::PmOpRule : public Cm::Parsing::Rule
{
public:
    PmOpRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Operator");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Operator>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PmOpRule>(this, &PmOpRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PmOpRule>(this, &PmOpRule::A1Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = dotStar;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = arrowStar;
    }
private:
    struct Context
    {
        Context(): value() {}
        Operator value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::CastExpressionRule : public Cm::Parsing::Rule
{
public:
    CastExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<CppObject>", "ce"));
        AddLocalVariable(AttrOrVariable("std::unique_ptr<CppObject>", "ti"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A3Action));
        Cm::Parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal("TypeId");
        typeIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CastExpressionRule>(this, &CastExpressionRule::PostTypeId));
        Cm::Parsing::NonterminalParser* castExpressionNonterminalParser = GetNonterminal("CastExpression");
        castExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CastExpressionRule>(this, &CastExpressionRule::PostCastExpression));
        Cm::Parsing::NonterminalParser* unaryExpressionNonterminalParser = GetNonterminal("UnaryExpression");
        unaryExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CastExpressionRule>(this, &CastExpressionRule::PostUnaryExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.ce.release();
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ce.reset(new CastExpr(context.ti.release(), context.fromCastExpression));
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ti.reset(context.fromTypeId);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ce.reset(context.fromUnaryExpression);
    }
    void PostTypeId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeId_value = std::move(stack.top());
            context.fromTypeId = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>*>(fromTypeId_value.get());
            stack.pop();
        }
    }
    void PostCastExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCastExpression_value = std::move(stack.top());
            context.fromCastExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromCastExpression_value.get());
            stack.pop();
        }
    }
    void PostUnaryExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromUnaryExpression_value = std::move(stack.top());
            context.fromUnaryExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromUnaryExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), ce(), ti(), fromTypeId(), fromCastExpression(), fromUnaryExpression() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        std::unique_ptr<CppObject> ce;
        std::unique_ptr<CppObject> ti;
        Cm::Parsing::CppObjectModel::TypeId* fromTypeId;
        Cm::Parsing::CppObjectModel::CppObject* fromCastExpression;
        Cm::Parsing::CppObjectModel::CppObject* fromUnaryExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::UnaryExpressionRule : public Cm::Parsing::Rule
{
public:
    UnaryExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<CppObject>", "ue"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A5Action));
        Cm::Parsing::ActionParser* a6ActionParser = GetAction("A6");
        a6ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A6Action));
        Cm::Parsing::ActionParser* a7ActionParser = GetAction("A7");
        a7ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A7Action));
        Cm::Parsing::ActionParser* a8ActionParser = GetAction("A8");
        a8ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A8Action));
        Cm::Parsing::ActionParser* a9ActionParser = GetAction("A9");
        a9ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A9Action));
        Cm::Parsing::NonterminalParser* postfixExpressionNonterminalParser = GetNonterminal("PostfixExpression");
        postfixExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostPostfixExpression));
        Cm::Parsing::NonterminalParser* postCastExpressionNonterminalParser = GetNonterminal("PostCastExpression");
        postCastExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostPostCastExpression));
        Cm::Parsing::NonterminalParser* e1NonterminalParser = GetNonterminal("e1");
        e1NonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Poste1));
        Cm::Parsing::NonterminalParser* e2NonterminalParser = GetNonterminal("e2");
        e2NonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Poste2));
        Cm::Parsing::NonterminalParser* unaryOperatorNonterminalParser = GetNonterminal("UnaryOperator");
        unaryOperatorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostUnaryOperator));
        Cm::Parsing::NonterminalParser* castExpressionNonterminalParser = GetNonterminal("CastExpression");
        castExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostCastExpression));
        Cm::Parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal("TypeId");
        typeIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostTypeId));
        Cm::Parsing::NonterminalParser* e3NonterminalParser = GetNonterminal("e3");
        e3NonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Poste3));
        Cm::Parsing::NonterminalParser* newExpressionNonterminalParser = GetNonterminal("NewExpression");
        newExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostNewExpression));
        Cm::Parsing::NonterminalParser* deleteExpressionNonterminalParser = GetNonterminal("DeleteExpression");
        deleteExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostDeleteExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.ue.release();
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ue.reset(context.fromPostfixExpression);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ue.reset(context.fromPostCastExpression);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ue.reset(new PreIncrementExpr(context.frome1));
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ue.reset(new PreDecrementExpr(context.frome2));
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ue.reset(new UnaryOpExpr(context.fromUnaryOperator, context.fromCastExpression));
    }
    void A6Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ue.reset(new SizeOfExpr(context.fromTypeId, true));
    }
    void A7Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ue.reset(new SizeOfExpr(context.frome3, false));
    }
    void A8Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ue.reset(context.fromNewExpression);
    }
    void A9Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ue.reset(context.fromDeleteExpression);
    }
    void PostPostfixExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromPostfixExpression_value = std::move(stack.top());
            context.fromPostfixExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromPostfixExpression_value.get());
            stack.pop();
        }
    }
    void PostPostCastExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromPostCastExpression_value = std::move(stack.top());
            context.fromPostCastExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromPostCastExpression_value.get());
            stack.pop();
        }
    }
    void Poste1(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> frome1_value = std::move(stack.top());
            context.frome1 = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(frome1_value.get());
            stack.pop();
        }
    }
    void Poste2(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> frome2_value = std::move(stack.top());
            context.frome2 = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(frome2_value.get());
            stack.pop();
        }
    }
    void PostUnaryOperator(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromUnaryOperator_value = std::move(stack.top());
            context.fromUnaryOperator = *static_cast<Cm::Parsing::ValueObject<Operator>*>(fromUnaryOperator_value.get());
            stack.pop();
        }
    }
    void PostCastExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCastExpression_value = std::move(stack.top());
            context.fromCastExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromCastExpression_value.get());
            stack.pop();
        }
    }
    void PostTypeId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeId_value = std::move(stack.top());
            context.fromTypeId = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>*>(fromTypeId_value.get());
            stack.pop();
        }
    }
    void Poste3(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> frome3_value = std::move(stack.top());
            context.frome3 = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(frome3_value.get());
            stack.pop();
        }
    }
    void PostNewExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromNewExpression_value = std::move(stack.top());
            context.fromNewExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromNewExpression_value.get());
            stack.pop();
        }
    }
    void PostDeleteExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDeleteExpression_value = std::move(stack.top());
            context.fromDeleteExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromDeleteExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), ue(), fromPostfixExpression(), fromPostCastExpression(), frome1(), frome2(), fromUnaryOperator(), fromCastExpression(), fromTypeId(), frome3(), fromNewExpression(), fromDeleteExpression() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        std::unique_ptr<CppObject> ue;
        Cm::Parsing::CppObjectModel::CppObject* fromPostfixExpression;
        Cm::Parsing::CppObjectModel::CppObject* fromPostCastExpression;
        Cm::Parsing::CppObjectModel::CppObject* frome1;
        Cm::Parsing::CppObjectModel::CppObject* frome2;
        Operator fromUnaryOperator;
        Cm::Parsing::CppObjectModel::CppObject* fromCastExpression;
        Cm::Parsing::CppObjectModel::TypeId* fromTypeId;
        Cm::Parsing::CppObjectModel::CppObject* frome3;
        Cm::Parsing::CppObjectModel::CppObject* fromNewExpression;
        Cm::Parsing::CppObjectModel::CppObject* fromDeleteExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::UnaryOperatorRule : public Cm::Parsing::Rule
{
public:
    UnaryOperatorRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Operator");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Operator>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A5Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = deref;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = addrOf;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = plus;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = minus;
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = not_;
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = neg;
    }
private:
    struct Context
    {
        Context(): value() {}
        Operator value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::NewExpressionRule : public Cm::Parsing::Rule
{
public:
    NewExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
        AddLocalVariable(AttrOrVariable("bool", "global"));
        AddLocalVariable(AttrOrVariable("TypeId*", "typeId"));
        AddLocalVariable(AttrOrVariable("bool", "parens"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NewExpressionRule>(this, &NewExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NewExpressionRule>(this, &NewExpressionRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NewExpressionRule>(this, &NewExpressionRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NewExpressionRule>(this, &NewExpressionRule::A3Action));
        Cm::Parsing::NonterminalParser* newPlacementNonterminalParser = GetNonterminal("NewPlacement");
        newPlacementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostNewPlacement));
        Cm::Parsing::NonterminalParser* newTypeIdNonterminalParser = GetNonterminal("NewTypeId");
        newTypeIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostNewTypeId));
        Cm::Parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal("TypeId");
        typeIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostTypeId));
        Cm::Parsing::NonterminalParser* newInitializerNonterminalParser = GetNonterminal("NewInitializer");
        newInitializerNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostNewInitializer));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new NewExpr(context.global, context.fromNewPlacement, context.typeId, context.parens, context.fromNewInitializer);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.global = true;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.typeId = context.fromNewTypeId;
        context.parens = false;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.typeId = context.fromTypeId;
        context.parens = true;
    }
    void PostNewPlacement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromNewPlacement_value = std::move(stack.top());
            context.fromNewPlacement = *static_cast<Cm::Parsing::ValueObject<std::vector<Cm::Parsing::CppObjectModel::CppObject*>>*>(fromNewPlacement_value.get());
            stack.pop();
        }
    }
    void PostNewTypeId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromNewTypeId_value = std::move(stack.top());
            context.fromNewTypeId = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>*>(fromNewTypeId_value.get());
            stack.pop();
        }
    }
    void PostTypeId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeId_value = std::move(stack.top());
            context.fromTypeId = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>*>(fromTypeId_value.get());
            stack.pop();
        }
    }
    void PostNewInitializer(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromNewInitializer_value = std::move(stack.top());
            context.fromNewInitializer = *static_cast<Cm::Parsing::ValueObject<std::vector<Cm::Parsing::CppObjectModel::CppObject*>>*>(fromNewInitializer_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), global(), typeId(), parens(), fromNewPlacement(), fromNewTypeId(), fromTypeId(), fromNewInitializer() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        bool global;
        TypeId* typeId;
        bool parens;
        std::vector<Cm::Parsing::CppObjectModel::CppObject*> fromNewPlacement;
        Cm::Parsing::CppObjectModel::TypeId* fromNewTypeId;
        Cm::Parsing::CppObjectModel::TypeId* fromTypeId;
        std::vector<Cm::Parsing::CppObjectModel::CppObject*> fromNewInitializer;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::NewTypeIdRule : public Cm::Parsing::Rule
{
public:
    NewTypeIdRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::TypeId*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<TypeId>", "ti"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NewTypeIdRule>(this, &NewTypeIdRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NewTypeIdRule>(this, &NewTypeIdRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NewTypeIdRule>(this, &NewTypeIdRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NewTypeIdRule>(this, &NewTypeIdRule::A3Action));
        Cm::Parsing::NonterminalParser* typeSpecifierSeqNonterminalParser = GetNonterminal("TypeSpecifierSeq");
        typeSpecifierSeqNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<NewTypeIdRule>(this, &NewTypeIdRule::PreTypeSpecifierSeq));
        Cm::Parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal("TypeName");
        typeNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NewTypeIdRule>(this, &NewTypeIdRule::PostTypeName));
        Cm::Parsing::NonterminalParser* newDeclaratorNonterminalParser = GetNonterminal("NewDeclarator");
        newDeclaratorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NewTypeIdRule>(this, &NewTypeIdRule::PostNewDeclarator));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ti.reset(new TypeId);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.ti.release();
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ti->Add(context.fromTypeName);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ti->Declarator() = context.fromNewDeclarator;
    }
    void PreTypeSpecifierSeq(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>(context.ti.get())));
    }
    void PostTypeName(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeName_value = std::move(stack.top());
            context.fromTypeName = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeName*>*>(fromTypeName_value.get());
            stack.pop();
        }
    }
    void PostNewDeclarator(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromNewDeclarator_value = std::move(stack.top());
            context.fromNewDeclarator = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromNewDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), ti(), fromTypeName(), fromNewDeclarator() {}
        Cm::Parsing::CppObjectModel::TypeId* value;
        std::unique_ptr<TypeId> ti;
        Cm::Parsing::CppObjectModel::TypeName* fromTypeName;
        std::string fromNewDeclarator;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::NewDeclaratorRule : public Cm::Parsing::Rule
{
public:
    NewDeclaratorRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NewDeclaratorRule>(this, &NewDeclaratorRule::A0Action));
        Cm::Parsing::NonterminalParser* newDeclaratorNonterminalParser = GetNonterminal("NewDeclarator");
        newDeclaratorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NewDeclaratorRule>(this, &NewDeclaratorRule::PostNewDeclarator));
        Cm::Parsing::NonterminalParser* directNewDeclaratorNonterminalParser = GetNonterminal("DirectNewDeclarator");
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = Trim(std::string(matchBegin, matchEnd));
    }
    void PostNewDeclarator(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromNewDeclarator_value = std::move(stack.top());
            context.fromNewDeclarator = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromNewDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromNewDeclarator() {}
        std::string value;
        std::string fromNewDeclarator;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::DirectNewDeclaratorRule : public Cm::Parsing::Rule
{
public:
    DirectNewDeclaratorRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddLocalVariable(AttrOrVariable("std::unique_ptr<CppObject>", "e"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DirectNewDeclaratorRule>(this, &DirectNewDeclaratorRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DirectNewDeclaratorRule>(this, &DirectNewDeclaratorRule::A1Action));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DirectNewDeclaratorRule>(this, &DirectNewDeclaratorRule::PostExpression));
        Cm::Parsing::NonterminalParser* constantExpressionNonterminalParser = GetNonterminal("ConstantExpression");
        constantExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DirectNewDeclaratorRule>(this, &DirectNewDeclaratorRule::PostConstantExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.e.reset(context.fromExpression);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.e.reset(context.fromConstantExpression);
    }
    void PostExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromExpression_value = std::move(stack.top());
            context.fromExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
    void PostConstantExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConstantExpression_value = std::move(stack.top());
            context.fromConstantExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromConstantExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): e(), fromExpression(), fromConstantExpression() {}
        std::unique_ptr<CppObject> e;
        Cm::Parsing::CppObjectModel::CppObject* fromExpression;
        Cm::Parsing::CppObjectModel::CppObject* fromConstantExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::NewPlacementRule : public Cm::Parsing::Rule
{
public:
    NewPlacementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("std::vector<Cm::Parsing::CppObjectModel::CppObject*>");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<std::vector<Cm::Parsing::CppObjectModel::CppObject*>>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NewPlacementRule>(this, &NewPlacementRule::A0Action));
        Cm::Parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal("ExpressionList");
        expressionListNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NewPlacementRule>(this, &NewPlacementRule::PostExpressionList));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromExpressionList;
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
private:
    struct Context
    {
        Context(): value(), fromExpressionList() {}
        std::vector<Cm::Parsing::CppObjectModel::CppObject*> value;
        std::vector<Cm::Parsing::CppObjectModel::CppObject*> fromExpressionList;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::NewInitializerRule : public Cm::Parsing::Rule
{
public:
    NewInitializerRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("std::vector<Cm::Parsing::CppObjectModel::CppObject*>");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<std::vector<Cm::Parsing::CppObjectModel::CppObject*>>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NewInitializerRule>(this, &NewInitializerRule::A0Action));
        Cm::Parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal("ExpressionList");
        expressionListNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NewInitializerRule>(this, &NewInitializerRule::PostExpressionList));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromExpressionList;
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
private:
    struct Context
    {
        Context(): value(), fromExpressionList() {}
        std::vector<Cm::Parsing::CppObjectModel::CppObject*> value;
        std::vector<Cm::Parsing::CppObjectModel::CppObject*> fromExpressionList;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::DeleteExpressionRule : public Cm::Parsing::Rule
{
public:
    DeleteExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
        AddLocalVariable(AttrOrVariable("bool", "global"));
        AddLocalVariable(AttrOrVariable("bool", "array"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeleteExpressionRule>(this, &DeleteExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeleteExpressionRule>(this, &DeleteExpressionRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeleteExpressionRule>(this, &DeleteExpressionRule::A2Action));
        Cm::Parsing::NonterminalParser* castExpressionNonterminalParser = GetNonterminal("CastExpression");
        castExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeleteExpressionRule>(this, &DeleteExpressionRule::PostCastExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new DeleteExpr(context.global, context.array, context.fromCastExpression);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.global = true;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.array = true;
    }
    void PostCastExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCastExpression_value = std::move(stack.top());
            context.fromCastExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromCastExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), global(), array(), fromCastExpression() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        bool global;
        bool array;
        Cm::Parsing::CppObjectModel::CppObject* fromCastExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::PostfixExpressionRule : public Cm::Parsing::Rule
{
public:
    PostfixExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<CppObject>", "pe"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A5Action));
        Cm::Parsing::ActionParser* a6ActionParser = GetAction("A6");
        a6ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A6Action));
        Cm::Parsing::ActionParser* a7ActionParser = GetAction("A7");
        a7ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A7Action));
        Cm::Parsing::ActionParser* a8ActionParser = GetAction("A8");
        a8ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A8Action));
        Cm::Parsing::NonterminalParser* primaryExpressionNonterminalParser = GetNonterminal("PrimaryExpression");
        primaryExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::PostPrimaryExpression));
        Cm::Parsing::NonterminalParser* indexNonterminalParser = GetNonterminal("index");
        indexNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postindex));
        Cm::Parsing::NonterminalParser* invokeNonterminalParser = GetNonterminal("invoke");
        invokeNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postinvoke));
        Cm::Parsing::NonterminalParser* memberNonterminalParser = GetNonterminal("member");
        memberNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postmember));
        Cm::Parsing::NonterminalParser* ptrmemberNonterminalParser = GetNonterminal("ptrmember");
        ptrmemberNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postptrmember));
        Cm::Parsing::NonterminalParser* typeSpecifierOrTypeNameNonterminalParser = GetNonterminal("TypeSpecifierOrTypeName");
        typeSpecifierOrTypeNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::PostTypeSpecifierOrTypeName));
        Cm::Parsing::NonterminalParser* typeConstructionNonterminalParser = GetNonterminal("typeConstruction");
        typeConstructionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::PosttypeConstruction));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.pe.release();
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.pe.reset(context.fromPrimaryExpression);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.pe.reset(new IndexExpr(context.pe.release(), context.fromindex));
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.pe.reset(new InvokeExpr(context.pe.release(), context.frominvoke));
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.pe.reset(new MemberAccessExpr(context.pe.release(), context.frommember));
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.pe.reset(new PtrMemberAccessExpr(context.pe.release(), context.fromptrmember));
    }
    void A6Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.pe.reset(new PostIncrementExpr(context.pe.release()));
    }
    void A7Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.pe.reset(new PostDecrementExpr(context.pe.release()));
    }
    void A8Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.pe.reset(new InvokeExpr(context.fromTypeSpecifierOrTypeName, context.fromtypeConstruction));
    }
    void PostPrimaryExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromPrimaryExpression_value = std::move(stack.top());
            context.fromPrimaryExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromPrimaryExpression_value.get());
            stack.pop();
        }
    }
    void Postindex(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromindex_value = std::move(stack.top());
            context.fromindex = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromindex_value.get());
            stack.pop();
        }
    }
    void Postinvoke(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> frominvoke_value = std::move(stack.top());
            context.frominvoke = *static_cast<Cm::Parsing::ValueObject<std::vector<Cm::Parsing::CppObjectModel::CppObject*>>*>(frominvoke_value.get());
            stack.pop();
        }
    }
    void Postmember(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> frommember_value = std::move(stack.top());
            context.frommember = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::IdExpr*>*>(frommember_value.get());
            stack.pop();
        }
    }
    void Postptrmember(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromptrmember_value = std::move(stack.top());
            context.fromptrmember = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::IdExpr*>*>(fromptrmember_value.get());
            stack.pop();
        }
    }
    void PostTypeSpecifierOrTypeName(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeSpecifierOrTypeName_value = std::move(stack.top());
            context.fromTypeSpecifierOrTypeName = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromTypeSpecifierOrTypeName_value.get());
            stack.pop();
        }
    }
    void PosttypeConstruction(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromtypeConstruction_value = std::move(stack.top());
            context.fromtypeConstruction = *static_cast<Cm::Parsing::ValueObject<std::vector<Cm::Parsing::CppObjectModel::CppObject*>>*>(fromtypeConstruction_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), pe(), fromPrimaryExpression(), fromindex(), frominvoke(), frommember(), fromptrmember(), fromTypeSpecifierOrTypeName(), fromtypeConstruction() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        std::unique_ptr<CppObject> pe;
        Cm::Parsing::CppObjectModel::CppObject* fromPrimaryExpression;
        Cm::Parsing::CppObjectModel::CppObject* fromindex;
        std::vector<Cm::Parsing::CppObjectModel::CppObject*> frominvoke;
        Cm::Parsing::CppObjectModel::IdExpr* frommember;
        Cm::Parsing::CppObjectModel::IdExpr* fromptrmember;
        Cm::Parsing::CppObjectModel::CppObject* fromTypeSpecifierOrTypeName;
        std::vector<Cm::Parsing::CppObjectModel::CppObject*> fromtypeConstruction;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::TypeSpecifierOrTypeNameRule : public Cm::Parsing::Rule
{
public:
    TypeSpecifierOrTypeNameRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeSpecifierOrTypeNameRule>(this, &TypeSpecifierOrTypeNameRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeSpecifierOrTypeNameRule>(this, &TypeSpecifierOrTypeNameRule::A1Action));
        Cm::Parsing::NonterminalParser* simpleTypeSpecifierNonterminalParser = GetNonterminal("SimpleTypeSpecifier");
        simpleTypeSpecifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypeSpecifierOrTypeNameRule>(this, &TypeSpecifierOrTypeNameRule::PostSimpleTypeSpecifier));
        Cm::Parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal("TypeName");
        typeNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypeSpecifierOrTypeNameRule>(this, &TypeSpecifierOrTypeNameRule::PostTypeName));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromSimpleTypeSpecifier;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromTypeName;
    }
    void PostSimpleTypeSpecifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromSimpleTypeSpecifier_value = std::move(stack.top());
            context.fromSimpleTypeSpecifier = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeSpecifier*>*>(fromSimpleTypeSpecifier_value.get());
            stack.pop();
        }
    }
    void PostTypeName(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeName_value = std::move(stack.top());
            context.fromTypeName = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeName*>*>(fromTypeName_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromSimpleTypeSpecifier(), fromTypeName() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::TypeSpecifier* fromSimpleTypeSpecifier;
        Cm::Parsing::CppObjectModel::TypeName* fromTypeName;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::PostCastExpressionRule : public Cm::Parsing::Rule
{
public:
    PostCastExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A4Action));
        Cm::Parsing::NonterminalParser* dctNonterminalParser = GetNonterminal("dct");
        dctNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postdct));
        Cm::Parsing::NonterminalParser* dceNonterminalParser = GetNonterminal("dce");
        dceNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postdce));
        Cm::Parsing::NonterminalParser* sctNonterminalParser = GetNonterminal("sct");
        sctNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postsct));
        Cm::Parsing::NonterminalParser* sceNonterminalParser = GetNonterminal("sce");
        sceNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postsce));
        Cm::Parsing::NonterminalParser* rctNonterminalParser = GetNonterminal("rct");
        rctNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postrct));
        Cm::Parsing::NonterminalParser* rceNonterminalParser = GetNonterminal("rce");
        rceNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postrce));
        Cm::Parsing::NonterminalParser* cctNonterminalParser = GetNonterminal("cct");
        cctNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postcct));
        Cm::Parsing::NonterminalParser* cceNonterminalParser = GetNonterminal("cce");
        cceNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postcce));
        Cm::Parsing::NonterminalParser* typeidExprNonterminalParser = GetNonterminal("typeidExpr");
        typeidExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::PosttypeidExpr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new PostCastExpr("dynamic_cast", context.fromdct, context.fromdce);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new PostCastExpr("static_cast", context.fromsct, context.fromsce);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new PostCastExpr("reinterpret_cast", context.fromrct, context.fromrce);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new PostCastExpr("const_cast", context.fromcct, context.fromcce);
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new TypeIdExpr(context.fromtypeidExpr);
    }
    void Postdct(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromdct_value = std::move(stack.top());
            context.fromdct = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>*>(fromdct_value.get());
            stack.pop();
        }
    }
    void Postdce(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromdce_value = std::move(stack.top());
            context.fromdce = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromdce_value.get());
            stack.pop();
        }
    }
    void Postsct(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromsct_value = std::move(stack.top());
            context.fromsct = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>*>(fromsct_value.get());
            stack.pop();
        }
    }
    void Postsce(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromsce_value = std::move(stack.top());
            context.fromsce = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromsce_value.get());
            stack.pop();
        }
    }
    void Postrct(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromrct_value = std::move(stack.top());
            context.fromrct = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>*>(fromrct_value.get());
            stack.pop();
        }
    }
    void Postrce(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromrce_value = std::move(stack.top());
            context.fromrce = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromrce_value.get());
            stack.pop();
        }
    }
    void Postcct(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromcct_value = std::move(stack.top());
            context.fromcct = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>*>(fromcct_value.get());
            stack.pop();
        }
    }
    void Postcce(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromcce_value = std::move(stack.top());
            context.fromcce = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromcce_value.get());
            stack.pop();
        }
    }
    void PosttypeidExpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromtypeidExpr_value = std::move(stack.top());
            context.fromtypeidExpr = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromtypeidExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromdct(), fromdce(), fromsct(), fromsce(), fromrct(), fromrce(), fromcct(), fromcce(), fromtypeidExpr() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::TypeId* fromdct;
        Cm::Parsing::CppObjectModel::CppObject* fromdce;
        Cm::Parsing::CppObjectModel::TypeId* fromsct;
        Cm::Parsing::CppObjectModel::CppObject* fromsce;
        Cm::Parsing::CppObjectModel::TypeId* fromrct;
        Cm::Parsing::CppObjectModel::CppObject* fromrce;
        Cm::Parsing::CppObjectModel::TypeId* fromcct;
        Cm::Parsing::CppObjectModel::CppObject* fromcce;
        Cm::Parsing::CppObjectModel::CppObject* fromtypeidExpr;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::ExpressionListRule : public Cm::Parsing::Rule
{
public:
    ExpressionListRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("std::vector<Cm::Parsing::CppObjectModel::CppObject*>");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<std::vector<Cm::Parsing::CppObjectModel::CppObject*>>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ExpressionListRule>(this, &ExpressionListRule::A0Action));
        Cm::Parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal("AssignmentExpression");
        assignmentExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ExpressionListRule>(this, &ExpressionListRule::PostAssignmentExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value.push_back(context.fromAssignmentExpression);
    }
    void PostAssignmentExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAssignmentExpression_value = std::move(stack.top());
            context.fromAssignmentExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromAssignmentExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromAssignmentExpression() {}
        std::vector<Cm::Parsing::CppObjectModel::CppObject*> value;
        Cm::Parsing::CppObjectModel::CppObject* fromAssignmentExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::PrimaryExpressionRule : public Cm::Parsing::Rule
{
public:
    PrimaryExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<CppObject>", "pe"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A4Action));
        Cm::Parsing::NonterminalParser* literalNonterminalParser = GetNonterminal("Literal");
        literalNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostLiteral));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostExpression));
        Cm::Parsing::NonterminalParser* idExpressionNonterminalParser = GetNonterminal("IdExpression");
        idExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostIdExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.pe.release();
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.pe.reset(context.fromLiteral);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.pe.reset(new ThisAccess);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.pe.reset(context.fromExpression);
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.pe.reset(context.fromIdExpression);
    }
    void PostLiteral(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromLiteral_value = std::move(stack.top());
            context.fromLiteral = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Literal*>*>(fromLiteral_value.get());
            stack.pop();
        }
    }
    void PostExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromExpression_value = std::move(stack.top());
            context.fromExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
    void PostIdExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIdExpression_value = std::move(stack.top());
            context.fromIdExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::IdExpr*>*>(fromIdExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), pe(), fromLiteral(), fromExpression(), fromIdExpression() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        std::unique_ptr<CppObject> pe;
        Cm::Parsing::CppObjectModel::Literal* fromLiteral;
        Cm::Parsing::CppObjectModel::CppObject* fromExpression;
        Cm::Parsing::CppObjectModel::IdExpr* fromIdExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ExpressionGrammar::IdExpressionRule : public Cm::Parsing::Rule
{
public:
    IdExpressionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::IdExpr*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::IdExpr*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IdExpressionRule>(this, &IdExpressionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IdExpressionRule>(this, &IdExpressionRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IdExpressionRule>(this, &IdExpressionRule::A2Action));
        Cm::Parsing::NonterminalParser* id1NonterminalParser = GetNonterminal("id1");
        id1NonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IdExpressionRule>(this, &IdExpressionRule::Postid1));
        Cm::Parsing::NonterminalParser* id2NonterminalParser = GetNonterminal("id2");
        id2NonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IdExpressionRule>(this, &IdExpressionRule::Postid2));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new IdExpr(std::string(matchBegin, matchEnd));
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new IdExpr(std::string(matchBegin, matchEnd));
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new IdExpr(std::string(matchBegin, matchEnd));
    }
    void Postid1(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromid1_value = std::move(stack.top());
            context.fromid1 = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromid1_value.get());
            stack.pop();
        }
    }
    void Postid2(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromid2_value = std::move(stack.top());
            context.fromid2 = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromid2_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromid1(), fromid2() {}
        Cm::Parsing::CppObjectModel::IdExpr* value;
        std::string fromid1;
        std::string fromid2;
    };
    std::stack<Context> contextStack;
    Context context;
};

void ExpressionGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.Cpp.LiteralGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::Cpp::LiteralGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parsing.Cpp.DeclarationGrammar");
    if (!grammar1)
    {
        grammar1 = Cm::Parsing::Cpp::DeclarationGrammar::Create(pd);
    }
    AddGrammarReference(grammar1);
    Cm::Parsing::Grammar* grammar2 = pd->GetGrammar("Cm.Parsing.Cpp.IdentifierGrammar");
    if (!grammar2)
    {
        grammar2 = Cm::Parsing::Cpp::IdentifierGrammar::Create(pd);
    }
    AddGrammarReference(grammar2);
    Cm::Parsing::Grammar* grammar3 = pd->GetGrammar("Cm.Parsing.Cpp.DeclaratorGrammar");
    if (!grammar3)
    {
        grammar3 = Cm::Parsing::Cpp::DeclaratorGrammar::Create(pd);
    }
    AddGrammarReference(grammar3);
}

void ExpressionGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("SimpleTypeSpecifier", this, "DeclarationGrammar.SimpleTypeSpecifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("Literal", this, "LiteralGrammar.Literal"));
    AddRuleLink(new Cm::Parsing::RuleLink("QualifiedId", this, "IdentifierGrammar.QualifiedId"));
    AddRuleLink(new Cm::Parsing::RuleLink("TypeId", this, "DeclaratorGrammar.TypeId"));
    AddRuleLink(new Cm::Parsing::RuleLink("TypeSpecifierSeq", this, "DeclaratorGrammar.TypeSpecifierSeq"));
    AddRuleLink(new Cm::Parsing::RuleLink("PtrOperator", this, "DeclaratorGrammar.PtrOperator"));
    AddRuleLink(new Cm::Parsing::RuleLink("TypeName", this, "DeclarationGrammar.TypeName"));
    AddRule(new ExpressionRule("Expression", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "AssignmentExpression", 0)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser(','),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("right", "AssignmentExpression", 0)))))));
    AddRule(new ConstantExpressionRule("ConstantExpression", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("Expression", "Expression", 0))));
    AddRule(new AssignmentExpressionRule("AssignmentExpression", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::NonterminalParser("LogicalOrExpression", "LogicalOrExpression", 0)),
                            new Cm::Parsing::NonterminalParser("AssingmentOp", "AssingmentOp", 0)),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("AssignmentExpression", "AssignmentExpression", 0)))),
                new Cm::Parsing::ActionParser("A2",
                    new Cm::Parsing::NonterminalParser("ConditionalExpression", "ConditionalExpression", 0))),
            new Cm::Parsing::ActionParser("A3",
                new Cm::Parsing::NonterminalParser("ThrowExpression", "ThrowExpression", 0)))));
    AddRule(new AssingmentOpRule("AssingmentOp", GetScope(),
        new Cm::Parsing::ActionParser("A0",
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
                                                    new Cm::Parsing::CharParser('='),
                                                    new Cm::Parsing::StringParser("*=")),
                                                new Cm::Parsing::StringParser("/=")),
                                            new Cm::Parsing::StringParser("%=")),
                                        new Cm::Parsing::StringParser("+=")),
                                    new Cm::Parsing::StringParser("-=")),
                                new Cm::Parsing::StringParser(">>=")),
                            new Cm::Parsing::StringParser("<<=")),
                        new Cm::Parsing::StringParser("&=")),
                    new Cm::Parsing::StringParser("^=")),
                new Cm::Parsing::StringParser("|=")))));
    AddRule(new ThrowExpressionRule("ThrowExpression", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::KeywordParser("throw"),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::NonterminalParser("AssignmentExpression", "AssignmentExpression", 0))))));
    AddRule(new ConditionalExpressionRule("ConditionalExpression", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "LogicalOrExpression", 0)),
            new Cm::Parsing::OptionalParser(
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::CharParser('?'),
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::NonterminalParser("Expression", "Expression", 0))),
                            new Cm::Parsing::CharParser(':')),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("AssignmentExpression", "AssignmentExpression", 0))))))));
    AddRule(new LogicalOrExpressionRule("LogicalOrExpression", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "LogicalAndExpression", 0)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::StringParser("||"),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("right", "LogicalAndExpression", 0))))))));
    AddRule(new LogicalAndExpressionRule("LogicalAndExpression", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "InclusiveOrExpression", 0)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::StringParser("&&"),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("right", "InclusiveOrExpression", 0))))))));
    AddRule(new InclusiveOrExpressionRule("InclusiveOrExpression", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "ExclusiveOrExpression", 0)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::DifferenceParser(
                        new Cm::Parsing::CharParser('|'),
                        new Cm::Parsing::StringParser("||")),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("right", "ExclusiveOrExpression", 0))))))));
    AddRule(new ExclusiveOrExpressionRule("ExclusiveOrExpression", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "AndExpression", 0)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('^'),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("right", "AndExpression", 0))))))));
    AddRule(new AndExpressionRule("AndExpression", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "EqualityExpression", 0)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::DifferenceParser(
                        new Cm::Parsing::CharParser('&'),
                        new Cm::Parsing::StringParser("&&")),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("right", "EqualityExpression", 0))))))));
    AddRule(new EqualityExpressionRule("EqualityExpression", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "RelationalExpression", 0)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::NonterminalParser("EqOp", "EqOp", 0),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("right", "RelationalExpression", 0))))))));
    AddRule(new EqOpRule("EqOp", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::StringParser("==")),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::StringParser("!=")))));
    AddRule(new RelationalExpressionRule("RelationalExpression", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "ShiftExpression", 0)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::NonterminalParser("RelOp", "RelOp", 0),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("right", "ShiftExpression", 0))))))));
    AddRule(new RelOpRule("RelOp", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::StringParser("<=")),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::StringParser(">="))),
                new Cm::Parsing::ActionParser("A2",
                    new Cm::Parsing::DifferenceParser(
                        new Cm::Parsing::CharParser('<'),
                        new Cm::Parsing::StringParser("<<")))),
            new Cm::Parsing::ActionParser("A3",
                new Cm::Parsing::DifferenceParser(
                    new Cm::Parsing::CharParser('>'),
                    new Cm::Parsing::StringParser(">>"))))));
    AddRule(new ShiftExpressionRule("ShiftExpression", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "AdditiveExpression", 0)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::NonterminalParser("ShiftOp", "ShiftOp", 0),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("right", "AdditiveExpression", 0))))))));
    AddRule(new ShiftOpRule("ShiftOp", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::DifferenceParser(
                    new Cm::Parsing::StringParser("<<"),
                    new Cm::Parsing::StringParser("<<="))),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::DifferenceParser(
                    new Cm::Parsing::StringParser(">>"),
                    new Cm::Parsing::StringParser(">>="))))));
    AddRule(new AdditiveExpressionRule("AdditiveExpression", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "MultiplicativeExpression", 0)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::NonterminalParser("AddOp", "AddOp", 0),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("right", "MultiplicativeExpression", 0))))))));
    AddRule(new AddOpRule("AddOp", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::DifferenceParser(
                    new Cm::Parsing::CharParser('+'),
                    new Cm::Parsing::StringParser("+="))),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::DifferenceParser(
                    new Cm::Parsing::CharParser('-'),
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::StringParser("-="),
                        new Cm::Parsing::StringParser("->")))))));
    AddRule(new MultiplicativeExpressionRule("MultiplicativeExpression", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "PmExpression", 0)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::NonterminalParser("MulOp", "MulOp", 0),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("right", "PmExpression", 0))))))));
    AddRule(new MulOpRule("MulOp", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::DifferenceParser(
                        new Cm::Parsing::CharParser('*'),
                        new Cm::Parsing::StringParser("*="))),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::DifferenceParser(
                        new Cm::Parsing::CharParser('/'),
                        new Cm::Parsing::StringParser("/=")))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::DifferenceParser(
                    new Cm::Parsing::CharParser('%'),
                    new Cm::Parsing::StringParser("%="))))));
    AddRule(new PmExpressionRule("PmExpression", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "CastExpression", 0)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::NonterminalParser("PmOp", "PmOp", 0),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("right", "CastExpression", 0))))))));
    AddRule(new PmOpRule("PmOp", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::StringParser(".*")),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::StringParser("->*")))));
    AddRule(new CastExpressionRule("CastExpression", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::CharParser('('),
                                new Cm::Parsing::ActionParser("A2",
                                    new Cm::Parsing::NonterminalParser("TypeId", "TypeId", 0))),
                            new Cm::Parsing::CharParser(')')),
                        new Cm::Parsing::NonterminalParser("CastExpression", "CastExpression", 0))),
                new Cm::Parsing::ActionParser("A3",
                    new Cm::Parsing::NonterminalParser("UnaryExpression", "UnaryExpression", 0))))));
    AddRule(new UnaryExpressionRule("UnaryExpression", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::AlternativeParser(
                                new Cm::Parsing::AlternativeParser(
                                    new Cm::Parsing::AlternativeParser(
                                        new Cm::Parsing::AlternativeParser(
                                            new Cm::Parsing::ActionParser("A1",
                                                new Cm::Parsing::NonterminalParser("PostfixExpression", "PostfixExpression", 0)),
                                            new Cm::Parsing::ActionParser("A2",
                                                new Cm::Parsing::NonterminalParser("PostCastExpression", "PostCastExpression", 0))),
                                        new Cm::Parsing::ActionParser("A3",
                                            new Cm::Parsing::SequenceParser(
                                                new Cm::Parsing::StringParser("++"),
                                                new Cm::Parsing::ExpectationParser(
                                                    new Cm::Parsing::NonterminalParser("e1", "UnaryExpression", 0))))),
                                    new Cm::Parsing::ActionParser("A4",
                                        new Cm::Parsing::SequenceParser(
                                            new Cm::Parsing::StringParser("--"),
                                            new Cm::Parsing::ExpectationParser(
                                                new Cm::Parsing::NonterminalParser("e2", "UnaryExpression", 0))))),
                                new Cm::Parsing::ActionParser("A5",
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::NonterminalParser("UnaryOperator", "UnaryOperator", 0),
                                        new Cm::Parsing::ExpectationParser(
                                            new Cm::Parsing::NonterminalParser("CastExpression", "CastExpression", 0))))),
                            new Cm::Parsing::ActionParser("A6",
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::SequenceParser(
                                            new Cm::Parsing::KeywordParser("sizeof"),
                                            new Cm::Parsing::CharParser('(')),
                                        new Cm::Parsing::ExpectationParser(
                                            new Cm::Parsing::NonterminalParser("TypeId", "TypeId", 0))),
                                    new Cm::Parsing::ExpectationParser(
                                        new Cm::Parsing::CharParser(')'))))),
                        new Cm::Parsing::ActionParser("A7",
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::KeywordParser("sizeof"),
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::NonterminalParser("e3", "UnaryExpression", 0))))),
                    new Cm::Parsing::ActionParser("A8",
                        new Cm::Parsing::NonterminalParser("NewExpression", "NewExpression", 0))),
                new Cm::Parsing::ActionParser("A9",
                    new Cm::Parsing::NonterminalParser("DeleteExpression", "DeleteExpression", 0))))));
    AddRule(new UnaryOperatorRule("UnaryOperator", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::ActionParser("A0",
                                new Cm::Parsing::DifferenceParser(
                                    new Cm::Parsing::CharParser('*'),
                                    new Cm::Parsing::StringParser("*="))),
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::DifferenceParser(
                                    new Cm::Parsing::CharParser('&'),
                                    new Cm::Parsing::StringParser("&=")))),
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::DifferenceParser(
                                new Cm::Parsing::CharParser('+'),
                                new Cm::Parsing::StringParser("+=")))),
                    new Cm::Parsing::ActionParser("A3",
                        new Cm::Parsing::DifferenceParser(
                            new Cm::Parsing::CharParser('-'),
                            new Cm::Parsing::StringParser("-=")))),
                new Cm::Parsing::ActionParser("A4",
                    new Cm::Parsing::DifferenceParser(
                        new Cm::Parsing::CharParser('!'),
                        new Cm::Parsing::StringParser("!=")))),
            new Cm::Parsing::ActionParser("A5",
                new Cm::Parsing::CharParser('~')))));
    AddRule(new NewExpressionRule("NewExpression", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::OptionalParser(
                                new Cm::Parsing::ActionParser("A1",
                                    new Cm::Parsing::StringParser("::"))),
                            new Cm::Parsing::KeywordParser("new")),
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::NonterminalParser("NewPlacement", "NewPlacement", 0))),
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::NonterminalParser("NewTypeId", "NewTypeId", 0)),
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::CharParser('('),
                                new Cm::Parsing::ActionParser("A3",
                                    new Cm::Parsing::NonterminalParser("TypeId", "TypeId", 0))),
                            new Cm::Parsing::CharParser(')')))),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::NonterminalParser("NewInitializer", "NewInitializer", 0))))));
    AddRule(new NewTypeIdRule("NewTypeId", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::EmptyParser()),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::NonterminalParser("TypeSpecifierSeq", "TypeSpecifierSeq", 1),
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::NonterminalParser("TypeName", "TypeName", 0))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::ActionParser("A3",
                            new Cm::Parsing::NonterminalParser("NewDeclarator", "NewDeclarator", 0))))))));
    AddRule(new NewDeclaratorRule("NewDeclarator", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::NonterminalParser("PtrOperator", "PtrOperator", 0),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("NewDeclarator", "NewDeclarator", 0))),
                new Cm::Parsing::NonterminalParser("DirectNewDeclarator", "DirectNewDeclarator", 0)))));
    AddRule(new DirectNewDeclaratorRule("DirectNewDeclarator", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('['),
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::NonterminalParser("Expression", "Expression", 0))),
                new Cm::Parsing::CharParser(']')),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::CharParser('['),
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::NonterminalParser("ConstantExpression", "ConstantExpression", 0))),
                    new Cm::Parsing::CharParser(']'))))));
    AddRule(new NewPlacementRule("NewPlacement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('('),
                    new Cm::Parsing::NonterminalParser("ExpressionList", "ExpressionList", 0)),
                new Cm::Parsing::CharParser(')')))));
    AddRule(new NewInitializerRule("NewInitializer", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('('),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("ExpressionList", "ExpressionList", 0))),
                new Cm::Parsing::CharParser(')')))));
    AddRule(new DeleteExpressionRule("DeleteExpression", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::StringParser("::"))),
                        new Cm::Parsing::KeywordParser("delete")),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::CharParser('['),
                            new Cm::Parsing::ActionParser("A2",
                                new Cm::Parsing::CharParser(']'))))),
                new Cm::Parsing::NonterminalParser("CastExpression", "CastExpression", 0)))));
    AddRule(new PostfixExpressionRule("PostfixExpression", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("PrimaryExpression", "PrimaryExpression", 0)),
                    new Cm::Parsing::KleeneStarParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::AlternativeParser(
                                new Cm::Parsing::AlternativeParser(
                                    new Cm::Parsing::AlternativeParser(
                                        new Cm::Parsing::AlternativeParser(
                                            new Cm::Parsing::ActionParser("A2",
                                                new Cm::Parsing::SequenceParser(
                                                    new Cm::Parsing::SequenceParser(
                                                        new Cm::Parsing::CharParser('['),
                                                        new Cm::Parsing::ExpectationParser(
                                                            new Cm::Parsing::NonterminalParser("index", "Expression", 0))),
                                                    new Cm::Parsing::ExpectationParser(
                                                        new Cm::Parsing::CharParser(']')))),
                                            new Cm::Parsing::ActionParser("A3",
                                                new Cm::Parsing::SequenceParser(
                                                    new Cm::Parsing::SequenceParser(
                                                        new Cm::Parsing::CharParser('('),
                                                        new Cm::Parsing::OptionalParser(
                                                            new Cm::Parsing::NonterminalParser("invoke", "ExpressionList", 0))),
                                                    new Cm::Parsing::ExpectationParser(
                                                        new Cm::Parsing::CharParser(')'))))),
                                        new Cm::Parsing::ActionParser("A4",
                                            new Cm::Parsing::SequenceParser(
                                                new Cm::Parsing::DifferenceParser(
                                                    new Cm::Parsing::CharParser('.'),
                                                    new Cm::Parsing::StringParser(".*")),
                                                new Cm::Parsing::ExpectationParser(
                                                    new Cm::Parsing::NonterminalParser("member", "IdExpression", 0))))),
                                    new Cm::Parsing::ActionParser("A5",
                                        new Cm::Parsing::SequenceParser(
                                            new Cm::Parsing::DifferenceParser(
                                                new Cm::Parsing::StringParser("->"),
                                                new Cm::Parsing::StringParser("->*")),
                                            new Cm::Parsing::ExpectationParser(
                                                new Cm::Parsing::NonterminalParser("ptrmember", "IdExpression", 0))))),
                                new Cm::Parsing::ActionParser("A6",
                                    new Cm::Parsing::StringParser("++"))),
                            new Cm::Parsing::ActionParser("A7",
                                new Cm::Parsing::StringParser("--"))))),
                new Cm::Parsing::ActionParser("A8",
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::NonterminalParser("TypeSpecifierOrTypeName", "TypeSpecifierOrTypeName", 0),
                                new Cm::Parsing::CharParser('(')),
                            new Cm::Parsing::OptionalParser(
                                new Cm::Parsing::NonterminalParser("typeConstruction", "ExpressionList", 0))),
                        new Cm::Parsing::CharParser(')')))))));
    AddRule(new TypeSpecifierOrTypeNameRule("TypeSpecifierOrTypeName", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("SimpleTypeSpecifier", "SimpleTypeSpecifier", 0)),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("TypeName", "TypeName", 0)))));
    AddRule(new PostCastExpressionRule("PostCastExpression", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::ActionParser("A0",
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::SequenceParser(
                                            new Cm::Parsing::SequenceParser(
                                                new Cm::Parsing::SequenceParser(
                                                    new Cm::Parsing::KeywordParser("dynamic_cast"),
                                                    new Cm::Parsing::ExpectationParser(
                                                        new Cm::Parsing::CharParser('<'))),
                                                new Cm::Parsing::ExpectationParser(
                                                    new Cm::Parsing::NonterminalParser("dct", "TypeId", 0))),
                                            new Cm::Parsing::ExpectationParser(
                                                new Cm::Parsing::CharParser('>'))),
                                        new Cm::Parsing::ExpectationParser(
                                            new Cm::Parsing::CharParser('('))),
                                    new Cm::Parsing::ExpectationParser(
                                        new Cm::Parsing::NonterminalParser("dce", "Expression", 0))),
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::CharParser(')')))),
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::SequenceParser(
                                            new Cm::Parsing::SequenceParser(
                                                new Cm::Parsing::SequenceParser(
                                                    new Cm::Parsing::KeywordParser("static_cast"),
                                                    new Cm::Parsing::ExpectationParser(
                                                        new Cm::Parsing::CharParser('<'))),
                                                new Cm::Parsing::ExpectationParser(
                                                    new Cm::Parsing::NonterminalParser("sct", "TypeId", 0))),
                                            new Cm::Parsing::ExpectationParser(
                                                new Cm::Parsing::CharParser('>'))),
                                        new Cm::Parsing::ExpectationParser(
                                            new Cm::Parsing::CharParser('('))),
                                    new Cm::Parsing::ExpectationParser(
                                        new Cm::Parsing::NonterminalParser("sce", "Expression", 0))),
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::CharParser(')'))))),
                    new Cm::Parsing::ActionParser("A2",
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::SequenceParser(
                                            new Cm::Parsing::SequenceParser(
                                                new Cm::Parsing::KeywordParser("reinterpret_cast"),
                                                new Cm::Parsing::ExpectationParser(
                                                    new Cm::Parsing::CharParser('<'))),
                                            new Cm::Parsing::ExpectationParser(
                                                new Cm::Parsing::NonterminalParser("rct", "TypeId", 0))),
                                        new Cm::Parsing::ExpectationParser(
                                            new Cm::Parsing::CharParser('>'))),
                                    new Cm::Parsing::ExpectationParser(
                                        new Cm::Parsing::CharParser('('))),
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::NonterminalParser("rce", "Expression", 0))),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::CharParser(')'))))),
                new Cm::Parsing::ActionParser("A3",
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::SequenceParser(
                                            new Cm::Parsing::KeywordParser("const_cast"),
                                            new Cm::Parsing::ExpectationParser(
                                                new Cm::Parsing::CharParser('<'))),
                                        new Cm::Parsing::ExpectationParser(
                                            new Cm::Parsing::NonterminalParser("cct", "TypeId", 0))),
                                    new Cm::Parsing::ExpectationParser(
                                        new Cm::Parsing::CharParser('>'))),
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::CharParser('('))),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::NonterminalParser("cce", "Expression", 0))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser(')'))))),
            new Cm::Parsing::ActionParser("A4",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::KeywordParser("typeid"),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::CharParser('('))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("typeidExpr", "Expression", 0))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser(')')))))));
    AddRule(new ExpressionListRule("ExpressionList", GetScope(),
        new Cm::Parsing::ListParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("AssignmentExpression", "AssignmentExpression", 0)),
            new Cm::Parsing::CharParser(','))));
    AddRule(new PrimaryExpressionRule("PrimaryExpression", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::NonterminalParser("Literal", "Literal", 0)),
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::KeywordParser("this"))),
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::CharParser('('),
                            new Cm::Parsing::NonterminalParser("Expression", "Expression", 0)),
                        new Cm::Parsing::ActionParser("A3",
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::CharParser(')'))))),
                new Cm::Parsing::ActionParser("A4",
                    new Cm::Parsing::NonterminalParser("IdExpression", "IdExpression", 0))))));
    AddRule(new IdExpressionRule("IdExpression", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::TokenParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::NonterminalParser("id1", "QualifiedId", 0),
                                new Cm::Parsing::StringParser("::")),
                            new Cm::Parsing::NonterminalParser("OperatorFunctionId", "OperatorFunctionId", 0)))),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::TokenParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::OptionalParser(
                                new Cm::Parsing::StringParser("::")),
                            new Cm::Parsing::NonterminalParser("OperatorFunctionId", "OperatorFunctionId", 0))))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::NonterminalParser("id2", "QualifiedId", 0)))));
    AddRule(new Cm::Parsing::Rule("OperatorFunctionId", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::KeywordParser("operator"),
            new Cm::Parsing::NonterminalParser("Operator", "Operator", 0))));
    AddRule(new Cm::Parsing::Rule("Operator", GetScope(),
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
                                                                                                                                            new Cm::Parsing::AlternativeParser(
                                                                                                                                                new Cm::Parsing::AlternativeParser(
                                                                                                                                                    new Cm::Parsing::AlternativeParser(
                                                                                                                                                        new Cm::Parsing::AlternativeParser(
                                                                                                                                                            new Cm::Parsing::AlternativeParser(
                                                                                                                                                                new Cm::Parsing::AlternativeParser(
                                                                                                                                                                    new Cm::Parsing::AlternativeParser(
                                                                                                                                                                        new Cm::Parsing::AlternativeParser(
                                                                                                                                                                            new Cm::Parsing::SequenceParser(
                                                                                                                                                                                new Cm::Parsing::CharParser('['),
                                                                                                                                                                                new Cm::Parsing::CharParser(']')),
                                                                                                                                                                            new Cm::Parsing::SequenceParser(
                                                                                                                                                                                new Cm::Parsing::CharParser('('),
                                                                                                                                                                                new Cm::Parsing::CharParser(')'))),
                                                                                                                                                                        new Cm::Parsing::StringParser("->*")),
                                                                                                                                                                    new Cm::Parsing::StringParser("->")),
                                                                                                                                                                new Cm::Parsing::CharParser(',')),
                                                                                                                                                            new Cm::Parsing::StringParser("--")),
                                                                                                                                                        new Cm::Parsing::StringParser("++")),
                                                                                                                                                    new Cm::Parsing::StringParser("||")),
                                                                                                                                                new Cm::Parsing::StringParser("&&")),
                                                                                                                                            new Cm::Parsing::StringParser("<=")),
                                                                                                                                        new Cm::Parsing::StringParser(">=")),
                                                                                                                                    new Cm::Parsing::StringParser("!=")),
                                                                                                                                new Cm::Parsing::StringParser("==")),
                                                                                                                            new Cm::Parsing::StringParser("<<=")),
                                                                                                                        new Cm::Parsing::StringParser(">>=")),
                                                                                                                    new Cm::Parsing::StringParser("<<")),
                                                                                                                new Cm::Parsing::StringParser(">>")),
                                                                                                            new Cm::Parsing::StringParser("|=")),
                                                                                                        new Cm::Parsing::StringParser("&=")),
                                                                                                    new Cm::Parsing::StringParser("^=")),
                                                                                                new Cm::Parsing::StringParser("%=")),
                                                                                            new Cm::Parsing::StringParser("/=")),
                                                                                        new Cm::Parsing::StringParser("*=")),
                                                                                    new Cm::Parsing::StringParser("-=")),
                                                                                new Cm::Parsing::StringParser("+=")),
                                                                            new Cm::Parsing::CharParser('<')),
                                                                        new Cm::Parsing::CharParser('>')),
                                                                    new Cm::Parsing::CharParser('=')),
                                                                new Cm::Parsing::CharParser('!')),
                                                            new Cm::Parsing::CharParser('~')),
                                                        new Cm::Parsing::CharParser('|')),
                                                    new Cm::Parsing::CharParser('&')),
                                                new Cm::Parsing::CharParser('^')),
                                            new Cm::Parsing::CharParser('%')),
                                        new Cm::Parsing::CharParser('/')),
                                    new Cm::Parsing::CharParser('*')),
                                new Cm::Parsing::CharParser('-')),
                            new Cm::Parsing::CharParser('+')),
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::KeywordParser("new"),
                                new Cm::Parsing::CharParser('[')),
                            new Cm::Parsing::CharParser(']'))),
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::KeywordParser("delete"),
                            new Cm::Parsing::CharParser('[')),
                        new Cm::Parsing::CharParser(']'))),
                new Cm::Parsing::KeywordParser("new")),
            new Cm::Parsing::KeywordParser("delete"))));
}

} } } // namespace Cm.Parsing.Cpp
