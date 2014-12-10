/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/BindingVisitor.hpp>
#include <Cm.Bind/Constant.hpp>
#include <Cm.Bind/Enumeration.hpp>

namespace Cm { namespace Bind {

BindingVisitor::BindingVisitor(Cm::Sym::SymbolTable& symbolTable_) : Cm::Ast::Visitor(true), symbolTable(symbolTable_), currentContainerScope(nullptr), currentFileScope(nullptr), currentStatement(nullptr)
{
}

void BindingVisitor::BeginContainerScope(Cm::Sym::ContainerScope* containerScope)
{
    containerScopeStack.push(currentContainerScope);
    currentContainerScope = containerScope;
}

void BindingVisitor::EndContainerScope()
{
    currentContainerScope = containerScopeStack.top();
    containerScopeStack.pop();
}

void BindingVisitor::BeginStatement(Cm::Ast::StatementNode* statement)
{
    statementStack.push(currentStatement);
    currentStatement = statement;
}

void BindingVisitor::EndStatement()
{
    currentStatement = statementStack.top();
    statementStack.pop();
}

void BindingVisitor::BeginVisit(Cm::Ast::CompileUnitNode& compileUnitNode)
{
    currentFileScope.reset(new Cm::Sym::FileScope());
}

void BindingVisitor::EndVisit(Cm::Ast::CompileUnitNode& compileUnitNode)
{
    currentFileScope.reset();
}

void BindingVisitor::BeginVisit(Cm::Ast::NamespaceNode& namespaceNode)
{
    BeginContainerScope(symbolTable.GetContainerScope(&namespaceNode));
}

void BindingVisitor::EndVisit(Cm::Ast::NamespaceNode& namespaceNode)
{
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::ClassNode& classNode)
{
    BeginContainerScope(symbolTable.GetContainerScope(&classNode));
}

void BindingVisitor::EndVisit(Cm::Ast::ClassNode& classNode)
{
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::ConstructorNode& constructorNode)
{
    BeginContainerScope(symbolTable.GetContainerScope(&constructorNode));
}

void BindingVisitor::EndVisit(Cm::Ast::ConstructorNode& constructorNode)
{
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::DestructorNode& destructorNode)
{
    BeginContainerScope(symbolTable.GetContainerScope(&destructorNode));
}

void BindingVisitor::EndVisit(Cm::Ast::DestructorNode& destructorNode)
{
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode)
{
    BeginContainerScope(symbolTable.GetContainerScope(&memberFunctionNode));
}

void BindingVisitor::EndVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode)
{
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode)
{
    BeginContainerScope(symbolTable.GetContainerScope(&conversionFunctionNode));
}

void BindingVisitor::EndVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode)
{
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::EnumTypeNode& enumTypeNode)
{
    BindEnumType(symbolTable, currentContainerScope, currentFileScope.get(), &enumTypeNode);
    BeginContainerScope(symbolTable.GetContainerScope(&enumTypeNode));
}

void BindingVisitor::Visit(Cm::Ast::EnumConstantNode& enumConstantNode)
{
    BindEnumConstant(symbolTable, currentContainerScope, currentFileScope.get(), &enumConstantNode);
}

void BindingVisitor::EndVisit(Cm::Ast::EnumTypeNode& enumTypeNode)
{
    EndContainerScope();
}

void BindingVisitor::Visit(Cm::Ast::ConstantNode& constantNode)
{
    BindConstant(symbolTable, currentContainerScope, currentFileScope.get(), &constantNode);
}

void BindingVisitor::BeginVisit(Cm::Ast::FunctionNode& functionNode)
{
    BeginContainerScope(symbolTable.GetContainerScope(&functionNode));
}

void BindingVisitor::EndVisit(Cm::Ast::FunctionNode& functionNode)
{
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::DelegateNode& delegateNode)
{
    BeginContainerScope(symbolTable.GetContainerScope(&delegateNode));
}

void BindingVisitor::EndVisit(Cm::Ast::DelegateNode& delegateNode)
{
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::ClassDelegateNode& classDelegateNode)
{
    BeginContainerScope(symbolTable.GetContainerScope(&classDelegateNode));
}

