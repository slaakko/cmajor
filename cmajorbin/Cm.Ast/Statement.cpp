/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Statement.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>

namespace Cm { namespace Ast {

StatementNodeList::StatementNodeList()
{
}

void StatementNodeList::Read(Reader& reader)
{
    uint32_t n = reader.ReadUInt();
    for (uint32_t i = 0; i < n; ++i)
    {
        statementNodes.push_back(std::unique_ptr<StatementNode>(reader.ReadStatementNode()));
    }
}

void StatementNodeList::Write(Writer& writer)
{
    uint32_t n = static_cast<uint32_t>(statementNodes.size());
    writer.Write(n);
    for (uint32_t i = 0; i < n; ++i)
    {
        writer.Write(statementNodes[i].get());
    }
}

LabelNode::LabelNode(const Span& span_) : Node(span_)
{
}

LabelNode::LabelNode(const Span& span_, const std::string& label_): Node(span_), label(label_)
{
}

Node* LabelNode::Clone() const
{
    return new LabelNode(GetSpan(), label);
}

void LabelNode::Read(Reader& reader)
{
    label = reader.ReadString();
}

void LabelNode::Write(Writer& writer)
{
    writer.Write(label);
}

StatementNode::StatementNode(const Span& span_) : Node(span_)
{
}

void StatementNode::Read(Reader& reader)
{
    bool hasLabel = reader.ReadBool();
    if (hasLabel)
    {
        labelNode.reset(reader.ReadLabelNode());
    }
}

void StatementNode::Write(Writer& writer)
{
    bool hasLabel = labelNode != nullptr;
    writer.Write(hasLabel);
    if (hasLabel)
    {
        writer.Write(labelNode.get());
    }
}

void StatementNode::SetLabelNode(LabelNode* labelNode_)
{
    labelNode.reset(labelNode_);
}

void StatementNode::CloneLabelTo(StatementNode* clone) const
{
    if (labelNode)
    {
        clone->SetLabelNode(static_cast<LabelNode*>(labelNode->Clone()));
    }
}

SimpleStatementNode::SimpleStatementNode(const Span& span_) : StatementNode(span_)
{
}

SimpleStatementNode::SimpleStatementNode(const Span& span_, Node* expr_) : StatementNode(span_), expr(expr_)
{
}

Node* SimpleStatementNode::Clone() const
{
    Node* clonedExpr = nullptr;
    if (expr)
    {
        clonedExpr = expr->Clone();
    }
    SimpleStatementNode* clone = new SimpleStatementNode(GetSpan(), clonedExpr);
    CloneLabelTo(clone);
    return clone;
}

void SimpleStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    bool hasExpr = reader.ReadBool();
    if (hasExpr)
    {
        expr.reset(reader.ReadNode());
    }
}

void SimpleStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    bool hasExpr = expr != nullptr;
    writer.Write(hasExpr);
    if (hasExpr)
    {
        writer.Write(expr.get());
    }
}

ReturnStatementNode::ReturnStatementNode(const Span& span_) : StatementNode(span_)
{
}

ReturnStatementNode::ReturnStatementNode(const Span& span_, Node* expr_) : StatementNode(span_), expr(expr_)
{
}

Node* ReturnStatementNode::Clone() const
{
    Node* clonedExpr = nullptr;
    if (expr)
    {
        clonedExpr = expr->Clone();
    }
    ReturnStatementNode* clone = new ReturnStatementNode(GetSpan(), clonedExpr);
    CloneLabelTo(clone);
    return clone;
}

void ReturnStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    bool hasExpr = reader.ReadBool();
    if (hasExpr)
    {
        expr.reset(reader.ReadNode());
    }
}

void ReturnStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    bool hasExpr = expr != nullptr;
    writer.Write(hasExpr);
    if (hasExpr)
    {
        writer.Write(expr.get());
    }
}

ConditionalStatement::ConditionalStatement(const Span& span_) : StatementNode(span_)
{
}

ConditionalStatement::ConditionalStatement(const Span& span_, Node* condition_, StatementNode* thenS_, StatementNode* elseS_) : StatementNode(span_), condition(condition_), thenS(thenS_), elseS(elseS_)
{
}

