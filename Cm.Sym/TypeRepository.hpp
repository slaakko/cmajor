/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_TYPE_REPOSITORY_INCLUDED
#define CM_SYM_TYPE_REPOSITORY_INCLUDED
#include <Cm.Sym/DerivedTypeSymbol.hpp>
#include <Cm.Ast/Concept.hpp>

namespace Cm {  namespace Sym {

Ir::Intf::Type* MakeIrType(TypeSymbol* baseType, const Cm::Ast::DerivationList& derivations, const Span& span);
void MergeDerivations(Cm::Ast::DerivationList& targetDerivations, const Cm::Ast::DerivationList& sourceDerivations);
Cm::Ast::DerivationList RemoveDerivations(const Cm::Ast::DerivationList& targetDerivations, const Cm::Ast::DerivationList& sourceDerivations);

class TypeRepository
{
public:
    void AddType(TypeSymbol* type);
    TypeSymbol* GetTypeNothrow(const TypeId& typeId) const;
    TypeSymbol* GetType(const TypeId& typeId) const;
    TypeSymbol* MakeDerivedType(const Cm::Ast::DerivationList& derivations, TypeSymbol* baseType, const Span& span);
    TypeSymbol* MakePointerType(TypeSymbol* baseType, const Span& span);
    TypeSymbol* MakeRvalueRefType(TypeSymbol* baseType, const Span& span);
    TypeSymbol* MakeReferenceType(TypeSymbol* baseType, const Span& span);
    TypeSymbol* MakeConstReferenceType(TypeSymbol* baseType, const Span& span);
    TypeSymbol* MakeConstPointerType(TypeSymbol* baseType, const Span& span);
    TypeSymbol* MakeConstCharPtrType(const Span& span);
    TypeSymbol* MakeConstCharPtrPtrType(const Span& span);
    TypeSymbol* MakeGenericPtrType(const Span& span);
    TypeSymbol* MakeTemplateType(TypeSymbol* subjectType, const std::vector<TypeSymbol*>& typeArguments, const Span& span);
    TypeSymbol* MakePlainType(TypeSymbol* type);
    TypeSymbol* MakePlainTypeWithOnePointerRemoved(TypeSymbol* type);
    void Import(Reader& reader);
private:
    typedef std::unordered_map<TypeId, TypeSymbol*, TypeIdHash> TypeSymbolMap;
    typedef TypeSymbolMap::const_iterator TypeSymbolMapIt;
    TypeSymbolMap typeSymbolMap;
    std::vector<std::unique_ptr<TypeSymbol>> types;
};

} } // namespace Cm::Sym

#endif // CM_SYM_TYPE_REPOSITORY_INCLUDED
