/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_SYMBOL_SCOPE_INCLUDED
#define CM_SYM_SYMBOL_SCOPE_INCLUDED
#include <Cm.Sym/Symbol.hpp>
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

class NamespaceSymbol;

class Scope
{
public:
    Scope();
    Scope* Base() const { return base; }
    void SetBase(Scope* base_) { base = base_; }
    Scope* Parent() const { return parent; }
    void SetParent(Scope* parent_) { parent = parent_; }
    Scope* Global() const { return global; }
    void SetGlobal(Scope* global_) { global = global_; }
    void Install(Symbol* symbol);
    Symbol* Lookup(const std::string& name) const;
    Symbol* Lookup(const std::string& name, ScopeLookup lookup) const;
    NamespaceSymbol* Ns() { return ns; }
    void SetNs(NamespaceSymbol* ns_) { ns = ns_; }
    Scope* CreateNamespace(const std::string& qualifiedNsName);
private:
    typedef std::unordered_map<std::string, Symbol*> SymbolMap;
    typedef SymbolMap::const_iterator SymbolMapIt;
    SymbolMap symbolMap;
    Scope* base;
    Scope* parent;
    Scope* global;
    NamespaceSymbol* ns;
};

} } // namespace Cm::Sym

#endif // CM_SYM_SYMBOL_SCOPE_INCLUDED
