/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <C.Ir/Type.hpp>
#include <C.Ir/Constant.hpp>
#include <Ir.Intf/Factory.hpp>
#include <map>
#include <stack>
#include <stdexcept>

namespace C {

VoidType::VoidType() : Ir::Intf::Type("void")
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

LabelType::LabelType() : Ir::Intf::Type("label")
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

UI8Type::UI8Type() : IntegerType("ui8")
{
}

Ir::Intf::Type* UI8Type::Clone() const
{
    return new UI8Type();
}

Ir::Intf::Object* UI8Type::CreateDefaultValue() const
{
    return CreateUI8Constant(uint8_t(0));
}

Ir::Intf::Object* UI8Type::CreateMinusOne() const
{
    return CreateUI8Constant(uint8_t(-1));
}

Ir::Intf::Object* UI8Type::CreatePlusOne() const
{
    return CreateUI8Constant(uint8_t(1));
}

Ir::Intf::Type* UI8()
{
    return new UI8Type();
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

UI16Type::UI16Type() : IntegerType("ui16")
{
}

Ir::Intf::Type* UI16Type::Clone() const
{
    return new UI16Type();
}

Ir::Intf::Object* UI16Type::CreateDefaultValue() const
{
    return CreateUI16Constant(uint16_t(0));
}

Ir::Intf::Object* UI16Type::CreateMinusOne() const
{
    return CreateUI16Constant(uint16_t(-1));
}

Ir::Intf::Object* UI16Type::CreatePlusOne() const
{
    return CreateUI16Constant(uint16_t(1));
}

Ir::Intf::Type* UI16()
{
    return new UI16Type();
}

I32Type::I32Type() : IntegerType("i32")
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

UI32Type::UI32Type() : IntegerType("ui32")
{
}

Ir::Intf::Type* UI32Type::Clone() const
{
    return new UI32Type();
}

Ir::Intf::Object* UI32Type::CreateDefaultValue() const
{
    return CreateUI32Constant(uint32_t(0));
}

Ir::Intf::Object* UI32Type::CreateMinusOne() const
{
    return CreateUI32Constant(uint32_t(-1));
}

Ir::Intf::Object* UI32Type::CreatePlusOne() const
{
    return CreateUI32Constant(uint32_t(1));
}

Ir::Intf::Type* UI32()
{
    return new UI32Type();
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

UI64Type::UI64Type() : IntegerType("ui64")
{
}

Ir::Intf::Type* UI64Type::Clone() const
{
    return new UI64Type();
}

Ir::Intf::Object* UI64Type::CreateDefaultValue() const
{
    return CreateUI64Constant(uint64_t(0));
}

Ir::Intf::Object* UI64Type::CreateMinusOne() const
{
    return CreateUI64Constant(uint64_t(-1));
}

Ir::Intf::Object* UI64Type::CreatePlusOne() const
{
    return CreateUI64Constant(uint64_t(1));
}

Ir::Intf::Type* UI64()
{
    return new UI64Type();
}

FloatingPointType::FloatingPointType(const std::string& name_) : Ir::Intf::Type(name_)
{
}

FloatType::FloatType() : FloatingPointType("float")
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

DoubleType::DoubleType() : FloatingPointType("double")
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

CharType::CharType() : Ir::Intf::Type("char")
{
}

Ir::Intf::Type* CharType::Clone() const
{
    return new CharType();
}

Ir::Intf::Object* CharType::CreateDefaultValue() const
{
    return CreateCharConstant('\0');
}

Ir::Intf::Type* Char()
{
    return new CharType();
}

std::string MakeArrayTypeName(Ir::Intf::Type* itemType, int size)
{
    std::string s(itemType->Name());
    return s;
}

ArrayType::ArrayType(Ir::Intf::Type* itemType_, int size_) : Type(MakeArrayTypeName(itemType_, size_)), itemType(itemType_), size(size_)
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

void ArrayType::GetFunctionPtrTypes(std::unordered_set<Ir::Intf::Type*>& functionPtrTypes) const
{
    itemType->GetFunctionPtrTypes(functionPtrTypes);
}

void ArrayType::ReplaceFunctionPtrTypes(const std::unordered_map<Ir::Intf::Type*, Ir::Intf::Type*>& tdfMap)
{
    itemType->ReplaceFunctionPtrTypes(tdfMap);
}

ArrayType* Array(Ir::Intf::Type* itemType, int size)
{
    return new ArrayType(itemType, size);
}

StringType::StringType(int size_) : Type("char*"), size(size_)
{
}

Ir::Intf::Type* StringType::Clone() const
{
    return new StringType(size);
}

Ir::Intf::Type* String(int size)
{
    return new StringType(size);
}

StructureType::StructureType(const std::string& tagName_, const std::vector<Ir::Intf::Type*>& elementTypes_, const std::vector<std::string>& elementNames_) : Ir::Intf::Type("*structure*"),
    tagName(tagName_), elementNames(elementNames_)
{
    std::string name("struct " + tagName + " {");
    int i = 0;
    for (Ir::Intf::Type* elementType : elementTypes_)
    {
        if (elementType->Owned())
        {
            throw std::runtime_error("structure element type already owned");
        }
        elementType->SetOwned();
        elementTypes.push_back(std::unique_ptr<Ir::Intf::Type>(elementType));
        if (elementType->GetBaseType()->IsFunctionType())
        {
            name.append(elementType->Prefix()).append(elementNames[i]).append(elementType->Suffix()).append(";");
        }
        else
        {
            name.append(elementType->Name());
            name.append(" ").append(elementNames[i]);
            if (elementType->IsArrayType())
            {
                ArrayType* arrayType = static_cast<ArrayType*>(elementType);
                name.append("[").append(std::to_string(arrayType->Size())).append("]");
            }
            name.append(";");
        }
        ++i;
    }
    name.append("}");
    SetName(name);
}

Ir::Intf::Type* StructureType::Clone() const
{
    std::vector<Ir::Intf::Type*> clonedElementTypes;
    for (const std::unique_ptr<Ir::Intf::Type>& elementType : elementTypes)
    {
        clonedElementTypes.push_back(elementType->Clone());
    }
    return new StructureType(tagName, clonedElementTypes, elementNames);
}

void StructureType::GetFunctionPtrTypes(std::unordered_set<Ir::Intf::Type*>& functionPtrTypes) const
{
    for (const std::unique_ptr<Ir::Intf::Type>& elementType : elementTypes)
    {
        elementType->GetFunctionPtrTypes(functionPtrTypes);
    }
}

void StructureType::ReplaceFunctionPtrTypes(const std::unordered_map<Ir::Intf::Type*, Ir::Intf::Type*>& tdfMap)
{
    for (const std::unique_ptr<Ir::Intf::Type>& elementType : elementTypes)
    {
        elementType->ReplaceFunctionPtrTypes(tdfMap);
    }
}

Ir::Intf::Type* Structure(const std::string& tagName, const std::vector<Ir::Intf::Type*>& elementTypes, const std::vector<std::string>& elementNames)
{
    return new StructureType(tagName, elementTypes, elementNames);
}

PointerType::PointerType(Ir::Intf::Type* baseType_, uint8_t numPointers_) : Ir::Intf::Type(baseType_->Name() + std::string(numPointers_, '*')), baseType(baseType_), numPointers(numPointers_)
{
    if (!baseType->Owned())
    {
        baseType->SetOwned();
        ownedBaseType.reset(baseType);
    }
    if (GetBaseType()->IsFunctionType())
    {
        SetName(Prefix() + Suffix());
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

void PointerType::GetFunctionPtrTypes(std::unordered_set<Ir::Intf::Type*>& functionPtrTypes) const
{
    if (GetBaseType()->IsFunctionType())
    {
        functionPtrTypes.insert(const_cast<PointerType*>(this));
    }
}

void PointerType::ReplaceFunctionPtrTypes(const std::unordered_map<Ir::Intf::Type*, Ir::Intf::Type*>& tdfMap)
{
    std::unordered_map<Ir::Intf::Type*, Ir::Intf::Type*>::const_iterator i = tdfMap.find(this);
    if (i != tdfMap.cend())
    {
        SetName(i->second->BaseName());
    }
}

std::string PointerType::Prefix() const
{
    if (GetBaseType()->IsFunctionType())
    {
        FunctionType* funType = static_cast<FunctionType*>(GetBaseType());
        return funType->ReturnType()->Name() + " (" + std::string(numPointers, '*');
    }
    return Ir::Intf::Type::Prefix();
}

std::string PointerType::Suffix() const
{
    if (GetBaseType()->IsFunctionType())
    {
        std::string parameterList = ")(";
        FunctionType* funType = static_cast<FunctionType*>(GetBaseType());
        bool first = true;
        for (const std::unique_ptr<Ir::Intf::Type>& paramType : funType->ParameterTypes())
        {
            if (first)
            {
                first = false;
            }
            else
            {
                parameterList.append(", ");
            }
            parameterList.append(paramType->Name());
        }
        parameterList.append(")");
        return parameterList;
    }
    return Ir::Intf::Type::Suffix();
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
        return new PointerType(basePtrType->GetBaseType()->Clone(), basePtrType->NumPointers() + 1);
    }
    return new PointerType(baseType, 1);
}

PointerType* MakePointerType(Ir::Intf::Type* baseType, int numPointers)
{
    if (baseType->IsPointerType())
    {
        PointerType* basePtrType = static_cast<PointerType*>(baseType);
        return new PointerType(basePtrType->GetBaseType()->Clone(), basePtrType->NumPointers() + numPointers);
    }
    return new PointerType(baseType, numPointers);
}

RvalueRefType::RvalueRefType(Ir::Intf::Type* baseType_) : PointerType(baseType_, 1)
{
}

RvalueRefType::RvalueRefType(Ir::Intf::Type* baseType_, uint8_t numPointers_) : PointerType(baseType_, numPointers_)
{
}

Ir::Intf::Type* RvalueRefType::Clone() const
{
    return new RvalueRefType(GetBaseType()->Clone(), NumPointers());
}

Ir::Intf::Object* RvalueRefType::CreateDefaultValue() const
{
    return Null(const_cast<Ir::Intf::Type*>(static_cast<const Ir::Intf::Type*>(this)));
}

Ir::Intf::Type* RvalueRef(Ir::Intf::Type* baseType)
{
    if (baseType->IsPointerType())
    {
        PointerType* basePtrType = static_cast<PointerType*>(baseType);
        return new RvalueRefType(basePtrType->GetBaseType()->Clone(), basePtrType->NumPointers() + 1);
    }
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

MetadataType::MetadataType() : Ir::Intf::Type("metadata")
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

TypeNameType::TypeNameType(const std::string& name_) : Ir::Intf::Type(name_), baseName(name_), isGlobal(true)
{
};

TypeNameType::TypeNameType(const std::string& name_, bool global) : Ir::Intf::Type(name_), baseName(name_), isGlobal(global)
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

Typedef::Typedef(const std::string& name_, Ir::Intf::Type* type_) :
    Ir::Intf::Type("typedef " + type_->Prefix() + name_ + type_->Suffix()), baseName(name_), type(type_)
{
    if (!type->Owned())
    {
        type->SetOwned();
        ownedType.reset(type);
    }
}

Ir::Intf::Type* Typedef::Clone() const
{
    return new Typedef(baseName, type);
}

Ir::Intf::Type* CreateTypedef(const std::string& name, Ir::Intf::Type* type)
{
    return new Typedef(name, type);
}

bool TypesEqual(Ir::Intf::Type* left, Ir::Intf::Type* right)
{
    return left->Name() == right->Name();
}

} // namespace C
