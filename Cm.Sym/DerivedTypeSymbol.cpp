/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/DerivedTypeSymbol.hpp>
#include <Cm.Sym/TypeRepository.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/Exception.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Sym {

TypeId ComputeDerivedTypeId(TypeSymbol* baseType, const Cm::Ast::DerivationList& derivations, const std::vector<int>& arrayDimensions)
{
    TypeId id = baseType->Id();
    int m = derivations.NumDerivations();
    for (int i = 0; i < m; ++i)
    {
        uint8_t derivationCode = 1 << uint8_t(derivations[i]);
        id.SetByte(i + 1,  id[i + 1] ^ derivationCode);
    }
    int b = 5;
    int n = int(arrayDimensions.size());
    for (int i = 0; i < n; ++i)
    {
        int arrayDimension = arrayDimensions[i];
        uint8_t d0 = (arrayDimension >> 24) & 0xFF;
        uint8_t d1 = (arrayDimension >> 16) & 0xFF;
        uint8_t d2 = (arrayDimension >> 8) & 0xFF;
        uint8_t d3 = arrayDimension & 0xFF;
        id.SetByte(b + i, id[b + i] ^ d0);
        id.SetByte(b + i + 1, id[b + i + 1] ^ d1);
        id.SetByte(b + i + 2, id[b + i + 2] ^ d2);
        id.SetByte(b + i + 3, id[b + i + 3] ^ d3);
    }
    return id;
}

bool HasPointertDerivation(const Cm::Ast::DerivationList& derivations)
{
    uint8_t n = derivations.NumDerivations();
    for (uint8_t i = 0; i < n; ++i)
    {
        if (derivations[i] == Cm::Ast::Derivation::pointer) return true;
    }
    return false;
}

bool HasVoidPtrDerivation(const Cm::Ast::DerivationList& derivations)
{
    int numPointers = 0;
    uint8_t n = derivations.NumDerivations();
    for (uint8_t i = 0; i < n; ++i)
    {
        if (derivations[i] == Cm::Ast::Derivation::pointer) ++numPointers;
        else return false;
    }
    return numPointers == 1;
}

int CountPointers(const Cm::Ast::DerivationList& derivations)
{
    int numPointers = 0;
    uint8_t n = derivations.NumDerivations();
    for (uint8_t i = 0; i < n; ++i)
    {
        if (derivations[i] == Cm::Ast::Derivation::pointer) ++numPointers;
    }
    return numPointers;
}

int CountPointersAfterArray(const Cm::Ast::DerivationList& derivations)
{
    int numPointers = 0;
    uint8_t n = derivations.NumDerivations();
    if (n > 0)
    {
        for (uint8_t i = n - 1; i >= 0; --i)
        {
            Cm::Ast::Derivation d = derivations[i];
            if (d == Cm::Ast::Derivation::pointer) ++numPointers;
            else if (d == Cm::Ast::Derivation::array_) break;
        }
    }
    return numPointers;
}

bool HasReferenceDerivation(const Cm::Ast::DerivationList& derivations)
{
    uint8_t n = derivations.NumDerivations();
    for (uint8_t i = 0; i < n; ++i)
    {
        if (derivations[i] == Cm::Ast::Derivation::reference) return true;
    }
    return false;
}

bool IsNonConstReferenceDerivationList(const Cm::Ast::DerivationList& derivations)
{
    uint8_t n = derivations.NumDerivations();
    for (uint8_t i = 0; i < n; ++i)
    {
        if (derivations[i] == Cm::Ast::Derivation::const_) return false;
        if (derivations[i] == Cm::Ast::Derivation::reference) return true;
    }
    return false;
}

bool HasConstDerivation(const Cm::Ast::DerivationList& derivations)
{
    uint8_t n = derivations.NumDerivations();
    for (uint8_t i = 0; i < n; ++i)
    {
        if (derivations[i] == Cm::Ast::Derivation::const_) return true;
    }
    return false;
}

bool HasConstReferenceDerivation(const Cm::Ast::DerivationList& derivations)
{
    return HasConstDerivation(derivations) && HasReferenceDerivation(derivations);
}

bool HasPureReferenceDerivation(const Cm::Ast::DerivationList& derivations)
{
    bool hasReferenceDerivation = true;
    uint8_t n = derivations.NumDerivations();
    for (uint8_t i = 0; i < n; ++i)
    {
        if (derivations[i] == Cm::Ast::Derivation::reference) hasReferenceDerivation = true;
        if (derivations[i] == Cm::Ast::Derivation::rvalueRef) return false;
        if (derivations[i] == Cm::Ast::Derivation::pointer) return false;
    }
    return hasReferenceDerivation;
}

