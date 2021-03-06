/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_SYMBOL_SCOPE_INCLUDED
#define CM_SYM_SYMBOL_SCOPE_INCLUDED
#include <Cm.Sym/SymbolTypeSet.hpp>
#include <Cm.Ast/Namespace.hpp>
#include <unordered_map>
#include <unordered_set>

namespace Cm { namespace Sym {

enum class ScopeLookup : uint8_t
{
    none = 0, 
    this_ = 1 << 0, 
    base = 1 << 1, 
    parent = 1 << 2,
    this_and_base = this_ | base,
    this_and_parent = this_ | parent,
    this_and_base_and_parent = this_ | base | parent,
    fileScopes = 1 << 3
};

inline ScopeLookup operator&(ScopeLookup left, ScopeLookup right)
{
    return ScopeLookup(uint8_t(left) & uint8_t(right));
}

inline ScopeLookup operator~(ScopeLookup subject)
{
    return ScopeLookup(~uint8_t(subject));
}

class ContainerSymbol;
class NamespaceSymbol;
class ClassTypeSymbol;
class FunctionSymbol;

void SetCCOverrideSymbols(bool ccOverrideSymbols_);

class Scope
{
public:
    virtual ~Scope();
    virtual Symbol* Lookup(const std::string& name) const = 0;
    virtual Symbol* Lookup(const std::string& name, SymbolTypeSetId symbolTypeSetId) const = 0;
    virtual Symbol* Lookup(const std::string& name, ScopeLookup lookup) const = 0;
    virtual Symbol* Lookup(const std::string& name, ScopeLookup lookup, SymbolTypeSetId symbolTypeSetId) const = 0;
};

class ContainerScope : public Scope
{
public:
    ContainerScope();
    ContainerScope(ContainerScope&& that);
    ContainerScope* Base() const { return base; }
    void SetBase(ContainerScope* base_) { base = base_; }
    ContainerScope* Parent() const { return parent; }
    void SetParent(ContainerScope* parent_) { parent = parent_; }
    void Install(Symbol* symbol);
    Symbol* Lookup(const std::string& name) const override;
    Symbol* Lookup(const std::string& name, SymbolTypeSetId symbolTypeSetId) const override;
    Symbol* Lookup(const std::string& name, ScopeLookup lookup) const override;
    Symbol* Lookup(const std::string& name, ScopeLookup lookup, SymbolTypeSetId symbolTypeSetId) const override;
    void CollectViableFunctions(ScopeLookup lookup, const std::string& groupName, int arity, std::unordered_set<FunctionSymbol*>& viableFunctions);
    void CollectSymbolsForCC(std::unordered_set<Symbol*>& ccSymbols, ScopeLookup lookup, SymbolTypeSetId symbolTypeSetId, bool includeConceptSymbols);
    ContainerSymbol* Container() { return container; }
    void SetContainer(ContainerSymbol* container_) { container = container_; }
    NamespaceSymbol* Ns() const;
    ClassTypeSymbol* Class() const;
    InterfaceTypeSymbol* Interface() const;
    ContainerScope* ClassInterfaceOrNsScope() const;
    NamespaceSymbol* CreateNamespace(const std::string& qualifiedNsName, const Span& span);
private:
    typedef std::unordered_map<std::string, Symbol*> SymbolMap;
    typedef SymbolMap::const_iterator SymbolMapIt;
    SymbolMap symbolMap;
    ContainerScope* base;
    ContainerScope* parent;
    ContainerSymbol* container;
    Symbol* LookupQualified(const std::vector<std::string>& components, ScopeLookup lookup, SymbolTypeSetId symbolTypeSetId) const;
};

class SymbolTable;
class TypeRepository;

class FileScope : public Scope
{
public:
    FileScope();
    void InstallAlias(ContainerScope* currenContainerScope, Cm::Ast::AliasNode* aliasNode);
    void InstallNamespaceImport(ContainerScope* currentContainerScope, Cm::Ast::NamespaceImportNode* namespaceImportNode);
    Symbol* Lookup(const std::string& name) const override;
    Symbol* Lookup(const std::string& name, SymbolTypeSetId symbolTypeSetId) const override;
    Symbol* Lookup(const std::string& name, ScopeLookup lookup) const override;
    Symbol* Lookup(const std::string& name, ScopeLookup lookup, SymbolTypeSetId symbolTypeSetId) const override;
    FileScope* Clone() const;
    void CollectViableFunctions(const std::string& groupName, int arity, std::unordered_set<FunctionSymbol*>& viableFunctions, std::unordered_set<ContainerScope*>& processedScopes);
    void CollectSymbolsForCC(std::unordered_set<Symbol*>& ccSymbols, SymbolTypeSetId symbolTypeSetId, bool includeConceptSymbols);
private:
    std::vector<ContainerScope*> containerScopes;
    typedef std::unordered_map<std::string, Symbol*> AliasSymbolMap;
    typedef AliasSymbolMap::const_iterator AliasSymbolMapIt;
    AliasSymbolMap aliasSymbolMap;
};

} } // namespace Cm::Sym

#endif // CM_SYM_SYMBOL_SCOPE_INCLUDED
