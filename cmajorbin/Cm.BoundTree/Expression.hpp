/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BOUND_TREE_EXPRESSION_INCLUDED
#define CM_BOUND_TREE_EXPRESSION_INCLUDED
#include <Cm.BoundTree/BoundNode.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/Value.hpp>

namespace Cm { namespace BoundTree {

class BoundExpression : public BoundNode
{
public:
    BoundExpression(Cm::Ast::Node* syntaxNode_);
    bool IsBoundExpressionNode() const override { return true; }
    void SetType(Cm::Sym::TypeSymbol* type_) { type = type_;  }
private:
    Cm::Sym::TypeSymbol* type;
};

class BoundExpressionList
{
public:
    BoundExpressionList();
    void AddExpression(BoundExpression* expression);
    typedef std::vector<std::unique_ptr<BoundExpression>>::iterator iterator;
    iterator begin() { return expressions.begin(); }
    iterator end() { return expressions.end(); }
private:
    std::vector<std::unique_ptr<BoundExpression>> expressions;
};

class BoundLiteral : public BoundExpression
{
public:
    BoundLiteral(Cm::Ast::Node* syntaxNode_);
    void SetValue(Cm::Sym::Value* value_) { value.reset(value_); }
private:
    std::unique_ptr<Cm::Sym::Value> value;
};

enum class Operator
{
};

enum class ResultType
{

};

class BoundUnaryOp : public BoundExpression
{
public:
    BoundUnaryOp(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_);
private:
    Operator op;
    ResultType result;
    std::unique_ptr<BoundExpression> operand;
    Cm::Sym::FunctionSymbol* fun;
};

class BoundBinaryOp : public BoundExpression
{
public:
    BoundBinaryOp(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_);
private:
    Operator op;
    ResultType result;
    std::unique_ptr<BoundExpression> left;
    std::unique_ptr<BoundExpression> right;
    Cm::Sym::FunctionSymbol* fun;
};

class BoundCall : public BoundExpression
{
private:
    Cm::Sym::TypeSymbol type;
    Cm::Sym::FunctionSymbol* fun;
};

class BoundNew : public BoundExpression
{
private:
    Cm::Sym::FunctionSymbol* ctor;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_EXPRESSION_INCLUDED
