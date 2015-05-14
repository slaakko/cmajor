/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/BoundStatement.hpp>
#include <Cm.BoundTree/Visitor.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <stdexcept>

namespace Cm { namespace BoundTree {

BoundStatement::BoundStatement(Cm::Ast::Node* syntaxNode_) : BoundNode(syntaxNode_), parent(nullptr), cfgNode(nullptr)
{
    if (syntaxNode_)
    {
        if (syntaxNode_->IsStatementNode())
        {
            Cm::Ast::StatementNode* statementNode = static_cast<Cm::Ast::StatementNode*>(syntaxNode_);
            Cm::Ast::LabelNode* labelNode = statementNode->Label();
            if (labelNode)
            {
                label = labelNode->Label();
            }
        }
    }
}

BoundCompoundStatement* BoundStatement::CompoundParent() const
{
    BoundStatement* p = parent;
    while (p && !p->IsBoundCompoundStatement())
    {
        p = p->parent;
    }
    if (!p)
    {
        return nullptr;
    }
    return static_cast<BoundCompoundStatement*>(p);
}

void BoundStatement::AddToPrevSet(Cm::Core::CfgNode* node)
{
    prevSet.insert(node);
}

void BoundStatement::SetCfgNode(Cm::Core::CfgNode* cfgNode_)
{ 
    cfgNode = cfgNode_; 
    PatchPrevSet();
}

void BoundStatement::PatchPrevSet()
{
    if (!cfgNode)
    {
        throw std::runtime_error("cfg node not set");
    }
    int thisNodeId = cfgNode->Id();
    for (Cm::Core::CfgNode* prev : prevSet)
    {
        prev->AddNext(thisNodeId);
    }
    prevSet.clear();
}

BoundStatementList::BoundStatementList()
{
}

void BoundStatementList::AddStatement(BoundStatement* statement)
{
    statements.push_back(std::unique_ptr<BoundStatement>(statement));
}

void BoundStatementList::InsertStatement(int index, BoundStatement* statement)
{
    statements.insert(statements.begin() + index, std::unique_ptr<BoundStatement>(statement));
}

void BoundStatementList::Accept(Visitor& visitor)
{
    for (const std::unique_ptr<BoundStatement>& statement : statements)
    {
        visitor.VisitStatement(*statement);
    }
}

BoundParentStatement::BoundParentStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

BoundCompoundStatement::BoundCompoundStatement(Cm::Ast::Node* syntaxNode_) : BoundParentStatement(syntaxNode_)
{
}

void BoundCompoundStatement::AddStatement(BoundStatement* statement)
{
    statement->SetParent(this);
    statementList.AddStatement(statement);
}

void BoundCompoundStatement::InsertStatement(int index, BoundStatement* statement)
{
    statement->SetParent(this);
    statementList.InsertStatement(index, statement);
}

void BoundCompoundStatement::Accept(Visitor& visitor) 
{
    visitor.BeginVisit(*this);
    statementList.Accept(visitor);
    visitor.EndVisit(*this);
}

BoundReceiveStatement::BoundReceiveStatement(Cm::Sym::ParameterSymbol* parameterSymbol_) : BoundStatement(nullptr), parameterSymbol(parameterSymbol_)
{
}

void BoundReceiveStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundInitClassObjectStatement::BoundInitClassObjectStatement(BoundFunctionCall* functionCall_) : BoundStatement(nullptr), functionCall(functionCall_)
{
}

void BoundInitClassObjectStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundInitVPtrStatement::BoundInitVPtrStatement(Cm::Sym::ClassTypeSymbol* classType_) : BoundStatement(nullptr), classType(classType_)
{
}

void BoundInitVPtrStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundInitMemberVariableStatement::BoundInitMemberVariableStatement(Cm::Sym::FunctionSymbol* ctor_, BoundExpressionList&& arguments_) : 
    BoundStatement(nullptr), ctor(ctor_), arguments(std::move(arguments_)), registerDestructor(false), memberVarSymbol(nullptr)
{
}

void BoundInitMemberVariableStatement::Accept(Visitor& visitor) 
{
    visitor.Visit(*this);
}

void BoundInitMemberVariableStatement::SetMemberVariableSymbol(Cm::Sym::MemberVariableSymbol* memberVarSymbol_)
{
    memberVarSymbol = memberVarSymbol_;
}

BoundFunctionCallStatement::BoundFunctionCallStatement(Cm::Sym::FunctionSymbol* function_, BoundExpressionList&& arguments_) : BoundStatement(nullptr), function(function_), arguments(std::move(arguments_))
{
}

void BoundFunctionCallStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundFunctionCallStatement::SetTraceCallInfo(TraceCallInfo* traceCallInfo_)
{
    traceCallInfo.reset(traceCallInfo_);
}

BoundReturnStatement::BoundReturnStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_), ctor(nullptr), returnType(nullptr)
{
}

void BoundReturnStatement::SetExpression(BoundExpression* expression_)
{
    expression.reset(expression_);
}

void BoundReturnStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundReturnStatement::SetTraceCallInfo(TraceCallInfo* traceCallInfo_)
{
    traceCallInfo.reset(traceCallInfo_);
}

BoundBeginTryStatement::BoundBeginTryStatement(Cm::Ast::Node* syntaxNode_, int firstCatchId_) : BoundStatement(syntaxNode_), firstCatchId(firstCatchId_)
{
}

void BoundBeginTryStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundEndTryStatement::BoundEndTryStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundEndTryStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundExitBlocksStatement::BoundExitBlocksStatement(Cm::Ast::Node* syntaxNode_, BoundCompoundStatement* targetBlock_) : BoundStatement(syntaxNode_), targetBlock(targetBlock_)
{
}

void BoundExitBlocksStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundBeginThrowStatement::BoundBeginThrowStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundBeginThrowStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundEndThrowStatement::BoundEndThrowStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundEndThrowStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundBeginCatchStatement::BoundBeginCatchStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundBeginCatchStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundEndCatchStatement::BoundEndCatchStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundEndCatchStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundConstructionStatement::BoundConstructionStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_), localVariable(nullptr), ctor(nullptr)
{
}

