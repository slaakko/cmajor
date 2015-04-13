/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Llvm.Ir/Type.hpp>
#include <Llvm.Ir/Constant.hpp>
#include <Ir.Intf/Factory.hpp>
#include <map>
#include <stack>
#include <stdexcept>

namespace Llvm { 

VoidType::VoidType(): Ir::Intf::Type("void")
{
}

Ir::Intf::Type* VoidType::Clone() const
{
    return new VoidType();
}

Ir::Intf::Type* Void()
{
    return new VoidType();
}

LabelType::LabelType(): Ir::Intf::Type("label")
{
}

Ir::Intf::Type* LabelType::Clone() const
{
    return new LabelType();
}

Ir::Intf::Type* Label()
{
    return new LabelType();
}

IntegerType::IntegerType(const std::string& name_) : Ir::Intf::Type(name_)
{
}

I1Type::I1Type() : IntegerType("i1")
{
}

Ir::Intf::Type* I1Type::Clone() const
{
    return new I1Type();
}

Ir::Intf::Object* I1Type::CreateDefaultValue() const
{
    return False();
}

Ir::Intf::Type* I1()
{
    return new I1Type();
}

I8Type::I8Type() : IntegerType("i8")
{
}

Ir::Intf::Type* I8Type::Clone() const
{
    return new I8Type();
}

Ir::Intf::Object* I8Type::CreateDefaultValue() const
{
    return CreateI8Constant(0);
}

Ir::Intf::Object* I8Type::CreateMinusOne() const
{
    return CreateI8Constant(int8_t(-1));
}

Ir::Intf::Object* I8Type::CreatePlusOne() const
{
    return CreateI8Constant(int8_t(1));
}

Ir::Intf::Type* I8()
{
    return new I8Type();
}

I16Type::I16Type() : IntegerType("i16")
{
}

Ir::Intf::Type* I16Type::Clone() const
{
    return new I16Type();
}

Ir::Intf::Object* I16Type::CreateDefaultValue() const
{
    return CreateI16Constant(0);
}

Ir::Intf::Object* I16Type::CreateMinusOne() const
{
    return CreateI16Constant(int16_t(-1));
}

Ir::Intf::Object* I16Type::CreatePlusOne() const
{
    return CreateI16Constant(int16_t(1));
}

Ir::Intf::Type* I16()
{
    return new I16Type();
}

I32Type::I32Type() : IntegerType("i32")
{
}

I32Type::~I32Type()
{
}

Ir::Intf::Type* I32Type::Clone() const
{
    return new I32Type();
}

Ir::Intf::Object* I32Type::CreateDefaultValue() const
{
    return CreateI32Constant(0);
}

Ir::Intf::Object* I32Type::CreateMinusOne() const
{
    return CreateI32Constant(int32_t(-1));
}

Ir::Intf::Object* I32Type::CreatePlusOne() const
{
    return CreateI32Constant(int32_t(1));
}

Ir::Intf::Type* I32()
{
    return new I32Type();
}

I64Type::I64Type() : IntegerType("i64")
{
}

Ir::Intf::Type* I64Type::Clone() const
{
    return new I64Type();
}

Ir::Intf::Object* I64Type::CreateDefaultValue() const
{
    return CreateI64Constant(0);
}

Ir::Intf::Object* I64Type::CreateMinusOne() const
{
    return CreateI64Constant(int64_t(-1));
}

Ir::Intf::Object* I64Type::CreatePlusOne() const
{
    return CreateI64Constant(int64_t(1));
}

Ir::Intf::Type* I64()
{
    return new I64Type();
}

FloatingPointType::FloatingPointType(const std::string& name_): Ir::Intf::Type(name_)
{
}

FloatType::FloatType(): FloatingPointType("float")
{
}

Ir::Intf::Type* FloatType::Clone() const
{
    return new FloatType();
}

Ir::Intf::Object* FloatType::CreateDefaultValue() const
{ 
    return CreateFloatConstant(0.0);
}

Ir::Intf::Type* Float()
{
    return new FloatType();
}

DoubleType::DoubleType(): FloatingPointType("double")
{
}

Ir::Intf::Type* DoubleType::Clone() const
{
    return new DoubleType();
}

Ir::Intf::Object* DoubleType::CreateDefaultValue() const
{ 
    return CreateDoubleConstant(0.0); 
}

Ir::Intf::Type* Double()
{
    return new DoubleType();
}

std::string MakeArrayTypeName(Ir::Intf::Type* itemType, int size)
{
    std::string s(1, '[');
    s.append(std::to_string(size)).append(" x ").append(itemType->Name()).append(1, ']');
    return s;
}

ArrayType::ArrayType(Ir::Intf::Type* itemType_, int size_): Type(MakeArrayTypeName(itemType_, size_)), itemType(itemType_), size(size_)
{
	if (!itemType->Owned())
	{
		ownedItemType.reset(itemType);
		itemType->SetOwned();
	}
}

Ir::Intf::Type* ArrayType::Clone() const
{
	if (ownedItemType)
	{
		return new ArrayType(itemType->Clone(), size);
	}
	else
	{
		return new ArrayType(itemType, size);
	}
}

ArrayType* Array(Ir::Intf::Type* itemType, int size)
{
    return new ArrayType(itemType, size);
}

StringType::StringType(int size_): ArrayType(Ir::Intf::GetFactory()->GetI8(), size_)
{
}

Ir::Intf::Type* StringType::Clone() const
{
    return new StringType(Size());
}

Ir::Intf::Type* String(int size)
{
    return new StringType(size);
}

StructureType::StructureType(const std::vector<Ir::Intf::Type*>& elementTypes_) : Ir::Intf::Type("*structure*")
{
    std::string s = "{ ";
    bool first = true;
    for (Ir::Intf::Type* elementType : elementTypes_)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            s.append(", ");
        }
        s.append(elementType->Name());
        if (elementType->Owned())
        {
            throw std::runtime_error("structure element type already owned");
        }
        elementTypes.push_back(std::unique_ptr<Ir::Intf::Type>(elementType));
        elementType->SetOwned();
    }
    s.append(" }");
    SetName(s);
}

