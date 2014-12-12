/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_MEMBER_VARIABLE_INCLUDED
#define CM_BIND_MEMBER_VARIABLE_INCLUDED
#include <Cm.Sym/SymbolTable.hpp>

namespace Cm { namespace Bind {

void BindMemberVariable(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::MemberVariableNode* memberVariableNode);

} } // namespace Cm::Bind

#endif // CM_BIND_MEMBER_VARIABLE_INCLUDED
