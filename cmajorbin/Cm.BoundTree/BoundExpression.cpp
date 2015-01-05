/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/BoundExpression.hpp>
#include <Cm.BoundTree/Visitor.hpp>

namespace Cm { namespace BoundTree {

BoundExpression::BoundExpression(Cm::Ast::Node* syntaxNode_) : BoundNode(syntaxNode_), type(nullptr)
{
}

BoundExpressionList::BoundExpressionList()
{
}

void BoundExpressionList::Add(BoundExpression* expression)
{
    expressions.push_back(std::unique_ptr<BoundExpression>(expression));
}

void BoundExpressionList::InsertFront(BoundExpression* expr)
{
    expressions.insert(expressions.begin(), std::unique_ptr<BoundExpression>(expr));
}

BoundExpression* BoundExpressionList::GetLast()
{
    std::unique_ptr<BoundExpression> last = std::move(expressions.back());
    expressions.pop_back();
    return last.release();
}

void BoundExpressionList::Reverse()
{
    std::reverse(expressions.begin(), expressions.end());
}

void BoundExpressionList::Accept(Visitor& visitor)
{
    for (const std::unique_ptr<BoundExpression>& expression : expressions)
    {
        expression->Accept(visitor);
    }
}

BoundStringLiteral::BoundStringLiteral(Cm::Ast::Node* syntaxNode_, int id_) : BoundExpression(syntaxNode_), id(id_)
{
}

void BoundStringLiteral::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundLiteral::BoundLiteral(Cm::Ast::Node* syntaxNode_) : BoundExpression(syntaxNode_)
{
}

void BoundLiteral::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundConstant::BoundConstant(Cm::Ast::Node* syntaxNode_, Cm::Sym::ConstantSymbol* symbol_) : BoundExpression(syntaxNode_), symbol(symbol_)
{
}

void BoundConstant::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundLocalVariable::BoundLocalVariable(Cm::Ast::Node* syntaxNode_, Cm::Sym::LocalVariableSymbol* symbol_) : BoundExpression(syntaxNode_), symbol(symbol_)
{
}

void BoundLocalVariable::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundParameter::BoundParameter(Cm::Ast::Node* syntaxNode_, Cm::Sym::ParameterSymbol* symbol_) : BoundExpression(syntaxNode_), symbol(symbol_)
{
}

void BoundParameter::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundMemberVariable::BoundMemberVariable(Cm::Ast::Node* syntaxNode_, Cm::Sym::MemberVariableSymbol* symbol_) : BoundExpression(syntaxNode_), symbol(symbol_)
{
}

void BoundMemberVariable::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundMemberVariable::SetClassObject(Cm::BoundTree::BoundExpression* classObject_)
{
    classObject.reset(classObject_);
}

BoundContainerExpression::BoundContainerExpression(Cm::Ast::Node* syntaxNode_, Cm::Sym::ContainerSymbol* containerSymbol_) : BoundExpression(syntaxNode_), containerSymbol(containerSymbol_)
{
}

void BoundContainerExpression::Accept(Visitor& visitor)
{
    throw std::runtime_error("member function not applicable");
}

BoundConversion::BoundConversion(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_, Cm::Sym::FunctionSymbol* conversionFun_) : BoundExpression(syntaxNode_), operand(operand_), conversionFun(conversionFun_)
{
}

void BoundConversion::Accept(Visitor& visitor)
{
    operand->Accept(visitor);
    visitor.Visit(*this);
}

BoundCast::BoundCast(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_, Cm::Sym::FunctionSymbol* conversionFun_) : BoundExpression(syntaxNode_), operand(operand_), conversionFun(conversionFun_)
{
}

void BoundCast::Accept(Visitor& visitor)
{
    operand->Accept(visitor);
    visitor.Visit(*this);
}

BoundUnaryOp::BoundUnaryOp(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_) : BoundExpression(syntaxNode_), operand(operand_), fun(nullptr)
{
}

void BoundUnaryOp::Accept(Visitor& visitor)
{
    operand->Accept(visitor);
    visitor.Visit(*this);
}

BoundBinaryOp::BoundBinaryOp(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_) : BoundExpression(syntaxNode_), left(left_), right(right_), fun(nullptr)
{
}

void BoundBinaryOp::Accept(Visitor& visitor)
{
    left->Accept(visitor);
    right->Accept(visitor);
    visitor.Visit(*this);
}

BoundFunctionGroup::BoundFunctionGroup(Cm::Ast::Node* syntaxNode_, Cm::Sym::FunctionGroupSymbol* functionGroupSymbol_) : BoundExpression(syntaxNode_), functionGroupSymbol(functionGroupSymbol_)
{
}

void BoundFunctionGroup::Accept(Visitor& visitor)
{
    throw std::runtime_error("member function not applicable");
}

BoundFunctionCall::BoundFunctionCall(Cm::Ast::Node* syntaxNode_, BoundExpressionList&& arguments_) : BoundExpression(syntaxNode_), arguments(std::move(arguments_))
{
}

void BoundFunctionCall::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundBooleanBinaryExpression::BoundBooleanBinaryExpression(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_) : BoundExpression(syntaxNode_), left(left_), right(right_)
{
}

BoundDisjunction::BoundDisjunction(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_) : BoundBooleanBinaryExpression(syntaxNode_, left_, right_)
{
}

void BoundDisjunction::Accept(Visitor& visitor)
{
    Left()->Accept(visitor);
    Right()->Accept(visitor);
    visitor.Visit(*this);
}

BoundConjunction::BoundConjunction(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_) : BoundBooleanBinaryExpression(syntaxNode_, left_, right_)
{
}

void BoundConjunction::Accept(Visitor& visitor)
{
    Left()->Accept(visitor);
    Right()->Accept(visitor);
    visitor.Visit(*this);
}

} } // namespace Cm::BoundTree
