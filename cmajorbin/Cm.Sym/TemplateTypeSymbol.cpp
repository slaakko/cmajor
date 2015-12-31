/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/NameMangling.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
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
        s.append(typeArguments[i]->FullName());
    }
    s.append(1, '>');
    return s;
}

TypeId ComputeTemplateTypeId(TypeSymbol* subjectType, const std::vector<TypeSymbol*>& typeArguments)
{
    TypeId id = subjectType->Id();
	uint8_t n = uint8_t(typeArguments.size());
    uint8_t m = uint8_t(typeIdRepSize);
	if (n >= m)
	{
		throw std::runtime_error("only " + std::to_string(typeIdRepSize - 1) + " template arguments supported");
	}
	for (uint8_t i = 0; i < n; ++i)
    {
		TypeSymbol* typeArgument = typeArguments[i];
        TypeId argumentId = typeArgument->Id();
		uint8_t positionCode = (i + (m / 2)) % m;
        std::rotate(argumentId.Rep(), argumentId.Rep() + positionCode, argumentId.Rep() + typeIdRepSize);
        id = id ^ argumentId;
    }
    id.InvalidateHashCode();
    return id;
}

TemplateTypeSymbol::TemplateTypeSymbol(const Span& span_, const std::string& name_) : ClassTypeSymbol(span_, name_), subjectType(nullptr), primaryTemplateTypeSymbol(nullptr)
{
}

TemplateTypeSymbol::TemplateTypeSymbol(const Span& span_, const std::string& name_, TypeSymbol* subjectType_, const std::vector<TypeSymbol*>& typeArguments_, const TypeId& id_) :
    ClassTypeSymbol(span_, name_, id_), subjectType(subjectType_), typeArguments(typeArguments_), primaryTemplateTypeSymbol(nullptr)
{
    subjectType->AddDependentType(this);
    for (TypeSymbol* typeArgument : typeArguments)
    {
        typeArgument->AddDependentType(this);
    }
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

bool TemplateTypeSymbol::IsExportSymbol() const
{
    if (IsReplica()) return false;
    for (TypeSymbol* typeArgument : typeArguments)
    {
        if (!typeArgument->IsPublic() && !typeArgument->Serialize()) return false;
        if (typeArgument->IsTypeParameterSymbol()) return false;
    }
    return true;
}

void TemplateTypeSymbol::Write(Writer& writer)
{
    ClassTypeSymbol::Write(writer);
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
    ClassTypeSymbol::Read(reader);
    std::string parentName = reader.GetBinaryReader().ReadString();
    Cm::Sym::Symbol* parent = reader.GetSymbolTable().GlobalScope()->Lookup(parentName);
    bool beginNamespace = false;
    if (!parent)
    {
        reader.GetSymbolTable().BeginNamespaceScope(parentName, Cm::Parsing::Span());
        parent = reader.GetSymbolTable().Container();
        beginNamespace = true;
    }
    if (parent->IsContainerSymbol())
    {
        Cm::Sym::ContainerSymbol* containerParent = static_cast<Cm::Sym::ContainerSymbol*>(parent);
        containerParent->AddSymbol(this);
    }
    else
    {
        throw std::runtime_error("not container parent");
    }
    if (beginNamespace)
    {
        reader.GetSymbolTable().EndNamespaceScope();
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

void TemplateTypeSymbol::AddTypeArgument(TypeSymbol* typeArgument)
{
    typeArguments.push_back(typeArgument);
    typeArgument->AddDependentType(this);
}

void TemplateTypeSymbol::SetType(TypeSymbol* type, int index)
{
    ClassTypeSymbol::SetType(type, index);
    if (index == -1)
    {
        SetSubjectType(type);
    }
    else if (index >= 0)
    {
        if (index >= int(typeArguments.size()))
        {
            throw std::runtime_error("invalid type index");
        }
        typeArguments[index] = type;
        type->AddDependentType(this);
    }
}

void TemplateTypeSymbol::MakeIrType()
{
    if (IrTypeMade()) return;
    SetIrType(Cm::IrIntf::CreateClassTypeName(FullName()));
}

void TemplateTypeSymbol::RecomputeIrType()
{
    ResetIrTypeMade();
    MakeIrType();
    for (TypeSymbol* dependentType : DependentTypes())
    {
        dependentType->RecomputeIrType();
    }
}

void TemplateTypeSymbol::SetSubjectType(TypeSymbol* subjectType_)
{
    subjectType = subjectType_;
    subjectType->AddDependentType(this);
}

void TemplateTypeSymbol::SetFileScope(FileScope* fileScope_)
{
    fileScope.reset(fileScope_);
}

void TemplateTypeSymbol::SetGlobalNs(Cm::Ast::NamespaceNode* globalNs_)
{
    globalNs.reset(globalNs_);
}

void TemplateTypeSymbol::CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes)
{
    if (!IsExportSymbol()) return;
    ClassTypeSymbol::CollectExportedDerivedTypes(collected, exportedDerivedTypes);
    if (collected.find(subjectType) == collected.end())
    {
        collected.insert(subjectType);
        subjectType->CollectExportedDerivedTypes(collected, exportedDerivedTypes);
    }
    for (TypeSymbol* typeArgument : typeArguments)
    {
        if (collected.find(typeArgument) == collected.end())
        {
            collected.insert(typeArgument);
            typeArgument->CollectExportedDerivedTypes(collected, exportedDerivedTypes);
        }
    }
}

void TemplateTypeSymbol::CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_map<TypeId, std::unordered_set<TemplateTypeSymbol*>, TypeIdHash>& exportedTemplateTypes)
{
    if (!IsExportSymbol()) return;
    ClassTypeSymbol::CollectExportedTemplateTypes(collected, exportedTemplateTypes);
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
        exportedTemplateTypes[Id()].insert(this);
        SetSource(SymbolSource::library);
    }
}

void TemplateTypeSymbol::SetConstraint(Cm::Ast::WhereConstraintNode* constraint_)
{
    constraint.reset(constraint_);
}

std::string TemplateTypeSymbol::FullDocId() const
{
    std::string fullDocId = subjectType->FullDocId();
    for (Cm::Sym::TypeSymbol* typeArgument : typeArguments)
    {
        fullDocId.append(1, '.').append(typeArgument->FullDocId());
    }
    return fullDocId;
}

void TemplateTypeSymbol::ReplaceReplicaTypes()
{
    ClassTypeSymbol::ReplaceReplicaTypes();
    for (TypeSymbol*& typeArgument : typeArguments)
    {
        if (typeArgument->IsReplica() && typeArgument->IsTemplateTypeSymbol())
        {
            TemplateTypeSymbol* replica = static_cast<TemplateTypeSymbol*>(typeArgument);
            typeArgument = replica->GetPrimaryTemplateTypeSymbol();
        }
    }
}

void TemplateTypeSymbol::DoSerialize()
{
    for (TypeSymbol* typeArgument : typeArguments)
    {
        if (typeArgument->IsTypeParameterSymbol()) return;
    }
    ClassTypeSymbol::DoSerialize();
    for (TypeSymbol* typeArgument : typeArguments)
    {
        typeArgument->DoSerialize();
    }
}

} } // namespace Cm::Sym
