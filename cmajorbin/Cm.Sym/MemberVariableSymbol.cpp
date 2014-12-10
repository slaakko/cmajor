/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Sym/TypeSymbol.hpp>

namespace Cm { namespace Sym {

MemberVariableSymbol::MemberVariableSymbol(const Span& span_, const std::string& name_) : Symbol(span_, name_)
{
}

TypeSymbol* MemberVariableSymbol::GetType() const
{
    return type.get();
}

void MemberVariableSymbol::SetType(TypeSymbol* type_)
{
    type.reset(type_);
}

} } // namespace Cm::Sym