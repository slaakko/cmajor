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
#include <Cm.Parser/Keyword.hpp>
#include <Cm.Parser/Expression.hpp>
#include <Cm.Parser/TypeExpr.hpp>
#include <Cm.Parser/Identifier.hpp>

namespace Cm { namespace Parser {

using namespace Cm::Ast;
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

StatementGrammar::StatementGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("StatementGrammar", parsingDomain_->GetNamespaceScope("Cm.Parser"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Ast::CompoundStatementNode* StatementGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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
    Cm::Ast::CompoundStatementNode* result = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompoundStatementNode*>*>(value.get());
    stack.pop();
    return result;
}

class StatementGrammar::StatementRule : public Cm::Parsing::Rule
{
public:
    StatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
        Synchronize(";}");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
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
        Cm::Parsing::ActionParser* a9ActionParser = GetAction("A9");
        a9ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A9Action));
        Cm::Parsing::ActionParser* a10ActionParser = GetAction("A10");
        a10ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A10Action));
        Cm::Parsing::ActionParser* a11ActionParser = GetAction("A11");
        a11ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A11Action));
        Cm::Parsing::NonterminalParser* labeledStatementNonterminalParser = GetNonterminal("LabeledStatement");
        labeledStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreLabeledStatement));
        labeledStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostLabeledStatement));
        Cm::Parsing::NonterminalParser* controlStatementNonterminalParser = GetNonterminal("ControlStatement");
        controlStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreControlStatement));
        controlStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostControlStatement));
        Cm::Parsing::NonterminalParser* typedefStatementNonterminalParser = GetNonterminal("TypedefStatement");
        typedefStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreTypedefStatement));
        typedefStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostTypedefStatement));
        Cm::Parsing::NonterminalParser* simpleStatementNonterminalParser = GetNonterminal("SimpleStatement");
        simpleStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreSimpleStatement));
        simpleStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostSimpleStatement));
        Cm::Parsing::NonterminalParser* assignmentStatementNonterminalParser = GetNonterminal("AssignmentStatement");
        assignmentStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreAssignmentStatement));
        assignmentStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostAssignmentStatement));
        Cm::Parsing::NonterminalParser* constructionStatementNonterminalParser = GetNonterminal("ConstructionStatement");
        constructionStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreConstructionStatement));
        constructionStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostConstructionStatement));
        Cm::Parsing::NonterminalParser* deleteStatementNonterminalParser = GetNonterminal("DeleteStatement");
        deleteStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreDeleteStatement));
        deleteStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostDeleteStatement));
        Cm::Parsing::NonterminalParser* destroyStatementNonterminalParser = GetNonterminal("DestroyStatement");
        destroyStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreDestroyStatement));
        destroyStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostDestroyStatement));
        Cm::Parsing::NonterminalParser* throwStatementNonterminalParser = GetNonterminal("ThrowStatement");
        throwStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreThrowStatement));
        throwStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostThrowStatement));
        Cm::Parsing::NonterminalParser* tryStatementNonterminalParser = GetNonterminal("TryStatement");
        tryStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreTryStatement));
        tryStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostTryStatement));
        Cm::Parsing::NonterminalParser* assertStatementNonterminalParser = GetNonterminal("AssertStatement");
        assertStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreAssertStatement));
        assertStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostAssertStatement));
        Cm::Parsing::NonterminalParser* conditionalCompilationStatementNonterminalParser = GetNonterminal("ConditionalCompilationStatement");
        conditionalCompilationStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreConditionalCompilationStatement));
        conditionalCompilationStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostConditionalCompilationStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromLabeledStatement;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromControlStatement;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromTypedefStatement;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromSimpleStatement;
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromAssignmentStatement;
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromConstructionStatement;
    }
    void A6Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromDeleteStatement;
    }
    void A7Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromDestroyStatement;
    }
    void A8Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromThrowStatement;
    }
    void A9Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromTryStatement;
    }
    void A10Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromAssertStatement;
    }
    void A11Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromConditionalCompilationStatement;
    }
    void PreLabeledStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostLabeledStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromLabeledStatement_value = std::move(stack.top());
            context.fromLabeledStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromLabeledStatement_value.get());
            stack.pop();
        }
    }
    void PreControlStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostControlStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromControlStatement_value = std::move(stack.top());
            context.fromControlStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromControlStatement_value.get());
            stack.pop();
        }
    }
    void PreTypedefStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostTypedefStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypedefStatement_value = std::move(stack.top());
            context.fromTypedefStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromTypedefStatement_value.get());
            stack.pop();
        }
    }
    void PreSimpleStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostSimpleStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromSimpleStatement_value = std::move(stack.top());
            context.fromSimpleStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromSimpleStatement_value.get());
            stack.pop();
        }
    }
    void PreAssignmentStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostAssignmentStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAssignmentStatement_value = std::move(stack.top());
            context.fromAssignmentStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromAssignmentStatement_value.get());
            stack.pop();
        }
    }
    void PreConstructionStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostConstructionStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConstructionStatement_value = std::move(stack.top());
            context.fromConstructionStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromConstructionStatement_value.get());
            stack.pop();
        }
    }
    void PreDeleteStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostDeleteStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDeleteStatement_value = std::move(stack.top());
            context.fromDeleteStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromDeleteStatement_value.get());
            stack.pop();
        }
    }
    void PreDestroyStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostDestroyStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDestroyStatement_value = std::move(stack.top());
            context.fromDestroyStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromDestroyStatement_value.get());
            stack.pop();
        }
    }
    void PreThrowStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostThrowStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromThrowStatement_value = std::move(stack.top());
            context.fromThrowStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromThrowStatement_value.get());
            stack.pop();
        }
    }
    void PreTryStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostTryStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTryStatement_value = std::move(stack.top());
            context.fromTryStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::TryStatementNode*>*>(fromTryStatement_value.get());
            stack.pop();
        }
    }
    void PreAssertStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostAssertStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAssertStatement_value = std::move(stack.top());
            context.fromAssertStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromAssertStatement_value.get());
            stack.pop();
        }
    }
    void PreConditionalCompilationStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostConditionalCompilationStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConditionalCompilationStatement_value = std::move(stack.top());
            context.fromConditionalCompilationStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CondCompStatementNode*>*>(fromConditionalCompilationStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromLabeledStatement(), fromControlStatement(), fromTypedefStatement(), fromSimpleStatement(), fromAssignmentStatement(), fromConstructionStatement(), fromDeleteStatement(), fromDestroyStatement(), fromThrowStatement(), fromTryStatement(), fromAssertStatement(), fromConditionalCompilationStatement() {}
        ParsingContext* ctx;
        Cm::Ast::StatementNode* value;
        Cm::Ast::StatementNode* fromLabeledStatement;
        Cm::Ast::StatementNode* fromControlStatement;
        Cm::Ast::StatementNode* fromTypedefStatement;
        Cm::Ast::StatementNode* fromSimpleStatement;
        Cm::Ast::StatementNode* fromAssignmentStatement;
        Cm::Ast::StatementNode* fromConstructionStatement;
        Cm::Ast::StatementNode* fromDeleteStatement;
        Cm::Ast::StatementNode* fromDestroyStatement;
        Cm::Ast::StatementNode* fromThrowStatement;
        Cm::Ast::TryStatementNode* fromTryStatement;
        Cm::Ast::StatementNode* fromAssertStatement;
        Cm::Ast::CondCompStatementNode* fromConditionalCompilationStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::LabelIdRule : public Cm::Parsing::Rule
{
public:
    LabelIdRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LabelIdRule>(this, &LabelIdRule::A0Action));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LabelIdRule>(this, &LabelIdRule::Postidentifier));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = std::string(matchBegin, matchEnd);
    }
    void Postidentifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromidentifier_value = std::move(stack.top());
            context.fromidentifier = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromidentifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromidentifier() {}
        std::string value;
        std::string fromidentifier;
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
        SetValueTypeName("Cm::Ast::LabelNode*");
        AddLocalVariable(AttrOrVariable("std::string", "label"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::LabelNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LabelRule>(this, &LabelRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LabelRule>(this, &LabelRule::A1Action));
        Cm::Parsing::NonterminalParser* labelIdNonterminalParser = GetNonterminal("LabelId");
        labelIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LabelRule>(this, &LabelRule::PostLabelId));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new LabelNode(span, context.label);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.label = context.fromLabelId;
    }
    void PostLabelId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromLabelId_value = std::move(stack.top());
            context.fromLabelId = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromLabelId_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), label(), fromLabelId() {}
        Cm::Ast::LabelNode* value;
        std::string label;
        std::string fromLabelId;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::TargetLabelRule : public Cm::Parsing::Rule
{
public:
    TargetLabelRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::LabelNode*");
        AddLocalVariable(AttrOrVariable("std::string", "label"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::LabelNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TargetLabelRule>(this, &TargetLabelRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TargetLabelRule>(this, &TargetLabelRule::A1Action));
        Cm::Parsing::NonterminalParser* labelIdNonterminalParser = GetNonterminal("LabelId");
        labelIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TargetLabelRule>(this, &TargetLabelRule::PostLabelId));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new LabelNode(span, context.label);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.label = context.fromLabelId;
    }
    void PostLabelId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromLabelId_value = std::move(stack.top());
            context.fromLabelId = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromLabelId_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), label(), fromLabelId() {}
        Cm::Ast::LabelNode* value;
        std::string label;
        std::string fromLabelId;
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
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LabeledStatementRule>(this, &LabeledStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* labelNonterminalParser = GetNonterminal("Label");
        labelNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::PostLabel));
        Cm::Parsing::NonterminalParser* statementNonterminalParser = GetNonterminal("Statement");
        statementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<LabeledStatementRule>(this, &LabeledStatementRule::PreStatement));
        statementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::PostStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.fromStatement->SetLabelNode(context.fromLabel);
        context.value = context.fromStatement;
    }
    void PostLabel(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromLabel_value = std::move(stack.top());
            context.fromLabel = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::LabelNode*>*>(fromLabel_value.get());
            stack.pop();
        }
    }
    void PreStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStatement_value = std::move(stack.top());
            context.fromStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromLabel(), fromStatement() {}
        ParsingContext* ctx;
        Cm::Ast::StatementNode* value;
        Cm::Ast::LabelNode* fromLabel;
        Cm::Ast::StatementNode* fromStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::SimpleStatementRule : public Cm::Parsing::Rule
{
public:
    SimpleStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<Node>", "expr"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SimpleStatementRule>(this, &SimpleStatementRule::A0Action));
        a0ActionParser->SetFailureAction(new Cm::Parsing::MemberFailureAction<SimpleStatementRule>(this, &SimpleStatementRule::A0ActionFail));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SimpleStatementRule>(this, &SimpleStatementRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SimpleStatementRule>(this, &SimpleStatementRule::A2Action));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<SimpleStatementRule>(this, &SimpleStatementRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SimpleStatementRule>(this, &SimpleStatementRule::PostExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ctx->EndParsingSimpleStatement();
        context.value = new SimpleStatementNode(span, context.expr.release());
    }
    void A0ActionFail()
    {
        context.ctx->EndParsingSimpleStatement();
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ctx->BeginParsingSimpleStatement();
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.expr.reset(context.fromExpression);
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
        Context(): ctx(), value(), expr(), fromExpression() {}
        ParsingContext* ctx;
        Cm::Ast::StatementNode* value;
        std::unique_ptr<Node> expr;
        Cm::Ast::Node* fromExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ControlStatementRule : public Cm::Parsing::Rule
{
public:
    ControlStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ControlStatementRule>(this, &ControlStatementRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ControlStatementRule>(this, &ControlStatementRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ControlStatementRule>(this, &ControlStatementRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ControlStatementRule>(this, &ControlStatementRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ControlStatementRule>(this, &ControlStatementRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ControlStatementRule>(this, &ControlStatementRule::A5Action));
        Cm::Parsing::ActionParser* a6ActionParser = GetAction("A6");
        a6ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ControlStatementRule>(this, &ControlStatementRule::A6Action));
        Cm::Parsing::ActionParser* a7ActionParser = GetAction("A7");
        a7ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ControlStatementRule>(this, &ControlStatementRule::A7Action));
        Cm::Parsing::ActionParser* a8ActionParser = GetAction("A8");
        a8ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ControlStatementRule>(this, &ControlStatementRule::A8Action));
        Cm::Parsing::ActionParser* a9ActionParser = GetAction("A9");
        a9ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ControlStatementRule>(this, &ControlStatementRule::A9Action));
        Cm::Parsing::ActionParser* a10ActionParser = GetAction("A10");
        a10ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ControlStatementRule>(this, &ControlStatementRule::A10Action));
        Cm::Parsing::ActionParser* a11ActionParser = GetAction("A11");
        a11ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ControlStatementRule>(this, &ControlStatementRule::A11Action));
        Cm::Parsing::ActionParser* a12ActionParser = GetAction("A12");
        a12ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ControlStatementRule>(this, &ControlStatementRule::A12Action));
        Cm::Parsing::NonterminalParser* returnStatementNonterminalParser = GetNonterminal("ReturnStatement");
        returnStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ControlStatementRule>(this, &ControlStatementRule::PreReturnStatement));
        returnStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ControlStatementRule>(this, &ControlStatementRule::PostReturnStatement));
        Cm::Parsing::NonterminalParser* conditionalStatementNonterminalParser = GetNonterminal("ConditionalStatement");
        conditionalStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ControlStatementRule>(this, &ControlStatementRule::PreConditionalStatement));
        conditionalStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ControlStatementRule>(this, &ControlStatementRule::PostConditionalStatement));
        Cm::Parsing::NonterminalParser* switchStatementNonterminalParser = GetNonterminal("SwitchStatement");
        switchStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ControlStatementRule>(this, &ControlStatementRule::PreSwitchStatement));
        switchStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ControlStatementRule>(this, &ControlStatementRule::PostSwitchStatement));
        Cm::Parsing::NonterminalParser* whileStatementNonterminalParser = GetNonterminal("WhileStatement");
        whileStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ControlStatementRule>(this, &ControlStatementRule::PreWhileStatement));
        whileStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ControlStatementRule>(this, &ControlStatementRule::PostWhileStatement));
        Cm::Parsing::NonterminalParser* doStatementNonterminalParser = GetNonterminal("DoStatement");
        doStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ControlStatementRule>(this, &ControlStatementRule::PreDoStatement));
        doStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ControlStatementRule>(this, &ControlStatementRule::PostDoStatement));
        Cm::Parsing::NonterminalParser* rangeForStatementNonterminalParser = GetNonterminal("RangeForStatement");
        rangeForStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ControlStatementRule>(this, &ControlStatementRule::PreRangeForStatement));
        rangeForStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ControlStatementRule>(this, &ControlStatementRule::PostRangeForStatement));
        Cm::Parsing::NonterminalParser* forStatementNonterminalParser = GetNonterminal("ForStatement");
        forStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ControlStatementRule>(this, &ControlStatementRule::PreForStatement));
        forStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ControlStatementRule>(this, &ControlStatementRule::PostForStatement));
        Cm::Parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal("CompoundStatement");
        compoundStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ControlStatementRule>(this, &ControlStatementRule::PreCompoundStatement));
        compoundStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ControlStatementRule>(this, &ControlStatementRule::PostCompoundStatement));
        Cm::Parsing::NonterminalParser* breakStatementNonterminalParser = GetNonterminal("BreakStatement");
        breakStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ControlStatementRule>(this, &ControlStatementRule::PreBreakStatement));
        breakStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ControlStatementRule>(this, &ControlStatementRule::PostBreakStatement));
        Cm::Parsing::NonterminalParser* continueStatementNonterminalParser = GetNonterminal("ContinueStatement");
        continueStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ControlStatementRule>(this, &ControlStatementRule::PreContinueStatement));
        continueStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ControlStatementRule>(this, &ControlStatementRule::PostContinueStatement));
        Cm::Parsing::NonterminalParser* gotoCaseStatementNonterminalParser = GetNonterminal("GotoCaseStatement");
        gotoCaseStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ControlStatementRule>(this, &ControlStatementRule::PreGotoCaseStatement));
        gotoCaseStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ControlStatementRule>(this, &ControlStatementRule::PostGotoCaseStatement));
        Cm::Parsing::NonterminalParser* gotoDefaultStatementNonterminalParser = GetNonterminal("GotoDefaultStatement");
        gotoDefaultStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ControlStatementRule>(this, &ControlStatementRule::PreGotoDefaultStatement));
        gotoDefaultStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ControlStatementRule>(this, &ControlStatementRule::PostGotoDefaultStatement));
        Cm::Parsing::NonterminalParser* gotoStatementNonterminalParser = GetNonterminal("GotoStatement");
        gotoStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ControlStatementRule>(this, &ControlStatementRule::PreGotoStatement));
        gotoStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ControlStatementRule>(this, &ControlStatementRule::PostGotoStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromReturnStatement;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromConditionalStatement;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromSwitchStatement;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromWhileStatement;
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromDoStatement;
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromRangeForStatement;
    }
    void A6Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromForStatement;
    }
    void A7Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromCompoundStatement;
    }
    void A8Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromBreakStatement;
    }
    void A9Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromContinueStatement;
    }
    void A10Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromGotoCaseStatement;
    }
    void A11Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromGotoDefaultStatement;
    }
    void A12Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromGotoStatement;
    }
    void PreReturnStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostReturnStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromReturnStatement_value = std::move(stack.top());
            context.fromReturnStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromReturnStatement_value.get());
            stack.pop();
        }
    }
    void PreConditionalStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostConditionalStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConditionalStatement_value = std::move(stack.top());
            context.fromConditionalStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromConditionalStatement_value.get());
            stack.pop();
        }
    }
    void PreSwitchStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostSwitchStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromSwitchStatement_value = std::move(stack.top());
            context.fromSwitchStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::SwitchStatementNode*>*>(fromSwitchStatement_value.get());
            stack.pop();
        }
    }
    void PreWhileStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostWhileStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromWhileStatement_value = std::move(stack.top());
            context.fromWhileStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromWhileStatement_value.get());
            stack.pop();
        }
    }
    void PreDoStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostDoStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDoStatement_value = std::move(stack.top());
            context.fromDoStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromDoStatement_value.get());
            stack.pop();
        }
    }
    void PreRangeForStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostRangeForStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromRangeForStatement_value = std::move(stack.top());
            context.fromRangeForStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromRangeForStatement_value.get());
            stack.pop();
        }
    }
    void PreForStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostForStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromForStatement_value = std::move(stack.top());
            context.fromForStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromForStatement_value.get());
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
    void PreBreakStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostBreakStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromBreakStatement_value = std::move(stack.top());
            context.fromBreakStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromBreakStatement_value.get());
            stack.pop();
        }
    }
    void PreContinueStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostContinueStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromContinueStatement_value = std::move(stack.top());
            context.fromContinueStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromContinueStatement_value.get());
            stack.pop();
        }
    }
    void PreGotoCaseStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostGotoCaseStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromGotoCaseStatement_value = std::move(stack.top());
            context.fromGotoCaseStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromGotoCaseStatement_value.get());
            stack.pop();
        }
    }
    void PreGotoDefaultStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostGotoDefaultStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromGotoDefaultStatement_value = std::move(stack.top());
            context.fromGotoDefaultStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromGotoDefaultStatement_value.get());
            stack.pop();
        }
    }
    void PreGotoStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostGotoStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromGotoStatement_value = std::move(stack.top());
            context.fromGotoStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromGotoStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromReturnStatement(), fromConditionalStatement(), fromSwitchStatement(), fromWhileStatement(), fromDoStatement(), fromRangeForStatement(), fromForStatement(), fromCompoundStatement(), fromBreakStatement(), fromContinueStatement(), fromGotoCaseStatement(), fromGotoDefaultStatement(), fromGotoStatement() {}
        ParsingContext* ctx;
        Cm::Ast::StatementNode* value;
        Cm::Ast::StatementNode* fromReturnStatement;
        Cm::Ast::StatementNode* fromConditionalStatement;
        Cm::Ast::SwitchStatementNode* fromSwitchStatement;
        Cm::Ast::StatementNode* fromWhileStatement;
        Cm::Ast::StatementNode* fromDoStatement;
        Cm::Ast::StatementNode* fromRangeForStatement;
        Cm::Ast::StatementNode* fromForStatement;
        Cm::Ast::CompoundStatementNode* fromCompoundStatement;
        Cm::Ast::StatementNode* fromBreakStatement;
        Cm::Ast::StatementNode* fromContinueStatement;
        Cm::Ast::StatementNode* fromGotoCaseStatement;
        Cm::Ast::StatementNode* fromGotoDefaultStatement;
        Cm::Ast::StatementNode* fromGotoStatement;
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
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ReturnStatementRule>(this, &ReturnStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ReturnStatementRule>(this, &ReturnStatementRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ReturnStatementRule>(this, &ReturnStatementRule::PostExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ReturnStatementNode(span, context.fromExpression);
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
        Cm::Ast::StatementNode* value;
        Cm::Ast::Node* fromExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ConditionalStatementRule : public Cm::Parsing::Rule
{
public:
    ConditionalStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalStatementRule>(this, &ConditionalStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConditionalStatementRule>(this, &ConditionalStatementRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalStatementRule>(this, &ConditionalStatementRule::PostExpression));
        Cm::Parsing::NonterminalParser* thenSNonterminalParser = GetNonterminal("thenS");
        thenSNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConditionalStatementRule>(this, &ConditionalStatementRule::PrethenS));
        thenSNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalStatementRule>(this, &ConditionalStatementRule::PostthenS));
        Cm::Parsing::NonterminalParser* elseSNonterminalParser = GetNonterminal("elseS");
        elseSNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConditionalStatementRule>(this, &ConditionalStatementRule::PreelseS));
        elseSNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalStatementRule>(this, &ConditionalStatementRule::PostelseS));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ConditionalStatementNode(span, context.fromExpression, context.fromthenS, context.fromelseS);
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
    void PrethenS(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostthenS(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromthenS_value = std::move(stack.top());
            context.fromthenS = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromthenS_value.get());
            stack.pop();
        }
    }
    void PreelseS(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostelseS(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromelseS_value = std::move(stack.top());
            context.fromelseS = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromelseS_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromExpression(), fromthenS(), fromelseS() {}
        ParsingContext* ctx;
        Cm::Ast::StatementNode* value;
        Cm::Ast::Node* fromExpression;
        Cm::Ast::StatementNode* fromthenS;
        Cm::Ast::StatementNode* fromelseS;
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
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::SwitchStatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::SwitchStatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SwitchStatementRule>(this, &SwitchStatementRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SwitchStatementRule>(this, &SwitchStatementRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SwitchStatementRule>(this, &SwitchStatementRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SwitchStatementRule>(this, &SwitchStatementRule::A3Action));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<SwitchStatementRule>(this, &SwitchStatementRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SwitchStatementRule>(this, &SwitchStatementRule::PostExpression));
        Cm::Parsing::NonterminalParser* caseStatementNonterminalParser = GetNonterminal("CaseStatement");
        caseStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<SwitchStatementRule>(this, &SwitchStatementRule::PreCaseStatement));
        caseStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SwitchStatementRule>(this, &SwitchStatementRule::PostCaseStatement));
        Cm::Parsing::NonterminalParser* defaultStatementNonterminalParser = GetNonterminal("DefaultStatement");
        defaultStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<SwitchStatementRule>(this, &SwitchStatementRule::PreDefaultStatement));
        defaultStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SwitchStatementRule>(this, &SwitchStatementRule::PostDefaultStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new SwitchStatementNode(span, context.fromExpression);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->AddCase(context.fromCaseStatement);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->SetDefault(context.fromDefaultStatement);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->GetSpan().SetEnd(span.End());
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
    void PreCaseStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostCaseStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCaseStatement_value = std::move(stack.top());
            context.fromCaseStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CaseStatementNode*>*>(fromCaseStatement_value.get());
            stack.pop();
        }
    }
    void PreDefaultStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostDefaultStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDefaultStatement_value = std::move(stack.top());
            context.fromDefaultStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::DefaultStatementNode*>*>(fromDefaultStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromExpression(), fromCaseStatement(), fromDefaultStatement() {}
        ParsingContext* ctx;
        Cm::Ast::SwitchStatementNode* value;
        Cm::Ast::Node* fromExpression;
        Cm::Ast::CaseStatementNode* fromCaseStatement;
        Cm::Ast::DefaultStatementNode* fromDefaultStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::CaseStatementRule : public Cm::Parsing::Rule
{
public:
    CaseStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::CaseStatementNode*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<CaseStatementNode>", "caseS"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CaseStatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CaseStatementRule>(this, &CaseStatementRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CaseStatementRule>(this, &CaseStatementRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CaseStatementRule>(this, &CaseStatementRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CaseStatementRule>(this, &CaseStatementRule::A3Action));
        Cm::Parsing::NonterminalParser* caseListNonterminalParser = GetNonterminal("CaseList");
        caseListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<CaseStatementRule>(this, &CaseStatementRule::PreCaseList));
        Cm::Parsing::NonterminalParser* statementNonterminalParser = GetNonterminal("Statement");
        statementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<CaseStatementRule>(this, &CaseStatementRule::PreStatement));
        statementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CaseStatementRule>(this, &CaseStatementRule::PostStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.caseS.release();
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.caseS.reset(new CaseStatementNode(span));
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.caseS->GetSpan().SetEnd(span.End());
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.caseS->AddStatement(context.fromStatement);
    }
    void PreCaseList(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CaseStatementNode*>(context.caseS.get())));
    }
    void PreStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStatement_value = std::move(stack.top());
            context.fromStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), caseS(), fromStatement() {}
        ParsingContext* ctx;
        Cm::Ast::CaseStatementNode* value;
        std::unique_ptr<CaseStatementNode> caseS;
        Cm::Ast::StatementNode* fromStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::DefaultStatementRule : public Cm::Parsing::Rule
{
public:
    DefaultStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::DefaultStatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::DefaultStatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DefaultStatementRule>(this, &DefaultStatementRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DefaultStatementRule>(this, &DefaultStatementRule::A1Action));
        Cm::Parsing::NonterminalParser* statementNonterminalParser = GetNonterminal("Statement");
        statementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DefaultStatementRule>(this, &DefaultStatementRule::PreStatement));
        statementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DefaultStatementRule>(this, &DefaultStatementRule::PostStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new DefaultStatementNode(span);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->AddStatement(context.fromStatement);
    }
    void PreStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStatement_value = std::move(stack.top());
            context.fromStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromStatement() {}
        ParsingContext* ctx;
        Cm::Ast::DefaultStatementNode* value;
        Cm::Ast::StatementNode* fromStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::CaseListRule : public Cm::Parsing::Rule
{
public:
    CaseListRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::CaseStatementNode*", "caseS"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> caseS_value = std::move(stack.top());
        context.caseS = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CaseStatementNode*>*>(caseS_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CaseListRule>(this, &CaseListRule::A0Action));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<CaseListRule>(this, &CaseListRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CaseListRule>(this, &CaseListRule::PostExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.caseS->AddExpr(context.fromExpression);
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
        Context(): ctx(), caseS(), fromExpression() {}
        ParsingContext* ctx;
        Cm::Ast::CaseStatementNode* caseS;
        Cm::Ast::Node* fromExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::GotoCaseStatementRule : public Cm::Parsing::Rule
{
public:
    GotoCaseStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<GotoCaseStatementRule>(this, &GotoCaseStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<GotoCaseStatementRule>(this, &GotoCaseStatementRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<GotoCaseStatementRule>(this, &GotoCaseStatementRule::PostExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new GotoCaseStatementNode(span, context.fromExpression);
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
        Cm::Ast::StatementNode* value;
        Cm::Ast::Node* fromExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::GotoDefaultStatementRule : public Cm::Parsing::Rule
{
public:
    GotoDefaultStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<GotoDefaultStatementRule>(this, &GotoDefaultStatementRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new GotoDefaultStatementNode(span);
    }
private:
    struct Context
    {
        Context(): ctx(), value() {}
        ParsingContext* ctx;
        Cm::Ast::StatementNode* value;
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
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<WhileStatementRule>(this, &WhileStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<WhileStatementRule>(this, &WhileStatementRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<WhileStatementRule>(this, &WhileStatementRule::PostExpression));
        Cm::Parsing::NonterminalParser* statementNonterminalParser = GetNonterminal("Statement");
        statementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<WhileStatementRule>(this, &WhileStatementRule::PreStatement));
        statementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<WhileStatementRule>(this, &WhileStatementRule::PostStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new WhileStatementNode(span, context.fromExpression, context.fromStatement);
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
    void PreStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStatement_value = std::move(stack.top());
            context.fromStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromExpression(), fromStatement() {}
        ParsingContext* ctx;
        Cm::Ast::StatementNode* value;
        Cm::Ast::Node* fromExpression;
        Cm::Ast::StatementNode* fromStatement;
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
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DoStatementRule>(this, &DoStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* statementNonterminalParser = GetNonterminal("Statement");
        statementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DoStatementRule>(this, &DoStatementRule::PreStatement));
        statementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DoStatementRule>(this, &DoStatementRule::PostStatement));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DoStatementRule>(this, &DoStatementRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DoStatementRule>(this, &DoStatementRule::PostExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new DoStatementNode(span, context.fromStatement, context.fromExpression);
    }
    void PreStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStatement_value = std::move(stack.top());
            context.fromStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromStatement_value.get());
            stack.pop();
        }
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
        Context(): ctx(), value(), fromStatement(), fromExpression() {}
        ParsingContext* ctx;
        Cm::Ast::StatementNode* value;
        Cm::Ast::StatementNode* fromStatement;
        Cm::Ast::Node* fromExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::RangeForStatementRule : public Cm::Parsing::Rule
{
public:
    RangeForStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<Node>", "varType"));
        AddLocalVariable(AttrOrVariable("std::unique_ptr<IdentifierNode>", "varId"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RangeForStatementRule>(this, &RangeForStatementRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RangeForStatementRule>(this, &RangeForStatementRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RangeForStatementRule>(this, &RangeForStatementRule::A2Action));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<RangeForStatementRule>(this, &RangeForStatementRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RangeForStatementRule>(this, &RangeForStatementRule::PostTypeExpr));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RangeForStatementRule>(this, &RangeForStatementRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<RangeForStatementRule>(this, &RangeForStatementRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RangeForStatementRule>(this, &RangeForStatementRule::PostExpression));
        Cm::Parsing::NonterminalParser* statementNonterminalParser = GetNonterminal("Statement");
        statementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<RangeForStatementRule>(this, &RangeForStatementRule::PreStatement));
        statementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RangeForStatementRule>(this, &RangeForStatementRule::PostStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new RangeForStatementNode(span, context.varType.release(), context.varId.release(), context.fromExpression, context.fromStatement);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.varType.reset(context.fromTypeExpr);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.varId.reset(context.fromIdentifier);
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
    void PreStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStatement_value = std::move(stack.top());
            context.fromStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), varType(), varId(), fromTypeExpr(), fromIdentifier(), fromExpression(), fromStatement() {}
        ParsingContext* ctx;
        Cm::Ast::StatementNode* value;
        std::unique_ptr<Node> varType;
        std::unique_ptr<IdentifierNode> varId;
        Cm::Ast::Node* fromTypeExpr;
        Cm::Ast::IdentifierNode* fromIdentifier;
        Cm::Ast::Node* fromExpression;
        Cm::Ast::StatementNode* fromStatement;
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
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ForStatementRule>(this, &ForStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* forInitStatementNonterminalParser = GetNonterminal("ForInitStatement");
        forInitStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ForStatementRule>(this, &ForStatementRule::PreForInitStatement));
        forInitStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ForStatementRule>(this, &ForStatementRule::PostForInitStatement));
        Cm::Parsing::NonterminalParser* conditionNonterminalParser = GetNonterminal("condition");
        conditionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ForStatementRule>(this, &ForStatementRule::Precondition));
        conditionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ForStatementRule>(this, &ForStatementRule::Postcondition));
        Cm::Parsing::NonterminalParser* incrementNonterminalParser = GetNonterminal("increment");
        incrementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ForStatementRule>(this, &ForStatementRule::Preincrement));
        incrementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ForStatementRule>(this, &ForStatementRule::Postincrement));
        Cm::Parsing::NonterminalParser* statementNonterminalParser = GetNonterminal("Statement");
        statementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ForStatementRule>(this, &ForStatementRule::PreStatement));
        statementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ForStatementRule>(this, &ForStatementRule::PostStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ForStatementNode(span, context.fromForInitStatement, context.fromcondition, context.fromincrement, context.fromStatement);
    }
    void PreForInitStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostForInitStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromForInitStatement_value = std::move(stack.top());
            context.fromForInitStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromForInitStatement_value.get());
            stack.pop();
        }
    }
    void Precondition(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void Postcondition(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromcondition_value = std::move(stack.top());
            context.fromcondition = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromcondition_value.get());
            stack.pop();
        }
    }
    void Preincrement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void Postincrement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromincrement_value = std::move(stack.top());
            context.fromincrement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromincrement_value.get());
            stack.pop();
        }
    }
    void PreStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStatement_value = std::move(stack.top());
            context.fromStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromForInitStatement(), fromcondition(), fromincrement(), fromStatement() {}
        ParsingContext* ctx;
        Cm::Ast::StatementNode* value;
        Cm::Ast::StatementNode* fromForInitStatement;
        Cm::Ast::Node* fromcondition;
        Cm::Ast::Node* fromincrement;
        Cm::Ast::StatementNode* fromStatement;
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
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
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
        Cm::Parsing::NonterminalParser* assignmentStatementNonterminalParser = GetNonterminal("AssignmentStatement");
        assignmentStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ForInitStatementRule>(this, &ForInitStatementRule::PreAssignmentStatement));
        assignmentStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ForInitStatementRule>(this, &ForInitStatementRule::PostAssignmentStatement));
        Cm::Parsing::NonterminalParser* constructionStatementNonterminalParser = GetNonterminal("ConstructionStatement");
        constructionStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ForInitStatementRule>(this, &ForInitStatementRule::PreConstructionStatement));
        constructionStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ForInitStatementRule>(this, &ForInitStatementRule::PostConstructionStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromAssignmentStatement;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromConstructionStatement;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new SimpleStatementNode(span, nullptr);
    }
    void PreAssignmentStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostAssignmentStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAssignmentStatement_value = std::move(stack.top());
            context.fromAssignmentStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromAssignmentStatement_value.get());
            stack.pop();
        }
    }
    void PreConstructionStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostConstructionStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConstructionStatement_value = std::move(stack.top());
            context.fromConstructionStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromConstructionStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromAssignmentStatement(), fromConstructionStatement() {}
        ParsingContext* ctx;
        Cm::Ast::StatementNode* value;
        Cm::Ast::StatementNode* fromAssignmentStatement;
        Cm::Ast::StatementNode* fromConstructionStatement;
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
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::CompoundStatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompoundStatementNode*>(context.value)));
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
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CompoundStatementRule>(this, &CompoundStatementRule::A3Action));
        Cm::Parsing::NonterminalParser* statementNonterminalParser = GetNonterminal("Statement");
        statementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<CompoundStatementRule>(this, &CompoundStatementRule::PreStatement));
        statementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CompoundStatementRule>(this, &CompoundStatementRule::PostStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->GetSpan().SetEnd(span.End());
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new CompoundStatementNode(span);
        context.value->SetBeginBraceSpan(span);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->AddStatement(context.fromStatement);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->SetEndBraceSpan(span);
    }
    void PreStatement(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostStatement(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStatement_value = std::move(stack.top());
            context.fromStatement = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromStatement() {}
        ParsingContext* ctx;
        Cm::Ast::CompoundStatementNode* value;
        Cm::Ast::StatementNode* fromStatement;
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
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
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
        context.value = new BreakStatementNode(span);
    }
private:
    struct Context
    {
        Context(): ctx(), value() {}
        ParsingContext* ctx;
        Cm::Ast::StatementNode* value;
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
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
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
        context.value = new ContinueStatementNode(span);
    }
private:
    struct Context
    {
        Context(): ctx(), value() {}
        ParsingContext* ctx;
        Cm::Ast::StatementNode* value;
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
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<GotoStatementRule>(this, &GotoStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* targetLabelNonterminalParser = GetNonterminal("TargetLabel");
        targetLabelNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<GotoStatementRule>(this, &GotoStatementRule::PostTargetLabel));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new GotoStatementNode(span, context.fromTargetLabel);
    }
    void PostTargetLabel(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTargetLabel_value = std::move(stack.top());
            context.fromTargetLabel = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::LabelNode*>*>(fromTargetLabel_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromTargetLabel() {}
        ParsingContext* ctx;
        Cm::Ast::StatementNode* value;
        Cm::Ast::LabelNode* fromTargetLabel;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::TypedefStatementRule : public Cm::Parsing::Rule
{
public:
    TypedefStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypedefStatementRule>(this, &TypedefStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<TypedefStatementRule>(this, &TypedefStatementRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypedefStatementRule>(this, &TypedefStatementRule::PostTypeExpr));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypedefStatementRule>(this, &TypedefStatementRule::PostIdentifier));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new TypedefStatementNode(span, context.fromTypeExpr, context.fromIdentifier);
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
        Context(): ctx(), value(), fromTypeExpr(), fromIdentifier() {}
        ParsingContext* ctx;
        Cm::Ast::StatementNode* value;
        Cm::Ast::Node* fromTypeExpr;
        Cm::Ast::IdentifierNode* fromIdentifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::AssignmentStatementRule : public Cm::Parsing::Rule
{
public:
    AssignmentStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<Node>", "targetExpr"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AssignmentStatementRule>(this, &AssignmentStatementRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AssignmentStatementRule>(this, &AssignmentStatementRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AssignmentStatementRule>(this, &AssignmentStatementRule::A2Action));
        a2ActionParser->SetFailureAction(new Cm::Parsing::MemberFailureAction<AssignmentStatementRule>(this, &AssignmentStatementRule::A2ActionFail));
        Cm::Parsing::NonterminalParser* targetNonterminalParser = GetNonterminal("target");
        targetNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<AssignmentStatementRule>(this, &AssignmentStatementRule::Pretarget));
        targetNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AssignmentStatementRule>(this, &AssignmentStatementRule::Posttarget));
        Cm::Parsing::NonterminalParser* sourceNonterminalParser = GetNonterminal("source");
        sourceNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<AssignmentStatementRule>(this, &AssignmentStatementRule::Presource));
        sourceNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AssignmentStatementRule>(this, &AssignmentStatementRule::Postsource));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ctx->BeginParsingLvalue();
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new AssignmentStatementNode(span, context.targetExpr.release(), context.fromsource);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ctx->EndParsingLvalue();
        context.targetExpr.reset(context.fromtarget);
    }
    void A2ActionFail()
    {
        context.ctx->EndParsingLvalue();
    }
    void Pretarget(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void Posttarget(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromtarget_value = std::move(stack.top());
            context.fromtarget = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromtarget_value.get());
            stack.pop();
        }
    }
    void Presource(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void Postsource(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromsource_value = std::move(stack.top());
            context.fromsource = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromsource_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), targetExpr(), fromtarget(), fromsource() {}
        ParsingContext* ctx;
        Cm::Ast::StatementNode* value;
        std::unique_ptr<Node> targetExpr;
        Cm::Ast::Node* fromtarget;
        Cm::Ast::Node* fromsource;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ConstructionStatementRule : public Cm::Parsing::Rule
{
public:
    ConstructionStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConstructionStatementRule>(this, &ConstructionStatementRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConstructionStatementRule>(this, &ConstructionStatementRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConstructionStatementRule>(this, &ConstructionStatementRule::A2Action));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConstructionStatementRule>(this, &ConstructionStatementRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConstructionStatementRule>(this, &ConstructionStatementRule::PostTypeExpr));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConstructionStatementRule>(this, &ConstructionStatementRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* argumentListNonterminalParser = GetNonterminal("ArgumentList");
        argumentListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConstructionStatementRule>(this, &ConstructionStatementRule::PreArgumentList));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConstructionStatementRule>(this, &ConstructionStatementRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConstructionStatementRule>(this, &ConstructionStatementRule::PostExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ConstructionStatementNode(span, context.fromTypeExpr, context.fromIdentifier);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->AddArgument(context.fromExpression);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->GetSpan().SetEnd(span.End());
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
    void PreArgumentList(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
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
        Context(): ctx(), value(), fromTypeExpr(), fromIdentifier(), fromExpression() {}
        ParsingContext* ctx;
        Cm::Ast::StatementNode* value;
        Cm::Ast::Node* fromTypeExpr;
        Cm::Ast::IdentifierNode* fromIdentifier;
        Cm::Ast::Node* fromExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::DeleteStatementRule : public Cm::Parsing::Rule
{
public:
    DeleteStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeleteStatementRule>(this, &DeleteStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DeleteStatementRule>(this, &DeleteStatementRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeleteStatementRule>(this, &DeleteStatementRule::PostExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new DeleteStatementNode(span, context.fromExpression);
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
        Cm::Ast::StatementNode* value;
        Cm::Ast::Node* fromExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::DestroyStatementRule : public Cm::Parsing::Rule
{
public:
    DestroyStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DestroyStatementRule>(this, &DestroyStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DestroyStatementRule>(this, &DestroyStatementRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DestroyStatementRule>(this, &DestroyStatementRule::PostExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new DestroyStatementNode(span, context.fromExpression);
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
        Cm::Ast::StatementNode* value;
        Cm::Ast::Node* fromExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ThrowStatementRule : public Cm::Parsing::Rule
{
public:
    ThrowStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ThrowStatementRule>(this, &ThrowStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ThrowStatementRule>(this, &ThrowStatementRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ThrowStatementRule>(this, &ThrowStatementRule::PostExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ThrowStatementNode(span, context.fromExpression);
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
        Cm::Ast::StatementNode* value;
        Cm::Ast::Node* fromExpression;
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
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::TryStatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::TryStatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TryStatementRule>(this, &TryStatementRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TryStatementRule>(this, &TryStatementRule::A1Action));
        Cm::Parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal("CompoundStatement");
        compoundStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<TryStatementRule>(this, &TryStatementRule::PreCompoundStatement));
        compoundStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TryStatementRule>(this, &TryStatementRule::PostCompoundStatement));
        Cm::Parsing::NonterminalParser* handlersNonterminalParser = GetNonterminal("Handlers");
        handlersNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<TryStatementRule>(this, &TryStatementRule::PreHandlers));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new TryStatementNode(span, context.fromCompoundStatement);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->GetSpan().SetEnd(span.End());
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
    void PreHandlers(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::TryStatementNode*>(context.value)));
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromCompoundStatement() {}
        ParsingContext* ctx;
        Cm::Ast::TryStatementNode* value;
        Cm::Ast::CompoundStatementNode* fromCompoundStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::HandlersRule : public Cm::Parsing::Rule
{
public:
    HandlersRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::TryStatementNode*", "tryS"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> tryS_value = std::move(stack.top());
        context.tryS = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::TryStatementNode*>*>(tryS_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<HandlersRule>(this, &HandlersRule::A0Action));
        Cm::Parsing::NonterminalParser* handlerNonterminalParser = GetNonterminal("Handler");
        handlerNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<HandlersRule>(this, &HandlersRule::PreHandler));
        handlerNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<HandlersRule>(this, &HandlersRule::PostHandler));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.tryS->AddHandler(context.fromHandler);
    }
    void PreHandler(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostHandler(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromHandler_value = std::move(stack.top());
            context.fromHandler = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CatchNode*>*>(fromHandler_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), tryS(), fromHandler() {}
        ParsingContext* ctx;
        Cm::Ast::TryStatementNode* tryS;
        Cm::Ast::CatchNode* fromHandler;
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
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::CatchNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CatchNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<HandlerRule>(this, &HandlerRule::A0Action));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<HandlerRule>(this, &HandlerRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<HandlerRule>(this, &HandlerRule::PostTypeExpr));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<HandlerRule>(this, &HandlerRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal("CompoundStatement");
        compoundStatementNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<HandlerRule>(this, &HandlerRule::PreCompoundStatement));
        compoundStatementNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<HandlerRule>(this, &HandlerRule::PostCompoundStatement));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new CatchNode(span, context.fromTypeExpr, context.fromIdentifier, context.fromCompoundStatement);
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
        Context(): ctx(), value(), fromTypeExpr(), fromIdentifier(), fromCompoundStatement() {}
        ParsingContext* ctx;
        Cm::Ast::CatchNode* value;
        Cm::Ast::Node* fromTypeExpr;
        Cm::Ast::IdentifierNode* fromIdentifier;
        Cm::Ast::CompoundStatementNode* fromCompoundStatement;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::AssertStatementRule : public Cm::Parsing::Rule
{
public:
    AssertStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::StatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AssertStatementRule>(this, &AssertStatementRule::A0Action));
        Cm::Parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal("Expression");
        expressionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<AssertStatementRule>(this, &AssertStatementRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AssertStatementRule>(this, &AssertStatementRule::PostExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new AssertStatementNode(span, context.fromExpression);
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
        Cm::Ast::StatementNode* value;
        Cm::Ast::Node* fromExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ConditionalCompilationStatementRule : public Cm::Parsing::Rule
{
public:
    ConditionalCompilationStatementRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::CondCompStatementNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CondCompStatementNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalCompilationStatementRule>(this, &ConditionalCompilationStatementRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalCompilationStatementRule>(this, &ConditionalCompilationStatementRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalCompilationStatementRule>(this, &ConditionalCompilationStatementRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalCompilationStatementRule>(this, &ConditionalCompilationStatementRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalCompilationStatementRule>(this, &ConditionalCompilationStatementRule::A4Action));
        Cm::Parsing::NonterminalParser* ifExprNonterminalParser = GetNonterminal("ifExpr");
        ifExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalCompilationStatementRule>(this, &ConditionalCompilationStatementRule::PostifExpr));
        Cm::Parsing::NonterminalParser* ifSNonterminalParser = GetNonterminal("ifS");
        ifSNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConditionalCompilationStatementRule>(this, &ConditionalCompilationStatementRule::PreifS));
        ifSNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalCompilationStatementRule>(this, &ConditionalCompilationStatementRule::PostifS));
        Cm::Parsing::NonterminalParser* elifExprNonterminalParser = GetNonterminal("elifExpr");
        elifExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalCompilationStatementRule>(this, &ConditionalCompilationStatementRule::PostelifExpr));
        Cm::Parsing::NonterminalParser* elifSNonterminalParser = GetNonterminal("elifS");
        elifSNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConditionalCompilationStatementRule>(this, &ConditionalCompilationStatementRule::PreelifS));
        elifSNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalCompilationStatementRule>(this, &ConditionalCompilationStatementRule::PostelifS));
        Cm::Parsing::NonterminalParser* elseSNonterminalParser = GetNonterminal("elseS");
        elseSNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConditionalCompilationStatementRule>(this, &ConditionalCompilationStatementRule::PreelseS));
        elseSNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalCompilationStatementRule>(this, &ConditionalCompilationStatementRule::PostelseS));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new CondCompStatementNode(span, context.fromifExpr);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->AddIfStatement(context.fromifS);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->AddElifExpr(span, context.fromelifExpr);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->AddElifStatement(context.fromelifS);
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->AddElseStatement(span, context.fromelseS);
    }
    void PostifExpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromifExpr_value = std::move(stack.top());
            context.fromifExpr = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CondCompExprNode*>*>(fromifExpr_value.get());
            stack.pop();
        }
    }
    void PreifS(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostifS(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromifS_value = std::move(stack.top());
            context.fromifS = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromifS_value.get());
            stack.pop();
        }
    }
    void PostelifExpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromelifExpr_value = std::move(stack.top());
            context.fromelifExpr = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CondCompExprNode*>*>(fromelifExpr_value.get());
            stack.pop();
        }
    }
    void PreelifS(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostelifS(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromelifS_value = std::move(stack.top());
            context.fromelifS = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromelifS_value.get());
            stack.pop();
        }
    }
    void PreelseS(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostelseS(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromelseS_value = std::move(stack.top());
            context.fromelseS = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::StatementNode*>*>(fromelseS_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromifExpr(), fromifS(), fromelifExpr(), fromelifS(), fromelseS() {}
        ParsingContext* ctx;
        Cm::Ast::CondCompStatementNode* value;
        Cm::Ast::CondCompExprNode* fromifExpr;
        Cm::Ast::StatementNode* fromifS;
        Cm::Ast::CondCompExprNode* fromelifExpr;
        Cm::Ast::StatementNode* fromelifS;
        Cm::Ast::StatementNode* fromelseS;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ConditionalCompilationExprRule : public Cm::Parsing::Rule
{
public:
    ConditionalCompilationExprRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::CondCompExprNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CondCompExprNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalCompilationExprRule>(this, &ConditionalCompilationExprRule::A0Action));
        Cm::Parsing::NonterminalParser* conditionalCompilationDisjunctionNonterminalParser = GetNonterminal("ConditionalCompilationDisjunction");
        conditionalCompilationDisjunctionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalCompilationExprRule>(this, &ConditionalCompilationExprRule::PostConditionalCompilationDisjunction));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromConditionalCompilationDisjunction;
    }
    void PostConditionalCompilationDisjunction(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConditionalCompilationDisjunction_value = std::move(stack.top());
            context.fromConditionalCompilationDisjunction = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CondCompExprNode*>*>(fromConditionalCompilationDisjunction_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromConditionalCompilationDisjunction() {}
        Cm::Ast::CondCompExprNode* value;
        Cm::Ast::CondCompExprNode* fromConditionalCompilationDisjunction;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ConditionalCompilationDisjunctionRule : public Cm::Parsing::Rule
{
public:
    ConditionalCompilationDisjunctionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::CondCompExprNode*");
        AddLocalVariable(AttrOrVariable("Span", "s"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CondCompExprNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalCompilationDisjunctionRule>(this, &ConditionalCompilationDisjunctionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalCompilationDisjunctionRule>(this, &ConditionalCompilationDisjunctionRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalCompilationDisjunctionRule>(this, &ConditionalCompilationDisjunctionRule::Postleft));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalCompilationDisjunctionRule>(this, &ConditionalCompilationDisjunctionRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.s = span;
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.s.SetEnd(span.End());
        context.value = new CondCompDisjunctionNode(context.s, context.value, context.fromright);
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CondCompExprNode*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CondCompExprNode*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), s(), fromleft(), fromright() {}
        Cm::Ast::CondCompExprNode* value;
        Span s;
        Cm::Ast::CondCompExprNode* fromleft;
        Cm::Ast::CondCompExprNode* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ConditionalCompilationConjunctionRule : public Cm::Parsing::Rule
{
public:
    ConditionalCompilationConjunctionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::CondCompExprNode*");
        AddLocalVariable(AttrOrVariable("Span", "s"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CondCompExprNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalCompilationConjunctionRule>(this, &ConditionalCompilationConjunctionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalCompilationConjunctionRule>(this, &ConditionalCompilationConjunctionRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalCompilationConjunctionRule>(this, &ConditionalCompilationConjunctionRule::Postleft));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalCompilationConjunctionRule>(this, &ConditionalCompilationConjunctionRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.s = span;
        context.value = context.fromleft;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.s.SetEnd(span.End());
        context.value = new CondCompConjunctionNode(context.s, context.value, context.fromright);
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CondCompExprNode*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CondCompExprNode*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), s(), fromleft(), fromright() {}
        Cm::Ast::CondCompExprNode* value;
        Span s;
        Cm::Ast::CondCompExprNode* fromleft;
        Cm::Ast::CondCompExprNode* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ConditionalCompilationPrefixRule : public Cm::Parsing::Rule
{
public:
    ConditionalCompilationPrefixRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::CondCompExprNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CondCompExprNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalCompilationPrefixRule>(this, &ConditionalCompilationPrefixRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalCompilationPrefixRule>(this, &ConditionalCompilationPrefixRule::A1Action));
        Cm::Parsing::NonterminalParser* leftNonterminalParser = GetNonterminal("left");
        leftNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalCompilationPrefixRule>(this, &ConditionalCompilationPrefixRule::Postleft));
        Cm::Parsing::NonterminalParser* rightNonterminalParser = GetNonterminal("right");
        rightNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalCompilationPrefixRule>(this, &ConditionalCompilationPrefixRule::Postright));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new CondCompNotNode(span, context.fromleft);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromright;
    }
    void Postleft(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromleft_value = std::move(stack.top());
            context.fromleft = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CondCompExprNode*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromright_value = std::move(stack.top());
            context.fromright = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CondCompExprNode*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromleft(), fromright() {}
        Cm::Ast::CondCompExprNode* value;
        Cm::Ast::CondCompExprNode* fromleft;
        Cm::Ast::CondCompExprNode* fromright;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ConditionalCompilationPrimaryRule : public Cm::Parsing::Rule
{
public:
    ConditionalCompilationPrimaryRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::CondCompExprNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CondCompExprNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalCompilationPrimaryRule>(this, &ConditionalCompilationPrimaryRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalCompilationPrimaryRule>(this, &ConditionalCompilationPrimaryRule::A1Action));
        Cm::Parsing::NonterminalParser* symbolNonterminalParser = GetNonterminal("symbol");
        symbolNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalCompilationPrimaryRule>(this, &ConditionalCompilationPrimaryRule::Postsymbol));
        Cm::Parsing::NonterminalParser* exprNonterminalParser = GetNonterminal("expr");
        exprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalCompilationPrimaryRule>(this, &ConditionalCompilationPrimaryRule::Postexpr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new CondCompPrimaryNode(span, context.fromsymbol);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromexpr;
    }
    void Postsymbol(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromsymbol_value = std::move(stack.top());
            context.fromsymbol = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CondCompSymbolNode*>*>(fromsymbol_value.get());
            stack.pop();
        }
    }
    void Postexpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromexpr_value = std::move(stack.top());
            context.fromexpr = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CondCompExprNode*>*>(fromexpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromsymbol(), fromexpr() {}
        Cm::Ast::CondCompExprNode* value;
        Cm::Ast::CondCompSymbolNode* fromsymbol;
        Cm::Ast::CondCompExprNode* fromexpr;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::ConditionalCompilationSymbolRule : public Cm::Parsing::Rule
{
public:
    ConditionalCompilationSymbolRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::CondCompSymbolNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CondCompSymbolNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConditionalCompilationSymbolRule>(this, &ConditionalCompilationSymbolRule::A0Action));
        Cm::Parsing::NonterminalParser* symbolNonterminalParser = GetNonterminal("Symbol");
        symbolNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConditionalCompilationSymbolRule>(this, &ConditionalCompilationSymbolRule::PostSymbol));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new CondCompSymbolNode(span, context.fromSymbol);
    }
    void PostSymbol(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromSymbol_value = std::move(stack.top());
            context.fromSymbol = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromSymbol_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromSymbol() {}
        Cm::Ast::CondCompSymbolNode* value;
        std::string fromSymbol;
    };
    std::stack<Context> contextStack;
    Context context;
};

class StatementGrammar::SymbolRule : public Cm::Parsing::Rule
{
public:
    SymbolRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SymbolRule>(this, &SymbolRule::A0Action));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SymbolRule>(this, &SymbolRule::Postidentifier));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = std::string(matchBegin, matchEnd);
    }
    void Postidentifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromidentifier_value = std::move(stack.top());
            context.fromidentifier = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromidentifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromidentifier() {}
        std::string value;
        std::string fromidentifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

void StatementGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parser.ExpressionGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parser::ExpressionGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar1)
    {
        grammar1 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar1);
    Cm::Parsing::Grammar* grammar2 = pd->GetGrammar("Cm.Parser.KeywordGrammar");
    if (!grammar2)
    {
        grammar2 = Cm::Parser::KeywordGrammar::Create(pd);
    }
    AddGrammarReference(grammar2);
    Cm::Parsing::Grammar* grammar3 = pd->GetGrammar("Cm.Parser.TypeExprGrammar");
    if (!grammar3)
    {
        grammar3 = Cm::Parser::TypeExprGrammar::Create(pd);
    }
    AddGrammarReference(grammar3);
    Cm::Parsing::Grammar* grammar4 = pd->GetGrammar("Cm.Parser.IdentifierGrammar");
    if (!grammar4)
    {
        grammar4 = Cm::Parser::IdentifierGrammar::Create(pd);
    }
    AddGrammarReference(grammar4);
}

void StatementGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("Expression", this, "ExpressionGrammar.Expression"));
    AddRuleLink(new Cm::Parsing::RuleLink("identifier", this, "Cm.Parsing.stdlib.identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("ArgumentList", this, "ExpressionGrammar.ArgumentList"));
    AddRuleLink(new Cm::Parsing::RuleLink("Keyword", this, "KeywordGrammar.Keyword"));
    AddRuleLink(new Cm::Parsing::RuleLink("TypeExpr", this, "TypeExprGrammar.TypeExpr"));
    AddRuleLink(new Cm::Parsing::RuleLink("Identifier", this, "IdentifierGrammar.Identifier"));
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
                                        new Cm::Parsing::AlternativeParser(
                                            new Cm::Parsing::AlternativeParser(
                                                new Cm::Parsing::AlternativeParser(
                                                    new Cm::Parsing::ActionParser("A0",
                                                        new Cm::Parsing::NonterminalParser("LabeledStatement", "LabeledStatement", 1)),
                                                    new Cm::Parsing::ActionParser("A1",
                                                        new Cm::Parsing::NonterminalParser("ControlStatement", "ControlStatement", 1))),
                                                new Cm::Parsing::ActionParser("A2",
                                                    new Cm::Parsing::NonterminalParser("TypedefStatement", "TypedefStatement", 1))),
                                            new Cm::Parsing::ActionParser("A3",
                                                new Cm::Parsing::NonterminalParser("SimpleStatement", "SimpleStatement", 1))),
                                        new Cm::Parsing::ActionParser("A4",
                                            new Cm::Parsing::NonterminalParser("AssignmentStatement", "AssignmentStatement", 1))),
                                    new Cm::Parsing::ActionParser("A5",
                                        new Cm::Parsing::NonterminalParser("ConstructionStatement", "ConstructionStatement", 1))),
                                new Cm::Parsing::ActionParser("A6",
                                    new Cm::Parsing::NonterminalParser("DeleteStatement", "DeleteStatement", 1))),
                            new Cm::Parsing::ActionParser("A7",
                                new Cm::Parsing::NonterminalParser("DestroyStatement", "DestroyStatement", 1))),
                        new Cm::Parsing::ActionParser("A8",
                            new Cm::Parsing::NonterminalParser("ThrowStatement", "ThrowStatement", 1))),
                    new Cm::Parsing::ActionParser("A9",
                        new Cm::Parsing::NonterminalParser("TryStatement", "TryStatement", 1))),
                new Cm::Parsing::ActionParser("A10",
                    new Cm::Parsing::NonterminalParser("AssertStatement", "AssertStatement", 1))),
            new Cm::Parsing::ActionParser("A11",
                new Cm::Parsing::NonterminalParser("ConditionalCompilationStatement", "ConditionalCompilationStatement", 1)))));
    AddRule(new LabelIdRule("LabelId", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::DifferenceParser(
                new Cm::Parsing::NonterminalParser("identifier", "identifier", 0),
                new Cm::Parsing::NonterminalParser("Keyword", "Keyword", 0)))));
    AddRule(new LabelRule("Label", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::NonterminalParser("LabelId", "LabelId", 0)),
                new Cm::Parsing::CharParser(':')))));
    AddRule(new TargetLabelRule("TargetLabel", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("LabelId", "LabelId", 0)))));
    AddRule(new LabeledStatementRule("LabeledStatement", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::NonterminalParser("Label", "Label", 0),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("Statement", "Statement", 1)))));
    AddRule(new SimpleStatementRule("SimpleStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::EmptyParser()),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::NonterminalParser("Expression", "Expression", 1)))),
                new Cm::Parsing::CharParser(';')))));
    AddRule(new ControlStatementRule("ControlStatement", GetScope(),
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
                                                        new Cm::Parsing::ActionParser("A0",
                                                            new Cm::Parsing::NonterminalParser("ReturnStatement", "ReturnStatement", 1)),
                                                        new Cm::Parsing::ActionParser("A1",
                                                            new Cm::Parsing::NonterminalParser("ConditionalStatement", "ConditionalStatement", 1))),
                                                    new Cm::Parsing::ActionParser("A2",
                                                        new Cm::Parsing::NonterminalParser("SwitchStatement", "SwitchStatement", 1))),
                                                new Cm::Parsing::ActionParser("A3",
                                                    new Cm::Parsing::NonterminalParser("WhileStatement", "WhileStatement", 1))),
                                            new Cm::Parsing::ActionParser("A4",
                                                new Cm::Parsing::NonterminalParser("DoStatement", "DoStatement", 1))),
                                        new Cm::Parsing::ActionParser("A5",
                                            new Cm::Parsing::NonterminalParser("RangeForStatement", "RangeForStatement", 1))),
                                    new Cm::Parsing::ActionParser("A6",
                                        new Cm::Parsing::NonterminalParser("ForStatement", "ForStatement", 1))),
                                new Cm::Parsing::ActionParser("A7",
                                    new Cm::Parsing::NonterminalParser("CompoundStatement", "CompoundStatement", 1))),
                            new Cm::Parsing::ActionParser("A8",
                                new Cm::Parsing::NonterminalParser("BreakStatement", "BreakStatement", 1))),
                        new Cm::Parsing::ActionParser("A9",
                            new Cm::Parsing::NonterminalParser("ContinueStatement", "ContinueStatement", 1))),
                    new Cm::Parsing::ActionParser("A10",
                        new Cm::Parsing::NonterminalParser("GotoCaseStatement", "GotoCaseStatement", 1))),
                new Cm::Parsing::ActionParser("A11",
                    new Cm::Parsing::NonterminalParser("GotoDefaultStatement", "GotoDefaultStatement", 1))),
            new Cm::Parsing::ActionParser("A12",
                new Cm::Parsing::NonterminalParser("GotoStatement", "GotoStatement", 1)))));
    AddRule(new ReturnStatementRule("ReturnStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("return"),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("Expression", "Expression", 1))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new ConditionalStatementRule("ConditionalStatement", GetScope(),
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
                                new Cm::Parsing::NonterminalParser("Expression", "Expression", 1))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser(')'))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("thenS", "Statement", 1))),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("else"),
                        new Cm::Parsing::NonterminalParser("elseS", "Statement", 1)))))));
    AddRule(new SwitchStatementRule("SwitchStatement", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::ActionParser("A0",
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::KeywordParser("switch"),
                                    new Cm::Parsing::ExpectationParser(
                                        new Cm::Parsing::CharParser('('))),
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::NonterminalParser("Expression", "Expression", 1)))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser(')'))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser('{'))),
                new Cm::Parsing::KleeneStarParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::NonterminalParser("CaseStatement", "CaseStatement", 1)),
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::NonterminalParser("DefaultStatement", "DefaultStatement", 1))))),
            new Cm::Parsing::ActionParser("A3",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser('}'))))));
    AddRule(new CaseStatementRule("CaseStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::EmptyParser()),
                    new Cm::Parsing::ActionParser("A2",
                        new Cm::Parsing::NonterminalParser("CaseList", "CaseList", 2))),
                new Cm::Parsing::KleeneStarParser(
                    new Cm::Parsing::ActionParser("A3",
                        new Cm::Parsing::NonterminalParser("Statement", "Statement", 1)))))));
    AddRule(new DefaultStatementRule("DefaultStatement", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("default"),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser(':')))),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::NonterminalParser("Statement", "Statement", 1))))));
    AddRule(new CaseListRule("CaseList", GetScope(),
        new Cm::Parsing::PositiveParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("case"),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("Expression", "Expression", 1))),
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser(':')))))));
    AddRule(new GotoCaseStatementRule("GotoCaseStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("goto"),
                        new Cm::Parsing::KeywordParser("case")),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("Expression", "Expression", 1))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new GotoDefaultStatementRule("GotoDefaultStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("goto"),
                    new Cm::Parsing::KeywordParser("default")),
                new Cm::Parsing::CharParser(';')))));
    AddRule(new WhileStatementRule("WhileStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::KeywordParser("while"),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::CharParser('('))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("Expression", "Expression", 1))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser(')'))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("Statement", "Statement", 1))))));
    AddRule(new DoStatementRule("DoStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::KeywordParser("do"),
                                    new Cm::Parsing::ExpectationParser(
                                        new Cm::Parsing::NonterminalParser("Statement", "Statement", 1))),
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::KeywordParser("while"))),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::CharParser('('))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("Expression", "Expression", 1))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser(')'))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new RangeForStatementRule("RangeForStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::KeywordParser("for"),
                                        new Cm::Parsing::CharParser('(')),
                                    new Cm::Parsing::ActionParser("A1",
                                        new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 1))),
                                new Cm::Parsing::ActionParser("A2",
                                    new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0))),
                            new Cm::Parsing::CharParser(':')),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("Expression", "Expression", 1))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser(')'))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("Statement", "Statement", 1))))));
    AddRule(new ForStatementRule("ForStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
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
                                        new Cm::Parsing::NonterminalParser("ForInitStatement", "ForInitStatement", 1))),
                                new Cm::Parsing::OptionalParser(
                                    new Cm::Parsing::NonterminalParser("condition", "Expression", 1))),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::CharParser(';'))),
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::NonterminalParser("increment", "Expression", 1))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser(')'))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("Statement", "Statement", 1))))));
    AddRule(new ForInitStatementRule("ForInitStatement", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("AssignmentStatement", "AssignmentStatement", 1)),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::NonterminalParser("ConstructionStatement", "ConstructionStatement", 1))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::CharParser(';')))));
    AddRule(new CompoundStatementRule("CompoundStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::CharParser('{')),
                    new Cm::Parsing::KleeneStarParser(
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::NonterminalParser("Statement", "Statement", 1)))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::ActionParser("A3",
                        new Cm::Parsing::CharParser('}')))))));
    AddRule(new BreakStatementRule("BreakStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::KeywordParser("break"),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new ContinueStatementRule("ContinueStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::KeywordParser("continue"),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new GotoStatementRule("GotoStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("goto"),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("TargetLabel", "TargetLabel", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new TypedefStatementRule("TypedefStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("typedef"),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 1))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new AssignmentStatementRule("AssignmentStatement", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::EmptyParser()),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::ActionParser("A2",
                                new Cm::Parsing::NonterminalParser("target", "Expression", 1)),
                            new Cm::Parsing::CharParser('=')),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("source", "Expression", 1))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser(';')))))));
    AddRule(new ConstructionStatementRule("ConstructionStatement", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 1),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0)))),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::CharParser('('),
                                new Cm::Parsing::NonterminalParser("ArgumentList", "ArgumentList", 2)),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::CharParser(')'))),
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::CharParser('='),
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::NonterminalParser("Expression", "Expression", 1))))))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new DeleteStatementRule("DeleteStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("delete"),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("Expression", "Expression", 1))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new DestroyStatementRule("DestroyStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("destroy"),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("Expression", "Expression", 1))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new ThrowStatementRule("ThrowStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("throw"),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("Expression", "Expression", 1))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new TryStatementRule("TryStatement", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("try"),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("CompoundStatement", "CompoundStatement", 1)))),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("Handlers", "Handlers", 2))))));
    AddRule(new HandlersRule("Handlers", GetScope(),
        new Cm::Parsing::PositiveParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("Handler", "Handler", 1)))));
    AddRule(new HandlerRule("Handler", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::KeywordParser("catch"),
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::CharParser('('))),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 1))),
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser(')'))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("CompoundStatement", "CompoundStatement", 1))))));
    AddRule(new AssertStatementRule("AssertStatement", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::CharParser('#'),
                        new Cm::Parsing::KeywordParser("assert")),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("Expression", "Expression", 1))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new ConditionalCompilationStatementRule("ConditionalCompilationStatement", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::ActionParser("A0",
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::SequenceParser(
                                            new Cm::Parsing::SequenceParser(
                                                new Cm::Parsing::CharParser('#'),
                                                new Cm::Parsing::KeywordParser("if")),
                                            new Cm::Parsing::ExpectationParser(
                                                new Cm::Parsing::CharParser('('))),
                                        new Cm::Parsing::NonterminalParser("ifExpr", "ConditionalCompilationExpr", 0))),
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::CharParser(')'))),
                            new Cm::Parsing::KleeneStarParser(
                                new Cm::Parsing::ActionParser("A1",
                                    new Cm::Parsing::NonterminalParser("ifS", "Statement", 1)))),
                        new Cm::Parsing::KleeneStarParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::ActionParser("A2",
                                        new Cm::Parsing::SequenceParser(
                                            new Cm::Parsing::SequenceParser(
                                                new Cm::Parsing::SequenceParser(
                                                    new Cm::Parsing::CharParser('#'),
                                                    new Cm::Parsing::KeywordParser("elif")),
                                                new Cm::Parsing::ExpectationParser(
                                                    new Cm::Parsing::CharParser('('))),
                                            new Cm::Parsing::NonterminalParser("elifExpr", "ConditionalCompilationExpr", 0))),
                                    new Cm::Parsing::ExpectationParser(
                                        new Cm::Parsing::CharParser(')'))),
                                new Cm::Parsing::KleeneStarParser(
                                    new Cm::Parsing::ActionParser("A3",
                                        new Cm::Parsing::NonterminalParser("elifS", "Statement", 1)))))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::CharParser('#'),
                                new Cm::Parsing::KeywordParser("else")),
                            new Cm::Parsing::KleeneStarParser(
                                new Cm::Parsing::ActionParser("A4",
                                    new Cm::Parsing::NonterminalParser("elseS", "Statement", 1)))))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser('#'))),
            new Cm::Parsing::ExpectationParser(
                new Cm::Parsing::KeywordParser("endif")))));
    AddRule(new ConditionalCompilationExprRule("ConditionalCompilationExpr", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("ConditionalCompilationDisjunction", "ConditionalCompilationDisjunction", 0))));
    AddRule(new ConditionalCompilationDisjunctionRule("ConditionalCompilationDisjunction", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "ConditionalCompilationConjunction", 0)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::StringParser("||"),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("right", "ConditionalCompilationConjunction", 0)))))));
    AddRule(new ConditionalCompilationConjunctionRule("ConditionalCompilationConjunction", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("left", "ConditionalCompilationPrefix", 0)),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::StringParser("&&"),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("right", "ConditionalCompilationPrefix", 0)))))));
    AddRule(new ConditionalCompilationPrefixRule("ConditionalCompilationPrefix", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('!'),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("left", "ConditionalCompilationPrefix", 0)))),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("right", "ConditionalCompilationPrimary", 0))))));
    AddRule(new ConditionalCompilationPrimaryRule("ConditionalCompilationPrimary", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("symbol", "ConditionalCompilationSymbol", 0)),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("expr", "ConditionalCompilationExpr", 0)))));
    AddRule(new ConditionalCompilationSymbolRule("ConditionalCompilationSymbol", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("Symbol", "Symbol", 0))));
    AddRule(new SymbolRule("Symbol", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::DifferenceParser(
                new Cm::Parsing::NonterminalParser("identifier", "identifier", 0),
                new Cm::Parsing::NonterminalParser("Keyword", "Keyword", 0)))));
    SetStartRuleName("CompoundStatement");
    SetSkipRuleName("spaces_and_comments");
    SetRecover();
}

} } // namespace Cm.Parser
