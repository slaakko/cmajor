/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_IRINTF_CBACKEND_INCLUDED
#define CM_IRINTF_CBACKEND_INCLUDED
#include <Cm.IrIntf/BackEnd.hpp>

namespace Cm { namespace IrIntf {

class CBackEnd : public BackEndIntf
{
public:
    std::string GetExCodeVarName() override;
    std::string GetIrecPtrName() override;
    Ir::Intf::LabelObject* CreateTempLabel(int tempLabelCounter) override;
    std::string CreateTempVarName(int tempVarCounter) override;
    Ir::Intf::LabelObject* CreateNextLocalLabel() override;
    Ir::Intf::LabelObject* CreateLabel() override;
    Ir::Intf::LabelObject* CreateLabel(const std::string& label) override;
    Ir::Intf::LabelObject* CreateLandingPadLabel(int index) override;
    Ir::Intf::RegVar* CreateTemporaryRegVar(Ir::Intf::Type* type) override;
    Ir::Intf::Type* I1() override;
    Ir::Intf::Type* I8() override;
    Ir::Intf::Type* I16() override;
    Ir::Intf::Type* I32() override;
    Ir::Intf::Type* I64() override;
    Ir::Intf::Type* UI8() override;
    Ir::Intf::Type* UI16() override;
    Ir::Intf::Type* UI32() override;
    Ir::Intf::Type* UI64() override;
    Ir::Intf::Type* Float() override;
    Ir::Intf::Type* Double() override;
    Ir::Intf::Type* Void() override;
    Ir::Intf::Type* Char() override;
    Ir::Intf::Type* RvalueRef(Ir::Intf::Type* baseType) override;
    Ir::Intf::Type* Pointer(Ir::Intf::Type* baseType, int numPointers) override;
    Ir::Intf::Type* Structure(const std::string& tagName, const std::vector<Ir::Intf::Type*>& elementTypes, const std::vector<std::string>& elementNames) override;
    Ir::Intf::Type* Array(Ir::Intf::Type* itemType, int size) override;
    Ir::Intf::Type* CreateFunctionType(Ir::Intf::Type* returnType, const std::vector<Ir::Intf::Type*>& parameterTypes) override;
    Ir::Intf::Type* CreateTypedef(const std::string& name, Ir::Intf::Type* type) override;
    Ir::Intf::Function* CreateFunction(const std::string& name, Ir::Intf::Type* returnType, const std::vector<Ir::Intf::Parameter*>& parameters) override;
    Ir::Intf::Function* CreateDoNothingFunction() override;
    Ir::Intf::Function* CreateDbgDeclareFunction() override;
    Ir::Intf::Function* CreateMemSetFunction(Ir::Intf::Type* i8Ptr) override;
    Ir::Intf::Function* CreateMemCopyFunction(Ir::Intf::Type* i8Ptr) override;
    Ir::Intf::Instruction* MemSet(Ir::Intf::Object* dest, Ir::Intf::Object* value, Ir::Intf::Object* len, int align, bool isVolatile) override;
    Ir::Intf::Instruction* MemCopy(Ir::Intf::Object* dest, Ir::Intf::Object* source, Ir::Intf::Object* len, int align, bool isVolatile) override;
    void ResetLocalLabelCounter() override;
    std::string MakeAssemblyName(const std::string& name) override;
    std::string MakeClassNameAssemblyName(const std::string& fullClassName) override;
    Ir::Intf::Type* CreateClassTypeName(const std::string& fullClassName) override;
    bool TypesEqual(Ir::Intf::Type* left, Ir::Intf::Type* right) override;
    Ir::Intf::Object* True() override;
    Ir::Intf::Object* False() override;
    Ir::Intf::Object* CreateConstant(const std::string& name, Ir::Intf::Type* type) override;
    Ir::Intf::Object* CreateBooleanConstant(bool value) override;
    Ir::Intf::Object* CreateCharConstant(char value) override;
    std::string GetSeparator() override;
    std::string GetPrivateSeparator() override;
    std::string GetStringValuePrefix() override;
    Ir::Intf::Object* CreateStringConstant(const std::string& value) override;
    Ir::Intf::Object* Null(Ir::Intf::Type* ptrType) override;
    Ir::Intf::Object* CreateI8Constant(int8_t value) override;
    Ir::Intf::Object* CreateUI8Constant(uint8_t value) override;
    Ir::Intf::Object* CreateI16Constant(int16_t value) override;
    Ir::Intf::Object* CreateUI16Constant(uint16_t value) override;
    Ir::Intf::Object* CreateI32Constant(int32_t value) override;
    Ir::Intf::Object* CreateUI32Constant(uint32_t value) override;
    Ir::Intf::Object* CreateI64Constant(int64_t value) override;
    Ir::Intf::Object* CreateUI64Constant(uint64_t value) override;
    Ir::Intf::Object* CreateFloatConstant(float value) override;
    Ir::Intf::Object* CreateDoubleConstant(double value) override;
    std::string MakeStringConstantName(const std::string& s) override;
    std::string MakeStringConstantName(const std::string& s, bool metadataSyntax) override;
    std::string MakeDestructionNodeName(const std::string& assemblyName) override;
    std::string GetDestructionNodeTypeName() override;
    std::string GetRegisterDestructorFunctionName() override;
    Ir::Intf::Type* CreateTypeName(const std::string& name, bool global) override;
    Ir::Intf::MemberVar* CreateMemberVar(const std::string& memberVarName, Ir::Intf::Object* ptr, int index, Ir::Intf::Type* type) override;
    Ir::Intf::Object* CreateStackVar(const std::string& assemblyName, Ir::Intf::Type* type) override;
    Ir::Intf::Object* CreateRefVar(const std::string& assemblyName, Ir::Intf::Type* type) override;
    std::string GetVPtrVarName() override;
    Ir::Intf::MetadataStringPtr CreateMetadataString(const std::string& s) override;
    Ir::Intf::MetadataNode* CreateMetadataNode(int id) override;
    Ir::Intf::Object* CreateMetadataNull() override;
    std::string GetStackFrameIdentifier() override;
    int GetDebugTagLexicalBlock() override;
    int GetDebugTagCompileUnit() override;
    int GetDebugTagSubprogram() override;
    int GetDebugTagFileType() override;
    int GetDebugTagSubroutineType() override;
    int GetDebugTagBaseType() override;
    int GetDebugTagClassType() override;
    int GetDebugTagEnumerationType() override;
    int GetDebugTagEnumerator() override;
    int GetDebugTagInheritance() override;
    int GetDebugTagPointerType() override;
    int GetDebugTagConstType() override;
    int GetDebugTagReferenceType() override;
    int GetDebugTagRvalueRefType() override;
    int GetDebugTagNamespace() override;
    int GetDebugTagAutoVariable() override;
    int GetDebugTagArgVariable() override;
    int GetDebugTagMember() override;
    int GetDebugLangCmajor() override;
    Ir::Intf::GlobalPtr CreateGlobal(const std::string& name, Ir::Intf::Type* type) override;
    Ir::Intf::Parameter* CreatePointerParameter(const std::string& name, Ir::Intf::Type* type) override;
    Ir::Intf::Parameter* CreateParameter(const std::string& parameterName, Ir::Intf::Type* parameterType) override;
    std::string GetClassObjectResultParamName() override;
    std::string GetExceptionCodeParamName() override;
    std::string GetExceptionBaseIdTableName() override;
    std::string GetClassHierarchyTableName() override;
    int GetDebugAttrEncodingBoolean() override;
    int GetDebugAttrEncodingUnsigned() override;
    int GetDebugAttrEncodingSigned() override;
    int GetDebugAttrEncodingSignedChar() override;
    int GetDebugAttrEncodingUnsignedChar() override;
    int GetDebugAttrEncodingFloat() override;
    Ir::Intf::Instruction* Add(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* Sub(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* Mul(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* UDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* SDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* URem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* SRem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* Shl(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* LShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* AShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* And(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* Or(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* Xor(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* FAdd(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* FSub(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* FMul(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* FDiv(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* FRem(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* Index(Ir::Intf::Object* result, Ir::Intf::Object* array_, Ir::Intf::Object* index) override;
    Ir::Intf::Instruction* Ret() override;
    Ir::Intf::Instruction* Ret(Ir::Intf::Object* value) override;
    Ir::Intf::Instruction* Br(Ir::Intf::LabelObject* dest) override;
    Ir::Intf::Instruction* Br(Ir::Intf::Object* cond, Ir::Intf::LabelObject* trueLabel, Ir::Intf::LabelObject* falseLabel) override;
    Ir::Intf::Instruction* Switch(Ir::Intf::Type* integerType, Ir::Intf::Object* value, Ir::Intf::LabelObject* defaultDest, const std::vector<std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>>& destinations) override;
    Ir::Intf::Instruction* Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result) override;
    Ir::Intf::Instruction* Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Type* numElementsType, int numElements) override;
    Ir::Intf::Instruction* Load(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* ptr) override;
    Ir::Intf::Instruction* Load(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Indirection leftIndirection, Ir::Intf::Indirection rightIndirection) override;
    Ir::Intf::Instruction* Store(Ir::Intf::Type* type, Ir::Intf::Object* value, Ir::Intf::Object* ptr) override;
    Ir::Intf::Instruction* Store(Ir::Intf::Type* type, Ir::Intf::Object* value, Ir::Intf::Object* ptr, Ir::Intf::Indirection leftIndirection, Ir::Intf::Indirection rightIndirection) override;
    Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index) override;
    Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index, Ir::Intf::Object* index1) override;
    Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index, const std::vector<Ir::Intf::Object*>& indeces) override;
    Ir::Intf::Instruction* Call(Ir::Intf::Object* result, Ir::Intf::Function* fun, const std::vector<Ir::Intf::Object*>& args) override;
    Ir::Intf::Instruction* IndirectCall(Ir::Intf::Object* result, Ir::Intf::Object* funPtr, const std::vector<Ir::Intf::Object*>& args) override;
    Ir::Intf::Instruction* ICmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::IConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* FCmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::FConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) override;
    Ir::Intf::Instruction* DoNothing() override;
    Ir::Intf::Instruction* Trunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) override;
    Ir::Intf::Instruction* Zext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) override;
    Ir::Intf::Instruction* Sext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) override;
    Ir::Intf::Instruction* Fptrunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) override;
    Ir::Intf::Instruction* Fpext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) override;
    Ir::Intf::Instruction* Fptoui(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) override;
    Ir::Intf::Instruction* Fptosi(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) override;
    Ir::Intf::Instruction* Uitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) override;
    Ir::Intf::Instruction* Sitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) override;
    Ir::Intf::Instruction* Ptrtoint(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) override;
    Ir::Intf::Instruction* Inttoptr(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) override;
    Ir::Intf::Instruction* Bitcast(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) override;
    Ir::Intf::Instruction* DbgDeclare(Ir::Intf::Object* variable, Ir::Intf::MetadataNode* descriptor) override;
    void Init(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* from, Ir::Intf::Object* to) override;
    void Assign(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* from, Ir::Intf::Object* to) override;
    Ir::Intf::Object* SizeOf(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type) override;
};

BackEndIntf* GetCBackEnd();
Ir::Intf::Factory* GetCFactory();

} } // namespace Cm::IrIntf

#endif // CM_IRINTF_CBACKEND_INCLUDED
