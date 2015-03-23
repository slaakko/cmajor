/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/TypeRepository.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/NamespaceSymbol.hpp>
#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/Exception.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <iostream>

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

void CountDerivations(const Cm::Ast::DerivationList& derivations, int& numPointers, bool& ref, bool& rvalueRef, const Span& span)
{
    numPointers = 0;
    ref = false;
    rvalueRef = false;
    for (Cm::Ast::Derivation derivation : derivations)
    {
        switch (derivation)
        {
            case Cm::Ast::Derivation::pointer: 
            {
                ++numPointers;
                break;
            }
            case Cm::Ast::Derivation::reference: 
            {
                ref = true;
                break;
            }
            case Cm::Ast::Derivation::rvalueRef:
            {
                rvalueRef = true;
                break;
            }
        }
    }
}

Ir::Intf::Type* MakeIrType(TypeSymbol* baseType, const Cm::Ast::DerivationList& derivations, const Span& span)
{
    Ir::Intf::Type* baseIrType = nullptr;
    if (baseType->IsEnumTypeSymbol())
    {
        EnumTypeSymbol* enumType = static_cast<EnumTypeSymbol*>(baseType);
        baseType = enumType->GetUnderlyingType();
    }
    if (baseType->IsVoidTypeSymbol())
    {
        baseIrType = Ir::Intf::GetFactory()->GetI8();
    }
    else
    {
        baseIrType = baseType->GetIrType();
    }
    int numPointers = 0;
    bool ref = false;
    bool rvalueRef = false;
    CountDerivations(derivations, numPointers, ref, rvalueRef, span);
    if (ref)
    {
        ++numPointers;
    }
    else if (rvalueRef)
    {
        if (numPointers == 0)
        {
            return Cm::IrIntf::RvalueRef(baseIrType);
        }
        else
        {
            return Cm::IrIntf::RvalueRef(Cm::IrIntf::Pointer(baseIrType, numPointers));
        }
    }
    if (numPointers == 0)
    {
        return baseIrType;
    }
    else
    {
        return Cm::IrIntf::Pointer(baseIrType, numPointers);
    }
}

void MergeDerivations(Cm::Ast::DerivationList& targetDerivations, const Cm::Ast::DerivationList& sourceDerivations)
{
    int n = sourceDerivations.NumDerivations();
    for (int i = 0; i < n; ++i)
    {
        Cm::Ast::Derivation derivation = sourceDerivations[i];
        bool found = false;
        if (derivation == Cm::Ast::Derivation::reference || derivation == Cm::Ast::Derivation::rvalueRef)
        {
            int m = targetDerivations.NumDerivations();
            for (int j = 0; j < m; ++j)
            {
                Cm::Ast::Derivation td = targetDerivations[j];
                if (td == derivation)
                {
                    found = true;
                    break;
                }
            }
        }
        if (!found)
        {
            if (derivation == Cm::Ast::Derivation::const_)
            {
                targetDerivations.InsertFront(derivation);
            }
            else
            {
                targetDerivations.Add(derivation);
            }
        }
    }
}

Cm::Ast::DerivationList RemoveDerivations(const Cm::Ast::DerivationList& targetDerivations, const Cm::Ast::DerivationList& sourceDerivations)
{
    Cm::Ast::DerivationList result;
    int n = targetDerivations.NumDerivations();
    for (int i = 0; i < n; ++i)
    {
        Cm::Ast::Derivation targetDerivation = targetDerivations[i];
        int m = sourceDerivations.NumDerivations();
        bool found = false;
        for (int j = 0; j < m; ++j)
        {
            Cm::Ast::Derivation sourceDerivation = sourceDerivations[j];
            if (targetDerivation == sourceDerivation)
            {
                found = true;
                break;
            }
            else if (targetDerivation == Cm::Ast::Derivation::reference && sourceDerivation == Cm::Ast::Derivation::rvalueRef)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            result.Add(targetDerivation);
        }
    }
    return result;
}

