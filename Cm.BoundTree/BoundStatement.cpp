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

void BoundStatementList::Accept(Visitor& visitor)
{
    for (const std::unique_ptr<BoundStatement>& statement : statements)
    {
        statement->Accept(visitor);
    }
}

BoundCompoundStatement::BoundCompoundStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundCompoundStatement::AddStatement(BoundStatement* statement)
{
    statementList.AddStatement(statement);
}

void BoundCompoundStatement::Accept(Visitor& visitor) 
{
    statementList.Accept(visitor);
    visitor.Visit(*this);
}

BoundReturnStatement::BoundReturnStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
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
        resolutionArguments.push_back(Cm::Core::Argument(argument->GetArgumentCategory(), argument->GetType()));
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
            argument.reset(new Cm::BoundTree::BoundConversion(argument->SyntaxNode(), argument.release(), conversionFun));
            argument->SetType(conversionFun->GetTargetType());
        }
    }
}

void BoundConstructionStatement::Accept(Visitor& visitor)
{
    arguments.Accept(visitor);
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

BoundSwitchStatement::BoundSwitchStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

BoundConditionalStatement::BoundConditionalStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

BoundForStatement::BoundForStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

} } // namespace Cm::BoundTree