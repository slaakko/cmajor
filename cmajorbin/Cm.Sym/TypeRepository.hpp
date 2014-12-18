/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_TYPE_REPOSITORY_INCLUDED
#define CM_SYM_TYPE_REPOSITORY_INCLUDED
#include <Cm.Sym/DerivedTypeSymbol.hpp>

namespace Cm {  namespace Sym {

class TypeRepository
{
public:
    void AddType(TypeSymbol* type);
    TypeSymbol* GetTypeNothrow(const TypeId& typeId) const;
    TypeSymbol* GetType(const TypeId& typeId) const;
    TypeSymbol* MakeDerivedType(const Cm::Ast::DerivationList& derivations, TypeSymbol* baseType, const Span& span, bool requirePublic);
    TypeSymbol* MakePointerType(TypeSymbol* baseType, const Span& span, bool requirePublic);
    TypeSymbol* MakeRvalueRefType(TypeSymbol* baseType, const Span& span, bool requirePublic);
    TypeSymbol* MakeConstPointerType(TypeSymbol* baseType, const Span& span, bool requirePublic);
    TypeSymbol* MakeConstCharPtrType(const Span& span);
    TypeSymbol* MakeGenericPtrType(const Span& span);
    TypeSymbol* MakeTemplateType(TypeSymbol* subjectType, const std::vector<TypeSymbol*>& typeArguments, const Span& span, bool requirePublic);
    TypeSymbol* MakePlainType(TypeSymbol* type);
    TypeSymbol* MakePlainTypeWithOnePointerRemoved(TypeSymbol* type);
    void Export(Writer& writer);
    void Import(Reader& reader);
    void ClearInternalTypes();
private:
    typedef std::unordered_map<TypeId, TypeSymbol*, TypeIdHash> TypeSymbolMap;
    typedef TypeSymbolMap::const_iterator TypeSymbolMapIt;
    TypeSymbolMap typeSymbolMap;
    std::vector<std::unique_ptr<TypeSymbol>> publicTypes;
    std::vector<std::unique_ptr<TypeSymbol>> internalTypes;
};

} } // namespace Cm::Sym

#endif // CM_SYM_TYPE_REPOSITORY_INCLUDED
