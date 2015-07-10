/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/ArrayTypeOpRepository.hpp>
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Bind {

class PrimitiveArrayTypeDefaultConstructor : public Cm::Core::BasicTypeOp
{
public:
    PrimitiveArrayTypeDefaultConstructor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result) override;
    bool IsPrimitiveArrayTypeDefaultConstructor() const override { return true; }
};

PrimitiveArrayTypeDefaultConstructor::PrimitiveArrayTypeDefaultConstructor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("@constructor");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(Type(), Cm::Parsing::Span()));
    AddSymbol(thisParam);
    ComputeName();
}

void PrimitiveArrayTypeDefaultConstructor::Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result)
{
    Ir::Intf::Object* zero = Ir::Intf::GetFactory()->GetI8()->CreateDefaultValue();
    emitter.Own(zero);
    Ir::Intf::Object* dim = Cm::IrIntf::CreateI64Constant(Type()->GetLastArrayDimension());
    emitter.Own(dim);
    Ir::Intf::Object* size = Cm::IrIntf::SizeOf(emitter, Type()->GetBaseType()->GetIrType());
    emitter.Own(size);
    Ir::Intf::Object* len = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI64());
    emitter.Own(len);
    emitter.Emit(Cm::IrIntf::Mul(Ir::Intf::GetFactory()->GetI64(), len, dim, size));
    Ir::Intf::Object* arrayObject = result.MainObject();
    Ir::Intf::Type* arrayObjectType = arrayObject->GetType();
    if (result.MemberVar())
    {
        Ir::Intf::Type* arrayType = Cm::IrIntf::Array(Type()->GetBaseType()->GetIrType(), Type()->GetLastArrayDimension());
        emitter.Own(arrayType);
        Ir::Intf::Type* ptrArrayType = Cm::IrIntf::Pointer(arrayType, 1);
        emitter.Own(ptrArrayType);
        arrayObject->SetType(ptrArrayType);
        arrayObjectType = ptrArrayType;
    }
    if (arrayObjectType->Name() == "i8*")
    {
        emitter.Emit(Cm::IrIntf::MemSet(arrayObject, zero, len, 1, false));
    }
    else
    {
        Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 1);
        emitter.Own(i8Ptr);
        Ir::Intf::Object* dest = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
        emitter.Own(dest);
        emitter.Emit(Cm::IrIntf::Bitcast(arrayObjectType, dest, arrayObject, i8Ptr));
        emitter.Emit(Cm::IrIntf::MemSet(dest, zero, len, 1, false));
    }
}

class PrimitiveArrayTypeCopyConstructor : public Cm::Core::BasicTypeOp
{
public:
    PrimitiveArrayTypeCopyConstructor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result) override;
    bool IsPrimitiveArrayTypeCopyConstructorOrCopyAssignment() const override { return true; }
};

PrimitiveArrayTypeCopyConstructor::PrimitiveArrayTypeCopyConstructor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("@constructor");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(Type(), Cm::Parsing::Span()));
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* thatParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "that"));
    thatParam->SetType(typeRepository.MakeConstReferenceType(Type(), Cm::Parsing::Span()));
    AddSymbol(thatParam);
    ComputeName();
}

void PrimitiveArrayTypeCopyConstructor::Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result)
{
    Ir::Intf::Object* dim = Cm::IrIntf::CreateI64Constant(Type()->GetLastArrayDimension());
    emitter.Own(dim);
    Ir::Intf::Object* size = Cm::IrIntf::SizeOf(emitter, Type()->GetBaseType()->GetIrType());
    emitter.Own(size);
    Ir::Intf::Object* len = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI64());
    emitter.Own(len);
    emitter.Emit(Cm::IrIntf::Mul(Ir::Intf::GetFactory()->GetI64(), len, dim, size));
    Ir::Intf::Object* arrayObject = result.MainObject();
    Ir::Intf::Object* arg1 = result.Arg1();
    Ir::Intf::Type* arrayObjectType = arrayObject->GetType();
    if (result.MemberVar())
    {
        Ir::Intf::Type* arrayType = Cm::IrIntf::Array(Type()->GetBaseType()->GetIrType(), Type()->GetLastArrayDimension());
        emitter.Own(arrayType);
        Ir::Intf::Type* ptrArrayType = Cm::IrIntf::Pointer(arrayType, 1);
        emitter.Own(ptrArrayType);
        arrayObject->SetType(ptrArrayType);
        arrayObjectType = ptrArrayType;
        arg1->SetType(ptrArrayType);
    }
    if (arrayObjectType->Name() == "i8*")
    {
        emitter.Emit(Cm::IrIntf::MemCopy(arrayObject, arg1, len, 1, false));
    }
    else
    {
        Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 1);
        emitter.Own(i8Ptr);
        Ir::Intf::Object* dest = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
        emitter.Own(dest);
        Ir::Intf::Object* source = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
        emitter.Own(source);
        emitter.Emit(Cm::IrIntf::Bitcast(arrayObjectType, dest, arrayObject, i8Ptr));
        emitter.Emit(Cm::IrIntf::Bitcast(arrayObjectType, source, arg1, i8Ptr));
        emitter.Emit(Cm::IrIntf::MemCopy(dest, source, len, 1, false));
    }
}

