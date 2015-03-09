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
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Ast {

StatementNodeList::StatementNodeList()
{
}

void StatementNodeList::SetParent(Node* parent)
{
    for (const std::unique_ptr<StatementNode>& statementNode : statementNodes)
    {
        statementNode->SetParent(parent);
    }
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

void StatementNodeList::Print(CodeFormatter& formatter)
{
    for (const std::unique_ptr<StatementNode>& statement : statementNodes)
    {
        statement->Print(formatter);
    }
}

void StatementNodeList::Accept(Visitor& visitor)
{
    for (const std::unique_ptr<StatementNode>& statement : statementNodes)
    {
        statement->Accept(visitor);
    }
}

LabelNode::LabelNode(const Span& span_) : Node(span_)
{
}

LabelNode::LabelNode(const Span& span_, const std::string& label_): Node(span_), label(label_)
{
}

Node* LabelNode::Clone(CloneContext& cloneContext) const
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
        labelNode->SetParent(this);
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

void StatementNode::Print(CodeFormatter& formatter)
{
    if (labelNode != nullptr)
    {
        formatter.DecIndent();
        formatter.WriteLine(labelNode->ToString());
        formatter.IncIndent();
    }
}

void StatementNode::SetLabelNode(LabelNode* labelNode_)
{
    labelNode.reset(labelNode_);
    if (labelNode)
    {
        labelNode->SetParent(this);
    }
}

void StatementNode::CloneLabelTo(StatementNode* clone, CloneContext& cloneContext) const
{
    if (labelNode)
    {
        clone->SetLabelNode(static_cast<LabelNode*>(labelNode->Clone(cloneContext)));
    }
}

SimpleStatementNode::SimpleStatementNode(const Span& span_) : StatementNode(span_)
{
}

SimpleStatementNode::SimpleStatementNode(const Span& span_, Node* expr_) : StatementNode(span_), expr(expr_)
{
    if (expr)
    {
        expr->SetParent(this);
    }
}

Node* SimpleStatementNode::Clone(CloneContext& cloneContext) const
{
    Node* clonedExpr = nullptr;
    if (expr)
    {
        clonedExpr = expr->Clone(cloneContext);
    }
    SimpleStatementNode* clone = new SimpleStatementNode(GetSpan(), clonedExpr);
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void SimpleStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    bool hasExpr = reader.ReadBool();
    if (hasExpr)
    {
        expr.reset(reader.ReadNode());
        expr->SetParent(this);
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

void SimpleStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    if (expr)
    {
        formatter.WriteLine(expr->ToString() + ";");
    }
    else
    {
        formatter.WriteLine(";");
    }
}

void SimpleStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (expr && visitor.VisitExpressions())
    {
        expr->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

ReturnStatementNode::ReturnStatementNode(const Span& span_) : StatementNode(span_)
{
}

ReturnStatementNode::ReturnStatementNode(const Span& span_, Node* expr_) : StatementNode(span_), expr(expr_)
{
    if (expr)
    {
        expr->SetParent(this);
    }
}

Node* ReturnStatementNode::Clone(CloneContext& cloneContext) const
{
    Node* clonedExpr = nullptr;
    if (expr)
    {
        clonedExpr = expr->Clone(cloneContext);
    }
    ReturnStatementNode* clone = new ReturnStatementNode(GetSpan(), clonedExpr);
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void ReturnStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    bool hasExpr = reader.ReadBool();
    if (hasExpr)
    {
        expr.reset(reader.ReadNode());
        expr->SetParent(this);
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

void ReturnStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.Write("return");
    if (expr)
    {
        formatter.WriteLine(" " + expr->ToString() + ";");
    }
    else
    {
        formatter.WriteLine(";");
    }
}

void ReturnStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (expr && visitor.VisitExpressions())
    {
        expr->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

ConditionalStatementNode::ConditionalStatementNode(const Span& span_) : StatementNode(span_)
{
}

ConditionalStatementNode::ConditionalStatementNode(const Span& span_, Node* condition_, StatementNode* thenS_, StatementNode* elseS_) :
    StatementNode(span_), condition(condition_), thenS(thenS_), elseS(elseS_)
{
    condition->SetParent(this);
    thenS->SetParent(this);
    if (elseS)
    {
        elseS->SetParent(this);
    }
}

Node* ConditionalStatementNode::Clone(CloneContext& cloneContext) const
{
    StatementNode* clonedElseS = nullptr;
    if (elseS)
    {
        clonedElseS = static_cast<StatementNode*>(elseS->Clone(cloneContext));
    }
    ConditionalStatementNode* clone = new ConditionalStatementNode(GetSpan(), condition->Clone(cloneContext), static_cast<StatementNode*>(thenS->Clone(cloneContext)), clonedElseS);
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void ConditionalStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    condition.reset(reader.ReadNode());
    condition->SetParent(this);
    thenS.reset(reader.ReadStatementNode());
    thenS->SetParent(this);
    bool hasElseS = reader.ReadBool();
    if (hasElseS)
    {
        elseS.reset(reader.ReadStatementNode());
        elseS->SetParent(this);
    }
}

void ConditionalStatementNode::Write(Writer& writer)
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

void ConditionalStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.Write("if (" + condition->ToString() + ")");
    if (thenS->IsCompoundStatementNode())
    {
        formatter.WriteLine();
        thenS->Print(formatter);
    }
    else
    {
        formatter.Write(" ");
        thenS->Print(formatter);
    }
    if (elseS)
    {
        formatter.Write("else");
        if (elseS->IsCompoundStatementNode())
        {
            formatter.WriteLine();
            elseS->Print(formatter);
        }
        else
        {
            formatter.Write(" ");
            elseS->Print(formatter);
        }
    }
}

void ConditionalStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (visitor.VisitExpressions())
    {
        condition->Accept(visitor);
    }
    if (!visitor.SkipContent())
    {
        thenS->Accept(visitor);
        if (elseS)
        {
            elseS->Accept(visitor);
        }
    }
    visitor.EndVisit(*this);
}

SwitchStatementNode::SwitchStatementNode(const Span& span_) : StatementNode(span_)
{
}

SwitchStatementNode::SwitchStatementNode(const Span& span_, Node* condition_) : StatementNode(span_), condition(condition_)
{
    condition->SetParent(this);
}

void SwitchStatementNode::AddCase(StatementNode* caseS)
{
    caseS->SetParent(this);
    caseStatements.Add(caseS);
}

void SwitchStatementNode::SetDefault(StatementNode* defaultS)
{
    if (defaultStatement != nullptr)
    {
        throw std::runtime_error("already has default statement");
    }
    defaultS->SetParent(this);
    defaultStatement.reset(defaultS);
}

Node* SwitchStatementNode::Clone(CloneContext& cloneContext) const
{
    SwitchStatementNode* clone = new SwitchStatementNode(GetSpan(), condition->Clone(cloneContext));
    for (const std::unique_ptr<StatementNode>& caseS : caseStatements)
    {
        clone->AddCase(static_cast<StatementNode*>(caseS->Clone(cloneContext)));
    }
    if (defaultStatement)
    {
        clone->SetDefault(static_cast<StatementNode*>(defaultStatement->Clone(cloneContext)));
    }
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void SwitchStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    condition.reset(reader.ReadNode());
    condition->SetParent(this);
    caseStatements.Read(reader);
    caseStatements.SetParent(this);
    bool hasDefaultS = reader.ReadBool();
    if (hasDefaultS)
    {
        defaultStatement.reset(reader.ReadStatementNode());
        defaultStatement->SetParent(this);
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

void SwitchStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.WriteLine("switch (" + condition->ToString() + ")");
    formatter.WriteLine("{");
    formatter.IncIndent();
    caseStatements.Print(formatter);
    if (defaultStatement)
    {
        defaultStatement->Print(formatter);
    }
    formatter.DecIndent();
    formatter.WriteLine("}");
}

void SwitchStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (!visitor.SkipContent())
    {
        if (visitor.VisitExpressions())
        {
            condition->Accept(visitor);
        }
        caseStatements.Accept(visitor);
        if (defaultStatement)
        {
            defaultStatement->Accept(visitor);
        }
    }
    visitor.EndVisit(*this);
}

CaseStatementNode::CaseStatementNode(const Span& span_) : StatementNode(span_)
{
}

void CaseStatementNode::AddExpr(Node* expr)
{
    expr->SetParent(this);
    expressions.Add(expr);
}

void CaseStatementNode::AddStatement(StatementNode* statement)
{
    statement->SetParent(this);
    statements.Add(statement);
}

Node* CaseStatementNode::Clone(CloneContext& cloneContext) const
{
    CaseStatementNode* clone = new CaseStatementNode(GetSpan());
    for (const std::unique_ptr<Node>& expr : expressions)
    {
        clone->AddExpr(expr->Clone(cloneContext));
    }
    for (const std::unique_ptr<StatementNode>& statement : statements)
    {
        clone->AddStatement(static_cast<StatementNode*>(statement->Clone(cloneContext)));
    }
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void CaseStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    expressions.Read(reader);
    expressions.SetParent(this);
    statements.Read(reader);
    statements.SetParent(this);
}

void CaseStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    expressions.Write(writer);
    statements.Write(writer);
}

void CaseStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    for (const std::unique_ptr<Node>& expr : expressions)
    {
        formatter.Write("case " + expr->ToString() + ": ");
    }
    formatter.WriteLine("{");
    formatter.IncIndent();
    statements.Print(formatter);
    formatter.DecIndent();
    formatter.WriteLine("}");
}

void CaseStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (!visitor.SkipContent())
    {
        if (visitor.VisitExpressions())
        {
            expressions.Accept(visitor);
        }
        statements.Accept(visitor);
    }
    visitor.EndVisit(*this);
}

DefaultStatementNode::DefaultStatementNode(const Span& span_) : StatementNode(span_)
{
}

void DefaultStatementNode::AddStatement(StatementNode* statement)
{
    statement->SetParent(this);
    statements.Add(statement);
}

Node* DefaultStatementNode::Clone(CloneContext& cloneContext) const
{
    DefaultStatementNode* clone = new DefaultStatementNode(GetSpan());
    for (const std::unique_ptr<StatementNode>& statement : statements)
    {
        clone->AddStatement(static_cast<StatementNode*>(statement->Clone(cloneContext)));
    }
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void DefaultStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    statements.Read(reader);
    statements.SetParent(this);
}

void DefaultStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    statements.Write(writer);
}

void DefaultStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.WriteLine("default:");
    formatter.WriteLine("{");
    formatter.IncIndent();
    statements.Print(formatter);
    formatter.DecIndent();
    formatter.WriteLine("}");
}

void DefaultStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    statements.Accept(visitor);
    visitor.EndVisit(*this);
}

GotoCaseStatementNode::GotoCaseStatementNode(const Span& span_) : StatementNode(span_)
{
}

GotoCaseStatementNode::GotoCaseStatementNode(const Span& span_, Node* targetCaseExpr_) : StatementNode(span_), targetCaseExpr(targetCaseExpr_)
{
    targetCaseExpr->SetParent(this);
}

Node* GotoCaseStatementNode::Clone(CloneContext& cloneContext) const
{
    GotoCaseStatementNode* clone = new GotoCaseStatementNode(GetSpan(), targetCaseExpr->Clone(cloneContext));
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void GotoCaseStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    targetCaseExpr.reset(reader.ReadNode());
    targetCaseExpr->SetParent(this);
}

void GotoCaseStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(targetCaseExpr.get());
}

void GotoCaseStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.WriteLine("goto case " + targetCaseExpr->ToString() + ";");
}

void GotoCaseStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (visitor.VisitExpressions())
    {
        targetCaseExpr->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

GotoDefaultStatementNode::GotoDefaultStatementNode(const Span& span_) : StatementNode(span_)
{
}

Node* GotoDefaultStatementNode::Clone(CloneContext& cloneContext) const
{
    GotoDefaultStatementNode* clone = new GotoDefaultStatementNode(GetSpan());
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void GotoDefaultStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.WriteLine("goto default;");
}

void GotoDefaultStatementNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

WhileStatementNode::WhileStatementNode(const Span& span_) : StatementNode(span_)
{
}

WhileStatementNode::WhileStatementNode(const Span& span_, Node* condition_, StatementNode* statement_) : StatementNode(span_), condition(condition_), statement(statement_)
{
    condition->SetParent(this);
    statement->SetParent(this);
}

Node* WhileStatementNode::Clone(CloneContext& cloneContext) const
{
    WhileStatementNode* clone = new WhileStatementNode(GetSpan(), condition->Clone(cloneContext), static_cast<StatementNode*>(statement->Clone(cloneContext)));
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void WhileStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    condition.reset(reader.ReadNode());
    condition->SetParent(this);
    statement.reset(reader.ReadStatementNode());
    statement->SetParent(this);
}

void WhileStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(condition.get());
    writer.Write(statement.get());
}

void WhileStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.Write("while (" + condition->ToString() + ")");
    if (statement->IsCompoundStatementNode())
    {
        formatter.WriteLine();
        statement->Print(formatter);
    }
    else
    {
        formatter.Write(" ");
        statement->Print(formatter);
    }
}

void WhileStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (visitor.VisitExpressions())
    {
        condition->Accept(visitor);
    }
    if (!visitor.SkipContent())
    {
        statement->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

DoStatementNode::DoStatementNode(const Span& span_) : StatementNode(span_)
{
}

DoStatementNode::DoStatementNode(const Span& span_, StatementNode* statement_, Node* condition_) : StatementNode(span_), statement(statement_), condition(condition_)
{
    statement->SetParent(this);
    condition->SetParent(this);
}

Node* DoStatementNode::Clone(CloneContext& cloneContext) const
{
    DoStatementNode* clone = new DoStatementNode(GetSpan(), static_cast<StatementNode*>(statement->Clone(cloneContext)), condition->Clone(cloneContext));
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void DoStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    statement.reset(reader.ReadStatementNode());
    statement->SetParent(this);
    condition.reset(reader.ReadNode());
    condition->SetParent(this);
}

void DoStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(statement.get());
    writer.Write(condition.get());
}

void DoStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.Write("do");
    if (statement->IsCompoundStatementNode())
    {
        formatter.WriteLine();
        statement->Print(formatter);
    }
    else
    {
        formatter.Write(" ");
        statement->Print(formatter);
    }
    formatter.WriteLine("while (" + condition->ToString() + ");");
}

void DoStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (!visitor.SkipContent())
    {
        statement->Accept(visitor);
    }
    if (visitor.VisitExpressions())
    {
        condition->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

ForStatementNode::ForStatementNode(const Span& span_) : StatementNode(span_)
{
}

ForStatementNode::ForStatementNode(const Span& span_, StatementNode* init_, Node* condition_, Node* increment_, StatementNode* action_) :
    StatementNode(span_), init(init_), condition(condition_), increment(increment_), action(action_)
{
    init->SetParent(this);
    if (condition)
    {
        condition->SetParent(this);
    }
    if (increment)
    {
        increment->SetParent(this);
    }
    action->SetParent(this);
}

Node* ForStatementNode::Clone(CloneContext& cloneContext) const
{
    Node* cloneOfCond = nullptr;
    if (condition)
    {
        cloneOfCond = condition->Clone(cloneContext);
    }
    Node* cloneOfInc = nullptr;
    if (increment)
    {
        cloneOfInc = increment->Clone(cloneContext);
    }
    ForStatementNode* clone = new ForStatementNode(GetSpan(), static_cast<StatementNode*>(init->Clone(cloneContext)), cloneOfCond, cloneOfInc, static_cast<StatementNode*>(action->Clone(cloneContext)));
    CloneLabelTo(clone, cloneContext);
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
        condition->SetParent(this);
    }
    bool hasInc = reader.ReadBool();
    if (hasInc)
    {
        increment.reset(reader.ReadNode());
        increment->SetParent(this);
    }
    action.reset(reader.ReadStatementNode());
    action->SetParent(this);
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

void ForStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.Write("for (" + init->ToString());
    if (condition)
    {
        formatter.Write(" " + condition->ToString());
    }
    else
    {
        formatter.Write("; ");
    }
    if (increment)
    {
        formatter.Write(increment->ToString());
    }
    formatter.Write(")");
    if (action->IsCompoundStatementNode())
    {
        formatter.WriteLine();
        action->Print(formatter);
    }
    else
    {
        formatter.Write(" ");
        action->Print(formatter);
    }
}

void ForStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (!visitor.SkipContent())
    {
        init->Accept(visitor);
    }
    if (visitor.VisitExpressions())
    {
        if (condition)
        {
            condition->Accept(visitor);
        }
        if (increment)
        {
            increment->Accept(visitor);
        }
    }
    if (!visitor.SkipContent())
    {
        action->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

RangeForStatementNode::RangeForStatementNode(const Span& span_) : StatementNode(span_)
{
}

RangeForStatementNode::RangeForStatementNode(const Span& span_, Node* varTypeExpr_, IdentifierNode* varId_, Node* container_, StatementNode* action_) :
    StatementNode(span_), varTypeExpr(varTypeExpr_), varId(varId_), container(container_), action(action_)
{
    varTypeExpr->SetParent(this);
    varId->SetParent(this);
    container->SetParent(this);
    action->SetParent(this);
}

Node* RangeForStatementNode::Clone(CloneContext& cloneContext) const
{
    RangeForStatementNode* clone = new RangeForStatementNode(GetSpan(), varTypeExpr->Clone(cloneContext), static_cast<IdentifierNode*>(varId->Clone(cloneContext)), container->Clone(cloneContext), static_cast<StatementNode*>(action->Clone(cloneContext)));
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void RangeForStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    varTypeExpr.reset(reader.ReadNode());
    varTypeExpr->SetParent(this);
    varId.reset(reader.ReadIdentifierNode());
    varId->SetParent(this);
    container.reset(reader.ReadNode());
    container->SetParent(this);
    action.reset(reader.ReadStatementNode());
    action->SetParent(this);
}

void RangeForStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(varTypeExpr.get());
    writer.Write(varId.get());
    writer.Write(container.get());
    writer.Write(action.get());
}

void RangeForStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.Write("for (" + varTypeExpr->ToString() + " " + varId->ToString() + " : " + container->ToString() + ")");
    if (action->IsCompoundStatementNode())
    {
        formatter.WriteLine();
        action->Print(formatter);
    }
    else
    {
        formatter.Write(" ");
        action->Print(formatter);
    }
}

void RangeForStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (visitor.VisitExpressions())
    {
        container->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

CompoundStatementNode::CompoundStatementNode(const Span& span_) : StatementNode(span_)
{
}

void CompoundStatementNode::AddStatement(StatementNode* statement)
{
    statement->SetParent(this);
    statements.Add(statement);
}

Node* CompoundStatementNode::Clone(CloneContext& cloneContext) const
{
    CompoundStatementNode* clone = new CompoundStatementNode(GetSpan());
    for (const std::unique_ptr<StatementNode>& statement : statements)
    {
        clone->AddStatement(static_cast<StatementNode*>(statement->Clone(cloneContext)));
    }
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void CompoundStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    statements.Read(reader);
    statements.SetParent(this);
}

void CompoundStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    statements.Write(writer);
}

void CompoundStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.WriteLine("{");
    formatter.IncIndent();
    statements.Print(formatter);
    formatter.DecIndent();
    formatter.WriteLine("}");
}

void CompoundStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    statements.Accept(visitor);
    visitor.EndVisit(*this);
}

BreakStatementNode::BreakStatementNode(const Span& span_) : StatementNode(span_)
{
}

Node* BreakStatementNode::Clone(CloneContext& cloneContext) const
{
    BreakStatementNode* clone = new BreakStatementNode(GetSpan());
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void BreakStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.WriteLine("break;");
}

void BreakStatementNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ContinueStatementNode::ContinueStatementNode(const Span& span_) : StatementNode(span_)
{
}

Node* ContinueStatementNode::Clone(CloneContext& cloneContext) const
{
    ContinueStatementNode* clone = new ContinueStatementNode(GetSpan());
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void ContinueStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.WriteLine("continue;");
}

void ContinueStatementNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

GotoStatementNode::GotoStatementNode(const Span& span_) : StatementNode(span_)
{
}

GotoStatementNode::GotoStatementNode(const Span& span_, LabelNode* target_) : StatementNode(span_), target(target_)
{
    target->SetParent(this);
}

Node* GotoStatementNode::Clone(CloneContext& cloneContext) const
{
    GotoStatementNode* clone = new GotoStatementNode(GetSpan(), static_cast<LabelNode*>(target->Clone(cloneContext)));
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void GotoStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    target.reset(reader.ReadLabelNode());
    target->SetParent(this);
}

void GotoStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(target.get());
}

void GotoStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.WriteLine("goto " + target->Label() + ";");
}

void GotoStatementNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

TypedefStatementNode::TypedefStatementNode(const Span& span_) : StatementNode(span_)
{
}

