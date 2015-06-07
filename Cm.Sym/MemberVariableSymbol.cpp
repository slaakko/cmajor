/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>

namespace Cm { namespace Sym {

MemberVariableSymbol::MemberVariableSymbol(const Span& span_, const std::string& name_) : Symbol(span_, name_), type(nullptr), layoutIndex(-1)
{
}

bool MemberVariableSymbol::IsExportSymbol() const 
{
    if (Parent()->IsClassTemplateSymbol()) return false;
    if (Parent()->IsTemplateTypeSymbol()) return false;
    return true; 
}

void MemberVariableSymbol::Write(Writer& writer)
{
    Symbol::Write(writer);
    writer.Write(type->Id());
}

void MemberVariableSymbol::Read(Reader& reader)
{
    Symbol::Read(reader);
    reader.FetchTypeFor(this, 0);
}

TypeSymbol* MemberVariableSymbol::GetType() const
{
    return type;
}

void MemberVariableSymbol::SetType(TypeSymbol* type_, int index)
{
    type = type_;
}

void MemberVariableSymbol::CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes)
{
	if (type->IsDerivedTypeSymbol())
	{
        if (collected.find(type) == collected.end())
        {
            collected.insert(type);
            type->CollectExportedDerivedTypes(collected, exportedDerivedTypes);
        }
	}
}

void MemberVariableSymbol::CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TemplateTypeSymbol*>& exportedTemplateTypes)
{
	if (type->IsTemplateTypeSymbol() || type->IsDerivedTypeSymbol())
	{
		if (collected.find(type) == collected.end())
		{
			collected.insert(type);
			type->CollectExportedTemplateTypes(collected, exportedTemplateTypes);
		}
	}
}

} } // namespace Cm::Sym