/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_INTERFACE_INCLUDED
#define CM_BIND_INTERFACE_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Ast/Interface.hpp>

namespace Cm { namespace Bind {

void BindInterface(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, Cm::Ast::InterfaceNode* interfaceNode);

} } // namespace Cm::Bind

#endif // CM_BIND_CLASS_INCLUDED