void BindingVisitor::EndVisit(Cm::Ast::ClassDelegateNode& classDelegateNode)
{
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode)
{
    BeginContainerScope(symbolTable.GetContainerScope(&compoundStatementNode));
    BeginStatement(&compoundStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode)
{
    EndStatement();
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode)
{
    BeginContainerScope(symbolTable.GetContainerScope(&rangeForStatementNode));
    BeginStatement(&rangeForStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode)
{
    EndStatement();
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::ForStatementNode& forStatementNode)
{
    BeginContainerScope(symbolTable.GetContainerScope(&forStatementNode));
    BeginStatement(&forStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::ForStatementNode& forStatementNode)
{
    EndStatement();
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::ReturnStatementNode& returnStatementNode)
{
    BeginStatement(&returnStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::ReturnStatementNode& returnStatementNode)
{
    EndStatement();
}

void BindingVisitor::BeginVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode)
{
    BeginStatement(&conditionalStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode)
{
    EndStatement();
}

void BindingVisitor::BeginVisit(Cm::Ast::SwitchStatementNode& switchStatementNode)
{
    BeginStatement(&switchStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::SwitchStatementNode& switchStatementNode)
{
    EndStatement();
}

void BindingVisitor::BeginVisit(Cm::Ast::CaseStatementNode& caseStatementNode)
{
    BeginStatement(&caseStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::CaseStatementNode& caseStatementNode)
{
    EndStatement();
}

void BindingVisitor::BeginVisit(Cm::Ast::DefaultStatementNode& defaultStatementNode)
{
    BeginStatement(&defaultStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::DefaultStatementNode& defaultStatementNode)
{
    EndStatement();
}

void BindingVisitor::BeginVisit(Cm::Ast::GotoCaseStatementNode& gotoCaseStatementNode)
{
    BeginStatement(&gotoCaseStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::GotoCaseStatementNode& gotoCaseStatementNode)
{
    EndStatement();
}

void BindingVisitor::Visit(Cm::Ast::GotoDefaultStatementNode& gotoDefaultStatementNode)
{
    BeginStatement(&gotoDefaultStatementNode);
    EndStatement();
}

void BindingVisitor::BeginVisit(Cm::Ast::WhileStatementNode& whileStatementNode)
{
    BeginStatement(&whileStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::WhileStatementNode& whileStatementNode)
{
    EndStatement();
}

void BindingVisitor::BeginVisit(Cm::Ast::DoStatementNode& doStatementNode)
{
    BeginStatement(&doStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::DoStatementNode& doStatementNode)
{
    EndStatement();
}

void BindingVisitor::Visit(Cm::Ast::BreakStatementNode& breakStatementNode) 
{
    BeginStatement(&breakStatementNode);
}

void BindingVisitor::Visit(Cm::Ast::ContinueStatementNode& continueStatementNode)
{
    EndStatement();
}

void BindingVisitor::Visit(Cm::Ast::GotoStatementNode& gotoStatementNode)
{
    BeginStatement(&gotoStatementNode);
    EndStatement();
}

void BindingVisitor::Visit(Cm::Ast::TypedefStatementNode& typedefStatementNode)
{
    BeginStatement(&typedefStatementNode);
    EndStatement();
}

void BindingVisitor::BeginVisit(Cm::Ast::SimpleStatementNode& simpleStatementNode)
{
    BeginStatement(&simpleStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::SimpleStatementNode& simpleStatementNode)
{
    EndStatement();
}

void BindingVisitor::BeginVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode)
{
    BeginStatement(&assignmentStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode)
{
    EndStatement();
}

void BindingVisitor::BeginVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
    BeginStatement(&constructionStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
    EndStatement();
}

void BindingVisitor::BeginVisit(Cm::Ast::DeleteStatementNode& deleteStatementNode)
{
    BeginStatement(&deleteStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::DeleteStatementNode& deleteStatementNode)
{
    EndStatement();
}

void BindingVisitor::BeginVisit(Cm::Ast::DestroyStatementNode& destroyStatementNode)
{
    BeginStatement(&destroyStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::DestroyStatementNode& destroyStatementNode)
{
    EndStatement();
}

void BindingVisitor::BeginVisit(Cm::Ast::ThrowStatementNode& throwStatementNode)
{
    BeginStatement(&throwStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::ThrowStatementNode& throwStatementNode)
{
    EndStatement();
}

void BindingVisitor::BeginVisit(Cm::Ast::TryStatementNode& tryStatementNode)
{
    BeginStatement(&tryStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::TryStatementNode& tryStatementNode)
{
    EndStatement();
}

void BindingVisitor::BeginVisit(Cm::Ast::CatchNode& catchNode)
{
    
}

void BindingVisitor::EndVisit(Cm::Ast::CatchNode& catchNode)
{

}

void BindingVisitor::BeginVisit(Cm::Ast::AssertStatementNode& assertStatementNode)
{
    BeginStatement(&assertStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::AssertStatementNode& assertStatementNode)
{
    EndStatement();
}

void BindingVisitor::BeginVisit(Cm::Ast::CondCompDisjunctionNode& condCompDisjunctionNode)
{

}

void BindingVisitor::EndVisit(Cm::Ast::CondCompDisjunctionNode& condCompDisjunctionNode)
{

}

void BindingVisitor::BeginVisit(Cm::Ast::CondCompConjunctionNode& condCompDisjunctionNode)
{

}

void BindingVisitor::EndVisit(Cm::Ast::CondCompConjunctionNode& condCompDisjunctionNode)
{

}

void BindingVisitor::BeginVisit(Cm::Ast::CondCompNotNode& condCompNotNode)
{

}

void BindingVisitor::EndVisit(Cm::Ast::CondCompNotNode& condCompNotNode)
{

}

void BindingVisitor::Visit(Cm::Ast::CondCompPrimaryNode& condCompPrimaryNode)
{

}

void BindingVisitor::BeginVisit(Cm::Ast::CondCompilationPartNode& condCompilationPartNode)
{

}

void BindingVisitor::EndVisit(Cm::Ast::CondCompilationPartNode& condCompilationPartNode)
{

}

void BindingVisitor::BeginVisit(Cm::Ast::CondCompStatementNode& condCompStatementNode)
{
    BeginStatement(&condCompStatementNode);
}

void BindingVisitor::EndVisit(Cm::Ast::CondCompStatementNode& condCompStatementNode)
{
    EndStatement();
}

void BindingVisitor::Visit(Cm::Ast::AliasNode& aliasNode)
{
    if (currentFileScope)
    {
        currentFileScope->InstallAlias(currentContainerScope, &aliasNode);
    }
    else
    {
        throw std::runtime_error("current file scope not set");
    }
}

void BindingVisitor::Visit(Cm::Ast::NamespaceImportNode& namespaceImportNode)
{
    if (currentFileScope)
    {
        currentFileScope->InstallNamespaceImport(currentContainerScope, &namespaceImportNode);
    }
    else
    {
        throw std::runtime_error("current file scope not set");
    }
}

} } // namespace Cm::Bind