TypedefStatementNode::TypedefStatementNode(const Span& span_, Node* typeExpr_, IdentifierNode* id_) : StatementNode(span_), typeExpr(typeExpr_), id(id_)
{
    typeExpr->SetParent(this);
    id->SetParent(this);
}

Node* TypedefStatementNode::Clone(CloneContext& cloneContext) const
{
    TypedefStatementNode* clone = new TypedefStatementNode(GetSpan(), typeExpr->Clone(cloneContext), static_cast<IdentifierNode*>(id->Clone(cloneContext)));
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void TypedefStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    typeExpr.reset(reader.ReadNode());
    typeExpr->SetParent(this);
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
}

void TypedefStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(typeExpr.get());
    writer.Write(id.get());
}

void TypedefStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.WriteLine("typedef " + typeExpr->ToString() + " " + id->ToString() + ";");
}

void TypedefStatementNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

AssignmentStatementNode::AssignmentStatementNode(const Span& span_) : StatementNode(span_)
{
}

AssignmentStatementNode::AssignmentStatementNode(const Span& span_, Node* targetExpr_, Node* sourceExpr_) : StatementNode(span_), targetExpr(targetExpr_), sourceExpr(sourceExpr_)
{
    targetExpr->SetParent(this);
    sourceExpr->SetParent(this);
}

Node* AssignmentStatementNode::Clone(CloneContext& cloneContext) const
{
    AssignmentStatementNode* clone = new AssignmentStatementNode(GetSpan(), targetExpr->Clone(cloneContext), sourceExpr->Clone(cloneContext));
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void AssignmentStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    targetExpr.reset(reader.ReadNode());
    targetExpr->SetParent(this);
    sourceExpr.reset(reader.ReadNode());
    sourceExpr->SetParent(this);
}

void AssignmentStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(targetExpr.get());
    writer.Write(sourceExpr.get());
}

void AssignmentStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.WriteLine(ToString());
}

void AssignmentStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (visitor.VisitExpressions())
    {
        targetExpr->Accept(visitor);
        sourceExpr->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

ConstructionStatementNode::ConstructionStatementNode(const Span& span_) : StatementNode(span_)
{
}

ConstructionStatementNode::ConstructionStatementNode(const Span& span_, Node* typeExpr_, IdentifierNode* id_) : StatementNode(span_), typeExpr(typeExpr_), id(id_)
{
    typeExpr->SetParent(this);
    id->SetParent(this);
}

void ConstructionStatementNode::AddArgument(Node* argument)
{
    argument->SetParent(this);
    arguments.Add(argument);
}

Node* ConstructionStatementNode::Clone(CloneContext& cloneContext) const
{
    ConstructionStatementNode* clone = new ConstructionStatementNode(GetSpan(), typeExpr->Clone(cloneContext), static_cast<IdentifierNode*>(id->Clone(cloneContext)));
    for (const std::unique_ptr<Node>& argument : arguments)
    {
        clone->AddArgument(argument->Clone(cloneContext));
    }
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void ConstructionStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    typeExpr.reset(reader.ReadNode());
    typeExpr->SetParent(this);
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    arguments.Read(reader);
    arguments.SetParent(this);
}

void ConstructionStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(typeExpr.get());
    writer.Write(id.get());
    arguments.Write(writer);
}

void ConstructionStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.WriteLine(ToString());
}

void ConstructionStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (visitor.VisitExpressions())
    {
        arguments.Accept(visitor);
    }
    visitor.EndVisit(*this);
}

std::string ConstructionStatementNode::ToString() const 
{ 
    return typeExpr->ToString() + " " + id->ToString() + "(" + arguments.ToString() + ");"; 
}

DeleteStatementNode::DeleteStatementNode(const Span& span_) : StatementNode(span_)
{
}

DeleteStatementNode::DeleteStatementNode(const Span& span_, Node* pointerExpr_) : StatementNode(span_), pointerExpr(pointerExpr_)
{
    pointerExpr->SetParent(this);
}

Node* DeleteStatementNode::Clone(CloneContext& cloneContext) const
{
    DeleteStatementNode* clone = new DeleteStatementNode(GetSpan(), pointerExpr->Clone(cloneContext));
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void DeleteStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    pointerExpr.reset(reader.ReadNode());
    pointerExpr->SetParent(this);
}

void DeleteStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(pointerExpr.get());
}

void DeleteStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.WriteLine("delete " + pointerExpr->ToString() + ";");
}

void DeleteStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (visitor.VisitExpressions())
    {
        pointerExpr->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

DestroyStatementNode::DestroyStatementNode(const Span& span_) : StatementNode(span_)
{
}

DestroyStatementNode::DestroyStatementNode(const Span& span_, Node* pointerExpr_) : StatementNode(span_), pointerExpr(pointerExpr_)
{
    pointerExpr->SetParent(this);
}

Node* DestroyStatementNode::Clone(CloneContext& cloneContext) const
{
    DestroyStatementNode* clone = new DestroyStatementNode(GetSpan(), pointerExpr->Clone(cloneContext));
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void DestroyStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    pointerExpr.reset(reader.ReadNode());
    pointerExpr->SetParent(this);
}

void DestroyStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(pointerExpr.get());
}

void DestroyStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.WriteLine("destroy " + pointerExpr->ToString() + ";");
}

void DestroyStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (visitor.VisitExpressions())
    {
        pointerExpr->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

ThrowStatementNode::ThrowStatementNode(const Span& span_) : StatementNode(span_)
{
}

ThrowStatementNode::ThrowStatementNode(const Span& span_, Node* exceptionExpr_) : StatementNode(span_), exceptionExpr(exceptionExpr_)
{
    exceptionExpr->SetParent(this);
}

Node* ThrowStatementNode::Clone(CloneContext& cloneContext) const
{
    ThrowStatementNode* clone = new ThrowStatementNode(GetSpan(), exceptionExpr->Clone(cloneContext));
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void ThrowStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    exceptionExpr.reset(reader.ReadNode());
    exceptionExpr->SetParent(this);
}

void ThrowStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(exceptionExpr.get());
}

void ThrowStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.WriteLine("throw " + exceptionExpr->ToString() + ";");
}

void ThrowStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (visitor.VisitExpressions())
    {
        exceptionExpr->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

TryStatementNode::TryStatementNode(const Span& span_) : StatementNode(span_)
{
}

TryStatementNode::TryStatementNode(const Span& span_, CompoundStatementNode* tryBlock_) : StatementNode(span_), tryBlock(tryBlock_)
{
    tryBlock->SetParent(this);
}

void TryStatementNode::AddHandler(CatchNode* handler)
{
    handler->SetParent(this);
    handlers.Add(handler);
}

bool TryStatementNode::IsLastHandler(Cm::Ast::CatchNode* handler)
{
    int n = handlers.Count();
    if (n > 0)
    {
        if (handlers[n - 1] == handler) return true;
    }
    return false;
}

CatchNode* TryStatementNode::GetFirstHandler() const
{
    if (handlers.Count() == 0)
    {
        throw std::runtime_error("no handlers");
    }
    return static_cast<CatchNode*>(handlers[0]);
}

CatchNode* TryStatementNode::GetNextHandler(CatchNode* handler)
{
    int n = handlers.Count();
    for (int i = 0; i < n; ++i)
    {
        if (handlers[i] == handler)
        {
            if (i < n - 1)
            {
                return static_cast<CatchNode*>(handlers[i + 1]);
            }
            else
            {
                throw std::runtime_error("no next handler");
            }
        }
    }
    throw std::runtime_error("handler not found");
}

Node* TryStatementNode::Clone(CloneContext& cloneContext) const
{
    TryStatementNode* clone = new TryStatementNode(GetSpan(), static_cast<CompoundStatementNode*>(tryBlock->Clone(cloneContext)));
    for (const std::unique_ptr<Node>& handler : handlers)
    {
        clone->AddHandler(static_cast<CatchNode*>(handler->Clone(cloneContext)));
    }
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void TryStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    tryBlock.reset(reader.ReadCompoundStatementNode());
    tryBlock->SetParent(this);
    handlers.Read(reader);
    handlers.SetParent(this);
}

void TryStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(tryBlock.get());
    handlers.Write(writer);
}

void TryStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    tryBlock->Print(formatter);
    for (const std::unique_ptr<Node>& handler : handlers)
    {
        handler->Print(formatter);
    }
}

void TryStatementNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void TryStatementNode::SetFirstCatchId(int catchId)
{
    if (handlers.Count() == 0)
    {
        throw std::runtime_error("no handlers");
    }
    CatchNode* catchNode = static_cast<CatchNode*>(handlers[0]);
    catchNode->SetCatchId(catchId);
}

CatchNode::CatchNode(const Span& span_) : Node(span_), catchId(-1)
{
}

CatchNode::CatchNode(const Span& span_, Node* exceptionTypeExpr_, IdentifierNode* exceptionId_, CompoundStatementNode* catchBlock_) :
    Node(span_), exceptionTypeExpr(exceptionTypeExpr_), exceptionId(exceptionId_), catchBlock(catchBlock_), catchId(-1)
{
    exceptionTypeExpr->SetParent(this);
    if (exceptionId)
    {
        exceptionId->SetParent(this);
    }
    catchBlock->SetParent(this);
}

Node* CatchNode::Clone(CloneContext& cloneContext) const
{
    IdentifierNode* clonedId = nullptr;
    if (exceptionId)
    {
        clonedId = static_cast<IdentifierNode*>(exceptionId->Clone(cloneContext));
    }
    return new CatchNode(GetSpan(), exceptionTypeExpr->Clone(cloneContext), clonedId, static_cast<CompoundStatementNode*>(catchBlock->Clone(cloneContext)));
}

void CatchNode::Read(Reader& reader)
{
    exceptionTypeExpr.reset(reader.ReadNode());
    exceptionTypeExpr->SetParent(this);
    bool hasId = reader.ReadBool();
    if (hasId)
    {
        exceptionId.reset(reader.ReadIdentifierNode());
        exceptionId->SetParent(this);
    }
    catchBlock.reset(reader.ReadCompoundStatementNode());
    catchBlock->SetParent(this);
}

void CatchNode::Write(Writer& writer) 
{
    writer.Write(exceptionTypeExpr.get());
    bool hasId = exceptionId != nullptr;
    writer.Write(hasId);
    if (hasId)
    {
        writer.Write(exceptionId.get());
    }
    writer.Write(catchBlock.get());
}

void CatchNode::Print(CodeFormatter& formatter)
{
    formatter.WriteLine("catch (" + exceptionTypeExpr->ToString() + " " + exceptionId->ToString() + ")");
    catchBlock->Print(formatter);
}

void CatchNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

AssertStatementNode::AssertStatementNode(const Span& span_) : StatementNode(span_)
{
}

AssertStatementNode::AssertStatementNode(const Span& span_, Node* assertExpr_) : StatementNode(span_), assertExpr(assertExpr_)
{
    assertExpr->SetParent(this);
}

Node* AssertStatementNode::Clone(CloneContext& cloneContext) const
{
    AssertStatementNode* clone = new AssertStatementNode(GetSpan(), assertExpr->Clone(cloneContext));
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void AssertStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    assertExpr.reset(reader.ReadNode());
    assertExpr->SetParent(this);
}

void AssertStatementNode::Write(Writer& writer)
{
    StatementNode::Write(writer);
    writer.Write(assertExpr.get());
}

void AssertStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    formatter.WriteLine("#assert(" + assertExpr->ToString() + ");");
}

void AssertStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (visitor.VisitExpressions())
    {
        assertExpr->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

CondCompSymbolNode::CondCompSymbolNode(const Span& span_) : Node(span_)
{
}

CondCompSymbolNode::CondCompSymbolNode(const Span& span_, const std::string& symbol_) : Node(span_), symbol(symbol_)
{
}

Node* CondCompSymbolNode::Clone(CloneContext& cloneContext) const
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
    left->SetParent(this);
    right->SetParent(this);
}

void CondCompBinExprNode::Read(Reader& reader)
{
    left.reset(reader.ReadCondCompExprNode());
    left->SetParent(this);
    right.reset(reader.ReadCondCompExprNode());
    right->SetParent(this);
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

Node* CondCompDisjunctionNode::Clone(CloneContext& cloneContext) const
{
    return new CondCompDisjunctionNode(GetSpan(), static_cast<CondCompExprNode*>(Left()->Clone(cloneContext)), static_cast<CondCompExprNode*>(Right()->Clone(cloneContext)));
}

std::string CondCompDisjunctionNode::ToString() const 
{
    return Left()->ToString() + " || " + Right()->ToString();
}

void CondCompDisjunctionNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    Left()->Accept(visitor);
    Right()->Accept(visitor);
    visitor.EndVisit(*this);
}

CondCompConjunctionNode::CondCompConjunctionNode(const Span& span_) : CondCompBinExprNode(span_)
{
}

CondCompConjunctionNode::CondCompConjunctionNode(const Span& span_, CondCompExprNode* left_, CondCompExprNode* right_) : CondCompBinExprNode(span_, left_, right_)
{
}

Node* CondCompConjunctionNode::Clone(CloneContext& cloneContext) const
{
    return new CondCompConjunctionNode(GetSpan(), static_cast<CondCompExprNode*>(Left()->Clone(cloneContext)), static_cast<CondCompExprNode*>(Right()->Clone(cloneContext)));
}

std::string CondCompConjunctionNode::ToString() const
{
    std::string left;
    if (Left()->IsCondCompDisjunctionNode())
    {
        left = "(" + Left()->ToString() + ")";
    }
    else
    {
        left = Left()->ToString();
    }
    std::string right;
    if (Right()->IsCondCompDisjunctionNode())
    {
        right = "(" + Right()->ToString() + ")";
    }
    else
    {
        right = Right()->ToString();
    }
    return left + " && " + right;
}

void CondCompConjunctionNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    Left()->Accept(visitor);
    Right()->Accept(visitor);
    visitor.EndVisit(*this);
}

CondCompNotNode::CondCompNotNode(const Span& span_) : CondCompExprNode(span_)
{
}

CondCompNotNode::CondCompNotNode(const Span& span_, CondCompExprNode* subject_) : CondCompExprNode(span_), subject(subject_)
{
    subject->SetParent(this);
}

Node* CondCompNotNode::Clone(CloneContext& cloneContext) const
{
    return new CondCompNotNode(GetSpan(), static_cast<CondCompExprNode*>(subject->Clone(cloneContext)));
}

void CondCompNotNode::Read(Reader& reader)
{
    subject.reset(reader.ReadCondCompExprNode());
    subject->SetParent(this);
}

void CondCompNotNode::Write(Writer& writer)
{
    writer.Write(subject.get());
}

std::string CondCompNotNode::ToString() const
{
    std::string s;
    if (subject->IsCondCompBinExprNode())
    {
        s = "(" + subject->ToString() + ")";
    }
    else
    {
        s = subject->ToString();
    }
    return "!" + s;
}

void CondCompNotNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    subject->Accept(visitor);
    visitor.EndVisit(*this);
}

CondCompPrimaryNode::CondCompPrimaryNode(const Span& span_) : CondCompExprNode(span_)
{
}

CondCompPrimaryNode::CondCompPrimaryNode(const Span& span_, CondCompSymbolNode* symbolNode_) : CondCompExprNode(span_), symbolNode(symbolNode_)
{
    symbolNode->SetParent(this);
}

Node* CondCompPrimaryNode::Clone(CloneContext& cloneContext) const
{
    return new CondCompPrimaryNode(GetSpan(), static_cast<CondCompSymbolNode*>(symbolNode->Clone(cloneContext)));
}

void CondCompPrimaryNode::Read(Reader& reader)
{
    symbolNode.reset(reader.ReadCondCompSymbolNode());
    symbolNode->SetParent(this);
}

