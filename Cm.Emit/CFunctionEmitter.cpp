/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Emit/CFunctionEmitter.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Emit {

CFunctionEmitter::CFunctionEmitter(Cm::Util::CodeFormatter& codeFormatter_, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_,
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::StringRepository& stringRepository_, Cm::BoundTree::BoundClass* currentClass_,
    std::unordered_set<std::string>& internalFunctionNames_, std::unordered_set<Ir::Intf::Function*>& externalFunctions_,
    Cm::Core::StaticMemberVariableRepository& staticMemberVariableRepository_, Cm::Core::ExternalConstantRepository& externalConstantRepository_,
    Cm::Ast::CompileUnitNode* currentCompileUnit_, Cm::Sym::FunctionSymbol* enterFrameFun_, Cm::Sym::FunctionSymbol* leaveFrameFun_, Cm::Sym::FunctionSymbol* enterTracedCalllFun_,
    Cm::Sym::FunctionSymbol* leaveTracedCallFun_) : FunctionEmitter(codeFormatter_, typeRepository_, irFunctionRepository_, irClassTypeRepository_, stringRepository_, currentClass_,
    internalFunctionNames_, externalFunctions_, staticMemberVariableRepository_, externalConstantRepository_, currentCompileUnit_, enterFrameFun_, leaveFrameFun_, enterTracedCalllFun_,
    leaveTracedCallFun_), functionMap(nullptr)
{
}

void CFunctionEmitter::EmitDummyVar(Cm::Core::Emitter* emitter)
{
    Ir::Intf::Object* dummyVariable = Cm::IrIntf::CreateStackVar("_X_dummy", Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI1(), 1));
    emitter->Own(dummyVariable);
    emitter->Emit(Cm::IrIntf::Alloca(Ir::Intf::GetFactory()->GetI1(), dummyVariable));
}

void CFunctionEmitter::SetStringLiteralResult(Cm::Core::Emitter* emitter, Ir::Intf::Object* resultObject, Ir::Intf::Object* stringConstant, Ir::Intf::Object* stringObject)
{
    Cm::IrIntf::Assign(*emitter, stringConstant->GetType(), stringObject, resultObject);
}

void CFunctionEmitter::Visit(Cm::BoundTree::BoundDynamicTypeNameExpression& boundDynamiceTypeNameExpression)
{
    Cm::Core::Emitter* emitter = Emitter();
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter, GenFlags()));
    boundDynamiceTypeNameExpression.Subject()->Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> subjectResult = ResultStack().Pop();
    Ir::Intf::LabelObject* resultLabel = subjectResult->GetLabel();
    if (resultLabel)
    {
        result->SetLabel(resultLabel);
    }
    Cm::Sym::ClassTypeSymbol* classType = boundDynamiceTypeNameExpression.ClassType();
    Ir::Intf::Type* classTypePtrType = Cm::IrIntf::Pointer(classType->GetIrType(), 1);
    emitter->Own(classTypePtrType);
    Ir::Intf::Object* objectPtr = subjectResult->MainObject();
    Ir::Intf::Type* voidPtr(Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 1));
    emitter->Own(voidPtr);
    Ir::Intf::Type* voidPtrPtr(Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 2));
    emitter->Own(voidPtrPtr);
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
    Ir::Intf::MemberVar* vptr = Cm::IrIntf::CreateMemberVar(Cm::IrIntf::GetVPtrVarName(), vtblPtrContainerPtr, vptrIndex, voidPtrPtr);
    emitter->Own(vptr);
    Ir::Intf::RegVar* loadedVptr = Cm::IrIntf::CreateTemporaryRegVar(voidPtrPtr);
    emitter->Own(loadedVptr);
    Cm::IrIntf::Assign(*emitter, voidPtrPtr, vptr, loadedVptr);
    Ir::Intf::Type* charPtrPtrType = Cm::IrIntf::Pointer(Cm::IrIntf::Char(), 2);
    emitter->Own(charPtrPtrType);
    Ir::Intf::RegVar* typenameCharPtrPtr = Cm::IrIntf::CreateTemporaryRegVar(charPtrPtrType);
    emitter->Own(typenameCharPtrPtr);
    emitter->Emit(Cm::IrIntf::Bitcast(voidPtrPtr, typenameCharPtrPtr, loadedVptr, charPtrPtrType));
    Ir::Intf::Type* charPtrType = Cm::IrIntf::Pointer(Cm::IrIntf::Char(), 1);
    emitter->Own(charPtrType);
    Ir::Intf::RegVar* loadedTypenameCharPtr = Cm::IrIntf::CreateTemporaryRegVar(charPtrType);
    emitter->Own(loadedTypenameCharPtr);
    Cm::IrIntf::Assign(*emitter, charPtrType, typenameCharPtrPtr, loadedTypenameCharPtr);
    result->SetMainObject(loadedTypenameCharPtr);
    result->Merge(subjectResult);
    ResultStack().Push(result);
}

