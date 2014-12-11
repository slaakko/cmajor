/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>

namespace Cm { namespace Sym {

TemplateTypeSymbol::TemplateTypeSymbol(const Span& span_, const std::string& name_) : TypeSymbol(span_, name_), subjectType(nullptr)
{
}

void TemplateTypeSymbol::Write(Writer& writer)
{
    writer.Write(subjectType->Id());
    uint8_t n = uint8_t(typeArguments.size());
    writer.GetBinaryWriter().Write(n);
    for (uint8_t i = 0; i < n; ++i)
    {
        writer.Write(typeArguments[i]->Id());
    }
}

void TemplateTypeSymbol::Read(Reader& reader)
{
    reader.FetchTypeFor(this, -1);
    uint8_t n = reader.GetBinaryReader().ReadByte();
    for (int i = 0; i < int(n); ++i)
    {
        reader.FetchTypeFor(this, i);
    }
}

void TemplateTypeSymbol::SetType(TypeSymbol* type, int index)
{
    if (index == -1)
    {
        SetSubjectType(type);
    }
    else
    {
        if (index != int(typeArguments.size()))
        {
            throw std::runtime_error("invalid type argument index");
        }
        AddTypeArgument(type);
    }
}


void TemplateTypeSymbol::SetSubjectType(TypeSymbol* subjectType_)
{
    subjectType = subjectType_;
}

void TemplateTypeSymbol::AddTypeArgument(TypeSymbol* typeArgument)
{
    typeArguments.push_back(typeArgument);
}

} } // namespace Cm::Sym
