/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BOUND_TREE_BOUND_EXPRESSION_INCLUDED
#define CM_BOUND_TREE_BOUND_EXPRESSION_INCLUDED
#include <Cm.BoundTree/BoundNode.hpp>
#include <Cm.Core/Argument.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/ConstantSymbol.hpp>
#include <Cm.Sym/LocalVariableSymbol.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Sym/Value.hpp>
#include <Cm.Sym/ParameterSymbol.hpp>

namespace Cm { namespace BoundTree {

class BoundExpression : public BoundNode
{
public:
    BoundExpression(Cm::Ast::Node* syntaxNode_);
    bool IsBoundExpressionNode() const override { return true; }
    virtual bool IsContainerExpression() const { return false; }
    virtual bool IsBoundFunctionGroup() const { return false; }
    virtual bool IsBoundMemberVariable() const { return false; }
    virtual bool IsBoundUnaryOp() const { return false; }
    virtual bool IsCast() const { return false; }
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
    void Reverse();
    void Accept(Visitor& visitor);
private:
    std::vector<std::unique_ptr<BoundExpression>> expressions;
};

class BoundStringLiteral : public BoundExpression
{
public:
    BoundStringLiteral(Cm::Ast::Node* syntaxNode_, int id_);
    int Id() const { return id; }
    void Accept(Visitor& visitor) override;
private:
    int id;
};

class BoundLiteral : public BoundExpression
{
public:
    BoundLiteral(Cm::Ast::Node* syntaxNode_);
    void SetValue(Cm::Sym::Value* value_) { value.reset(value_); }
    Cm::Sym::Value* GetValue() const { return value.get(); }
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<Cm::Sym::Value> value;
};

class BoundConstant : public BoundExpression
{
public:
    BoundConstant(Cm::Ast::Node* syntaxNode_, Cm::Sym::ConstantSymbol* symbol_);
    Cm::Sym::ConstantSymbol* Symbol() const { return symbol; }
    void Accept(Visitor& visitor) override;
private:
    Cm::Sym::ConstantSymbol* symbol;
};

class BoundLocalVariable : public BoundExpression
{
public:
    BoundLocalVariable(Cm::Ast::Node* syntaxNode_, Cm::Sym::LocalVariableSymbol* symbol_);
    Cm::Sym::LocalVariableSymbol* Symbol() const { return symbol; }
    Cm::Core::ArgumentCategory GetArgumentCategory() const override { return Cm::Core::ArgumentCategory::lvalue; }
    void Accept(Visitor& visitor) override;
private:
    Cm::Sym::LocalVariableSymbol* symbol;
};

class BoundParameter: public BoundExpression
{
public:
    BoundParameter(Cm::Ast::Node* syntaxNode_, Cm::Sym::ParameterSymbol* symbol_);
    Cm::Sym::ParameterSymbol* Symbol() const { return symbol; }
    Cm::Core::ArgumentCategory GetArgumentCategory() const override;
    void Accept(Visitor& visitor) override;
private:
    Cm::Sym::ParameterSymbol* symbol;
};

class BoundMemberVariable : public BoundExpression
{
public:
    BoundMemberVariable(Cm::Ast::Node* syntaxNode_, Cm::Sym::MemberVariableSymbol* symbol_);
    bool IsBoundMemberVariable() const override { return true; }
    Cm::Sym::MemberVariableSymbol* Symbol() const { return symbol; }
    Cm::Core::ArgumentCategory GetArgumentCategory() const override { return Cm::Core::ArgumentCategory::lvalue; }
    void Accept(Visitor& visitor) override;
    void SetClassObject(Cm::BoundTree::BoundExpression* classObject_);
    Cm::BoundTree::BoundExpression* GetClassObject() const { return classObject.get(); }
private:
    std::unique_ptr<Cm::BoundTree::BoundExpression> classObject;
    Cm::Sym::MemberVariableSymbol* symbol;
};

class BoundContainerExpression : public BoundExpression
{
public:
    BoundContainerExpression(Cm::Ast::Node* syntaxNode_, Cm::Sym::ContainerSymbol* containerSymbol_);
    Cm::Sym::ContainerSymbol* ContainerSymbol() const { return containerSymbol; }
    bool IsContainerExpression() const override { return true; }
    void Accept(Visitor& visitor) override;
private:
    Cm::Sym::ContainerSymbol* containerSymbol;
};

class BoundConversion : public BoundExpression
{
public:
    BoundConversion(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_, Cm::Sym::FunctionSymbol* conversionFun_);
    BoundExpression* Operand() const { return operand.get(); }
    Cm::Sym::FunctionSymbol* ConversionFun() const { return conversionFun; }
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<BoundExpression> operand;
    Cm::Sym::FunctionSymbol* conversionFun;
};

class BoundCast : public BoundExpression
{
public:
    BoundCast(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_, Cm::Sym::FunctionSymbol* conversionFun_);
    Cm::Sym::FunctionSymbol* ConversionFun() const { return conversionFun; }
    void Accept(Visitor& visitor) override;
    bool IsCast() const override { return true; }
private:
    std::unique_ptr<BoundExpression> operand;
    Cm::Sym::FunctionSymbol* conversionFun;
};

class BoundUnaryOp : public BoundExpression
{
public:
    BoundUnaryOp(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_);
    bool IsBoundUnaryOp() const override { return true; }
    void SetFunction(Cm::Sym::FunctionSymbol* fun_) { fun = fun_; }
    Cm::Sym::FunctionSymbol* GetFunction() const { return fun; }
    BoundExpression* ReleaseOperand() { return operand.release(); }
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<BoundExpression> operand;
    Cm::Sym::FunctionSymbol* fun;
};

class BoundBinaryOp : public BoundExpression
{
public:
    BoundBinaryOp(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_);
    void SetFunction(Cm::Sym::FunctionSymbol* fun_) { fun = fun_; }
    Cm::Sym::FunctionSymbol* GetFunction() const { return fun; }
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<BoundExpression> left;
    std::unique_ptr<BoundExpression> right;
    Cm::Sym::FunctionSymbol* fun;
};

class BoundFunctionGroup : public BoundExpression
{
public:
    BoundFunctionGroup(Cm::Ast::Node* syntaxNode_, Cm::Sym::FunctionGroupSymbol* functionGroupSymbol_);
    Cm::Sym::FunctionGroupSymbol* GetFunctionGroupSymbol() const { return functionGroupSymbol; }
    void Accept(Visitor& visitor) override;
    bool IsBoundFunctionGroup() const override { return true; }
private:
    Cm::Sym::FunctionGroupSymbol* functionGroupSymbol;
};

class BoundFunctionCall : public BoundExpression
{
public:
    BoundFunctionCall(Cm::Ast::Node* syntaxNode_, BoundExpressionList&& arguments_);
    void SetFunction(Cm::Sym::FunctionSymbol* fun_) { fun = fun_; }
    Cm::Sym::FunctionSymbol* GetFunction() const { return fun; }
    void Accept(Visitor& visitor) override;
    BoundExpressionList& Arguments() { return arguments; }
private:
    BoundExpressionList arguments;
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
    void Accept(Visitor& visitor) override;
};

class BoundConjunction : public BoundBooleanBinaryExpression
{
public:
    BoundConjunction(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_);
    void Accept(Visitor& visitor) override;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_BOUND_EXPRESSION_INCLUDED