Node* ConditionalStatement::Clone() const
{
    StatementNode* clonedElseS = nullptr;
    if (elseS)
    {
        clonedElseS = static_cast<StatementNode*>(elseS->Clone());
    }
    ConditionalStatement* clone = new ConditionalStatement(GetSpan(), condition->Clone(), static_cast<StatementNode*>(thenS->Clone()), clonedElseS);
    CloneLabelTo(clone);
    return clone;
}

void ConditionalStatement::Read(Reader& reader)
{
    StatementNode::Read(reader);
    condition.reset(reader.ReadNode());
    thenS.reset(reader.ReadStatementNode());
    bool hasElseS = reader.ReadBool();
    if (hasElseS)
    {
        elseS.reset(reader.ReadStatementNode());
    }
}

void ConditionalStatement::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(condition.get());
    writer.Write(thenS.get());
    bool hasElseS = elseS != nullptr;
    writer.Write(hasElseS);
    if (hasElseS)
    {
        writer.Write(elseS.get());
    }
}

SwitchStatementNode::SwitchStatementNode(const Span& span_) : StatementNode(span_)
{
}

SwitchStatementNode::SwitchStatementNode(const Span& span_, Node* condition_) : StatementNode(span_), condition(condition_)
{
}

void SwitchStatementNode::AddCase(StatementNode* caseS)
{
    caseStatements.Add(caseS);
}

void SwitchStatementNode::SetDefault(StatementNode* defaultS)
{
    if (defaultStatement != nullptr)
    {
        throw std::runtime_error("already has default statement");
    }
    defaultStatement.reset(defaultS);
}

Node* SwitchStatementNode::Clone() const
{
    SwitchStatementNode* clone = new SwitchStatementNode(GetSpan(), condition->Clone());
    for (const std::unique_ptr<StatementNode>& caseS : caseStatements)
    {
        clone->AddCase(static_cast<StatementNode*>(caseS->Clone()));
    }
    if (defaultStatement)
    {
        clone->SetDefault(static_cast<StatementNode*>(defaultStatement->Clone()));
    }
    CloneLabelTo(clone);
    return clone;
}

void SwitchStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    condition.reset(reader.ReadNode());
    caseStatements.Read(reader);
    bool hasDefaultS = reader.ReadBool();
    if (hasDefaultS)
    {
        defaultStatement.reset(reader.ReadStatementNode());
    }
}

void SwitchStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(condition.get());
    caseStatements.Write(writer);
    bool hasDefaultS = defaultStatement != nullptr;
    writer.Write(hasDefaultS);
    if (hasDefaultS)
    {
        writer.Write(defaultStatement.get());
    }
}

CaseStatementNode::CaseStatementNode(const Span& span_) : StatementNode(span_)
{
}

void CaseStatementNode::AddExpr(Node* expr)
{
    expressions.Add(expr);
}

void CaseStatementNode::AddStatement(StatementNode* statement)
{
    statements.Add(statement);
}

Node* CaseStatementNode::Clone() const
{
    CaseStatementNode* clone = new CaseStatementNode(GetSpan());
    for (const std::unique_ptr<Node>& expr : expressions)
    {
        clone->AddExpr(expr->Clone());
    }
    for (const std::unique_ptr<StatementNode>& statement : statements)
    {
        clone->AddStatement(static_cast<StatementNode*>(statement->Clone()));
    }
    CloneLabelTo(clone);
    return clone;
}

void CaseStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    expressions.Read(reader);
    statements.Read(reader);
}

void CaseStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    expressions.Write(writer);
    statements.Write(writer);
}

DefaultStatementNode::DefaultStatementNode(const Span& span_) : StatementNode(span_)
{
}

void DefaultStatementNode::AddStatement(StatementNode* statement)
{
    statements.Add(statement);
}

Node* DefaultStatementNode::Clone() const
{
    DefaultStatementNode* clone = new DefaultStatementNode(GetSpan());
    for (const std::unique_ptr<StatementNode>& statement : statements)
    {
        clone->AddStatement(static_cast<StatementNode*>(statement->Clone()));
    }
    CloneLabelTo(clone);
    return clone;
}

void DefaultStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    statements.Read(reader);
}

void DefaultStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    statements.Write(writer);
}

GotoCaseStatementNode::GotoCaseStatementNode(const Span& span_) : StatementNode(span_)
{
}

GotoCaseStatementNode::GotoCaseStatementNode(const Span& span_, Node* targetCaseExpr_) : StatementNode(span_), targetCaseExpr(targetCaseExpr_)
{
}

