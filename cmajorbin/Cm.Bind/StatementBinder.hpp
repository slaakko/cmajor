/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_STATEMENT_INCLUDED
#define CM_BIND_STATEMENT_INCLUDED
#include <Cm.Bind/ExpressionBinder.hpp>
#include <Cm.BoundTree/BoundStatement.hpp>

namespace Cm { namespace Bind {

class StatementBinder : public ExpressionBinder
{
public:
    StatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScope_, 
        Cm::BoundTree::BoundFunction* currentFunction_);
    Cm::Sym::SymbolTable& SymbolTable() { return symbolTable; }
    Cm::Sym::ContainerScope* ContainerScope() const { return containerScope; }
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& FileScopes() const { return fileScopes; }
    void SetResult(Cm::BoundTree::BoundStatement* result_) { result = result_; }
    Cm::BoundTree::BoundStatement* Result() const { return result; }
private:
    Cm::Sym::SymbolTable& symbolTable;
    Cm::Sym::ContainerScope* containerScope;
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes;
    Cm::BoundTree::BoundStatement* result;
};

class ConstructionStatementBinder : public StatementBinder
{
public:
    ConstructionStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
        Cm::BoundTree::BoundFunction* currentFunction_);
    void BeginVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode) override;
    void EndVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode) override;
private:
    Cm::BoundTree::BoundConstructionStatement* constructionStatement;
};

class AssignmentStatementBinder : public StatementBinder
{
public:
    AssignmentStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, 
        Cm::BoundTree::BoundFunction* currentFunction_);
    void EndVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode) override;
};

class SimpleStatementBinder : public StatementBinder
{
public:
    SimpleStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, 
        Cm::BoundTree::BoundFunction* currentFunction_);
    void EndVisit(Cm::Ast::SimpleStatementNode& simpleStatementNode) override;
};

class ReturnStatementBinder : public StatementBinder
{
public:
    ReturnStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, 
        Cm::BoundTree::BoundFunction* currentFunction_);
    void EndVisit(Cm::Ast::ReturnStatementNode& returnStatementNode) override;
};

class ConditionalStatementBinder : public StatementBinder
{
public:
    ConditionalStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
        Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundConditionalStatement* conditionalStatement_);
    void EndVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode) override;
private:
    Cm::BoundTree::BoundConditionalStatement* conditionalStatement;
};

class WhileStatementBinder : public StatementBinder
{
public:
    WhileStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, 
        Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundWhileStatement* whileStatement_);
    void EndVisit(Cm::Ast::WhileStatementNode& whileStatementNode) override;
private:
    Cm::BoundTree::BoundWhileStatement* whileStatement;
};

class DoStatementBinder : public StatementBinder
{
public:
    DoStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, 
        Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundDoStatement* doStatement_);
    void EndVisit(Cm::Ast::DoStatementNode& doStatementNode) override;
private:
    Cm::BoundTree::BoundDoStatement* doStatement;
};

class ForStatementBinder : public StatementBinder
{
public:
    ForStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, 
        Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundForStatement* forStatement_);
    void EndVisit(Cm::Ast::ForStatementNode& forStatementNode) override;
private:
    Cm::BoundTree::BoundForStatement* forStatement;
};

class Binder;

class RangeForStatementBinder : public StatementBinder
{
public:
    RangeForStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
        Cm::BoundTree::BoundFunction* currentFunction_, Cm::Ast::RangeForStatementNode& rangeForStatementNode, Binder& binder_);
    void EndVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode) override;
private:
    Binder& binder;
};

class SwitchStatementBinder : public StatementBinder
{
public:
    SwitchStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, 
        Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundSwitchStatement* switchStatement_);
    void BeginVisit(Cm::Ast::SwitchStatementNode& switchStatementNode) override;
    void EndVisit(Cm::Ast::SwitchStatementNode& switchStatementNode) override;
private:
    Cm::BoundTree::BoundSwitchStatement* switchStatement;
};

class CaseStatementBinder : public StatementBinder
{
public:
    CaseStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, 
        Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundCaseStatement* caseStatement_, Cm::BoundTree::BoundSwitchStatement* switchStatement_);
    void EndVisit(Cm::Ast::CaseStatementNode& caseStatementNode) override;
private:
    Cm::BoundTree::BoundCaseStatement* caseStatement;
    Cm::BoundTree::BoundSwitchStatement* switchStatement;
};

class DefaultStatementBinder : public StatementBinder
{
public:
    DefaultStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, 
        Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundDefaultStatement* defaultStatement_);
    void EndVisit(Cm::Ast::DefaultStatementNode& defaultStatementNode) override;
private:
    Cm::BoundTree::BoundDefaultStatement* defaultStatement;
};

class BreakStatementBinder : public StatementBinder
{
public:
    BreakStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, 
        Cm::BoundTree::BoundFunction* currentFunction_);
    void Visit(Cm::Ast::BreakStatementNode& breakStatementNode) override;
};

class ContinueStatementBinder : public StatementBinder
{
public:
    ContinueStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, 
        Cm::BoundTree::BoundFunction* currentFunction_);
    void Visit(Cm::Ast::ContinueStatementNode& continueStatementNode) override;
};

class GotoCaseStatementBinder : public StatementBinder
{
public:
    GotoCaseStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, 
        Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundSwitchStatement* switchStatement_);
    void EndVisit(Cm::Ast::GotoCaseStatementNode& gotoCaseStatementNode) override;
private:
    Cm::BoundTree::BoundSwitchStatement* switchStatement;
};

class GotoDefaultStatementBinder : public StatementBinder
{
public:
    GotoDefaultStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, 
        Cm::BoundTree::BoundFunction* currentFunction_);
    void Visit(Cm::Ast::GotoDefaultStatementNode& gotoDefaultStatementNode) override;
};

class DestroyStatementBinder : public StatementBinder
{
public:
    DestroyStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
        Cm::BoundTree::BoundFunction* currentFunction_);
    void EndVisit(Cm::Ast::DestroyStatementNode& destroyStatementNode) override;
};

class DeleteStatementBinder : public StatementBinder
{
public:
    DeleteStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
        Cm::BoundTree::BoundFunction* currentFunction_);
    void EndVisit(Cm::Ast::DeleteStatementNode& deleteStatementNode) override;
    Cm::BoundTree::BoundStatement* GetFreeStatement() const { return freeStatement; }
private:
    Cm::BoundTree::BoundStatement* freeStatement;
};

class ThrowStatementBinder : public StatementBinder
{
public:
    ThrowStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
        Cm::BoundTree::BoundFunction* currentFunction_, Binder& binder_);
    void EndVisit(Cm::Ast::ThrowStatementNode& throwStatementNode) override;
private:
    Binder& binder;
};

class TryBinder : public StatementBinder
{
public:
    TryBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
        Cm::BoundTree::BoundFunction* currentFunction_, Binder& binder_);
    void Visit(Cm::Ast::TryStatementNode& tryStatementNode) override;
private:
    Binder& binder;
};

class CatchBinder : public StatementBinder
{
public:
    CatchBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
        Cm::BoundTree::BoundFunction* currentFunction_, Binder& binder_);
    void Visit(Cm::Ast::CatchNode& catchNode) override;
private:
    Binder& binder;
};

} } // namespace Cm::Bind

#endif CM_BIND_STATEMENT_INCLUDED
