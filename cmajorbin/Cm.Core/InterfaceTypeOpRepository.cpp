/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/InterfaceTypeOpRepository.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/InterfaceTypeSymbol.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/TypeRepository.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>

namespace Cm { namespace Core {

InterfaceObjectDefaultCtor::InterfaceObjectDefaultCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType_) : BasicTypeOp(interfaceType_)
{
    SetGroupName("@constructor");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(Type(), Cm::Parsing::Span()));
    thisParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(thisParam);
    ComputeName();
    Cm::Sym::EntrySymbol* entry = new Cm::Sym::EntrySymbol(Cm::Parsing::Span());
    AddSymbol(entry);
}

void InterfaceObjectDefaultCtor::Generate(Emitter& emitter, GenResult& result)
{
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        GenerateLlvm(emitter, result);
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        GenerateC(emitter, result);
    }
}

void InterfaceObjectDefaultCtor::GenerateLlvm(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 1);
    emitter.Own(i8Ptr);
    Ir::Intf::Object* mainObject = result.MainObject();
    Ir::Intf::MemberVar* obj = Cm::IrIntf::CreateMemberVar("obj", mainObject, 0, i8Ptr);
    emitter.Own(obj);
    Ir::Intf::Object* nullPtr = i8Ptr->CreateDefaultValue();
    emitter.Own(nullPtr);
    Cm::IrIntf::Assign(emitter, i8Ptr, nullPtr, obj);
    Ir::Intf::MemberVar* itab = Cm::IrIntf::CreateMemberVar("itab", mainObject, 1, i8Ptr);
    emitter.Own(itab);
    Cm::IrIntf::Assign(emitter, i8Ptr, nullPtr, itab);
}

void InterfaceObjectDefaultCtor::GenerateC(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Type* voidPtr = Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 1);
    emitter.Own(voidPtr);
    Ir::Intf::Object* mainObject = result.MainObject();
    Ir::Intf::MemberVar* obj = Cm::IrIntf::CreateMemberVar("obj", mainObject, 0, voidPtr);
    emitter.Own(obj);
    if (!mainObject->GetType()->IsPointerType())
    {
        obj->SetDotMember();
    }
    Ir::Intf::Object* nullPtr = voidPtr->CreateDefaultValue();
    Cm::IrIntf::Assign(emitter, voidPtr, nullPtr, obj);
    Ir::Intf::MemberVar* itab = Cm::IrIntf::CreateMemberVar("itab", mainObject, 1, voidPtr);
    emitter.Own(itab);
    if (!mainObject->GetType()->IsPointerType())
    {
        itab->SetDotMember();
    }
    Cm::IrIntf::Assign(emitter, voidPtr, nullPtr, itab);
}

InterfaceObjectCopyCtor::InterfaceObjectCopyCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType_) : BasicTypeOp(interfaceType_)
{
    SetGroupName("@constructor");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(Type(), Cm::Parsing::Span()));
    thisParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* thatParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "that"));
    thatParam->SetType(typeRepository.MakeConstReferenceType(Type(), Cm::Parsing::Span()));
    thatParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(thatParam);
    ComputeName();
    Cm::Sym::EntrySymbol* entry = new Cm::Sym::EntrySymbol(Cm::Parsing::Span());
    AddSymbol(entry);
}

void InterfaceObjectCopyCtor::Generate(Emitter& emitter, GenResult& result)
{
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        GenerateLlvm(emitter, result);
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        GenerateC(emitter, result);
    }
}

void InterfaceObjectCopyCtor::GenerateLlvm(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 1);
    emitter.Own(i8Ptr);
    Ir::Intf::Object* mainObject = result.MainObject();
    Ir::Intf::MemberVar* obj = Cm::IrIntf::CreateMemberVar("obj", mainObject, 0, i8Ptr);
    emitter.Own(obj);
    Ir::Intf::Object* that = result.Arg1();
    Ir::Intf::MemberVar* thatObj = Cm::IrIntf::CreateMemberVar("obj", that, 0, i8Ptr);
    emitter.Own(thatObj);
    Cm::IrIntf::Assign(emitter, i8Ptr, thatObj, obj);
    Ir::Intf::MemberVar* itab = Cm::IrIntf::CreateMemberVar("itab", mainObject, 1, i8Ptr);
    emitter.Own(itab);
    Ir::Intf::MemberVar* thatItab = Cm::IrIntf::CreateMemberVar("itab", that, 1, i8Ptr);
    emitter.Own(itab);
    Cm::IrIntf::Assign(emitter, i8Ptr, thatItab, itab);
}

void InterfaceObjectCopyCtor::GenerateC(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Type* voidPtr = Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 1);
    emitter.Own(voidPtr);
    Ir::Intf::Object* mainObject = result.MainObject();
    Ir::Intf::MemberVar* obj = Cm::IrIntf::CreateMemberVar("obj", mainObject, 0, voidPtr);
    emitter.Own(obj);
    if (!mainObject->GetType()->IsPointerType())
    {
        obj->SetDotMember();
    }
    Ir::Intf::Object* that = result.Arg1();
    Ir::Intf::MemberVar* thatObj = Cm::IrIntf::CreateMemberVar("obj", that, 0, voidPtr);
    emitter.Own(thatObj);
    if (!that->GetType()->IsPointerType())
    {
        thatObj->SetDotMember();
    }
    Cm::IrIntf::Assign(emitter, voidPtr, thatObj, obj);
    Ir::Intf::MemberVar* itab = Cm::IrIntf::CreateMemberVar("itab", mainObject, 1, voidPtr);
    emitter.Own(itab);
    if (!mainObject->GetType()->IsPointerType())
    {
        itab->SetDotMember();
    }
    Ir::Intf::MemberVar* thatItab = Cm::IrIntf::CreateMemberVar("itab", that, 1, voidPtr);
    emitter.Own(thatItab);
    if (!that->GetType()->IsPointerType())
    {
        thatItab->SetDotMember();
    }
    Cm::IrIntf::Assign(emitter, voidPtr, thatItab, itab);
}

