/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Prebinder.hpp>
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
#include <Cm.Bind/Access.hpp>
#include <Cm.BoundTree/BoundClass.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Bind {

Prebinder::Prebinder(Cm::Sym::SymbolTable& symbolTable_) : Cm::Ast::Visitor(false, false), symbolTable(symbolTable_), currentContainerScope(nullptr), parameterIndex(0), currentClass(nullptr),
    dontCompleteFunctions(false)
{
}

void Prebinder::BeginCompileUnit()
{
    fileScopes.push_back(std::unique_ptr<Cm::Sym::FileScope>(new Cm::Sym::FileScope()));
    usingNodes.clear();
}

void Prebinder::EndCompileUnit()
{
    fileScope = std::move(fileScopes.front());
    fileScopes.pop_back();
}

void Prebinder::BeginContainerScope(Cm::Sym::ContainerScope* containerScope)
{
    containerScopeStack.push(currentContainerScope);
    currentContainerScope = containerScope;
}

void Prebinder::EndContainerScope()
{
    currentContainerScope = containerScopeStack.top();
    containerScopeStack.pop();
}

void Prebinder::BeginVisit(Cm::Ast::CompileUnitNode& compileUnitNode)
{
    BeginCompileUnit();
}

void Prebinder::EndVisit(Cm::Ast::CompileUnitNode& compileUnitNode)
{
    EndCompileUnit();
}

void Prebinder::BeginVisit(Cm::Ast::NamespaceNode& namespaceNode)
{
    Cm::Sym::ContainerScope* containerScope = symbolTable.GetContainerScope(&namespaceNode);
    Cm::Sym::ContainerSymbol* container = containerScope->Container();
    BeginContainerScope(containerScope);
}

void Prebinder::EndVisit(Cm::Ast::NamespaceNode& namespaceNode)
{
    EndContainerScope();
}

void Prebinder::Visit(Cm::Ast::AliasNode& aliasNode)
{
    fileScopes.front()->InstallAlias(currentContainerScope, &aliasNode);
    usingNodes.push_back(&aliasNode);
}

void Prebinder::Visit(Cm::Ast::NamespaceImportNode& namespaceImportNode)
{
    fileScopes.front()->InstallNamespaceImport(currentContainerScope, &namespaceImportNode);
    usingNodes.push_back(&namespaceImportNode);
}

void Prebinder::BeginVisit(Cm::Ast::ClassNode& classNode)
{
    Cm::Sym::ClassTypeSymbol* classTypeSymbol = BindClass(symbolTable, currentContainerScope, fileScopes, &classNode);
    if (classNode.TemplateParameters().Count() > 0)
    {
        classTypeSymbol->SetUsingNodes(usingNodes);
    }
    currentClass = classTypeSymbol;
    Cm::Sym::ContainerScope* containerScope = symbolTable.GetContainerScope(&classNode);
    BeginContainerScope(containerScope);
}

void Prebinder::EndVisit(Cm::Ast::ClassNode& classNode)
{
    EndContainerScope();
    currentClass = nullptr;
}

void Prebinder::BeginVisit(Cm::Ast::ConstructorNode& constructorNode)
{
    currentFunction = BindFunction(symbolTable, currentContainerScope, fileScopes, &constructorNode, currentClass);
    BeginContainerScope(symbolTable.GetContainerScope(&constructorNode));
    parameterIndex = 0;
}

void Prebinder::EndVisit(Cm::Ast::ConstructorNode& constructorNode)
{
    if (!dontCompleteFunctions)
    {
        CompleteBindFunction(symbolTable, currentContainerScope, fileScopes, &constructorNode, currentFunction, currentClass);
    }
    EndContainerScope();
}

void Prebinder::BeginVisit(Cm::Ast::DestructorNode& destructorNode)
{
    currentFunction = BindFunction(symbolTable, currentContainerScope, fileScopes, &destructorNode, currentClass);
    BeginContainerScope(symbolTable.GetContainerScope(&destructorNode));
    parameterIndex = 0;
}

void Prebinder::EndVisit(Cm::Ast::DestructorNode& destructorNode)
{
    if (!dontCompleteFunctions)
    {
        CompleteBindFunction(symbolTable, currentContainerScope, fileScopes, &destructorNode, currentFunction, currentClass);
    }
    EndContainerScope();
}

void Prebinder::BeginVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode)
{
    currentFunction = BindFunction(symbolTable, currentContainerScope, fileScopes, &memberFunctionNode, currentClass);
    BeginContainerScope(symbolTable.GetContainerScope(&memberFunctionNode));
    parameterIndex = 0;
}

void Prebinder::EndVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode)
{
    if (!dontCompleteFunctions)
    {
        CompleteBindFunction(symbolTable, currentContainerScope, fileScopes, &memberFunctionNode, currentFunction, currentClass);
    }
    EndContainerScope();
}

