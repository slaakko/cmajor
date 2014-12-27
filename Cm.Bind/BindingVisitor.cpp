/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/BindingVisitor.hpp>
#include <Cm.Bind/Class.hpp>
#include <Cm.Bind/Function.hpp>
#include <Cm.Bind/Constant.hpp>
#include <Cm.Bind/Enumeration.hpp>
#include <Cm.Bind/Parameter.hpp>
#include <Cm.Bind/Typedef.hpp>
#include <Cm.Bind/MemberVariable.hpp>
#include <Cm.Bind/LocalVariable.hpp>
#include <Cm.Bind/StatementBinder.hpp>
#include <Cm.BoundTree/BoundClass.hpp>

namespace Cm { namespace Bind {

BindingVisitor::BindingVisitor(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_) : Cm::Ast::Visitor(false), boundCompileUnit(boundCompileUnit_), 
    currentContainerScope(nullptr), currentFileScope(nullptr), parameterIndex(0), currentBlock(nullptr)
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

void BindingVisitor::BeginVisit(Cm::Ast::CompileUnitNode& compileUnitNode)
{
    currentFileScope.reset(new Cm::Sym::FileScope());
}

void BindingVisitor::EndVisit(Cm::Ast::CompileUnitNode& compileUnitNode)
{
    boundCompileUnit.SymbolTable().GetTypeRepository().ClearInternalTypes();
    currentFileScope.reset();
}

void BindingVisitor::BeginVisit(Cm::Ast::NamespaceNode& namespaceNode)
{
    Cm::Sym::ContainerScope* containerScope = boundCompileUnit.SymbolTable().GetContainerScope(&namespaceNode);
    Cm::Sym::ContainerSymbol* container = containerScope->Container();
    BeginContainerScope(containerScope);
}

void BindingVisitor::EndVisit(Cm::Ast::NamespaceNode& namespaceNode)
{
    EndContainerScope();
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

void BindingVisitor::BeginVisit(Cm::Ast::ClassNode& classNode)
{
    if (classNode.TemplateParameters().Count() > 0)
    {
        PushSkipContent();
    }
    else
    {
        Cm::Sym::ClassTypeSymbol* classTypeSymbol = BindClass(boundCompileUnit.SymbolTable(), currentContainerScope, currentFileScope.get(), &classNode);
        boundCompileUnit.AddBoundNode(new Cm::BoundTree::BoundClass(classTypeSymbol, &classNode));
        Cm::Sym::ContainerScope* containerScope = boundCompileUnit.SymbolTable().GetContainerScope(&classNode);
        BeginContainerScope(containerScope);
    }
}

void BindingVisitor::EndVisit(Cm::Ast::ClassNode& classNode)
{
    if (classNode.TemplateParameters().Count() > 0)
    {
        PopSkipContent();
    }
    else
    {
        EndContainerScope();

    }
}

void BindingVisitor::BeginVisit(Cm::Ast::ConstructorNode& constructorNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&constructorNode));
    parameterIndex = 0;
}

void BindingVisitor::EndVisit(Cm::Ast::ConstructorNode& constructorNode)
{
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::DestructorNode& destructorNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&destructorNode));
    parameterIndex = 0;
}

void BindingVisitor::EndVisit(Cm::Ast::DestructorNode& destructorNode)
{
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&memberFunctionNode));
    parameterIndex = 0;
}

void BindingVisitor::EndVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode)
{
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&conversionFunctionNode));
    parameterIndex = 0;
}

void BindingVisitor::EndVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode)
{
    EndContainerScope();
}

void BindingVisitor::Visit(Cm::Ast::MemberVariableNode& memberVariableNode)
{
    BindMemberVariable(boundCompileUnit.SymbolTable(), currentContainerScope, currentFileScope.get(), &memberVariableNode);
}

void BindingVisitor::BeginVisit(Cm::Ast::EnumTypeNode& enumTypeNode)
{
    BindEnumType(boundCompileUnit.SymbolTable(), currentContainerScope, currentFileScope.get(), &enumTypeNode);
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&enumTypeNode));
}

