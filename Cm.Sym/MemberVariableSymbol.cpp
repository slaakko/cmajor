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
#include <Cm.Sym/TemplateTypeSymbol.hpp>

namespace Cm { namespace Sym {

MemberVariableSymbol::MemberVariableSymbol(const Span& span_, const std::string& name_) : VariableSymbol(span_, name_), layoutIndex(-1)
{
}

bool MemberVariableSymbol::IsExportSymbol() const
{
    if (Parent()->IsClassTemplateSymbol()) return false;
    if (Parent()->IsTemplateTypeSymbol()) return false;
    return true;
}

void MemberVariableSymbol::CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes)
{
    TypeSymbol* type = GetType();
	if (type->IsDerivedTypeSymbol())
	{
        if (collected.find(type) == collected.end())
        {
            collected.insert(type);
            type->CollectExportedDerivedTypes(collected, exportedDerivedTypes);
        }
	}
}

void MemberVariableSymbol::CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_map<TypeId, std::unordered_set<TemplateTypeSymbol*>, TypeIdHash>& exportedTemplateTypes)
{
    TypeSymbol* type = GetType();
	if (type->IsTemplateTypeSymbol() || type->IsDerivedTypeSymbol())
	{
		if (collected.find(type) == collected.end())
		{
			collected.insert(type);
			type->CollectExportedTemplateTypes(collected, exportedTemplateTypes);
		}
	}
}

void MemberVariableSymbol::Dump(CodeFormatter& formatter)
{
}

void MemberVariableSymbol::ReplaceReplicaTypes()
{
    TypeSymbol* type = GetType();
    if (type->IsReplica() && type->IsTemplateTypeSymbol())
    {
        TemplateTypeSymbol* replica = static_cast<TemplateTypeSymbol*>(type);
        type = replica->GetPrimaryTemplateTypeSymbol();
    }
}

} } // namespace Cm::Sym