InterfaceObjectCopyAssignment::InterfaceObjectCopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType_) : BasicTypeOp(interfaceType_)
{
    SetGroupName("operator=");
    Cm::Sym::TypeSymbol* voidType = typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId));
    SetReturnType(voidType);
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(Type(), Cm::Parsing::Span()));
    thisParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* thatParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "that"));
    thatParam->SetType(typeRepository.MakeConstReferenceType(Type(), Cm::Parsing::Span()));
    thatParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(thatParam);
    ComputeName();
    Cm::Sym::EntrySymbol* entry = new Cm::Sym::EntrySymbol(Cm::Parsing::Span());
    AddSymbol(entry);
}

void InterfaceObjectCopyAssignment::Generate(Emitter& emitter, GenResult& result)
{
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        GenerateLlvm(emitter, result);
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        GenerateC(emitter, result);
    }
}

void InterfaceObjectCopyAssignment::GenerateLlvm(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 1);
    emitter.Own(i8Ptr);
    Ir::Intf::Object* mainObject = result.MainObject();
    Ir::Intf::MemberVar* obj = Cm::IrIntf::CreateMemberVar("obj", mainObject, 0, i8Ptr);
    emitter.Own(obj);
    Ir::Intf::Object* that = result.Arg1();
    Ir::Intf::MemberVar* thatObj = Cm::IrIntf::CreateMemberVar("obj", that, 0, i8Ptr);
    emitter.Own(thatObj);
    Cm::IrIntf::Assign(emitter, i8Ptr, thatObj, obj);
    Ir::Intf::MemberVar* itab = Cm::IrIntf::CreateMemberVar("itab", mainObject, 1, i8Ptr);
    emitter.Own(itab);
    Ir::Intf::MemberVar* thatItab = Cm::IrIntf::CreateMemberVar("itab", that, 1, i8Ptr);
    emitter.Own(itab);
    Cm::IrIntf::Assign(emitter, i8Ptr, thatItab, itab);
}

void InterfaceObjectCopyAssignment::GenerateC(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Type* voidPtr = Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 1);
    emitter.Own(voidPtr);
    Ir::Intf::Object* mainObject = result.MainObject();
    Ir::Intf::MemberVar* obj = Cm::IrIntf::CreateMemberVar("obj", mainObject, 0, voidPtr);
    emitter.Own(obj);
    if (!mainObject->GetType()->IsPointerType())
    {
        obj->SetDotMember();
    }
    Ir::Intf::Object* that = result.Arg1();
    Ir::Intf::MemberVar* thatObj = Cm::IrIntf::CreateMemberVar("obj", that, 0, voidPtr);
    emitter.Own(thatObj);
    if (!that->GetType()->IsPointerType())
    {
        thatObj->SetDotMember();
    }
    Cm::IrIntf::Assign(emitter, voidPtr, thatObj, obj);
    Ir::Intf::MemberVar* itab = Cm::IrIntf::CreateMemberVar("itab", mainObject, 1, voidPtr);
    emitter.Own(itab);
    if (!mainObject->GetType()->IsPointerType())
    {
        itab->SetDotMember();
    }
    Ir::Intf::MemberVar* thatItab = Cm::IrIntf::CreateMemberVar("itab", that, 1, voidPtr);
    emitter.Own(thatItab);
    if (!that->GetType()->IsPointerType())
    {
        thatItab->SetDotMember();
    }
    Cm::IrIntf::Assign(emitter, voidPtr, thatItab, itab);
}

InterfaceObjectOpEqual::InterfaceObjectOpEqual(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType_) : BasicTypeOp(interfaceType_)
{
    SetGroupName("operator==");
    Cm::Sym::TypeSymbol* boolType = typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
    SetReturnType(boolType);
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "left"));
    leftParam->SetType(Type());
    leftParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "right"));
    rightParam->SetType(Type());
    rightParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(rightParam);
    ComputeName();
    Cm::Sym::EntrySymbol* entry = new Cm::Sym::EntrySymbol(Cm::Parsing::Span());
    AddSymbol(entry);
}

void InterfaceObjectOpEqual::Generate(Emitter& emitter, GenResult& result)
{
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        GenerateLlvm(emitter, result);
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        GenerateC(emitter, result);
    }
}

void InterfaceObjectOpEqual::GenerateLlvm(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Type* boolPtr = Cm::IrIntf::Pointer(Cm::IrIntf::I1(), 1);
    emitter.Own(boolPtr);
    Ir::Intf::Object* resultStackVar = Cm::IrIntf::CreateStackVar("intf$compare$result", boolPtr);
    if (!emitter.TempBoolVarAllocted())
    {
        emitter.SetTempBoolVarAllocted();
        emitter.Emit(Cm::IrIntf::Alloca(Cm::IrIntf::I1(), resultStackVar));
    }
    Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 1);
    emitter.Own(i8Ptr);
    Ir::Intf::Object* leftObject = result.Arg1();
    Ir::Intf::MemberVar* leftObj = Cm::IrIntf::CreateMemberVar("obj", leftObject, 0, i8Ptr);
    emitter.Own(leftObj);
    Ir::Intf::Object* temp0 = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
    emitter.Own(temp0);
    Cm::IrIntf::Assign(emitter, i8Ptr, leftObj, temp0);
    Ir::Intf::Object* rightObject = result.Arg2();
    Ir::Intf::MemberVar* rightObj = Cm::IrIntf::CreateMemberVar("obj", rightObject, 0, i8Ptr);
    emitter.Own(rightObj);
    Ir::Intf::Object* temp1 = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
    emitter.Own(temp1);
    Cm::IrIntf::Assign(emitter, i8Ptr, rightObj, temp1);
    Ir::Intf::Object* result0 = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI1());
    emitter.Own(result0);
    emitter.Emit(Cm::IrIntf::ICmp(i8Ptr, result0, Ir::Intf::IConditionCode::eq, temp0, temp1));
    Ir::Intf::LabelObject* nextLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter.Own(nextLabel);
    Ir::Intf::LabelObject* falseLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter.Own(falseLabel);
    emitter.Emit(Cm::IrIntf::Br(result0, nextLabel, falseLabel));
    emitter.AddNextInstructionLabel(nextLabel);
    Ir::Intf::MemberVar* leftItab = Cm::IrIntf::CreateMemberVar("itab", leftObject, 1, i8Ptr);
    emitter.Own(leftItab);
    Ir::Intf::Object* temp2 = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
    emitter.Own(temp2);
    Cm::IrIntf::Assign(emitter, i8Ptr, leftItab, temp2);
    Ir::Intf::MemberVar* rightItab = Cm::IrIntf::CreateMemberVar("itab", rightObject, 1, i8Ptr);
    emitter.Own(rightItab);
    Ir::Intf::Object* temp3 = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
    emitter.Own(temp3);
    Cm::IrIntf::Assign(emitter, i8Ptr, rightItab, temp3);
    Ir::Intf::Object* result1 = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI1());
    emitter.Own(result1);
    emitter.Emit(Cm::IrIntf::ICmp(i8Ptr, result1, Ir::Intf::IConditionCode::eq, temp2, temp3));
    Cm::IrIntf::Assign(emitter, Ir::Intf::GetFactory()->GetI1(), result1, resultStackVar);
    Ir::Intf::LabelObject* commonLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter.Emit(Cm::IrIntf::Br(commonLabel));
    emitter.AddNextInstructionLabel(falseLabel);
    Ir::Intf::Object* falseObject = Cm::IrIntf::CreateBooleanConstant(false);
    emitter.Own(falseObject);
    Cm::IrIntf::Assign(emitter, Ir::Intf::GetFactory()->GetI1(), falseObject, resultStackVar);
    emitter.AddNextInstructionLabel(commonLabel);
    Cm::IrIntf::Assign(emitter, Ir::Intf::GetFactory()->GetI1(), resultStackVar, result.MainObject());
    if (result.GenJumpingBoolCode())
    {
        Ir::Intf::LabelObject* trueLabel = Cm::IrIntf::CreateLabel();
        emitter.Own(trueLabel);
        Ir::Intf::LabelObject* falseLabel = Cm::IrIntf::CreateLabel();
        emitter.Own(falseLabel);
        emitter.Emit(Cm::IrIntf::Br(result.MainObject(), trueLabel, falseLabel));
        result.AddTrueTarget(trueLabel);
        result.AddFalseTarget(falseLabel);
    }
}

