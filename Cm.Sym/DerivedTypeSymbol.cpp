/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/DerivedTypeSymbol.hpp>
#include <Cm.Sym/Writer.hpp>

namespace Cm { namespace Sym {

DerivedTypeSymbol::DerivedTypeSymbol(const Span& span_, const std::string& name_) : TypeSymbol(span_, name_), baseType(nullptr)
{
}

void DerivedTypeSymbol::Read(Reader& reader)
{
    // todo
}

TypeSymbol* DerivedTypeSymbol::GetBaseType() const
{
    return baseType;
}

void DerivedTypeSymbol::SetBaseType(TypeSymbol* baseType_) 
{
    baseType = baseType_;
}

const Cm::Ast::DerivationList& Derivations() const;
void SetDerivations(const Cm::Ast::DerivationList& derivations_);

void DerivedTypeSymbol::SetDerivations(const Cm::Ast::DerivationList& derivations_)
{
    derivations = derivations_;
}

bool DerivedTypeSymbol::Equals(TypeSymbol* that) const
{
    if (that->IsDerivedTypeSymbol())
    {
        DerivedTypeSymbol* derivedThat = static_cast<DerivedTypeSymbol*>(that);
        return baseType->Equals(derivedThat->baseType) && derivations == derivedThat->derivations;
    }
    return false;
}


} } // namespace Cm::Sym