void Prebinder::BeginVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode)
{
    currentFunction = BindFunction(symbolTable, currentContainerScope, fileScopes, &conversionFunctionNode, currentClass);
    BeginContainerScope(symbolTable.GetContainerScope(&conversionFunctionNode));
    parameterIndex = 0;
}

void Prebinder::EndVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode)
{
    if (!dontCompleteFunctions)
    {
        CompleteBindFunction(symbolTable, currentContainerScope, fileScopes, &conversionFunctionNode, currentFunction, currentClass);
    }
    EndContainerScope();
}

void Prebinder::BeginVisit(Cm::Ast::StaticConstructorNode& staticConstructorNode)
{
    currentFunction = BindFunction(symbolTable, currentContainerScope, fileScopes, &staticConstructorNode, currentClass);
    BeginContainerScope(symbolTable.GetContainerScope(&staticConstructorNode));
    parameterIndex = 0;
}

void Prebinder::EndVisit(Cm::Ast::StaticConstructorNode& staticConstructorNode)
{
    if (!dontCompleteFunctions)
    {
        CompleteBindFunction(symbolTable, currentContainerScope, fileScopes, &staticConstructorNode, currentFunction, currentClass);
    }
    EndContainerScope();
}

void Prebinder::Visit(Cm::Ast::MemberVariableNode& memberVariableNode)
{
    BindMemberVariable(symbolTable, currentContainerScope, fileScopes, &memberVariableNode);
}

void Prebinder::BeginVisit(Cm::Ast::EnumTypeNode& enumTypeNode)
{
    BindEnumType(symbolTable, currentContainerScope, fileScopes, &enumTypeNode);
    BeginContainerScope(symbolTable.GetContainerScope(&enumTypeNode));
}

void Prebinder::Visit(Cm::Ast::TypedefNode& typedefNode)
{
    BindTypedef(symbolTable, currentContainerScope, fileScopes, &typedefNode);
}

void Prebinder::Visit(Cm::Ast::EnumConstantNode& enumConstantNode)
{
    BindEnumConstant(symbolTable, currentContainerScope, fileScopes, &enumConstantNode);
}

void Prebinder::EndVisit(Cm::Ast::EnumTypeNode& enumTypeNode)
{
    EndContainerScope();
}

void Prebinder::Visit(Cm::Ast::ConstantNode& constantNode)
{
    BindConstant(symbolTable, currentContainerScope, fileScopes, &constantNode);
}

void Prebinder::Visit(Cm::Ast::ParameterNode& parameterNode)
{
    if (currentClass && currentClass->IsClassTemplateSymbol())
    {
        return;
    }
    BindParameter(symbolTable, currentContainerScope, fileScopes, &parameterNode, parameterIndex);
    ++parameterIndex;
}

void Prebinder::BeginVisit(Cm::Ast::FunctionNode& functionNode)
{
    if (functionNode.TemplateParameters().Count() > 0)
    {
        PushSkipContent();
        Cm::Sym::FunctionSymbol* templateFunction = BindFunction(symbolTable, currentContainerScope, fileScopes, &functionNode, nullptr);
        templateFunction->SetUsingNodes(usingNodes);
    }
    else
    {
        currentFunction = BindFunction(symbolTable, currentContainerScope, fileScopes, &functionNode, nullptr);
        BeginContainerScope(symbolTable.GetContainerScope(&functionNode));
        parameterIndex = 0;
    }
}

void Prebinder::EndVisit(Cm::Ast::FunctionNode& functionNode)
{
    if (functionNode.TemplateParameters().Count() > 0)
    {
        PopSkipContent();
    }
    else
    {
        if (!dontCompleteFunctions)
        {
            CompleteBindFunction(symbolTable, currentContainerScope, fileScopes, &functionNode, currentFunction, currentClass);
        }
        EndContainerScope();
    }
}

void Prebinder::BeginVisit(Cm::Ast::DelegateNode& delegateNode)
{
    BindDelegate(symbolTable, currentContainerScope, fileScopes, &delegateNode);
    BeginContainerScope(symbolTable.GetContainerScope(&delegateNode));
    parameterIndex = 0;
}

void Prebinder::EndVisit(Cm::Ast::DelegateNode& delegateNode)
{
    EndContainerScope();
}

void Prebinder::BeginVisit(Cm::Ast::ClassDelegateNode& classDelegateNode)
{
    BindClassDelegate(symbolTable, currentContainerScope, fileScopes, &classDelegateNode);
    BeginContainerScope(symbolTable.GetContainerScope(&classDelegateNode));
    parameterIndex = 0;
}

void Prebinder::EndVisit(Cm::Ast::ClassDelegateNode& classDelegateNode)
{
    EndContainerScope();
}

} } // namespace Cm::Bind
