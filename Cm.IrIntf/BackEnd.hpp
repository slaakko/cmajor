/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_IRINTF_BACKEND_INCLUDED
#define CM_IRINTF_BACKEND_INCLUDED
#include <Cm.IrIntf/Fwd.hpp>
#include <Ir.Intf/Interface.hpp>
#include <Ir.Intf/Factory.hpp>

namespace Cm { namespace IrIntf {

enum class BackEnd
{
    llvm, c
};

void SetBackEnd(BackEnd backEnd);
BackEnd GetBackEnd();
std::string GetBackEndStr();

class BackEndIntf
{
public:
    virtual ~BackEndIntf();
    virtual std::string GetExCodeVarName() = 0;
    virtual std::string GetIrecPtrName() = 0;
    virtual Ir::Intf::LabelObject* CreateTempLabel(int tempLabelCounter) = 0;
    virtual std::string CreateTempVarName(int tempVarCounter) = 0;
    virtual Ir::Intf::LabelObject* CreateNextLocalLabel() = 0;
    virtual Ir::Intf::LabelObject* CreateLabel() = 0;
    virtual Ir::Intf::LabelObject* CreateLabel(const std::string& label) = 0;
    virtual Ir::Intf::LabelObject* CreateLandingPadLabel(int index) = 0;
    virtual Ir::Intf::RegVar* CreateTemporaryRegVar(Ir::Intf::Type* type) = 0;
    virtual Ir::Intf::Type* I1() = 0;
    virtual Ir::Intf::Type* I8() = 0;
    virtual Ir::Intf::Type* I16() = 0;
    virtual Ir::Intf::Type* I32() = 0;
    virtual Ir::Intf::Type* I64() = 0;
    virtual Ir::Intf::Type* UI8() = 0;
    virtual Ir::Intf::Type* UI16() = 0;
    virtual Ir::Intf::Type* UI32() = 0;
    virtual Ir::Intf::Type* UI64() = 0;
    virtual Ir::Intf::Type* Float() = 0;
    virtual Ir::Intf::Type* Double() = 0;
    virtual Ir::Intf::Type* Void() = 0;
    virtual Ir::Intf::Type* Char() = 0;
    virtual Ir::Intf::Type* RvalueRef(Ir::Intf::Type* baseType) = 0;
    virtual Ir::Intf::Type* Pointer(Ir::Intf::Type* baseType, int numPointers) = 0;
    virtual Ir::Intf::Type* Structure(const std::string& tagName, const std::vector<Ir::Intf::Type*>& elementTypes, const std::vector<std::string>& elementNames) = 0;
    virtual Ir::Intf::Type* Array(Ir::Intf::Type* itemType, int size) = 0;
    virtual Ir::Intf::Type* CreateFunctionType(Ir::Intf::Type* returnType, const std::vector<Ir::Intf::Type*>& parameterTypes) = 0;
    virtual Ir::Intf::Type* CreateTypedef(const std::string& name, Ir::Intf::Type* type) = 0;
    virtual Ir::Intf::Function* CreateFunction(const std::string& name, Ir::Intf::Type* returnType, const std::vector<Ir::Intf::Parameter*>& parameters) = 0;
    virtual Ir::Intf::Function* CreateDoNothingFunction() = 0;
    virtual Ir::Intf::Function* CreateDbgDeclareFunction() = 0;
    virtual Ir::Intf::Function* CreateMemSetFunction(Ir::Intf::Type* i8Ptr) = 0;
    virtual Ir::Intf::Function* CreateMemCopyFunction(Ir::Intf::Type* i8Ptr) = 0;
    virtual Ir::Intf::Instruction* MemSet(Ir::Intf::Object* dest, Ir::Intf::Object* value, Ir::Intf::Object* len, int align, bool isVolatile) = 0;
    virtual Ir::Intf::Instruction* MemCopy(Ir::Intf::Object* dest, Ir::Intf::Object* source, Ir::Intf::Object* len, int align, bool isVolatile) = 0;
    virtual void ResetLocalLabelCounter() = 0;
    virtual std::string MakeAssemblyName(const std::string& name) = 0;
    virtual std::string MakeClassNameAssemblyName(const std::string& fullClassName) = 0;
    virtual Ir::Intf::Type* CreateClassTypeName(const std::string& fullClassName) = 0;
    virtual bool TypesEqual(Ir::Intf::Type* left, Ir::Intf::Type* right) = 0;
    virtual Ir::Intf::Object* True() = 0;
    virtual Ir::Intf::Object* False() = 0;
    virtual Ir::Intf::Object* CreateConstant(const std::string& name, Ir::Intf::Type* type) = 0;
    virtual Ir::Intf::Object* CreateBooleanConstant(bool value) = 0;
    virtual Ir::Intf::Object* CreateCharConstant(char value) = 0;
    virtual std::string GetSeparator() = 0;
    virtual std::string GetPrivateSeparator() = 0;
    virtual std::string GetStringValuePrefix() = 0;
    virtual Ir::Intf::Object* CreateStringConstant(const std::string& value) = 0;
    virtual Ir::Intf::Object* Null(Ir::Intf::Type* ptrType) = 0;
    virtual Ir::Intf::Object* CreateI8Constant(int8_t value) = 0;
    virtual Ir::Intf::Object* CreateUI8Constant(uint8_t value) = 0;
    virtual Ir::Intf::Object* CreateI16Constant(int16_t value) = 0;
    virtual Ir::Intf::Object* CreateUI16Constant(uint16_t value) = 0;
    virtual Ir::Intf::Object* CreateI32Constant(int32_t value) = 0;
    virtual Ir::Intf::Object* CreateUI32Constant(uint32_t value) = 0;
    virtual Ir::Intf::Object* CreateI64Constant(int64_t value) = 0;
    virtual Ir::Intf::Object* CreateUI64Constant(uint64_t value) = 0;
    virtual Ir::Intf::Object* CreateFloatConstant(float value) = 0;
    virtual Ir::Intf::Object* CreateDoubleConstant(double value) = 0;
    virtual std::string MakeStringConstantName(const std::string& s) = 0;
    virtual std::string MakeStringConstantName(const std::string& s, bool metadataSyntax) = 0;
    virtual std::string MakeDestructionNodeName(const std::string& assemblyName) = 0;
    virtual std::string GetDestructionNodeTypeName() = 0;
    virtual std::string GetRegisterDestructorFunctionName() = 0;
    virtual Ir::Intf::Type* CreateTypeName(const std::string& name, bool global) = 0;
    virtual Ir::Intf::MemberVar* CreateMemberVar(const std::string& memberVarName, Ir::Intf::Object* ptr, int index, Ir::Intf::Type* type) = 0;
    virtual Ir::Intf::Object* CreateStackVar(const std::string& assemblyName, Ir::Intf::Type* type) = 0;
    virtual Ir::Intf::Object* CreateRefVar(const std::string& assemblyName, Ir::Intf::Type* type) = 0;
    virtual std::string GetVPtrVarName() = 0;
    virtual Ir::Intf::MetadataStringPtr CreateMetadataString(const std::string& s) = 0;
    virtual Ir::Intf::MetadataNode* CreateMetadataNode(int id) = 0;
    virtual Ir::Intf::Object* CreateMetadataNull() = 0;
    virtual std::string GetStackFrameIdentifier() = 0;
    virtual int GetDebugTagLexicalBlock() = 0;
    virtual int GetDebugTagCompileUnit() = 0;
    virtual int GetDebugTagSubprogram() = 0;
    virtual int GetDebugTagFileType() = 0;
    virtual int GetDebugTagSubroutineType() = 0;
    virtual int GetDebugTagBaseType() = 0;
    virtual int GetDebugTagClassType() = 0;
    virtual int GetDebugTagEnumerationType() = 0;
    virtual int GetDebugTagEnumerator() = 0;
    virtual int GetDebugTagInheritance() = 0;
    virtual int GetDebugTagPointerType() = 0;
    virtual int GetDebugTagConstType() = 0;
    virtual int GetDebugTagReferenceType() = 0;
    virtual int GetDebugTagRvalueRefType() = 0;
    virtual int GetDebugTagNamespace() = 0;
    virtual int GetDebugTagAutoVariable() = 0;
    virtual int GetDebugTagArgVariable() = 0;
    virtual int GetDebugTagMember() = 0;
    virtual int GetDebugLangCmajor() = 0;
    virtual Ir::Intf::GlobalPtr CreateGlobal(const std::string& name, Ir::Intf::Type* type) = 0;
    virtual Ir::Intf::Parameter* CreatePointerParameter(const std::string& name, Ir::Intf::Type* type) = 0;
    virtual Ir::Intf::Parameter* CreateParameter(const std::string& parameterName, Ir::Intf::Type* parameterType) = 0;
    virtual std::string GetClassObjectResultParamName() = 0;
    virtual std::string GetExceptionCodeParamName() = 0;
    virtual std::string GetExceptionBaseIdTableName() = 0;
    virtual std::string GetClassHierarchyTableName() = 0;
    virtual int GetDebugAttrEncodingBoolean() = 0;
    virtual int GetDebugAttrEncodingUnsigned() = 0;
    virtual int GetDebugAttrEncodingSigned() = 0;
    virtual int GetDebugAttrEncodingSignedChar() = 0;
    virtual int GetDebugAttrEncodingUnsignedChar() = 0;
    virtual int GetDebugAttrEncodingFloat() = 0;
    virtual Ir::Intf::Instruction* Add(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* Sub(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* Mul(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* UDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* SDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* URem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* SRem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* Shl(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* LShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* AShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* And(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* Or(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* Xor(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* FAdd(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* FSub(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* FMul(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* FDiv(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* FRem(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* Index(Ir::Intf::Object* result, Ir::Intf::Object* array_, Ir::Intf::Object* index) = 0;
    virtual Ir::Intf::Instruction* Ret() = 0;
    virtual Ir::Intf::Instruction* Ret(Ir::Intf::Object* value) = 0;
    virtual Ir::Intf::Instruction* Br(Ir::Intf::LabelObject* dest) = 0;
    virtual Ir::Intf::Instruction* Br(Ir::Intf::Object* cond, Ir::Intf::LabelObject* trueLabel, Ir::Intf::LabelObject* falseLabel) = 0;
    virtual Ir::Intf::Instruction* Switch(Ir::Intf::Type* integerType, Ir::Intf::Object* value, Ir::Intf::LabelObject* defaultDest, const std::vector<std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>>& destinations) = 0;
    virtual Ir::Intf::Instruction* Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result) = 0;
    virtual Ir::Intf::Instruction* Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Type* numElementsType, int numElements) = 0;
    virtual Ir::Intf::Instruction* Load(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* ptr) = 0;
    virtual Ir::Intf::Instruction* Load(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Indirection leftIndirection, Ir::Intf::Indirection rightIndirection) = 0;
    virtual Ir::Intf::Instruction* Store(Ir::Intf::Type* type, Ir::Intf::Object* value, Ir::Intf::Object* ptr) = 0;
    virtual Ir::Intf::Instruction* Store(Ir::Intf::Type* type, Ir::Intf::Object* value, Ir::Intf::Object* ptr, Ir::Intf::Indirection leftIndirection, Ir::Intf::Indirection rightIndirection) = 0;
    virtual Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index) = 0;
    virtual Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index, Ir::Intf::Object* index1) = 0;
    virtual Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index, const std::vector<Ir::Intf::Object*>& indeces) = 0;
    virtual Ir::Intf::Instruction* Call(Ir::Intf::Object* result, Ir::Intf::Function* fun, const std::vector<Ir::Intf::Object*>& args) = 0;
    virtual Ir::Intf::Instruction* IndirectCall(Ir::Intf::Object* result, Ir::Intf::Object* funPtr, const std::vector<Ir::Intf::Object*>& args) = 0;
    virtual Ir::Intf::Instruction* ICmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::IConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* FCmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::FConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) = 0;
    virtual Ir::Intf::Instruction* DoNothing() = 0;
    virtual Ir::Intf::Instruction* Trunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) = 0;
    virtual Ir::Intf::Instruction* Zext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) = 0;
    virtual Ir::Intf::Instruction* Sext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) = 0;
    virtual Ir::Intf::Instruction* Fptrunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) = 0;
    virtual Ir::Intf::Instruction* Fpext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) = 0;
    virtual Ir::Intf::Instruction* Fptoui(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) = 0;
    virtual Ir::Intf::Instruction* Fptosi(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) = 0;
    virtual Ir::Intf::Instruction* Uitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) = 0;
    virtual Ir::Intf::Instruction* Sitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) = 0;
    virtual Ir::Intf::Instruction* Ptrtoint(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) = 0;
    virtual Ir::Intf::Instruction* Inttoptr(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) = 0;
    virtual Ir::Intf::Instruction* Bitcast(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) = 0;
    virtual Ir::Intf::Instruction* DbgDeclare(Ir::Intf::Object* variable, Ir::Intf::MetadataNode* descriptor) = 0;
    virtual void Init(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* from, Ir::Intf::Object* to) = 0;
    virtual void Assign(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* from, Ir::Intf::Object* to) = 0;
    virtual Ir::Intf::Object* SizeOf(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type) = 0;
};

BackEndIntf* GetBackEndImpl();

} } // namespace Cm::IrIntf

#endif // CM_IRINTF_BACKEND_INCLUDED
