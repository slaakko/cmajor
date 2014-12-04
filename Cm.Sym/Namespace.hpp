/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_NAMESPACE_INCLUDED
#define CM_SYM_NAMESPACE_INCLUDED
#include <Cm.Sym/Scope.hpp>

namespace Cm { namespace Sym {

class NamespaceSymbol : public Symbol
{
public:
    NamespaceSymbol();
    NamespaceSymbol(const std::string& name_);
    virtual Scope* GetScope() { return &scope; }
    virtual bool IsNamespaceSymbol() const { return true; }
    void AddSymbol(Symbol* symbol);
    bool IsGlobalNamespace() const { return Name().empty(); }
private:
    Scope scope;
    std::vector<std::unique_ptr<Symbol>> symbols;
};

} } // namespace Cm::Sym

#endif // CM_SYM_NAMESPACE_INCLUDED
