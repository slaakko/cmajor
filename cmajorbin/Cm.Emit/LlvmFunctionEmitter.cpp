/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Emit/LlvmFunctionEmitter.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/GlobalFlags.hpp>

namespace Cm { namespace Emit {

LlvmFunctionEmitter::LlvmFunctionEmitter(Cm::Util::CodeFormatter& codeFormatter_, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_,
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::StringRepository& stringRepository_, Cm::BoundTree::BoundClass* currentClass_,
    std::unordered_set<std::string>& internalFunctionNames_, std::unordered_set<Ir::Intf::Function*>& externalFunctions_,
    Cm::Core::StaticMemberVariableRepository& staticMemberVariableRepository_, Cm::Core::ExternalConstantRepository& externalConstantRepository_,
    Cm::Ast::CompileUnitNode* currentCompileUnit_, Cm::Sym::FunctionSymbol* enterFrameFun_, Cm::Sym::FunctionSymbol* leaveFrameFun_, Cm::Sym::FunctionSymbol* enterTracedCalllFun_,
    Cm::Sym::FunctionSymbol* leaveTracedCallFun_, bool profile_) : FunctionEmitter(codeFormatter_, typeRepository_, irFunctionRepository_, irClassTypeRepository_, stringRepository_, currentClass_,
    internalFunctionNames_, externalFunctions_, staticMemberVariableRepository_, externalConstantRepository_, currentCompileUnit_, enterFrameFun_, leaveFrameFun_, enterTracedCalllFun_,
    leaveTracedCallFun_, false, profile_)
{
}

void LlvmFunctionEmitter::EmitDummyVar(Cm::Core::Emitter* emitter)
{
    // nothing to do for LLVM backend
}

void LlvmFunctionEmitter::SetStringLiteralResult(Cm::Core::Emitter* emitter, Ir::Intf::Object* resultObject, Ir::Intf::Object* stringConstant, Ir::Intf::Object* stringObject)
{
    Ir::Intf::Object* zero = Cm::IrIntf::CreateI32Constant(0);
    emitter->Own(zero);
    Ir::Intf::Type* s = Cm::IrIntf::Pointer(stringConstant->GetType(), 1);
    emitter->Own(s);
    emitter->Emit(Cm::IrIntf::GetElementPtr(s, resultObject, stringObject, zero, zero));
}

void LlvmFunctionEmitter::Visit(Cm::BoundTree::BoundDynamicTypeNameExpression& boundDynamicTypeNameExpression)
{
    Cm::Core::Emitter* emitter = Emitter();
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter, GenFlags()));
    boundDynamicTypeNameExpression.Subject()->Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> subjectResult = ResultStack().Pop();
    Ir::Intf::LabelObject* resultLabel = subjectResult->GetLabel();
    if (resultLabel)
    {
        result->SetLabel(resultLabel);
    }
    Cm::Sym::ClassTypeSymbol* classType = boundDynamicTypeNameExpression.ClassType();
    Ir::Intf::Type* classTypePtrType = Cm::IrIntf::Pointer(classType->GetIrType(), 1);
    emitter->Own(classTypePtrType);
    Ir::Intf::Object* objectPtr = subjectResult->MainObject();
    Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 1);
    emitter->Own(i8Ptr);
    Ir::Intf::Type* i8PtrPtr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 2);
    emitter->Own(i8PtrPtr);
    Ir::Intf::Object* vtblPtrContainerPtr = objectPtr;
    int vptrIndex = classType->VPtrIndex();
    if (vptrIndex == -1)
    {
        Cm::Sym::ClassTypeSymbol* vptrContainerClass = classType->VPtrContainerClass();
        vptrIndex = vptrContainerClass->VPtrIndex();
        Ir::Intf::Type* containerPtrType = Cm::IrIntf::Pointer(vptrContainerClass->GetIrType(), 1);
        emitter->Own(containerPtrType);
        Ir::Intf::RegVar* containerPtr = Cm::IrIntf::CreateTemporaryRegVar(containerPtrType);
        emitter->Own(containerPtr);
        emitter->Emit(Cm::IrIntf::Bitcast(classTypePtrType, containerPtr, objectPtr, containerPtrType));
        vtblPtrContainerPtr = containerPtr;
    }
    Ir::Intf::MemberVar* vptr = Cm::IrIntf::CreateMemberVar(Cm::IrIntf::GetVPtrVarName(), vtblPtrContainerPtr, vptrIndex, i8PtrPtr);
    emitter->Own(vptr);
    Ir::Intf::RegVar* loadedVptr = Cm::IrIntf::CreateTemporaryRegVar(i8PtrPtr);
    emitter->Own(loadedVptr);
    Cm::IrIntf::Assign(*emitter, i8PtrPtr, vptr, loadedVptr);
    Ir::Intf::RegVar* i8rttiPtr = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
    emitter->Own(i8rttiPtr);
    Cm::IrIntf::Assign(*emitter, i8Ptr, loadedVptr, i8rttiPtr);
    std::vector<Ir::Intf::Type*> rttiElementTypes;
    rttiElementTypes.push_back(i8Ptr->Clone());
    rttiElementTypes.push_back(Cm::IrIntf::UI64());
    std::vector<std::string> rttiElementNames;
    rttiElementNames.push_back("class_name");
    rttiElementNames.push_back("class_id");
    Ir::Intf::Type* rttiPtrIrType(Cm::IrIntf::Pointer(Cm::IrIntf::Structure("rtti_", rttiElementTypes, rttiElementNames), 1));
    emitter->Own(rttiPtrIrType);
    Ir::Intf::RegVar* rttiPtr = Cm::IrIntf::CreateTemporaryRegVar(rttiPtrIrType);
    emitter->Own(rttiPtr);
    emitter->Emit(Cm::IrIntf::Bitcast(i8Ptr, rttiPtr, i8rttiPtr, rttiPtrIrType));
    Ir::Intf::RegVar* typenameI8PtrPtr = Cm::IrIntf::CreateTemporaryRegVar(i8PtrPtr);
    emitter->Own(typenameI8PtrPtr);
    Ir::Intf::Object* zero = Ir::Intf::GetFactory()->GetI32()->CreateDefaultValue();
    emitter->Own(zero);
    emitter->Emit(Cm::IrIntf::GetElementPtr(rttiPtrIrType, typenameI8PtrPtr, rttiPtr, zero, zero));
    Ir::Intf::RegVar* loadedTypenameI8Ptr = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
    emitter->Own(loadedTypenameI8Ptr);
    Cm::IrIntf::Assign(*emitter, i8Ptr, typenameI8PtrPtr, loadedTypenameI8Ptr);
    result->SetMainObject(loadedTypenameI8Ptr);
    result->Merge(subjectResult);
    ResultStack().Push(result);
}

