/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef C_IR_TYPE_INCLUDED
#define C_IR_TYPE_INCLUDED
#include <Ir.Intf/Type.hpp>
#include <vector>
#include <memory>

namespace C {

class VoidType : public Ir::Intf::Type
{
public:
    VoidType();
    Ir::Intf::Type* Clone() const override;
    bool IsVoidType() const override { return true; }
};

Ir::Intf::Type* Void();

class LabelType : public Ir::Intf::Type
{
public:
    LabelType();
    Ir::Intf::Type* Clone() const override;
    bool IsLabelType() const override { return true; }
};

Ir::Intf::Type* Label();

class IntegerType : public Ir::Intf::Type
{
public:
    IntegerType(const std::string& name_);
    bool IsIntegerType() const override { return true; }
};

class I1Type : public IntegerType
{
public:
    I1Type();
    Ir::Intf::Type* Clone() const override;
    Ir::Intf::Object* CreateDefaultValue() const override;
};

Ir::Intf::Type* I1();

class I8Type : public IntegerType
{
public:
    I8Type();
    Ir::Intf::Type* Clone() const override;
    Ir::Intf::Object* CreateDefaultValue() const override;
    Ir::Intf::Object* CreateMinusOne() const override;
    Ir::Intf::Object* CreatePlusOne() const override;
};

Ir::Intf::Type* I8();

class UI8Type : public IntegerType
{
public:
    UI8Type();
    Ir::Intf::Type* Clone() const override;
    Ir::Intf::Object* CreateDefaultValue() const override;
    Ir::Intf::Object* CreateMinusOne() const override;
    Ir::Intf::Object* CreatePlusOne() const override;
};

Ir::Intf::Type* UI8();

class I16Type : public IntegerType
{
public:
    I16Type();
    Ir::Intf::Type* Clone() const override;
    Ir::Intf::Object* CreateDefaultValue() const override;
    Ir::Intf::Object* CreateMinusOne() const override;
    Ir::Intf::Object* CreatePlusOne() const override;
};

Ir::Intf::Type* I16();

class UI16Type : public IntegerType
{
public:
    UI16Type();
    Ir::Intf::Type* Clone() const override;
    Ir::Intf::Object* CreateDefaultValue() const override;
    Ir::Intf::Object* CreateMinusOne() const override;
    Ir::Intf::Object* CreatePlusOne() const override;
};

Ir::Intf::Type* UI16();

class I32Type : public IntegerType
{
public:
    I32Type();
    Ir::Intf::Type* Clone() const override;
    Ir::Intf::Object* CreateDefaultValue() const override;
    Ir::Intf::Object* CreateMinusOne() const override;
    Ir::Intf::Object* CreatePlusOne() const override;
};

Ir::Intf::Type* I32();

class UI32Type : public IntegerType
{
public:
    UI32Type();
    Ir::Intf::Type* Clone() const override;
    Ir::Intf::Object* CreateDefaultValue() const override;
    Ir::Intf::Object* CreateMinusOne() const override;
    Ir::Intf::Object* CreatePlusOne() const override;
};

Ir::Intf::Type* UI32();

class I64Type : public IntegerType
{
public:
    I64Type();
    Ir::Intf::Type* Clone() const override;
    Ir::Intf::Object* CreateDefaultValue() const override;
    Ir::Intf::Object* CreateMinusOne() const override;
    Ir::Intf::Object* CreatePlusOne() const override;
};

Ir::Intf::Type* I64();

class UI64Type : public IntegerType
{
public:
    UI64Type();
    Ir::Intf::Type* Clone() const override;
    Ir::Intf::Object* CreateDefaultValue() const override;
    Ir::Intf::Object* CreateMinusOne() const override;
    Ir::Intf::Object* CreatePlusOne() const override;
};

Ir::Intf::Type* UI64();

class FloatingPointType : public Ir::Intf::Type
{
public:
    FloatingPointType(const std::string& name_);
    bool IsFloatingPointType() const override { return true; }
};

class FloatType : public FloatingPointType
{
public:
    FloatType();
    Ir::Intf::Type* Clone() const override;
    Ir::Intf::Object* CreateDefaultValue() const override;
};

Ir::Intf::Type* Float();

class DoubleType : public FloatingPointType
{
public:
    DoubleType();
    Ir::Intf::Type* Clone() const override;
    Ir::Intf::Object* CreateDefaultValue() const override;
};

Ir::Intf::Type* Double();

class CharType : public Ir::Intf::Type
{
public:
    CharType();
    Ir::Intf::Type* Clone() const override;
    Ir::Intf::Object* CreateDefaultValue() const override;
};

Ir::Intf::Type* Char();

class ArrayType : public Ir::Intf::Type
{
public:
    ArrayType(Ir::Intf::Type* itemType_, int size_);
    Ir::Intf::Type* ItemType() const { return itemType; }
    int Size() const { return size; }
    Ir::Intf::Type* Clone() const override;
    bool IsArrayType() const override { return true; }
    void GetFunctionPtrTypes(std::unordered_set<Ir::Intf::Type*>& functionPtrTypes) const override;
    void ReplaceFunctionPtrTypes(const std::unordered_map<Ir::Intf::Type*, Ir::Intf::Type*>& tdfMap) override;
private:
    Ir::Intf::Type* itemType;
    std::unique_ptr<Ir::Intf::Type> ownedItemType;
    int size;
};

ArrayType* Array(Ir::Intf::Type* itemType, int size);

class StringType : public Ir::Intf::Type
{
public:
    StringType(int size_);
    Ir::Intf::Type* Clone() const override;
private:
    int size;
};

Ir::Intf::Type* String(int size);

class StructureType : public Ir::Intf::Type
{
public:
    StructureType(const std::string& tagName_, const std::vector<Ir::Intf::Type*>& elementTypes_, const std::vector<std::string>& elementNames_);
    int Size() const { return int(elementTypes.size()); }
    Ir::Intf::Type* operator[](int index) const { return elementTypes[index].get(); }
    const std::vector<std::unique_ptr<Ir::Intf::Type>>& ElementTypes() const { return elementTypes; }
    const std::vector<std::string>& ElementNames() const { return elementNames; }
    const std::string& GetTagName() const override { return tagName; }
    Ir::Intf::Type* Clone() const override;
    void GetFunctionPtrTypes(std::unordered_set<Ir::Intf::Type*>& functionPtrTypes) const override;
    void ReplaceFunctionPtrTypes(const std::unordered_map<Ir::Intf::Type*, Ir::Intf::Type*>& tdfMap) override;
private:
    std::string tagName;
    std::vector<std::unique_ptr<Ir::Intf::Type>> elementTypes;
    std::vector<std::string> elementNames;
};

Ir::Intf::Type* Structure(const std::string& tagName, const std::vector<Ir::Intf::Type*>& elementTypes, const std::vector<std::string>& elementNames);

class PointerType : public Ir::Intf::Type
{
public:
    PointerType(Ir::Intf::Type* baseType_, uint8_t numPointers_);
    Ir::Intf::Type* GetBaseType() const override { return baseType; }
    bool IsPointerType() const override { return true; }
    bool IsFunctionPointerType() const override;
    bool IsFunctionPtrPtrType() const override;
    void GetFunctionPtrTypes(std::unordered_set<Ir::Intf::Type*>& functionPtrTypes) const override;
    void ReplaceFunctionPtrTypes(const std::unordered_map<Ir::Intf::Type*, Ir::Intf::Type*>& tdfMap) override;
    Ir::Intf::Type* Clone() const override;
    Ir::Intf::Object* CreateDefaultValue() const override;
    uint8_t NumPointers() const override { return numPointers; }
    std::string Prefix() const override;
    std::string Suffix() const override;
private:
    Ir::Intf::Type* baseType;
    std::unique_ptr<Ir::Intf::Type> ownedBaseType;
    uint8_t numPointers;
};

PointerType* MakePointerType(Ir::Intf::Type* baseType);
PointerType* MakePointerType(Ir::Intf::Type* baseType, int numPointers);

class RvalueRefType : public PointerType
{
public:
    RvalueRefType(Ir::Intf::Type* baseType_);
    RvalueRefType(Ir::Intf::Type* baseType_, uint8_t numPointers_);
    bool IsRvalueRefType() const override { return true; }
    Ir::Intf::Type* Clone() const override;
    Ir::Intf::Object* CreateDefaultValue() const override;
};

Ir::Intf::Type* RvalueRef(Ir::Intf::Type* baseType);

class FunctionType : public Ir::Intf::Type
{
public:
    FunctionType(Ir::Intf::Type* returnType_, const std::vector<Ir::Intf::Type*>& parameterTypes_);
    Ir::Intf::Type* ReturnType() const { return returnType.get(); }
    const std::vector<std::unique_ptr<Ir::Intf::Type>>& ParameterTypes() const { return parameterTypes; }
    Ir::Intf::Type* Clone() const override;
    bool IsFunctionType() const override { return true; }
private:
    std::unique_ptr<Ir::Intf::Type> returnType;
    std::vector<std::unique_ptr<Ir::Intf::Type>> parameterTypes;
};

Ir::Intf::Type* CreateFunctionType(Ir::Intf::Type* returnType, const std::vector<Ir::Intf::Type*>& parameterTypes);

class MetadataType : public Ir::Intf::Type
{
public:
    MetadataType();
    Ir::Intf::Type* Clone() const override;
};

Ir::Intf::Type* Metadata();

class TypeNameType : public Ir::Intf::Type
{
public:
    TypeNameType(const std::string& name_);
    TypeNameType(const std::string& name_, bool global);
    virtual bool IsTypeNameType() const { return true; }
    Ir::Intf::Type* Clone() const override;
    const std::string& BaseName() const override { return baseName; }
private:
    std::string baseName;
    bool isGlobal;
};

Ir::Intf::Type* TypeName(const std::string& name);
Ir::Intf::Type* TypeName(const std::string& name, bool global);

class Typedef : public Ir::Intf::Type
{
public:
    Typedef(const std::string& name_, Ir::Intf::Type* type_);
    Ir::Intf::Type* Clone() const override;
    const std::string& BaseName() const override { return baseName; }
private:
    std::string baseName;
    Ir::Intf::Type* type;
    std::unique_ptr<Ir::Intf::Type> ownedType;
};

Ir::Intf::Type* CreateTypedef(const std::string& name, Ir::Intf::Type* type);

bool TypesEqual(Ir::Intf::Type* left, Ir::Intf::Type* right);

} // namespace C

#endif // C_IR_TYPE_INCLUDED
