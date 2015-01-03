/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/DerivedTypeSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>

namespace Cm { namespace Sym {

TypeId ComputeDerivedTypeId(TypeSymbol* baseType, const Cm::Ast::DerivationList& derivations)
{
    TypeId id = baseType->Id();
    int m = derivations.NumDerivations();
    for (int k = 0; k < m; ++k)
    {
        uint8_t derivationCode = 1 << uint8_t(derivations[k]);
        uint8_t positionCode = 1 << k;
        int n = int(id.Rep().Tag().size());
        for (int i = 0; i < n; ++i)
        {
            if ((i & 1) == 0)
            {
                id.Rep().Tag().data[i] ^= derivationCode;
            }
            else
            {
                id.Rep().Tag().data[i] ^= positionCode;
            }
        }
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

bool HasRvalueRefDerivation(const Cm::Ast::DerivationList& derivations)
{
    uint8_t n = derivations.NumDerivations();
    for (uint8_t i = 0; i < n; ++i)
    {
        if (derivations[i] == Cm::Ast::Derivation::rvalueRef) return true;
    }
    return false;
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
}

std::string DerivedTypeSymbol::GetMangleId() const
{
    return MakeMangleId(derivations) + baseType->GetMangleId();
}

void DerivedTypeSymbol::Write(Writer& writer)
{
    TypeSymbol::Write(writer);
    writer.Write(baseType->Id());
    writer.Write(derivations);
}

void DerivedTypeSymbol::Read(Reader& reader)
{
    TypeSymbol::Read(reader);
    reader.FetchTypeFor(this, 0);
    derivations = reader.ReadDerivationList();
}

void DerivedTypeSymbol::SetType(TypeSymbol* type, int index)
{
    baseType = type;
}

void DerivedTypeSymbol::CollectExportedDerivedTypes(std::vector<TypeSymbol*>& exportedDerivedTypes)
{
    if (Source() == SymbolSource::project)
    {
        exportedDerivedTypes.push_back(this);
        SetSource(SymbolSource::library);
    }
}

} } // namespace Cm::Sym