bool HasRvalueRefDerivation(const Cm::Ast::DerivationList& derivations)
{
    uint8_t n = derivations.NumDerivations();
    for (uint8_t i = 0; i < n; ++i)
    {
        if (derivations[i] == Cm::Ast::Derivation::rvalueRef) return true;
    }
    return false;
}

bool HasConstPointerDerivation(const Cm::Ast::DerivationList& derivations)
{
    uint8_t n = derivations.NumDerivations();
    return n == 2 && derivations[0] == Cm::Ast::Derivation::const_ && derivations[1] == Cm::Ast::Derivation::pointer;
}

bool HasConstPointerPointerDerivation(const Cm::Ast::DerivationList& derivations)
{
    uint8_t n = derivations.NumDerivations();
    return n == 3 && derivations[0] == Cm::Ast::Derivation::const_ && derivations[1] == Cm::Ast::Derivation::pointer && derivations[2] == Cm::Ast::Derivation::pointer;;
}

bool HasPointerToArrayDerivation(const Cm::Ast::DerivationList& derivations)
{
    uint8_t n = derivations.NumDerivations();
    if (n > 1)
    {
        if (derivations[n - 1] == Cm::Ast::Derivation::pointer && derivations[n - 2] == Cm::Ast::Derivation::array_) return true;
    }
    return false;
}

bool HasRvalueRefOfArrayDerivation(const Cm::Ast::DerivationList& derivations)
{
    uint8_t n = derivations.NumDerivations();
    if (n != 2) return false;
    return derivations[0] == Cm::Ast::Derivation::array_ && derivations[1] == Cm::Ast::Derivation::rvalueRef;
}

DerivationCounts CountDerivations(const Cm::Ast::DerivationList& derivations)
{
    DerivationCounts counts;
    uint8_t n = derivations.NumDerivations();
    for (uint8_t i = 0; i < n; ++i)
    {
        switch (derivations[i])
        {
            case Cm::Ast::Derivation::const_: ++counts.consts; break;
            case Cm::Ast::Derivation::reference: ++counts.refs; break;
            case Cm::Ast::Derivation::rvalueRef: ++counts.rvalueRefs; break;
            case Cm::Ast::Derivation::pointer: ++counts.pointers; break;
        }
    }
    return counts;
}

std::string MakeMangleId(const Cm::Ast::DerivationList& derivations, const std::vector<int>& arrayDimensions)
{
    std::string mangleId;
    uint8_t n = derivations.NumDerivations();
    int arrayDimIndex = 0;
    for (uint8_t i = 0; i < n; ++i)
    {
        switch (derivations[i])
        {
            case Cm::Ast::Derivation::const_: mangleId.append(1, 'C'); break;
            case Cm::Ast::Derivation::reference: mangleId.append(1, 'R'); break;
            case Cm::Ast::Derivation::rvalueRef: mangleId.append("RR"); break;
            case Cm::Ast::Derivation::pointer: mangleId.append(1, 'P'); break;
            case Cm::Ast::Derivation::leftParen: mangleId.append("l"); break;
            case Cm::Ast::Derivation::rightParen: mangleId.append("r"); break;
            case Cm::Ast::Derivation::array_: mangleId.append("a").append(std::to_string(arrayDimensions[arrayDimIndex++])); break;
        }
    }
    return mangleId;
}

DerivedTypeSymbol::DerivedTypeSymbol(const Span& span_, const std::string& name_) : TypeSymbol(span_, name_), baseType(nullptr), arrayDimensions()
{
}

DerivedTypeSymbol::DerivedTypeSymbol(const Span& span_, const std::string& name_, TypeSymbol* baseType_, const Cm::Ast::DerivationList& derivations_, const std::vector<int>& arrayDimensions_, 
    const TypeId& id_) :
    TypeSymbol(span_, name_, id_), baseType(baseType_), derivations(derivations_), arrayDimensions(arrayDimensions_)
{
    baseType->AddDependentType(this);
}

std::string DerivedTypeSymbol::GetMangleId() const
{
    return MakeMangleId(derivations, arrayDimensions) + baseType->GetMangleId();
}

std::string DerivedTypeSymbol::FullName() const
{
    return Cm::Ast::MakeDerivedTypeName(derivations, baseType->FullName(), arrayDimensions);
}

bool DerivedTypeSymbol::IsExportSymbol() const
{
    if (baseType->IsTypeParameterSymbol()) return false;
    if (baseType->IsTemplateTypeSymbol())
    {
        TemplateTypeSymbol* baseTemplateType = static_cast<TemplateTypeSymbol*>(baseType);
        for (TypeSymbol* typeArgument : baseTemplateType->TypeArguments())
        {
            if (typeArgument->IsTypeParameterSymbol()) return false;
        }
    }
    return (baseType->IsPublic() || baseType->Serialize()) && !baseType->IsReplica();
}

