/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_ENUMERATION_INCLUDED
#define CM_BIND_ENUMERATION_INCLUDED
#include <Cm.Sym/SymbolTable.hpp>

namespace Cm { namespace Bind {

void BindEnumType(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, Cm::Ast::EnumTypeNode* enumTypeNode);
void BindEnumConstant(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Ast::EnumConstantNode* enumConstantNode);

} } // namespace Cm::Bind

#endif // CM_BIND_ENUMERATION_INCLUDED