void CFunctionEmitter::DoNothing(Cm::Core::GenResult& genResult)
{
    Emitter()->Emit(Cm::IrIntf::DoNothing());
}

void CFunctionEmitter::Visit(Cm::BoundTree::BoundInitVPtrStatement& boundInitVPtrStatement)
{
    Cm::Core::Emitter* emitter = Emitter();
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter, GenFlags()));
    Ir::Intf::Type* voidPtr = Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 1);
    emitter->Own(voidPtr);
    Ir::Intf::Type* voidPtrPtr = Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 2);
    emitter->Own(voidPtrPtr);
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
    Ir::Intf::MemberVar* vptr = Cm::IrIntf::CreateMemberVar(Cm::IrIntf::GetVPtrVarName(), vptrContainerPtr, vptrIndex, voidPtrPtr);
    emitter->Own(vptr);
    Ir::Intf::RegVar* vtblAsVoidPtrPtr = Cm::IrIntf::CreateTemporaryRegVar(voidPtrPtr);
    emitter->Own(vtblAsVoidPtrPtr);
    Ir::Intf::Type* vtblAddrType = Cm::IrIntf::Pointer(Cm::IrIntf::Array(voidPtr->Clone(), int(classType->Vtbl().size())), 1);
    emitter->Own(vtblAddrType);
    std::string vtblName = Cm::IrIntf::MakeAssemblyName(classType->FullName() + Cm::IrIntf::GetPrivateSeparator() + "vtbl");
    Ir::Intf::Object* vtblObject = Cm::IrIntf::CreateGlobal(vtblName, vtblAddrType);
    emitter->Own(vtblObject);
    emitter->Emit(Cm::IrIntf::Bitcast(vtblAddrType, vtblAsVoidPtrPtr, vtblObject, voidPtrPtr));
    Cm::IrIntf::Assign(*emitter, voidPtrPtr, vtblAsVoidPtrPtr, vptr);
    result->Merge(thisResult);
    ResultStack().Push(result);
}

