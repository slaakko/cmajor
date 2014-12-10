/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ParameterSymbol.hpp>
#include <Cm.Sym/TypeSymbol.hpp>

namespace Cm { namespace Sym {

ParameterSymbol::ParameterSymbol(const Span& span_, const std::string& name_) : Symbol(span_, name_)
{
}

TypeSymbol* ParameterSymbol::GetType() const
{
    return type.get();
}

void ParameterSymbol::SetType(TypeSymbol* type_)
{
    type.reset(type_);
}

} } // namespace Cm::Sym