void InterfaceObjectOpEqual::GenerateC(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Object* resultStackVar = Cm::IrIntf::CreateStackVar("intf_X_compare_X_result", Cm::IrIntf::I1());
    if (!emitter.TempBoolVarAllocted())
    {
        emitter.SetTempBoolVarAllocted();
        emitter.Emit(Cm::IrIntf::Alloca(Cm::IrIntf::I1(), resultStackVar));
    }
    Ir::Intf::Type* voidPtr = Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 1);
    emitter.Own(voidPtr);
    Ir::Intf::Object* leftObject = result.Arg1();
    Ir::Intf::MemberVar* leftObj = Cm::IrIntf::CreateMemberVar("obj", leftObject, 0, voidPtr);
    if (!leftObject->GetType()->IsPointerType())
    {
        leftObj->SetDotMember();
    }
    emitter.Own(leftObj);
    Ir::Intf::Object* temp0 = Cm::IrIntf::CreateTemporaryRegVar(voidPtr);
    emitter.Own(temp0);
    Cm::IrIntf::Assign(emitter, voidPtr, leftObj, temp0);
    Ir::Intf::Object* rightObject = result.Arg2();
    Ir::Intf::MemberVar* rightObj = Cm::IrIntf::CreateMemberVar("obj", rightObject, 0, voidPtr);
    if (!rightObject->GetType()->IsPointerType())
    {
        rightObj->SetDotMember();
    }
    emitter.Own(rightObj);
    Ir::Intf::Object* temp1 = Cm::IrIntf::CreateTemporaryRegVar(voidPtr);
    emitter.Own(temp1);
    Cm::IrIntf::Assign(emitter, voidPtr, rightObj, temp1);
    Ir::Intf::Object* result0 = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI1());
    emitter.Own(result0);
    emitter.Emit(Cm::IrIntf::ICmp(voidPtr, result0, Ir::Intf::IConditionCode::eq, temp0, temp1));
    Ir::Intf::LabelObject* nextLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter.Own(nextLabel);
    Ir::Intf::LabelObject* falseLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter.Own(falseLabel);
    emitter.Emit(Cm::IrIntf::Br(result0, nextLabel, falseLabel));
    emitter.AddNextInstructionLabel(nextLabel);
    Ir::Intf::MemberVar* leftItab = Cm::IrIntf::CreateMemberVar("itab", leftObject, 1, voidPtr);
    if (!leftObject->GetType()->IsPointerType())
    {
        leftItab->SetDotMember();
    }
    emitter.Own(leftItab);
    Ir::Intf::Object* temp2 = Cm::IrIntf::CreateTemporaryRegVar(voidPtr);
    emitter.Own(temp2);
    Cm::IrIntf::Assign(emitter, voidPtr, leftItab, temp2);
    Ir::Intf::MemberVar* rightItab = Cm::IrIntf::CreateMemberVar("itab", rightObject, 1, voidPtr);
    if (!rightObject->GetType()->IsPointerType())
    {
        rightItab->SetDotMember();
    }
    emitter.Own(rightItab);
    Ir::Intf::Object* temp3 = Cm::IrIntf::CreateTemporaryRegVar(voidPtr);
    emitter.Own(temp3);
    Cm::IrIntf::Assign(emitter, voidPtr, rightItab, temp3);
    Ir::Intf::Object* result1 = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI1());
    emitter.Own(result1);
    emitter.Emit(Cm::IrIntf::ICmp(voidPtr, result1, Ir::Intf::IConditionCode::eq, temp2, temp3));
    Cm::IrIntf::Assign(emitter, Ir::Intf::GetFactory()->GetI1(), result1, resultStackVar);
    Ir::Intf::LabelObject* commonLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter.Emit(Cm::IrIntf::Br(commonLabel));
    emitter.AddNextInstructionLabel(falseLabel);
    Ir::Intf::Object* falseObject = Cm::IrIntf::CreateBooleanConstant(false);
    emitter.Own(falseObject);
    Cm::IrIntf::Assign(emitter, Ir::Intf::GetFactory()->GetI1(), falseObject, resultStackVar);
    emitter.AddNextInstructionLabel(commonLabel);
    Cm::IrIntf::Assign(emitter, Ir::Intf::GetFactory()->GetI1(), resultStackVar, result.MainObject());
    if (result.GenJumpingBoolCode())
    {
        Ir::Intf::LabelObject* trueLabel = Cm::IrIntf::CreateLabel();
        emitter.Own(trueLabel);
        Ir::Intf::LabelObject* falseLabel = Cm::IrIntf::CreateLabel();
        emitter.Own(falseLabel);
        emitter.Emit(Cm::IrIntf::Br(result.MainObject(), trueLabel, falseLabel));
        result.AddTrueTarget(trueLabel);
        result.AddFalseTarget(falseLabel);
    }
}

