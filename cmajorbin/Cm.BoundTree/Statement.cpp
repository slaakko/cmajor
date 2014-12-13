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

BoundConstructionStatement::BoundConstructionStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_), localVariable(nullptr)
{
}

void BoundConstructionStatement::AddArgument(BoundExpression* argument)
{
    arguments.AddExpression(argument);
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