class PrimitiveArrayTypeCopyAssignment : public Cm::Core::BasicTypeOp
{
public:
    PrimitiveArrayTypeCopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result) override;
    bool IsPrimitiveArrayTypeCopyConstructorOrCopyAssignment() const override { return true; }
};

PrimitiveArrayTypeCopyAssignment::PrimitiveArrayTypeCopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator=");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(Type(), Cm::Parsing::Span()));
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* thatParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "that"));
    thatParam->SetType(typeRepository.MakeConstReferenceType(Type(), Cm::Parsing::Span()));
    AddSymbol(thatParam);
    ComputeName();
}

void PrimitiveArrayTypeCopyAssignment::Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result)
{
    Ir::Intf::Object* dim = Cm::IrIntf::CreateI64Constant(Type()->GetLastArrayDimension());
    emitter.Own(dim);
    Ir::Intf::Object* size = Cm::IrIntf::SizeOf(emitter, Type()->GetBaseType()->GetIrType());
    emitter.Own(size);
    Ir::Intf::Object* len = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI64());
    emitter.Own(len);
    emitter.Emit(Cm::IrIntf::Mul(Ir::Intf::GetFactory()->GetI64(), len, dim, size));
    Ir::Intf::Object* arrayObject = result.MainObject();
    Ir::Intf::Object* arg1 = result.Arg1();
    Ir::Intf::Type* arrayObjectType = arrayObject->GetType();
    if (result.MemberVar())
    {
        Ir::Intf::Type* arrayType = Cm::IrIntf::Array(Type()->GetBaseType()->GetIrType(), Type()->GetLastArrayDimension());
        emitter.Own(arrayType);
        Ir::Intf::Type* ptrArrayType = Cm::IrIntf::Pointer(arrayType, 1);
        emitter.Own(ptrArrayType);
        arrayObject->SetType(ptrArrayType);
        arrayObjectType = ptrArrayType;
        arg1->SetType(ptrArrayType);
    }
    if (arrayObjectType->Name() == "i8*")
    {
        emitter.Emit(Cm::IrIntf::MemCopy(arrayObject, arg1, len, 1, false));
    }
    else
    {
        Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 1);
        emitter.Own(i8Ptr);
        Ir::Intf::Object* dest = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
        emitter.Own(dest);
        Ir::Intf::Object* source = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
        emitter.Own(source);
        emitter.Emit(Cm::IrIntf::Bitcast(arrayObjectType, dest, arrayObject, i8Ptr));
        emitter.Emit(Cm::IrIntf::Bitcast(arrayObjectType, source, arg1, i8Ptr));
        emitter.Emit(Cm::IrIntf::MemCopy(dest, source, len, 1, false));
    }
}

Cm::Sym::FunctionSymbol* ArrayTypeOpCache::GetDefaultConstructor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!defaultConstructor)
    {
        if (type->IsPrimitiveSingleDimensionArrayType())
        {
            defaultConstructor.reset(new PrimitiveArrayTypeDefaultConstructor(typeRepository, type));
        }
    }
    return defaultConstructor.get();
}

Cm::Sym::FunctionSymbol* ArrayTypeOpCache::GetCopyConstructor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!copyConstructor)
    {
        if (type->IsPrimitiveSingleDimensionArrayType())
        {
            copyConstructor.reset(new PrimitiveArrayTypeCopyConstructor(typeRepository, type));
        }
    }
    return copyConstructor.get();
}

Cm::Sym::FunctionSymbol* ArrayTypeOpCache::GetCopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!copyAssignment)
    {
        if (type->IsPrimitiveSingleDimensionArrayType())
        {
            copyAssignment.reset(new PrimitiveArrayTypeCopyAssignment(typeRepository, type));
        }
    }
    return copyAssignment.get();
}

