/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/TypeRepository.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>

namespace Cm { namespace Sym {

void TypeRepository::AddType(TypeSymbol* type)
{
    typeSymbolMap[type->Id()] = type;
}

TypeSymbol* TypeRepository::GetTypeNothrow(const TypeId& typeId) const
{
    TypeSymbolMapIt i = typeSymbolMap.find(typeId);
    if (i != typeSymbolMap.end())
    {
        return i->second;
    }
    else
    {
        return nullptr;
    }
}

TypeSymbol* TypeRepository::GetType(const TypeId& typeId) const
{
    TypeSymbol* typeSymbol = GetTypeNothrow(typeId);
    if (typeSymbol)
    {
        return typeSymbol;
    }
    else
    {
        throw std::runtime_error("type symbol not found");
    }
}

std::string MakeDerivedTypeName(const Cm::Ast::DerivationList& derivations, TypeSymbol* baseType)
{
    return Cm::Ast::MakeDerivedTypeName(derivations, baseType->Name());
}

TypeSymbol* TypeRepository::MakeDerivedType(const Cm::Ast::DerivationList& derivations, TypeSymbol* baseType, const Span& span)
{
    TypeId typeId = ComputeTypeId(baseType, derivations);
    TypeSymbol* typeSymbol = GetTypeNothrow(typeId);
    if (typeSymbol)
    {
        return typeSymbol;
    }
    DerivedTypeSymbol* derivedTypeSymbol = new DerivedTypeSymbol(span, MakeDerivedTypeName(derivations, baseType), baseType, derivations, typeId);
    AddType(derivedTypeSymbol);
    types.push_back(std::unique_ptr<TypeSymbol>(derivedTypeSymbol));
    derivedTypeSymbol->SetExportSymbol();
    return derivedTypeSymbol;
}

TypeSymbol* TypeRepository::MakeConstCharPtrType(const Span& span)
{
    TypeId charTypeId = GetBasicTypeId(ShortBasicTypeId::charId);
    TypeSymbol* charType = GetType(charTypeId);
    Cm::Ast::DerivationList derivations;
    derivations.Add(Cm::Ast::Derivation::const_);
    derivations.Add(Cm::Ast::Derivation::pointer);
    return MakeDerivedType(derivations, charType, span);
}

TypeSymbol* TypeRepository::MakeGenericPtrType(const Span& span)
{
    TypeId voidTypeId = GetBasicTypeId(ShortBasicTypeId::voidId);
    TypeSymbol* voidType = GetType(voidTypeId);
    Cm::Ast::DerivationList derivations;
    derivations.Add(Cm::Ast::Derivation::pointer);
    return MakeDerivedType(derivations, voidType, span);
}

TypeSymbol* TypeRepository::MakeTemplateType(TypeSymbol* subjectType, const std::vector<TypeSymbol*>& typeArguments, const Span& span)
{
    TypeId typeId = ComputeTypeId(subjectType, typeArguments);
    TypeSymbol* typeSymbol = GetTypeNothrow(typeId);
    if (typeSymbol)
    {
        return typeSymbol;
    }
    TemplateTypeSymbol* templateTypeSymbol = new TemplateTypeSymbol(span, MakeTemplateTypeSymbolName(subjectType, typeArguments), subjectType, typeArguments, typeId);
    AddType(templateTypeSymbol);
    types.push_back(std::unique_ptr<TypeSymbol>(templateTypeSymbol));
    templateTypeSymbol->SetExportSymbol();
    return templateTypeSymbol;
}

void TypeRepository::Export(Writer& writer)
{
    std::vector<TypeSymbol*> exportedTypes;
    for (const std::unique_ptr<TypeSymbol>& type : types)
    {
        if (!type->IsBasicTypeSymbol() && type->IsExportSymbol())
        {
            exportedTypes.push_back(type.get());
        }
    }
    int32_t n = int32_t(exportedTypes.size());
    writer.GetBinaryWriter().Write(n);
    for (int32_t i = 0; i < n; ++i)
    {
        TypeSymbol* type = exportedTypes[i];
        writer.Write(type);
    }
}

void TypeRepository::Import(Reader& reader)
{
    int32_t n = reader.GetBinaryReader().ReadInt();
    for (int32_t i = 0; i < n; ++i)
    {
        Symbol* symbol = reader.ReadSymbol();
        if (symbol->IsTypeSymbol())
        {
            TypeSymbol* typeSymbol = static_cast<TypeSymbol*>(symbol);
            types.push_back(std::unique_ptr<TypeSymbol>(typeSymbol));
        }
        else
        {
            throw std::runtime_error("type symbol expected");
        }
    }
}

} } // namespace Cm::Sym