InterfaceObjectFromClassPtrCtor::InterfaceObjectFromClassPtrCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType_, Cm::Sym::TypeSymbol* classPtrType_) : 
    BasicTypeOp(interfaceType_), classPtrType(classPtrType_)
{
    SetGroupName("@constructor");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(Type(), Cm::Parsing::Span()));
    thisParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* thatParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "that"));
    thatParam->SetType(classPtrType);
    thatParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(thatParam);
    ComputeName();
    Cm::Sym::EntrySymbol* entry = new Cm::Sym::EntrySymbol(Cm::Parsing::Span());
    AddSymbol(entry);
}

void InterfaceObjectFromClassPtrCtor::Generate(Emitter& emitter, GenResult& result)
{
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        GenerateLlvm(emitter, result);
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        GenerateC(emitter, result);
    }
}

void InterfaceObjectFromClassPtrCtor::GenerateLlvm(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 1);
    emitter.Own(i8Ptr);
    Ir::Intf::Type* i8PtrPtr = Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 2);
    emitter.Own(i8PtrPtr);
    Ir::Intf::Object* mainObject = result.MainObject();
    Ir::Intf::MemberVar* obj = Cm::IrIntf::CreateMemberVar("obj", mainObject, 0, i8Ptr);
    emitter.Own(obj);
    Ir::Intf::Object* objectPtr = result.Arg1();
    Ir::Intf::Type* interfacePtrType = Cm::IrIntf::Pointer(Type()->GetIrType(), 1);
    emitter.Own(interfacePtrType);
    Ir::Intf::RegVar* ptrReg = Cm::IrIntf::CreateTemporaryRegVar(i8PtrPtr);
    emitter.Own(ptrReg);
    Ir::Intf::Object* zero = Cm::IrIntf::CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* index = Cm::IrIntf::CreateI32Constant(obj->Index());
    emitter.Own(index);
    emitter.Emit(Cm::IrIntf::GetElementPtr(interfacePtrType, ptrReg, obj->Ptr(), zero, index));
    Ir::Intf::Object* objAsI8Ptr = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
    emitter.Emit(Cm::IrIntf::Bitcast(objectPtr->GetType(), objAsI8Ptr, objectPtr, i8Ptr));
    emitter.Emit(Cm::IrIntf::Store(i8Ptr, objAsI8Ptr, ptrReg));
    Ir::Intf::Object* vptrContainerPtr = objectPtr;
    Cm::Sym::TypeSymbol* classPtrBaseType = ClassPtrType()->GetBaseType();
    Cm::Sym::ClassTypeSymbol* classType = nullptr;
    if (classPtrBaseType->IsClassTypeSymbol())
    {
        classType = static_cast<Cm::Sym::ClassTypeSymbol*>(classPtrBaseType);
    }
    else
    {
        throw std::runtime_error("class type expected");
    }
    int16_t vptrIndex = classType->VPtrIndex();
    if (vptrIndex == -1)
    {
        Cm::Sym::ClassTypeSymbol* vptrContainingType = classType->VPtrContainerClass();
        vptrIndex = vptrContainingType->VPtrIndex();
        Ir::Intf::Type* vptrContainingPtrIrType = Cm::IrIntf::Pointer(vptrContainingType->GetIrType(), 1);
        emitter.Own(vptrContainingPtrIrType);
        Ir::Intf::RegVar* containerPtr = Cm::IrIntf::CreateTemporaryRegVar(vptrContainingPtrIrType);
        emitter.Own(containerPtr);
        Ir::Intf::Type* classTypePtrIrType = ClassPtrType()->GetIrType();
        emitter.Own(classTypePtrIrType);
        emitter.Emit(Cm::IrIntf::Bitcast(classTypePtrIrType, containerPtr, objectPtr, vptrContainingPtrIrType));
        vptrContainerPtr = containerPtr;
    }
    Ir::Intf::MemberVar* vptr = Cm::IrIntf::CreateMemberVar(Cm::IrIntf::GetVPtrVarName(), vptrContainerPtr, vptrIndex, i8PtrPtr);
    emitter.Own(vptr);
    Ir::Intf::RegVar* loadedVptr = Cm::IrIntf::CreateTemporaryRegVar(i8PtrPtr);
    emitter.Own(loadedVptr);
    Cm::IrIntf::Assign(emitter, i8PtrPtr, vptr, loadedVptr);
    Ir::Intf::RegVar* i8rttiPtr = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
    emitter.Own(i8rttiPtr);
    Cm::IrIntf::Assign(emitter, i8Ptr, loadedVptr, i8rttiPtr);
    std::vector<Ir::Intf::Type*> rttiElementTypes;
    rttiElementTypes.push_back(i8Ptr->Clone());
    rttiElementTypes.push_back(Cm::IrIntf::UI64());
    Ir::Intf::Type* irecPtrType = Cm::IrIntf::Pointer(Cm::IrIntf::CreateTypeName("irec", false), 1);
    emitter.Own(irecPtrType);
    Ir::Intf::Type* irecPtrPtrType = Cm::IrIntf::Pointer(Cm::IrIntf::CreateTypeName("irec", false), 2);
    emitter.Own(irecPtrPtrType);
    rttiElementTypes.push_back(irecPtrType->Clone());
    std::vector<std::string> rttiElementNames;
    rttiElementNames.push_back("class_name");
    rttiElementNames.push_back("class_id");
    rttiElementNames.push_back("irectab");
    Ir::Intf::Type* rttiPtrIrType(Cm::IrIntf::Pointer(Cm::IrIntf::Structure("rtti_", rttiElementTypes, rttiElementNames), 1));
    emitter.Own(rttiPtrIrType);
    Ir::Intf::RegVar* rttiPtr = Cm::IrIntf::CreateTemporaryRegVar(rttiPtrIrType);
    emitter.Own(rttiPtr);
    emitter.Emit(Cm::IrIntf::Bitcast(i8Ptr, rttiPtr, i8rttiPtr, rttiPtrIrType));
    Ir::Intf::Object* two = Cm::IrIntf::CreateI32Constant(2);
    emitter.Own(two);
    Ir::Intf::Object* irecTabPtrPtr = Cm::IrIntf::CreateTemporaryRegVar(irecPtrPtrType);
    emitter.Own(irecTabPtrPtr);
    emitter.Emit(Cm::IrIntf::GetElementPtr(rttiPtrIrType, irecTabPtrPtr, rttiPtr, zero, two));
    Ir::Intf::RegVar* irecTabPtr = Cm::IrIntf::CreateTemporaryRegVar(irecPtrType);
    emitter.Own(irecTabPtr);
    Cm::IrIntf::Assign(emitter, irecPtrType, irecTabPtrPtr, irecTabPtr);
    Ir::Intf::Object* irecPtr = Cm::IrIntf::CreateStackVar(Cm::IrIntf::GetIrecPtrName(), irecPtrPtrType);
    emitter.Own(irecPtr);
    if (!emitter.IrecPtrAllocated())
    {
        emitter.SetIrecPtrAllocated();
        emitter.Emit(Cm::IrIntf::Alloca(irecPtrType, irecPtr));
    }
    emitter.Emit(Cm::IrIntf::Store(irecPtrType, irecTabPtr, irecPtr));
    Ir::Intf::LabelObject* loopLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter.Own(loopLabel);
    emitter.AddNextInstructionLabel(loopLabel);
    Ir::Intf::Object* loopVar =  Cm::IrIntf::CreateTemporaryRegVar(irecPtrType);
    Cm::IrIntf::Assign(emitter, irecPtrType, irecPtr, loopVar);
    Ir::Intf::MemberVar* iid = Cm::IrIntf::CreateMemberVar("iid", loopVar, 0, Ir::Intf::GetFactory()->GetUI64());
    emitter.Own(iid);
    Ir::Intf::Object* iidReg = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetUI64());
    emitter.Own(iidReg);
    Cm::IrIntf::Assign(emitter, Ir::Intf::GetFactory()->GetUI64(), iid, iidReg);
    Ir::Intf::Object* sentinelIid = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetUI64());
    emitter.Own(sentinelIid);
    Ir::Intf::Object* minusOne = Ir::Intf::GetFactory()->GetUI64()->CreateMinusOne();
    emitter.Own(minusOne);
    Cm::IrIntf::Assign(emitter, Ir::Intf::GetFactory()->GetUI64(), minusOne, sentinelIid);
    Ir::Intf::Object* isMinusOne = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI1());
    emitter.Own(isMinusOne);
    emitter.Emit(Cm::IrIntf::ICmp(Ir::Intf::GetFactory()->GetUI64(), isMinusOne, Ir::Intf::IConditionCode::eq, iidReg, sentinelIid));
    Ir::Intf::LabelObject* outLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter.Own(outLabel);
    Ir::Intf::LabelObject* nextLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter.Own(nextLabel);
    emitter.Emit(Cm::IrIntf::Br(isMinusOne, outLabel, nextLabel));
    emitter.AddNextInstructionLabel(nextLabel);
    Cm::Sym::InterfaceTypeSymbol* interfaceType = static_cast<Cm::Sym::InterfaceTypeSymbol*>(Type());
    Ir::Intf::Object* intfIid = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetUI64());
    emitter.Own(intfIid);
    Ir::Intf::Object* intfIidConstant = Cm::IrIntf::CreateUI64Constant(interfaceType->Iid());
    emitter.Own(intfIidConstant);
    Cm::IrIntf::Assign(emitter, Ir::Intf::GetFactory()->GetUI64(), intfIidConstant, intfIid);
    Ir::Intf::Object* iidMatches = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI1());
    emitter.Own(iidMatches);
    emitter.Emit(Cm::IrIntf::ICmp(Ir::Intf::GetFactory()->GetUI64(), iidMatches, Ir::Intf::IConditionCode::eq, iidReg, intfIid));
    Ir::Intf::LabelObject* matchLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter.Own(matchLabel);
    Ir::Intf::LabelObject* next2Label = Cm::IrIntf::CreateNextLocalLabel();
    emitter.Own(next2Label);
    emitter.Emit(Cm::IrIntf::Br(iidMatches, matchLabel, next2Label));
    emitter.AddNextInstructionLabel(next2Label);
    Ir::Intf::Object* one = Cm::IrIntf::CreateI32Constant(1);
    emitter.Own(one);
    Ir::Intf::Object* nextIrecValue = Cm::IrIntf::CreateTemporaryRegVar(irecPtrType);
    emitter.Own(nextIrecValue);
    emitter.Emit(Cm::IrIntf::GetElementPtr(irecPtrType, nextIrecValue, loopVar, one));
    emitter.Emit(Cm::IrIntf::Store(irecPtrType, nextIrecValue, irecPtr));
    emitter.Emit(Cm::IrIntf::Br(loopLabel));
    emitter.AddNextInstructionLabel(outLabel);
    Ir::Intf::Function* interfaceLookupFailed = emitter.InterfaceLookupFailed();
    std::vector<Ir::Intf::Object*> interfaceLookupFailedArgs;
    emitter.Emit(Cm::IrIntf::Call(nullptr, interfaceLookupFailed, interfaceLookupFailedArgs));
    emitter.AddNextInstructionLabel(matchLabel);
    Ir::Intf::MemberVar* itab = Cm::IrIntf::CreateMemberVar("itab", loopVar, 1, i8Ptr);
    emitter.Own(itab);
    Ir::Intf::Object* itabPtr = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
    emitter.Own(itabPtr);
    Cm::IrIntf::Assign(emitter, i8Ptr, itab, itabPtr);
    Ir::Intf::MemberVar* mainItab = Cm::IrIntf::CreateMemberVar("itab", mainObject, 1, i8Ptr);
    emitter.Own(mainItab);
    Cm::IrIntf::Assign(emitter, i8Ptr, itabPtr, mainItab);
}

