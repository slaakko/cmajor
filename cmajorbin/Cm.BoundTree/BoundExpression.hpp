/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BOUND_TREE_EXPRESSION_INCLUDED
#define CM_BOUND_TREE_EXPRESSION_INCLUDED
#include <Cm.BoundTree/BoundNode.hpp>
#include <Cm.Core/Argument.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/ConstantSymbol.hpp>
#include <Cm.Sym/LocalVariableSymbol.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Sym/Value.hpp>

namespace Cm { namespace BoundTree {

class BoundExpression : public BoundNode
{
public:
    BoundExpression(Cm::Ast::Node* syntaxNode_);
    bool IsBoundExpressionNode() const override { return true; }
    virtual bool IsContainerExpression() const { return false; }
    void SetType(Cm::Sym::TypeSymbol* type_) { type = type_;  }
    Cm::Sym::TypeSymbol* GetType() const { return type; }
    virtual Cm::Core::ArgumentCategory GetArgumentCategory() const { return Cm::Core::ArgumentCategory::rvalue; }
private:
    Cm::Sym::TypeSymbol* type;
};

class BoundExpressionList
{
public:
    BoundExpressionList();
    typedef std::vector<std::unique_ptr<BoundExpression>>::iterator iterator;
    iterator begin() { return expressions.begin(); }
    iterator end() { return expressions.end(); }
    BoundExpressionList(BoundExpressionList&& that) = default;
    BoundExpressionList& operator=(BoundExpressionList&& that) = default;
    void Add(BoundExpression* expression);
    bool Empty() const { return expressions.empty(); }
    int Count() const { return int(expressions.size()); }
    std::unique_ptr<BoundExpression>& operator[](int index) { return expressions[index]; }
    void InsertFront(BoundExpression* expr);
    BoundExpression* GetLast();
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

class BoundConstant : public BoundExpression
{
public:
    BoundConstant(Cm::Ast::Node* syntaxNode_, Cm::Sym::ConstantSymbol* symbol_);
    Cm::Sym::ConstantSymbol* Symbol() const { return symbol; }
private:
    Cm::Sym::ConstantSymbol* symbol;
};

class BoundLocalVariable : public BoundExpression
{
public:
    BoundLocalVariable(Cm::Ast::Node* syntaxNode_, Cm::Sym::LocalVariableSymbol* symbol_);
    Cm::Sym::LocalVariableSymbol* Symbol() const { return symbol; }
private:
    Cm::Sym::LocalVariableSymbol* symbol;
};

class BoundMemberVariable : public BoundExpression
{
public:
    BoundMemberVariable(Cm::Ast::Node* syntaxNode_, Cm::Sym::MemberVariableSymbol* symbol_);
    Cm::Sym::MemberVariableSymbol* Symbol() const { return symbol; }
private:
    Cm::Sym::MemberVariableSymbol* symbol;
};

class BoundContainerExpression : public BoundExpression
{
public:
    BoundContainerExpression(Cm::Ast::Node* syntaxNode_, Cm::Sym::ContainerSymbol* containerSymbol_);
    Cm::Sym::ContainerSymbol* ContainerSymbol() const { return containerSymbol; }
    bool IsContainerExpression() const override { return true; }
private:
    Cm::Sym::ContainerSymbol* containerSymbol;
};

class BoundConversion : public BoundExpression
{
public:
    BoundConversion(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_, Cm::Sym::FunctionSymbol* conversionFun_);
private:
    std::unique_ptr<BoundExpression> operand;
    Cm::Sym::FunctionSymbol* conversionFun;
};

class BoundCast : public BoundExpression
{
public:
    BoundCast(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_);
private:
    std::unique_ptr<BoundExpression> operand;
};

class BoundUnaryOp : public BoundExpression
{
public:
    BoundUnaryOp(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_);
    void SetFuncion(Cm::Sym::FunctionSymbol* fun_) { fun = fun_; }
private:
    std::unique_ptr<BoundExpression> operand;
    Cm::Sym::FunctionSymbol* fun;
};

class BoundBinaryOp : public BoundExpression
{
public:
    BoundBinaryOp(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_);
    void SetFuncion(Cm::Sym::FunctionSymbol* fun_) { fun = fun_; }
private:
    std::unique_ptr<BoundExpression> left;
    std::unique_ptr<BoundExpression> right;
    Cm::Sym::FunctionSymbol* fun;
};

class BoundBooleanBinaryExpression : public BoundExpression
{
public:
    BoundBooleanBinaryExpression(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_);
    BoundExpression* Left() const { return left.get(); }
    BoundExpression* Right() const { return right.get(); }
private:
    std::unique_ptr<BoundExpression> left;
    std::unique_ptr<BoundExpression> right;
};

class BoundDisjunction : public BoundBooleanBinaryExpression
{
public:    
    BoundDisjunction(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_);
};

class BoundConjunction : public BoundBooleanBinaryExpression
{
public:
    BoundConjunction(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_);
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_EXPRESSION_INCLUDED
