/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

MemberVariableSymbol::MemberVariableSymbol(Cm::Ast::MemberVariableNode* memberVariableNode) : Symbol(memberVariableNode->Id()->Str())
{
}

} } // namespace Cm::Sym