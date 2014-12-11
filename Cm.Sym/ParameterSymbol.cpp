/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ParameterSymbol.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>

namespace Cm { namespace Sym {

ParameterSymbol::ParameterSymbol(const Span& span_, const std::string& name_) : Symbol(span_, name_), type(nullptr)
{
}

void ParameterSymbol::Write(Writer& writer)
{
    Symbol::Write(writer);
    writer.Write(type->Id());
}

void ParameterSymbol::Read(Reader& reader)
{
    Symbol::Read(reader);
    reader.FetchTypeFor(this, 0);
}

TypeSymbol* ParameterSymbol::GetType() const
{
    return type;
}

void ParameterSymbol::SetType(TypeSymbol* type_, int index)
{
    type = type_;
    if (Source() == SymbolSource::project)
    {
        type->SetExportSymbol();
    }
}

} } // namespace Cm::Sym