TypeSymbol* TypeRepository::MakeDerivedType(const Cm::Ast::DerivationList& derivations, TypeSymbol* baseType, const Span& span)
{
    Cm::Ast::DerivationList finalDerivations = derivations;
    TypeSymbol* finalBaseType = baseType;
    if (baseType->IsDerivedTypeSymbol())
    {
        DerivedTypeSymbol* baseDerivedType = static_cast<DerivedTypeSymbol*>(baseType);
        finalDerivations = baseDerivedType->Derivations();
        MergeDerivations(finalDerivations, derivations);
        finalBaseType = baseDerivedType->GetBaseType();
    }
    if (HasRvalueRefDerivation(finalDerivations) && HasReferenceDerivation(finalDerivations))
    {
        finalDerivations.RemoveReference();
        return MakeDerivedType(finalDerivations, finalBaseType, span); // hack to remove rvalue references to reference types
    }
    TypeId typeId = ComputeDerivedTypeId(finalBaseType, finalDerivations);
    TypeSymbol* typeSymbol = GetTypeNothrow(typeId);
    if (typeSymbol)
    {
        return typeSymbol;
    }
    std::unique_ptr<DerivedTypeSymbol> derivedTypeSymbol(new DerivedTypeSymbol(span, MakeDerivedTypeName(finalDerivations, finalBaseType), finalBaseType, finalDerivations, typeId));
    derivedTypeSymbol->SetAccess(SymbolAccess::public_);
    if (!baseType->IsTypeParameterSymbol() && !baseType->IsFunctionGroupTypeSymbol())
    {
        derivedTypeSymbol->SetIrType(MakeIrType(finalBaseType, finalDerivations, span));
        derivedTypeSymbol->SetDefaultIrValue(derivedTypeSymbol->GetIrType()->CreateDefaultValue());
    }
    types.push_back(std::unique_ptr<TypeSymbol>(derivedTypeSymbol.get()));
    AddType(derivedTypeSymbol.get());
    return derivedTypeSymbol.release();
}

TypeSymbol* TypeRepository::MakePointerType(TypeSymbol* baseType, const Span& span)
{
    if (baseType->IsDerivedTypeSymbol())
    {
        DerivedTypeSymbol* derivedType = static_cast<DerivedTypeSymbol*>(baseType);
        Cm::Ast::DerivationList derivations = derivedType->Derivations();
        derivations.Add(Cm::Ast::Derivation::pointer);
        return MakeDerivedType(derivations, derivedType->GetBaseType(), span);
    }
    Cm::Ast::DerivationList derivations;
    derivations.Add(Cm::Ast::Derivation::pointer);
    return MakeDerivedType(derivations, baseType, span);
}

TypeSymbol* TypeRepository::MakeRvalueRefType(TypeSymbol* baseType, const Span& span)
{
    if (baseType->IsRvalueRefType())
    {
        return baseType;
    }
    if (baseType->IsDerivedTypeSymbol())
    {
        DerivedTypeSymbol* derivedType = static_cast<DerivedTypeSymbol*>(baseType);
        Cm::Ast::DerivationList derivations = derivedType->Derivations();
        derivations.Add(Cm::Ast::Derivation::rvalueRef);
        return MakeDerivedType(derivations, derivedType->GetBaseType(), span);
    }
    Cm::Ast::DerivationList derivations;
    derivations.Add(Cm::Ast::Derivation::rvalueRef);
    return MakeDerivedType(derivations, baseType, span);
}

TypeSymbol* TypeRepository::MakeReferenceType(TypeSymbol* baseType, const Span& span)
{
    if (baseType->IsNonConstReferenceType())
    {
        return baseType;
    }
    if (baseType->IsDerivedTypeSymbol())
    {
        DerivedTypeSymbol* derivedType = static_cast<DerivedTypeSymbol*>(baseType);
        Cm::Ast::DerivationList derivations = derivedType->Derivations();
        if (!HasReferenceDerivation(derivations))
        {
            derivations.Add(Cm::Ast::Derivation::reference);
        }
        return MakeDerivedType(derivations, derivedType->GetBaseType(), span);
    }
    Cm::Ast::DerivationList derivations;
    derivations.Add(Cm::Ast::Derivation::reference);
    return MakeDerivedType(derivations, baseType, span);
}

TypeSymbol* TypeRepository::MakeConstReferenceType(TypeSymbol* baseType, const Span& span)
{
    if (baseType->IsConstReferenceType())
    {
        return baseType;
    }
    if (baseType->IsDerivedTypeSymbol())
    {
        DerivedTypeSymbol* derivedType = static_cast<DerivedTypeSymbol*>(baseType);
        Cm::Ast::DerivationList derivations = derivedType->Derivations();
        if (!HasConstDerivation(derivations))
        {
            derivations.InsertFront(Cm::Ast::Derivation::const_);
        }
        if (!HasReferenceDerivation(derivations))
        {
            derivations.Add(Cm::Ast::Derivation::reference);
        }
        return MakeDerivedType(derivations, derivedType->GetBaseType(), span);
    }
    Cm::Ast::DerivationList derivations;
    derivations.Add(Cm::Ast::Derivation::const_);
    derivations.Add(Cm::Ast::Derivation::reference);
    return MakeDerivedType(derivations, baseType, span);
}