void InterfaceObjectFromClassPtrCtor::GenerateC(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Type* voidPtr = Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 1);
    emitter.Own(voidPtr);
    Ir::Intf::Type* voidPtrPtr = Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 2);
    emitter.Own(voidPtrPtr);
    Ir::Intf::Object* mainObject = result.MainObject();
    Ir::Intf::MemberVar* obj = Cm::IrIntf::CreateMemberVar("obj", mainObject, 0, voidPtr);
    emitter.Own(obj);
    if (!mainObject->GetType()->IsPointerType())
    {
        obj->SetDotMember();
    }
    Ir::Intf::Object* objectPtr = result.Arg1();
    Ir::Intf::Type* interfacePtrType = Cm::IrIntf::Pointer(Type()->GetIrType(), 1);
    emitter.Own(interfacePtrType);
    Ir::Intf::Object* zero = Cm::IrIntf::CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* objAsVoidPtr = Cm::IrIntf::CreateTemporaryRegVar(voidPtr);
    emitter.Own(objAsVoidPtr);
    emitter.Emit(Cm::IrIntf::Bitcast(objectPtr->GetType(), objAsVoidPtr, objectPtr, voidPtr));
    Ir::Intf::RegVar* ptrReg = Cm::IrIntf::CreateTemporaryRegVar(voidPtrPtr);
    emitter.Own(ptrReg);
    emitter.Emit(Cm::IrIntf::Load(voidPtrPtr, ptrReg, obj, Ir::Intf::Indirection::none, Ir::Intf::Indirection::addr));
    emitter.Emit(Cm::IrIntf::Store(voidPtr, objAsVoidPtr, ptrReg, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
    Ir::Intf::Object* vptrContainerPtr = objectPtr;
    Cm::Sym::TypeSymbol* classPtrBaseType = ClassPtrType()->GetBaseType();
    Cm::Sym::ClassTypeSymbol* classType = nullptr;
    if (classPtrBaseType->IsClassTypeSymbol())
    {
        classType = static_cast<Cm::Sym::ClassTypeSymbol*>(classPtrBaseType);
    }
    else
    {
        throw std::runtime_error("class type expected");
    }
    int16_t vptrIndex = classType->VPtrIndex();
    if (vptrIndex == -1)
    {
        Cm::Sym::ClassTypeSymbol* vptrContainingType = classType->VPtrContainerClass();
        vptrIndex = vptrContainingType->VPtrIndex();
        Ir::Intf::Type* vptrContainingPtrIrType = Cm::IrIntf::Pointer(vptrContainingType->GetIrType(), 1);
        emitter.Own(vptrContainingPtrIrType);
        Ir::Intf::RegVar* containerPtr = Cm::IrIntf::CreateTemporaryRegVar(vptrContainingPtrIrType);
        emitter.Own(containerPtr);
        Ir::Intf::Type* classTypePtrIrType = ClassPtrType()->GetIrType();
        emitter.Own(classTypePtrIrType);
        emitter.Emit(Cm::IrIntf::Bitcast(classTypePtrIrType, containerPtr, objectPtr, vptrContainingPtrIrType));
        vptrContainerPtr = containerPtr;
    }
    Ir::Intf::MemberVar* vptr = Cm::IrIntf::CreateMemberVar(Cm::IrIntf::GetVPtrVarName(), vptrContainerPtr, vptrIndex, voidPtrPtr);
    emitter.Own(vptr);
    Ir::Intf::RegVar* loadedVptr = Cm::IrIntf::CreateTemporaryRegVar(voidPtrPtr);
    emitter.Own(loadedVptr);
    Cm::IrIntf::Assign(emitter, voidPtrPtr, vptr, loadedVptr);
    Ir::Intf::RegVar* voidrttiPtr = Cm::IrIntf::CreateTemporaryRegVar(voidPtr);
    emitter.Own(voidrttiPtr);
    Cm::IrIntf::Assign(emitter, voidPtr, loadedVptr, voidrttiPtr);
    Ir::Intf::Type* rttiPtrIrType(Cm::IrIntf::Pointer(Cm::IrIntf::CreateTypeName("rtti", false), 1));
    emitter.Own(rttiPtrIrType);
    Ir::Intf::RegVar* rttiPtr = Cm::IrIntf::CreateTemporaryRegVar(rttiPtrIrType);
    emitter.Own(rttiPtr);
    emitter.Emit(Cm::IrIntf::Bitcast(voidPtr, rttiPtr, voidrttiPtr, rttiPtrIrType));
    Ir::Intf::Type* irecTabPtrType = Cm::IrIntf::Pointer(Cm::IrIntf::CreateTypeName("irec", false), 1);
    emitter.Own(irecTabPtrType);
    Ir::Intf::MemberVar* irecTabMemberVar = Cm::IrIntf::CreateMemberVar("irectab", rttiPtr, 2, irecTabPtrType);
    emitter.Own(irecTabMemberVar);
    Ir::Intf::Type* irecPtrType = Cm::IrIntf::Pointer(Cm::IrIntf::CreateTypeName("irec", false), 1);
    emitter.Own(irecPtrType);
    Ir::Intf::RegVar* irecTabPtr = Cm::IrIntf::CreateTemporaryRegVar(irecPtrType);
    emitter.Own(irecTabPtr);
    Cm::IrIntf::Assign(emitter, irecPtrType, irecTabMemberVar, irecTabPtr);
    Ir::Intf::Object* irecPtr = Cm::IrIntf::CreateStackVar(Cm::IrIntf::GetIrecPtrName(), irecPtrType);
    emitter.Own(irecPtr);
    if (!emitter.IrecPtrAllocated())
    {
        emitter.SetIrecPtrAllocated();
        emitter.Emit(Cm::IrIntf::Alloca(irecPtrType, irecPtr));
    }
    emitter.Emit(Cm::IrIntf::Store(irecPtrType, irecTabPtr, irecPtr));
    Ir::Intf::LabelObject* loopLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter.Own(loopLabel);
    emitter.AddNextInstructionLabel(loopLabel);
    Ir::Intf::Object* loopVar = Cm::IrIntf::CreateTemporaryRegVar(irecPtrType);
    Cm::IrIntf::Assign(emitter, irecPtrType, irecPtr, loopVar);
    Ir::Intf::MemberVar* iid = Cm::IrIntf::CreateMemberVar("iid", loopVar, 0, Ir::Intf::GetFactory()->GetUI64());
    emitter.Own(iid);
    Ir::Intf::Object* iidReg = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetUI64());
    emitter.Own(iidReg);
    Cm::IrIntf::Assign(emitter, Ir::Intf::GetFactory()->GetUI64(), iid, iidReg);
    Ir::Intf::Object* sentinelIid = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetUI64());
    emitter.Own(sentinelIid);
    Ir::Intf::Object* minusOne = Ir::Intf::GetFactory()->GetUI64()->CreateMinusOne();
    emitter.Own(minusOne);
    Cm::IrIntf::Assign(emitter, Ir::Intf::GetFactory()->GetUI64(), minusOne, sentinelIid);
    Ir::Intf::Object* isMinusOne = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI1());
    emitter.Own(isMinusOne);
    emitter.Emit(Cm::IrIntf::ICmp(Ir::Intf::GetFactory()->GetUI64(), isMinusOne, Ir::Intf::IConditionCode::eq, iidReg, sentinelIid));
    Ir::Intf::LabelObject* outLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter.Own(outLabel);
    Ir::Intf::LabelObject* nextLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter.Own(nextLabel);
    emitter.Emit(Cm::IrIntf::Br(isMinusOne, outLabel, nextLabel));
    emitter.AddNextInstructionLabel(nextLabel);
    Cm::Sym::InterfaceTypeSymbol* interfaceType = static_cast<Cm::Sym::InterfaceTypeSymbol*>(Type());
    Ir::Intf::Object* intfIid = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetUI64());
    emitter.Own(intfIid);
    Ir::Intf::Object* intfIidConstant = Cm::IrIntf::CreateUI64Constant(interfaceType->Iid());
    emitter.Own(intfIidConstant);
    Cm::IrIntf::Assign(emitter, Ir::Intf::GetFactory()->GetUI64(), intfIidConstant, intfIid);
    Ir::Intf::Object* iidMatches = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI1());
    emitter.Own(iidMatches);
    emitter.Emit(Cm::IrIntf::ICmp(Ir::Intf::GetFactory()->GetUI64(), iidMatches, Ir::Intf::IConditionCode::eq, iidReg, intfIid));
    Ir::Intf::LabelObject* matchLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter.Own(matchLabel);
    Ir::Intf::LabelObject* next2Label = Cm::IrIntf::CreateNextLocalLabel();
    emitter.Own(next2Label);
    emitter.Emit(Cm::IrIntf::Br(iidMatches, matchLabel, next2Label));
    emitter.AddNextInstructionLabel(next2Label);
    Ir::Intf::Object* one = Cm::IrIntf::CreateI32Constant(1);
    emitter.Own(one);
    emitter.Emit(Cm::IrIntf::Add(irecPtrType, irecPtr, irecPtr, one));
    emitter.Emit(Cm::IrIntf::Br(loopLabel));
    emitter.AddNextInstructionLabel(outLabel);
    Ir::Intf::Function* interfaceLookupFailed = emitter.InterfaceLookupFailed();
    std::vector<Ir::Intf::Object*> interfaceLookupFailedArgs;
    emitter.Emit(Cm::IrIntf::Call(nullptr, interfaceLookupFailed, interfaceLookupFailedArgs));
    emitter.AddNextInstructionLabel(matchLabel);
    Ir::Intf::MemberVar* itab = Cm::IrIntf::CreateMemberVar("itab", loopVar, 1, voidPtr);
    emitter.Own(itab);
    Ir::Intf::Object* itabPtr = Cm::IrIntf::CreateTemporaryRegVar(voidPtr);
    emitter.Own(itabPtr);
    Cm::IrIntf::Assign(emitter, voidPtr, itab, itabPtr);
    Ir::Intf::MemberVar* mainItab = Cm::IrIntf::CreateMemberVar("itab", mainObject, 1, voidPtr);
    emitter.Own(mainItab);
    if (!mainObject->GetType()->IsPointerType())
    {
        mainItab->SetDotMember();
    }
    Cm::IrIntf::Assign(emitter, voidPtr, itabPtr, mainItab);
}

