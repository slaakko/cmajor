/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/LocalVariableSymbol.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/Writer.hpp>

namespace Cm { namespace Sym {

LocalVariableSymbol::LocalVariableSymbol(const Span& span_, const std::string& name_) : Symbol(span_, name_), type(nullptr)
{
}

void LocalVariableSymbol::Write(Writer& writer)
{
    writer.Write(type->Id());
}

void LocalVariableSymbol::Read(Reader& reader)
{
    // todo
}

TypeSymbol* LocalVariableSymbol::GetType() const
{
    return type;
}

void LocalVariableSymbol::SetType(TypeSymbol* type_)
{
    type = type_;
}

} } // namespace Cm::Sym