void BindingVisitor::Visit(Cm::Ast::TypedefNode& typedefNode)
{
    BindTypedef(boundCompileUnit.SymbolTable(), currentContainerScope, currentFileScope.get(), &typedefNode);
}

void BindingVisitor::Visit(Cm::Ast::EnumConstantNode& enumConstantNode)
{
    BindEnumConstant(boundCompileUnit.SymbolTable(), currentContainerScope, currentFileScope.get(), &enumConstantNode);
}

void BindingVisitor::EndVisit(Cm::Ast::EnumTypeNode& enumTypeNode)
{
    EndContainerScope();
}

void BindingVisitor::Visit(Cm::Ast::ConstantNode& constantNode)
{
    BindConstant(boundCompileUnit.SymbolTable(), currentContainerScope, currentFileScope.get(), &constantNode);
}

void BindingVisitor::Visit(Cm::Ast::ParameterNode& parameterNode)
{
    BindParameter(boundCompileUnit.SymbolTable(), currentContainerScope, currentFileScope.get(), &parameterNode, parameterIndex);
    ++parameterIndex;
}

void BindingVisitor::BeginVisit(Cm::Ast::FunctionNode& functionNode)
{
    if (functionNode.TemplateParameters().Count() > 0)
    {
        PushSkipContent();
    }
    else
    {
        Cm::Sym::FunctionSymbol* functionSymbol = BindFunction(boundCompileUnit.SymbolTable(), currentContainerScope, currentFileScope.get(), &functionNode);
        BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&functionNode));
        parameterIndex = 0;
        boundFunction.reset(new Cm::BoundTree::BoundFunction(&functionNode, functionSymbol));
    }
}

void BindingVisitor::EndVisit(Cm::Ast::FunctionNode& functionNode)
{
    if (functionNode.TemplateParameters().Count() > 0)
    {
        PopSkipContent();
    }
    else
    {
        EndContainerScope();
    }
    CheckFunctionAccessLevels(boundFunction->GetFunctionSymbol());
    boundCompileUnit.AddBoundNode(boundFunction.release());
}

void BindingVisitor::BeginVisit(Cm::Ast::DelegateNode& delegateNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&delegateNode));
    parameterIndex = 0;
}

void BindingVisitor::EndVisit(Cm::Ast::DelegateNode& delegateNode)
{
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::ClassDelegateNode& classDelegateNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&classDelegateNode));
    parameterIndex = 0;
}

void BindingVisitor::EndVisit(Cm::Ast::ClassDelegateNode& classDelegateNode)
{
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&compoundStatementNode));
    blockStack.push(currentBlock.release());
    currentBlock.reset(new Cm::BoundTree::BoundCompoundStatement(&compoundStatementNode));
}

void BindingVisitor::EndVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode)
{
    Cm::BoundTree::BoundCompoundStatement* parent = blockStack.top();
    blockStack.pop();
    if (parent)
    {
        parent->AddStatement(currentBlock.release());
    }
    else
    {
        boundFunction->SetBody(currentBlock.release());
    }
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&rangeForStatementNode));
}

void BindingVisitor::EndVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode)
{
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::ForStatementNode& forStatementNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&forStatementNode));
}

void BindingVisitor::EndVisit(Cm::Ast::ForStatementNode& forStatementNode)
{
    EndContainerScope();
}

void BindingVisitor::BeginVisit(Cm::Ast::ReturnStatementNode& returnStatementNode)
{
}

void BindingVisitor::EndVisit(Cm::Ast::ReturnStatementNode& returnStatementNode)
{
}

void BindingVisitor::BeginVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode)
{
}

void BindingVisitor::EndVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode)
{
}

void BindingVisitor::BeginVisit(Cm::Ast::SwitchStatementNode& switchStatementNode)
{
}

void BindingVisitor::EndVisit(Cm::Ast::SwitchStatementNode& switchStatementNode)
{
}

void BindingVisitor::BeginVisit(Cm::Ast::CaseStatementNode& caseStatementNode)
{
}

void BindingVisitor::EndVisit(Cm::Ast::CaseStatementNode& caseStatementNode)
{
}

