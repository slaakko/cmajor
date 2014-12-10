/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

EnumTypeSymbol::EnumTypeSymbol(const Span& span_, const std::string& name_) : TypeSymbol(span_, name_)
{
    underlyingType.reset(new IntTypeSymbol());
}

void EnumTypeSymbol::SetUnderlyingType(TypeSymbol* underlyingType_)
{
    underlyingType.reset(underlyingType_);
}

EnumConstantSymbol::EnumConstantSymbol(const Span& span_, const std::string& name_) : Symbol(span_, name_), evaluating(false)
{
}

void EnumConstantSymbol::SetValue(Value* value_)
{
    value.reset(value_);
}

} } // namespace Cm::Sym