void LlvmFunctionEmitter::Visit(Cm::BoundTree::BoundIsExpression& boundIsExpression)
{
    Cm::Core::Emitter* emitter = Emitter();
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter, GenFlags()));
    boundIsExpression.Expr()->Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> exprResult = ResultStack().Pop();
    Ir::Intf::LabelObject* resultLabel = exprResult->GetLabel();
    if (resultLabel)
    {
        result->SetLabel(resultLabel);
    }
    Cm::Sym::ClassTypeSymbol* leftClassType = boundIsExpression.LeftClassType();
    Ir::Intf::Type* classTypePtrType = Cm::IrIntf::Pointer(leftClassType->GetIrType(), 1);
    emitter->Own(classTypePtrType);
    Ir::Intf::Object* objectPtr = exprResult->MainObject();
    Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 1);
    emitter->Own(i8Ptr);
    Ir::Intf::Type* i8PtrPtr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 2);
    emitter->Own(i8PtrPtr);
    Ir::Intf::Object* vtblPtrContainerPtr = objectPtr;
    int vptrIndex = leftClassType->VPtrIndex();
    if (vptrIndex == -1)
    {
        Cm::Sym::ClassTypeSymbol* vptrContainerClass = leftClassType->VPtrContainerClass();
        vptrIndex = vptrContainerClass->VPtrIndex();
        Ir::Intf::Type* containerPtrType = Cm::IrIntf::Pointer(vptrContainerClass->GetIrType(), 1);
        emitter->Own(containerPtrType);
        Ir::Intf::RegVar* containerPtr = Cm::IrIntf::CreateTemporaryRegVar(containerPtrType);
        emitter->Own(containerPtr);
        emitter->Emit(Cm::IrIntf::Bitcast(classTypePtrType, containerPtr, objectPtr, containerPtrType));
        vtblPtrContainerPtr = containerPtr;
    }
    Ir::Intf::MemberVar* vptr = Cm::IrIntf::CreateMemberVar(Cm::IrIntf::GetVPtrVarName(), vtblPtrContainerPtr, vptrIndex, i8PtrPtr);
    emitter->Own(vptr);
    Ir::Intf::RegVar* loadedVptr = Cm::IrIntf::CreateTemporaryRegVar(i8PtrPtr);
    emitter->Own(loadedVptr);
    Cm::IrIntf::Assign(*emitter, i8PtrPtr, vptr, loadedVptr);
    Ir::Intf::RegVar* i8rttiPtr = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
    emitter->Own(i8rttiPtr);
    Cm::IrIntf::Assign(*emitter, i8Ptr, loadedVptr, i8rttiPtr);
    std::vector<Ir::Intf::Type*> rttiElementTypes;
    rttiElementTypes.push_back(i8Ptr->Clone());
    rttiElementTypes.push_back(Cm::IrIntf::UI64());
    std::vector<std::string> rttiElementNames;
    rttiElementNames.push_back("class_name");
    rttiElementNames.push_back("class_id");
    Ir::Intf::Type* rttiPtrIrType(Cm::IrIntf::Pointer(Cm::IrIntf::Structure("rtti_", rttiElementTypes, rttiElementNames), 1));
    emitter->Own(rttiPtrIrType);
    Ir::Intf::RegVar* rttiPtr = Cm::IrIntf::CreateTemporaryRegVar(rttiPtrIrType);
    emitter->Own(rttiPtr);
    emitter->Emit(Cm::IrIntf::Bitcast(i8Ptr, rttiPtr, i8rttiPtr, rttiPtrIrType));
    Ir::Intf::Type* cidPtrType = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetUI64(), 1);
    emitter->Own(cidPtrType);
    Ir::Intf::RegVar* cidPtr = Cm::IrIntf::CreateTemporaryRegVar(cidPtrType);
    emitter->Own(cidPtr);
    Ir::Intf::Object* zero = Ir::Intf::GetFactory()->GetI32()->CreateDefaultValue();
    emitter->Own(zero);
    Ir::Intf::Object* one = Ir::Intf::GetFactory()->GetI32()->CreatePlusOne();
    emitter->Own(one);
    emitter->Emit(Cm::IrIntf::GetElementPtr(rttiPtrIrType, cidPtr, rttiPtr, zero, one));
    Ir::Intf::RegVar* leftCid = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetUI64());
    emitter->Own(leftCid);
    Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetUI64(), cidPtr, leftCid);
    Cm::Sym::ClassTypeSymbol* rightClassType = boundIsExpression.RightClassType();
    Cm::BoundTree::BoundLiteral rightCidLiteral(boundIsExpression.SyntaxNode());
    rightCidLiteral.SetValue(new Cm::Sym::ULongValue(rightClassType->Cid()));
    Cm::Sym::TypeSymbol* ulongType = GetSymbolTable()->GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ulongId));
    rightCidLiteral.SetType(ulongType);
    rightCidLiteral.Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> rightCidResult = ResultStack().Pop();
    Ir::Intf::Object* rightCid = rightCidResult->MainObject();
    Ir::Intf::RegVar* resultRegVar = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI1());
    emitter->Own(resultRegVar);
    if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::fullConfig))
    {
        Ir::Intf::RegVar* remainderResult = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI64());
        emitter->Emit(Cm::IrIntf::URem(Ir::Intf::GetFactory()->GetUI64(), remainderResult, leftCid, rightCid));
        Ir::Intf::Object* ui64Zero = Ir::Intf::GetFactory()->GetUI64()->CreateDefaultValue();
        emitter->Own(ui64Zero);
        emitter->Emit(Cm::IrIntf::ICmp(Ir::Intf::GetFactory()->GetUI64(), resultRegVar, Ir::Intf::IConditionCode::eq, remainderResult, ui64Zero));
    }
    else
    {
        Cm::Sym::FunctionSymbol* fun = GetSymbolTable()->GetOverload("is_class_same_or_derived_from");
        std::shared_ptr<Cm::Core::GenResult> funCallResult(new Cm::Core::GenResult(emitter, GenFlags()));
        funCallResult->SetMainObject(resultRegVar);
        funCallResult->AddObject(leftCid);
        funCallResult->AddObject(rightCid);
        GenerateCall(fun, nullptr, *funCallResult);
    }
    result->SetMainObject(resultRegVar);
    result->Merge(exprResult);
    result->Merge(rightCidResult);
    if (boundIsExpression.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        GenJumpingBoolCode(*result);
    }
    ResultStack().Push(result);
}

