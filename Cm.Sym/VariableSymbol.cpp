/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/VariableSymbol.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>

namespace Cm { namespace Sym {

VariableSymbol::VariableSymbol(const Span& span_, const std::string& name_) : Symbol(span_, name_), type(nullptr), hasType(false)
{
}

VariableSymbol::~VariableSymbol()
{
    int x = 0;
}

void VariableSymbol::Write(Writer& writer)
{
    Symbol::Write(writer);
    bool hasType = type != nullptr;
    writer.GetBinaryWriter().Write(hasType);
    if (hasType)
    {
        writer.Write(type->Id());
    }
}

void VariableSymbol::Read(Reader& reader)
{
    Symbol::Read(reader);
    if (Name() == "$initialized")
    {
        int x = 0;
    }
    hasType = reader.GetBinaryReader().ReadBool();
    if (hasType)
    {
        reader.FetchTypeFor(this, 0);
    }
}

void VariableSymbol::SetType(TypeSymbol* type_, int index)
{
    SetType(type_);
}

std::string VariableSymbol::UniqueFullName() const
{
    if (Parent())
    {
        return Parent()->FullName() + "." + UniqueName();
    }
    else
    {
        return UniqueName();
    }
}

} } // namespace Cm::Sym
