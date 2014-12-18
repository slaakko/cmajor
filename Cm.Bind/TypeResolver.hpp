/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_TYPE_RESOLVER_INCLUDED
#define CM_BIND_TYPE_RESOLVER_INCLUDED
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Bind {

Cm::Sym::TypeSymbol* ResolveType(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* currentContainerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::Node* typeExpr, bool requirePublic);

} } // namespace Cm::Bind

#endif // CM_BIND_TYPE_RESOLVER_INCLUDED
