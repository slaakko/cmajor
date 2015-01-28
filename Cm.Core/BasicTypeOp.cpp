/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/TypeRepository.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Core {

using Cm::Parsing::Span;

BasicTypeOp::BasicTypeOp(Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*basic_type_op*"), type(type_)
{ 
    SetAccess(Cm::Sym::SymbolAccess::public_);
}

DefaultCtor::DefaultCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("@constructor");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(Type(), Span()));
    AddSymbol(thisParam);
    ComputeName();
}

void DefaultCtor::Generate(Emitter& emitter, GenResult& result)
{
    Cm::IrIntf::Init(emitter, GetIrType(), GetDefaultIrValue(), result.MainObject());
}

CopyCtor::CopyCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("@constructor");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(Type(), Span()));
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* thatParam(new Cm::Sym::ParameterSymbol(Span(), "that"));
    thatParam->SetType(Type());
    AddSymbol(thatParam);
    ComputeName();
}

void CopyCtor::Generate(Emitter& emitter, GenResult& result)
{
    Cm::IrIntf::Init(emitter, GetIrType(), result.Arg1(), result.MainObject());
}

CopyAssignment::CopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator=");
    Cm::Sym::TypeSymbol* voidType = typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId));
    SetReturnType(voidType);
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(Type(), Span()));
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* thatParam(new Cm::Sym::ParameterSymbol(Span(), "that"));
    thatParam->SetType(Type());
    AddSymbol(thatParam);
    ComputeName();
}

void CopyAssignment::Generate(Emitter& emitter, GenResult& result)
{
    Cm::IrIntf::Assign(emitter, GetIrType(), result.Arg1(), result.MainObject());
}

MoveCtor::MoveCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("@constructor");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(Type(), Span()));
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* thatParam(new Cm::Sym::ParameterSymbol(Span(), "that"));
    thatParam->SetType(typeRepository.MakeRvalueRefType(Type(), Span()));
    AddSymbol(thatParam);
    ComputeName();
}

void MoveCtor::Generate(Emitter& emitter, GenResult& result)
{
    Cm::IrIntf::Init(emitter, GetIrType(), result.Arg1(), result.MainObject());
}

MoveAssignment::MoveAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator=");
    Cm::Sym::TypeSymbol* voidType = typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId));
    SetReturnType(voidType);
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(Type(), Span()));
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* thatParam(new Cm::Sym::ParameterSymbol(Span(), "that"));
    thatParam->SetType(typeRepository.MakeRvalueRefType(Type(), Span()));
    AddSymbol(thatParam);
    ComputeName();
}

void MoveAssignment::Generate(Emitter& emitter, GenResult& result)
{
    Cm::IrIntf::Assign(emitter, GetIrType(), result.Arg1(), result.MainObject());
}

OpEqual::OpEqual(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator==");
    Cm::Sym::TypeSymbol* boolType = typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
    SetReturnType(boolType);
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(Type());
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(Type());
    AddSymbol(rightParam);
    ComputeName();
}