ArrayTypeOpFunGroup::~ArrayTypeOpFunGroup()
{
}

void ArrayTypeConstructorGroup::CollectViableFunctions(ArrayTypeCacheMap& cacheMap, Cm::Sym::TypeSymbol* arrayType, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const Cm::Parsing::Span& span,
    int arity, const std::vector<Cm::Core::Argument>& arguments, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity == 1)
    {
        ArrayTypeOpCache& cache = cacheMap[arrayType];
        Cm::Sym::FunctionSymbol* defaultConstructor = cache.GetDefaultConstructor(boundCompileUnit.SymbolTable().GetTypeRepository(), arrayType);
        if (defaultConstructor)
        {
            viableFunctions.insert(defaultConstructor);
        }
    }
    else if (arity == 2)
    {
        Cm::Sym::TypeSymbol* plainArg1Type = boundCompileUnit.SymbolTable().GetTypeRepository().MakePlainType(arguments[1].Type());
        if (Cm::Sym::TypesEqual(arrayType, plainArg1Type))
        {
            ArrayTypeOpCache& cache = cacheMap[arrayType];
            Cm::Sym::FunctionSymbol* copyConstructor = cache.GetCopyConstructor(boundCompileUnit.SymbolTable().GetTypeRepository(), arrayType);
            if (copyConstructor)
            {
                viableFunctions.insert(copyConstructor);
            }
        }
    }
}

void ArrayTypeAssignmentGroup::CollectViableFunctions(ArrayTypeCacheMap& cacheMap, Cm::Sym::TypeSymbol* arrayType, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const Cm::Parsing::Span& span,
    int arity, const std::vector<Cm::Core::Argument>& arguments, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) 
{
    if (arity == 2)
    {
        Cm::Sym::TypeSymbol* plainArg1Type = boundCompileUnit.SymbolTable().GetTypeRepository().MakePlainType(arguments[1].Type());
        if (Cm::Sym::TypesEqual(arrayType, plainArg1Type))
        {
            ArrayTypeOpCache& cache = cacheMap[arrayType];
            Cm::Sym::FunctionSymbol* copyAssignment = cache.GetCopyAssignment(boundCompileUnit.SymbolTable().GetTypeRepository(), arrayType);
            if (copyAssignment)
            {
                viableFunctions.insert(copyAssignment);
            }
        }
    }
}

ArrayTypeOpRepository::ArrayTypeOpRepository(Cm::BoundTree::BoundCompileUnit& compileUnit_) : compileUnit(compileUnit_)
{
    arrayTypeOpFunGroupMap["@constructor"] = &arrayTypeConstructorGroup;
    arrayTypeOpFunGroupMap["operator="] = &arrayTypeAssignmentGroup;
}

void ArrayTypeOpRepository::CollectViableFunctions(const std::string& groupName, int arity, const std::vector<Cm::Core::Argument>& arguments, const Cm::Parsing::Span& span,
    std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity < 1 || arity > 2) return;
    Cm::Sym::TypeSymbol* leftArgType = arguments[0].Type();
    if (!leftArgType->IsArrayType() || leftArgType->IsReferenceType() || leftArgType->IsRvalueRefType() || !leftArgType->IsPointerType() || leftArgType->GetPointerCountAfterArray() != 1) return;
    Cm::Sym::DerivedTypeSymbol* ptrArrayType = static_cast<Cm::Sym::DerivedTypeSymbol*>(leftArgType);
    if (!Cm::Sym::HasPointerToArrayDerivation(ptrArrayType->Derivations())) return;
    Cm::Ast::DerivationList derivations = ptrArrayType->Derivations();
    derivations.RemoveLastPointer();
    Cm::Sym::TypeSymbol* arrayType = compileUnit.SymbolTable().GetTypeRepository().MakeDerivedType(derivations, ptrArrayType->GetBaseType(), ptrArrayType->GetArrayDimensions(), span);
    ArrayTypeOpFunGroupMapIt i = arrayTypeOpFunGroupMap.find(groupName);
    if (i != arrayTypeOpFunGroupMap.end())
    {
        ArrayTypeOpFunGroup* group = i->second;
        group->CollectViableFunctions(cacheMap, arrayType, compileUnit, span, arity, arguments, viableFunctions);
    }
}

} } // namespace Cm::Bind