Cm::Sym::FunctionSymbol* InterfaceTypeOpCache::GetDefaultCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType)
{
    if (!defaultCtor)
    {
        defaultCtor.reset(new InterfaceObjectDefaultCtor(typeRepository, interfaceType));
    }
    return defaultCtor.get();
}

Cm::Sym::FunctionSymbol* InterfaceTypeOpCache::GetCopyCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType)
{
    if (!copyCtor)
    {
        copyCtor.reset(new InterfaceObjectCopyCtor(typeRepository, interfaceType));
    }
    return copyCtor.get();
}

Cm::Sym::FunctionSymbol* InterfaceTypeOpCache::GetCopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType)
{
    if (!copyAssignment)
    {
        copyAssignment.reset(new InterfaceObjectCopyAssignment(typeRepository, interfaceType));
    }
    return copyAssignment.get();
}

Cm::Sym::FunctionSymbol* InterfaceTypeOpCache::GetInterfaceObjectFromClassPtrCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType, Cm::Sym::TypeSymbol* classPtrType)
{
    if (!interfaceObjectFromClassPtrCtor)
    {
        interfaceObjectFromClassPtrCtor.reset(new InterfaceObjectFromClassPtrCtor(typeRepository, interfaceType, classPtrType));
    }
    return interfaceObjectFromClassPtrCtor.get();
}