void LlvmFunctionEmitter::Visit(Cm::BoundTree::BoundAsExpression& boundAsExpression)
{
    Cm::Core::Emitter* emitter = Emitter();
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter, GenFlags()));
    boundAsExpression.Expr()->Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> exprResult = ResultStack().Pop();
    Ir::Intf::LabelObject* resultLabel = exprResult->GetLabel();
    if (resultLabel)
    {
        result->SetLabel(resultLabel);
    }
    Cm::Sym::ClassTypeSymbol* leftClassType = boundAsExpression.LeftClassType();
    Ir::Intf::Type* classTypePtrType = Cm::IrIntf::Pointer(leftClassType->GetIrType(), 1);
    emitter->Own(classTypePtrType);
    Ir::Intf::Object* objectPtr = exprResult->MainObject();
    Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 1);
    emitter->Own(i8Ptr);
    Ir::Intf::Type* i8PtrPtr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 2);
    emitter->Own(i8PtrPtr);
    Ir::Intf::Object* vtblPtrContainerPtr = objectPtr;
    int vptrIndex = leftClassType->VPtrIndex();
    if (vptrIndex == -1)
    {
        Cm::Sym::ClassTypeSymbol* vptrContainerClass = leftClassType->VPtrContainerClass();
        vptrIndex = vptrContainerClass->VPtrIndex();
        Ir::Intf::Type* containerPtrType = Cm::IrIntf::Pointer(vptrContainerClass->GetIrType(), 1);
        emitter->Own(containerPtrType);
        Ir::Intf::RegVar* containerPtr = Cm::IrIntf::CreateTemporaryRegVar(containerPtrType);
        emitter->Own(containerPtr);
        emitter->Emit(Cm::IrIntf::Bitcast(classTypePtrType, containerPtr, objectPtr, containerPtrType));
        vtblPtrContainerPtr = containerPtr;
    }
    Ir::Intf::MemberVar* vptr = Cm::IrIntf::CreateMemberVar(Cm::IrIntf::GetVPtrVarName(), vtblPtrContainerPtr, vptrIndex, i8PtrPtr);
    emitter->Own(vptr);
    Ir::Intf::RegVar* loadedVptr = Cm::IrIntf::CreateTemporaryRegVar(i8PtrPtr);
    emitter->Own(loadedVptr);
    Cm::IrIntf::Assign(*emitter, i8PtrPtr, vptr, loadedVptr);
    Ir::Intf::RegVar* i8rttiPtr = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
    emitter->Own(i8rttiPtr);
    Cm::IrIntf::Assign(*emitter, i8Ptr, loadedVptr, i8rttiPtr);
    std::vector<Ir::Intf::Type*> rttiElementTypes;
    rttiElementTypes.push_back(i8Ptr->Clone());
    rttiElementTypes.push_back(Cm::IrIntf::UI64());
    std::vector<std::string> rttiElementNames;
    rttiElementNames.push_back("class_name");
    rttiElementNames.push_back("class_id");
    Ir::Intf::Type* rttiPtrIrType(Cm::IrIntf::Pointer(Cm::IrIntf::Structure("rtti_", rttiElementTypes, rttiElementNames), 1));
    emitter->Own(rttiPtrIrType);
    Ir::Intf::RegVar* rttiPtr = Cm::IrIntf::CreateTemporaryRegVar(rttiPtrIrType);
    emitter->Own(rttiPtr);
    emitter->Emit(Cm::IrIntf::Bitcast(i8Ptr, rttiPtr, i8rttiPtr, rttiPtrIrType));
    Ir::Intf::Type* cidPtrType = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetUI64(), 1);
    emitter->Own(cidPtrType);
    Ir::Intf::RegVar* cidPtr = Cm::IrIntf::CreateTemporaryRegVar(cidPtrType);
    emitter->Own(cidPtr);
    Ir::Intf::Object* zero = Ir::Intf::GetFactory()->GetI32()->CreateDefaultValue();
    emitter->Own(zero);
    Ir::Intf::Object* one = Ir::Intf::GetFactory()->GetI32()->CreatePlusOne();
    emitter->Own(one);
    emitter->Emit(Cm::IrIntf::GetElementPtr(rttiPtrIrType, cidPtr, rttiPtr, zero, one));
    Ir::Intf::RegVar* leftCid = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetUI64());
    emitter->Own(leftCid);
    Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetUI64(), cidPtr, leftCid);
    Cm::Sym::ClassTypeSymbol* rightClassType = boundAsExpression.RightClassType();
    Cm::BoundTree::BoundLiteral rightCidLiteral(boundAsExpression.SyntaxNode());
    rightCidLiteral.SetValue(new Cm::Sym::ULongValue(rightClassType->Cid()));
    Cm::Sym::TypeSymbol* ulongType = GetSymbolTable()->GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ulongId));
    rightCidLiteral.SetType(ulongType);
    rightCidLiteral.Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> rightCidResult = ResultStack().Pop();
    Ir::Intf::Object* rightCid = rightCidResult->MainObject();
    Ir::Intf::RegVar* resultRegVar = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI1());
    emitter->Own(resultRegVar);
    if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::fullConfig))
    {
        Ir::Intf::RegVar* remainderResult = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI64());
        emitter->Emit(Cm::IrIntf::URem(Ir::Intf::GetFactory()->GetUI64(), remainderResult, leftCid, rightCid));
        Ir::Intf::Object* ui64Zero = Ir::Intf::GetFactory()->GetUI64()->CreateDefaultValue();
        emitter->Own(ui64Zero);
        emitter->Emit(Cm::IrIntf::ICmp(Ir::Intf::GetFactory()->GetUI64(), resultRegVar, Ir::Intf::IConditionCode::eq, remainderResult, ui64Zero));
    }
    else
    {
        Cm::Sym::FunctionSymbol* fun = GetSymbolTable()->GetOverload("is_class_same_or_derived_from");
        std::shared_ptr<Cm::Core::GenResult> funCallResult(new Cm::Core::GenResult(emitter, GenFlags()));
        funCallResult->SetMainObject(resultRegVar);
        funCallResult->AddObject(leftCid);
        funCallResult->AddObject(rightCid);
        GenerateCall(fun, nullptr, *funCallResult);
    }
    Ir::Intf::LabelObject* trueLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter->Own(trueLabel);
    Ir::Intf::LabelObject* falseLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter->Own(falseLabel);
    Ir::Intf::LabelObject* exitLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter->Own(exitLabel);
    emitter->Emit(Cm::IrIntf::Br(resultRegVar, trueLabel, falseLabel));
    Cm::BoundTree::BoundExpression* temporary = boundAsExpression.BoundTemporary();
    temporary->Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> temporaryResult = ResultStack().Pop();
    emitter->AddNextInstructionLabel(trueLabel);
    Ir::Intf::Object* exprAsRightOperand = Cm::IrIntf::CreateTemporaryRegVar(boundAsExpression.GetType()->GetIrType());
    emitter->Emit(Cm::IrIntf::Bitcast(exprResult->MainObject()->GetType(), exprAsRightOperand, exprResult->MainObject(), boundAsExpression.GetType()->GetIrType()));
    emitter->Emit(Cm::IrIntf::Store(boundAsExpression.GetType()->GetIrType(), exprAsRightOperand, temporaryResult->MainObject()));
    emitter->Emit(Cm::IrIntf::Br(exitLabel));
    Ir::Intf::Object* null = boundAsExpression.GetType()->GetIrType()->CreateDefaultValue();
    emitter->AddNextInstructionLabel(falseLabel);
    emitter->Emit(Cm::IrIntf::Store(boundAsExpression.GetType()->GetIrType(), null, temporaryResult->MainObject()));
    emitter->AddNextInstructionLabel(exitLabel);
    result->SetMainObject(GetSymbolTable()->GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), GetSymbolTable()->GetTypeRepository());
    DoNothing(*result);
    result->SetMainObject(temporaryResult->MainObject());
    result->Merge(exprResult);
    result->Merge(rightCidResult);
    result->Merge(temporaryResult);
    ResultStack().Push(result);
}

