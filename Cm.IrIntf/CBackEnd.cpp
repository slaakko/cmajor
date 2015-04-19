/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.IrIntf/CBackEnd.hpp>
#include <C.Ir/Instruction.hpp>
#include <C.Ir/Util.hpp>
#include <C.Ir/RegVar.hpp>
#include <C.Ir/Parameter.hpp>
#include <C.Ir/Global.hpp>
#include <C.Ir/MemberVar.hpp>
#include <C.Ir/StackVar.hpp>
#include <C.Ir/RefVar.hpp>
#include <C.Ir/Constant.hpp>
#include <C.Ir/Factory.hpp>
#include <C.Ir/Function.hpp>

namespace Cm { namespace IrIntf {

CBackEnd cBackEnd;
C::Factory cFactory;

BackEndIntf* GetCBackEnd()
{
    return &cBackEnd;
}

Ir::Intf::Factory* GetCFactory()
{
    return &cFactory;
}

std::string CBackEnd::GetExCodeVarName()
{
    return "_X_ex";
}

Ir::Intf::LabelObject* CBackEnd::CreateTempLabel(int tempLabelCounter)
{
    return C::CreateLabel(std::to_string(tempLabelCounter));
}

std::string CBackEnd::CreateTempVarName(int tempVarCounter)
{
    return std::to_string(tempVarCounter);
}

Ir::Intf::RegVar* CBackEnd::CreateTemporaryRegVar(Ir::Intf::Type* type)
{
    return C::CreateTemporaryRegVar(type);
}

Ir::Intf::LabelObject* CBackEnd::CreateNextLocalLabel()
{
    return C::CreateNextLocalLabel();
}

Ir::Intf::LabelObject* CBackEnd::CreateLabel()
{
    return C::CreateLabel();
}

Ir::Intf::LabelObject* CBackEnd::CreateLabel(const std::string& label)
{
    return C::CreateLabel(label);
}

Ir::Intf::LabelObject* CBackEnd::CreateLandingPadLabel(int index)
{
    return CreateLabel("_LP_" + std::to_string(index));
}

Ir::Intf::Type* CBackEnd::I1()
{
    return C::I1();
}

Ir::Intf::Type* CBackEnd::I8()
{
    return C::I8();
}

Ir::Intf::Type* CBackEnd::I16()
{
    return C::I16();
}

Ir::Intf::Type* CBackEnd::I32()
{
    return C::I32();
}

Ir::Intf::Type* CBackEnd::I64()
{
    return C::I64();
}

Ir::Intf::Type* CBackEnd::UI8()
{
    return C::UI8();
}

Ir::Intf::Type* CBackEnd::UI16()
{
    return C::UI16();
}

Ir::Intf::Type* CBackEnd::UI32()
{
    return C::UI32();
}

Ir::Intf::Type* CBackEnd::UI64()
{
    return C::UI64();
}

Ir::Intf::Type* CBackEnd::Float()
{
    return C::Float();
}

Ir::Intf::Type* CBackEnd::Double()
{
    return C::Double();
}

Ir::Intf::Type* CBackEnd::Void()
{
    return C::Void();
}

Ir::Intf::Type* CBackEnd::Char()
{
    return C::Char();
}

Ir::Intf::Type* CBackEnd::RvalueRef(Ir::Intf::Type* baseType)
{
    return C::RvalueRef(baseType);
}

Ir::Intf::Type* CBackEnd::Pointer(Ir::Intf::Type* baseType, int numPointers)
{
    return C::MakePointerType(baseType, numPointers);
}

Ir::Intf::Type* CBackEnd::Structure(const std::string& tagName, const std::vector<Ir::Intf::Type*>& elementTypes, const std::vector<std::string>& elementNames)
{
    return C::Structure(tagName, elementTypes, elementNames);
}

Ir::Intf::Type* CBackEnd::Array(Ir::Intf::Type* itemType, int size)
{
    return C::Array(itemType, size);
}

Ir::Intf::Type* CBackEnd::CreateFunctionType(Ir::Intf::Type* returnType, const std::vector<Ir::Intf::Type*>& parameterTypes)
{
    return C::CreateFunctionType(returnType, parameterTypes);
}

Ir::Intf::Type* CBackEnd::CreateTypedef(const std::string& name, Ir::Intf::Type* type)
{
    return C::CreateTypedef(name, type);
}

Ir::Intf::Function* CBackEnd::CreateFunction(const std::string& name, Ir::Intf::Type* returnType, const std::vector<Ir::Intf::Parameter*>& parameters)
{
    return new C::Function(name, returnType, parameters);
}

Ir::Intf::Function* CBackEnd::CreateDoNothingFunction()
{
    throw std::runtime_error("member function not applicable to C backend");
}

Ir::Intf::Function* CBackEnd::CreateDbgDeclareFunction()
{
    throw std::runtime_error("member function not applicable to C backend");
}

void CBackEnd::ResetLocalLabelCounter()
{
    C::ResetLocalLabelCounter();
}

std::string CBackEnd::MakeAssemblyName(const std::string& name)
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
        case '<': assemblyName.append("_B"); break;
        case '>': assemblyName.append("_E"); break;
        case ',': assemblyName.append("_"); break;
        case '.': assemblyName.append("_N_"); break;
        case '(': assemblyName.append("_O_"); break;
        case ')': assemblyName.append("_C_"); break;
        case ' ': break;
        default: assemblyName.append(1, c); break;
        }
    }
    return assemblyName;
}