Node* GotoCaseStatementNode::Clone() const
{
    GotoCaseStatementNode* clone = new GotoCaseStatementNode(GetSpan(), targetCaseExpr->Clone());
    CloneLabelTo(clone);
    return clone;
}

void GotoCaseStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    targetCaseExpr.reset(reader.ReadNode());
}

void GotoCaseStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(targetCaseExpr.get());
}

GotoDefaultStatementNode::GotoDefaultStatementNode(const Span& span_) : StatementNode(span_)
{
}

Node* GotoDefaultStatementNode::Clone() const
{
    GotoDefaultStatementNode* clone = new GotoDefaultStatementNode(GetSpan());
    CloneLabelTo(clone);
    return clone;
}

WhileStatementNode::WhileStatementNode(const Span& span_) : StatementNode(span_)
{
}

WhileStatementNode::WhileStatementNode(const Span& span_, Node* condition_, StatementNode* statement_) : StatementNode(span_), condition(condition_), statement(statement_)
{
}

Node* WhileStatementNode::Clone() const
{
    WhileStatementNode* clone = new WhileStatementNode(GetSpan(), condition->Clone(), static_cast<StatementNode*>(statement->Clone()));
    CloneLabelTo(clone);
    return clone;
}

void WhileStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    condition.reset(reader.ReadNode());
    statement.reset(reader.ReadStatementNode());
}

void WhileStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(condition.get());
    writer.Write(statement.get());
}

DoStatementNode::DoStatementNode(const Span& span_) : StatementNode(span_)
{
}

DoStatementNode::DoStatementNode(const Span& span_, StatementNode* statement_, Node* condition_) : StatementNode(span_), statement(statement_), condition(condition_)
{
}

Node* DoStatementNode::Clone() const
{
    DoStatementNode* clone = new DoStatementNode(GetSpan(), static_cast<StatementNode*>(statement->Clone()), condition->Clone());
    CloneLabelTo(clone);
    return clone;
}

void DoStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    statement.reset(reader.ReadStatementNode());
    condition.reset(reader.ReadNode());
}

void DoStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(statement.get());
    writer.Write(condition.get());
}

RangeForStatementNode::RangeForStatementNode(const Span& span_) : StatementNode(span_)
{
}

RangeForStatementNode::RangeForStatementNode(const Span& span_, Node* varTypeExpr_, IdentifierNode* varId_, Node* container_, StatementNode* action_) :
    StatementNode(span_), varTypeExpr(varTypeExpr_), varId(varId_), container(container_), action(action_)
{
}

Node* RangeForStatementNode::Clone() const
{
    RangeForStatementNode* clone = new RangeForStatementNode(GetSpan(), varTypeExpr->Clone(), static_cast<IdentifierNode*>(varId->Clone()), container->Clone(), static_cast<StatementNode*>(action->Clone()));
    CloneLabelTo(clone);
    return clone;
}

void RangeForStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    varTypeExpr.reset(reader.ReadNode());
    varId.reset(reader.ReadIdentifierNode());
    container.reset(reader.ReadNode());
    action.reset(reader.ReadStatementNode());
}

void RangeForStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(varTypeExpr.get());
    writer.Write(varId.get());
    writer.Write(container.get());
    writer.Write(action.get());
}

ForStatementNode::ForStatementNode(const Span& span_) : StatementNode(span_)
{
}

ForStatementNode::ForStatementNode(const Span& span_, StatementNode* init_, Node* condition_, Node* increment_, StatementNode* action_) :
    StatementNode(span_), init(init_), condition(condition_), increment(increment_), action(action_)
{
}

Node* ForStatementNode::Clone() const
{
    Node* cloneOfCond = nullptr;
    if (condition)
    {
        cloneOfCond = condition->Clone();
    }
    Node* cloneOfInc = nullptr;
    if (increment)
    {
        cloneOfInc = increment->Clone();
    }
    ForStatementNode* clone = new ForStatementNode(GetSpan(), static_cast<StatementNode*>(init->Clone()), cloneOfCond, cloneOfInc, static_cast<StatementNode*>(action->Clone()));
    CloneLabelTo(clone);
    return clone;
}

void ForStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    init.reset(reader.ReadStatementNode());
    bool hasCond = reader.ReadBool();
    if (hasCond)
    {
        condition.reset(reader.ReadNode());
    }
    bool hasInc = reader.ReadBool();
    if (hasInc)
    {
        increment.reset(reader.ReadNode());
    }
    action.reset(reader.ReadStatementNode());
}

void ForStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(init.get());
    bool hasCond = condition != nullptr;
    writer.Write(hasCond);
    if (hasCond)
    {
        writer.Write(condition.get());
    }
    bool hasInc = increment != nullptr;
    writer.Write(hasInc);
    if (hasInc)
    {
        writer.Write(increment.get());
    }
    writer.Write(action.get());
}

CompoundStatementNode::CompoundStatementNode(const Span& span_) : StatementNode(span_)
{
}

void CompoundStatementNode::AddStatement(StatementNode* statement)
{
    statements.Add(statement);
}

Node* CompoundStatementNode::Clone() const
{
    CompoundStatementNode* clone = new CompoundStatementNode(GetSpan());
    for (const std::unique_ptr<StatementNode>& statement : statements)
    {
        clone->AddStatement(static_cast<StatementNode*>(statement->Clone()));
    }
    CloneLabelTo(clone);
    return clone;
}

void CompoundStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    statements.Read(reader);
}

void CompoundStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    statements.Write(writer);
}

BreakStatementNode::BreakStatementNode(const Span& span_) : StatementNode(span_)
{
}

Node* BreakStatementNode::Clone() const
{
    BreakStatementNode* clone = new BreakStatementNode(GetSpan());
    CloneLabelTo(clone);
    return clone;
}

ContinueStatementNode::ContinueStatementNode(const Span& span_) : StatementNode(span_)
{
}

Node* ContinueStatementNode::Clone() const
{
    ContinueStatementNode* clone = new ContinueStatementNode(GetSpan());
    CloneLabelTo(clone);
    return clone;
}

GotoStatementNode::GotoStatementNode(const Span& span_) : StatementNode(span_)
{
}

GotoStatementNode::GotoStatementNode(const Span& span_, LabelNode* target_) : StatementNode(span_), target(target_)
{
}

Node* GotoStatementNode::Clone() const
{
    GotoStatementNode* clone = new GotoStatementNode(GetSpan(), static_cast<LabelNode*>(target->Clone()));
    CloneLabelTo(clone);
    return clone;
}

void GotoStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    target.reset(reader.ReadLabelNode());
}

void GotoStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(target.get());
}

TypedefStatementNode::TypedefStatementNode(const Span& span_) : StatementNode(span_)
{
}

TypedefStatementNode::TypedefStatementNode(const Span& span_, Node* typeExpr_, IdentifierNode* id_) : StatementNode(span_), typeExpr(typeExpr_), id(id_)
{
}

Node* TypedefStatementNode::Clone() const
{
    TypedefStatementNode* clone = new TypedefStatementNode(GetSpan(), typeExpr->Clone(), static_cast<IdentifierNode*>(id->Clone()));
    CloneLabelTo(clone);
    return clone;
}

void TypedefStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    typeExpr.reset(reader.ReadNode());
    id.reset(reader.ReadIdentifierNode());
}

void TypedefStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(typeExpr.get());
    writer.Write(id.get());
}

AssignmentStatementNode::AssignmentStatementNode(const Span& span_) : StatementNode(span_)
{
}

AssignmentStatementNode::AssignmentStatementNode(const Span& span_, Node* targetExpr_, Node* sourceExpr_) : StatementNode(span_), targetExpr(targetExpr_), sourceExpr(sourceExpr_)
{
}

Node* AssignmentStatementNode::Clone() const
{
    AssignmentStatementNode* clone = new AssignmentStatementNode(GetSpan(), targetExpr->Clone(), sourceExpr->Clone());
    CloneLabelTo(clone);
    return clone;
}

void AssignmentStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    targetExpr.reset(reader.ReadNode());
    sourceExpr.reset(reader.ReadNode());
}

void AssignmentStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(targetExpr.get());
    writer.Write(sourceExpr.get());
}

ConstructionStatementNode::ConstructionStatementNode(const Span& span_) : StatementNode(span_)
{
}

ConstructionStatementNode::ConstructionStatementNode(const Span& span_, Node* typeExpr_, IdentifierNode* id_) : StatementNode(span_), typeExpr(typeExpr_), id(id_)
{
}

void ConstructionStatementNode::AddArgument(Node* argument)
{
    arguments.Add(argument);
}