Ir::Intf::Type* StructureType::Clone() const
{
    std::vector<Ir::Intf::Type*> clonedElementTypes;
    for (const std::unique_ptr<Ir::Intf::Type>& elementType : elementTypes)
    {
        clonedElementTypes.push_back(elementType->Clone());
    }
    return new StructureType(clonedElementTypes);
}

Ir::Intf::Type* Structure(const std::vector<Ir::Intf::Type*>& elementTypes)
{
    return new StructureType(elementTypes);
}

PointerType::PointerType(Ir::Intf::Type* baseType_, uint8_t numPointers_) : Ir::Intf::Type(baseType_->Name() + std::string(numPointers_, '*')), baseType(baseType_), numPointers(numPointers_)
{
    if (!baseType->Owned())
    {
        baseType->SetOwned();
        ownedBaseType.reset(baseType);
    }
}

Ir::Intf::Type* PointerType::Clone() const
{
    return new PointerType(baseType->Clone(), numPointers);
}

bool PointerType::IsFunctionPointerType() const
{
    return baseType->IsFunctionType() && numPointers == 1;
}

bool PointerType::IsFunctionPtrPtrType() const
{
    return baseType->IsFunctionType() && numPointers == 2;
}

Ir::Intf::Object* PointerType::CreateDefaultValue() const
{
    return Null(const_cast<Ir::Intf::Type*>(static_cast<const Ir::Intf::Type*>(this)));
}

PointerType* MakePointerType(Ir::Intf::Type* baseType)
{
    if (baseType->IsPointerType())
    {
        PointerType* basePtrType = static_cast<PointerType*>(baseType);
        return new PointerType(basePtrType->BaseType()->Clone(), basePtrType->NumPointers() + 1);
    }
    return new PointerType(baseType, 1);
}

PointerType* MakePointerType(Ir::Intf::Type* baseType, int numPointers)
{
    if (baseType->IsPointerType())
    {
        PointerType* basePtrType = static_cast<PointerType*>(baseType);
        return new PointerType(basePtrType->BaseType(), basePtrType->NumPointers() + numPointers);
    }
    return new PointerType(baseType, numPointers);
}

RvalueRefType::RvalueRefType(Ir::Intf::Type* baseType_) : PointerType(baseType_, 1)
{
}

Ir::Intf::Type* RvalueRefType::Clone() const
{
    return new RvalueRefType(BaseType()->Clone());
}

Ir::Intf::Object* RvalueRefType::CreateDefaultValue() const
{
    return Null(const_cast<Ir::Intf::Type*>(static_cast<const Ir::Intf::Type*>(this)));
}

Ir::Intf::Type* RvalueRef(Ir::Intf::Type* baseType)
{
    return new RvalueRefType(baseType);
}

FunctionType::FunctionType(Ir::Intf::Type* returnType_, const std::vector<Ir::Intf::Type*>& parameterTypes_) : Ir::Intf::Type("*function*"), returnType(returnType_)
{
    if (returnType->Owned())
    {
        throw std::runtime_error("function return type already owned");
    }
    returnType->SetOwned();
    std::string s = returnType->Name();
    s.append(" (");
    bool first = true;
    for (Ir::Intf::Type* parameterType : parameterTypes_)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            s.append(", ");
        }
        s.append(parameterType->Name());
        if (parameterType->Owned())
        {
            throw std::runtime_error("function parameter type already owned");
        }
        parameterType->SetOwned();
        parameterTypes.push_back(std::unique_ptr<Ir::Intf::Type>(parameterType));
    }
    s.append(")");
    SetName(s);
}

Ir::Intf::Type* FunctionType::Clone() const
{
    std::vector<Ir::Intf::Type*> clonedParameterTypes;
    for (const std::unique_ptr<Ir::Intf::Type>& parameterType : parameterTypes)
    {
        clonedParameterTypes.push_back(parameterType->Clone());
    }
    return new FunctionType(returnType->Clone(), clonedParameterTypes);
}

Ir::Intf::Type* CreateFunctionType(Ir::Intf::Type* returnType, const std::vector<Ir::Intf::Type*>& parameterTypes)
{
    return new FunctionType(returnType, parameterTypes);
}

MetadataType::MetadataType(): Ir::Intf::Type("metadata")
{
}

Ir::Intf::Type* MetadataType::Clone() const
{
    return new MetadataType();
}

Ir::Intf::Type* Metadata()
{
    return new MetadataType();
}

TypeNameType::TypeNameType(const std::string& name_): Ir::Intf::Type("@" + name_), baseName(name_), isGlobal(true)
{
};

TypeNameType::TypeNameType(const std::string& name_, bool global): Ir::Intf::Type((global ? "@" : "%") + name_), baseName(name_), isGlobal(global)
{
}

Ir::Intf::Type* TypeNameType::Clone() const
{
    return new TypeNameType(baseName, isGlobal);
}

Ir::Intf::Type* TypeName(const std::string& name)
{
    return new TypeNameType(name);
}

Ir::Intf::Type* TypeName(const std::string& name, bool global)
{
    return new TypeNameType(name, global);
}

bool TypesEqual(Ir::Intf::Type* left, Ir::Intf::Type* right)
{
    return left->Name() == right->Name();
}

} // namespace Llvm