std::string CBackEnd::MakeClassNameAssemblyName(const std::string& fullClassName)
{
    return MakeAssemblyName(fullClassName + "_classname");
}

Ir::Intf::Type* CBackEnd::CreateClassTypeName(const std::string& fullClassName)
{
    return C::TypeName("class_" + MakeAssemblyName(fullClassName), false);
}

std::string CBackEnd::GetVPtrVarName()
{
    return "__vptr";
}

Ir::Intf::MetadataStringPtr CBackEnd::CreateMetadataString(const std::string& s)
{
    throw std::runtime_error("member function not applicable to C backend");
}

Ir::Intf::MetadataNode* CBackEnd::CreateMetadataNode(int id)
{
    throw std::runtime_error("member function not applicable to C backend");
}

Ir::Intf::Object* CBackEnd::CreateMetadataNull()
{
    throw std::runtime_error("member function not applicable to C backend");
}

std::string CBackEnd::GetStackFrameIdentifier()
{
    return "_V_F_frame";
}

int CBackEnd::GetDebugTagLexicalBlock()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugTagCompileUnit()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugTagSubprogram()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugTagFileType()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugTagSubroutineType()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugTagBaseType()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugTagClassType()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugTagEnumerationType()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugTagEnumerator()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugTagInheritance()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugTagPointerType()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugTagConstType()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugTagReferenceType()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugTagRvalueRefType()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugTagNamespace()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugTagAutoVariable()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugTagArgVariable()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugTagMember()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugLangCmajor()
{
    throw std::runtime_error("member function not applicable to C backend");
}

bool CBackEnd::TypesEqual(Ir::Intf::Type* left, Ir::Intf::Type* right)
{
    return C::TypesEqual(left, right);
}

Ir::Intf::Object* CBackEnd::True()
{
    return C::True();
}
Ir::Intf::Object* CBackEnd::False()
{
    return C::False();
}

Ir::Intf::Object* CBackEnd::CreateConstant(const std::string& name, Ir::Intf::Type* type)
{
    return C::CreateConstant(name, type);
}

Ir::Intf::Object* CBackEnd::CreateBooleanConstant(bool value)
{
    return C::CreateBooleanConstant(value);
}

Ir::Intf::Object* CBackEnd::CreateCharConstant(char value)
{
    return C::CreateCharConstant(value);
}

std::string CBackEnd::GetSeparator()
{
    return "_N_";
}

std::string CBackEnd::GetPrivateSeparator()
{
    return "_M_";
}

std::string CBackEnd::GetStringValuePrefix()
{
    return "_S_";
}

Ir::Intf::Object* CBackEnd::CreateStringConstant(const std::string& value)
{
    return C::CreateStringConstant(value);
}

Ir::Intf::Object* CBackEnd::Null(Ir::Intf::Type* ptrType)
{
    return C::Null(ptrType);
}

Ir::Intf::Object* CBackEnd::CreateI8Constant(int8_t value)
{
    return C::CreateI8Constant(value);
}

Ir::Intf::Object* CBackEnd::CreateUI8Constant(uint8_t value)
{
    return C::CreateUI8Constant(value);
}

Ir::Intf::Object* CBackEnd::CreateI16Constant(int16_t value)
{
    return C::CreateI16Constant(value);
}

Ir::Intf::Object* CBackEnd::CreateUI16Constant(uint16_t value)
{
    return C::CreateUI16Constant(value);
}

Ir::Intf::Object* CBackEnd::CreateI32Constant(int32_t value)
{
    return C::CreateI32Constant(value);
}

Ir::Intf::Object* CBackEnd::CreateUI32Constant(uint32_t value)
{
    return C::CreateUI32Constant(value);
}

Ir::Intf::Object* CBackEnd::CreateI64Constant(int64_t value)
{
    return C::CreateI64Constant(value);
}

