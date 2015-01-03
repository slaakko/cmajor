/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BOUND_TREE_BOUND_STATEMENT_INCLUDED
#define CM_BOUND_TREE_BOUND_STATEMENT_INCLUDED
#include <Cm.BoundTree/BoundExpression.hpp>
#include <Cm.Core/Argument.hpp>
#include <Cm.Sym/LocalVariableSymbol.hpp>

namespace Cm { namespace BoundTree {

class BoundStatement : public BoundNode
{
public:
    BoundStatement(Cm::Ast::Node* syntaxNode_);
    virtual bool IsConditionStatement() const { return false; }
    virtual bool IsBoundCompoundStatement() const { return false; }
    virtual bool IsBoundConditionalStatement() const { return false; }
    virtual bool IsBoundWhileStatement() const { return false; }
    virtual bool IsBoundDoStatement() const { return false; }
    virtual bool IsBoundForStatement() const { return false; }
};

class BoundStatementList
{
public:
    BoundStatementList();
    void AddStatement(BoundStatement* statement);
    void InsertStatement(int index, BoundStatement* statement);
    void Accept(Visitor& visitor);
    bool IsEmpty() const { return statements.empty(); }
private:
    std::vector<std::unique_ptr<BoundStatement>> statements;
};

class BoundParentStatement : public BoundStatement
{
public:
    BoundParentStatement(Cm::Ast::Node* syntaxNode_);
    virtual void AddStatement(BoundStatement* statement) = 0;
};

class BoundCompoundStatement : public BoundParentStatement
{
public:
    BoundCompoundStatement(Cm::Ast::Node* syntaxNode_);
    void AddStatement(BoundStatement* statement) override;
    void InsertStatement(int index, BoundStatement* statement);
    bool IsBoundCompoundStatement() const override { return true; }
    void Accept(Visitor& visitor) override;
    bool IsEmpty() const { return statementList.IsEmpty(); }
private:
    BoundStatementList statementList;
};

class BoundReceiveStatement : public BoundStatement
{
public:
    BoundReceiveStatement(Cm::Sym::ParameterSymbol* parameterSymbol_);
    Cm::Sym::ParameterSymbol* GetParameterSymbol() const { return parameterSymbol; }
    void SetConstructor(Cm::Sym::FunctionSymbol* ctor_) { ctor = ctor_; }
    Cm::Sym::FunctionSymbol* Constructor() const { return ctor; }
    void Accept(Visitor& visitor) override;
private:
    Cm::Sym::ParameterSymbol* parameterSymbol;
    Cm::Sym::FunctionSymbol* ctor;
};

class BoundReturnStatement : public BoundStatement
{
public:
    BoundReturnStatement(Cm::Ast::Node* syntaxNode_);
    void SetExpression(BoundExpression* expression_);
    BoundExpression* Expression() const { return expression.get(); }
    void SetConstructor(Cm::Sym::FunctionSymbol* ctor_) { ctor = ctor_; }
    Cm::Sym::FunctionSymbol* Constructor() const { return ctor; }
    void Accept(Visitor& visitor) override;
    bool ReturnsValue() const { return expression != nullptr; }
    Cm::Sym::TypeSymbol* GetReturnType() const { return returnType; }
    void SetReturnType(Cm::Sym::TypeSymbol* returnType_) { returnType = returnType_; }
private:
    std::unique_ptr<BoundExpression> expression;
    Cm::Sym::FunctionSymbol* ctor;
    Cm::Sym::TypeSymbol* returnType;
};

class BoundConstructionStatement : public BoundStatement
{
public:
    BoundConstructionStatement(Cm::Ast::Node* syntaxNode_);
    void SetLocalVariable(Cm::Sym::LocalVariableSymbol* localVariable_) { localVariable = localVariable_;  }
    Cm::Sym::LocalVariableSymbol* LocalVariable() const { return localVariable; }
    void SetArguments(BoundExpressionList&& arguments_);
    void GetResolutionArguments(std::vector<Cm::Core::Argument>& resolutionArguments);
    void SetConstructor(Cm::Sym::FunctionSymbol* ctor_) { ctor = ctor_; }
    Cm::Sym::FunctionSymbol* Constructor() const { return ctor; }
    void InsertLocalVariableToArguments();
    void ApplyConversions(const std::vector<Cm::Sym::FunctionSymbol*>& conversions);
    void Accept(Visitor& visitor) override;
    BoundExpressionList& Arguments() { return arguments; }
private:
    Cm::Sym::LocalVariableSymbol* localVariable;
    BoundExpressionList arguments;
    Cm::Sym::FunctionSymbol* ctor;
};

class BoundAssignmentStatement : public BoundStatement
{
public:
    BoundAssignmentStatement(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_, Cm::Sym::FunctionSymbol* assignment_);
    void Accept(Visitor& visitor) override;
    Cm::Sym::FunctionSymbol* Assignment() const { return assignment; }
private:
    std::unique_ptr<BoundExpression> left;
    std::unique_ptr<BoundExpression> right;
    Cm::Sym::FunctionSymbol* assignment;
};

class BoundThrowStatement : public BoundStatement
{

private:
    std::unique_ptr<BoundExpression> exception;
};

class BoundSimpleStatement : public BoundStatement
{
public:
    BoundSimpleStatement(Cm::Ast::Node* syntaxNode_);
    void SetExpression(BoundExpression* expression_);
    void Accept(Visitor& visitor) override;
    bool HasExpression() const { return expression != nullptr; }
private:
    std::unique_ptr<BoundExpression> expression;
};

class BoundSwitchStatement : public BoundStatement
{
public:
    BoundSwitchStatement(Cm::Ast::Node* syntaxNode_);
    bool IsConditionStatement() const override { return true; }
private:
    std::unique_ptr<BoundExpression> condition;
};

class BoundBreakStatement : public BoundStatement
{

private:
    // target label
};

class BoundContinueStatement : public BoundStatement
{

private:
    // target label
};

class BoundConditionalStatement : public BoundParentStatement
{
public:
    BoundConditionalStatement(Cm::Ast::Node* syntaxNode_);
    bool IsConditionStatement() const override { return true; }
    void SetCondition(BoundExpression* condition_);
    void AddStatement(BoundStatement* statement) override;
    void Accept(Visitor& visitor) override;
    bool IsBoundConditionalStatement() const override { return true; }
    BoundStatement* ThenS() const { return thenS.get(); }
    BoundStatement* ElseS() const { return elseS.get(); }
private:
    std::unique_ptr<BoundExpression> condition;
    std::unique_ptr<BoundStatement> thenS;
    std::unique_ptr<BoundStatement> elseS;
};

class BoundWhileStatement : public BoundParentStatement
{
public:
    BoundWhileStatement(Cm::Ast::Node* syntaxNode_);
    bool IsConditionStatement() const override { return true; }
    void SetCondition(BoundExpression* condition_);
    void AddStatement(BoundStatement* statement_) override;
    bool IsBoundWhileStatement() const override { return true; }
    void Accept(Visitor& visitor) override;
    BoundStatement* Statement() const { return statement.get(); }
private:
    std::unique_ptr<BoundExpression> condition;
    std::unique_ptr<BoundStatement> statement;
};

class BoundDoStatement : public BoundParentStatement
{
public:
    BoundDoStatement(Cm::Ast::Node* syntaxNode_);
    bool IsConditionStatement() const override { return true; }
    void SetCondition(BoundExpression* condition_);
    void AddStatement(BoundStatement* statement_) override;
    bool IsBoundDoStatement() const override { return true; }
    void Accept(Visitor& visitor) override;
    BoundStatement* Statement() const { return statement.get(); }
    BoundExpression* Condition() const { return condition.get(); }
private:
    std::unique_ptr<BoundStatement> statement;
    std::unique_ptr<BoundExpression> condition;
};

class BoundForStatement : public BoundParentStatement
{
public:
    BoundForStatement(Cm::Ast::Node* syntaxNode_);
    void SetCondition(BoundExpression* condition_);
    void SetIncrement(BoundExpression* increment_);
    void AddStatement(BoundStatement* statement_) override;
    bool IsConditionStatement() const override { return true; }
    bool IsBoundForStatement() const override { return true; }
    void Accept(Visitor& visitor) override;
    BoundStatement* InitS() const { return initS.get(); }
    BoundExpression* Condition() const { return condition.get(); }
    BoundExpression* Increment() const { return increment.get(); }
    BoundStatement* Action() const { return action.get(); }
private:
    std::unique_ptr<BoundStatement> initS;
    std::unique_ptr<BoundExpression> condition;
    std::unique_ptr<BoundExpression> increment;
    std::unique_ptr<BoundStatement> action;
};

class BoundTryStatement : public BoundStatement
{
private:
    std::unique_ptr<BoundStatement> tryBlock;
    // catches
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_BOUND_STATEMENT_INCLUDED