void OpEqual::Generate(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Object* arg1 = result.Arg1();
    Ir::Intf::Object* arg2 = result.Arg2();
    Ir::Intf::Type* ptrType = Cm::IrIntf::Pointer(GetIrType(), 1);
    emitter.Own(ptrType);
    if (Cm::IrIntf::TypesEqual(arg1->GetType(), ptrType))
    {
        arg1 = Cm::IrIntf::CreateTemporaryRegVar(GetIrType());
        emitter.Own(arg1);
        Cm::IrIntf::Assign(emitter, GetIrType(), result.Arg1(), arg1);
    }
    if (Cm::IrIntf::TypesEqual(arg2->GetType(), ptrType))
    {
        arg2 = Cm::IrIntf::CreateTemporaryRegVar(GetIrType());
        emitter.Own(arg2);
        Cm::IrIntf::Assign(emitter, GetIrType(), result.Arg2(), arg2);
    }
    if (Type()->IsFloatingPointTypeSymbol())
    {
        emitter.Emit(Cm::IrIntf::FCmp(GetIrType(), result.MainObject(), Ir::Intf::FConditionCode::oeq, arg1, arg2));
    }
    else
    {
        emitter.Emit(Cm::IrIntf::ICmp(GetIrType(), result.MainObject(), Ir::Intf::IConditionCode::eq, arg1, arg2));
    }
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

OpLess::OpLess(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator<");
    Cm::Sym::TypeSymbol* boolType = typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
    SetReturnType(boolType);
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(Type());
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(Type());
    AddSymbol(rightParam);
    ComputeName();
}

void OpLess::Generate(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Object* arg1 = result.Arg1();
    Ir::Intf::Object* arg2 = result.Arg2();
    Ir::Intf::Type* ptrType = Cm::IrIntf::Pointer(GetIrType(), 1);
    emitter.Own(ptrType);
    if (Cm::IrIntf::TypesEqual(arg1->GetType(), ptrType))
    {
        arg1 = Cm::IrIntf::CreateTemporaryRegVar(GetIrType());
        emitter.Own(arg1);
        Cm::IrIntf::Assign(emitter, GetIrType(), result.Arg1(), arg1);
    }
    if (Cm::IrIntf::TypesEqual(arg2->GetType(), ptrType))
    {
        arg2 = Cm::IrIntf::CreateTemporaryRegVar(GetIrType());
        emitter.Own(arg2);
        Cm::IrIntf::Assign(emitter, GetIrType(), result.Arg2(), arg2);
    }
    if (Type()->IsFloatingPointTypeSymbol())
    {
        emitter.Emit(Cm::IrIntf::FCmp(GetIrType(), result.MainObject(), Ir::Intf::FConditionCode::olt, arg1, arg2));
    }
    else 
    {
        Ir::Intf::IConditionCode condCode = Type()->IsSignedType() ? Ir::Intf::IConditionCode::slt : Ir::Intf::IConditionCode::ult;
        emitter.Emit(Cm::IrIntf::ICmp(GetIrType(), result.MainObject(), condCode, arg1, arg2));
    }
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

BinOp::BinOp(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_, const std::string& groupName_) : BasicTypeOp(type_)
{
    SetGroupName(groupName_);
    SetReturnType(Type());
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(Type());
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(Type());
    AddSymbol(rightParam);
    ComputeName();
}

void BinOp::Generate(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Object* arg1 = result.Arg1();
    Ir::Intf::Object* arg2 = result.Arg2();
    Ir::Intf::Type* ptrArg1 = Cm::IrIntf::Pointer(arg1->GetType(), arg1->GetType()->NumPointers() + 1);
    emitter.Own(ptrArg1);
    if (Cm::IrIntf::TypesEqual(ptrArg1, arg2->GetType()))
    {
        arg2 = Cm::IrIntf::CreateTemporaryRegVar(GetIrType());
        emitter.Own(arg2);
        Cm::IrIntf::Assign(emitter, GetIrType(), result.Arg2(), arg2);
    }
    else
    {
        Ir::Intf::Type* ptrArg2 = Cm::IrIntf::Pointer(arg2->GetType(), arg2->GetType()->NumPointers() + 1);
        emitter.Own(ptrArg2);
        if (Cm::IrIntf::TypesEqual(arg1->GetType(), ptrArg2))
        {
            arg1 = Cm::IrIntf::CreateTemporaryRegVar(GetIrType());
            emitter.Own(arg1);
            Cm::IrIntf::Assign(emitter, GetIrType(), result.Arg1(), arg1);
        }
    }
    emitter.Emit(CreateInstruction(GetIrType(), result.MainObject(), arg1, arg2));
}

OpAdd::OpAdd(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BinOp(typeRepository, type_, "operator+")
{
}

Ir::Intf::Instruction* OpAdd::CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const
{
    if (Type()->IsFloatingPointTypeSymbol())
    {
        return Cm::IrIntf::FAdd(irType, result, operand1, operand2);
    }
    else
    {
        return Cm::IrIntf::Add(irType, result, operand1, operand2);
    }
}

OpSub::OpSub(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BinOp(typeRepository, type_, "operator-")
{
}

Ir::Intf::Instruction* OpSub::CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const
{
    if (Type()->IsFloatingPointTypeSymbol())
    {
        return Cm::IrIntf::FSub(irType, result, operand1, operand2);
    }
    else
    {
        return Cm::IrIntf::Sub(irType, result, operand1, operand2);
    }
}

OpMul::OpMul(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BinOp(typeRepository, type_, "operator*")
{
}

Ir::Intf::Instruction* OpMul::CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const
{
    if (Type()->IsFloatingPointTypeSymbol())
    {
        return Cm::IrIntf::FMul(irType, result, operand1, operand2);
    }
    else
    {
        return Cm::IrIntf::Mul(irType, result, operand1, operand2);
    }
}

OpDiv::OpDiv(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BinOp(typeRepository, type_, "operator/")
{
}

Ir::Intf::Instruction* OpDiv::CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const
{
    if (Type()->IsFloatingPointTypeSymbol())
    {
        return Cm::IrIntf::FDiv(irType, result, operand1, operand2);
    }
    else
    {
        if (Type()->IsSignedType())
        {
            return Cm::IrIntf::SDiv(irType, result, operand1, operand2);
        }
        else
        {
            return Cm::IrIntf::UDiv(irType, result, operand1, operand2);
        }
    }
}

OpRem::OpRem(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BinOp(typeRepository, type_, "operator%")
{
}

Ir::Intf::Instruction* OpRem::CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const
{
    if (Type()->IsSignedType())
    {
        return Cm::IrIntf::SRem(irType, result, operand1, operand2);
    }
    else
    {
        return Cm::IrIntf::URem(irType, result, operand1, operand2);
    }
}

OpShl::OpShl(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BinOp(typeRepository, type_, "operator<<")
{
}

Ir::Intf::Instruction* OpShl::CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const
{
    return Cm::IrIntf::Shl(irType, result, operand1, operand2);
}

OpShr::OpShr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BinOp(typeRepository, type_, "operator>>")
{
}

Ir::Intf::Instruction* OpShr::CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const
{
    if (Type()->IsSignedType())
    {
        return Cm::IrIntf::AShr(irType, result, operand1, operand2);
    }
    else
    {
        return Cm::IrIntf::LShr(irType, result, operand1, operand2);
    }
}

OpBitAnd::OpBitAnd(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BinOp(typeRepository, type_, "operator&")
{
}

Ir::Intf::Instruction* OpBitAnd::CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const
{
    return Cm::IrIntf::And(irType, result, operand1, operand2);
}

OpBitOr::OpBitOr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BinOp(typeRepository, type_, "operator|")
{
}

Ir::Intf::Instruction* OpBitOr::CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const
{
    return Cm::IrIntf::Or(irType, result, operand1, operand2);
}

OpBitXor::OpBitXor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BinOp(typeRepository, type_, "operator^")
{
}

Ir::Intf::Instruction* OpBitXor::CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const
{
    return Cm::IrIntf::Xor(irType, result, operand1, operand2);
}

OpNot::OpNot(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator!");
    Cm::Sym::TypeSymbol* boolType = typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
    SetReturnType(boolType);
    Cm::Sym::ParameterSymbol* operandParam(new Cm::Sym::ParameterSymbol(Span(), "operand"));
    operandParam->SetType(Type());
    AddSymbol(operandParam);
    ComputeName();
}

void OpNot::Generate(Emitter& emitter, GenResult& result)
{
    if (result.GenJumpingBoolCode())
    {
        Ir::Intf::Object* true_ = Cm::IrIntf::True();
        emitter.Own(true_);
        emitter.Emit(Cm::IrIntf::Xor(result.MainObject()->GetType(), result.MainObject(), result.Arg1(), true_));
        Ir::Intf::LabelObject* trueLabel = Cm::IrIntf::CreateLabel();
        emitter.Own(trueLabel);
        Ir::Intf::LabelObject* falseLabel = Cm::IrIntf::CreateLabel();
        emitter.Own(falseLabel);
        emitter.Emit(Cm::IrIntf::Br(result.MainObject(), trueLabel, falseLabel));
        result.AddTrueTarget(trueLabel);
        result.AddFalseTarget(falseLabel);
    }
    else
    {
        Ir::Intf::Object* true_ = Cm::IrIntf::True();
        emitter.Own(true_);
        emitter.Emit(Cm::IrIntf::Xor(result.MainObject()->GetType(), result.MainObject(), result.Arg1(), true_));
    }
}

OpUnaryPlus::OpUnaryPlus(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator+");
    SetReturnType(Type());
    Cm::Sym::ParameterSymbol* operandParam(new Cm::Sym::ParameterSymbol(Span(), "operand"));
    operandParam->SetType(Type());
    AddSymbol(operandParam);
    ComputeName();
}

void OpUnaryPlus::Generate(Emitter& emitter, GenResult& result)
{
    result.SetMainObject(result.Arg1());
}

OpUnaryMinus::OpUnaryMinus(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator-");
    SetReturnType(Type());
    Cm::Sym::ParameterSymbol* operandParam(new Cm::Sym::ParameterSymbol(Span(), "operand"));
    operandParam->SetType(Type());
    AddSymbol(operandParam);
    ComputeName();
}

void OpUnaryMinus::Generate(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Object* arg1 = result.Arg1();
    Ir::Intf::Type* ptrType = Cm::IrIntf::Pointer(GetIrType(), GetIrType()->NumPointers() + 1);
    emitter.Own(ptrType);
    if (Cm::IrIntf::TypesEqual(arg1->GetType(), ptrType))
    {
        arg1 = Cm::IrIntf::CreateTemporaryRegVar(GetIrType());
        emitter.Own(arg1);
        Cm::IrIntf::Assign(emitter, GetIrType(), result.Arg1(), arg1);
    }
    if (Type()->IsFloatingPointTypeSymbol())
    {
        emitter.Emit(Cm::IrIntf::FSub(GetIrType(), result.MainObject(), GetDefaultIrValue(), arg1));
    }
    else
    {
        emitter.Emit(Cm::IrIntf::Sub(GetIrType(), result.MainObject(), GetDefaultIrValue(), arg1));
    }
}

OpComplement::OpComplement(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator~");
    SetReturnType(Type());
    Cm::Sym::ParameterSymbol* operandParam(new Cm::Sym::ParameterSymbol(Span(), "operand"));
    operandParam->SetType(Type());
    AddSymbol(operandParam);
    ComputeName();
}

void OpComplement::Generate(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Object* arg1 = result.Arg1();
    Ir::Intf::Type* ptrType = Cm::IrIntf::Pointer(GetIrType(), GetIrType()->NumPointers() + 1);
    emitter.Own(ptrType);
    if (Cm::IrIntf::TypesEqual(arg1->GetType(), ptrType))
    {
        arg1 = Cm::IrIntf::CreateTemporaryRegVar(GetIrType());
        emitter.Own(arg1);
        Cm::IrIntf::Assign(emitter, GetIrType(), result.Arg1(), arg1);
    }
    Ir::Intf::Object* minus1 = GetIrType()->CreateMinusOne();
    emitter.Own(minus1);
    emitter.Emit(Cm::IrIntf::Xor(GetIrType(), result.MainObject(), arg1, minus1));

}

OpIncrement::OpIncrement(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator++");
    SetReturnType(Type());
    Cm::Sym::ParameterSymbol* operandParam(new Cm::Sym::ParameterSymbol(Span(), "operand"));
    operandParam->SetType(Type());
    AddSymbol(operandParam);
    ComputeName();
}

void OpIncrement::Generate(Emitter& emitter, GenResult& result)
{
    Ir::Intf::RegVar* arg1 = Cm::IrIntf::CreateTemporaryRegVar(result.Arg1()->GetType());
    emitter.Own(arg1);
    Cm::IrIntf::Assign(emitter, GetIrType(), result.Arg1(), arg1);
    Ir::Intf::Object* one = GetIrType()->CreatePlusOne();
    emitter.Own(one);
    emitter.Emit(Cm::IrIntf::Add(GetIrType(), result.MainObject(), arg1, one));
    Cm::IrIntf::Assign(emitter, GetIrType(), result.MainObject(), result.Arg1());
}

OpDecrement::OpDecrement(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator--");
    SetReturnType(Type());
    Cm::Sym::ParameterSymbol* operandParam(new Cm::Sym::ParameterSymbol(Span(), "operand"));
    operandParam->SetType(Type());
    AddSymbol(operandParam);
    ComputeName();
}

void OpDecrement::Generate(Emitter& emitter, GenResult& result)
{
    Ir::Intf::RegVar* arg1 = Cm::IrIntf::CreateTemporaryRegVar(result.Arg1()->GetType());
    emitter.Own(arg1);
    Cm::IrIntf::Assign(emitter, GetIrType(), result.Arg1(), arg1);
    Ir::Intf::Object* one = GetIrType()->CreatePlusOne();
    emitter.Own(one);
    emitter.Emit(Cm::IrIntf::Sub(GetIrType(), result.MainObject(), arg1, one));
    Cm::IrIntf::Assign(emitter, GetIrType(), result.MainObject(), result.Arg1());
}

ConvertingCtor::ConvertingCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* targetType_, Cm::Sym::TypeSymbol* sourceType_, Cm::Sym::ConversionType conversionType_, ConversionInst conversionInst_,
    Cm::Sym::ConversionRank conversionRank_, int conversionDistance_) : BasicTypeOp(targetType_), targetType(targetType_), sourceType(sourceType_), conversionType(conversionType_), conversionInst(conversionInst_),
    conversionRank(conversionRank_), conversionDistance(conversionDistance_)
{
    SetGroupName("@constructor");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(targetType, Span()));
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* thatParam(new Cm::Sym::ParameterSymbol(Span(), "that"));
    thatParam->SetType(sourceType);
    AddSymbol(thatParam);
    ComputeName();
}

void ConvertingCtor::Generate(Emitter& emitter, GenResult& result)
{
    if (conversionInst == ConversionInst::none || Cm::IrIntf::TypesEqual(sourceType->GetIrType(), targetType->GetIrType()))
    {
        if (result.AddrArg())
        {
            Cm::IrIntf::Init(emitter, targetType->GetIrType(), result.Arg1(), result.MainObject());
        }
        else
        {
            result.SetMainObject(result.Arg1());
        }
        return;
    }
    Ir::Intf::Object* from = result.Arg1();
    Ir::Intf::Object* to = result.MainObject();
    Ir::Intf::Object* origTo = nullptr;
    Ir::Intf::Type* ptrTarget = Cm::IrIntf::Pointer(targetType->GetIrType(), targetType->GetIrType()->NumPointers() + 1);
    emitter.Own(ptrTarget);
    if (!to->IsRegVar() || to->IsRegVar() && Cm::IrIntf::TypesEqual(to->GetType(), ptrTarget))
    {
        origTo = to;
        to = Cm::IrIntf::CreateTemporaryRegVar(targetType->GetIrType());
        emitter.Own(to);
    }
    Ir::Intf::Type* ptrSource = Cm::IrIntf::Pointer(sourceType->GetIrType(), sourceType->GetIrType()->NumPointers() + 1);
    emitter.Own(ptrSource);
    if (!result.ClassTypeToPointerTypeConversion() && !sourceType->IsFunctionType() &&
        (!from->IsGlobal() || from->IsGlobal() && Cm::IrIntf::TypesEqual(from->GetType(), ptrSource)) &&
        (!from->IsRegVar() || from->IsRegVar() && Cm::IrIntf::TypesEqual(from->GetType(), ptrSource)) &&
        !from->IsConstant() && !result.AddrArg())
    {
        Ir::Intf::Object* origFrom = from;
        from = Cm::IrIntf::CreateTemporaryRegVar(sourceType->GetIrType());
        emitter.Own(from);
        Cm::IrIntf::Assign(emitter, sourceType->GetIrType(), origFrom, from);
    }
    switch (conversionInst)
    {
        case ConversionInst::sext: emitter.Emit(Cm::IrIntf::Sext(sourceType->GetIrType(), to, from, targetType->GetIrType())); break;
        case ConversionInst::zext: emitter.Emit(Cm::IrIntf::Zext(sourceType->GetIrType(), to, from, targetType->GetIrType())); break;
        case ConversionInst::trunc: emitter.Emit(Cm::IrIntf::Trunc(sourceType->GetIrType(), to, from, targetType->GetIrType())); break;
        case ConversionInst::bitcast: emitter.Emit(Cm::IrIntf::Bitcast(sourceType->GetIrType(), to, from, targetType->GetIrType())); break;
        case ConversionInst::uitofp: emitter.Emit(Cm::IrIntf::Uitofp(sourceType->GetIrType(), to, from, targetType->GetIrType())); break;
        case ConversionInst::sitofp: emitter.Emit(Cm::IrIntf::Sitofp(sourceType->GetIrType(), to, from, targetType->GetIrType())); break;
        case ConversionInst::fptosi: emitter.Emit(Cm::IrIntf::Fptosi(sourceType->GetIrType(), to, from, targetType->GetIrType())); break;
        case ConversionInst::fptoui: emitter.Emit(Cm::IrIntf::Fptoui(sourceType->GetIrType(), to, from, targetType->GetIrType())); break;
        case ConversionInst::fpext: emitter.Emit(Cm::IrIntf::Fpext(sourceType->GetIrType(), to, from, targetType->GetIrType())); break;
        case ConversionInst::fptrunc: emitter.Emit(Cm::IrIntf::Fptrunc(sourceType->GetIrType(), to, from, targetType->GetIrType())); break;
        case ConversionInst::ptrtoint: emitter.Emit(Cm::IrIntf::Ptrtoint(sourceType->GetIrType(), to, from, targetType->GetIrType())); break;
    }
    if (origTo)
    {
        Cm::IrIntf::Assign(emitter, targetType->GetIrType(), to, origTo);
    }
}

} } // namespace Cm::Core
