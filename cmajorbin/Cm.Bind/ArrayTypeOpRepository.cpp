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
    if (result.MainObject()->GetType()->Name() == "i8*")
    {
        emitter.Emit(Cm::IrIntf::MemSet(result.MainObject(), zero, len, 1, false));
    }
    else
    {
        Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 1);
        emitter.Own(i8Ptr);
        Ir::Intf::Object* dest = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
        emitter.Emit(Cm::IrIntf::Bitcast(result.MainObject()->GetType(), dest, result.MainObject(), i8Ptr));
        emitter.Emit(Cm::IrIntf::MemSet(dest, zero, len, 1, false));
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
}

ArrayTypeOpRepository::ArrayTypeOpRepository(Cm::BoundTree::BoundCompileUnit& compileUnit_) : compileUnit(compileUnit_)
{
    arrayTypeOpFunGroupMap["@constructor"] = &arrayTypeConstructorGroup;
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