Ir::Intf::Object* CBackEnd::CreateUI64Constant(uint64_t value)
{
    return C::CreateUI64Constant(value);
}

Ir::Intf::Object* CBackEnd::CreateFloatConstant(float value)
{
    return C::CreateFloatConstant(value);
}

Ir::Intf::Object* CBackEnd::CreateDoubleConstant(double value)
{
    return C::CreateDoubleConstant(value);
}

std::string CBackEnd::MakeStringConstantName(const std::string& s)
{
    return C::MakeStringConstantName(s);
}

std::string CBackEnd::MakeStringConstantName(const std::string& s, bool metadataSyntax)
{
    return C::MakeStringConstantName(s, metadataSyntax);
}

std::string CBackEnd::MakeDestructionNodeName(const std::string& assemblyName)
{
    return assemblyName + "_X_dnode";
}

std::string CBackEnd::GetDestructionNodeTypeName()
{
    return "destruction_X_node";
}

std::string CBackEnd::GetRegisterDestructorFunctionName()
{
    return "register_X_destructor";
}

Ir::Intf::Type* CBackEnd::CreateTypeName(const std::string& name, bool global)
{
    return C::TypeName(name, global);
}

Ir::Intf::MemberVar* CBackEnd::CreateMemberVar(const std::string& memberVarName, Ir::Intf::Object* ptr, int index, Ir::Intf::Type* type)
{
    return new C::MemberVar(memberVarName, ptr, index, type);
}

Ir::Intf::Object* CBackEnd::CreateStackVar(const std::string& assemblyName, Ir::Intf::Type* type)
{
    return new C::StackVar(assemblyName, type);
}

Ir::Intf::Object* CBackEnd::CreateRefVar(const std::string& assemblyName, Ir::Intf::Type* type)
{
    return new C::RefVar(assemblyName, type);
}

Ir::Intf::GlobalPtr CBackEnd::CreateGlobal(const std::string& name, Ir::Intf::Type* type)
{
    return C::CreateGlobal(name, type);
}

Ir::Intf::Parameter* CBackEnd::CreatePointerParameter(const std::string& name, Ir::Intf::Type* type)
{
    return new C::Parameter(name, type);
}

Ir::Intf::Parameter* CBackEnd::CreateParameter(const std::string& parameterName, Ir::Intf::Type* parameterType)
{
    return new C::Parameter(parameterName, parameterType);
}

std::string CBackEnd::GetClassObjectResultParamName()
{
    return C::classObjectResultParamName;
}

std::string CBackEnd::GetExceptionCodeParamName()
{
    return C::exceptionCodeParamName;
}

std::string CBackEnd::GetExceptionBaseIdTableName()
{
    return "_X_exception_X_base_X_id_X_table_X_addr";
}

int CBackEnd::GetDebugAttrEncodingBoolean()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugAttrEncodingUnsigned()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugAttrEncodingSigned()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugAttrEncodingSignedChar()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugAttrEncodingUnsignedChar()
{
    throw std::runtime_error("member function not applicable to C backend");
}

int CBackEnd::GetDebugAttrEncodingFloat()
{
    throw std::runtime_error("member function not applicable to C backend");
}