void LlvmFunctionEmitter::DoNothing(Cm::Core::GenResult& genResult)
{
    GenerateCall(nullptr, IrFunctionRepository().GetDoNothingFunction(), nullptr, genResult, false);
}

void LlvmFunctionEmitter::Visit(Cm::BoundTree::BoundInitVPtrStatement& boundInitVPtrStatement)
{
    Cm::Core::Emitter* emitter = Emitter();
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter, GenFlags()));
    Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 1);
    emitter->Own(i8Ptr);
    Ir::Intf::Type* i8PtrPtr = Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 2);
    emitter->Own(i8PtrPtr);
    Cm::Sym::ClassTypeSymbol* classType = boundInitVPtrStatement.ClassType();
    Cm::BoundTree::BoundParameter boundThisParam(nullptr, ThisParam());
    boundThisParam.Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> thisResult = ResultStack().Pop();
    int16_t vptrIndex = classType->VPtrIndex();
    Ir::Intf::Object* vptrContainerPtr = thisResult->MainObject();
    if (vptrIndex == -1)
    {
        Cm::Sym::ClassTypeSymbol* vptrContainingType = classType->VPtrContainerClass();
        vptrIndex = vptrContainingType->VPtrIndex();
        Ir::Intf::Type* vptrContainingTypeIrType = vptrContainingType->GetIrType();
        Ir::Intf::Type* vptrContainingTypePtrType = Cm::IrIntf::Pointer(vptrContainingTypeIrType, 1);
        emitter->Own(vptrContainingTypePtrType);
        Ir::Intf::RegVar* containerPtr = Cm::IrIntf::CreateTemporaryRegVar(vptrContainingTypePtrType);
        emitter->Own(containerPtr);
        Ir::Intf::Type* classTypeIrType = classType->GetIrType();
        Ir::Intf::Type* classTypePtrType = Cm::IrIntf::Pointer(classTypeIrType, 1);
        emitter->Own(classTypePtrType);
        emitter->Emit(Cm::IrIntf::Bitcast(classTypePtrType, containerPtr, thisResult->MainObject(), vptrContainingTypePtrType));
        vptrContainerPtr = containerPtr;
    }
    Ir::Intf::MemberVar* vptr = Cm::IrIntf::CreateMemberVar(Cm::IrIntf::GetVPtrVarName(), vptrContainerPtr, vptrIndex, i8PtrPtr);
    emitter->Own(vptr);
    Ir::Intf::RegVar* vtblAddrAsI8PtrPtr = Cm::IrIntf::CreateTemporaryRegVar(i8PtrPtr);
    emitter->Own(vtblAddrAsI8PtrPtr);
    Ir::Intf::Type* vtblAddrType = Cm::IrIntf::Pointer(Cm::IrIntf::Array(i8Ptr->Clone(), int(classType->Vtbl().size())), 1);
    emitter->Own(vtblAddrType);
    std::string vtblName = Cm::IrIntf::MakeAssemblyName(classType->FullName() + Cm::IrIntf::GetPrivateSeparator() + "vtbl");
    Ir::Intf::Object* vtblObject = Cm::IrIntf::CreateGlobal(vtblName, vtblAddrType);
    emitter->Own(vtblObject);
    emitter->Emit(Cm::IrIntf::Bitcast(vtblAddrType, vtblAddrAsI8PtrPtr, vtblObject, i8PtrPtr));
    Cm::IrIntf::Assign(*emitter, i8PtrPtr, vtblAddrAsI8PtrPtr, vptr);
    result->Merge(thisResult);
    ResultStack().Push(result);
}

