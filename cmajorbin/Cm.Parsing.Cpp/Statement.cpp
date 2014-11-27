#include "Statement.hpp"
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
#include <Cm.Parsing.Cpp/Declaration.hpp>
#include <Cm.Parsing.Cpp/Declarator.hpp>
#include <Cm.Parsing.Cpp/Identifier.hpp>
#include <Cm.Parsing.CppObjectModel/Type.hpp>

namespace Cm { namespace Parsing { namespace Cpp {

using namespace Cm::Parsing::CppObjectModel;
using namespace Cm::Parsing;

StatementGrammar* StatementGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

StatementGrammar* StatementGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    StatementGrammar* grammar(new StatementGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

StatementGrammar::StatementGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("StatementGrammar", parsingDomain_->GetNamespaceScope("Cm.Parsing.Cpp"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Parsing::CppObjectModel::CompoundStatement* StatementGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
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
    Cm::Parsing::CppObjectModel::CompoundStatement* result = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CompoundStatement*>*>(value.get());
    stack.pop();
    return result;
}

class StatementGrammar::StatementRule : public Cm::Parsing::Rule
{
public:
    StatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Statement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A5Action));
        Cm::Parsing::ActionParser* a6ActionParser = GetAction("A6");
        a6ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A6Action));
        Cm::Parsing::ActionParser* a7ActionParser = GetAction("A7");
        a7ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A7Action));
        Cm::Parsing::ActionParser* a8ActionParser = GetAction("A8");
        a8ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A8Action));
        Cm::Parsing::NonterminalParser* labeledStatementNonterminalParser = GetNonterminal("LabeledStatement");
        labeledStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostLabeledStatement));
        Cm::Parsing::NonterminalParser* emptyStatementNonterminalParser = GetNonterminal("EmptyStatement");
        emptyStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostEmptyStatement));
        Cm::Parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal("CompoundStatement");
        compoundStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostCompoundStatement));
        Cm::Parsing::NonterminalParser* selectionStatementNonterminalParser = GetNonterminal("SelectionStatement");
        selectionStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostSelectionStatement));
        Cm::Parsing::NonterminalParser* iterationStatementNonterminalParser = GetNonterminal("IterationStatement");
        iterationStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostIterationStatement));
        Cm::Parsing::NonterminalParser* jumpStatementNonterminalParser = GetNonterminal("JumpStatement");
        jumpStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostJumpStatement));
        Cm::Parsing::NonterminalParser* declarationStatementNonterminalParser = GetNonterminal("DeclarationStatement");
        declarationStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostDeclarationStatement));
        Cm::Parsing::NonterminalParser* tryStatementNonterminalParser = GetNonterminal("TryStatement");
        tryStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostTryStatement));
        Cm::Parsing::NonterminalParser* expressionStatementNonterminalParser = GetNonterminal("ExpressionStatement");
        expressionStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostExpressionStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromLabeledStatement;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromEmptyStatement;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromCompoundStatement;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromSelectionStatement;
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromIterationStatement;
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromJumpStatement;
    }
    void A6Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromDeclarationStatement;
    }
    void A7Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromTryStatement;
    }
    void A8Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromExpressionStatement;
    }
    void PostLabeledStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromLabeledStatement_value = std::move(stack.top());
            context.fromLabeledStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromLabeledStatement_value.get());
            stack.pop();
        }
    }
    void PostEmptyStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromEmptyStatement_value = std::move(stack.top());
            context.fromEmptyStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromEmptyStatement_value.get());
            stack.pop();
        }
    }
    void PostCompoundStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context.fromCompoundStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CompoundStatement*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
    void PostSelectionStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromSelectionStatement_value = std::move(stack.top());
            context.fromSelectionStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromSelectionStatement_value.get());
            stack.pop();
        }
    }
    void PostIterationStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIterationStatement_value = std::move(stack.top());
            context.fromIterationStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromIterationStatement_value.get());
            stack.pop();
        }
    }
    void PostJumpStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromJumpStatement_value = std::move(stack.top());
            context.fromJumpStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromJumpStatement_value.get());
            stack.pop();
        }
    }
    void PostDeclarationStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDeclarationStatement_value = std::move(stack.top());
            context.fromDeclarationStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromDeclarationStatement_value.get());
            stack.pop();
        }
    }
    void PostTryStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTryStatement_value = std::move(stack.top());
            context.fromTryStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TryStatement*>*>(fromTryStatement_value.get());
            stack.pop();
        }
    }
    void PostExpressionStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromExpressionStatement_value = std::move(stack.top());
            context.fromExpressionStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromExpressionStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromLabeledStatement(), fromEmptyStatement(), fromCompoundStatement(), fromSelectionStatement(), fromIterationStatement(), fromJumpStatement(), fromDeclarationStatement(), fromTryStatement(), fromExpressionStatement() {}
        Cm::Parsing::CppObjectModel::Statement* value;
        Cm::Parsing::CppObjectModel::Statement* fromLabeledStatement;
        Cm::Parsing::CppObjectModel::Statement* fromEmptyStatement;
        Cm::Parsing::CppObjectModel::CompoundStatement* fromCompoundStatement;
        Cm::Parsing::CppObjectModel::Statement* fromSelectionStatement;
        Cm::Parsing::CppObjectModel::Statement* fromIterationStatement;
        Cm::Parsing::CppObjectModel::Statement* fromJumpStatement;
        Cm::Parsing::CppObjectModel::Statement* fromDeclarationStatement;
        Cm::Parsing::CppObjectModel::TryStatement* fromTryStatement;
        Cm::Parsing::CppObjectModel::Statement* fromExpressionStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::LabeledStatementRule : public Cm::Parsing::Rule
{
public:
    LabeledStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Statement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LabeledStatementRule>(this, &LabeledStatementRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LabeledStatementRule>(this, &LabeledStatementRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LabeledStatementRule>(this, &LabeledStatementRule::A2Action));
        Cm::Parsing::NonterminalParser* labelNonterminalParser = GetNonterminal("Label");
        labelNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::PostLabel));
        Cm::Parsing::NonterminalParser* s1NonterminalParser = GetNonterminal("s1");
        s1NonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::Posts1));
        Cm::Parsing::NonterminalParser* constantExpressionNonterminalParser = GetNonterminal("ConstantExpression");
        constantExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::PostConstantExpression));
        Cm::Parsing::NonterminalParser* s2NonterminalParser = GetNonterminal("s2");
        s2NonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::Posts2));
        Cm::Parsing::NonterminalParser* s3NonterminalParser = GetNonterminal("s3");
        s3NonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::Posts3));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new LabeledStatement(context.fromLabel, context.froms1);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new CaseStatement(context.fromConstantExpression, context.froms2);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new DefaultStatement(context.froms3);
    }
    void PostLabel(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromLabel_value = std::move(stack.top());
            context.fromLabel = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromLabel_value.get());
            stack.pop();
        }
    }
    void Posts1(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> froms1_value = std::move(stack.top());
            context.froms1 = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(froms1_value.get());
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
    void Posts2(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> froms2_value = std::move(stack.top());
            context.froms2 = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(froms2_value.get());
            stack.pop();
        }
    }
    void Posts3(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> froms3_value = std::move(stack.top());
            context.froms3 = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(froms3_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromLabel(), froms1(), fromConstantExpression(), froms2(), froms3() {}
        Cm::Parsing::CppObjectModel::Statement* value;
        std::string fromLabel;
        Cm::Parsing::CppObjectModel::Statement* froms1;
        Cm::Parsing::CppObjectModel::CppObject* fromConstantExpression;
        Cm::Parsing::CppObjectModel::Statement* froms2;
        Cm::Parsing::CppObjectModel::Statement* froms3;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::LabelRule : public Cm::Parsing::Rule
{
public:
    LabelRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LabelRule>(this, &LabelRule::A0Action));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LabelRule>(this, &LabelRule::PostIdentifier));
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

class StatementGrammar::EmptyStatementRule : public Cm::Parsing::Rule
{
public:
    EmptyStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Statement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<EmptyStatementRule>(this, &EmptyStatementRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new EmptyStatement;
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::CppObjectModel::Statement* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ExpressionStatementRule : public Cm::Parsing::Rule
{
public:
    ExpressionStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Statement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ExpressionStatementRule>(this, &ExpressionStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ExpressionStatementRule>(this, &ExpressionStatementRule::PostExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ExpressionStatement(context.fromExpression);
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
        Cm::Parsing::CppObjectModel::Statement* value;
        Cm::Parsing::CppObjectModel::CppObject* fromExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::CompoundStatementRule : public Cm::Parsing::Rule
{
public:
    CompoundStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CompoundStatement*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<CompoundStatement>", "cs"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CompoundStatement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CompoundStatementRule>(this, &CompoundStatementRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CompoundStatementRule>(this, &CompoundStatementRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CompoundStatementRule>(this, &CompoundStatementRule::A2Action));
        Cm::Parsing::NonterminalParser* statementNonterminalParser = GetNonterminal("Statement");
        statementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CompoundStatementRule>(this, &CompoundStatementRule::PostStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.cs.reset(new CompoundStatement);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.cs->Add(context.fromStatement);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.cs.release();
    }
    void PostStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStatement_value = std::move(stack.top());
            context.fromStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), cs(), fromStatement() {}
        Cm::Parsing::CppObjectModel::CompoundStatement* value;
        std::unique_ptr<CompoundStatement> cs;
        Cm::Parsing::CppObjectModel::Statement* fromStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::SelectionStatementRule : public Cm::Parsing::Rule
{
public:
    SelectionStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Statement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SelectionStatementRule>(this, &SelectionStatementRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SelectionStatementRule>(this, &SelectionStatementRule::A1Action));
        Cm::Parsing::NonterminalParser* ifStatementNonterminalParser = GetNonterminal("IfStatement");
        ifStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SelectionStatementRule>(this, &SelectionStatementRule::PostIfStatement));
        Cm::Parsing::NonterminalParser* switchStatementNonterminalParser = GetNonterminal("SwitchStatement");
        switchStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SelectionStatementRule>(this, &SelectionStatementRule::PostSwitchStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromIfStatement;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromSwitchStatement;
    }
    void PostIfStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIfStatement_value = std::move(stack.top());
            context.fromIfStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromIfStatement_value.get());
            stack.pop();
        }
    }
    void PostSwitchStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromSwitchStatement_value = std::move(stack.top());
            context.fromSwitchStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromSwitchStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromIfStatement(), fromSwitchStatement() {}
        Cm::Parsing::CppObjectModel::Statement* value;
        Cm::Parsing::CppObjectModel::Statement* fromIfStatement;
        Cm::Parsing::CppObjectModel::Statement* fromSwitchStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::IfStatementRule : public Cm::Parsing::Rule
{
public:
    IfStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Statement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IfStatementRule>(this, &IfStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* conditionNonterminalParser = GetNonterminal("Condition");
        conditionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IfStatementRule>(this, &IfStatementRule::PostCondition));
        Cm::Parsing::NonterminalParser* thenStatementNonterminalParser = GetNonterminal("thenStatement");
        thenStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IfStatementRule>(this, &IfStatementRule::PostthenStatement));
        Cm::Parsing::NonterminalParser* elseStatementNonterminalParser = GetNonterminal("elseStatement");
        elseStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IfStatementRule>(this, &IfStatementRule::PostelseStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new IfStatement(context.fromCondition, context.fromthenStatement, context.fromelseStatement);
    }
    void PostCondition(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCondition_value = std::move(stack.top());
            context.fromCondition = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromCondition_value.get());
            stack.pop();
        }
    }
    void PostthenStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromthenStatement_value = std::move(stack.top());
            context.fromthenStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromthenStatement_value.get());
            stack.pop();
        }
    }
    void PostelseStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromelseStatement_value = std::move(stack.top());
            context.fromelseStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromelseStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromCondition(), fromthenStatement(), fromelseStatement() {}
        Cm::Parsing::CppObjectModel::Statement* value;
        Cm::Parsing::CppObjectModel::CppObject* fromCondition;
        Cm::Parsing::CppObjectModel::Statement* fromthenStatement;
        Cm::Parsing::CppObjectModel::Statement* fromelseStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::SwitchStatementRule : public Cm::Parsing::Rule
{
public:
    SwitchStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Statement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SwitchStatementRule>(this, &SwitchStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* conditionNonterminalParser = GetNonterminal("Condition");
        conditionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SwitchStatementRule>(this, &SwitchStatementRule::PostCondition));
        Cm::Parsing::NonterminalParser* statementNonterminalParser = GetNonterminal("Statement");
        statementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SwitchStatementRule>(this, &SwitchStatementRule::PostStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new SwitchStatement(context.fromCondition, context.fromStatement);
    }
    void PostCondition(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCondition_value = std::move(stack.top());
            context.fromCondition = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromCondition_value.get());
            stack.pop();
        }
    }
    void PostStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStatement_value = std::move(stack.top());
            context.fromStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromCondition(), fromStatement() {}
        Cm::Parsing::CppObjectModel::Statement* value;
        Cm::Parsing::CppObjectModel::CppObject* fromCondition;
        Cm::Parsing::CppObjectModel::Statement* fromStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::IterationStatementRule : public Cm::Parsing::Rule
{
public:
    IterationStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Statement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IterationStatementRule>(this, &IterationStatementRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IterationStatementRule>(this, &IterationStatementRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IterationStatementRule>(this, &IterationStatementRule::A2Action));
        Cm::Parsing::NonterminalParser* whileStatementNonterminalParser = GetNonterminal("WhileStatement");
        whileStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::PostWhileStatement));
        Cm::Parsing::NonterminalParser* doStatementNonterminalParser = GetNonterminal("DoStatement");
        doStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::PostDoStatement));
        Cm::Parsing::NonterminalParser* forStatementNonterminalParser = GetNonterminal("ForStatement");
        forStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::PostForStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromWhileStatement;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromDoStatement;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromForStatement;
    }
    void PostWhileStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromWhileStatement_value = std::move(stack.top());
            context.fromWhileStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromWhileStatement_value.get());
            stack.pop();
        }
    }
    void PostDoStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDoStatement_value = std::move(stack.top());
            context.fromDoStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromDoStatement_value.get());
            stack.pop();
        }
    }
    void PostForStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromForStatement_value = std::move(stack.top());
            context.fromForStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromForStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromWhileStatement(), fromDoStatement(), fromForStatement() {}
        Cm::Parsing::CppObjectModel::Statement* value;
        Cm::Parsing::CppObjectModel::Statement* fromWhileStatement;
        Cm::Parsing::CppObjectModel::Statement* fromDoStatement;
        Cm::Parsing::CppObjectModel::Statement* fromForStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::WhileStatementRule : public Cm::Parsing::Rule
{
public:
    WhileStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Statement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<WhileStatementRule>(this, &WhileStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* conditionNonterminalParser = GetNonterminal("Condition");
        conditionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<WhileStatementRule>(this, &WhileStatementRule::PostCondition));
        Cm::Parsing::NonterminalParser* statementNonterminalParser = GetNonterminal("Statement");
        statementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<WhileStatementRule>(this, &WhileStatementRule::PostStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new WhileStatement(context.fromCondition, context.fromStatement);
    }
    void PostCondition(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCondition_value = std::move(stack.top());
            context.fromCondition = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromCondition_value.get());
            stack.pop();
        }
    }
    void PostStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStatement_value = std::move(stack.top());
            context.fromStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromCondition(), fromStatement() {}
        Cm::Parsing::CppObjectModel::Statement* value;
        Cm::Parsing::CppObjectModel::CppObject* fromCondition;
        Cm::Parsing::CppObjectModel::Statement* fromStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::DoStatementRule : public Cm::Parsing::Rule
{
public:
    DoStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Statement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DoStatementRule>(this, &DoStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* statementNonterminalParser = GetNonterminal("Statement");
        statementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DoStatementRule>(this, &DoStatementRule::PostStatement));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DoStatementRule>(this, &DoStatementRule::PostExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new DoStatement(context.fromStatement, context.fromExpression);
    }
    void PostStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStatement_value = std::move(stack.top());
            context.fromStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromStatement_value.get());
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
private:
    struct Context
    {
        Context(): value(), fromStatement(), fromExpression() {}
        Cm::Parsing::CppObjectModel::Statement* value;
        Cm::Parsing::CppObjectModel::Statement* fromStatement;
        Cm::Parsing::CppObjectModel::CppObject* fromExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ForStatementRule : public Cm::Parsing::Rule
{
public:
    ForStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Statement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ForStatementRule>(this, &ForStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* forInitStatementNonterminalParser = GetNonterminal("ForInitStatement");
        forInitStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ForStatementRule>(this, &ForStatementRule::PostForInitStatement));
        Cm::Parsing::NonterminalParser* conditionNonterminalParser = GetNonterminal("Condition");
        conditionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ForStatementRule>(this, &ForStatementRule::PostCondition));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ForStatementRule>(this, &ForStatementRule::PostExpression));
        Cm::Parsing::NonterminalParser* statementNonterminalParser = GetNonterminal("Statement");
        statementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ForStatementRule>(this, &ForStatementRule::PostStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ForStatement(context.fromForInitStatement, context.fromCondition, context.fromExpression, context.fromStatement);
    }
    void PostForInitStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromForInitStatement_value = std::move(stack.top());
            context.fromForInitStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromForInitStatement_value.get());
            stack.pop();
        }
    }
    void PostCondition(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCondition_value = std::move(stack.top());
            context.fromCondition = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromCondition_value.get());
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
    void PostStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStatement_value = std::move(stack.top());
            context.fromStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromForInitStatement(), fromCondition(), fromExpression(), fromStatement() {}
        Cm::Parsing::CppObjectModel::Statement* value;
        Cm::Parsing::CppObjectModel::CppObject* fromForInitStatement;
        Cm::Parsing::CppObjectModel::CppObject* fromCondition;
        Cm::Parsing::CppObjectModel::CppObject* fromExpression;
        Cm::Parsing::CppObjectModel::Statement* fromStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ForInitStatementRule : public Cm::Parsing::Rule
{
public:
    ForInitStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ForInitStatementRule>(this, &ForInitStatementRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ForInitStatementRule>(this, &ForInitStatementRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ForInitStatementRule>(this, &ForInitStatementRule::A2Action));
        Cm::Parsing::NonterminalParser* emptyStatementNonterminalParser = GetNonterminal("EmptyStatement");
        emptyStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ForInitStatementRule>(this, &ForInitStatementRule::PostEmptyStatement));
        Cm::Parsing::NonterminalParser* expressionStatementNonterminalParser = GetNonterminal("ExpressionStatement");
        expressionStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ForInitStatementRule>(this, &ForInitStatementRule::PostExpressionStatement));
        Cm::Parsing::NonterminalParser* simpleDeclarationNonterminalParser = GetNonterminal("SimpleDeclaration");
        simpleDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ForInitStatementRule>(this, &ForInitStatementRule::PostSimpleDeclaration));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromEmptyStatement;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromExpressionStatement;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromSimpleDeclaration;
    }
    void PostEmptyStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromEmptyStatement_value = std::move(stack.top());
            context.fromEmptyStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromEmptyStatement_value.get());
            stack.pop();
        }
    }
    void PostExpressionStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromExpressionStatement_value = std::move(stack.top());
            context.fromExpressionStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromExpressionStatement_value.get());
            stack.pop();
        }
    }
    void PostSimpleDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromSimpleDeclaration_value = std::move(stack.top());
            context.fromSimpleDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::SimpleDeclaration*>*>(fromSimpleDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromEmptyStatement(), fromExpressionStatement(), fromSimpleDeclaration() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::Statement* fromEmptyStatement;
        Cm::Parsing::CppObjectModel::Statement* fromExpressionStatement;
        Cm::Parsing::CppObjectModel::SimpleDeclaration* fromSimpleDeclaration;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::JumpStatementRule : public Cm::Parsing::Rule
{
public:
    JumpStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Statement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<JumpStatementRule>(this, &JumpStatementRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<JumpStatementRule>(this, &JumpStatementRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<JumpStatementRule>(this, &JumpStatementRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<JumpStatementRule>(this, &JumpStatementRule::A3Action));
        Cm::Parsing::NonterminalParser* breakStatementNonterminalParser = GetNonterminal("BreakStatement");
        breakStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<JumpStatementRule>(this, &JumpStatementRule::PostBreakStatement));
        Cm::Parsing::NonterminalParser* continueStatementNonterminalParser = GetNonterminal("ContinueStatement");
        continueStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<JumpStatementRule>(this, &JumpStatementRule::PostContinueStatement));
        Cm::Parsing::NonterminalParser* returnStatementNonterminalParser = GetNonterminal("ReturnStatement");
        returnStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<JumpStatementRule>(this, &JumpStatementRule::PostReturnStatement));
        Cm::Parsing::NonterminalParser* gotoStatementNonterminalParser = GetNonterminal("GotoStatement");
        gotoStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<JumpStatementRule>(this, &JumpStatementRule::PostGotoStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromBreakStatement;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromContinueStatement;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromReturnStatement;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromGotoStatement;
    }
    void PostBreakStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromBreakStatement_value = std::move(stack.top());
            context.fromBreakStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromBreakStatement_value.get());
            stack.pop();
        }
    }
    void PostContinueStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromContinueStatement_value = std::move(stack.top());
            context.fromContinueStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromContinueStatement_value.get());
            stack.pop();
        }
    }
    void PostReturnStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromReturnStatement_value = std::move(stack.top());
            context.fromReturnStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromReturnStatement_value.get());
            stack.pop();
        }
    }
    void PostGotoStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromGotoStatement_value = std::move(stack.top());
            context.fromGotoStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>*>(fromGotoStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromBreakStatement(), fromContinueStatement(), fromReturnStatement(), fromGotoStatement() {}
        Cm::Parsing::CppObjectModel::Statement* value;
        Cm::Parsing::CppObjectModel::Statement* fromBreakStatement;
        Cm::Parsing::CppObjectModel::Statement* fromContinueStatement;
        Cm::Parsing::CppObjectModel::Statement* fromReturnStatement;
        Cm::Parsing::CppObjectModel::Statement* fromGotoStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::BreakStatementRule : public Cm::Parsing::Rule
{
public:
    BreakStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Statement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<BreakStatementRule>(this, &BreakStatementRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new BreakStatement;
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::CppObjectModel::Statement* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ContinueStatementRule : public Cm::Parsing::Rule
{
public:
    ContinueStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Statement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ContinueStatementRule>(this, &ContinueStatementRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ContinueStatement;
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::CppObjectModel::Statement* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ReturnStatementRule : public Cm::Parsing::Rule
{
public:
    ReturnStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Statement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ReturnStatementRule>(this, &ReturnStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ReturnStatementRule>(this, &ReturnStatementRule::PostExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ReturnStatement(context.fromExpression);
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
        Cm::Parsing::CppObjectModel::Statement* value;
        Cm::Parsing::CppObjectModel::CppObject* fromExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::GotoStatementRule : public Cm::Parsing::Rule
{
public:
    GotoStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Statement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<GotoStatementRule>(this, &GotoStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* gotoTargetNonterminalParser = GetNonterminal("GotoTarget");
        gotoTargetNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<GotoStatementRule>(this, &GotoStatementRule::PostGotoTarget));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new GotoStatement(context.fromGotoTarget);
    }
    void PostGotoTarget(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromGotoTarget_value = std::move(stack.top());
            context.fromGotoTarget = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromGotoTarget_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromGotoTarget() {}
        Cm::Parsing::CppObjectModel::Statement* value;
        std::string fromGotoTarget;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::GotoTargetRule : public Cm::Parsing::Rule
{
public:
    GotoTargetRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<GotoTargetRule>(this, &GotoTargetRule::A0Action));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<GotoTargetRule>(this, &GotoTargetRule::PostIdentifier));
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

class StatementGrammar::DeclarationStatementRule : public Cm::Parsing::Rule
{
public:
    DeclarationStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Statement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Statement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclarationStatementRule>(this, &DeclarationStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* blockDeclarationNonterminalParser = GetNonterminal("BlockDeclaration");
        blockDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclarationStatementRule>(this, &DeclarationStatementRule::PostBlockDeclaration));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new DeclarationStatement(context.fromBlockDeclaration);
    }
    void PostBlockDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromBlockDeclaration_value = std::move(stack.top());
            context.fromBlockDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromBlockDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromBlockDeclaration() {}
        Cm::Parsing::CppObjectModel::Statement* value;
        Cm::Parsing::CppObjectModel::CppObject* fromBlockDeclaration;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ConditionRule : public Cm::Parsing::Rule
{
public:
    ConditionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionRule>(this, &ConditionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionRule>(this, &ConditionRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionRule>(this, &ConditionRule::A2Action));
        Cm::Parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal("TypeId");
        typeIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionRule>(this, &ConditionRule::PostTypeId));
        Cm::Parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal("Declarator");
        declaratorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionRule>(this, &ConditionRule::PostDeclarator));
        Cm::Parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal("AssignmentExpression");
        assignmentExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionRule>(this, &ConditionRule::PostAssignmentExpression));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionRule>(this, &ConditionRule::PostExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ti.reset(context.fromTypeId);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ConditionWithDeclarator(context.ti.release(), context.fromDeclarator, context.fromAssignmentExpression);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromExpression;
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
    void PostDeclarator(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDeclarator_value = std::move(stack.top());
            context.fromDeclarator = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromDeclarator_value.get());
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
        Context(): value(), ti(), fromTypeId(), fromDeclarator(), fromAssignmentExpression(), fromExpression() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        std::unique_ptr<TypeId> ti;
        Cm::Parsing::CppObjectModel::TypeId* fromTypeId;
        std::string fromDeclarator;
        Cm::Parsing::CppObjectModel::CppObject* fromAssignmentExpression;
        Cm::Parsing::CppObjectModel::CppObject* fromExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::TryStatementRule : public Cm::Parsing::Rule
{
public:
    TryStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::TryStatement*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TryStatement*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TryStatementRule>(this, &TryStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal("CompoundStatement");
        compoundStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TryStatementRule>(this, &TryStatementRule::PostCompoundStatement));
        Cm::Parsing::NonterminalParser* handlerSeqNonterminalParser = GetNonterminal("HandlerSeq");
        handlerSeqNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<TryStatementRule>(this, &TryStatementRule::PreHandlerSeq));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new TryStatement(context.fromCompoundStatement);
    }
    void PostCompoundStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context.fromCompoundStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CompoundStatement*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
    void PreHandlerSeq(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<TryStatement*>(context.value)));
    }
private:
    struct Context
    {
        Context(): value(), fromCompoundStatement() {}
        Cm::Parsing::CppObjectModel::TryStatement* value;
        Cm::Parsing::CppObjectModel::CompoundStatement* fromCompoundStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::HandlerSeqRule : public Cm::Parsing::Rule
{
public:
    HandlerSeqRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("TryStatement*", "st"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> st_value = std::move(stack.top());
        context.st = *static_cast<Cm::Parsing::ValueObject<TryStatement*>*>(st_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<HandlerSeqRule>(this, &HandlerSeqRule::A0Action));
        Cm::Parsing::NonterminalParser* handlerNonterminalParser = GetNonterminal("Handler");
        handlerNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<HandlerSeqRule>(this, &HandlerSeqRule::PostHandler));
        Cm::Parsing::NonterminalParser* handlerSeqNonterminalParser = GetNonterminal("HandlerSeq");
        handlerSeqNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<HandlerSeqRule>(this, &HandlerSeqRule::PreHandlerSeq));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.st->Add(context.fromHandler);
    }
    void PostHandler(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromHandler_value = std::move(stack.top());
            context.fromHandler = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Handler*>*>(fromHandler_value.get());
            stack.pop();
        }
    }
    void PreHandlerSeq(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<TryStatement*>(context.st)));
    }
private:
    struct Context
    {
        Context(): st(), fromHandler() {}
        TryStatement* st;
        Cm::Parsing::CppObjectModel::Handler* fromHandler;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::HandlerRule : public Cm::Parsing::Rule
{
public:
    HandlerRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Handler*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Handler*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<HandlerRule>(this, &HandlerRule::A0Action));
        Cm::Parsing::NonterminalParser* exceptionDeclarationNonterminalParser = GetNonterminal("ExceptionDeclaration");
        exceptionDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<HandlerRule>(this, &HandlerRule::PostExceptionDeclaration));
        Cm::Parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal("CompoundStatement");
        compoundStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<HandlerRule>(this, &HandlerRule::PostCompoundStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Handler(context.fromExceptionDeclaration, context.fromCompoundStatement);
    }
    void PostExceptionDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromExceptionDeclaration_value = std::move(stack.top());
            context.fromExceptionDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::ExceptionDeclaration*>*>(fromExceptionDeclaration_value.get());
            stack.pop();
        }
    }
    void PostCompoundStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context.fromCompoundStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CompoundStatement*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromExceptionDeclaration(), fromCompoundStatement() {}
        Cm::Parsing::CppObjectModel::Handler* value;
        Cm::Parsing::CppObjectModel::ExceptionDeclaration* fromExceptionDeclaration;
        Cm::Parsing::CppObjectModel::CompoundStatement* fromCompoundStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ExceptionDeclarationRule : public Cm::Parsing::Rule
{
public:
    ExceptionDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::ExceptionDeclaration*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<ExceptionDeclaration>", "ed"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::ExceptionDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::A4Action));
        Cm::Parsing::NonterminalParser* typeSpecifierSeqNonterminalParser = GetNonterminal("TypeSpecifierSeq");
        typeSpecifierSeqNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::PreTypeSpecifierSeq));
        Cm::Parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal("Declarator");
        declaratorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::PostDeclarator));
        Cm::Parsing::NonterminalParser* abstractDeclaratorNonterminalParser = GetNonterminal("AbstractDeclarator");
        abstractDeclaratorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::PostAbstractDeclarator));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ed.reset(new ExceptionDeclaration);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.ed.release();
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ed->GetTypeId()->Declarator() = context.fromDeclarator;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ed->GetTypeId()->Declarator() = context.fromAbstractDeclarator;
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->CatchAll() = true;
    }
    void PreTypeSpecifierSeq(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>(context.ed->GetTypeId())));
    }
    void PostDeclarator(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDeclarator_value = std::move(stack.top());
            context.fromDeclarator = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
    void PostAbstractDeclarator(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAbstractDeclarator_value = std::move(stack.top());
            context.fromAbstractDeclarator = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromAbstractDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), ed(), fromDeclarator(), fromAbstractDeclarator() {}
        Cm::Parsing::CppObjectModel::ExceptionDeclaration* value;
        std::unique_ptr<ExceptionDeclaration> ed;
        std::string fromDeclarator;
        std::string fromAbstractDeclarator;
    };
    std::stack<Context> contextStack;
    Context context;
};

void StatementGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.Cpp.DeclarationGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::Cpp::DeclarationGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parsing.Cpp.IdentifierGrammar");
    if (!grammar1)
    {
        grammar1 = Cm::Parsing::Cpp::IdentifierGrammar::Create(pd);
    }
    AddGrammarReference(grammar1);
    Cm::Parsing::Grammar* grammar2 = pd->GetGrammar("Cm.Parsing.Cpp.DeclaratorGrammar");
    if (!grammar2)
    {
        grammar2 = Cm::Parsing::Cpp::DeclaratorGrammar::Create(pd);
    }
    AddGrammarReference(grammar2);
    Cm::Parsing::Grammar* grammar3 = pd->GetGrammar("Cm.Parsing.Cpp.ExpressionGrammar");
    if (!grammar3)
    {
        grammar3 = Cm::Parsing::Cpp::ExpressionGrammar::Create(pd);
    }
    AddGrammarReference(grammar3);
    Cm::Parsing::Grammar* grammar4 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar4)
    {
        grammar4 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar4);
}

void StatementGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("BlockDeclaration", this, "DeclarationGrammar.BlockDeclaration"));
    AddRuleLink(new Cm::Parsing::RuleLink("SimpleDeclaration", this, "DeclarationGrammar.SimpleDeclaration"));
    AddRuleLink(new Cm::Parsing::RuleLink("Identifier", this, "IdentifierGrammar.Identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("Expression", this, "ExpressionGrammar.Expression"));
    AddRuleLink(new Cm::Parsing::RuleLink("ConstantExpression", this, "ExpressionGrammar.ConstantExpression"));
    AddRuleLink(new Cm::Parsing::RuleLink("Declarator", this, "DeclaratorGrammar.Declarator"));
    AddRuleLink(new Cm::Parsing::RuleLink("AssignmentExpression", this, "ExpressionGrammar.AssignmentExpression"));
    AddRuleLink(new Cm::Parsing::RuleLink("TypeId", this, "DeclaratorGrammar.TypeId"));
    AddRuleLink(new Cm::Parsing::RuleLink("TypeSpecifierSeq", this, "DeclaratorGrammar.TypeSpecifierSeq"));
    AddRuleLink(new Cm::Parsing::RuleLink("AbstractDeclarator", this, "DeclaratorGrammar.AbstractDeclarator"));
    AddRuleLink(new Cm::Parsing::RuleLink("spaces_and_comments", this, "Cm.Parsing.stdlib.spaces_and_comments"));
    AddRule(new StatementRule("Statement", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::AlternativeParser(
                                new Cm::Parsing::AlternativeParser(
                                    new Cm::Parsing::AlternativeParser(
                                        new Cm::Parsing::ActionParser("A0",
                                            new Cm::Parsing::NonterminalParser("LabeledStatement", "LabeledStatement", 0)),
                                        new Cm::Parsing::ActionParser("A1",
                                            new Cm::Parsing::NonterminalParser("EmptyStatement", "EmptyStatement", 0))),
                                    new Cm::Parsing::ActionParser("A2",
                                        new Cm::Parsing::NonterminalParser("CompoundStatement", "CompoundStatement", 0))),
                                new Cm::Parsing::ActionParser("A3",
                                    new Cm::Parsing::NonterminalParser("SelectionStatement", "SelectionStatement", 0))),
                            new Cm::Parsing::ActionParser("A4",
                                new Cm::Parsing::NonterminalParser("IterationStatement", "IterationStatement", 0))),
                        new Cm::Parsing::ActionParser("A5",
                            new Cm::Parsing::NonterminalParser("JumpStatement", "JumpStatement", 0))),
                    new Cm::Parsing::ActionParser("A6",
                        new Cm::Parsing::NonterminalParser("DeclarationStatement", "DeclarationStatement", 0))),
                new Cm::Parsing::ActionParser("A7",
                    new Cm::Parsing::NonterminalParser("TryStatement", "TryStatement", 0))),
            new Cm::Parsing::ActionParser("A8",
                new Cm::Parsing::NonterminalParser("ExpressionStatement", "ExpressionStatement", 0)))));
    AddRule(new LabeledStatementRule("LabeledStatement", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::NonterminalParser("Label", "Label", 0),
                            new Cm::Parsing::DifferenceParser(
                                new Cm::Parsing::CharParser(':'),
                                new Cm::Parsing::StringParser("::"))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("s1", "Statement", 0)))),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::KeywordParser("case"),
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::NonterminalParser("ConstantExpression", "ConstantExpression", 0))),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::CharParser(':'))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("s2", "Statement", 0))))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("default"),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser(':'))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("s3", "Statement", 0)))))));
    AddRule(new LabelRule("Label", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0))));
    AddRule(new EmptyStatementRule("EmptyStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::CharParser(';'))));
    AddRule(new ExpressionStatementRule("ExpressionStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::NonterminalParser("Expression", "Expression", 0),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new CompoundStatementRule("CompoundStatement", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::CharParser('{')),
                new Cm::Parsing::KleeneStarParser(
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("Statement", "Statement", 0)))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser('}'))))));
    AddRule(new SelectionStatementRule("SelectionStatement", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("IfStatement", "IfStatement", 0)),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("SwitchStatement", "SwitchStatement", 0)))));
    AddRule(new IfStatementRule("IfStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::KeywordParser("if"),
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::CharParser('('))),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::NonterminalParser("Condition", "Condition", 0))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser(')'))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("thenStatement", "Statement", 0))),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("else"),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("elseStatement", "Statement", 0))))))));
    AddRule(new SwitchStatementRule("SwitchStatement", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("switch"),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser('('))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("Condition", "Condition", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(')'))),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("Statement", "Statement", 0))))));
    AddRule(new IterationStatementRule("IterationStatement", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("WhileStatement", "WhileStatement", 0)),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::NonterminalParser("DoStatement", "DoStatement", 0))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::NonterminalParser("ForStatement", "ForStatement", 0)))));
    AddRule(new WhileStatementRule("WhileStatement", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("while"),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser('('))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("Condition", "Condition", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(')'))),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("Statement", "Statement", 0))))));
    AddRule(new DoStatementRule("DoStatement", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::KeywordParser("do"),
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::NonterminalParser("Statement", "Statement", 0))),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::KeywordParser("while"))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser('('))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("Expression", "Expression", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(')'))),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new ForStatementRule("ForStatement", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::KeywordParser("for"),
                                    new Cm::Parsing::ExpectationParser(
                                        new Cm::Parsing::CharParser('('))),
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::NonterminalParser("ForInitStatement", "ForInitStatement", 0))),
                            new Cm::Parsing::OptionalParser(
                                new Cm::Parsing::NonterminalParser("Condition", "Condition", 0))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser(';'))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("Expression", "Expression", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(')'))),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("Statement", "Statement", 0))))));
    AddRule(new ForInitStatementRule("ForInitStatement", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("EmptyStatement", "EmptyStatement", 0)),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::NonterminalParser("ExpressionStatement", "ExpressionStatement", 0))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::NonterminalParser("SimpleDeclaration", "SimpleDeclaration", 0)))));
    AddRule(new JumpStatementRule("JumpStatement", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::NonterminalParser("BreakStatement", "BreakStatement", 0)),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("ContinueStatement", "ContinueStatement", 0))),
                new Cm::Parsing::ActionParser("A2",
                    new Cm::Parsing::NonterminalParser("ReturnStatement", "ReturnStatement", 0))),
            new Cm::Parsing::ActionParser("A3",
                new Cm::Parsing::NonterminalParser("GotoStatement", "GotoStatement", 0)))));
    AddRule(new BreakStatementRule("BreakStatement", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::KeywordParser("break"),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new ContinueStatementRule("ContinueStatement", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::KeywordParser("continue"),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new ReturnStatementRule("ReturnStatement", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::KeywordParser("return"),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::NonterminalParser("Expression", "Expression", 0))),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new GotoStatementRule("GotoStatement", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::KeywordParser("goto"),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("GotoTarget", "GotoTarget", 0))),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new GotoTargetRule("GotoTarget", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0))));
    AddRule(new DeclarationStatementRule("DeclarationStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("BlockDeclaration", "BlockDeclaration", 0))));
    AddRule(new ConditionRule("Condition", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::ActionParser("A0",
                            new Cm::Parsing::NonterminalParser("TypeId", "TypeId", 0)),
                        new Cm::Parsing::NonterminalParser("Declarator", "Declarator", 0)),
                    new Cm::Parsing::CharParser('=')),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::NonterminalParser("AssignmentExpression", "AssignmentExpression", 0))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::NonterminalParser("Expression", "Expression", 0)))));
    AddRule(new TryStatementRule("TryStatement", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::KeywordParser("try"),
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("CompoundStatement", "CompoundStatement", 0)))),
            new Cm::Parsing::ExpectationParser(
                new Cm::Parsing::NonterminalParser("HandlerSeq", "HandlerSeq", 1)))));
    AddRule(new HandlerSeqRule("HandlerSeq", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("Handler", "Handler", 0))),
            new Cm::Parsing::OptionalParser(
                new Cm::Parsing::NonterminalParser("HandlerSeq", "HandlerSeq", 1)))));
    AddRule(new HandlerRule("Handler", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("catch"),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser('('))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("ExceptionDeclaration", "ExceptionDeclaration", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(')'))),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("CompoundStatement", "CompoundStatement", 0))))));
    AddRule(new ExceptionDeclarationRule("ExceptionDeclaration", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::EmptyParser()),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::NonterminalParser("TypeSpecifierSeq", "TypeSpecifierSeq", 1),
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::AlternativeParser(
                                new Cm::Parsing::ActionParser("A2",
                                    new Cm::Parsing::NonterminalParser("Declarator", "Declarator", 0)),
                                new Cm::Parsing::ActionParser("A3",
                                    new Cm::Parsing::NonterminalParser("AbstractDeclarator", "AbstractDeclarator", 0))),
                            new Cm::Parsing::EmptyParser())),
                    new Cm::Parsing::ActionParser("A4",
                        new Cm::Parsing::StringParser("...")))))));
    SetStartRuleName("CompoundStatement");
    SetSkipRuleName("spaces_and_comments");
}

} } } // namespace Cm.Parsing.Cpp