void BoundConstructionStatement::SetArguments(BoundExpressionList&& arguments_) 
{ 
    arguments = std::move(arguments_); 
}

void BoundConstructionStatement::InsertLocalVariableToArguments()
{
    Cm::BoundTree::BoundLocalVariable* boundLocalVariable = new Cm::BoundTree::BoundLocalVariable(SyntaxNode(), localVariable);
    boundLocalVariable->SetType(localVariable->GetType());
    arguments.InsertFront(boundLocalVariable);
}

void BoundConstructionStatement::GetResolutionArguments(Cm::Sym::TypeSymbol* localVariableType, std::vector<Cm::Core::Argument>& resolutionArguments)
{
    if (localVariableType->IsPointerType() && arguments.Count() == 1 && arguments[0]->IsBoundNullLiteral())
    {
        arguments[0]->SetType(localVariableType);
    }
    for (const std::unique_ptr<BoundExpression>& argument : arguments)
    {
        Cm::Core::Argument arg(argument->GetArgumentCategory(), argument->GetType());
        if (argument->GetFlag(Cm::BoundTree::BoundNodeFlags::argIsTemporary))
        {
            arg.SetBindToRvalueRef();
        }
        resolutionArguments.push_back(arg);
    }
}

void BoundConstructionStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundConstructionStatement::SetTraceCallInfo(TraceCallInfo* traceCallInfo_)
{
    traceCallInfo.reset(traceCallInfo_);
}

