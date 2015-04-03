/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/TypeParameterSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

TypeParameterSymbol::TypeParameterSymbol(const Span& span_, const std::string& name_) : TypeSymbol(span_, name_, TypeId()), index(-1)
{
}

void TypeParameterSymbol::Write(Writer& writer)
{
    TypeSymbol::Write(writer);
}

void TypeParameterSymbol::Read(Reader& reader)
{
    TypeSymbol::Read(reader);
}

BoundTypeParameterSymbol::BoundTypeParameterSymbol(const Span& span_, const std::string& name_): Symbol(span_, name_), type(nullptr)
{
}

} } // namespace Cm::Sym