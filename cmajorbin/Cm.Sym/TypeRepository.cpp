/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/TypeRepository.hpp>
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
    return Cm::Ast::MakeDerivedTypeName(derivations, baseType->FullName());
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
    return derivedTypeSymbol;
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
    return templateTypeSymbol;
}

void TypeRepository::Export(Writer& writer)
{
    std::vector<TypeSymbol*> exportedTypes;
    for (const std::unique_ptr<TypeSymbol>& type : types)
    {
        if (type->IsExportSymbol())
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
