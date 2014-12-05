/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/Exception.hpp>
#include <Cm.Sym/ClassSymbol.hpp>
#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Ast/Namespace.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

SymbolTable::SymbolTable() : globalNs(), container(&globalNs)
{
    container->GetScope()->SetNs(&globalNs);
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
    }
    else
    {
        Symbol* symbol = container->GetScope()->Lookup(namespaceNode->Id()->Str());
        if (symbol)
        {
            if (symbol->IsNamespaceSymbol())
            {
                BeginContainer(static_cast<ContainerSymbol*>(symbol));
            }
            else
            {
                throw Exception("symbol '" + symbol->Name() + "' does not denote a namespace", symbol->GetNode(), nullptr);
            }
        }
        else
        {
            BeginContainer(container->GetScope()->CreateNamespace(namespaceNode->Id()->Str(), namespaceNode));
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

} } // namespace Cm::Sym