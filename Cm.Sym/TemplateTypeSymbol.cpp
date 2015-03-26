/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/NameMangling.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <algorithm>
#include <stdexcept>

namespace Cm { namespace Sym {

std::string MakeTemplateTypeSymbolName(TypeSymbol* subjectType, const std::vector<TypeSymbol*>& typeArguments)
{
    std::string s = subjectType->Name();
    s.append(1, '<');
    int n = int(typeArguments.size());
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            s.append(", ");
        }
        s.append(typeArguments[i]->Name());
    }
    s.append(1, '>');
    return s;
}

TypeId ComputeTemplateTypeId(TypeSymbol* subjectType, const std::vector<TypeSymbol*>& typeArguments)
{
    TypeId id = subjectType->Id();
	uint8_t n = uint8_t(typeArguments.size());
	if (n >= id.Rep().Tag().size())
	{
		throw std::runtime_error("only " + std::to_string(id.Rep().Tag().size() - 1) + " supported");
	}
	for (uint8_t i = 0; i < n; ++i)
    {
		TypeSymbol* typeArgument = typeArguments[i];
		Cm::Util::Uuid argumentId = typeArgument->Id().Rep();
		uint8_t positionCode = i;
		std::rotate(argumentId.Tag().begin(), argumentId.Tag().begin() + positionCode, argumentId.Tag().end());
		id.Rep() = id.Rep() ^ argumentId;
    }
    id.InvalidateHashCode();
    return id;
}

TemplateTypeSymbol::TemplateTypeSymbol(const Span& span_, const std::string& name_) : ClassTypeSymbol(span_, name_), subjectType(nullptr)
{
}

TemplateTypeSymbol::TemplateTypeSymbol(const Span& span_, const std::string& name_, TypeSymbol* subjectType_, const std::vector<TypeSymbol*>& typeArguments_, const TypeId& id_) :
    ClassTypeSymbol(span_, name_, id_), subjectType(subjectType_), typeArguments(typeArguments_)
{
}

std::string TemplateTypeSymbol::GetMangleId() const
{
    std::string mangleId = MakeAssemblyName(subjectType->FullName());
    mangleId.append(Cm::IrIntf::GetPrivateSeparator());
    for (TypeSymbol* typeArgument : typeArguments)
    {
        mangleId.append(MakeAssemblyName(typeArgument->FullName()));
    }
    return mangleId;
}

void TemplateTypeSymbol::Write(Writer& writer)
{
    TypeSymbol::Write(writer);
    writer.GetBinaryWriter().Write(Parent()->FullName());
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
    TypeSymbol::Read(reader);
    std::string parentName = reader.GetBinaryReader().ReadString();
    Cm::Sym::Symbol* parent = reader.GetSymbolTable().GlobalScope()->Lookup(parentName);
    if (parent)
    {
        if (parent->IsContainerSymbol())
        {
            Cm::Sym::ContainerSymbol* containerParent = static_cast<Cm::Sym::ContainerSymbol*>(parent);
            containerParent->AddSymbol(this);
        }
        else
        {
            throw std::runtime_error("not container parent");
        }
    }
    else
    {
        throw std::runtime_error("not parent");
    }
    reader.FetchTypeFor(this, -1);
    uint8_t n = reader.GetBinaryReader().ReadByte();
    typeArguments.resize(n);
    for (int i = 0; i < int(n); ++i)
    {
        reader.FetchTypeFor(this, i);
    }
    reader.EnqueueMakeIrTypeFor(this);
}

void TemplateTypeSymbol::SetType(TypeSymbol* type, int index)
{
    if (index == -1)
    {
        SetSubjectType(type);
    }
    else
    {
        if (index >= int(typeArguments.size()))
        {
            throw std::runtime_error("invalid type index");
        }
        typeArguments[index] = type;
    }
}

void TemplateTypeSymbol::MakeIrType()
{
    if (IrTypeMade()) return;
    SetIrTypeMade();
    SetIrType(Cm::IrIntf::CreateClassTypeName(FullName()));
}

void TemplateTypeSymbol::SetSubjectType(TypeSymbol* subjectType_)
{
    subjectType = subjectType_;
}

void TemplateTypeSymbol::SetFileScope(FileScope* fileScope_)
{
    fileScope.reset(fileScope_);
}

void TemplateTypeSymbol::SetGlobalNs(Cm::Ast::NamespaceNode* globalNs_)
{
    globalNs.reset(globalNs_);
}

void TemplateTypeSymbol::CollectExportedDerivedTypes(std::unordered_set<TypeSymbol*>& exportedDerivedTypes)
{
    subjectType->CollectExportedDerivedTypes(exportedDerivedTypes);
    for (TypeSymbol* typeArgument : typeArguments)
    {
        typeArgument->CollectExportedDerivedTypes(exportedDerivedTypes);
    }
}

void TemplateTypeSymbol::CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TemplateTypeSymbol*>& exportedTemplateTypes)
{
    if (Source() == SymbolSource::project)
    {
        if (collected.find(subjectType) == collected.end())
        {
            collected.insert(subjectType);
            subjectType->CollectExportedTemplateTypes(collected, exportedTemplateTypes);
        }
        for (TypeSymbol* typeArgument : typeArguments)
        {
            if (collected.find(typeArgument) == collected.end())
            {
                collected.insert(typeArgument);
                typeArgument->CollectExportedTemplateTypes(collected, exportedTemplateTypes);
            }
        }
        collected.insert(this);
        exportedTemplateTypes.insert(this);
        SetSource(SymbolSource::library);
    }
}

} } // namespace Cm::Sym
