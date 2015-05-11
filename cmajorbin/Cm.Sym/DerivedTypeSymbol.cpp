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

namespace Cm { namespace Sym {

TypeId ComputeDerivedTypeId(TypeSymbol* baseType, const Cm::Ast::DerivationList& derivations)
{
    TypeId id = baseType->Id();
    int m = derivations.NumDerivations();
    for (int i = 0; i < m; ++i)
    {
        uint8_t derivationCode = 1 << uint8_t(derivations[i]);
        id.Rep().Tag().data[i + 1] ^= derivationCode;
    }
    id.InvalidateHashCode();
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

std::string MakeMangleId(const Cm::Ast::DerivationList& derivations)
{
    std::string mangleId;
    uint8_t n = derivations.NumDerivations();
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
        }
    }
    return mangleId;
}

DerivedTypeSymbol::DerivedTypeSymbol(const Span& span_, const std::string& name_) : TypeSymbol(span_, name_), baseType(nullptr)
{
}

DerivedTypeSymbol::DerivedTypeSymbol(const Span& span_, const std::string& name_, TypeSymbol* baseType_, const Cm::Ast::DerivationList& derivations_, const TypeId& id_) : 
    TypeSymbol(span_, name_, id_), baseType(baseType_), derivations(derivations_)
{
    baseType->AddDependentType(this);
}

std::string DerivedTypeSymbol::GetMangleId() const
{
    return MakeMangleId(derivations) + baseType->GetMangleId();
}

std::string DerivedTypeSymbol::FullName() const
{
    return Cm::Ast::MakeDerivedTypeName(derivations, baseType->FullName());
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
}

void DerivedTypeSymbol::Read(Reader& reader)
{
    TypeSymbol::Read(reader);
    derivations = reader.ReadDerivationList();
    reader.FetchTypeFor(this, 0);
    reader.EnqueueMakeIrTypeFor(this);
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
        throw std::runtime_error("base type not set for derived type '" + FullName() + "'");
    }
    baseType->MakeIrType();
    SetIrType(Cm::Sym::MakeIrType(baseType, derivations, Cm::Parsing::Span()));
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
    if (Source() == SymbolSource::project)
    {
        exportedDerivedTypes.insert(this);
        SetSource(SymbolSource::library);
    }
}

void DerivedTypeSymbol::CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TemplateTypeSymbol*>& exportedTemplateTypes)
{
    if (Source() == SymbolSource::project)
    {
        if (collected.find(baseType) == collected.end())
        {
            collected.insert(baseType);
            baseType->CollectExportedTemplateTypes(collected, exportedTemplateTypes);
        }
    }
}

} } // namespace Cm::Sym