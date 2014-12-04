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

StatementNode::StatementNode(const Span& span_) : Node(span_), parent(nullptr)
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
}

void StatementNode::CloneLabelTo(StatementNode* clone) const
{
    if (labelNode)
    {
        clone->SetLabelNode(static_cast<LabelNode*>(labelNode->Clone()));
    }
}

Node* StatementNode::Parent() const
{
    return parent;
}

void StatementNode::SetParent(Node* parent_)
{
    parent = parent_;
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

void SimpleStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    if (expr)
    {
        formatter.WriteLine(expr->ToString());
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

void ReturnStatementNode::Print(CodeFormatter& formatter)
{
    StatementNode::Print(formatter);
    if (expr)
    {
        formatter.WriteLine(expr->ToString());
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
    thenS->SetParent(this);
    if (elseS)
    {
        elseS->SetParent(this);
    }
}

Node* ConditionalStatementNode::Clone() const
{
    StatementNode* clonedElseS = nullptr;
    if (elseS)
    {
        clonedElseS = static_cast<StatementNode*>(elseS->Clone());
    }
    ConditionalStatementNode* clone = new ConditionalStatementNode(GetSpan(), condition->Clone(), static_cast<StatementNode*>(thenS->Clone()), clonedElseS);
    CloneLabelTo(clone);
    return clone;
}

void ConditionalStatementNode::Read(Reader& reader)
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
    thenS->Accept(visitor);
    if (elseS)
    {
        elseS->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

SwitchStatementNode::SwitchStatementNode(const Span& span_) : StatementNode(span_)
{
}

SwitchStatementNode::SwitchStatementNode(const Span& span_, Node* condition_) : StatementNode(span_), condition(condition_)
{
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
    if (visitor.VisitExpressions())
    {
        condition->Accept(visitor);
    }
    caseStatements.Accept(visitor);
    if (defaultStatement)
    {
        defaultStatement->Accept(visitor);
    }
    visitor.EndVisit(*this);
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
    statement->SetParent(this);
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
    if (visitor.VisitExpressions())
    {
        expressions.Accept(visitor);
    }
    statements.Accept(visitor);
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

Node* GotoDefaultStatementNode::Clone() const
{
    GotoDefaultStatementNode* clone = new GotoDefaultStatementNode(GetSpan());
    CloneLabelTo(clone);
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
    statement->SetParent(this);
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
    statement->Accept(visitor);
    visitor.EndVisit(*this);
}

DoStatementNode::DoStatementNode(const Span& span_) : StatementNode(span_)
{
}

DoStatementNode::DoStatementNode(const Span& span_, StatementNode* statement_, Node* condition_) : StatementNode(span_), statement(statement_), condition(condition_)
{
    statement->SetParent(this);
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
    statement->Accept(visitor);
    if (visitor.VisitExpressions())
    {
        condition->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

RangeForStatementNode::RangeForStatementNode(const Span& span_) : StatementNode(span_)
{
}

RangeForStatementNode::RangeForStatementNode(const Span& span_, Node* varTypeExpr_, IdentifierNode* varId_, Node* container_, StatementNode* action_) :
    StatementNode(span_), varTypeExpr(varTypeExpr_), varId(varId_), container(container_), action(action_)
{
    action->SetParent(this);
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
    action->Accept(visitor);
    visitor.EndVisit(*this);
}

ForStatementNode::ForStatementNode(const Span& span_) : StatementNode(span_)
{
}

ForStatementNode::ForStatementNode(const Span& span_, StatementNode* init_, Node* condition_, Node* increment_, StatementNode* action_) :
    StatementNode(span_), init(init_), condition(condition_), increment(increment_), action(action_)
{
    init->SetParent(this);
    action->SetParent(this);
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
    init->Accept(visitor);
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
    action->Accept(visitor);
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

Node* BreakStatementNode::Clone() const
{
    BreakStatementNode* clone = new BreakStatementNode(GetSpan());
    CloneLabelTo(clone);
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

Node* ContinueStatementNode::Clone() const
{
    ContinueStatementNode* clone = new ContinueStatementNode(GetSpan());
    CloneLabelTo(clone);
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
    visitor.BeginVisit(*this);
    tryBlock->Accept(visitor);
    handlers.Accept(visitor);
    visitor.EndVisit(*this);
}

CatchNode::CatchNode(const Span& span_) : Node(span_), parent(nullptr)
{
}

CatchNode::CatchNode(const Span& span_, Node* exceptionTypeExpr_, IdentifierNode* exceptionId_, CompoundStatementNode* catchBlock_) :
    Node(span_), exceptionTypeExpr(exceptionTypeExpr_), exceptionId(exceptionId_), catchBlock(catchBlock_), parent(nullptr)
{
    catchBlock->SetParent(this);
}

Node* CatchNode::Clone() const
{
    IdentifierNode* clonedId = nullptr;
    if (exceptionId)
    {
        clonedId = static_cast<IdentifierNode*>(exceptionId->Clone());
    }
    return new CatchNode(GetSpan(), exceptionTypeExpr->Clone(), clonedId, static_cast<CompoundStatementNode*>(catchBlock->Clone()));
}

void CatchNode::Read(Reader& reader)
{
    exceptionTypeExpr.reset(reader.ReadNode());
    bool hasId = reader.ReadBool();
    if (hasId)
    {
        exceptionId.reset(reader.ReadIdentifierNode());
    }
    catchBlock.reset(reader.ReadCompoundStatementNode());
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

Node* CatchNode::Parent() const
{
    return parent;
}

void CatchNode::SetParent(Node* parent_)
{
    parent = parent_;
}

void CatchNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    catchBlock->Accept(visitor);
    visitor.EndVisit(*this);
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

Node* CondCompConjunctionNode::Clone() const
{
    return new CondCompConjunctionNode(GetSpan(), static_cast<CondCompExprNode*>(Left()->Clone()), static_cast<CondCompExprNode*>(Right()->Clone()));
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

std::string CondCompPrimaryNode::ToString() const
{
    return symbolNode->ToString();
}

void CondCompPrimaryNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

CondCompilationPartNode::CondCompilationPartNode(const Span& span_) : Node(span_), parent(nullptr)
{
}

CondCompilationPartNode::CondCompilationPartNode(const Span& span_, CondCompExprNode* expr_) : Node(span_), expr(expr_), parent(nullptr)
{
}

void CondCompilationPartNode::AddStatement(StatementNode* statement)
{
    statement->SetParent(this);
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
    bool exprPresent = reader.ReadBool();
    if (exprPresent)
    {
        expr.reset(reader.ReadCondCompExprNode());
    }
    statements.Read(reader);
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

Node* CondCompilationPartNode::Parent() const
{
    return parent;
}

void CondCompilationPartNode::SetParent(Node* parent_)
{
    parent = parent_;
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

CondCompStatementNode::CondCompStatementNode(const Span& span_) : StatementNode(span_)
{
}

CondCompStatementNode::CondCompStatementNode(const Span& span_, CondCompExprNode* ifExpr) : StatementNode(span_), ifPart(new CondCompilationPartNode(span_, ifExpr))
{
    ifPart->SetParent(this);
}

Node* CondCompStatementNode::Clone() const
{
    CondCompStatementNode* clone = new CondCompStatementNode(GetSpan(), static_cast<CondCompExprNode*>(ifPart->Expr()->Clone()));
    for (const std::unique_ptr<CondCompilationPartNode>& elifPart : elifParts)
    {
        CondCompilationPartNode* clonedElifPart = static_cast<CondCompilationPartNode*>(elifPart->Clone());
        clonedElifPart->SetParent(clone);
        clone->elifParts.Add(clonedElifPart);
    }
    if (elsePart)
    {
        CondCompilationPartNode* clonedElsePart = static_cast<CondCompilationPartNode*>(elsePart->Clone());
        clonedElsePart->SetParent(clone);
        clone->elsePart.reset(clonedElsePart);
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
