/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/Expression.hpp>

namespace Cm { namespace BoundTree {

BoundExpression::BoundExpression(Cm::Ast::Node* syntaxNode_) : BoundNode(syntaxNode_), type(nullptr)
{
}

BoundExpressionList::BoundExpressionList()
{
}

void BoundExpressionList::AddExpression(BoundExpression* expression)
{
    expressions.push_back(std::unique_ptr<BoundExpression>(expression));
}

BoundLiteral::BoundLiteral(Cm::Ast::Node* syntaxNode_) : BoundExpression(syntaxNode_)
{
}

BoundUnaryOp::BoundUnaryOp(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_) : BoundExpression(syntaxNode_), operand(operand_), fun(nullptr)
{
}

BoundBinaryOp::BoundBinaryOp(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_) : BoundExpression(syntaxNode_), left(left_), right(right_)
{
}

} } // namespace Cm::BoundTree