void LlvmFunctionEmitter::RegisterDestructor(Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol)
{
    Cm::Core::Emitter* emitter = Emitter();
    Ir::Intf::Object* irObject = StaticMemberVariableRepository().GetStaticMemberVariableIrObject(staticMemberVariableSymbol);
    Ir::Intf::Object* destructionNode = StaticMemberVariableRepository().GetDestructionNode(staticMemberVariableSymbol);
    Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 1);
    emitter->Own(i8Ptr);
    Ir::Intf::Type* i8PtrPtr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 2);
    emitter->Own(i8PtrPtr);
    Ir::Intf::RegVar* objectFieldPtr = Cm::IrIntf::CreateTemporaryRegVar(i8PtrPtr);
    emitter->Own(objectFieldPtr);
    Ir::Intf::Object* zero = Cm::IrIntf::CreateI32Constant(0);
    emitter->Own(zero);
    Ir::Intf::Object* one = Cm::IrIntf::CreateI32Constant(1);
    emitter->Own(one);
    emitter->Emit(Cm::IrIntf::GetElementPtr(destructionNode->GetType(), objectFieldPtr, destructionNode, zero, one));
    Ir::Intf::Object* irObjectAsI8Ptr = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
    emitter->Own(irObjectAsI8Ptr);
    emitter->Emit(Cm::IrIntf::Bitcast(irObject->GetType(), irObjectAsI8Ptr, irObject, i8Ptr));
    emitter->Emit(Cm::IrIntf::Store(i8Ptr, irObjectAsI8Ptr, objectFieldPtr));

    Cm::Sym::TypeSymbol* type = staticMemberVariableSymbol->GetType();
    if (type->IsClassTypeSymbol())
    {
        Cm::Sym::ClassTypeSymbol* classType = static_cast<Cm::Sym::ClassTypeSymbol*>(type);
        if (classType->Destructor())
        {
            Cm::Sym::FunctionSymbol* destructor = classType->Destructor();
            Ir::Intf::Function* destructorIrFun = IrFunctionRepository().CreateIrFunction(destructor);
            Ir::Intf::Type* destructorPtrType = IrFunctionRepository().GetFunPtrIrType(destructor);
            std::vector<Ir::Intf::Type*> dtorParamTypes1(1, i8Ptr->Clone());
            Ir::Intf::Type* destructorFieldType = Cm::IrIntf::Pointer(Cm::IrIntf::CreateFunctionType(Cm::IrIntf::Void(), dtorParamTypes1), 1);
            emitter->Own(destructorFieldType);
            std::vector<Ir::Intf::Type*> dtorParamTypes2(1, i8Ptr->Clone());
            Ir::Intf::Type* destructorFieldPtrType = Cm::IrIntf::Pointer(Cm::IrIntf::CreateFunctionType(Cm::IrIntf::Void(), dtorParamTypes2), 2);
            emitter->Own(destructorFieldPtrType);
            Ir::Intf::RegVar* destructorFieldPtr = Cm::IrIntf::CreateTemporaryRegVar(destructorFieldPtrType);
            emitter->Own(destructorFieldPtr);
            Ir::Intf::Object* two = Cm::IrIntf::CreateI32Constant(2);
            emitter->Own(two);
            emitter->Emit(Cm::IrIntf::GetElementPtr(destructionNode->GetType(), destructorFieldPtr, destructionNode, zero, two));
            Ir::Intf::Global* dtor(Cm::IrIntf::CreateGlobal(destructorIrFun->Name(), destructorPtrType));
            emitter->Own(dtor);
            Ir::Intf::RegVar* dtorPtr = Cm::IrIntf::CreateTemporaryRegVar(destructorFieldType);
            emitter->Own(dtorPtr);
            emitter->Emit(Cm::IrIntf::Bitcast(destructorPtrType, dtorPtr, dtor, destructorFieldType));
            emitter->Emit(Cm::IrIntf::Store(destructorFieldType, dtorPtr, destructorFieldPtr));

            std::vector<Ir::Intf::Parameter*> registerFunParams;
            Ir::Intf::Parameter* param = Cm::IrIntf::CreateParameter("node", destructionNode->GetType()->Clone());
            emitter->Own(param);
            registerFunParams.push_back(param);
            Ir::Intf::Function* registerFun = Cm::IrIntf::CreateFunction(Cm::IrIntf::GetRegisterDestructorFunctionName(), Ir::Intf::GetFactory()->GetVoid(), registerFunParams);
            emitter->Own(registerFun);
            std::vector<Ir::Intf::Object*> registerFunArgs;
            registerFunArgs.push_back(destructionNode);
            Ir::Intf::RegVar* result = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetVoid());
            emitter->Own(result);
            emitter->Emit(Cm::IrIntf::Call(result, registerFun, registerFunArgs));
        }
    }
}

