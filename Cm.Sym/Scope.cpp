/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Scope.hpp>
#include <Cm.Sym/Exception.hpp>
#include <Cm.Sym/NamespaceSymbol.hpp>
#include <Cm.Sym/FunctionGroupSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <unordered_set>

namespace Cm { namespace Sym {

Scope::~Scope()
{
}

ContainerScope::ContainerScope() : base(nullptr), parent(nullptr), container(nullptr)
{
}

ContainerScope::ContainerScope(ContainerScope&& that) : symbolMap(std::move(that.symbolMap)), base(that.base), parent(that.parent), container(that.container)
{
    that.base = nullptr;
    that.parent = nullptr;
    that.container = nullptr;
}

void ContainerScope::Install(Symbol* symbol)
{
    SymbolMapIt i = symbolMap.find(symbol->Name());
    if (i != symbolMap.end() && !symbol->IsTemplateTypeSymbol())
    {
        const Span& defined = symbol->GetSpan();
        Symbol* prev = i->second;
        const Span& referenced = prev->GetSpan();
        throw Exception("symbol '" + symbol->Name() + "' already defined", defined, referenced);
    }
    else
    {
        symbolMap[symbol->Name()] = symbol;
		if (symbol->IsContainerSymbol())
		{
			ContainerSymbol* containerSymbol = static_cast<ContainerSymbol*>(symbol);
			containerSymbol->GetContainerScope()->SetParent(this);
		}
    }
}

Symbol* ContainerScope::LookupQualified(const std::vector<std::string>& components, ScopeLookup lookup) const
{
    const ContainerScope* scope = this;
    Symbol* s = nullptr;
    for (const std::string& component : components)
    {
        if (scope)
        {
            s = scope->Lookup(component, ScopeLookup::this_);
            if (s)
            {
                scope = s->GetContainerScope();
            }
        }
    }
    if (!s)
    {
        if ((lookup & ScopeLookup::parent) != ScopeLookup::none)
        {
            if (parent)
            {
                return parent->LookupQualified(components, lookup);
            }
            else
            {
                return nullptr;
            }
        }
    }
    return s;
}

Symbol* ContainerScope::Lookup(const std::string& name) const
{
    return Lookup(name, ScopeLookup::this_);
}

Symbol* ContainerScope::Lookup(const std::string& name, ScopeLookup lookup) const
{
    std::string::size_type dotPos = name.find('.');
    if (dotPos != std::string::npos)
    {
        std::vector<std::string> components = Cm::Util::Split(name, '.');
        return LookupQualified(components, lookup);
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

NamespaceSymbol* ContainerScope::Ns() const
{
    return container->Ns();
}

ClassTypeSymbol* ContainerScope::Class() const
{
    return container->Class();
}

ContainerScope* ContainerScope::ClassOrNsScope() const
{
    Symbol* clasOrNsSymbol = container->ClassOrNs();
    if (clasOrNsSymbol)
    {
        return clasOrNsSymbol->GetContainerScope();
    }
    else
    {
        return nullptr;
    }
}

void ContainerScope::CollectViableFunctions(ScopeLookup lookup, const std::string& groupName, int arity, std::unordered_set<FunctionSymbol*>& viableFunctions)
{
    if ((lookup & ScopeLookup::this_) != ScopeLookup::none)
    {
        Cm::Sym::Symbol* symbol = Lookup(groupName);
        if (symbol && symbol->IsFunctionGroupSymbol())
        {
            FunctionGroupSymbol* functionGroupSymbol = static_cast<FunctionGroupSymbol*>(symbol);
            functionGroupSymbol->CollectViableFunctions(arity, viableFunctions);
        }
    }
    if ((lookup & ScopeLookup::base) != ScopeLookup::none)
    {
        if (base)
        {
            base->CollectViableFunctions(lookup, groupName, arity, viableFunctions);
        }
    }
    if ((lookup & ScopeLookup::parent) != ScopeLookup::none)
    {
        if (parent)
        {
            parent->CollectViableFunctions(lookup, groupName, arity, viableFunctions);
        }
    }
}

NamespaceSymbol* ContainerScope::CreateNamespace(const std::string& qualifiedNsName, const Span& span)
{
    ContainerScope* scope = this;
    NamespaceSymbol* parentNs = scope->Ns();
    std::vector<std::string> components = Cm::Util::Split(qualifiedNsName, '.');
    for (const std::string& component : components)
    {
        Symbol* s = scope->Lookup(component);
        if (s)
        {
            if (s->IsNamespaceSymbol())
            {
                scope = s->GetContainerScope();
                parentNs = scope->Ns();
            }
            else
            {
                throw Exception("symbol '" + s->Name() + "' does not denote a namespace", s->GetSpan());
            }
        }
        else
        {
            NamespaceSymbol* newNs = new NamespaceSymbol(span, component);
            scope = newNs->GetContainerScope();
            scope->SetParent(parentNs->GetContainerScope());
            parentNs->AddSymbol(newNs);
            parentNs = newNs;
        }
    }
    return parentNs;
}

FileScope::FileScope()
{
}

void FileScope::InstallAlias(ContainerScope* currenContainerScope, Cm::Ast::AliasNode* aliasNode)
{
    if (currenContainerScope)
    {
        Symbol* symbol = currenContainerScope->Lookup(aliasNode->Qid()->Str(), ScopeLookup::this_and_parent);
        if (symbol)
        {
            aliasSymbolMap[aliasNode->Id()->Str()] = symbol;
        }
        else
        {
            throw Exception("referred symbol '" + aliasNode->Qid()->Str() + "' not found", aliasNode->Qid()->GetSpan());
        }
    }
    else
    {
        throw std::runtime_error("current container scope is null");
    }
}

void FileScope::InstallNamespaceImport(ContainerScope* currentContainerScope, Cm::Ast::NamespaceImportNode* namespaceImportNode)
{
    if (currentContainerScope)
    {
        Symbol* symbol = currentContainerScope->Lookup(namespaceImportNode->Ns()->Str(), ScopeLookup::this_and_parent);
        if (symbol)
        {
            if (symbol->IsNamespaceSymbol())
            {
                ContainerScope* containerScope = symbol->GetContainerScope();
                if (std::find(containerScopes.begin(), containerScopes.end(), containerScope) == containerScopes.end())
                {
                    containerScopes.push_back(containerScope);
                }
            }
            else
            {
                throw Exception("'" + namespaceImportNode->Ns()->Str() + "' does not denote a namespace", namespaceImportNode->Ns()->GetSpan());
            }
        }
        else
        {
            throw Exception("referred namespace symbol '" + namespaceImportNode->Ns()->Str() + "' not found", namespaceImportNode->Ns()->GetSpan());
        }
    }
    else
    {
        throw std::runtime_error("current container scope is null");
    }
}

Symbol* FileScope::Lookup(const std::string& name) const
{
    return Lookup(name, ScopeLookup::this_);
}

Symbol* FileScope::Lookup(const std::string& name, ScopeLookup lookup) const
{
    if (lookup != ScopeLookup::this_)
    {
        throw std::runtime_error("file scope supports only this scope lookup");
    }
    std::unordered_set<Symbol*> foundSymbols;
    AliasSymbolMapIt i = aliasSymbolMap.find(name);
    if (i != aliasSymbolMap.end())
    {
        Symbol* symbol = i->second;
        foundSymbols.insert(symbol);
    }
    else
    {
        for (ContainerScope* containerScope : containerScopes)
        {
            Symbol* symbol = containerScope->Lookup(name, ScopeLookup::this_);
            if (symbol)
            {
                foundSymbols.insert(symbol);
            }
        }
    }
    if (foundSymbols.empty())
    {
        return nullptr;
    }
    else if (foundSymbols.size() > 1)
    {
        std::string message("reference to object '" + name + "' is ambiguous: ");
        bool first = true;
        for (Symbol* symbol : foundSymbols)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                message.append(" or ");
            }
            message.append(symbol->FullName());
        }
        throw Exception(message, Span());
    }
    else
    {
        return *foundSymbols.begin();
    }
}

FileScope* FileScope::Clone() const
{
    return new FileScope(*this);
}

void FileScope::CollectViableFunctions(const std::string& groupName, int arity, std::unordered_set<FunctionSymbol*>& viableFunctions)
{
    for (ContainerScope* containerScope : containerScopes)
    {
        containerScope->CollectViableFunctions(Cm::Sym::ScopeLookup::this_, groupName, arity, viableFunctions);
    }
}

} } // namespace Cm::Sym