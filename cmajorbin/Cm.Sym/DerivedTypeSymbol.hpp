/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_DERIVED_TYPE_SYMBOL_INCLUDED
#define CM_SYM_DERIVED_TYPE_SYMBOL_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Ast/TypeExpr.hpp>

namespace Cm { namespace Sym {

class DerivedTypeSymbol : public TypeSymbol
{
public:
    DerivedTypeSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::derivedTypeSymbol; }
    TypeSymbol* GetBaseType() const;
    void SetBaseType(TypeSymbol* baseType_);
    const Cm::Ast::DerivationList& Derivations() const { return derivations; }
    void SetDerivations(const Cm::Ast::DerivationList& derivations_);
    bool Equals(TypeSymbol* that) const override;
    bool IsDerivedTypeSymbol() const override { return true; }
private:
    std::unique_ptr<TypeSymbol> baseType;
    Cm::Ast::DerivationList derivations;
};

} } // namespace Cm::Sym

#endif // CM_SYM_DERIVED_TYPE_SYMBOL_INCLUDED
