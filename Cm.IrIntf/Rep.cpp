/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace IrIntf {

std::string GetExCodeVarName()
{
    return GetBackEndImpl()->GetExCodeVarName();
}

Ir::Intf::LabelObject* CreateTempLabel(int tempLabelCounter)
{
    return GetBackEndImpl()->CreateTempLabel(tempLabelCounter);
}

std::string CreateTempVarName(int tempVarCounter)
{
    return GetBackEndImpl()->CreateTempVarName(tempVarCounter);
}

Ir::Intf::LabelObject* CreateNextLocalLabel()
{
    return GetBackEndImpl()->CreateNextLocalLabel();
}

Ir::Intf::LabelObject* CreateLabel()
{
    return GetBackEndImpl()->CreateLabel();
}

Ir::Intf::LabelObject* CreateLabel(const std::string& label)
{
    return GetBackEndImpl()->CreateLabel(label);
}

Ir::Intf::LabelObject* CreateLandingPadLabel(int index)
{
    return GetBackEndImpl()->CreateLandingPadLabel(index);
}

Ir::Intf::RegVar* CreateTemporaryRegVar(Ir::Intf::Type* type)
{
    return GetBackEndImpl()->CreateTemporaryRegVar(type);
}

Ir::Intf::Type* I1()
{
    return GetBackEndImpl()->I1();
}

Ir::Intf::Type* I8()
{
    return GetBackEndImpl()->I8();
}

Ir::Intf::Type* I16()
{
    return GetBackEndImpl()->I16();
}

Ir::Intf::Type* I32()
{
    return GetBackEndImpl()->I32();
}

Ir::Intf::Type* I64()
{
    return GetBackEndImpl()->I64();
}

Ir::Intf::Type* UI8()
{
    return GetBackEndImpl()->UI8();
}

Ir::Intf::Type* UI16()
{
    return GetBackEndImpl()->UI16();
}

Ir::Intf::Type* UI32()
{
    return GetBackEndImpl()->UI32();
}

Ir::Intf::Type* UI64()
{
    return GetBackEndImpl()->UI64();
}

Ir::Intf::Type* Float()
{
    return GetBackEndImpl()->Float();
}

Ir::Intf::Type* Double()
{
    return GetBackEndImpl()->Double();
}

Ir::Intf::Type* Void()
{
    return GetBackEndImpl()->Void();
}

Ir::Intf::Type* Char()
{
    return GetBackEndImpl()->Char();
}

Ir::Intf::Type* RvalueRef(Ir::Intf::Type* baseType)
{
    return GetBackEndImpl()->RvalueRef(baseType);
}

Ir::Intf::Type* Pointer(Ir::Intf::Type* baseType, int numPointers)
{
    return GetBackEndImpl()->Pointer(baseType, numPointers);
}

Ir::Intf::Type* Structure(const std::string& tagName, const std::vector<Ir::Intf::Type*>& elementTypes, const std::vector<std::string>& elementNames)
{
    return GetBackEndImpl()->Structure(tagName, elementTypes, elementNames);
}

Ir::Intf::Type* Array(Ir::Intf::Type* itemType, int size)
{
    return GetBackEndImpl()->Array(itemType, size);
}

Ir::Intf::Type* CreateFunctionType(Ir::Intf::Type* returnType, const std::vector<Ir::Intf::Type*>& parameterTypes)
{
    return GetBackEndImpl()->CreateFunctionType(returnType, parameterTypes);
}

int delegateTypeCounter = 0;

std::string CreateDelegateTypeName()
{
    return "__delegate_type_" + std::to_string(delegateTypeCounter++);
}

Ir::Intf::Type* CreateTypedef(const std::string& name, Ir::Intf::Type* type)
{
    return GetBackEndImpl()->CreateTypedef(name, type);
}

Ir::Intf::Function* CreateFunction(const std::string& name, Ir::Intf::Type* returnType, const std::vector<Ir::Intf::Parameter*>& parameters)
{
    return GetBackEndImpl()->CreateFunction(name, returnType, parameters);
}

Ir::Intf::Function* CreateDoNothingFunction()
{
    return GetBackEndImpl()->CreateDoNothingFunction();
}

Ir::Intf::Function* CreateDbgDeclareFunction()
{
    return GetBackEndImpl()->CreateDbgDeclareFunction();
}

void ResetLocalLabelCounter()
{
    GetBackEndImpl()->ResetLocalLabelCounter();
}

void ResetTemporaryCounter()
{
    GetBackEndImpl()->ResetTemporaryCounter();
}

std::string MakeAssemblyName(const std::string& name)
{
    return GetBackEndImpl()->MakeAssemblyName(name);
}

std::string MakeClassNameAssemblyName(const std::string& fullClassName)
{
    return GetBackEndImpl()->MakeClassNameAssemblyName(fullClassName);
}

Ir::Intf::Type* CreateClassTypeName(const std::string& fullClassName)
{
    return GetBackEndImpl()->CreateClassTypeName(fullClassName);
}

Ir::Intf::MetadataStringPtr CreateMetadataString(const std::string& s)
{
    return GetBackEndImpl()->CreateMetadataString(s);
}

Ir::Intf::MetadataNode* CreateMetadataNode(int id)
{
    return GetBackEndImpl()->CreateMetadataNode(id);
}

int GetDebugTagLexicalBlock()
{
    return GetBackEndImpl()->GetDebugTagLexicalBlock();
}

int GetDebugTagCompileUnit()
{
    return GetBackEndImpl()->GetDebugTagCompileUnit();
}

int GetDebugTagSubprogram()
{
    return GetBackEndImpl()->GetDebugTagSubprogram();
}

int GetDebugTagFileType()
{
    return GetBackEndImpl()->GetDebugTagFileType();
}

int GetDebugTagSubroutineType()
{
    return GetBackEndImpl()->GetDebugTagSubroutineType();
}

int GetDebugTagBaseType()
{
    return GetBackEndImpl()->GetDebugTagBaseType();
}

int GetDebugTagClassType()
{
    return GetBackEndImpl()->GetDebugTagClassType();
}

int GetDebugTagEnumerationType()
{
    return GetBackEndImpl()->GetDebugTagEnumerationType();
}

int GetDebugTagEnumerator()
{
    return GetBackEndImpl()->GetDebugTagEnumerator();
}

int GetDebugTagInheritance()
{
    return GetBackEndImpl()->GetDebugTagInheritance();
}

int GetDebugTagPointerType()
{
    return GetBackEndImpl()->GetDebugTagPointerType();
}

int GetDebugTagConstType()
{
    return GetBackEndImpl()->GetDebugTagConstType();
}

int GetDebugTagReferenceType()
{
    return GetBackEndImpl()->GetDebugTagReferenceType();
}

int GetDebugTagRvalueRefType()
{
    return GetBackEndImpl()->GetDebugTagRvalueRefType();
}

int GetDebugTagNamespace()
{
    return GetBackEndImpl()->GetDebugTagNamespace();
}

int GetDebugTagAutoVariable()
{
    return GetBackEndImpl()->GetDebugTagAutoVariable();
}

int GetDebugTagArgVariable()
{
    return GetBackEndImpl()->GetDebugTagArgVariable();
}

int GetDebugTagMember()
{
    return GetBackEndImpl()->GetDebugTagMember();
}

int GetDebugLangCmajor()
{
    return GetBackEndImpl()->GetDebugLangCmajor();
}

Ir::Intf::Object* CreateMetadataNull()
{
    return GetBackEndImpl()->CreateMetadataNull();
}

std::string GetVPtrVarName()
{
    return GetBackEndImpl()->GetVPtrVarName();
}

bool TypesEqual(Ir::Intf::Type* left, Ir::Intf::Type* right)
{
    return GetBackEndImpl()->TypesEqual(left, right);
}

Ir::Intf::Object* True()
{
    return GetBackEndImpl()->True();
}

Ir::Intf::Object* False()
{
    return GetBackEndImpl()->False();
}

std::string GetStackFrameIdentifier()
{
    return GetBackEndImpl()->GetStackFrameIdentifier();
}

Ir::Intf::Object* CreateConstant(const std::string& name, Ir::Intf::Type* type)
{
    return GetBackEndImpl()->CreateConstant(name, type);
}

Ir::Intf::Object* CreateBooleanConstant(bool value)
{
    return GetBackEndImpl()->CreateBooleanConstant(value);
}

Ir::Intf::Object* CreateCharConstant(char value)
{
    return GetBackEndImpl()->CreateCharConstant(value);
}

std::string GetSeparator()
{
    return GetBackEndImpl()->GetSeparator();
}

std::string GetPrivateSeparator()
{
    return GetBackEndImpl()->GetPrivateSeparator();
}

std::string GetStringValuePrefix()
{
    return GetBackEndImpl()->GetStringValuePrefix();
}

Ir::Intf::Object* CreateStringConstant(const std::string& value)
{
    return GetBackEndImpl()->CreateStringConstant(value);
}

Ir::Intf::Object* Null(Ir::Intf::Type* ptrType)
{
    return GetBackEndImpl()->Null(ptrType);
}

Ir::Intf::Object* CreateI8Constant(int8_t value)
{
    return GetBackEndImpl()->CreateI8Constant(value);
}

Ir::Intf::Object* CreateUI8Constant(uint8_t value)
{
    return GetBackEndImpl()->CreateUI8Constant(value);
}

Ir::Intf::Object* CreateI16Constant(int16_t value)
{
    return GetBackEndImpl()->CreateI16Constant(value);
}

Ir::Intf::Object* CreateUI16Constant(uint16_t value)
{
    return GetBackEndImpl()->CreateUI16Constant(value);
}

Ir::Intf::Object* CreateI32Constant(int32_t value)
{
    return GetBackEndImpl()->CreateI32Constant(value);
}

Ir::Intf::Object* CreateUI32Constant(uint32_t value)
{
    return GetBackEndImpl()->CreateUI32Constant(value);
}

Ir::Intf::Object* CreateI64Constant(int64_t value)
{
    return GetBackEndImpl()->CreateI64Constant(value);
}

Ir::Intf::Object* CreateUI64Constant(uint64_t value)
{
    return GetBackEndImpl()->CreateUI64Constant(value);
}

Ir::Intf::Object* CreateFloatConstant(float value)
{
    return GetBackEndImpl()->CreateFloatConstant(value);
}

Ir::Intf::Object* CreateDoubleConstant(double value)
{
    return GetBackEndImpl()->CreateDoubleConstant(value);
}

std::string MakeStringConstantName(const std::string& s)
{
    return GetBackEndImpl()->MakeStringConstantName(s);
}

std::string MakeStringConstantName(const std::string& s, bool metadataSyntax)
{
    return GetBackEndImpl()->MakeStringConstantName(s, metadataSyntax);
}

std::string MakeDestructionNodeName(const std::string& assemblyName)
{
    return GetBackEndImpl()->MakeDestructionNodeName(assemblyName);
}

std::string GetDestructionNodeTypeName()
{
    return GetBackEndImpl()->GetDestructionNodeTypeName();
}

std::string GetRegisterDestructorFunctionName()
{
    return GetBackEndImpl()->GetRegisterDestructorFunctionName();
}

Ir::Intf::Type* CreateTypeName(const std::string& name, bool global)
{
    return GetBackEndImpl()->CreateTypeName(name, global);
}

Ir::Intf::MemberVar* CreateMemberVar(const std::string& memberVarName, Ir::Intf::Object* ptr, int index, Ir::Intf::Type* type)
{
    return GetBackEndImpl()->CreateMemberVar(memberVarName, ptr, index, type);
}

Ir::Intf::Object* CreateStackVar(const std::string& assemblyName, Ir::Intf::Type* type)
{
    return GetBackEndImpl()->CreateStackVar(assemblyName, type);
}

Ir::Intf::Object* CreateRefVar(const std::string& assemblyName, Ir::Intf::Type* type)
{
    return GetBackEndImpl()->CreateRefVar(assemblyName, type);
}

Ir::Intf::GlobalPtr CreateGlobal(const std::string& name, Ir::Intf::Type* type)
{
    return GetBackEndImpl()->CreateGlobal(name, type);
}

Ir::Intf::Parameter* CreatePointerParameter(const std::string& name, Ir::Intf::Type* type)
{
    return GetBackEndImpl()->CreatePointerParameter(name, type);
}

std::string GetClassObjectResultParamName()
{
    return GetBackEndImpl()->GetClassObjectResultParamName();
}

std::string GetExceptionCodeParamName()
{
    return GetBackEndImpl()->GetExceptionCodeParamName();
}

Ir::Intf::Parameter* CreateParameter(const std::string& parameterName, Ir::Intf::Type* parameterType)
{
    return GetBackEndImpl()->CreateParameter(parameterName, parameterType);
}

int GetDebugAttrEncodingBoolean()
{
    return GetBackEndImpl()->GetDebugAttrEncodingBoolean();
}

int GetDebugAttrEncodingUnsigned()
{
    return GetBackEndImpl()->GetDebugAttrEncodingUnsigned();
}

int GetDebugAttrEncodingSigned()
{
    return GetBackEndImpl()->GetDebugAttrEncodingSigned();
}

int GetDebugAttrEncodingSignedChar()
{
    return GetBackEndImpl()->GetDebugAttrEncodingSignedChar();
}

int GetDebugAttrEncodingUnsignedChar()
{
    return GetBackEndImpl()->GetDebugAttrEncodingUnsignedChar();
}

int GetDebugAttrEncodingFloat()
{
    return GetBackEndImpl()->GetDebugAttrEncodingFloat();
}

Ir::Intf::Instruction* Add(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->Add(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* Sub(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->Sub(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* Mul(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->Mul(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* UDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->UDiv(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* SDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->SDiv(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* URem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->URem(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* SRem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->SRem(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* Shl(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->Shl(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* LShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->LShr(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* AShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->AShr(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* And(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->And(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* Or(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->Or(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* Xor(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->Xor(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* FAdd(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->FAdd(floatingPointType, result, operand1, operand2);
}

Ir::Intf::Instruction* FSub(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->FSub(floatingPointType, result, operand1, operand2);
}

Ir::Intf::Instruction* FMul(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->FMul(floatingPointType, result, operand1, operand2);
}

Ir::Intf::Instruction* FDiv(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->FDiv(floatingPointType, result, operand1, operand2);
}

Ir::Intf::Instruction* FRem(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->FRem(floatingPointType, result, operand1, operand2);
}

Ir::Intf::Instruction* Ret()
{
    return GetBackEndImpl()->Ret();
}

Ir::Intf::Instruction* Ret(Ir::Intf::Object* value)
{
    return GetBackEndImpl()->Ret(value);
}

Ir::Intf::Instruction* Br(Ir::Intf::LabelObject* dest)
{
    return GetBackEndImpl()->Br(dest);
}

Ir::Intf::Instruction* Br(Ir::Intf::Object* cond, Ir::Intf::LabelObject* trueLabel, Ir::Intf::LabelObject* falseLabel)
{
    return GetBackEndImpl()->Br(cond, trueLabel, falseLabel);
}

Ir::Intf::Instruction* Switch(Ir::Intf::Type* integerType, Ir::Intf::Object* value, Ir::Intf::LabelObject* defaultDest, const std::vector<std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>>& destinations)
{
    return GetBackEndImpl()->Switch(integerType, value, defaultDest, destinations);
}

Ir::Intf::Instruction* Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result)
{
    return GetBackEndImpl()->Alloca(type, result);
}

Ir::Intf::Instruction* Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Type* elementType, int numElements)
{
    return GetBackEndImpl()->Alloca(type, result, elementType, numElements);
}

Ir::Intf::Instruction* Load(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* ptr)
{
    return GetBackEndImpl()->Load(type, result, ptr);
}

Ir::Intf::Instruction* Load(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Indirection leftIndirection, Ir::Intf::Indirection rightIndirection)
{
    return GetBackEndImpl()->Load(type, result, ptr, leftIndirection, rightIndirection);
}

Ir::Intf::Instruction* Store(Ir::Intf::Type* type, Ir::Intf::Object* value, Ir::Intf::Object* ptr)
{
    return GetBackEndImpl()->Store(type, value, ptr);
}

Ir::Intf::Instruction* Store(Ir::Intf::Type* type, Ir::Intf::Object* value, Ir::Intf::Object* ptr, Ir::Intf::Indirection leftIndirection, Ir::Intf::Indirection rightIndirection)
{
    return GetBackEndImpl()->Store(type, value, ptr, leftIndirection, rightIndirection);
}

Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index)
{
    return GetBackEndImpl()->GetElementPtr(ptrType, result, ptr, index);
}

Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index, Ir::Intf::Object* index1)
{
    return GetBackEndImpl()->GetElementPtr(ptrType, result, ptr, index, index1);
}

Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index, const std::vector<Ir::Intf::Object*>& indeces)
{
    return GetBackEndImpl()->GetElementPtr(ptrType, result, ptr, index, indeces);
}

Ir::Intf::Instruction* Call(Ir::Intf::Object* result, Ir::Intf::Function* fun, const std::vector<Ir::Intf::Object*>& args)
{
    return GetBackEndImpl()->Call(result, fun, args);
}

Ir::Intf::Instruction* IndirectCall(Ir::Intf::Object* result, Ir::Intf::Object* funPtr, const std::vector<Ir::Intf::Object*>& args)
{
    return GetBackEndImpl()->IndirectCall(result, funPtr, args);
}

Ir::Intf::Instruction* ICmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::IConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->ICmp(type, result, cond, operand1, operand2);
}

Ir::Intf::Instruction* FCmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::FConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return GetBackEndImpl()->FCmp(type, result, cond, operand1, operand2);
}

Ir::Intf::Instruction* DoNothing()
{
    return GetBackEndImpl()->DoNothing();
}

Ir::Intf::Instruction* Trunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return GetBackEndImpl()->Trunc(type, result, value, toType);
}

Ir::Intf::Instruction* Zext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return GetBackEndImpl()->Zext(type, result, value, toType);
}

Ir::Intf::Instruction* Sext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return GetBackEndImpl()->Sext(type, result, value, toType);
}

Ir::Intf::Instruction* Fptrunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return GetBackEndImpl()->Fptrunc(type, result, value, toType);
}

Ir::Intf::Instruction* Fpext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return GetBackEndImpl()->Fpext(type, result, value, toType);
}

Ir::Intf::Instruction* Fptoui(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return GetBackEndImpl()->Fptoui(type, result, value, toType);
}

Ir::Intf::Instruction* Fptosi(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return GetBackEndImpl()->Fptosi(type, result, value, toType);
}

Ir::Intf::Instruction* Uitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return GetBackEndImpl()->Uitofp(type, result, value, toType);
}

Ir::Intf::Instruction* Sitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return GetBackEndImpl()->Sitofp(type, result, value, toType);
}

Ir::Intf::Instruction* Ptrtoint(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return GetBackEndImpl()->Ptrtoint(type, result, value, toType);
}

Ir::Intf::Instruction* Inttoptr(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return GetBackEndImpl()->Inttoptr(type, result, value, toType);
}

Ir::Intf::Instruction* Bitcast(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return GetBackEndImpl()->Bitcast(type, result, value, toType);
}

Ir::Intf::Instruction* DbgDeclare(Ir::Intf::Object* variable, Ir::Intf::MetadataNode* descriptor)
{
    return GetBackEndImpl()->DbgDeclare(variable, descriptor);
}

void Init(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* from, Ir::Intf::Object* to)
{
    GetBackEndImpl()->Init(emitter, type, from, to);
}

void Assign(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* from, Ir::Intf::Object* to)
{
    GetBackEndImpl()->Assign(emitter, type, from, to);
}

Ir::Intf::Object* SizeOf(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type)
{
    return GetBackEndImpl()->SizeOf(emitter, type);
}

} } // namespace Cm::Ir
