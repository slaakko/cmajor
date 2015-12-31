/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_TYPE_REPOSITORY_INCLUDED
#define CM_SYM_TYPE_REPOSITORY_INCLUDED
#include <Cm.Sym/DerivedTypeSymbol.hpp>
#include <Cm.Ast/Concept.hpp>

namespace Cm {  namespace Sym {

Ir::Intf::Type* MakeIrType(TypeSymbol* baseType, const Cm::Ast::DerivationList& derivations, const Span& span, int lastArrayDimension);
void MergeDerivations(Cm::Ast::DerivationList& targetDerivations, const Cm::Ast::DerivationList& sourceDerivations, std::vector<int>& targetArrayDimensions, const std::vector<int>& sourceArrayDimensions);
Cm::Ast::DerivationList RemoveDerivations(const Cm::Ast::DerivationList& targetDerivations, const Cm::Ast::DerivationList& sourceDerivations);

class TypeRepository
{
public:
    TypeRepository(SymbolTable& symbolTable_);
    SymbolTable& GetSymbolTable() { return symbolTable; }
    void AddType(TypeSymbol* type);
    TypeSymbol* GetTypeNothrow(const TypeId& typeId);
    TypeSymbol* GetType(const TypeId& typeId);
    TypeSymbol* MakeDerivedType(const Cm::Ast::DerivationList& derivations, TypeSymbol* baseType, const std::vector<int>& arrayDimensions, const Span& span);
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
    void CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_map<TypeId, std::unordered_set<TemplateTypeSymbol*>, TypeIdHash>& exportedTemplateTypes);
    void CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes);
    void Import(Reader& reader, SymbolTable& symbolTable);
    void ReplaceReplicaTypes();
    void Own(TypeSymbol* type);
private:
    SymbolTable& symbolTable;
    typedef std::unordered_map<TypeId, TypeSymbol*, TypeIdHash> TypeSymbolMap;
    typedef TypeSymbolMap::const_iterator TypeSymbolMapIt;
    TypeSymbolMap typeSymbolMap;
    std::vector<std::unique_ptr<TypeSymbol>> types;
};

} } // namespace Cm::Sym

#endif // CM_SYM_TYPE_REPOSITORY_INCLUDED
