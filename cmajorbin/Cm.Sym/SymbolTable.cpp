/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/Exception.hpp>
#include <Cm.Sym/ClassSymbol.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/DelegateSymbol.hpp>
#include <Cm.Sym/ConstantSymbol.hpp>
#include <Cm.Sym/ParameterSymbol.hpp>
#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Sym/DeclarationBlock.hpp>
#include <Cm.Sym/LocalVariableSymbol.hpp>
#include <Cm.Sym/TemplateParameterSymbol.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Ast/Namespace.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

SymbolTable::SymbolTable() : globalNs(), container(&globalNs)
{
}

void SymbolTable::BeginContainer(ContainerSymbol* container_)
{
    containerStack.push(container);
    container = container_;
}

void SymbolTable::EndContainer()
{
    container = containerStack.top();
    containerStack.pop();
}

void SymbolTable::BeginNamespaceScope(Cm::Ast::NamespaceNode* namespaceNode)
{
    if (namespaceNode->IsGlobalNamespaceNode())
    {
        if (!globalNs.GetNode())
        {
            globalNs.SetNode(namespaceNode);
        }
        BeginContainer(&globalNs);
        nodeScopeMap[namespaceNode] = globalNs.GetContainerScope();
    }
    else
    {
        Symbol* symbol = container->GetContainerScope()->Lookup(namespaceNode->Id()->Str());
        if (symbol)
        {
            if (symbol->IsNamespaceSymbol())
            {
                BeginContainer(static_cast<ContainerSymbol*>(symbol));
                nodeScopeMap[namespaceNode] = symbol->GetContainerScope();
            }
            else
            {
                throw Exception("symbol '" + symbol->Name() + "' does not denote a namespace", symbol->GetNode(), nullptr);
            }
        }
        else
        {
            NamespaceSymbol* namespaceSymbol = container->GetContainerScope()->CreateNamespace(namespaceNode->Id()->Str(), namespaceNode);
            BeginContainer(namespaceSymbol);
            nodeScopeMap[namespaceNode] = namespaceSymbol->GetContainerScope();
        }
    }
}

void SymbolTable::EndNamespaceScope()
{
    EndContainer();
}

void SymbolTable::BeginClassScope(Cm::Ast::ClassNode* classNode)
{
    ClassSymbol* classSymbol = new ClassSymbol(classNode);
    ContainerScope* classScope = classSymbol->GetContainerScope();
    nodeScopeMap[classNode] = classScope;
    ContainerScope* containerScope = container->GetContainerScope();
    classScope->SetParent(containerScope);
    container->AddSymbol(classSymbol);
    BeginContainer(classSymbol);
}

void SymbolTable::EndClassScope()
{
    EndContainer();
}

void SymbolTable::BeginEnumScope(Cm::Ast::EnumTypeNode* enumTypeNode)
{
    EnumTypeSymbol* enumTypeSymbol = new EnumTypeSymbol(enumTypeNode);
    ContainerScope* enumScope = enumTypeSymbol->GetContainerScope();
    nodeScopeMap[enumTypeNode] = enumScope;
    ContainerScope* containerScope = container->GetContainerScope();
    enumScope->SetParent(containerScope);
    container->AddSymbol(enumTypeSymbol);
    BeginContainer(enumTypeSymbol);
}

void SymbolTable::EndEnumScope()
{
    EndContainer();
}

void SymbolTable::AddEnumConstant(Cm::Ast::EnumConstantNode* enumConstantNode)
{
    container->AddSymbol(new EnumConstantSymbol(enumConstantNode));
}

void SymbolTable::BeginFunctionScope(Cm::Ast::FunctionNode* functionNode)
{
    FunctionSymbol* functionSymbol = new FunctionSymbol(functionNode);
    ContainerScope* functionScope = functionSymbol->GetContainerScope();
    nodeScopeMap[functionNode] = functionScope;
    ContainerScope* containerScope = container->GetContainerScope();
    functionScope->SetParent(containerScope);
    container->AddFunctionSymbol(functionSymbol);
    BeginContainer(functionSymbol);
}

void SymbolTable::EndFunctionScope()
{
    EndContainer();
}

void SymbolTable::BeginDelegateScope(Cm::Ast::DelegateNode* delegateNode)
{
    DelegateSymbol* delegateSymbol = new DelegateSymbol(delegateNode);
    ContainerScope* delegateScope = delegateSymbol->GetContainerScope();
    nodeScopeMap[delegateNode] = delegateScope;
    ContainerScope* containerScope = container->GetContainerScope();
    delegateScope->SetParent(containerScope);
    container->AddSymbol(delegateSymbol);
    BeginContainer(delegateSymbol);
}

void SymbolTable::EndDelegateScope()
{
    EndContainer();
}

void SymbolTable::BeginClassDelegateScope(Cm::Ast::ClassDelegateNode* classDelegateNode)
{
    ClassDelegateSymbol* classDelegateSymbol = new ClassDelegateSymbol(classDelegateNode);
    ContainerScope* classDelegateScope = classDelegateSymbol->GetContainerScope();
    nodeScopeMap[classDelegateNode] = classDelegateScope;
    ContainerScope* containerScope = container->GetContainerScope();
    classDelegateScope->SetParent(containerScope);
    container->AddSymbol(classDelegateSymbol);
    BeginContainer(classDelegateSymbol);
}

void SymbolTable::EndClassDelegateScope()
{
    EndContainer();
}

void SymbolTable::AddConstant(Cm::Ast::ConstantNode* constantNode)
{
    container->AddSymbol(new ConstantSymbol(constantNode));
}

void SymbolTable::AddParameter(Cm::Ast::ParameterNode* parameterNode, const std::string& parameterName)
{
    container->AddSymbol(new ParameterSymbol(parameterNode, parameterName));
}

void SymbolTable::AddTemplateParameter(Cm::Ast::TemplateParameterNode* templateParameterNode)
{
    container->AddSymbol(new TemplateParameterSymbol(templateParameterNode));
}

void SymbolTable::BeginDeclarationScope(Cm::Ast::StatementNode* statementNode)
{
    DeclarationBlock* declarationBlock = new DeclarationBlock(statementNode);
    ContainerScope* declarationBlockScope = declarationBlock->GetContainerScope();
    nodeScopeMap[statementNode] = declarationBlockScope;
    ContainerScope* containerScope = container->GetContainerScope();
    declarationBlockScope->SetParent(containerScope);
    container->AddSymbol(declarationBlock);
    BeginContainer(declarationBlock);
}

void SymbolTable::EndDeclarationcope()
{
    EndContainer();
}

void SymbolTable::AddLocalVariable(Cm::Ast::ConstructionStatementNode* constructionStatementNode)
{
    container->AddSymbol(new LocalVariableSymbol(constructionStatementNode));
}

void SymbolTable::AddMemberVariable(Cm::Ast::MemberVariableNode* memberVariableNode)
{
    container->AddSymbol(new MemberVariableSymbol(memberVariableNode));
}

ContainerScope* SymbolTable::GetContainerScope(Cm::Ast::Node* node) const
{
    NodeScopeMapIt i = nodeScopeMap.find(node);
    if (i != nodeScopeMap.end())
    {
        return i->second;
    }
    else
    {
        throw std::runtime_error("container scope not found");
    }
}

} } // namespace Cm::Sym