Node* ConstructionStatementNode::Clone() const
{
    ConstructionStatementNode* clone = new ConstructionStatementNode(GetSpan(), typeExpr->Clone(), static_cast<IdentifierNode*>(id->Clone()));
    for (const std::unique_ptr<Node>& argument : arguments)
    {
        clone->AddArgument(argument->Clone());
    }
    CloneLabelTo(clone);
    return clone;
}

void ConstructionStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    typeExpr.reset(reader.ReadNode());
    id.reset(reader.ReadIdentifierNode());
    arguments.Read(reader);
}

void ConstructionStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(typeExpr.get());
    writer.Write(id.get());
    arguments.Write(writer);
}

DeleteStatementNode::DeleteStatementNode(const Span& span_) : StatementNode(span_)
{
}

DeleteStatementNode::DeleteStatementNode(const Span& span_, Node* pointerExpr_) : StatementNode(span_), pointerExpr(pointerExpr_)
{
}

Node* DeleteStatementNode::Clone() const
{
    DeleteStatementNode* clone = new DeleteStatementNode(GetSpan(), pointerExpr->Clone());
    CloneLabelTo(clone);
    return clone;
}

void DeleteStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    pointerExpr.reset(reader.ReadNode());
}

void DeleteStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(pointerExpr.get());
}

DestroyStatementNode::DestroyStatementNode(const Span& span_) : StatementNode(span_)
{
}

DestroyStatementNode::DestroyStatementNode(const Span& span_, Node* pointerExpr_) : StatementNode(span_), pointerExpr(pointerExpr_)
{
}

Node* DestroyStatementNode::Clone() const
{
    DestroyStatementNode* clone = new DestroyStatementNode(GetSpan(), pointerExpr->Clone());
    CloneLabelTo(clone);
    return clone;
}

void DestroyStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    pointerExpr.reset(reader.ReadNode());
}

void DestroyStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(pointerExpr.get());
}

ThrowStatementNode::ThrowStatementNode(const Span& span_) : StatementNode(span_)
{
}

ThrowStatementNode::ThrowStatementNode(const Span& span_, Node* exceptionExpr_) : StatementNode(span_), exceptionExpr(exceptionExpr_)
{
}

Node* ThrowStatementNode::Clone() const
{
    ThrowStatementNode* clone = new ThrowStatementNode(GetSpan(), exceptionExpr->Clone());
    CloneLabelTo(clone);
    return clone;
}

void ThrowStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    exceptionExpr.reset(reader.ReadNode());
}

void ThrowStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(exceptionExpr.get());
}

TryStatementNode::TryStatementNode(const Span& span_) : StatementNode(span_)
{
}

TryStatementNode::TryStatementNode(const Span& span_, CompoundStatementNode* tryBlock_) : StatementNode(span_), tryBlock(tryBlock_)
{
}

void TryStatementNode::AddHandler(CatchNode* handler)
{
    handlers.Add(handler);
}

Node* TryStatementNode::Clone() const
{
    TryStatementNode* clone = new TryStatementNode(GetSpan(), static_cast<CompoundStatementNode*>(tryBlock->Clone()));
    for (const std::unique_ptr<Node>& handler : handlers)
    {
        clone->AddHandler(static_cast<CatchNode*>(handler->Clone()));
    }
    CloneLabelTo(clone);
    return clone;
}

void TryStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    tryBlock.reset(reader.ReadCompoundStatementNode());
    handlers.Read(reader);
}

void TryStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(tryBlock.get());
    handlers.Write(writer);
}

CatchNode::CatchNode(const Span& span_) : Node(span_) 
{
}

CatchNode::CatchNode(const Span& span_, Node* exceptionTypeExpr_, IdentifierNode* exceptionId_, CompoundStatementNode* catchBlock_) :
    Node(span_), exceptionTypeExpr(exceptionTypeExpr_), exceptionId(exceptionId_), catchBlock(catchBlock_)
{
}

Node* CatchNode::Clone() const
{
    return new CatchNode(GetSpan(), exceptionTypeExpr->Clone(), static_cast<IdentifierNode*>(exceptionId->Clone()), static_cast<CompoundStatementNode*>(catchBlock->Clone()));
}