BoundDestructionStatement::BoundDestructionStatement(Cm::Ast::Node* syntaxNode_, Ir::Intf::Object* object_, Cm::Sym::FunctionSymbol* destructor_) :
    BoundStatement(syntaxNode_), object(object_), destructor(destructor_)
{
}

void BoundDestructionStatement::Accept(Visitor& visitor) 
{
    visitor.Visit(*this);
}

BoundAssignmentStatement::BoundAssignmentStatement(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_, Cm::Sym::FunctionSymbol* assignment_) : 
    BoundStatement(syntaxNode_), left(left_), right(right_), assignment(assignment_)
{
}

void BoundAssignmentStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundAssignmentStatement::SetTraceCallInfo(TraceCallInfo* traceCallInfo_)
{
    traceCallInfo.reset(traceCallInfo_);
}

BoundSimpleStatement::BoundSimpleStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundSimpleStatement::SetExpression(BoundExpression* expression_)
{
    expression.reset(expression_);
}

void BoundSimpleStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundSwitchStatement::BoundSwitchStatement(Cm::Ast::Node* syntaxNode_) : BoundParentStatement(syntaxNode_)
{
}

void BoundSwitchStatement::SetCondition(BoundExpression* condition_)
{
    condition.reset(condition_);
}

void BoundSwitchStatement::AddStatement(BoundStatement* statement_)
{
    statement_->SetParent(this);
    if (statement_->IsBoundCaseStatement())
    {
        caseStatements.AddStatement(statement_);
    }
    else
    {
        defaultStatement.reset(statement_);
    }
}

void BoundSwitchStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundSwitchStatement::AddBreakTargetLabel(Ir::Intf::LabelObject* breakTargetLabel)
{
    breakTargetLabels.push_back(breakTargetLabel);
}

void BoundSwitchStatement::AddToBreakNextSet(Cm::Core::CfgNode* node)
{
    breakNextSet.insert(node);
}

BoundCaseStatement::BoundCaseStatement(Cm::Ast::Node* syntaxNode_) : BoundParentStatement(syntaxNode_)
{
}

void BoundCaseStatement::AddStatement(BoundStatement* statement_)
{
    statement_->SetParent(this);
    statements.AddStatement(statement_);
}

void BoundCaseStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundCaseStatement::AddValue(Cm::Sym::Value* value)
{
    values.push_back(std::unique_ptr<Cm::Sym::Value>(value));
}

BoundDefaultStatement::BoundDefaultStatement(Cm::Ast::Node* syntaxNode_) : BoundParentStatement(syntaxNode_)
{
}

void BoundDefaultStatement::AddStatement(BoundStatement* statement_)
{
    statement_->SetParent(this);
    statements.AddStatement(statement_);
}

void BoundDefaultStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundBreakStatement::BoundBreakStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundBreakStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundContinueStatement::BoundContinueStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundContinueStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundGotoStatement::BoundGotoStatement(Cm::Ast::Node* syntaxNode_, const std::string& targetLabel_) : 
    BoundStatement(syntaxNode_), targetLabel(targetLabel_), targetStatement(nullptr), targetCompoundParent(nullptr), isExceptionHandlingGoto(false)
{
}

void BoundGotoStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundGotoCaseStatement::BoundGotoCaseStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundGotoCaseStatement::SetValue(Cm::Sym::Value* value_)
{
    value.reset(value_);
}

void BoundGotoCaseStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundGotoDefaultStatement::BoundGotoDefaultStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundGotoDefaultStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundConditionalStatement::BoundConditionalStatement(Cm::Ast::Node* syntaxNode_) : BoundParentStatement(syntaxNode_)
{
}

void BoundConditionalStatement::SetCondition(BoundExpression* condition_)
{
    condition.reset(condition_);
}

void BoundConditionalStatement::AddStatement(BoundStatement* statement)
{
    statement->SetParent(this);
    if (!thenS)
    {
        thenS.reset(statement);
    }
    else
    {
        elseS.reset(statement);
    }
}