void LlvmFunctionEmitter::GenVirtualCall(Cm::Sym::FunctionSymbol* fun, Cm::Core::GenResult& memberFunctionResult)
{
    Cm::Core::Emitter* emitter = Emitter();
    Ir::Intf::Object* objectPtr = memberFunctionResult.Arg1();
    Cm::Sym::ClassTypeSymbol* classType = fun->Class();
    Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 1);
    emitter->Own(i8Ptr);
    Ir::Intf::Type* i8PtrPtr = Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 2);
    emitter->Own(i8PtrPtr);
    Ir::Intf::Object* vptrContainerPtr = objectPtr;
    int16_t vptrIndex = classType->VPtrIndex();
    if (vptrIndex == -1)
    {
        Cm::Sym::ClassTypeSymbol* vptrContainingType = classType->VPtrContainerClass();
        vptrIndex = vptrContainingType->VPtrIndex();
        Ir::Intf::Type* vptrContainingPtrIrType = Cm::IrIntf::Pointer(vptrContainingType->GetIrType(), 1);
        emitter->Own(vptrContainingPtrIrType);
        Ir::Intf::RegVar* containerPtr = Cm::IrIntf::CreateTemporaryRegVar(vptrContainingPtrIrType);
        emitter->Own(containerPtr);
        Ir::Intf::Type* classTypePtrIrType = Cm::IrIntf::Pointer(classType->GetIrType(), 1);
        emitter->Own(classTypePtrIrType);
        emitter->Emit(Cm::IrIntf::Bitcast(classTypePtrIrType, containerPtr, objectPtr, vptrContainingPtrIrType));
        vptrContainerPtr = containerPtr;
    }
    Ir::Intf::MemberVar* vptr = Cm::IrIntf::CreateMemberVar(Cm::IrIntf::GetVPtrVarName(), vptrContainerPtr, vptrIndex, i8PtrPtr);
    emitter->Own(vptr);
    Ir::Intf::RegVar* loadedVptr = Cm::IrIntf::CreateTemporaryRegVar(i8PtrPtr);
    emitter->Own(loadedVptr);
    Cm::IrIntf::Assign(*emitter, i8PtrPtr, vptr, loadedVptr);
    Ir::Intf::RegVar* functionI8PtrPtr = Cm::IrIntf::CreateTemporaryRegVar(i8PtrPtr);
    emitter->Own(functionI8PtrPtr);
    Ir::Intf::Object* functionIndex = Cm::IrIntf::CreateI16Constant(fun->VtblIndex());
    emitter->Own(functionIndex);
    emitter->Emit(Cm::IrIntf::GetElementPtr(i8PtrPtr, functionI8PtrPtr, loadedVptr, functionIndex));
    Ir::Intf::RegVar* loadedFunctionI8Ptr = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
    emitter->Own(loadedFunctionI8Ptr);
    Cm::IrIntf::Assign(*emitter, i8Ptr, functionI8PtrPtr, loadedFunctionI8Ptr);
    Ir::Intf::Type* functionPtrType = IrFunctionRepository().GetFunPtrIrType(fun);
    Ir::Intf::RegVar* loadedFunctionPtr = Cm::IrIntf::CreateTemporaryRegVar(functionPtrType);
    emitter->Own(loadedFunctionPtr);
    emitter->Emit(Cm::IrIntf::Bitcast(i8Ptr, loadedFunctionPtr, loadedFunctionI8Ptr, functionPtrType));
    emitter->Emit(Cm::IrIntf::IndirectCall(memberFunctionResult.MainObject(), loadedFunctionPtr, memberFunctionResult.Args()));
}

