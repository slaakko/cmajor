/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Scope.hpp>
#include <Cm.Sym/Exception.hpp>
#include <Cm.Sym/NamespaceSymbol.hpp>
#include <Cm.Util/TextUtils.hpp>

namespace Cm { namespace Sym {

Scope::Scope() : base(nullptr), parent(nullptr), global(nullptr), ns(nullptr)
{
}

void Scope::Install(Symbol* symbol)
{
    SymbolMapIt i = symbolMap.find(symbol->Name());
    if (i != symbolMap.end())
    {
        Cm::Ast::Node* defNode = symbol->GetNode();
        Symbol* prev = i->second;
        Cm::Ast::Node* refNode = prev->GetNode();
        throw Exception("symbol '" + symbol->Name() + "' already defined", defNode, refNode);
    }
    else
    {
        symbolMap[symbol->Name()] = symbol;
    }
}

Symbol* Scope::Lookup(const std::string& name) const
{
    return Lookup(name, ScopeLookup::this_);
}

Symbol* Scope::Lookup(const std::string& name, ScopeLookup lookup) const
{
    std::string::size_type dotPos = name.find('.');
    if (dotPos != std::string::npos)
    {
        if (global)
        {
            const Scope* scope = global;
            Symbol* s = nullptr;
            std::vector<std::string> components = Cm::Util::Split(name, '.');
            for (const std::string& component : components)
            {
                if (scope)
                {
                    s = scope->Lookup(component, ScopeLookup::this_);
                    if (s)
                    {
                        scope = s->GetScope();
                    }
                }
                else
                {
                    return nullptr;
                }
            }
            return s;
        }
        else
        {
            return nullptr;
        }
    }
    else
    {
        SymbolMapIt i = symbolMap.find(name);
        if (i != symbolMap.end())
        {
            return i->second;
        }
        if ((lookup & ScopeLookup::base) != ScopeLookup::none)
        {
            if (base)
            {
                Symbol* s = base->Lookup(name, lookup);
                if (s)
                {
                    return s;
                }
            }
        }
        if ((lookup & ScopeLookup::parent) != ScopeLookup::none)
        {
            if (parent)
            {
                Symbol* s = parent->Lookup(name, lookup);
                if (s)
                {
                    return s;
                }
            }
        }
        return nullptr;
    }
}

NamespaceSymbol* Scope::CreateNamespace(const std::string& qualifiedNsName, Cm::Ast::Node* node)
{
    Scope* scope = this;
    NamespaceSymbol* parentNs = scope->Ns();
    std::vector<std::string> components = Cm::Util::Split(qualifiedNsName, '.');
    for (const std::string& component : components)
    {
        Symbol* s = scope->Lookup(component);
        if (s)
        {
            if (s->IsNamespaceSymbol())
            {
                scope = s->GetScope();
                parentNs = scope->Ns();
            }
            else
            {
                throw Exception("symbol '" + s->Name() + "' does not denote a namespace", s->GetNode(), nullptr);
            }
        }
        else
        {
            NamespaceSymbol* newNs = new NamespaceSymbol(component);
            newNs->SetNode(node);
            scope = newNs->GetScope();
            scope->SetParent(parentNs->GetScope());
            parentNs->AddSymbol(newNs);
            if (parentNs->IsGlobalNamespace())
            {
                scope->SetGlobal(parentNs->GetScope());
            }
            else
            {
                scope->SetGlobal(parentNs->GetScope()->Global());
            }
            parentNs = newNs;
        }
    }
    return parentNs;
}

} } // namespace Cm::Sym
