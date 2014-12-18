/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BOUND_TREE_STATEMENT_INCLUDED
#define CM_BOUND_TREE_STATEMENT_INCLUDED
#include <Cm.BoundTree/Expression.hpp>
#include <Cm.Core/Argument.hpp>
#include <Cm.Sym/LocalVariableSymbol.hpp>

namespace Cm { namespace BoundTree {

class BoundStatement : public BoundNode
{
public:
    BoundStatement(Cm::Ast::Node* syntaxNode_);
    virtual bool IsBoundCompoundStatement() const { return false; }
};

class BoundStatementList
{
public:
    BoundStatementList();
    void AddStatement(BoundStatement* statement);
private:
    std::vector<std::unique_ptr<BoundStatement>> statements;
};

class BoundCompoundStatement : public BoundStatement
{
public:
    BoundCompoundStatement(Cm::Ast::Node* syntaxNode_);
    void AddStatement(BoundStatement* statement);
    bool IsBoundCompoundStatement() const override { return true; }
private:
    BoundStatementList statementList;
};

class BoundReceiveStatement : public BoundStatement
{

};

class BoundReturnStatement : public BoundStatement
{
public:
    BoundReturnStatement(Cm::Ast::Node* syntaxNode_);
private:
    std::unique_ptr<BoundExpression> expression;
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
    void InsertLocalVariableToArguments();
    void ApplyConversions(const std::vector<Cm::Sym::FunctionSymbol*>& conversions);
private:
    Cm::Sym::LocalVariableSymbol* localVariable;
    BoundExpressionList arguments;
    Cm::Sym::FunctionSymbol* ctor;
};

class BoundThrowStatement : public BoundStatement
{

private:
    std::unique_ptr<BoundExpression> exception;
};

class BoundSimpleStatement : public BoundStatement
{
private:
    std::unique_ptr<BoundExpression> expression;
};

class BoundSwitchStatement : public BoundStatement
{
public:
    BoundSwitchStatement(Cm::Ast::Node* syntaxNode_);
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

class BoundConditionalStatement : public BoundStatement
{
public:
    BoundConditionalStatement(Cm::Ast::Node* syntaxNode_);
private:
    std::unique_ptr<BoundExpression> condition;
    std::unique_ptr<BoundStatement> thenS;
    std::unique_ptr<BoundStatement> elseS;
};

class BoundDoStatement : public BoundStatement
{
private:
    std::unique_ptr<BoundStatement> statement;
    std::unique_ptr<BoundExpression> condition;
};

class BoundWhileStatement : public BoundStatement
{
private:
    std::unique_ptr<BoundExpression> condition;
    std::unique_ptr<BoundStatement> statement;
};

class BoundForStatement : public BoundStatement
{
public:
    BoundForStatement(Cm::Ast::Node* syntaxNode_);
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

#endif // CM_BOUND_TREE_STATEMENT_INCLUDED