void CondCompPrimaryNode::Write(Writer& writer)
{
    writer.Write(symbolNode.get());
}

std::string CondCompPrimaryNode::ToString() const
{
    return symbolNode->ToString();
}

void CondCompPrimaryNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

CondCompilationPartNode::CondCompilationPartNode(const Span& span_) : Node(span_)
{
}

CondCompilationPartNode::CondCompilationPartNode(const Span& span_, CondCompExprNode* expr_) : Node(span_), expr(expr_)
{
    if (expr)
    {
        expr->SetParent(this);
    }
}

void CondCompilationPartNode::AddStatement(StatementNode* statement)
{
    statement->SetParent(this);
    statements.Add(statement);
}

Node* CondCompilationPartNode::Clone(CloneContext& cloneContext) const
{
    CondCompilationPartNode* clone = new CondCompilationPartNode(GetSpan(), static_cast<CondCompExprNode*>(expr->Clone(cloneContext)));
    for (const std::unique_ptr<StatementNode>& statement : statements)
    {
        clone->AddStatement(static_cast<StatementNode*>(statement->Clone(cloneContext)));
    }
    return clone;
}

void CondCompilationPartNode::Read(Reader& reader)
{
    bool exprPresent = reader.ReadBool();
    if (exprPresent)
    {
        expr.reset(reader.ReadCondCompExprNode());
        expr->SetParent(this);
    }
    statements.Read(reader);
    statements.SetParent(this);
}

void CondCompilationPartNode::Write(Writer& writer)
{
    bool exprPesent = expr != nullptr;
    writer.Write(exprPesent);
    if (exprPesent)
    {
        writer.Write(expr.get());
    }
    statements.Write(writer);
}

void CondCompilationPartNode::Print(CodeFormatter& formatter)
{
    formatter.WriteLine("(" + expr->ToString() + ")");
    formatter.IncIndent();
    statements.Print(formatter);
    formatter.DecIndent();
}

void CondCompilationPartNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    expr->Accept(visitor);
    statements.Accept(visitor);
    visitor.EndVisit(*this);
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

void CondCompilationPartNodeList::Accept(Visitor& visitor)
{
    for (const std::unique_ptr<CondCompilationPartNode>& part : partNodes)
    {
        part->Accept(visitor);
    }
}

void CondCompilationPartNodeList::SetParent(Node* parent)
{
    for (const std::unique_ptr<CondCompilationPartNode>& part : partNodes)
    {
        part->SetParent(parent);
    }
}

CondCompStatementNode::CondCompStatementNode(const Span& span_) : StatementNode(span_)
{
}

CondCompStatementNode::CondCompStatementNode(const Span& span_, CondCompExprNode* ifExpr) : StatementNode(span_), ifPart(new CondCompilationPartNode(span_, ifExpr))
{
    ifPart->SetParent(this);
}

Node* CondCompStatementNode::Clone(CloneContext& cloneContext) const
{
    CondCompStatementNode* clone = new CondCompStatementNode(GetSpan(), static_cast<CondCompExprNode*>(ifPart->Expr()->Clone(cloneContext)));
    for (const std::unique_ptr<CondCompilationPartNode>& elifPart : elifParts)
    {
        CondCompilationPartNode* clonedElifPart = static_cast<CondCompilationPartNode*>(elifPart->Clone(cloneContext));
        clonedElifPart->SetParent(clone);
        clone->elifParts.Add(clonedElifPart);
    }
    if (elsePart)
    {
        CondCompilationPartNode* clonedElsePart = static_cast<CondCompilationPartNode*>(elsePart->Clone(cloneContext));
        clonedElsePart->SetParent(clone);
        clone->elsePart.reset(clonedElsePart);
    }
    CloneLabelTo(clone, cloneContext);
    return clone;
}

void CondCompStatementNode::Read(Reader& reader)
{
    StatementNode::Read(reader);
    ifPart.reset(reader.ReadCondCompPartNode());
    ifPart->SetParent(this);
    elifParts.Read(reader);
    elifParts.SetParent(this);
    bool elsePartPresent = reader.ReadBool();
    if (elsePartPresent)
    {
        elsePart.reset(reader.ReadCondCompPartNode());
        elsePart->SetParent(this);
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

void CondCompStatementNode::Print(CodeFormatter& formatter)
{
    formatter.DecIndent();
    formatter.Write("#if ");
    ifPart->Print(formatter);
    for (const std::unique_ptr<CondCompilationPartNode>& elifPart : elifParts)
    {
        formatter.Write("#elif ");
        elifPart->Print(formatter);
    }
    if (elsePart)
    {
        formatter.Write("#else");
        elsePart->Print(formatter);
    }
    formatter.WriteLine("#endif");
    formatter.IncIndent();
}

void CondCompStatementNode::AddIfStatement(StatementNode* ifS)
{
    ifPart->AddStatement(ifS);
}

void CondCompStatementNode::AddElifExpr(const Span& span, CondCompExprNode* elifExpr)
{
    CondCompilationPartNode* elifPart = new CondCompilationPartNode(span, elifExpr);
    elifPart->SetParent(this);
    elifParts.Add(elifPart);
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
        elsePart->SetParent(this);
    }
    elsePart->AddStatement(elseS);
}

void CondCompStatementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    ifPart->Accept(visitor);
    elifParts.Accept(visitor);
    if (elsePart)
    {
        elsePart->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

} } // namespace Cm::Ast
