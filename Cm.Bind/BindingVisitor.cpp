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
#include <Cm.Bind/Delegate.hpp>
#include <Cm.BoundTree/BoundClass.hpp>

namespace Cm { namespace Bind {

BindingVisitor::BindingVisitor(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_) : Cm::Ast::Visitor(false, false), boundCompileUnit(boundCompileUnit_), 
    currentContainerScope(nullptr), parameterIndex(0)
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
    fileScope.reset(new Cm::Sym::FileScope());
}

void BindingVisitor::EndVisit(Cm::Ast::CompileUnitNode& compileUnitNode)
{
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
    boundCompileUnit.GetFileScope().InstallAlias(currentContainerScope, &aliasNode);
}

void BindingVisitor::Visit(Cm::Ast::NamespaceImportNode& namespaceImportNode)
{
    boundCompileUnit.GetFileScope().InstallNamespaceImport(currentContainerScope, &namespaceImportNode);
}

void BindingVisitor::BeginVisit(Cm::Ast::ClassNode& classNode)
{
    if (classNode.TemplateParameters().Count() > 0)
    {
        PushSkipContent();
    }
    else
    {
        Cm::Sym::ClassTypeSymbol* classTypeSymbol = BindClass(boundCompileUnit.SymbolTable(), currentContainerScope, &boundCompileUnit.GetFileScope(), &classNode);
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
    BindMemberVariable(boundCompileUnit.SymbolTable(), currentContainerScope, &boundCompileUnit.GetFileScope(), &memberVariableNode);
}

void BindingVisitor::BeginVisit(Cm::Ast::EnumTypeNode& enumTypeNode)
{
    BindEnumType(boundCompileUnit.SymbolTable(), currentContainerScope, &boundCompileUnit.GetFileScope(), &enumTypeNode);
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&enumTypeNode));
}

void BindingVisitor::Visit(Cm::Ast::TypedefNode& typedefNode)
{
    BindTypedef(boundCompileUnit.SymbolTable(), currentContainerScope, &boundCompileUnit.GetFileScope(), &typedefNode);
}

void BindingVisitor::Visit(Cm::Ast::EnumConstantNode& enumConstantNode)
{
    BindEnumConstant(boundCompileUnit.SymbolTable(), currentContainerScope, &boundCompileUnit.GetFileScope(), &enumConstantNode);
}

void BindingVisitor::EndVisit(Cm::Ast::EnumTypeNode& enumTypeNode)
{
    EndContainerScope();
}

void BindingVisitor::Visit(Cm::Ast::ConstantNode& constantNode)
{
    BindConstant(boundCompileUnit.SymbolTable(), currentContainerScope, &boundCompileUnit.GetFileScope(), &constantNode);
}

void BindingVisitor::Visit(Cm::Ast::ParameterNode& parameterNode)
{
    BindParameter(boundCompileUnit.SymbolTable(), currentContainerScope, &boundCompileUnit.GetFileScope(), &parameterNode, parameterIndex);
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
        Cm::Sym::FunctionSymbol* functionSymbol = BindFunction(boundCompileUnit.SymbolTable(), currentContainerScope, &boundCompileUnit.GetFileScope(), &functionNode);
        BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&functionNode));
        parameterIndex = 0;
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
}

void BindingVisitor::BeginVisit(Cm::Ast::DelegateNode& delegateNode)
{
    BindDelegate(boundCompileUnit.SymbolTable(), currentContainerScope, &boundCompileUnit.GetFileScope(), &delegateNode);
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

} } // namespace Cm::Bind
