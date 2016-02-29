/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_IRINTF_REP_INCLUDED
#define CM_IRINTF_REP_INCLUDED
#include <Cm.IrIntf/BackEnd.hpp>

namespace Cm { namespace IrIntf {

std::string GetExCodeVarName();
std::string GetIrecPtrName();
Ir::Intf::LabelObject* CreateTempLabel(int tempLabelCounter);
std::string CreateTempVarName(int tempVarCounter);
Ir::Intf::LabelObject* CreateNextLocalLabel();
Ir::Intf::LabelObject* CreateLabel();
Ir::Intf::LabelObject* CreateLabel(const std::string& label);
Ir::Intf::LabelObject* CreateLandingPadLabel(int index);
Ir::Intf::RegVar* CreateTemporaryRegVar(Ir::Intf::Type* type);
Ir::Intf::Type* I1();
Ir::Intf::Type* I8();
Ir::Intf::Type* I16();
Ir::Intf::Type* I32();
Ir::Intf::Type* I64();
Ir::Intf::Type* UI8();
Ir::Intf::Type* UI16();
Ir::Intf::Type* UI32();
Ir::Intf::Type* UI64();
Ir::Intf::Type* Float();
Ir::Intf::Type* Double();
Ir::Intf::Type* Void();
Ir::Intf::Type* Char();
Ir::Intf::Type* RvalueRef(Ir::Intf::Type* baseType);
Ir::Intf::Type* Pointer(Ir::Intf::Type* baseType, int numPointers);
Ir::Intf::Type* Structure(const std::string& tagName, const std::vector<Ir::Intf::Type*>& elementTypes, const std::vector<std::string>& elementNames);
Ir::Intf::Type* Array(Ir::Intf::Type* itemType, int size);
Ir::Intf::Type* CreateFunctionType(Ir::Intf::Type* returnType, const std::vector<Ir::Intf::Type*>& parameterTypes);
std::string CreateDelegateTypeName();
Ir::Intf::Type* CreateTypedef(const std::string& name, Ir::Intf::Type* type);
Ir::Intf::Function* CreateFunction(const std::string& name, Ir::Intf::Type* returnType, const std::vector<Ir::Intf::Parameter*>& parameters);
Ir::Intf::Function* CreateDoNothingFunction();
Ir::Intf::Function* CreateDbgDeclareFunction();
Ir::Intf::Function* CreateMemSetFunction(Ir::Intf::Type* i8Ptr);
Ir::Intf::Function* CreateMemCopyFunction(Ir::Intf::Type* i8Ptr);
Ir::Intf::Instruction* MemSet(Ir::Intf::Object* dest, Ir::Intf::Object* value, Ir::Intf::Object* len, int align, bool isVolatile);
Ir::Intf::Instruction* MemCopy(Ir::Intf::Object* dest, Ir::Intf::Object* source, Ir::Intf::Object* len, int align, bool isVolatile);
void ResetLocalLabelCounter();
bool TypesEqual(Ir::Intf::Type* left, Ir::Intf::Type* right);
std::string MakeAssemblyName(const std::string& name);
std::string MakeClassNameAssemblyName(const std::string& fullClassName);
Ir::Intf::Type* CreateClassTypeName(const std::string& fullClassName);
Ir::Intf::Object* True();
Ir::Intf::Object* False();
Ir::Intf::Object* CreateConstant(const std::string& name, Ir::Intf::Type* type);
Ir::Intf::Object* CreateBooleanConstant(bool value);
Ir::Intf::Object* CreateCharConstant(char value);
std::string GetStringValuePrefix();
std::string GetSeparator();
std::string GetPrivateSeparator();
Ir::Intf::Object* CreateStringConstant(const std::string& value);
Ir::Intf::Object* Null(Ir::Intf::Type* ptrType);
Ir::Intf::Object* CreateI8Constant(int8_t value);
Ir::Intf::Object* CreateI16Constant(int16_t value);
Ir::Intf::Object* CreateI32Constant(int32_t value);
Ir::Intf::Object* CreateI64Constant(int64_t value);
Ir::Intf::Object* CreateUI8Constant(uint8_t value);
Ir::Intf::Object* CreateUI16Constant(uint16_t value);
Ir::Intf::Object* CreateUI32Constant(uint32_t value);
Ir::Intf::Object* CreateUI64Constant(uint64_t value);
Ir::Intf::Object* CreateFloatConstant(float value);
Ir::Intf::Object* CreateDoubleConstant(double value);
std::string MakeStringConstantName(const std::string& s);
std::string MakeStringConstantName(const std::string& s, bool metadataSyntax);
std::string MakeDestructionNodeName(const std::string& assemblyName);
std::string GetDestructionNodeTypeName();
std::string GetRegisterDestructorFunctionName();
Ir::Intf::Type* CreateTypeName(const std::string& name, bool global);
Ir::Intf::MemberVar* CreateMemberVar(const std::string& memberVarName, Ir::Intf::Object* ptr, int index, Ir::Intf::Type* type);
Ir::Intf::Object* CreateStackVar(const std::string& assemblyName, Ir::Intf::Type* type);
Ir::Intf::Object* CreateRefVar(const std::string& assemblyName, Ir::Intf::Type* type);
std::string GetVPtrVarName();
Ir::Intf::GlobalPtr CreateGlobal(const std::string& name, Ir::Intf::Type* type);
Ir::Intf::Parameter* CreatePointerParameter(const std::string& name, Ir::Intf::Type* type);
Ir::Intf::Parameter* CreateParameter(const std::string& parameterName, Ir::Intf::Type* parameterType);
Ir::Intf::MetadataStringPtr CreateMetadataString(const std::string& s);
Ir::Intf::MetadataNode* CreateMetadataNode(int id);
std::string GetStackFrameIdentifier();
int GetDebugTagLexicalBlock();
int GetDebugTagCompileUnit();
int GetDebugTagSubprogram();
int GetDebugTagFileType();
int GetDebugTagSubroutineType();
int GetDebugTagBaseType();
int GetDebugTagClassType();
int GetDebugTagEnumerationType();
int GetDebugTagEnumerator();
int GetDebugTagInheritance();
int GetDebugTagPointerType();
int GetDebugTagConstType();
int GetDebugTagReferenceType();
int GetDebugTagRvalueRefType();
int GetDebugTagNamespace();
int GetDebugTagAutoVariable();
int GetDebugTagArgVariable();
int GetDebugTagMember();
int GetDebugLangCmajor();
Ir::Intf::Object* CreateMetadataNull();
std::string GetClassObjectResultParamName();
std::string GetExceptionCodeParamName();
std::string GetExceptionBaseIdTableName();
std::string GetClassHierarchyTableName();
int GetDebugAttrEncodingBoolean();
int GetDebugAttrEncodingUnsigned();
int GetDebugAttrEncodingSigned();
int GetDebugAttrEncodingSignedChar();
int GetDebugAttrEncodingUnsignedChar();
int GetDebugAttrEncodingFloat();
Ir::Intf::Instruction* Add(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* Sub(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* Mul(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* UDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* SDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* URem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* SRem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* Shl(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* LShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* AShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* And(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* Or(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* Xor(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* FAdd(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* FSub(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* FMul(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* FDiv(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* FRem(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* Index(Ir::Intf::Object* result, Ir::Intf::Object* array_, Ir::Intf::Object* index);
Ir::Intf::Instruction* Ret();
Ir::Intf::Instruction* Ret(Ir::Intf::Object* value);
Ir::Intf::Instruction* Br(Ir::Intf::LabelObject* dest);
Ir::Intf::Instruction* Br(Ir::Intf::Object* cond, Ir::Intf::LabelObject* trueLabel, Ir::Intf::LabelObject* falseLabel);
Ir::Intf::Instruction* Switch(Ir::Intf::Type* integerType, Ir::Intf::Object* value, Ir::Intf::LabelObject* defaultDest, const std::vector<std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>>& destinations);
Ir::Intf::Instruction* Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result);
Ir::Intf::Instruction* Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Type* numElementsType, int numElements);
Ir::Intf::Instruction* Load(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* ptr);
Ir::Intf::Instruction* Load(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Indirection leftIndirection, Ir::Intf::Indirection rightIndirection);
Ir::Intf::Instruction* Store(Ir::Intf::Type* type, Ir::Intf::Object* value, Ir::Intf::Object* ptr);
Ir::Intf::Instruction* Store(Ir::Intf::Type* type, Ir::Intf::Object* value, Ir::Intf::Object* ptr, Ir::Intf::Indirection leftIndirection, Ir::Intf::Indirection rightIndirection);
Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index);
Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index, Ir::Intf::Object* index1);
Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index, const std::vector<Ir::Intf::Object*>& indeces);
Ir::Intf::Instruction* Call(Ir::Intf::Object* result, Ir::Intf::Function* fun, const std::vector<Ir::Intf::Object*>& args);
Ir::Intf::Instruction* IndirectCall(Ir::Intf::Object* result, Ir::Intf::Object* funPtr, const std::vector<Ir::Intf::Object*>& args);
Ir::Intf::Instruction* ICmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::IConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* FCmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::FConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);
Ir::Intf::Instruction* DoNothing();
Ir::Intf::Instruction* Trunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);
Ir::Intf::Instruction* Zext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);
Ir::Intf::Instruction* Sext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);
Ir::Intf::Instruction* Fptrunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);
Ir::Intf::Instruction* Fpext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);
Ir::Intf::Instruction* Fptoui(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);
Ir::Intf::Instruction* Fptosi(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);
Ir::Intf::Instruction* Uitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);
Ir::Intf::Instruction* Sitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);
Ir::Intf::Instruction* Ptrtoint(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);
Ir::Intf::Instruction* Inttoptr(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);
Ir::Intf::Instruction* Bitcast(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);
Ir::Intf::Instruction* DbgDeclare(Ir::Intf::Object* variable, Ir::Intf::MetadataNode* descriptor);
void Init(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* from, Ir::Intf::Object* to);
void Assign(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* from, Ir::Intf::Object* to);
Ir::Intf::Object* SizeOf(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type);

} } // namespace Cm::IrIntf

#endif // CM_IR_REP_INCLUDED