Ir::Intf::LabelObject* LlvmFunctionEmitter::CreateLandingPadLabel(int landingPadId)
{
    return Cm::IrIntf::CreateLabel("$P" + std::to_string(landingPadId));
}

void LlvmFunctionEmitter::MapIrFunToFun(Ir::Intf::Function* irFun, Cm::Sym::FunctionSymbol* fun)
{
    // nothing to do for LLVM backend
}

Ir::Intf::Object* LlvmFunctionEmitter::MakeLocalVarIrObject(Cm::Sym::TypeSymbol* type, Ir::Intf::Object* source)
{
    return source;
}

void LlvmFunctionEmitter::SetCallDebugInfoInfo(Ir::Intf::Instruction* callInst, Ir::Intf::Function* fun)
{
    // nothing to do for LLVM backend
}

Cm::Core::CfgNode* LlvmFunctionEmitter::CreateDebugNode(Cm::BoundTree::BoundStatement& statement, const Cm::Parsing::Span& span, bool addToPrevNodes)
{
    // nothing to do for LLVM backend
    return nullptr;
}

void LlvmFunctionEmitter::CreateDebugNode(Cm::BoundTree::BoundExpression& expr, const Cm::Parsing::Span& span)
{
    // nothing to do for LLVM backend
}

void LlvmFunctionEmitter::AddDebugNodeTransition(Cm::BoundTree::BoundStatement& fromStatement, Cm::BoundTree::BoundStatement& toStatement)
{
    // nothing to do for LLVM backend
}

