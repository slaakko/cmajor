/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_BINDING_VISITOR_INCLUDED
#define CM_BIND_BINDING_VISITOR_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.BoundTree/BoundStatement.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Bind {

class BindingVisitor : public Cm::Ast::Visitor
{
public:
    BindingVisitor(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_);
    void BeginVisit(Cm::Ast::CompileUnitNode& compileUnitNode) override;
    void EndVisit(Cm::Ast::CompileUnitNode& compileUnitNode) override;
    void BeginVisit(Cm::Ast::NamespaceNode& namespaceNode) override;
    void EndVisit(Cm::Ast::NamespaceNode& namespaceNode) override;
    void Visit(Cm::Ast::AliasNode& aliasNode) override;
    void Visit(Cm::Ast::NamespaceImportNode& namespaceImportNode) override;

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
    void Visit(Cm::Ast::MemberVariableNode& memberVariableNode) override;
    void BeginVisit(Cm::Ast::EnumTypeNode& enumTypeNode) override;
    void EndVisit(Cm::Ast::EnumTypeNode& enumTypeNode) override;
    void Visit(Cm::Ast::EnumConstantNode& enumConstantNode) override;
    void Visit(Cm::Ast::TypedefNode& typedefNode) override;
    void Visit(Cm::Ast::ConstantNode& constantNode) override;
    void Visit(Cm::Ast::ParameterNode& parameterNode) override;

    void BeginVisit(Cm::Ast::FunctionNode& functionNode) override;
    void EndVisit(Cm::Ast::FunctionNode& functionNode) override;
    void BeginVisit(Cm::Ast::DelegateNode& delegateNode) override;
    void EndVisit(Cm::Ast::DelegateNode& delegateNode) override;
    void BeginVisit(Cm::Ast::ClassDelegateNode& classDelegateNode) override;
    void EndVisit(Cm::Ast::ClassDelegateNode& classDelegateNode) override;

    void BeginVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode) override;
    void EndVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode) override;
    void BeginVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode) override;
    void EndVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode) override;
    void BeginVisit(Cm::Ast::ForStatementNode& forStatementNode) override;
    void EndVisit(Cm::Ast::ForStatementNode& forStatementNode) override;

    void BeginVisit(Cm::Ast::ReturnStatementNode& returnStatementNode) override;
    void EndVisit(Cm::Ast::ReturnStatementNode& returnStatementNode) override;
    void BeginVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode) override;
    void EndVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode) override;
    void BeginVisit(Cm::Ast::SwitchStatementNode& switchStatementNode) override;
    void EndVisit(Cm::Ast::SwitchStatementNode& switchStatementNode) override;
    void BeginVisit(Cm::Ast::CaseStatementNode& caseStatementNode) override;
    void EndVisit(Cm::Ast::CaseStatementNode& caseStatementNode) override;
    void BeginVisit(Cm::Ast::DefaultStatementNode& defaultStatementNode) override;
    void EndVisit(Cm::Ast::DefaultStatementNode& defaultStatementNode) override;
    void BeginVisit(Cm::Ast::GotoCaseStatementNode& gotoCaseStatementNode) override;
    void EndVisit(Cm::Ast::GotoCaseStatementNode& gotoCaseStatementNode) override;
    void Visit(Cm::Ast::GotoDefaultStatementNode& gotoDefaultStatementNode) override;
    void BeginVisit(Cm::Ast::WhileStatementNode& whileStatementNode) override;
    void EndVisit(Cm::Ast::WhileStatementNode& whileStatementNode) override;
    void BeginVisit(Cm::Ast::DoStatementNode& doStatementNode) override;
    void EndVisit(Cm::Ast::DoStatementNode& doStatementNode) override;
    void Visit(Cm::Ast::BreakStatementNode& breakStatementNode) override;
    void Visit(Cm::Ast::ContinueStatementNode& continueStatementNode) override;
    void Visit(Cm::Ast::GotoStatementNode& gotoStatementNode) override;
    void Visit(Cm::Ast::TypedefStatementNode& typedefStatementNode) override;
    void BeginVisit(Cm::Ast::SimpleStatementNode& simpleStatementNode) override;
    void EndVisit(Cm::Ast::SimpleStatementNode& simpleStatementNode) override;
    void BeginVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode) override;
    void EndVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode) override;
    void BeginVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode) override;
    void EndVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode) override;
    void BeginVisit(Cm::Ast::DeleteStatementNode& deleteStatementNode) override;
    void EndVisit(Cm::Ast::DeleteStatementNode& deleteStatementNode) override;
    void BeginVisit(Cm::Ast::DestroyStatementNode& destroyStatementNode) override;
    void EndVisit(Cm::Ast::DestroyStatementNode& destroyStatementNode) override;
    void BeginVisit(Cm::Ast::ThrowStatementNode& throwStatementNode) override;
    void EndVisit(Cm::Ast::ThrowStatementNode& throwStatementNode) override;
    void BeginVisit(Cm::Ast::TryStatementNode& tryStatementNode) override;
    void EndVisit(Cm::Ast::TryStatementNode& tryStatementNode) override;
    void BeginVisit(Cm::Ast::CatchNode& catchNode) override;
    void EndVisit(Cm::Ast::CatchNode& catchNode) override;
    void BeginVisit(Cm::Ast::AssertStatementNode& assertStatementNode) override;
    void EndVisit(Cm::Ast::AssertStatementNode& assertStatementNode) override;
    void BeginVisit(Cm::Ast::CondCompDisjunctionNode& condCompDisjunctionNode) override;
    void EndVisit(Cm::Ast::CondCompDisjunctionNode& condCompDisjunctionNode) override;
    void BeginVisit(Cm::Ast::CondCompConjunctionNode& condCompDisjunctionNode) override;
    void EndVisit(Cm::Ast::CondCompConjunctionNode& condCompDisjunctionNode) override;
    void BeginVisit(Cm::Ast::CondCompNotNode& condCompNotNode) override;
    void EndVisit(Cm::Ast::CondCompNotNode& condCompNotNode) override;
    void Visit(Cm::Ast::CondCompPrimaryNode& condCompPrimaryNode) override;
    void BeginVisit(Cm::Ast::CondCompilationPartNode& condCompilationPartNode) override;
    void EndVisit(Cm::Ast::CondCompilationPartNode& condCompilationPartNode) override;
    void BeginVisit(Cm::Ast::CondCompStatementNode& condCompStatementNode) override;
    void EndVisit(Cm::Ast::CondCompStatementNode& condCompStatementNode) override;
private:
    Cm::BoundTree::BoundCompileUnit& boundCompileUnit;
    Cm::Sym::ContainerScope* currentContainerScope;
    std::stack<Cm::Sym::ContainerScope*> containerScopeStack;
    std::unique_ptr<Cm::Sym::FileScope> currentFileScope;
    int parameterIndex;
    std::unique_ptr<Cm::BoundTree::BoundFunction> boundFunction;
    std::unique_ptr<Cm::BoundTree::BoundParentStatement> currentParent;
    std::stack<Cm::BoundTree::BoundParentStatement*> parentStack;
    void BeginContainerScope(Cm::Sym::ContainerScope* containerScope);
    void EndContainerScope();
};

} } // namespace Cm::Bind

#endif // CM_BIND_BINDING_VISITOR_INCLUDED