Ir::Intf::Instruction* CBackEnd::Add(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::Add(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::Sub(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::Sub(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::Mul(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::Mul(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::UDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::UDiv(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::SDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::SDiv(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::URem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::URem(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::SRem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::SRem(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::Shl(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::Shl(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::LShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::LShr(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::AShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::AShr(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::And(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::And(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::Or(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::Or(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::Xor(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::Xor(integerType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::FAdd(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::FAdd(floatingPointType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::FSub(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::FSub(floatingPointType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::FMul(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::FMul(floatingPointType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::FDiv(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::FDiv(floatingPointType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::FRem(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::FRem(floatingPointType, result, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::Ret()
{
    return C::Ret();
}

Ir::Intf::Instruction* CBackEnd::Ret(Ir::Intf::Object* value)
{
    return C::Ret(value);
}

Ir::Intf::Instruction* CBackEnd::Br(Ir::Intf::LabelObject* dest)
{
    return C::Br(dest);
}

Ir::Intf::Instruction* CBackEnd::Br(Ir::Intf::Object* cond, Ir::Intf::LabelObject* trueLabel, Ir::Intf::LabelObject* falseLabel)
{
    return C::Br(cond, trueLabel, falseLabel);
}

Ir::Intf::Instruction* CBackEnd::Switch(Ir::Intf::Type* integerType, Ir::Intf::Object* value, Ir::Intf::LabelObject* defaultDest, const std::vector<std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>>& destinations)
{
    return C::Switch(integerType, value, defaultDest, destinations);
}

Ir::Intf::Instruction* CBackEnd::Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result)
{
    return C::Alloca(type, result);
}

Ir::Intf::Instruction* CBackEnd::Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Type* elementType, int numElements)
{
    return C::Alloca(type, result, elementType, numElements);
}

Ir::Intf::Instruction* CBackEnd::Load(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* ptr)
{
    return C::Load(type, result, ptr, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none);
}

Ir::Intf::Instruction* CBackEnd::Load(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Indirection leftIndirection, Ir::Intf::Indirection rightIndirection)
{
    return C::Load(type, result, ptr, leftIndirection, rightIndirection);
}

Ir::Intf::Instruction* CBackEnd::Store(Ir::Intf::Type* type, Ir::Intf::Object* value, Ir::Intf::Object* ptr)
{
    return C::Store(type, value, ptr, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none);
}

Ir::Intf::Instruction* CBackEnd::Store(Ir::Intf::Type* type, Ir::Intf::Object* value, Ir::Intf::Object* ptr, Ir::Intf::Indirection leftIndirection, Ir::Intf::Indirection rightIndirection)
{
    return C::Store(type, value, ptr, leftIndirection, rightIndirection);
}

Ir::Intf::Instruction* CBackEnd::GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index)
{
    throw std::runtime_error("member function not applicable to C backend");
}

Ir::Intf::Instruction* CBackEnd::GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index, Ir::Intf::Object* index1)
{
    throw std::runtime_error("member function not applicable to C backend");
}

Ir::Intf::Instruction* CBackEnd::GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index, const std::vector<Ir::Intf::Object*>& indeces)
{
    throw std::runtime_error("member function not applicable to C backend");
}

Ir::Intf::Instruction* CBackEnd::Call(Ir::Intf::Object* result, Ir::Intf::Function* fun, const std::vector<Ir::Intf::Object*>& args)
{
    return C::Call(result, fun, args);
}

Ir::Intf::Instruction* CBackEnd::IndirectCall(Ir::Intf::Object* result, Ir::Intf::Object* funPtr, const std::vector<Ir::Intf::Object*>& args)
{
    return C::IndirectCall(result, funPtr, args);
}

Ir::Intf::Instruction* CBackEnd::ICmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::IConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::ICmp(type, result, cond, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::FCmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::FConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2)
{
    return C::FCmp(type, result, cond, operand1, operand2);
}

Ir::Intf::Instruction* CBackEnd::DoNothing()
{
    return C::DoNothing();
}

Ir::Intf::Instruction* CBackEnd::Trunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return C::Trunc(type, result, value, toType);
}

Ir::Intf::Instruction* CBackEnd::Zext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return C::Zext(type, result, value, toType);
}

Ir::Intf::Instruction* CBackEnd::Sext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return C::Sext(type, result, value, toType);
}

Ir::Intf::Instruction* CBackEnd::Fptrunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return C::Fptrunc(type, result, value, toType);
}

Ir::Intf::Instruction* CBackEnd::Fpext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return C::Fpext(type, result, value, toType);
}

Ir::Intf::Instruction* CBackEnd::Fptoui(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return C::Fptoui(type, result, value, toType);
}

Ir::Intf::Instruction* CBackEnd::Fptosi(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return C::Fptosi(type, result, value, toType);
}

Ir::Intf::Instruction* CBackEnd::Uitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return C::Uitofp(type, result, value, toType);
}

Ir::Intf::Instruction* CBackEnd::Sitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return C::Sitofp(type, result, value, toType);
}

Ir::Intf::Instruction* CBackEnd::Ptrtoint(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return C::Ptrtoint(type, result, value, toType);
}

Ir::Intf::Instruction* CBackEnd::Inttoptr(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return C::Inttoptr(type, result, value, toType);
}

Ir::Intf::Instruction* CBackEnd::Bitcast(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType)
{
    return C::Bitcast(type, result, value, toType);
}

Ir::Intf::Instruction* CBackEnd::DbgDeclare(Ir::Intf::Object* variable, Ir::Intf::MetadataNode* descriptor)
{
    throw std::runtime_error("member function not applicable to C backend");
}

void CBackEnd::Init(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* from, Ir::Intf::Object* to)
{
    C::Init(emitter, type, from, to);
}

void CBackEnd::Assign(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* from, Ir::Intf::Object* to)
{
    C::Assign(emitter, type, from, to);
}

Ir::Intf::Object* CBackEnd::SizeOf(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type)
{
    return C::SizeOf(emitter, type);
}

} } // namespace Cm::IrIntf
