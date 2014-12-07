/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_SYMBOL_SCOPE_INCLUDED
#define CM_SYM_SYMBOL_SCOPE_INCLUDED
#include <Cm.Sym/Symbol.hpp>
#include <Cm.Ast/Namespace.hpp>
#include <unordered_map>

namespace Cm { namespace Sym {

enum class ScopeLookup : uint8_t
{
    none = 0, 
    this_ = 1, 
    base = 2, 
    parent = 4,
    this_and_base = this_ | base,
    this_and_parent = this_ | parent,
    this_and_base_and_parent = this_ | base | parent
};

inline ScopeLookup operator&(ScopeLookup left, ScopeLookup right)
{
    return ScopeLookup(uint8_t(left) & uint8_t(right));
}

class ContainerSymbol;
class NamespaceSymbol;
class ClassSymbol;

class Scope
{
public:
    virtual ~Scope();
    virtual Symbol* Lookup(const std::string& name) const = 0;
    virtual Symbol* Lookup(const std::string& name, ScopeLookup lookup) const = 0;
};

class ContainerScope : public Scope
{
public:
    ContainerScope();
    ContainerScope* Base() const { return base; }
    void SetBase(ContainerScope* base_) { base = base_; }
    ContainerScope* Parent() const { return parent; }
    void SetParent(ContainerScope* parent_) { parent = parent_; }
    void Install(Symbol* symbol);
    Symbol* Lookup(const std::string& name) const override;
    Symbol* Lookup(const std::string& name, ScopeLookup lookup) const override;
    ContainerSymbol* Container() { return container; }
    void SetContainer(ContainerSymbol* container_) { container = container_; }
    NamespaceSymbol* Ns() const;
    ClassSymbol* Class() const;
    NamespaceSymbol* CreateNamespace(const std::string& qualifiedNsName, Cm::Ast::Node* node);
private:
    typedef std::unordered_map<std::string, Symbol*> SymbolMap;
    typedef SymbolMap::const_iterator SymbolMapIt;
    SymbolMap symbolMap;
    ContainerScope* base;
    ContainerScope* parent;
    ContainerSymbol* container;
    Symbol* LookupQualified(const std::vector<std::string>& components, ScopeLookup lookup) const;
};

class SymbolTable;

class FileScope : public Scope
{
public:
    void InstallAlias(ContainerScope* currenContainerScope, Cm::Ast::AliasNode* aliasNode);
    void InstallNamespaceImport(ContainerScope* currentContainerScope, Cm::Ast::NamespaceImportNode* namespaceImportNode);
    Symbol* Lookup(const std::string& name) const override;
    Symbol* Lookup(const std::string& name, ScopeLookup lookup) const override;
private:
    std::vector<ContainerScope*> containerScopes;
    typedef std::unordered_map<std::string, Symbol*> AliasSymbolMap;
    typedef AliasSymbolMap::const_iterator AliasSymbolMapIt;
    AliasSymbolMap aliasSymbolMap;
};

} } // namespace Cm::Sym

#endif // CM_SYM_SYMBOL_SCOPE_INCLUDED
