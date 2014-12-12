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

TypeId ComputeTypeId(TypeSymbol* baseType, const Cm::Ast::DerivationList& derivations)
{
    TypeId id = baseType->Id();
    int m = derivations.NumDerivations();
    for (int k = 0; k < m; ++k)
    {
        uint8_t derivationCode = uint8_t(derivations[k]);
        int n = int(id.Rep().Tag().size());
        for (int i = 0; i < n; ++i)
        {
            id.Rep().Tag().data[i] ^= derivationCode;
        }
    }
    return id;
}

DerivedTypeSymbol::DerivedTypeSymbol(const Span& span_, const std::string& name_) : TypeSymbol(span_, name_), baseType(nullptr)
{
}

DerivedTypeSymbol::DerivedTypeSymbol(const Span& span_, const std::string& name_, TypeSymbol* baseType_, const Cm::Ast::DerivationList& derivations_, const TypeId& id_) : 
    TypeSymbol(span_, name_, id_), baseType(baseType_), derivations(derivations_)
{
}

void DerivedTypeSymbol::SetDerivations(const Cm::Ast::DerivationList& derivations_)
{
    derivations = derivations_;
    SetId(ComputeTypeId(baseType, derivations));
    
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

} } // namespace Cm::Sym