void BindingVisitor::BeginVisit(Cm::Ast::DefaultStatementNode& defaultStatementNode)
{
}

void BindingVisitor::EndVisit(Cm::Ast::DefaultStatementNode& defaultStatementNode)
{
}

void BindingVisitor::BeginVisit(Cm::Ast::GotoCaseStatementNode& gotoCaseStatementNode)
{
}

void BindingVisitor::EndVisit(Cm::Ast::GotoCaseStatementNode& gotoCaseStatementNode)
{
}

void BindingVisitor::Visit(Cm::Ast::GotoDefaultStatementNode& gotoDefaultStatementNode)
{
}

void BindingVisitor::BeginVisit(Cm::Ast::WhileStatementNode& whileStatementNode)
{
}

void BindingVisitor::EndVisit(Cm::Ast::WhileStatementNode& whileStatementNode)
{
}

void BindingVisitor::BeginVisit(Cm::Ast::DoStatementNode& doStatementNode)
{
}

void BindingVisitor::EndVisit(Cm::Ast::DoStatementNode& doStatementNode)
{
}

void BindingVisitor::Visit(Cm::Ast::BreakStatementNode& breakStatementNode) 
{
}

void BindingVisitor::Visit(Cm::Ast::ContinueStatementNode& continueStatementNode)
{
}

void BindingVisitor::Visit(Cm::Ast::GotoStatementNode& gotoStatementNode)
{
}

void BindingVisitor::Visit(Cm::Ast::TypedefStatementNode& typedefStatementNode)
{
}

void BindingVisitor::BeginVisit(Cm::Ast::SimpleStatementNode& simpleStatementNode)
{
}

void BindingVisitor::EndVisit(Cm::Ast::SimpleStatementNode& simpleStatementNode)
{
}

void BindingVisitor::BeginVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode)
{
    AssignmentStatementBinder binder(boundCompileUnit.SymbolTable(), boundCompileUnit.ConversionTable(), boundCompileUnit.ClassConversionTable(),
        currentContainerScope, currentFileScope.get(), boundFunction.get());
    assignmentStatementNode.Accept(binder);
    currentBlock->AddStatement(binder.Result());
}

void BindingVisitor::EndVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode)
{
}

void BindingVisitor::BeginVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
    ConstructionStatementBinder binder(boundCompileUnit.SymbolTable(), boundCompileUnit.ConversionTable(), boundCompileUnit.ClassConversionTable(), 
        currentContainerScope, currentFileScope.get(), boundFunction.get());
    constructionStatementNode.Accept(binder);
    currentBlock->AddStatement(binder.Result());
}

void BindingVisitor::EndVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
}

void BindingVisitor::BeginVisit(Cm::Ast::DeleteStatementNode& deleteStatementNode)
{
}

void BindingVisitor::EndVisit(Cm::Ast::DeleteStatementNode& deleteStatementNode)
{
}

void BindingVisitor::BeginVisit(Cm::Ast::DestroyStatementNode& destroyStatementNode)
{
}

void BindingVisitor::EndVisit(Cm::Ast::DestroyStatementNode& destroyStatementNode)
{
}

void BindingVisitor::BeginVisit(Cm::Ast::ThrowStatementNode& throwStatementNode)
{
}

void BindingVisitor::EndVisit(Cm::Ast::ThrowStatementNode& throwStatementNode)
{
}

void BindingVisitor::BeginVisit(Cm::Ast::TryStatementNode& tryStatementNode)
{
}

void BindingVisitor::EndVisit(Cm::Ast::TryStatementNode& tryStatementNode)
{
}

void BindingVisitor::BeginVisit(Cm::Ast::CatchNode& catchNode)
{
    
}

void BindingVisitor::EndVisit(Cm::Ast::CatchNode& catchNode)
{

}

void BindingVisitor::BeginVisit(Cm::Ast::AssertStatementNode& assertStatementNode)
{
}

void BindingVisitor::EndVisit(Cm::Ast::AssertStatementNode& assertStatementNode)
{
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
}

void BindingVisitor::EndVisit(Cm::Ast::CondCompStatementNode& condCompStatementNode)
{
}

} } // namespace Cm::Bind