std::string DerivedTypeSymbol::FullDocId() const
{
    std::string fullDocId;
    int arrayDimIndex = 0;
    int n = derivations.NumDerivations();
    for (int i = 0; i < n; ++i)
    {
        Cm::Ast::Derivation d = derivations[i];
        switch (d)
        {
            case Cm::Ast::Derivation::const_: if (!fullDocId.empty()) fullDocId.append(1, '.'); fullDocId.append("C"); break;
            case Cm::Ast::Derivation::pointer: if (!fullDocId.empty()) fullDocId.append(1, '.'); fullDocId.append("P"); break;
            case Cm::Ast::Derivation::reference: if (!fullDocId.empty()) fullDocId.append(1, '.'); fullDocId.append("R"); break;
            case Cm::Ast::Derivation::rvalueRef: if (!fullDocId.empty()) fullDocId.append(1, '.'); fullDocId.append("RR"); break;
            case Cm::Ast::Derivation::array_: if (!fullDocId.empty()) fullDocId.append(1, '.'); fullDocId.append("a").append(std::to_string(arrayDimensions[arrayDimIndex++])); break;
        }
    }
    if (!fullDocId.empty())
    {
        fullDocId.append(1, '.');
    }
    fullDocId.append(baseType->FullDocId());
    return fullDocId;
}

Symbol* DerivedTypeSymbol::Parent() const 
{
    Symbol* parent = TypeSymbol::Parent();
    if (!parent)
    {
        parent = baseType->Parent();
    }
    return parent;
}

void DerivedTypeSymbol::Write(Writer& writer)
{
    TypeSymbol::Write(writer);
    writer.Write(derivations);
    writer.Write(baseType->Id());
    uint8_t n = uint8_t(arrayDimensions.size());
    writer.GetBinaryWriter().Write(n);
    for (uint8_t i = 0; i < n; ++i)
    {
        writer.GetBinaryWriter().Write(arrayDimensions[i]);
    }
}

void DerivedTypeSymbol::Read(Reader& reader)
{
    TypeSymbol::Read(reader);
    derivations = reader.ReadDerivationList();
    reader.FetchTypeFor(this, 0);
    reader.EnqueueMakeIrTypeFor(this);
    uint8_t n = reader.GetBinaryReader().ReadByte();
    for (uint8_t i = 0; i < n; ++i)
    {
        int arrayDimension = reader.GetBinaryReader().ReadInt();
        arrayDimensions.push_back(arrayDimension);
    }
}

void DerivedTypeSymbol::SetType(TypeSymbol* type, int index)
{
    baseType = type;
    baseType->AddDependentType(this);
}

void DerivedTypeSymbol::MakeIrType()
{
    if (IrTypeMade()) return;
    if (!baseType)
    {
        throw Cm::Sym::Exception("base type not set for derived type", GetSpan());
    }
    baseType->MakeIrType();
    uint8_t n = uint8_t(arrayDimensions.size());
    if (n > 0)
    {
        if (n != 1)
        {
            throw Cm::Sym::Exception("arrays of arrays not supported", GetSpan());
        }
        SetIrType(Cm::Sym::MakeIrType(baseType, derivations, Cm::Parsing::Span(), GetLastArrayDimension()));
    }
    else
    {
        SetIrType(Cm::Sym::MakeIrType(baseType, derivations, Cm::Parsing::Span(), 0));
    }
    if (IsPointerType())
    {
        SetDefaultIrValue(GetIrType()->CreateDefaultValue());
    }
}

void DerivedTypeSymbol::RecomputeIrType()
{
    ResetIrTypeMade();
    MakeIrType();
}

void DerivedTypeSymbol::CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes)
{
    if (!IsExportSymbol()) return;
    if (Source() == SymbolSource::project)
    {
        exportedDerivedTypes.insert(this);
        SetSource(SymbolSource::library);
    }
}

void DerivedTypeSymbol::CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_map<TypeId, std::unordered_set<TemplateTypeSymbol*>, TypeIdHash>& exportedTemplateTypes)
{
    if (!IsExportSymbol()) return;
    if (Source() == SymbolSource::project)
    {
        if (collected.find(baseType) == collected.end())
        {
            collected.insert(baseType);
            baseType->CollectExportedTemplateTypes(collected, exportedTemplateTypes);
        }
    }
}

void DerivedTypeSymbol::ReplaceReplicaTypes() 
{
    TypeSymbol::ReplaceReplicaTypes();
    if (baseType->IsReplica() && baseType->IsTemplateTypeSymbol())
    {
        TemplateTypeSymbol* replica = static_cast<TemplateTypeSymbol*>(baseType);
        baseType = replica->GetPrimaryTemplateTypeSymbol();
    }
}

void DerivedTypeSymbol::DoSerialize()
{
    TypeSymbol::DoSerialize();
    baseType->DoSerialize();
}

} } // namespace Cm::Sym