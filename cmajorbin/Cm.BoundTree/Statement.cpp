/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/Statement.hpp>

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

BoundCompoundStatement::BoundCompoundStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundCompoundStatement::AddStatement(BoundStatement* statement)
{
    statementList.AddStatement(statement);
}

BoundReturnStatement::BoundReturnStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
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
        }
    }
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