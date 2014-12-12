/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_DERIVED_YPE_SYMBOL_INCLUDED
#define CM_SYM_DERIVED_YPE_SYMBOL_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Ast/TypeExpr.hpp>

namespace Cm { namespace Sym {

TypeId ComputeTypeId(TypeSymbol* baseType, const Cm::Ast::DerivationList& derivations);

class DerivedTypeSymbol : public TypeSymbol
{
public:
    DerivedTypeSymbol(const Span& span_, const std::string& name_);
    DerivedTypeSymbol(const Span& span_, const std::string& name_, TypeSymbol* baseType_, const Cm::Ast::DerivationList& derivations_, const TypeId& id_);
    SymbolType GetSymbolType() const override { return SymbolType::derivedTypeSymbol; }
    std::string TypeString() const override { return "derived type"; };
    const Cm::Ast::DerivationList& Derivations() const { return derivations; }
    void SetDerivations(const Cm::Ast::DerivationList& derivations_);
    bool IsDerivedTypeSymbol() const override { return true; }
    TypeSymbol* GetBaseType() const override { return baseType; }
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    void SetType(TypeSymbol* type, int index) override;
private:
    TypeSymbol* baseType;
    Cm::Ast::DerivationList derivations;
};

} } // namespace Cm::Sym

#endif // CM_SYM_DERIVED_YPE_SYMBOL_INCLUDED
