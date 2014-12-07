/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_MEMBER_VARIABLE_SYMBOL_INCLUDED
#define CM_SYM_MEMBER_VARIABLE_SYMBOL_INCLUDED
#include <Cm.Sym/Symbol.hpp>
#include <Cm.Ast/Class.hpp>

namespace Cm { namespace Sym {

class MemberVariableSymbol : public Symbol
{
public:
    MemberVariableSymbol(Cm::Ast::MemberVariableNode* memberVariableNode);
};

} } // namespace Cm::Sym

#endif // CM_SYM_MEMBER_VARIABLE_SYMBOL_INCLUDED
