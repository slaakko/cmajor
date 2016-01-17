/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_BINDER_INCLUDED
#define CM_BIND_BINDER_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.BoundTree/BoundClass.hpp>
#include <Cm.BoundTree/BoundStatement.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Bind {
  
class Binder: public Cm::Ast::Visitor
{
public:
    Binder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_);

    void BeginVisit(Cm::Ast::NamespaceNode& namespaceNode) override;
    void EndVisit(Cm::Ast::NamespaceNode& namespaceNode) override;

    void BeginVisit(Cm::Ast::ClassNode& classNode) override;
    void EndVisit(Cm::Ast::ClassNode& classNode) override;
    void BeginVisit(Cm::Ast::ConstructorNode& constructorNode) override;
    void EndVisit(Cm::Ast::ConstructorNode& constructorNode) override;
    void BeginVisit(Cm::Ast::DestructorNode& destructorNode) override;
    void EndVisit(Cm::Ast::DestructorNode& destructorNode) override;
    void BeginVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode) override;
    void EndVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode) override;
    void BeginVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode) override;
    void EndVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode) override;
    void BeginVisit(Cm::Ast::StaticConstructorNode& staticConstructorNode) override;
    void EndVisit(Cm::Ast::StaticConstructorNode& staticConstructorNode) override;

    void BeginVisit(Cm::Ast::FunctionNode& functionNode) override;
    void EndVisit(Cm::Ast::FunctionNode& functionNode) override;

    void BeginVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode) override;
    void EndVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode) override;
    void BeginVisit(Cm::Ast::ForStatementNode& forStatementNode) override;
    void EndVisit(Cm::Ast::ForStatementNode& forStatementNode) override;
    void BeginVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode) override;

    void BeginVisit(Cm::Ast::ReturnStatementNode& returnStatementNode) override;
    void BeginVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode) override;
    void EndVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode) override;
    void BeginVisit(Cm::Ast::SwitchStatementNode& switchStatementNode) override;
    void EndVisit(Cm::Ast::SwitchStatementNode& switchStatementNode) override;
    void BeginVisit(Cm::Ast::CaseStatementNode& caseStatementNode) override;
    void EndVisit(Cm::Ast::CaseStatementNode& caseStatementNode) override;
    void BeginVisit(Cm::Ast::DefaultStatementNode& defaultStatementNode) override;
    void EndVisit(Cm::Ast::DefaultStatementNode& defaultStatementNode) override;
    void EndVisit(Cm::Ast::GotoCaseStatementNode& gotoCaseStatementNode) override;
    void Visit(Cm::Ast::GotoDefaultStatementNode& gotoDefaultStatementNode) override;
    void BeginVisit(Cm::Ast::WhileStatementNode& whileStatementNode) override;
    void EndVisit(Cm::Ast::WhileStatementNode& whileStatementNode) override;
    void BeginVisit(Cm::Ast::DoStatementNode& doStatementNode) override;
    void EndVisit(Cm::Ast::DoStatementNode& doStatementNode) override;
    void Visit(Cm::Ast::BreakStatementNode& breakStatementNode) override;
    void Visit(Cm::Ast::ContinueStatementNode& continueStatementNode) override;
    void Visit(Cm::Ast::GotoStatementNode& gotoStatementNode) override;
    void BeginVisit(Cm::Ast::SimpleStatementNode& simpleStatementNode) override;
    void BeginVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode) override;
    void BeginVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode) override;
    void BeginVisit(Cm::Ast::DeleteStatementNode& deleteStatementNode) override;
    void BeginVisit(Cm::Ast::DestroyStatementNode& destroyStatementNode) override;
    void BeginVisit(Cm::Ast::ThrowStatementNode& throwStatementNode) override;
    void Visit(Cm::Ast::TryStatementNode& tryStatementNode) override;
    void Visit(Cm::Ast::CatchNode& catchNode) override;
    void Visit(Cm::Ast::ExitTryStatementNode& exitTryStatementNode) override;
    void Visit(Cm::Ast::BeginCatchStatementNode& beginCatchStatementNode) override;
    void Visit(Cm::Ast::AssertStatementNode& assertStatementNode) override;
    void EndVisit(Cm::Ast::CondCompDisjunctionNode& condCompDisjunctionNode) override;
    void EndVisit(Cm::Ast::CondCompConjunctionNode& condCompConjunctionNode) override;
    void EndVisit(Cm::Ast::CondCompNotNode& condCompNotNode) override;
    void Visit(Cm::Ast::CondCompPrimaryNode& condCompPrimaryNode) override;
    void Visit(Cm::Ast::CondCompStatementNode& condCompStatementNode) override;
    void BeginContainerScope(Cm::Sym::ContainerScope* containerScope);
    void EndContainerScope();
    void AddBoundStatement(Cm::BoundTree::BoundStatement* boundStatement);
    Cm::BoundTree::BoundCompoundStatement* GetCurrentCompound();
    Cm::BoundTree::BoundCompoundStatement* ReleaseCurrentCompound();
    void BeginClass(Cm::Sym::ClassTypeSymbol* classTypeSymbol);
    void EndClass();
    void SetCurrentFunction(Cm::BoundTree::BoundFunction* function);
    Cm::BoundTree::BoundFunction* ReleaseCurrentFunction();
    void SetCurrentParent(Cm::BoundTree::BoundParentStatement* parent);
private:
    Cm::BoundTree::BoundCompileUnit& boundCompileUnit;
    Cm::Sym::ContainerScope* currentContainerScope;
    std::stack<Cm::Sym::ContainerScope*> containerScopeStack;
    std::unique_ptr<Cm::BoundTree::BoundFunction> boundFunction;
    std::stack<std::unique_ptr<Cm::BoundTree::BoundClass>> boundClassStack;
    std::unique_ptr<Cm::BoundTree::BoundClass> boundClass;
    std::unique_ptr<Cm::BoundTree::BoundParentStatement> currentParent;
    std::stack<Cm::BoundTree::BoundParentStatement*> parentStack;
    Cm::BoundTree::BoundSwitchStatement* switchStatement;
    std::stack<bool> condCompilationEvaluationStack;
    void PushCondCompValue(bool value);
    bool PopCondCompValue();
};

} } // namespace Cm::Bind

#endif // CM_BIND_BINDER_INCLUDED


