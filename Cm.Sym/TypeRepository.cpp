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

std::string MakeDerivedTypeName(const Cm::Ast::DerivationList& derivations, TypeSymbol* baseType, const std::vector<int>& arrayDimensions)
{
    return Cm::Ast::MakeDerivedTypeName(derivations, baseType->Name(), arrayDimensions);
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

Ir::Intf::Type* MakeIrType(TypeSymbol* baseType, const Cm::Ast::DerivationList& derivations, const Span& span, int lastArrayDimension)
{
    Ir::Intf::Type* baseIrType = nullptr;
    if (baseType->IsEnumTypeSymbol())
    {
        EnumTypeSymbol* enumType = static_cast<EnumTypeSymbol*>(baseType);
        baseType = enumType->GetUnderlyingType();
    }
    Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
    if (baseType->IsVoidTypeSymbol())
    {
        if (backend == Cm::IrIntf::BackEnd::llvm)
        {
            baseIrType = Ir::Intf::GetFactory()->GetI8();
        }
        else if (backend == Cm::IrIntf::BackEnd::c)
        {
            baseIrType = Ir::Intf::GetFactory()->GetVoid();
        }
    }
    else
    {
        baseIrType = baseType->GetIrType();
    }
    if (lastArrayDimension != 0)
    {
        baseIrType = Cm::IrIntf::Array(baseIrType, lastArrayDimension);
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

void MergeDerivations(Cm::Ast::DerivationList& targetDerivations, const Cm::Ast::DerivationList& sourceDerivations, std::vector<int>& targetArrayDimensions, const std::vector<int>& sourceArrayDimensions)
{
    int sourceArrayDimensionIndex = 0;
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
            else if (derivation == Cm::Ast::Derivation::array_)
            {
                targetArrayDimensions.push_back(sourceArrayDimensions[sourceArrayDimensionIndex++]);
                targetDerivations.Add(derivation);
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
    Cm::Ast::DerivationList srcDerivations = sourceDerivations;
    int n = targetDerivations.NumDerivations();
    for (int i = 0; i < n; ++i)
    {
        Cm::Ast::Derivation targetDerivation = targetDerivations[i];
        int m = srcDerivations.NumDerivations();
        bool found = false;
        for (int j = 0; j < m; ++j)
        {
            Cm::Ast::Derivation sourceDerivation = srcDerivations[j];
            if (targetDerivation == sourceDerivation)
            {
                found = true;
                srcDerivations[j] = Cm::Ast::Derivation::none;
                break;
            }
            else if (targetDerivation == Cm::Ast::Derivation::reference && sourceDerivation == Cm::Ast::Derivation::rvalueRef)
            {
                found = true;
                srcDerivations[j] = Cm::Ast::Derivation::none;
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

TypeSymbol* TypeRepository::MakeDerivedType(const Cm::Ast::DerivationList& derivations, TypeSymbol* baseType, const std::vector<int>& arrayDimensions, const Span& span)
{
    Cm::Ast::DerivationList finalDerivations = derivations;
    TypeSymbol* finalBaseType = baseType;
    std::vector<int> finalArrayDimensions = arrayDimensions;
    if (baseType->IsDerivedTypeSymbol())
    {
        DerivedTypeSymbol* baseDerivedType = static_cast<DerivedTypeSymbol*>(baseType);
        finalDerivations = baseDerivedType->Derivations();
        finalArrayDimensions = baseDerivedType->GetArrayDimensions();
        MergeDerivations(finalDerivations, derivations, finalArrayDimensions, arrayDimensions);
        finalBaseType = baseDerivedType->GetBaseType();
    }
    if (HasRvalueRefDerivation(finalDerivations) && HasReferenceDerivation(finalDerivations))
    {
        finalDerivations.RemoveReference();
        return MakeDerivedType(finalDerivations, finalBaseType, finalArrayDimensions, span); // hack to remove rvalue references to reference types
    }
    TypeId typeId = ComputeDerivedTypeId(finalBaseType, finalDerivations, finalArrayDimensions);
    TypeSymbol* typeSymbol = GetTypeNothrow(typeId);
    if (typeSymbol)
    {
        return typeSymbol;
    }
    std::unique_ptr<DerivedTypeSymbol> derivedTypeSymbol(new DerivedTypeSymbol(span, MakeDerivedTypeName(finalDerivations, finalBaseType, finalArrayDimensions), finalBaseType, finalDerivations,
        finalArrayDimensions, typeId));
    derivedTypeSymbol->SetAccess(SymbolAccess::public_);
    if (!baseType->IsTypeParameterSymbol() && !baseType->IsFunctionGroupTypeSymbol())
    {
        uint8_t n = uint8_t(finalArrayDimensions.size());
        if (n > 0)
        {
            if (n != 1)
            {
                throw Cm::Sym::Exception("arrays of arrays not supported", span);
            }
            derivedTypeSymbol->SetIrType(MakeIrType(finalBaseType, finalDerivations, span, derivedTypeSymbol->GetLastArrayDimension()));
        }
        else
        {
            derivedTypeSymbol->SetIrType(MakeIrType(finalBaseType, finalDerivations, span, 0));
            derivedTypeSymbol->SetDefaultIrValue(derivedTypeSymbol->GetIrType()->CreateDefaultValue());
        }
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
        return MakeDerivedType(derivations, derivedType->GetBaseType(), derivedType->GetArrayDimensions(), span);
    }
    Cm::Ast::DerivationList derivations;
    derivations.Add(Cm::Ast::Derivation::pointer);
    return MakeDerivedType(derivations, baseType, std::vector<int>(), span);
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
        return MakeDerivedType(derivations, derivedType->GetBaseType(), derivedType->GetArrayDimensions(), span);
    }
    Cm::Ast::DerivationList derivations;
    derivations.Add(Cm::Ast::Derivation::rvalueRef);
    return MakeDerivedType(derivations, baseType, std::vector<int>(), span);
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
        return MakeDerivedType(derivations, derivedType->GetBaseType(), derivedType->GetArrayDimensions(), span);
    }
    Cm::Ast::DerivationList derivations;
    derivations.Add(Cm::Ast::Derivation::reference);
    return MakeDerivedType(derivations, baseType, std::vector<int>(), span);
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
        return MakeDerivedType(derivations, derivedType->GetBaseType(), derivedType->GetArrayDimensions(), span);
    }
    Cm::Ast::DerivationList derivations;
    derivations.Add(Cm::Ast::Derivation::const_);
    derivations.Add(Cm::Ast::Derivation::reference);
    return MakeDerivedType(derivations, baseType, std::vector<int>(), span);
}

TypeSymbol* TypeRepository::MakeConstPointerType(TypeSymbol* baseType, const Span& span)
{
    Cm::Ast::DerivationList derivations;
    derivations.Add(Cm::Ast::Derivation::const_);
    derivations.Add(Cm::Ast::Derivation::pointer);
    return MakeDerivedType(derivations, baseType, std::vector<int>(), span);
}

TypeSymbol* TypeRepository::MakeConstCharPtrType(const Span& span)
{
    TypeId charTypeId = GetBasicTypeId(ShortBasicTypeId::charId);
    TypeSymbol* charType = GetType(charTypeId);
    Cm::Ast::DerivationList derivations;
    derivations.Add(Cm::Ast::Derivation::const_);
    derivations.Add(Cm::Ast::Derivation::pointer);
    return MakeDerivedType(derivations, charType, std::vector<int>(), span);
}

TypeSymbol* TypeRepository::MakeConstCharPtrPtrType(const Span& span)
{
    TypeId charTypeId = GetBasicTypeId(ShortBasicTypeId::charId);
    TypeSymbol* charType = GetType(charTypeId);
    Cm::Ast::DerivationList derivations;
    derivations.Add(Cm::Ast::Derivation::const_);
    derivations.Add(Cm::Ast::Derivation::pointer);
    derivations.Add(Cm::Ast::Derivation::pointer);
    return MakeDerivedType(derivations, charType, std::vector<int>(), span);
}

TypeSymbol* TypeRepository::MakeGenericPtrType(const Span& span)
{
    TypeId voidTypeId = GetBasicTypeId(ShortBasicTypeId::voidId);
    TypeSymbol* voidType = GetType(voidTypeId);
    Cm::Ast::DerivationList derivations;
    derivations.Add(Cm::Ast::Derivation::pointer);
    return MakeDerivedType(derivations, voidType, std::vector<int>(), span);
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
    templateTypeSymbol->SetParent(subjectType->Ns());
    templateTypeSymbol->MakeIrType();
    templateTypeSymbol->SetAccess(SymbolAccess::public_);
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
        if (derivation == Cm::Ast::Derivation::pointer || derivation == Cm::Ast::Derivation::array_)
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
        if (derivations.NumDerivations() == 0 && !derivedType->IsArrayType())
        {
            return derivedType->GetBaseType();
        }
        else
        {
            return MakeDerivedType(derivations, derivedType->GetBaseType(), derivedType->GetArrayDimensions(), derivedType->GetSpan());
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
        if (derivations.NumDerivations() == 0 && !derivedType->IsArrayType())
        {
            return derivedType->GetBaseType();
        }
        else
        {
            return MakeDerivedType(derivations, derivedType->GetBaseType(), derivedType->GetArrayDimensions(), derivedType->GetSpan());
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
