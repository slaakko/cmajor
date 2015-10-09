/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/TypedefSymbol.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>

namespace Cm { namespace Sym {

TypedefSymbol::TypedefSymbol(const Span& span_, const std::string& name_) : Symbol(span_, name_), type(nullptr), evaluating(false)
{
}

bool TypedefSymbol::IsExportSymbol() const 
{
    if (Parent()->IsClassTemplateSymbol()) return false;
    if (Parent()->IsTemplateTypeSymbol()) return false;
    return Symbol::IsExportSymbol();
}

void TypedefSymbol::Write(Writer& writer)
{
    Symbol::Write(writer);
    writer.Write(type->Id());
}

void TypedefSymbol::Read(Reader& reader)
{
    Symbol::Read(reader);
    reader.FetchTypeFor(this, 0);
}

void TypedefSymbol::SetType(TypeSymbol* type_, int index)
{
    type = type_; 
}

void TypedefSymbol::CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes)
{
    if (collected.find(type) == collected.end())
    {
        collected.insert(type);
        type->CollectExportedDerivedTypes(collected, exportedDerivedTypes);
    }
}

void TypedefSymbol::CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_map<TypeId, std::unordered_set<TemplateTypeSymbol*>, TypeIdHash>& exportedTemplateTypes)
{
    if (collected.find(type) == collected.end())
    {
        collected.insert(type);
        type->CollectExportedTemplateTypes(collected, exportedTemplateTypes);
    }
}

void TypedefSymbol::Dump(CodeFormatter& formatter)
{
    if (!IsProject()) return;
    std::string typeString;
    if (type)
    {
        typeString.append(type->FullName()).append(" ");
    }
    formatter.WriteLine(SymbolFlagStr(Flags(), DeclaredAccess(), true) + " " + TypeString() + " " + typeString + Name());
}

void TypedefSymbol::ReplaceReplicaTypes()
{
    if (type->IsReplica() && type->IsTemplateTypeSymbol())
    {
        TemplateTypeSymbol* replica = static_cast<TemplateTypeSymbol*>(type);
        type = replica->GetPrimaryTemplateTypeSymbol();
    }
}

} } // namespace Cm::Sym