void BoundConditionalStatement::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    condition->Accept(visitor);
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

BoundWhileStatement::BoundWhileStatement(Cm::Ast::Node* syntaxNode_) : BoundParentStatement(syntaxNode_)
{
}

void BoundWhileStatement::SetCondition(BoundExpression* condition_)
{
    condition.reset(condition_);
}

void BoundWhileStatement::AddStatement(BoundStatement* statement_)
{
    statement_->SetParent(this);
    statement.reset(statement_);
}

void BoundWhileStatement::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    condition->Accept(visitor);
    if (!visitor.SkipContent())
    {
        statement->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

void BoundWhileStatement::AddBreakTargetLabel(Ir::Intf::LabelObject* breakTargetLabel) 
{
    breakTargetLabels.push_back(breakTargetLabel);
}

void BoundWhileStatement::AddContinueTargetLabel(Ir::Intf::LabelObject* continueTargetLabel)
{
    continueTargetLabels.push_back(continueTargetLabel);
}

void BoundWhileStatement::AddToBreakNextSet(Cm::Core::CfgNode* node)
{
    breakNextSet.insert(node);
}

void BoundWhileStatement::AddToContinueNextSet(Cm::Core::CfgNode* node)
{
    continueNextSet.insert(node);
}

BoundDoStatement::BoundDoStatement(Cm::Ast::Node* syntaxNode_) : BoundParentStatement(syntaxNode_)
{
}

void BoundDoStatement::SetCondition(BoundExpression* condition_)
{
    condition.reset(condition_);
}

void BoundDoStatement::AddStatement(BoundStatement* statement_)
{
    statement_->SetParent(this);
    statement.reset(statement_);
}

void BoundDoStatement::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (!visitor.SkipContent())
    {
        statement->Accept(visitor);
        condition->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

void BoundDoStatement::AddBreakTargetLabel(Ir::Intf::LabelObject* breakTargetLabel) 
{
    breakTargetLabels.push_back(breakTargetLabel);
}

void BoundDoStatement::AddContinueTargetLabel(Ir::Intf::LabelObject* continueTargetLabel)
{
    continueTargetLabels.push_back(continueTargetLabel);
}

void BoundDoStatement::AddToBreakNextSet(Cm::Core::CfgNode* node)
{
    breakNextSet.insert(node);
}

void BoundDoStatement::AddToContinueNextSet(Cm::Core::CfgNode* node)
{
    continueNextSet.insert(node);
}

BoundForStatement::BoundForStatement(Cm::Ast::Node* syntaxNode_) : BoundParentStatement(syntaxNode_)
{
}

void BoundForStatement::SetCondition(BoundExpression* condition_)
{
    condition.reset(condition_);
}

void BoundForStatement::SetIncrement(BoundExpression* increment_)
{
    increment.reset(increment_);
}

void BoundForStatement::AddStatement(BoundStatement* statement_)
{
    statement_->SetParent(this);
    if (!initS)
    {
        initS.reset(statement_);
    }
    else
    {
        action.reset(statement_);
    }
}

void BoundForStatement::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (!visitor.SkipContent())
    {
        initS->Accept(visitor);
        condition->Accept(visitor);
        increment->Accept(visitor);
        action->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

void BoundForStatement::AddBreakTargetLabel(Ir::Intf::LabelObject* breakTargetLabel)
{
    breakTargetLabels.push_back(breakTargetLabel);
}

void BoundForStatement::AddContinueTargetLabel(Ir::Intf::LabelObject* continueTargetLabel)
{
    continueTargetLabels.push_back(continueTargetLabel);
}

void BoundForStatement::AddToBreakNextSet(Cm::Core::CfgNode* node)
{
    breakNextSet.insert(node);
}

void BoundForStatement::AddToContinueNextSet(Cm::Core::CfgNode* node)
{
    continueNextSet.insert(node);
}

} } // namespace Cm::BoundTree
