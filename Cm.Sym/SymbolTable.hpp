/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_SYMBOL_TABLE_INCLUDED
#define CM_SYM_SYMBOL_TABLE_INCLUDED
#include <Cm.Sym/Namespace.hpp>
#include <stack>

namespace Cm { namespace Sym {

class SymbolTable
{
public:
    SymbolTable();
    void BeginNamespaceScope(const std::string& qualifiedNsName);
    void EndNamespaceScope();
private:
    NamespaceSymbol globalNs;
    Scope* currentScope;
    std::stack<Scope*> scopeStack;
};

} } // namespace Cm::Sym

#endif // CM_SYM_SYMBOL_TABLE_INCLUDED
