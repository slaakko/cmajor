/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_CONSTANT_SYMBOL_INCLUDED
#define CM_SYM_CONSTANT_SYMBOL_INCLUDED
#include <Cm.Sym/Symbol.hpp>
#include <Cm.Ast/Constant.hpp>

namespace Cm { namespace Sym {

class ConstantSymbol : public Symbol
{
public:
    ConstantSymbol(Cm::Ast::ConstantNode* constantNode);
};

} } // namespace Cm::Sym

#endif // CM_SYM_CONSTANT_SYMBOL_INCLUDED