void CatchNode::Read(Reader& reader)
{
    exceptionTypeExpr.reset(reader.ReadNode());
    exceptionId.reset(reader.ReadIdentifierNode());
    catchBlock.reset(reader.ReadCompoundStatementNode());
}

void CatchNode::Write(Writer& writer) 
{
    writer.Write(exceptionTypeExpr.get());
    writer.Write(exceptionId.get());
    writer.Write(catchBlock.get());
}

AssertStatementNode::AssertStatementNode(const Span& span_) : StatementNode(span_)
{
}

AssertStatementNode::AssertStatementNode(const Span& span_, Node* assertExpr_) : StatementNode(span_), assertExpr(assertExpr_)
{
}

Node* AssertStatementNode::Clone() const
{
    AssertStatementNode* clone = new AssertStatementNode(GetSpan(), assertExpr->Clone());
    CloneLabelTo(clone);
    return clone;
}

void AssertStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    assertExpr.reset(reader.ReadNode());
}

void AssertStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(assertExpr.get());
}

CondCompSymbolNode::CondCompSymbolNode(const Span& span_) : Node(span_)
{
}

CondCompSymbolNode::CondCompSymbolNode(const Span& span_, const std::string& symbol_) : Node(span_), symbol(symbol_)
{
}

Node* CondCompSymbolNode::Clone() const
{
    return new CondCompSymbolNode(GetSpan(), symbol);
}

void CondCompSymbolNode::Read(Reader& reader)
{
    symbol = reader.ReadString();
}

void CondCompSymbolNode::Write(Writer& writer)
{
    writer.Write(symbol);
}

CondCompExprNode::CondCompExprNode(const Span& span_) : Node(span_)
{
}

CondCompBinExprNode::CondCompBinExprNode(const Span& span_) : CondCompExprNode(span_)
{
}

CondCompBinExprNode::CondCompBinExprNode(const Span& span_, CondCompExprNode* left_, CondCompExprNode* right_) : CondCompExprNode(span_), left(left_), right(right_)
{
}

void CondCompBinExprNode::Read(Reader& reader)
{
    left.reset(reader.ReadCondCompExprNode());
    right.reset(reader.ReadCondCompExprNode());
}

void CondCompBinExprNode::Write(Writer& writer)
{
    writer.Write(left.get());
    writer.Write(right.get());
}

CondCompDisjunctionNode::CondCompDisjunctionNode(const Span& span_) : CondCompBinExprNode(span_)
{
}

CondCompDisjunctionNode::CondCompDisjunctionNode(const Span& span_, CondCompExprNode* left_, CondCompExprNode* right_) : CondCompBinExprNode(span_, left_, right_)
{
}

Node* CondCompDisjunctionNode::Clone() const
{
    return new CondCompDisjunctionNode(GetSpan(), static_cast<CondCompExprNode*>(Left()->Clone()), static_cast<CondCompExprNode*>(Right()->Clone()));
}

CondCompConjunctionNode::CondCompConjunctionNode(const Span& span_) : CondCompBinExprNode(span_)
{
}

CondCompConjunctionNode::CondCompConjunctionNode(const Span& span_, CondCompExprNode* left_, CondCompExprNode* right_) : CondCompBinExprNode(span_, left_, right_)
{
}

Node* CondCompConjunctionNode::Clone() const
{
    return new CondCompConjunctionNode(GetSpan(), static_cast<CondCompExprNode*>(Left()->Clone()), static_cast<CondCompExprNode*>(Right()->Clone()));
}

CondCompNotNode::CondCompNotNode(const Span& span_) : CondCompExprNode(span_)
{
}

CondCompNotNode::CondCompNotNode(const Span& span_, CondCompExprNode* subject_) : CondCompExprNode(span_), subject(subject_)
{
}

Node* CondCompNotNode::Clone() const
{
    return new CondCompNotNode(GetSpan(), static_cast<CondCompExprNode*>(subject->Clone()));
}

void CondCompNotNode::Read(Reader& reader)
{
    subject.reset(reader.ReadCondCompExprNode());
}

void CondCompNotNode::Write(Writer& writer)
{
    writer.Write(subject.get());
}

CondCompPrimaryNode::CondCompPrimaryNode(const Span& span_) : CondCompExprNode(span_)
{
}

CondCompPrimaryNode::CondCompPrimaryNode(const Span& span_, CondCompSymbolNode* symbolNode_) : CondCompExprNode(span_), symbolNode(symbolNode_)
{
}

