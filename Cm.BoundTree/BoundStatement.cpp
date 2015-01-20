/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/BoundStatement.hpp>
#include <Cm.BoundTree/Visitor.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>

namespace Cm { namespace BoundTree {

BoundStatement::BoundStatement(Cm::Ast::Node* syntaxNode_) : BoundNode(syntaxNode_)
{
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
    statementList.AddStatement(statement);
}

void BoundCompoundStatement::InsertStatement(int index, BoundStatement* statement)
{
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

BoundInitMemberVariableStatement::BoundInitMemberVariableStatement(Cm::Sym::FunctionSymbol* ctor_, BoundExpressionList&& arguments_) : BoundStatement(nullptr), ctor(ctor_), arguments(std::move(arguments_))
{
}

void BoundInitMemberVariableStatement::Accept(Visitor& visitor) 
{
    visitor.Visit(*this);
}

BoundFunctionCallStatement::BoundFunctionCallStatement(Cm::Sym::FunctionSymbol* function_, BoundExpressionList&& arguments_) : BoundStatement(nullptr), function(function_), arguments(std::move(arguments_))
{
}

void BoundFunctionCallStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
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
    expression->Accept(visitor);
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

void BoundConstructionStatement::GetResolutionArguments(std::vector<Cm::Core::Argument>& resolutionArguments)
{
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

void BoundConstructionStatement::ApplyConversions(const std::vector<Cm::Sym::FunctionSymbol*>& conversions)
{
    int n = int(conversions.size());
    if (n != arguments.Count())
    {
        throw std::runtime_error("wrong number of conversions");
    }
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::FunctionSymbol* conversionFun = conversions[i];
        if (conversionFun)
        {
            std::unique_ptr<BoundExpression>& argument = arguments[i];
            BoundExpression* arg = argument.release();
            argument.reset(new Cm::BoundTree::BoundConversion(arg->SyntaxNode(), arg, conversionFun));
            argument->SetType(conversionFun->GetTargetType());
        }
    }
}

void BoundConstructionStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundAssignmentStatement::BoundAssignmentStatement(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_, Cm::Sym::FunctionSymbol* assignment_) : 
    BoundStatement(syntaxNode_), left(left_), right(right_), assignment(assignment_)
{
}

void BoundAssignmentStatement::Accept(Visitor& visitor)
{
    left->Accept(visitor);
    right->Accept(visitor);
    visitor.Visit(*this);
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
    if (expression)
    {
        expression->Accept(visitor);
    }
    visitor.Visit(*this);
}

BoundSwitchStatement::BoundSwitchStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
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

BoundDoStatement::BoundDoStatement(Cm::Ast::Node* syntaxNode_) : BoundParentStatement(syntaxNode_)
{
}

void BoundDoStatement::SetCondition(BoundExpression* condition_)
{
    condition.reset(condition_);
}

void BoundDoStatement::AddStatement(BoundStatement* statement_)
{
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

} } // namespace Cm::BoundTree