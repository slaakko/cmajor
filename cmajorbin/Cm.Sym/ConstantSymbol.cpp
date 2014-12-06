/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ConstantSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

ConstantSymbol::ConstantSymbol(Cm::Ast::ConstantNode* constantNode) : Symbol(constantNode->Id()->Str())
{
    SetNode(constantNode);
}

} } // namespace Cm::Sym