TypeSymbol* TypeRepository::MakeConstPointerType(TypeSymbol* baseType, const Span& span)
{
    Cm::Ast::DerivationList derivations;
    derivations.Add(Cm::Ast::Derivation::const_);
    derivations.Add(Cm::Ast::Derivation::pointer);
    return MakeDerivedType(derivations, baseType, span);
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

TypeSymbol* TypeRepository::MakeConstCharPtrPtrType(const Span& span)
{
    TypeId charTypeId = GetBasicTypeId(ShortBasicTypeId::charId);
    TypeSymbol* charType = GetType(charTypeId);
    Cm::Ast::DerivationList derivations;
    derivations.Add(Cm::Ast::Derivation::const_);
    derivations.Add(Cm::Ast::Derivation::pointer);
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
    TypeId typeId = ComputeTemplateTypeId(subjectType, typeArguments);
    TypeSymbol* typeSymbol = GetTypeNothrow(typeId);
    if (typeSymbol)
    {
        return typeSymbol;
    }
    std::unique_ptr<TemplateTypeSymbol> templateTypeSymbol(new TemplateTypeSymbol(subjectType->GetSpan(), MakeTemplateTypeSymbolName(subjectType, typeArguments), subjectType, typeArguments, typeId));
    templateTypeSymbol->MakeIrType();
    templateTypeSymbol->SetAccess(SymbolAccess::public_);
    templateTypeSymbol->SetParent(subjectType->Ns());
    types.push_back(std::unique_ptr<TypeSymbol>(templateTypeSymbol.get()));
    AddType(templateTypeSymbol.get());
    return templateTypeSymbol.release();
}

Cm::Ast::DerivationList ClearConstsRefsAndRvalueRefs(const Cm::Ast::DerivationList& derivations)
{
    Cm::Ast::DerivationList result;
    for (uint8_t i = 0; i < derivations.NumDerivations(); ++i)
    {
        Cm::Ast::Derivation derivation = derivations[i];
        if (derivation == Cm::Ast::Derivation::pointer)
        {
            result.Add(derivation); 
        }
    }
    return result;
}

Cm::Ast::DerivationList ClearConstsRefsRvalueRefsAndOnePointer(const Cm::Ast::DerivationList& derivations)
{
    Cm::Ast::DerivationList result = ClearConstsRefsAndRvalueRefs(derivations);
    result.RemoveLastPointer();
    return result;
}

TypeSymbol* TypeRepository::MakePlainType(TypeSymbol* type)
{
    if (type->IsDerivedTypeSymbol())
    {
        DerivedTypeSymbol* derivedType = static_cast<DerivedTypeSymbol*>(type);
        Cm::Ast::DerivationList derivations = derivedType->Derivations();
        derivations = ClearConstsRefsAndRvalueRefs(derivations);
        if (derivations.NumDerivations() == 0)
        {
            return derivedType->GetBaseType();
        }
        else
        {
            return MakeDerivedType(derivations, derivedType->GetBaseType(), derivedType->GetSpan());
        }
    }
    else
    {
        return type;
    }
}

TypeSymbol* TypeRepository::MakePlainTypeWithOnePointerRemoved(TypeSymbol* type)
{
    if (type->IsDerivedTypeSymbol())
    {
        DerivedTypeSymbol* derivedType = static_cast<DerivedTypeSymbol*>(type);
        Cm::Ast::DerivationList derivations = derivedType->Derivations();
        derivations = ClearConstsRefsRvalueRefsAndOnePointer(derivations);
        if (derivations.NumDerivations() == 0)
        {
            return derivedType->GetBaseType();
        }
        else
        {
            return MakeDerivedType(derivations, derivedType->GetBaseType(), derivedType->GetSpan());
        }
    }
    else
    {
        return type;
    }
}

void TypeRepository::Import(Reader& reader)
{
    int32_t nt = reader.GetBinaryReader().ReadInt();
    for (int32_t i = 0; i < nt; ++i)
    {
        Symbol* symbol = reader.ReadSymbol();
        if (symbol->IsTemplateTypeSymbol())
        {
            TemplateTypeSymbol* templateTypeSymbol = static_cast<TemplateTypeSymbol*>(symbol);
            types.push_back(std::unique_ptr<TypeSymbol>(templateTypeSymbol));
        }
        else
        {
            throw std::runtime_error("template type symbol expected");
        }
    }
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
    reader.MakeIrTypes();
    if (!reader.AllTypesFetched())
    {
        if (GetGlobalFlag(GlobalFlags::ide))
        {
            std::cout << "not all types fetched!" << std::endl;
        }
        else
        {
            std::cerr << "not all types fetched!" << std::endl;
        }
    }
}

} } // namespace Cm::Sym