Cm::Sym::FunctionSymbol* InterfaceTypeOpCache::GetOpEqual(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType)
{
    if (!opEqual)
    {
        opEqual.reset(new InterfaceObjectOpEqual(typeRepository, interfaceType));
    }
    return opEqual.get();
}

InterfaceTypeOpGroup::~InterfaceTypeOpGroup()
{
}

void InterfaceConstructorOpGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, const Cm::Parsing::Span& span, Cm::Sym::TypeRepository& typeRepository,
    InterfaceTypeOpCacheMap& interfaceTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity == 1)
    {
        Cm::Sym::TypeSymbol* firstType = arguments[0].Type();
        if (firstType->IsPointerToInterfaceTypeSymbol())
        {
            std::pair<Cm::Sym::TypeSymbol*, Cm::Sym::TypeSymbol*> p = std::make_pair(firstType, nullptr);
            InterfaceTypeOpCache& cache = interfaceTypeOpCacheMap[p];
            Cm::Sym::FunctionSymbol* ctor = cache.GetDefaultCtor(typeRepository, firstType->GetBaseType());
            viableFunctions.insert(ctor);
        }
    }
    else if (arity == 2)
    {
        Cm::Sym::TypeSymbol* firstType = arguments[0].Type();
        if (firstType->IsPointerToInterfaceTypeSymbol())
        {
            Cm::Sym::TypeSymbol* secondType = arguments[1].Type();
            Cm::Sym::TypeSymbol* constRefIntfType = typeRepository.MakeConstReferenceType(firstType->GetBaseType(), Cm::Parsing::Span());
            if (Cm::Sym::TypesEqual(secondType, constRefIntfType))
            {
                std::pair<Cm::Sym::TypeSymbol*, Cm::Sym::TypeSymbol*> p = std::make_pair(firstType->GetBaseType(), nullptr);
                InterfaceTypeOpCache& cache = interfaceTypeOpCacheMap[p];
                Cm::Sym::FunctionSymbol* ctor = cache.GetCopyCtor(typeRepository, firstType->GetBaseType());
                viableFunctions.insert(ctor);
            }
            else if (Cm::Sym::TypesEqual(secondType, firstType->GetBaseType()))
            {
                std::pair<Cm::Sym::TypeSymbol*, Cm::Sym::TypeSymbol*> p = std::make_pair(firstType->GetBaseType(), nullptr);
                InterfaceTypeOpCache& cache = interfaceTypeOpCacheMap[p];
                Cm::Sym::FunctionSymbol* ctor = cache.GetCopyCtor(typeRepository, firstType->GetBaseType());
                viableFunctions.insert(ctor);
            }
            else if (secondType->IsPointerToClassType())
            {
                Cm::Sym::InterfaceTypeSymbol* intfType = static_cast<Cm::Sym::InterfaceTypeSymbol*>(firstType->GetBaseType());
                Cm::Sym::ClassTypeSymbol* classType = static_cast<Cm::Sym::ClassTypeSymbol*>(secondType->GetBaseType());
                bool found = false;
                for (Cm::Sym::InterfaceTypeSymbol* implIntf : classType->ImplementedInterfaces())
                {
                    if (Cm::Sym::TypesEqual(implIntf, intfType))
                    {
                        found = true;
                        break;
                    }
                }
                if (!found) return;
                std::pair<Cm::Sym::TypeSymbol*, Cm::Sym::TypeSymbol*> p = std::make_pair(firstType->GetBaseType(), secondType);
                InterfaceTypeOpCache& cache = interfaceTypeOpCacheMap[p];
                Cm::Sym::FunctionSymbol* ctor = cache.GetInterfaceObjectFromClassPtrCtor(typeRepository, firstType->GetBaseType(), secondType);
                viableFunctions.insert(ctor);
            }
        }
    }
}

void InterfaceAssignmentOpGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, const Cm::Parsing::Span& span, Cm::Sym::TypeRepository& typeRepository,
    InterfaceTypeOpCacheMap& interfaceTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity == 2)
    {
        Cm::Sym::TypeSymbol* firstType = arguments[0].Type();
        if (firstType->IsPointerToInterfaceTypeSymbol())
        {
            Cm::Sym::TypeSymbol* secondType = arguments[1].Type();
            Cm::Sym::TypeSymbol* constRefIntfType = typeRepository.MakeConstReferenceType(firstType->GetBaseType(), Cm::Parsing::Span());
            if (Cm::Sym::TypesEqual(secondType, constRefIntfType))
            {
                std::pair<Cm::Sym::TypeSymbol*, Cm::Sym::TypeSymbol*> p = std::make_pair(firstType->GetBaseType(), nullptr);
                InterfaceTypeOpCache& cache = interfaceTypeOpCacheMap[p];
                Cm::Sym::FunctionSymbol* assignment = cache.GetCopyAssignment(typeRepository, firstType->GetBaseType());
                viableFunctions.insert(assignment);
            }
            else if (Cm::Sym::TypesEqual(secondType, firstType->GetBaseType()))
            {
                std::pair<Cm::Sym::TypeSymbol*, Cm::Sym::TypeSymbol*> p = std::make_pair(firstType->GetBaseType(), nullptr);
                InterfaceTypeOpCache& cache = interfaceTypeOpCacheMap[p];
                Cm::Sym::FunctionSymbol* assignment = cache.GetCopyAssignment(typeRepository, firstType->GetBaseType());
                viableFunctions.insert(assignment);
            }
        }
    }
}

void InterfaceOpEqualGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, const Cm::Parsing::Span& span, Cm::Sym::TypeRepository& typeRepository,
    InterfaceTypeOpCacheMap& interfaceTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity == 2)
    {
        Cm::Sym::TypeSymbol* firstType = arguments[0].Type();
        Cm::Sym::TypeSymbol* plainFirstType = typeRepository.MakePlainType(firstType);
        if (plainFirstType->IsInterfaceTypeSymbol())
        {
            Cm::Sym::TypeSymbol* secondType = arguments[1].Type();
            Cm::Sym::TypeSymbol* plainSecondType = typeRepository.MakePlainType(secondType);
            if (plainSecondType->IsInterfaceTypeSymbol())
            {
                if (Cm::Sym::TypesEqual(plainFirstType, plainSecondType))
                {
                    std::pair<Cm::Sym::TypeSymbol*, Cm::Sym::TypeSymbol*> p = std::make_pair(plainFirstType, nullptr);
                    InterfaceTypeOpCache& cache = interfaceTypeOpCacheMap[p];
                    Cm::Sym::FunctionSymbol* opEqual = cache.GetOpEqual(typeRepository, plainFirstType);
                    viableFunctions.insert(opEqual);
                }
            }
        }
    }
}

InterfaceTypeOpRepository::InterfaceTypeOpRepository(Cm::Sym::TypeRepository& typeRepository_) : typeRepository(typeRepository_)
{
    groupMap["@constructor"] = &ctorOpGroup;
    groupMap["operator="] = &assignmentOpGroup;
    groupMap["operator=="] = &opEqualGroup;
}

void InterfaceTypeOpRepository::CollectViableFunctions(Cm::Sym::ContainerScope* containerScope, const std::string& groupName, int arity, std::vector<Cm::Core::Argument>& arguments, 
    const Cm::Parsing::Span& span, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    std::unordered_map<std::string, InterfaceTypeOpGroup*>::const_iterator i = groupMap.find(groupName);
    if (i != groupMap.cend())
    {
        InterfaceTypeOpGroup* group = i->second;
        group->CollectViableFunctions(arity, arguments, span, typeRepository, cacheMap, viableFunctions);
    }
}

} } // namespace Cm::Core