void CFunctionEmitter::RegisterDestructor(Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol)
{
    Cm::Core::Emitter* emitter = Emitter();
    Ir::Intf::Object* irObject = StaticMemberVariableRepository().GetStaticMemberVariableIrObject(staticMemberVariableSymbol);
    Ir::Intf::Object* destructionNode = StaticMemberVariableRepository().GetDestructionNode(staticMemberVariableSymbol);
    Ir::Intf::Type* voidPtr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetVoid(), 1);
    emitter->Own(voidPtr);
    Ir::Intf::MemberVar* objectField = Cm::IrIntf::CreateMemberVar("cls", destructionNode, 1, voidPtr);
    objectField->SetDotMember();
    emitter->Emit(Cm::IrIntf::Store(voidPtr, irObject, objectField, Ir::Intf::Indirection::addr, Ir::Intf::Indirection::none));
    Cm::Sym::TypeSymbol* type = staticMemberVariableSymbol->GetType();
    if (type->IsClassTypeSymbol())
    {
        Cm::Sym::ClassTypeSymbol* classType = static_cast<Cm::Sym::ClassTypeSymbol*>(type);
        if (classType->Destructor())
        {
            Cm::Sym::FunctionSymbol* destructor = classType->Destructor();
            Ir::Intf::Function* destructorIrFun = IrFunctionRepository().CreateIrFunction(destructor);
            Ir::Intf::Type* destructorPtrType = IrFunctionRepository().GetFunPtrIrType(destructor);
            std::vector<Ir::Intf::Type*> dtorParamTypes1(1, voidPtr->Clone());
            Ir::Intf::Type* destructorFieldType = Cm::IrIntf::Pointer(Cm::IrIntf::CreateFunctionType(Cm::IrIntf::Void(), dtorParamTypes1), 1);
            emitter->Own(destructorFieldType);
            std::vector<Ir::Intf::Type*> dtorParamTypes2(1, voidPtr->Clone());
            Ir::Intf::Type* destructorFieldPtrType = Cm::IrIntf::Pointer(Cm::IrIntf::CreateFunctionType(Cm::IrIntf::Void(), dtorParamTypes2), 2);
            emitter->Own(destructorFieldPtrType);
            Ir::Intf::MemberVar* destructorField = Cm::IrIntf::CreateMemberVar("destructor", destructionNode, 2, destructorFieldPtrType);
            destructorField->SetDotMember();
            emitter->Own(destructorField);
            Ir::Intf::RegVar* destructorPtr = Cm::IrIntf::CreateTemporaryRegVar(destructorPtrType);
            emitter->Own(destructorPtr);
            Ir::Intf::Object* destructorIrObject = Cm::IrIntf::CreateGlobal(destructorIrFun->Name(), destructorPtrType);
            emitter->Own(destructorIrObject);
            emitter->Emit(Cm::IrIntf::Load(destructorPtrType, destructorPtr, destructorIrObject, Ir::Intf::Indirection::none, Ir::Intf::Indirection::addr));
            Ir::Intf::Object* destructorFunPtr = Cm::IrIntf::CreateTemporaryRegVar(destructorFieldType);
            emitter->Own(destructorFunPtr);
            emitter->Emit(Cm::IrIntf::Bitcast(destructorPtrType, destructorFunPtr, destructorPtr, destructorFieldType));
            emitter->Emit(Cm::IrIntf::Store(destructorFieldPtrType, destructorFunPtr, destructorField, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
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

void CFunctionEmitter::GenVirtualCall(Cm::Sym::FunctionSymbol* fun, Cm::Core::GenResult& memberFunctionResult)
{
    Cm::Core::Emitter* emitter = Emitter();
    Ir::Intf::Object* objectPtr = memberFunctionResult.Arg1();
    Cm::Sym::ClassTypeSymbol* classType = fun->Class();
    Ir::Intf::Type* voidPtr = Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 1);
    emitter->Own(voidPtr);
    Ir::Intf::Type* voidPtrPtr = Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 2);
    emitter->Own(voidPtrPtr);
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
    Ir::Intf::MemberVar* vptr = Cm::IrIntf::CreateMemberVar(Cm::IrIntf::GetVPtrVarName(), vptrContainerPtr, vptrIndex, voidPtrPtr);
    emitter->Own(vptr);
    Ir::Intf::RegVar* loadedVptr = Cm::IrIntf::CreateTemporaryRegVar(voidPtrPtr);
    emitter->Own(loadedVptr);
    Cm::IrIntf::Assign(*emitter, voidPtrPtr, vptr, loadedVptr);
    Ir::Intf::RegVar* functionVoidPtrPtr = Cm::IrIntf::CreateTemporaryRegVar(voidPtrPtr);
    emitter->Own(functionVoidPtrPtr);
    Ir::Intf::Object* functionIndex = Cm::IrIntf::CreateI16Constant(fun->VtblIndex());
    emitter->Own(functionIndex);
    emitter->Emit(Cm::IrIntf::Add(voidPtrPtr, functionVoidPtrPtr, loadedVptr, functionIndex));
    Ir::Intf::RegVar* loadedFunctionVoidPtr = Cm::IrIntf::CreateTemporaryRegVar(voidPtr);
    emitter->Own(loadedFunctionVoidPtr);
    Cm::IrIntf::Assign(*emitter, voidPtr, functionVoidPtrPtr, loadedFunctionVoidPtr);
    Ir::Intf::Type* functionPtrType = IrFunctionRepository().GetFunPtrIrType(fun);
    Ir::Intf::RegVar* loadedFunctionPtr = Cm::IrIntf::CreateTemporaryRegVar(functionPtrType);
    emitter->Own(loadedFunctionPtr);
    emitter->Emit(Cm::IrIntf::Bitcast(voidPtr, loadedFunctionPtr, loadedFunctionVoidPtr, functionPtrType));
    emitter->Emit(Cm::IrIntf::IndirectCall(memberFunctionResult.MainObject(), loadedFunctionPtr, memberFunctionResult.Args()));
}

Ir::Intf::LabelObject* CFunctionEmitter::CreateLandingPadLabel(int landingPadId)
{
    return Cm::IrIntf::CreateLabel("_P_" + std::to_string(landingPadId));
}

void CFunctionEmitter::MapIrFunToFun(Ir::Intf::Function* irFun, Cm::Sym::FunctionSymbol* fun)
{
    (*functionMap)[irFun] = fun;
}

} } // namespace Cm::Emit