void LlvmFunctionEmitter::AddDebugNodeTransition(Cm::BoundTree::BoundExpression& fromExpression, Cm::BoundTree::BoundStatement& toStatement)
{
    // nothing to do for LLVM backend
}

void LlvmFunctionEmitter::AddDebugNodeTransition(Cm::BoundTree::BoundExpression& fromExpression, Cm::BoundTree::BoundExpression& toExpression)
{
    // nothing to do for LLVM backend
}

int LlvmFunctionEmitter::RetrievePrevDebugNodes()
{
    // nothing to do for LLVM backend
    return -1;
}

void LlvmFunctionEmitter::AddToPrevDebugNodes(int debugNodeSetHandle)
{
    // nothing to do for LLVM backend
}

void LlvmFunctionEmitter::AddToPrevDebugNodes(Cm::BoundTree::BoundStatement& statement)
{
    // nothing to do for LLVM backend
}

void LlvmFunctionEmitter::AddToPrevDebugNodes(Cm::BoundTree::BoundExpression& expr)
{
    // nothing to do for LLVM backend
}

void LlvmFunctionEmitter::AddToPrevDebugNodes(const std::unordered_set<Cm::Core::CfgNode*>& nodeSet)
{
    // nothing to do for LLVM backend
}

void LlvmFunctionEmitter::CreateEntryDebugNode(Cm::BoundTree::BoundStatement& statement, const Cm::Parsing::Span& span)
{
    // nothing to do for LLVM backend
}

void LlvmFunctionEmitter::CreateExitDebugNode(Cm::BoundTree::BoundStatement& statement, const Cm::Parsing::Span& span)
{
    // nothing to do for LLVM backend
}

void LlvmFunctionEmitter::PatchPrevDebugNodes(Cm::BoundTree::BoundStatement& statement)
{
    // nothing to do for LLVM backend
}

void LlvmFunctionEmitter::SetCfgNode(Cm::BoundTree::BoundStatement& fromStatement, Cm::BoundTree::BoundStatement& toStatement)
{
    // nothing to do for LLVM backend
}

void LlvmFunctionEmitter::PatchDebugNodes(const std::unordered_set<Cm::Core::CfgNode*>& nodeSet, Cm::Core::CfgNode* nextNode)
{
    // nothing to do for LLVM backend
}

} } // namespace Cm::Emit
