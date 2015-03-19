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

TypeId ComputeDerivedTypeId(TypeSymbol* baseType, const Cm::Ast::DerivationList& derivations);
bool HasPointertDerivation(const Cm::Ast::DerivationList& derivations);
bool HasVoidPtrDerivation(const Cm::Ast::DerivationList& derivations);
int CountPointers(const Cm::Ast::DerivationList& derivations);
bool HasReferenceDerivation(const Cm::Ast::DerivationList& derivations);
bool IsNonConstReferenceDerivationList(const Cm::Ast::DerivationList& derivations);
bool HasConstDerivation(const Cm::Ast::DerivationList& derivations);
bool HasConstReferenceDerivation(const Cm::Ast::DerivationList& derivations);
bool HasRvalueRefDerivation(const Cm::Ast::DerivationList& derivations);
bool HasConstPointerDerivation(const Cm::Ast::DerivationList& derivations);
bool HasConstPointerPointerDerivation(const Cm::Ast::DerivationList& derivations);
DerivationCounts CountDerivations(const Cm::Ast::DerivationList& derivations);

class DerivedTypeSymbol : public TypeSymbol
{
public:
    DerivedTypeSymbol(const Span& span_, const std::string& name_);
    DerivedTypeSymbol(const Span& span_, const std::string& name_, TypeSymbol* baseType_, const Cm::Ast::DerivationList& derivations_, const TypeId& id_);
    SymbolType GetSymbolType() const override { return SymbolType::derivedTypeSymbol; }
    std::string TypeString() const override { return "derived type"; };
    std::string GetMangleId() const override;
    const Cm::Ast::DerivationList& Derivations() const override { return derivations; }
    bool IsDerivedTypeSymbol() const override { return true; }
    TypeSymbol* GetBaseType() const override { return baseType; }
    bool IsPointerType() const override { return HasPointertDerivation(derivations); }
    bool IsPointerToClassType() const override { return GetPointerCount() == 1 && GetBaseType()->IsClassTypeSymbol(); }
    bool IsPointerToTemplateType() const override { return GetPointerCount() == 1 && GetBaseType()->IsTemplateTypeSymbol(); }
    bool IsVoidPtrType() const override { return baseType->IsVoidTypeSymbol() && HasVoidPtrDerivation(derivations); }
    int GetPointerCount() const override { return CountPointers(derivations); }
    bool IsReferenceType() const { return HasReferenceDerivation(derivations); }
    bool IsNonConstReferenceType() const override { return IsNonConstReferenceDerivationList(derivations); }
    bool IsConstType() const override { return HasConstDerivation(derivations); }
    bool IsRvalueRefType() const override { return HasRvalueRefDerivation(derivations); }
    bool IsConstReferenceType() const { return HasConstReferenceDerivation(derivations); }
    bool IsConstPointerType() const { return HasConstPointerDerivation(derivations); }
    bool IsConstPointerPointerType() const { return HasConstPointerPointerDerivation(derivations); }
    DerivationCounts GetDerivationCounts() const override { return CountDerivations(derivations); }
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    void SetType(TypeSymbol* type, int index) override;
    void MakeIrType() override;
    void CollectExportedDerivedTypes(std::unordered_set<TypeSymbol*>& exportedDerivedTypes) override;
    void CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TemplateTypeSymbol*>& exportedTemplateTypes) override;
private:
    TypeSymbol* baseType;
    Cm::Ast::DerivationList derivations;
};

} } // namespace Cm::Sym

#endif // CM_SYM_DERIVED_YPE_SYMBOL_INCLUDED
