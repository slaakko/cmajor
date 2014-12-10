/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/DerivedTypeSymbol.hpp>

namespace Cm { namespace Sym {

DerivedTypeSymbol::DerivedTypeSymbol(const Span& span_, const std::string& name_) : TypeSymbol(span_, name_)
{
}

TypeSymbol* DerivedTypeSymbol::GetBaseType() const
{
    return baseType.get();
}

void DerivedTypeSymbol::SetBaseType(TypeSymbol* baseType_) 
{
    baseType.reset(baseType_);
}

void DerivedTypeSymbol::SetDerivations(const Cm::Ast::DerivationList& derivations_)
{
    derivations = derivations_;
}

bool DerivedTypeSymbol::Equals(TypeSymbol* that) const
{
    if (that->IsDerivedTypeSymbol())
    {
        DerivedTypeSymbol* derivedThat = static_cast<DerivedTypeSymbol*>(that);
        return baseType->Equals(derivedThat->baseType.get()) && derivations == derivedThat->derivations;
    }
    return false;
}


} } // namespace Cm::Sym