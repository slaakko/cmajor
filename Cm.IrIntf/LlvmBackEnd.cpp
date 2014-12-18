/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.IrIntf/LlvmBackEnd.hpp>
#include <Llvm.Ir/Instruction.hpp>
#include <Llvm.Ir/DebugInfo.hpp>
#include <Llvm.Ir/Util.hpp>
#include <Llvm.Ir/RegVar.hpp>
#include <Llvm.Ir/Parameter.hpp>
#include <Llvm.Ir/Global.hpp>
#include <Llvm.Ir/MemberVar.hpp>
#include <Llvm.Ir/Metadata.hpp>
#include <Llvm.Ir/Temporary.hpp>
#include <Llvm.Ir/StackVar.hpp>
#include <Llvm.Ir/RefVar.hpp>
#include <Llvm.Ir/Constant.hpp>
#include <Llvm.Ir/Factory.hpp>

namespace Cm { namespace IrIntf {

LlvmBackEnd llvmBackEnd;
Llvm::Factory llvmFactory;

BackEndIntf* GetLlvmBackEnd()
{
    return &llvmBackEnd;
}

Ir::Intf::Factory* GetLlvmFactory()
{
    return &llvmFactory;
}

std::string LlvmBackEnd::GetExCodeVarName()
{
    return "$ex";
}

Ir::Intf::LabelObject* LlvmBackEnd::CreateTempLabel(int tempLabelCounter)
{
    return Llvm::CreateLabel("$T" + std::to_string(tempLabelCounter));
}

std::string LlvmBackEnd::CreateTempVarName(int tempVarCounter)
{
    return "$" + std::to_string(tempVarCounter);
}

Ir::Intf::RegVarPtr LlvmBackEnd::CreateTemporaryRegVar(Ir::Intf::Type* type)
{
    return Llvm::CreateTemporaryRegVar(type);
}

Ir::Intf::LabelObject* LlvmBackEnd::CreateNextLocalLabel()
{
    return Llvm::CreateNextLocalLabel();
}

Ir::Intf::LabelObject* LlvmBackEnd::CreateLabel()
{
    return Llvm::CreateLabel();
}

Ir::Intf::LabelObject* LlvmBackEnd::CreateLabel(const std::string& label)
{
    return Llvm::CreateLabel(label);
}

Ir::Intf::LabelObject* LlvmBackEnd::CreateLandingPadLabel(int index)
{
    return CreateLabel("$landingpad_" + std::to_string(index));
}

Ir::Intf::Type* LlvmBackEnd::I1()
{
    return Llvm::I1();
}

Ir::Intf::Type* LlvmBackEnd::I8()
{
    return Llvm::I8();
}

Ir::Intf::Type* LlvmBackEnd::I16()
{
    return Llvm::I16();
}

Ir::Intf::Type* LlvmBackEnd::I32()
{
    return Llvm::I32();
}

Ir::Intf::Type* LlvmBackEnd::I64()
{
    return Llvm::I64();
}

Ir::Intf::Type* LlvmBackEnd::UI8()
{
    return Llvm::I8();
}

Ir::Intf::Type* LlvmBackEnd::UI16()
{
    return Llvm::I16();
}

Ir::Intf::Type* LlvmBackEnd::UI32()
{
    return Llvm::I32();
}

Ir::Intf::Type* LlvmBackEnd::UI64()
{
    return Llvm::I64();
}

Ir::Intf::Type* LlvmBackEnd::Float()
{
    return Llvm::Float();
}

Ir::Intf::Type* LlvmBackEnd::Double()
{
    return Llvm::Double();
}

Ir::Intf::Type* LlvmBackEnd::Void()
{
    return Llvm::Void();
}

Ir::Intf::Type* LlvmBackEnd::Char()
{
    return Llvm::I8();
}

Ir::Intf::Type* LlvmBackEnd::RvalueRef(Ir::Intf::Type* baseType)
{
    return Llvm::RvalueRef(baseType);
}

Ir::Intf::Type* LlvmBackEnd::Pointer(Ir::Intf::Type* baseType, int numPointers)
{
    return Llvm::MakePointerType(baseType, numPointers);
}

Ir::Intf::Type* LlvmBackEnd::Structure(const std::string&, const std::vector<Ir::Intf::Type*>& elementTypes, const std::vector<std::string>&)
{
    return Llvm::Structure(elementTypes);
}

Ir::Intf::Type* LlvmBackEnd::Array(Ir::Intf::Type* itemType, int size)
{
    return Llvm::Array(itemType, size);
}

Ir::Intf::Type* LlvmBackEnd::CreateFunctionType(Ir::Intf::Type* returnType, const std::vector<Ir::Intf::Type*>& parameterTypes)
{
    return Llvm::CreateFunctionType(returnType, parameterTypes);
}

Ir::Intf::Type* LlvmBackEnd::CreateTypedef(const std::string& name, Ir::Intf::Type* type)
{
    return nullptr;
}

Ir::Intf::Function* LlvmBackEnd::CreateFunction(const std::string& name, Ir::Intf::Type* returnType, const std::vector<Ir::Intf::Parameter*>& parameters)
{
    return new Llvm::Function(name, returnType, parameters);
}

Ir::Intf::Function* LlvmBackEnd::CreateDoNothingFunction()
{
    return Llvm::CreateDoNothingFunction();
}

Ir::Intf::Function* LlvmBackEnd::CreateDbgDeclareFunction()
{
    return Llvm::CreateDbgDeclareFunction();
}

void LlvmBackEnd::ResetLocalLabelCounter()
{
    Llvm::ResetLocalLabelCounter();
}

void LlvmBackEnd::ResetTemporaryCounter()
{
    Llvm::ResetTemporaryCounter();
}

std::string LlvmBackEnd::MakeAssemblyName(const std::string& name)
{
    std::string assemblyName;
    assemblyName.reserve(2 * name.length());
    int n = int(name.size());
    for (int i = 0; i < n; ++i)
    {
        char c = name[i];
        switch (c)
        {
            case '&': assemblyName.append("_R"); break;
            case '*': assemblyName.append("_P"); break;
            case '<': assemblyName.append("$$"); break;
            case '>': assemblyName.append("$$"); break;
            case ',': assemblyName.append("."); break;
            case '(': assemblyName.append("_O"); break;
            case ')': assemblyName.append("_C"); break;
            case ' ': break;
            default: assemblyName.append(1, c); break;
        }
    }
    return assemblyName;
}

std::string LlvmBackEnd::MakeClassNameAssemblyName(const std::string& fullClassName)
{
    return MakeAssemblyName(fullClassName + "$classname");
}

Ir::Intf::Type* LlvmBackEnd::CreateClassTypeName(const std::string& fullClassName)
{
    return Llvm::TypeName("class." + MakeAssemblyName(fullClassName), false);
}

std::string LlvmBackEnd::GetVtblPtrVarName()
{
    return "$vtblptr";
}

Ir::Intf::MetadataStringPtr LlvmBackEnd::CreateMetadataString(const std::string& s)
{
    return new Llvm::MetadataString(s);
}

Ir::Intf::MetadataNode* LlvmBackEnd::CreateMetadataNode(int id)
{
    return new Llvm::MetadataNode(id);
}

Ir::Intf::Object* LlvmBackEnd::CreateMetadataNull()
{
    return new Llvm::MetadataNull();
}

std::string LlvmBackEnd::GetStackFrameIdentifier()
{
    return "stack$frame";
}

int LlvmBackEnd::GetDebugTagLexicalBlock()
{
    return Llvm::debugTagLexicalBlock;
}

int LlvmBackEnd::GetDebugTagCompileUnit()
{
    return Llvm::debugTagCompileUnit;
}

int LlvmBackEnd::GetDebugTagSubprogram()
{
    return Llvm::debugTagSubprogram;
}

int LlvmBackEnd::GetDebugTagFileType()
{
    return Llvm::debugTagFileType;
}

int LlvmBackEnd::GetDebugTagSubroutineType()
{
    return Llvm::debugTagSubroutineType;
}

int LlvmBackEnd::GetDebugTagBaseType()
{
    return Llvm::debugTagBaseType;
}

int LlvmBackEnd::GetDebugTagClassType()
{
    return Llvm::debugTagClassType;
}

int LlvmBackEnd::GetDebugTagEnumerationType()
{
    return Llvm::debugTagEnumerationType;
}

int LlvmBackEnd::GetDebugTagEnumerator()
{
    return Llvm::debugTagEnumerator;
}

int LlvmBackEnd::GetDebugTagInheritance()
{
    return Llvm::debugTagInheritance;
}

int LlvmBackEnd::GetDebugTagPointerType()
{
    return Llvm::debugTagPointerType;
}

int LlvmBackEnd::GetDebugTagConstType()
{
    return Llvm::debugTagConstType;
}

int LlvmBackEnd::GetDebugTagReferenceType()
{
    return Llvm::debugTagReferenceType;
}

int LlvmBackEnd::GetDebugTagRvalueRefType()
{
    return Llvm::debugTagRvalueRefType;
}

int LlvmBackEnd::GetDebugTagNamespace()
{
    return Llvm::debugTagNamespace;
}

int LlvmBackEnd::GetDebugTagAutoVariable()
{
    return Llvm::debugTagAutoVariable;
}

int LlvmBackEnd::GetDebugTagArgVariable()
{
    return Llvm::debugTagArgVariable;
}

int LlvmBackEnd::GetDebugTagMember()
{
    return Llvm::debugTagMember;
}

int LlvmBackEnd::GetDebugLangCmajor()
{
    return Llvm::debugLangCmajor;
}

bool LlvmBackEnd::TypesEqual(Ir::Intf::Type* left, Ir::Intf::Type* right)
{
    return Llvm::TypesEqual(left, right);
}

Ir::Intf::Object* LlvmBackEnd::True()
{
    return Llvm::True();
}
Ir::Intf::Object* LlvmBackEnd::False()
{
    return Llvm::False();
}

Ir::Intf::Object* LlvmBackEnd::CreateConstant(const std::string& name, Ir::Intf::Type* type)
{
    return Llvm::CreateConstant(name, type);
}

Ir::Intf::Object* LlvmBackEnd::CreateBooleanConstant(bool value)
{
    return Llvm::CreateBooleanConstant(value);
}

Ir::Intf::Object* LlvmBackEnd::CreateCharConstant(char value)
{
    return Llvm::CreateCharConstant(value);
}

std::string LlvmBackEnd::GetSeparator()
{
    return ".";
}

std::string LlvmBackEnd::GetPrivateSeparator()
{
    return "$";
}

std::string LlvmBackEnd::GetStringValuePrefix()
{
    return ".s";
}

Ir::Intf::Object* LlvmBackEnd::CreateStringConstant(const std::string& value)
{
    return Llvm::CreateStringConstant(value);
}

Ir::Intf::Object* LlvmBackEnd::Null(Ir::Intf::Type* ptrType)
{
    return Llvm::Null(ptrType);
}

Ir::Intf::Object* LlvmBackEnd::CreateI8Constant(int8_t value)
{
    return Llvm::CreateI8Constant(value);
}

Ir::Intf::Object* LlvmBackEnd::CreateUI8Constant(uint8_t value)
{
    return Llvm::CreateI8Constant(value);
}

Ir::Intf::Object* LlvmBackEnd::CreateI16Constant(int16_t value)
{
    return Llvm::CreateI16Constant(value);
}

Ir::Intf::Object* LlvmBackEnd::CreateUI16Constant(uint16_t value)
{
    return Llvm::CreateI16Constant(value);
}

Ir::Intf::Object* LlvmBackEnd::CreateI32Constant(int32_t value)
{
    return Llvm::CreateI32Constant(value);
}

Ir::Intf::Object* LlvmBackEnd::CreateUI32Constant(uint32_t value)
{
    return Llvm::CreateUI32Constant(value);
}

Ir::Intf::Object* LlvmBackEnd::CreateI64Constant(int64_t value)
{
    return Llvm::CreateI64Constant(value);
}

Ir::Intf::Object* LlvmBackEnd::CreateUI64Constant(uint64_t value)
{
    return Llvm::CreateUI64Constant(value);
}

Ir::Intf::Object* LlvmBackEnd::CreateFloatConstant(float value)
{
    return Llvm::CreateFloatConstant(value);
}

Ir::Intf::Object* LlvmBackEnd::CreateDoubleConstant(double value)
{
    return Llvm::CreateDoubleConstant(value);
}

std::string LlvmBackEnd::MakeStringConstantName(const std::string& s)
{
    return Llvm::MakeStringConstantName(s);
}

std::string LlvmBackEnd::MakeStringConstantName(const std::string& s, bool metadataSyntax)
{
    return Llvm::MakeStringConstantName(s, metadataSyntax);
}

std::string LlvmBackEnd::MakeDestructionNodeName(const std::string& assemblyName)
{
    return assemblyName + "$dnode";
}

std::string LlvmBackEnd::GetDestructionNodeTypeName()
{
    return "destruction$node";
}

std::string LlvmBackEnd::GetRegisterDestructorFunctionName()
{
    return "register$destructor";
}

Ir::Intf::Type* LlvmBackEnd::CreateTypeName(const std::string& name, bool global)
{
    return Llvm::TypeName(name, global);
}

Ir::Intf::MemberVarPtr LlvmBackEnd::CreateMemberVar(const std::string& memberVarName, Ir::Intf::Object* ptr, int index, Ir::Intf::Type* type)
{
    return Ir::Intf::MemberVarPtr(new Llvm::MemberVar(memberVarName, ptr, index, type));
}

Ir::Intf::Object* LlvmBackEnd::CreateStackVar(const std::string& assemblyName, Ir::Intf::Type* type)
{
    return new Llvm::StackVar(assemblyName, type);
}

Ir::Intf::Object* LlvmBackEnd::CreateRefVar(const std::string& assemblyName, Ir::Intf::Type* type)
{
    return new Llvm::RefVar(assemblyName, type);
}

Ir::Intf::GlobalPtr LlvmBackEnd::CreateGlobal(const std::string& name, Ir::Intf::Type* type)
{
    return Llvm::CreateGlobal(name, type);
}

Ir::Intf::Parameter* LlvmBackEnd::CreatePointerParameter(const std::string& name, Ir::Intf::Type* type)
{
    return new Llvm::Parameter(name + "$p", type);
}

Ir::Intf::Parameter* LlvmBackEnd::CreateParameter(const std::string& parameterName, Ir::Intf::Type* parameterType)
{
    return new Llvm::Parameter(parameterName, parameterType);
}

std::string LlvmBackEnd::GetClassObjectResultParamName()
{
    return Llvm::classObjectResultParamName;
}

std::string LlvmBackEnd::GetExceptionCodeParamName()
{
    return Llvm::exceptionCodeParamName;
}

int LlvmBackEnd::GetDebugAttrEncodingBoolean()
{
    return Llvm::debugAttrEncodingBoolean;
}

int LlvmBackEnd::GetDebugAttrEncodingUnsigned()
{
    return Llvm::debugAttrEncodingUnsigned;
}

int LlvmBackEnd::GetDebugAttrEncodingSigned()
{
    return Llvm::debugAttrEncodingSigned;
}

int LlvmBackEnd::GetDebugAttrEncodingSignedChar()
{
    return Llvm::debugAttrEncodingSignedChar;
}

int LlvmBackEnd::GetDebugAttrEncodingUnsignedChar()
{
    return Llvm::debugAttrEncodingUnsignedChar;
}

int LlvmBackEnd::GetDebugAttrEncodingFloat()
{
    return Llvm::debugAttrEncodingFloat;
}

Ir::Intf::Instruction* LlvmBackEnd::Add(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::Add(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::Sub(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::Sub(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::Mul(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::Mul(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::UDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::UDiv(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::SDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::SDiv(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::URem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::URem(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::SRem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::SRem(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::Shl(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::Shl(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::LShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::LShr(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::AShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::AShr(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::And(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::And(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::Or(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::Or(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::Xor(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::Xor(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::FAdd(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::FAdd(floatingPointType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::FSub(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::FSub(floatingPointType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::FMul(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::FMul(floatingPointType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::FDiv(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::FDiv(floatingPointType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::FRem(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::FRem(floatingPointType, result, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::Ret()
{
    return Llvm::Ret();
}

Ir::Intf::Instruction* LlvmBackEnd::Ret(Ir::Intf::Object* value)
{
    return Llvm::Ret(value);
}

Ir::Intf::Instruction* LlvmBackEnd::Br(Ir::Intf::LabelObject* dest)
{
    return Llvm::Br(dest);
}

Ir::Intf::Instruction* LlvmBackEnd::Br(Ir::Intf::Object* cond, Ir::Intf::LabelObject* trueLabel, Ir::Intf::LabelObject* falseLabel)
{
    return Llvm::Br(cond, trueLabel, falseLabel);
}

Ir::Intf::Instruction* LlvmBackEnd::Switch(Ir::Intf::Type* integerType, Ir::Intf::Object* value, Ir::Intf::LabelObject* defaultDest, const std::vector<std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>>& destinations)
{
    return Llvm::Switch(integerType, value, defaultDest, destinations);
}

Ir::Intf::Instruction* LlvmBackEnd::Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result)
{
    return Llvm::Alloca(type, result);
}

Ir::Intf::Instruction* LlvmBackEnd::Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Type* elementType, int numElements)
{
    return Llvm::Alloca(type, result, elementType, numElements);
}

Ir::Intf::Instruction* LlvmBackEnd::Load(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* ptr)
{
    return Llvm::Load(type, result, ptr);
}

Ir::Intf::Instruction* LlvmBackEnd::Load(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Indirection leftIndirection, Ir::Intf::Indirection rightIndirection)
{
    return Llvm::Load(type, result, ptr);
}

Ir::Intf::Instruction* LlvmBackEnd::Store(Ir::Intf::Type* type, Ir::Intf::Object* value, Ir::Intf::Object* ptr)
{
    return Llvm::Store(type, value, ptr);
}

Ir::Intf::Instruction* LlvmBackEnd::Store(Ir::Intf::Type* type, Ir::Intf::Object* value, Ir::Intf::Object* ptr, Ir::Intf::Indirection leftIndirection, Ir::Intf::Indirection rightIndirection)
{
    return Llvm::Store(type, value, ptr);
}

Ir::Intf::Instruction* LlvmBackEnd::GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index)
{
    return Llvm::GetElementPtr(ptrType, result, ptr, index);
}

Ir::Intf::Instruction* LlvmBackEnd::GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index, Ir::Intf::Object* index1)
{
    return Llvm::GetElementPtr(ptrType, result, ptr, index, index1);
}

Ir::Intf::Instruction* LlvmBackEnd::GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index, const std::vector<Ir::Intf::Object*>& indeces)
{
    return Llvm::GetElementPtr(ptrType, result, ptr, index, indeces);
}

Ir::Intf::Instruction* LlvmBackEnd::Call(Ir::Intf::Object* result, Ir::Intf::Function* fun, const std::vector<Ir::Intf::Object*>& args)
{
    return Llvm::Call(result, fun, args);
}

Ir::Intf::Instruction* LlvmBackEnd::IndirectCall(Ir::Intf::Object* result, Ir::Intf::Object* funPtr, const std::vector<Ir::Intf::Object*>& args)
{
    return Llvm::IndirectCall(result, funPtr, args);
}

Ir::Intf::Instruction* LlvmBackEnd::ICmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::IConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::ICmp(type, result, cond, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::FCmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::FConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return Llvm::FCmp(type, result, cond, operand1, operand2);
}

Ir::Intf::Instruction* LlvmBackEnd::DoNothing()
{
    return Llvm::DoNothing();
}

Ir::Intf::Instruction* LlvmBackEnd::Trunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return Llvm::Trunc(type, result, value, toType);
}

Ir::Intf::Instruction* LlvmBackEnd::Zext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return Llvm::Zext(type, result, value, toType);
}

Ir::Intf::Instruction* LlvmBackEnd::Sext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return Llvm::Sext(type, result, value, toType);
}

Ir::Intf::Instruction* LlvmBackEnd::Fptrunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return Llvm::Fptrunc(type, result, value, toType);
}

Ir::Intf::Instruction* LlvmBackEnd::Fpext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return Llvm::Fpext(type, result, value, toType);
}

Ir::Intf::Instruction* LlvmBackEnd::Fptoui(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return Llvm::Fptoui(type, result, value, toType);
}

Ir::Intf::Instruction* LlvmBackEnd::Fptosi(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return Llvm::Fptosi(type, result, value, toType);
}

Ir::Intf::Instruction* LlvmBackEnd::Uitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return Llvm::Uitofp(type, result, value, toType);
}

Ir::Intf::Instruction* LlvmBackEnd::Sitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return Llvm::Sitofp(type, result, value, toType);
}

Ir::Intf::Instruction* LlvmBackEnd::Ptrtoint(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return Llvm::Ptrtoint(type, result, value, toType);
}

Ir::Intf::Instruction* LlvmBackEnd::Inttoptr(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return Llvm::Inttoptr(type, result, value, toType);
}

Ir::Intf::Instruction* LlvmBackEnd::Bitcast(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return Llvm::Bitcast(type, result, value, toType);
}

Ir::Intf::Instruction* LlvmBackEnd::DbgDeclare(Ir::Intf::Object* variable, Ir::Intf::MetadataNode* descriptor)
{
    return Llvm::DbgDeclare(variable, descriptor);
}

void LlvmBackEnd::Init(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* from, Ir::Intf::Object* to)
{
    Llvm::Init(emitter, type, from, to);
}

void LlvmBackEnd::Assign(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* from, Ir::Intf::Object* to)
{
    Llvm::Assign(emitter, type, from, to);
}

Ir::Intf::Object* LlvmBackEnd::SizeOf(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type)
{
    return Llvm::SizeOf(emitter, type);
}

} } // namespace Cm::IrIntf