Node* CondCompPrimaryNode::Clone() const
{
    return new CondCompPrimaryNode(GetSpan(), static_cast<CondCompSymbolNode*>(symbolNode->Clone()));
}

void CondCompPrimaryNode::Read(Reader& reader)
{
    symbolNode.reset(reader.ReadCondCompSymbolNode());
}

void CondCompPrimaryNode::Write(Writer& writer)
{
    writer.Write(symbolNode.get());
}

CondCompilationPartNode::CondCompilationPartNode(const Span& span_) : Node(span_)
{
}

CondCompilationPartNode::CondCompilationPartNode(const Span& span_, CondCompExprNode* expr_) : Node(span_), expr(expr_)
{
}

void CondCompilationPartNode::AddStatement(StatementNode* statement)
{
    statements.Add(statement);
}

Node* CondCompilationPartNode::Clone() const
{
    CondCompilationPartNode* clone = new CondCompilationPartNode(GetSpan(), static_cast<CondCompExprNode*>(expr->Clone()));
    for (const std::unique_ptr<StatementNode>& statement : statements)
    {
        clone->AddStatement(static_cast<StatementNode*>(statement->Clone()));
    }
    return clone;
}

void CondCompilationPartNode::Read(Reader& reader)
{
    expr.reset(reader.ReadCondCompExprNode());
    statements.Read(reader);
}

void CondCompilationPartNode::Write(Writer& writer)
{
    writer.Write(expr.get());
    statements.Write(writer);
}

CondCompilationPartNodeList::CondCompilationPartNodeList()
{
}

void CondCompilationPartNodeList::Read(Reader& reader)
{
    uint32_t n = reader.ReadUInt();
    for (uint32_t i = 0; i < n; ++i)
    {
        partNodes.push_back(std::unique_ptr<CondCompilationPartNode>(reader.ReadCondCompPartNode()));
    }
}

void CondCompilationPartNodeList::Write(Writer& writer)
{
    uint32_t n = static_cast<uint32_t>(partNodes.size());
    writer.Write(n);
    for (uint32_t i = 0; i < n; ++i)
    {
        writer.Write(partNodes[i].get());
    }
}

CondCompStatementNode::CondCompStatementNode(const Span& span_) : StatementNode(span_)
{
}

CondCompStatementNode::CondCompStatementNode(const Span& span_, CondCompExprNode* ifExpr) : StatementNode(span_), ifPart(new CondCompilationPartNode(span_, ifExpr))
{
}

Node* CondCompStatementNode::Clone() const
{
    CondCompStatementNode* clone = new CondCompStatementNode(GetSpan(), static_cast<CondCompExprNode*>(ifPart->Expr()->Clone()));
    for (const std::unique_ptr<CondCompilationPartNode>& elifPart : elifParts)
    {
        clone->elifParts.Add(static_cast<CondCompilationPartNode*>(elifPart->Clone()));
    }
    if (elsePart)
    {
        clone->elsePart.reset(static_cast<CondCompilationPartNode*>(elsePart->Clone()));
    }
    CloneLabelTo(clone);
    return clone;
}

void CondCompStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    ifPart.reset(reader.ReadCondCompPartNode());
    elifParts.Read(reader);
    bool elsePartPresent = reader.ReadBool();
    if (elsePartPresent)
    {
        elsePart.reset(reader.ReadCondCompPartNode());
    }
}

void CondCompStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(ifPart.get());
    elifParts.Write(writer);
    bool elsePartPresent = elsePart != nullptr;
    writer.Write(elsePartPresent);
    if (elsePartPresent)
    {
        writer.Write(elsePart.get());
    }
}

void CondCompStatementNode::AddIfStatement(StatementNode* ifS)
{
    ifPart->AddStatement(ifS);
}

void CondCompStatementNode::AddElifExpr(const Span& span, CondCompExprNode* elifExpr)
{
    elifParts.Add(new CondCompilationPartNode(span, elifExpr));
}

void CondCompStatementNode::AddElifStatement(StatementNode* elifS)
{
    elifParts.Back()->AddStatement(elifS);
}

void CondCompStatementNode::AddElseStatement(const Span& span, StatementNode* elseS)
{
    if (!elsePart)
    {
        elsePart.reset(new CondCompilationPartNode(span));
    }
    elsePart->AddStatement(elseS);
}

} } // namespace Cm::Ast
