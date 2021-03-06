/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/DerivedTypeOpRepository.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <stdexcept>

namespace Cm { namespace Core {

using Cm::Parsing::Span;

OpAddPtrInt::OpAddPtrInt(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator+");
    SetReturnType(Type());
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(Type());
    leftParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId)));
    rightParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(rightParam);
    ComputeName();
    Cm::Sym::EntrySymbol* entry = new Cm::Sym::EntrySymbol(Span());
    AddSymbol(entry);
}

void OpAddPtrInt::Generate(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Type* ptrType = Type()->GetIrType();
    Ir::Intf::Object* arg1 = result.Arg1();
    if (!arg1->IsConstant() && !arg1->IsRegVar())
    {
        arg1 = Cm::IrIntf::CreateTemporaryRegVar(ptrType);
        emitter.Own(arg1);
        Cm::IrIntf::Assign(emitter, arg1->GetType(), result.Arg1(), arg1);
    }
    Ir::Intf::Object* arg2 = result.Arg2();
    if (!arg2->IsConstant() && !arg2->IsRegVar())
    {
        arg2 = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI32());
        emitter.Own(arg2);
        Cm::IrIntf::Assign(emitter, Ir::Intf::GetFactory()->GetI32(), result.Arg2(), arg2);
    }
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        emitter.Emit(Cm::IrIntf::GetElementPtr(ptrType, result.MainObject(), arg1, arg2));
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        emitter.Emit(Cm::IrIntf::Add(ptrType, result.MainObject(), arg1, arg2));
    }
}

OpAddIntPtr::OpAddIntPtr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator+");
    SetReturnType(Type());
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId)));
    leftParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(Type());
    rightParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(rightParam);
    ComputeName();
    Cm::Sym::EntrySymbol* entry = new Cm::Sym::EntrySymbol(Span());
    AddSymbol(entry);
}

void OpAddIntPtr::Generate(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Type* ptrType = Type()->GetIrType();
    Ir::Intf::Object* arg1 = result.Arg1();
    if (!arg1->IsConstant() && !arg1->IsRegVar())
    {
        arg1 = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI32());
        emitter.Own(arg1);
        Cm::IrIntf::Assign(emitter, Ir::Intf::GetFactory()->GetI32(), result.Arg1(), arg1);
    }
    Ir::Intf::Object* arg2 = result.Arg2();
    if (!arg2->IsConstant() && !arg2->IsRegVar())
    {
        arg2 = Cm::IrIntf::CreateTemporaryRegVar(ptrType);
        emitter.Own(arg2);
        Cm::IrIntf::Assign(emitter, arg2->GetType(), result.Arg2(), arg2);
    }
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        emitter.Emit(Cm::IrIntf::GetElementPtr(ptrType, result.MainObject(), arg2, arg1));
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        emitter.Emit(Cm::IrIntf::Add(ptrType, result.MainObject(), arg1, arg2));
    }
}

OpSubPtrInt::OpSubPtrInt(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator-");
    SetReturnType(Type());
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(Type());
    leftParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId)));
    rightParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(rightParam);
    ComputeName();
    Cm::Sym::EntrySymbol* entry = new Cm::Sym::EntrySymbol(Span());
    AddSymbol(entry);
}

void OpSubPtrInt::Generate(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Type* ptrType = Type()->GetIrType();
    Ir::Intf::Object* arg1 = result.Arg1();
    if (!arg1->IsConstant() && !arg1->IsRegVar())
    {
        arg1 = Cm::IrIntf::CreateTemporaryRegVar(ptrType);
        emitter.Own(arg1);
        Cm::IrIntf::Assign(emitter, arg1->GetType(), result.Arg1(), arg1);
    }
    Ir::Intf::Object* arg2 = result.Arg2();
    if (!arg2->IsConstant() && !arg2->IsRegVar())
    {
        arg2 = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI32());
        emitter.Own(arg2);
        Cm::IrIntf::Assign(emitter, Ir::Intf::GetFactory()->GetI32(), result.Arg2(), arg2);
    }
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        Ir::Intf::Object* minusArg2 = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI32());
        emitter.Own(minusArg2);
        Ir::Intf::Object* zero = Ir::Intf::GetFactory()->GetI32()->CreateDefaultValue();
        emitter.Own(zero);
        emitter.Emit(Cm::IrIntf::Sub(Ir::Intf::GetFactory()->GetI32(), minusArg2, zero, arg2));
        emitter.Emit(Cm::IrIntf::GetElementPtr(ptrType, result.MainObject(), arg1, minusArg2));
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        emitter.Emit(Cm::IrIntf::Sub(ptrType, result.MainObject(), arg1, arg2));
    }
}

OpSubPtrPtr::OpSubPtrPtr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator-");
    SetReturnType(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId)));
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(Type());
    leftParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(Type());
    rightParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(rightParam);
    ComputeName();
    Cm::Sym::EntrySymbol* entry = new Cm::Sym::EntrySymbol(Span());
    AddSymbol(entry);
}

void OpSubPtrPtr::Generate(Emitter& emitter, GenResult& result)
{
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        Cm::Sym::TypeSymbol* pointeeType = Type()->GetBaseType();
        Ir::Intf::Type* pnteeType = pointeeType->GetIrType();
        Ir::Intf::RegVar* arg1I64 = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI64());
        emitter.Own(arg1I64);
        Ir::Intf::RegVar* arg1 = Cm::IrIntf::CreateTemporaryRegVar(result.Arg1()->GetType());
        emitter.Own(arg1);
        Cm::IrIntf::Assign(emitter, arg1->GetType(), result.Arg1(), arg1);
        emitter.Emit(Cm::IrIntf::Ptrtoint(Type()->GetIrType(), arg1I64, arg1, Ir::Intf::GetFactory()->GetI64()));
        Ir::Intf::RegVar* arg2I64 = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI64());
        emitter.Own(arg2I64);
        Ir::Intf::RegVar* arg2 = Cm::IrIntf::CreateTemporaryRegVar(result.Arg2()->GetType());
        emitter.Own(arg2);
        Cm::IrIntf::Assign(emitter, arg2->GetType(), result.Arg2(), arg2);
        emitter.Emit(Cm::IrIntf::Ptrtoint(Type()->GetIrType(), arg2I64, arg2, Ir::Intf::GetFactory()->GetI64()));
        Ir::Intf::RegVar* subResult = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI64());
        emitter.Own(subResult);
        emitter.Emit(Cm::IrIntf::Sub(Ir::Intf::GetFactory()->GetI64(), subResult, arg1I64, arg2I64));
        Ir::Intf::Object* size = Cm::IrIntf::SizeOf(emitter, pnteeType);
        Ir::Intf::RegVar* resultI64 = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI64());
        emitter.Own(resultI64);
        emitter.Emit(Cm::IrIntf::UDiv(Ir::Intf::GetFactory()->GetI64(), resultI64, subResult, size));
        emitter.Emit(Cm::IrIntf::Trunc(Ir::Intf::GetFactory()->GetI64(), result.MainObject(), resultI64, Ir::Intf::GetFactory()->GetI32()));
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        emitter.Emit(Cm::IrIntf::Sub(Type()->GetIrType(), result.MainObject(), result.Arg1(), result.Arg2()));
    }
}

OpDeref::OpDeref(Cm::Sym::TypeRepository& typeRepository_, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_), typeRepository(typeRepository_)
{
    SetGroupName("operator*");
    SetReturnType(typeRepository.MakePlainTypeWithOnePointerRemoved(Type()));
    Cm::Sym::ParameterSymbol* operandParam(new Cm::Sym::ParameterSymbol(Span(), "operand"));
    operandParam->SetType(Type());
    operandParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(operandParam);
    ComputeName();
    Cm::Sym::EntrySymbol* entry = new Cm::Sym::EntrySymbol(Span());
    AddSymbol(entry);
}

void OpDeref::Generate(Emitter& emitter, GenResult& result)
{
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        if ((result.Flags() & GenFlags::lvalue) != GenFlags::none)
        {
            result.MainObject()->SetType(Type()->GetIrType());
            Cm::IrIntf::Assign(emitter, Type()->GetIrType(), result.Arg1(), result.MainObject());
        }
        else
        {
            Cm::Sym::TypeSymbol* pointeeType = typeRepository.MakePlainTypeWithOnePointerRemoved(Type());
            if (pointeeType->IsClassTypeSymbol())
            {
                Cm::IrIntf::Assign(emitter, Type()->GetIrType(), result.Arg1(), result.MainObject());
                result.MainObject()->SetType(Type()->GetIrType());
            }
            else
            {
                emitter.Emit(Cm::IrIntf::Load(Type()->GetIrType(), result.MainObject(), result.Arg1()));
            }
        }
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        if ((result.Flags() & GenFlags::lvalue) != GenFlags::none)
        {
            result.MainObject()->SetType(Type()->GetIrType());
            Cm::IrIntf::Assign(emitter, Type()->GetIrType(), result.Arg1(), result.MainObject());
        }
        else
        {
            Cm::Sym::TypeSymbol* pointeeType = typeRepository.MakePlainTypeWithOnePointerRemoved(Type());
            if (pointeeType->IsClassTypeSymbol())
            {
                Cm::IrIntf::Assign(emitter, Type()->GetIrType(), result.Arg1(), result.MainObject());
                result.MainObject()->SetType(Type()->GetIrType());
            }
            else
            {
                emitter.Emit(Cm::IrIntf::Load(Type()->GetIrType(), result.MainObject(), result.Arg1(), Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
            }
        }
    }
}

OpIncPtr::OpIncPtr(Cm::Sym::TypeRepository& typeRepository_, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_), typeRepository(typeRepository_)
{
    SetGroupName("operator++");
    SetReturnType(Type());
    Cm::Sym::ParameterSymbol* operandParam(new Cm::Sym::ParameterSymbol(Span(), "operand"));
    operandParam->SetType(Type());
    operandParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(operandParam);
    ComputeName();
    Cm::Sym::EntrySymbol* entry = new Cm::Sym::EntrySymbol(Span());
    AddSymbol(entry);
}

void OpIncPtr::Generate(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Type* ptrType = Type()->GetIrType();
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        Ir::Intf::RegVar* arg1 = Cm::IrIntf::CreateTemporaryRegVar(result.Arg1()->GetType());
        emitter.Own(arg1);
        Cm::IrIntf::Assign(emitter, ptrType, result.Arg1(), arg1);
        Ir::Intf::Object* one = Cm::IrIntf::CreateI32Constant(1);
        emitter.Own(one);
        emitter.Emit(Cm::IrIntf::GetElementPtr(ptrType, result.MainObject(), arg1, one));
        Cm::IrIntf::Assign(emitter, ptrType, result.MainObject(), result.Arg1());
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        /*
        Ir::Intf::Object* one = Cm::IrIntf::CreateI32Constant(1);
        emitter.Own(one);
        emitter.Emit(Cm::IrIntf::Add(ptrType, result.MainObject(), result.Arg1(), one));
        Cm::IrIntf::Assign(emitter, ptrType, result.MainObject(), result.Arg1());
        */
        Ir::Intf::RegVar* arg1 = Cm::IrIntf::CreateTemporaryRegVar(GetIrType());
        emitter.Own(arg1);
        Cm::IrIntf::Assign(emitter, GetIrType(), result.Arg1(), arg1);
        Ir::Intf::Object* one = Cm::IrIntf::CreateI32Constant(1);
        emitter.Own(one);
        emitter.Emit(Cm::IrIntf::Add(GetIrType(), result.MainObject(), arg1, one));
        Cm::IrIntf::Assign(emitter, GetIrType(), result.MainObject(), result.Arg1());
    }
}

OpDecPtr::OpDecPtr(Cm::Sym::TypeRepository& typeRepository_, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_), typeRepository(typeRepository_)
{
    SetGroupName("operator--");
    SetReturnType(Type());
    Cm::Sym::ParameterSymbol* operandParam(new Cm::Sym::ParameterSymbol(Span(), "operand"));
    operandParam->SetType(Type());
    operandParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(operandParam);
    ComputeName();
    Cm::Sym::EntrySymbol* entry = new Cm::Sym::EntrySymbol(Span());
    AddSymbol(entry);
}

void OpDecPtr::Generate(Emitter& emitter, GenResult& result)
{
    Ir::Intf::Type* ptrType = Type()->GetIrType();
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        Ir::Intf::RegVar* arg1 = Cm::IrIntf::CreateTemporaryRegVar(result.Arg1()->GetType());
        emitter.Own(arg1);
        Cm::IrIntf::Assign(emitter, ptrType, result.Arg1(), arg1);
        Ir::Intf::Object* minusOne = Cm::IrIntf::CreateI32Constant(-1);
        emitter.Own(minusOne);
        emitter.Emit(Cm::IrIntf::GetElementPtr(ptrType, result.MainObject(), arg1, minusOne));
        Cm::IrIntf::Assign(emitter, ptrType, result.MainObject(), result.Arg1());
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        /*
        Ir::Intf::Object* minusOne = Cm::IrIntf::CreateI32Constant(-1);
        emitter.Own(minusOne);
        emitter.Emit(Cm::IrIntf::Add(ptrType, result.MainObject(), result.Arg1(), minusOne));
        Cm::IrIntf::Assign(emitter, ptrType, result.MainObject(), result.Arg1());
        */
        Ir::Intf::RegVar* arg1 = Cm::IrIntf::CreateTemporaryRegVar(GetIrType()); // arg1->GetType() changed to GetIrType() 
        emitter.Own(arg1);
        Cm::IrIntf::Assign(emitter, GetIrType(), result.Arg1(), arg1);
        Ir::Intf::Object* one = Cm::IrIntf::CreateI32Constant(1);
        emitter.Own(one);
        emitter.Emit(Cm::IrIntf::Sub(GetIrType(), result.MainObject(), arg1, one));
        Cm::IrIntf::Assign(emitter, GetIrType(), result.MainObject(), result.Arg1());
    }
}

OpAddrOf::OpAddrOf(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator&");
    SetReturnType(Type());
    Cm::Sym::ParameterSymbol* operandParam(new Cm::Sym::ParameterSymbol(Span(), "operand"));
    Cm::Sym::TypeSymbol* operandType = typeRepository.MakePlainTypeWithOnePointerRemoved(Type());
    operandParam->SetType(operandType);
    operandParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(operandParam);
    ComputeName();
    Cm::Sym::EntrySymbol* entry = new Cm::Sym::EntrySymbol(Span());
    AddSymbol(entry);
}

void OpAddrOf::Generate(Emitter& emitter, GenResult& result)
{
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        result.SetMainObject(result.Arg1()->CreateAddr(emitter, result.Arg1()->GetType()));
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        if (Cm::IrIntf::TypesEqual(result.MainObject()->GetType(), result.Arg1()->GetType()))
        {
            result.SetMainObject(result.Arg1()->CreateAddr(emitter, result.Arg1()->GetType()));
        }
        else
        {
            emitter.Emit(Cm::IrIntf::Load(result.MainObject()->GetType(), result.MainObject(), result.Arg1(), Ir::Intf::Indirection::none, Ir::Intf::Indirection::addr));
        }
    }
}

OpArrow::OpArrow(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator->");
    SetReturnType(Type());
    Cm::Sym::ParameterSymbol* operandParam(new Cm::Sym::ParameterSymbol(Span(), "operand"));
    operandParam->SetType(Type());
    operandParam->SetSid(typeRepository.GetSymbolTable().GetSid());
    AddSymbol(operandParam);
    ComputeName();
    Cm::Sym::EntrySymbol* entry = new Cm::Sym::EntrySymbol(Span());
    AddSymbol(entry);
}

void OpArrow::Generate(Emitter& emitter, GenResult& result)
{
    Cm::IrIntf::Assign(emitter, Type()->GetIrType(), result.Arg1(), result.MainObject());
}

DerivedTypeOpCache::DerivedTypeOpCache()
{
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetDefaultCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!defaultCtor)
    {
        defaultCtor.reset(new DefaultCtor(typeRepository, type));
    }
    return defaultCtor.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetCopyCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!copyCtor)
    {
        copyCtor.reset(new CopyCtor(typeRepository, type));
    }
    return copyCtor.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetMoveCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!moveCtor)
    {
        moveCtor.reset(new MoveCtor(typeRepository, type));
    }
    return moveCtor.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetPtrToVoidPtrConversion(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type, Cm::Sym::ConversionTable& conversionTable, const Span& span)
{
    if (!ptrToVoidPtrConversion)
    {
        ptrToVoidPtrConversion.reset(new ConvertingCtor(typeRepository, typeRepository.MakePointerType(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), span), type,
            Cm::Sym::ConversionType::implicit, ConversionInst::bitcast, Cm::Sym::ConversionRank::conversion, 100));
        conversionTable.AddConversion(ptrToVoidPtrConversion.get());
    }
    return ptrToVoidPtrConversion.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetVoidPtrToPtrConversion(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type, const Span& span)
{
    if (!voidPtrToPtrConversion)
    {
        voidPtrToPtrConversion.reset(new ConvertingCtor(typeRepository, type, typeRepository.MakePointerType(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), span),
            Cm::Sym::ConversionType::explicit_, ConversionInst::bitcast, Cm::Sym::ConversionRank::conversion, 100));
    }
    return voidPtrToPtrConversion.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetNullPtrToPtrConversion(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type, Cm::Sym::ConversionTable& conversionTable)
{ 
    if (!nullPtrToPtrConversion)
    {
        nullPtrToPtrConversion.reset(new ConvertingCtor(typeRepository, type, typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::nullPtrId)),
            Cm::Sym::ConversionType::implicit, ConversionInst::bitcast, Cm::Sym::ConversionRank::conversion, 1));
        conversionTable.AddConversion(nullPtrToPtrConversion.get());
    }
    return nullPtrToPtrConversion.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetVoidPtrToUlongConversion(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type, Cm::Sym::ConversionTable& conversionTable, const Span& span)
{
    if (!voidPtrToULongConversion)
    {
        Cm::Sym::TypeSymbol* ulongType = typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ulongId));
        Cm::Sym::TypeSymbol* voidPtrType = typeRepository.MakePointerType(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), span);
        voidPtrToULongConversion.reset(new ConvertingCtor(typeRepository, ulongType, voidPtrType, Cm::Sym::ConversionType::explicit_, ConversionInst::ptrtoint, Cm::Sym::ConversionRank::conversion, 100));
        conversionTable.AddConversion(voidPtrToULongConversion.get());
    }
    return voidPtrToULongConversion.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetExplicitPointerConversion(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* targetType, Cm::Sym::TypeSymbol* sourceType, Cm::Sym::ConversionTable& conversionTable)
{
    if (!explicitPointerConversion)
    {
        explicitPointerConversion.reset(new ConvertingCtor(typeRepository, targetType, sourceType, Cm::Sym::ConversionType::explicit_, ConversionInst::bitcast, Cm::Sym::ConversionRank::conversion, 100));
        conversionTable.AddConversion(explicitPointerConversion.get());
    }
    return explicitPointerConversion.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetCopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!copyAssignment)
    {
        copyAssignment.reset(new CopyAssignment(typeRepository, type));
    }
    return copyAssignment.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetMoveAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!moveAssignment)
    {
        moveAssignment.reset(new MoveAssignment(typeRepository, type));
    }
    return moveAssignment.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetOpEqual(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!opEqual)
    {
        opEqual.reset(new OpEqual(typeRepository, type));
    }
    return opEqual.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetOpLess(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!opLess)
    {
        opLess.reset(new OpLess(typeRepository, type));
    }
    return opLess.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetOpAddPtrInt(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!opAddPtrInt)
    {
        opAddPtrInt.reset(new OpAddPtrInt(typeRepository, type));
    }
    return opAddPtrInt.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetOpAddIntPtr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!opAddIntPtr)
    {
        opAddIntPtr.reset(new OpAddIntPtr(typeRepository, type));
    }
    return opAddIntPtr.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetOpSubPtrInt(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!opSubPtrInt)
    {
        opSubPtrInt.reset(new OpSubPtrInt(typeRepository, type));
    }
    return opSubPtrInt.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetOpSubPtrPtr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!opSubPtrPtr)
    {
        opSubPtrPtr.reset(new OpSubPtrPtr(typeRepository, type));
    }
    return opSubPtrPtr.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetOpDeref(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!opDeref)
    {
        opDeref.reset(new OpDeref(typeRepository, type));
    }
    return opDeref.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetOpIncPtr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!opIncPtr)
    {
        opIncPtr.reset(new OpIncPtr(typeRepository, type));
    }
    return opIncPtr.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetOpDecPtr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!opDecPtr)
    {
        opDecPtr.reset(new OpDecPtr(typeRepository, type));
    }
    return opDecPtr.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetOpAddrOf(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!opAddrOf)
    {
        opAddrOf.reset(new OpAddrOf(typeRepository, type));
    }
    return opAddrOf.get();
}

Cm::Sym::FunctionSymbol* DerivedTypeOpCache::GetOpArrow(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type)
{
    if (!opArrow)
    {
        opArrow.reset(new OpArrow(typeRepository, type));
    }
    return opArrow.get();
}

DerivedTypeOpGroup::~DerivedTypeOpGroup()
{
}

void ConstructorOpGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
    Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    switch (arity)
    {
        case 1:
        {
            Cm::Sym::TypeSymbol* type = arguments[0].Type();
            if (type->IsPointerType() && type->GetPointerCount() > 1) // pointer default constructor
            {
                Cm::Sym::TypeSymbol* pointerType = typeRepository.MakePlainTypeWithOnePointerRemoved(type);
                DerivedTypeOpCache& cache = derivedTypeOpCacheMap[pointerType];
                viableFunctions.insert(cache.GetDefaultCtor(typeRepository, pointerType));
            }
            break;
        }
        case 2:
        {
            Cm::Sym::TypeSymbol* leftType = arguments[0].Type();
            if (leftType->IsReferenceType())
            {
                Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
                Cm::Sym::TypeSymbol* leftPlainType = typeRepository.MakePlainTypeWithOnePointerRemoved(leftType);
                Cm::Sym::TypeSymbol* rightPlainType = typeRepository.MakePlainType(rightType);
                if (Cm::Sym::TypesEqual(leftPlainType, rightPlainType)) // reference type copy constructor
                {
                    if (leftType->IsConstType())
                    {
                        Cm::Sym::TypeSymbol* constReferenceType = typeRepository.MakeConstReferenceType(leftPlainType, span);
                        DerivedTypeOpCache& cache = derivedTypeOpCacheMap[constReferenceType];
                        viableFunctions.insert(cache.GetCopyCtor(typeRepository, constReferenceType));
                    }
                    else
                    {
                        Cm::Sym::TypeSymbol* referenceType = typeRepository.MakeReferenceType(leftPlainType, span);
                        DerivedTypeOpCache& cache = derivedTypeOpCacheMap[referenceType];
                        viableFunctions.insert(cache.GetCopyCtor(typeRepository, referenceType));
                    }
                }
                else if (leftPlainType->IsClassTypeSymbol() && rightPlainType->IsClassTypeSymbol()) // reference type copy constructor with class type params
                {
                    Cm::Sym::ClassTypeSymbol* leftClass = static_cast<Cm::Sym::ClassTypeSymbol*>(leftPlainType);
                    Cm::Sym::ClassTypeSymbol* rightClass = static_cast<Cm::Sym::ClassTypeSymbol*>(rightPlainType);
                    if (leftClass->HasBaseClass(rightClass) || rightClass->HasBaseClass(leftClass))
                    {
                        Cm::Sym::TypeSymbol* referenceType = typeRepository.MakeReferenceType(leftPlainType, span);
                        DerivedTypeOpCache& cache = derivedTypeOpCacheMap[referenceType];
                        viableFunctions.insert(cache.GetCopyCtor(typeRepository, referenceType));
                    }
                }
            }
            else if (leftType->IsRvalueRefType())
            {
                Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
                Cm::Sym::TypeSymbol* leftPlainType = typeRepository.MakePlainTypeWithOnePointerRemoved(leftType);
                Cm::Sym::TypeSymbol* rightPlainType = typeRepository.MakePlainType(rightType);
                if (Cm::Sym::TypesEqual(leftPlainType, rightPlainType)) // rvalue reference type copy constructor
                {
                    Cm::Sym::TypeSymbol* rvalueRefType = typeRepository.MakeRvalueRefType(leftPlainType, span);
                    DerivedTypeOpCache& cache = derivedTypeOpCacheMap[rvalueRefType];
                    viableFunctions.insert(cache.GetCopyCtor(typeRepository, rvalueRefType));
                }
            }
            else if (leftType->IsPointerType() && leftType->GetPointerCount() == 2 && leftType->GetBaseType()->IsVoidTypeSymbol()) // void* 
            {
                Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
                Cm::Sym::TypeSymbol* rightPlainType = typeRepository.MakePlainType(rightType);
                if (rightPlainType->GetBaseType()->IsVoidTypeSymbol())  // void* copy constructor
                {
                    DerivedTypeOpCache& cache = derivedTypeOpCacheMap[rightPlainType];
                    viableFunctions.insert(cache.GetCopyCtor(typeRepository, rightPlainType));
                }
                else if (rightType->IsPointerType()) // conversion from pointer type to void*
                {
                    DerivedTypeOpCache& cache = derivedTypeOpCacheMap[rightType];
                    Cm::Sym::FunctionSymbol* ptrToVoidPtrConversion = cache.GetPtrToVoidPtrConversion(typeRepository, rightType, conversionTable, span);
                    viableFunctions.insert(ptrToVoidPtrConversion);
                }
            }
            else if (leftType->IsPointerType() && leftType->GetPointerCount() > 1) // pointer constructor
            {
                Cm::Sym::TypeSymbol* pointerType = typeRepository.MakePlainTypeWithOnePointerRemoved(leftType);
                Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
                Cm::Sym::TypeSymbol* rightPlainType = typeRepository.MakePlainType(rightType);
                if (rightPlainType->IsVoidPtrType()) // pointer type to void* conversion;
                {
                    DerivedTypeOpCache& cache = derivedTypeOpCacheMap[pointerType];
                    viableFunctions.insert(cache.GetVoidPtrToPtrConversion(typeRepository, pointerType, span));
                    return;
                }
                else if (rightPlainType->IsNullPtrType()) // nullptr type to pointer type conversion
                {
                    DerivedTypeOpCache& cache = derivedTypeOpCacheMap[pointerType];
                    viableFunctions.insert(cache.GetNullPtrToPtrConversion(typeRepository, pointerType, conversionTable));
                    return;
                }
                else if (rightPlainType->IsPointerType())
                {
                    if (Cm::Sym::TypesEqual(pointerType, rightPlainType)) // pointer copy constructor
                    {
                        DerivedTypeOpCache& cache = derivedTypeOpCacheMap[pointerType];
                        viableFunctions.insert(cache.GetCopyCtor(typeRepository, pointerType));
                        return;
                    }
                    else
                    {
                        Cm::Sym::TypeSymbol* leftBaseType = pointerType->GetBaseType();
                        if (leftBaseType->IsClassTypeSymbol())
                        {
                            Cm::Sym::TypeSymbol* rightBaseType = rightPlainType->GetBaseType();
                            if (rightBaseType->IsClassTypeSymbol())
                            {
                                Cm::Sym::ClassTypeSymbol* leftClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(leftBaseType);
                                Cm::Sym::ClassTypeSymbol* rightClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(rightBaseType);
                                if (leftClassType->HasBaseClass(rightClassType) || rightClassType->HasBaseClass(leftClassType))
                                {
                                    DerivedTypeOpCache& cache = derivedTypeOpCacheMap[pointerType];
                                    viableFunctions.insert(cache.GetCopyCtor(typeRepository, pointerType));
                                    return;
                                }
                            }
                        }
                    }
                }
                Cm::Sym::TypeSymbol* alternateRightType = typeRepository.MakeConstReferenceType(pointerType, span);
                if (Cm::Sym::TypesEqual(alternateRightType, rightType)) // pointer copy constructor
                {
                    DerivedTypeOpCache& cache = derivedTypeOpCacheMap[pointerType];
                    viableFunctions.insert(cache.GetCopyCtor(typeRepository, pointerType));
                }
                else
                {
                    Cm::Sym::TypeSymbol* rvalueRefRightType = typeRepository.MakeRvalueRefType(pointerType, span);
                    if (Cm::Sym::TypesEqual(rvalueRefRightType, rightType)) // pointer move constructor
                    {
                        DerivedTypeOpCache& cache = derivedTypeOpCacheMap[pointerType];
                        viableFunctions.insert(cache.GetMoveCtor(typeRepository, pointerType));
                    }
                    else if (rightPlainType->IsPointerType())
                    {
                        DerivedTypeOpCache& cache = derivedTypeOpCacheMap[pointerType];
                        viableFunctions.insert(cache.GetExplicitPointerConversion(typeRepository, pointerType, rightPlainType, conversionTable));
                        return;
                    }
                }
            }
            else if (leftType->IsPointerType() && leftType->GetPointerCount() == 1 && leftType->GetBaseType()->IsUlongType())
            {
                Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
                if (rightType->IsVoidPtrType())
                {
                    DerivedTypeOpCache& cache = derivedTypeOpCacheMap[rightType];
                    viableFunctions.insert(cache.GetVoidPtrToUlongConversion(typeRepository, rightType, conversionTable, span));
                }
            }
            break;
        }
    }
}

void AssignmentOpGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
    Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 2) return;
    Cm::Sym::TypeSymbol* leftType = arguments[0].Type();
    if (leftType->IsReferenceType())
    {
        Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
        Cm::Sym::TypeSymbol* leftPlainType = typeRepository.MakePlainTypeWithOnePointerRemoved(leftType);
        Cm::Sym::TypeSymbol* rightPlainType = typeRepository.MakePlainType(rightType);
        if (Cm::Sym::TypesEqual(leftPlainType, rightPlainType)) // reference type copy assignment
        {
            if (leftType->IsConstType())
            {
                Cm::Sym::TypeSymbol* constReferenceType = typeRepository.MakeConstReferenceType(leftPlainType, span);
                DerivedTypeOpCache& cache = derivedTypeOpCacheMap[constReferenceType];
                viableFunctions.insert(cache.GetCopyAssignment(typeRepository, constReferenceType));
            }
            else
            {
                Cm::Sym::TypeSymbol* referenceType = typeRepository.MakeReferenceType(leftPlainType, span);
                DerivedTypeOpCache& cache = derivedTypeOpCacheMap[referenceType];
                viableFunctions.insert(cache.GetCopyAssignment(typeRepository, referenceType));
            }
        }
        else if (leftPlainType->IsClassTypeSymbol() && rightPlainType->IsClassTypeSymbol()) // reference type copy assignment with class type params
        {
            Cm::Sym::ClassTypeSymbol* leftClass = static_cast<Cm::Sym::ClassTypeSymbol*>(leftPlainType);
            Cm::Sym::ClassTypeSymbol* rightClass = static_cast<Cm::Sym::ClassTypeSymbol*>(rightPlainType);
            if (leftClass->HasBaseClass(rightClass) || rightClass->HasBaseClass(leftClass))
            {
                Cm::Sym::TypeSymbol* referenceType = typeRepository.MakeReferenceType(leftPlainType, span);
                DerivedTypeOpCache& cache = derivedTypeOpCacheMap[referenceType];
                viableFunctions.insert(cache.GetCopyAssignment(typeRepository, referenceType));
            }
        }
    }
    else if (leftType->IsRvalueRefType())
    {
        Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
        Cm::Sym::TypeSymbol* leftPlainType = typeRepository.MakePlainTypeWithOnePointerRemoved(leftType);
        Cm::Sym::TypeSymbol* rightPlainType = typeRepository.MakePlainType(rightType);
        if (Cm::Sym::TypesEqual(leftPlainType, rightPlainType)) // rvalue reference type copy assignment
        {
            Cm::Sym::TypeSymbol* rvalueRefType = typeRepository.MakeRvalueRefType(leftPlainType, span);
            DerivedTypeOpCache& cache = derivedTypeOpCacheMap[rvalueRefType];
            viableFunctions.insert(cache.GetCopyAssignment(typeRepository, rvalueRefType));
        }
    }
    else if (leftType->IsPointerType() && leftType->GetPointerCount() > 1) // pointer assignment
    {
        Cm::Sym::TypeSymbol* pointerType = typeRepository.MakePlainTypeWithOnePointerRemoved(leftType);
        Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
        Cm::Sym::TypeSymbol* rightPlainType = typeRepository.MakePlainType(rightType);
        if (rightPlainType->IsNullPtrType()) // pointer from nullptr type assignment
        {
            DerivedTypeOpCache& cache = derivedTypeOpCacheMap[pointerType];
            viableFunctions.insert(cache.GetCopyAssignment(typeRepository, pointerType));
            return;
        }
        if (rightType->IsPointerType())
        {
            if (Cm::Sym::TypesEqual(pointerType, rightPlainType)) // pointer copy assignment
            {
                DerivedTypeOpCache& cache = derivedTypeOpCacheMap[pointerType];
                viableFunctions.insert(cache.GetCopyAssignment(typeRepository, pointerType));
            }
            else if (pointerType->GetBaseType()->IsClassTypeSymbol() && rightPlainType->GetBaseType()->IsClassTypeSymbol())
            {
                Cm::Sym::ClassTypeSymbol* leftClass = static_cast<Cm::Sym::ClassTypeSymbol*>(pointerType->GetBaseType());
                Cm::Sym::ClassTypeSymbol* rightClass = static_cast<Cm::Sym::ClassTypeSymbol*>(rightPlainType->GetBaseType());
                if (leftClass->HasBaseClass(rightClass) || rightClass->HasBaseClass(leftClass))
                {
                    DerivedTypeOpCache& cache = derivedTypeOpCacheMap[pointerType];
                    viableFunctions.insert(cache.GetCopyAssignment(typeRepository, pointerType));
                }
            }
        }
        else
        {
            Cm::Sym::TypeSymbol* alternateRightType = typeRepository.MakeConstReferenceType(pointerType, span);
            if (Cm::Sym::TypesEqual(alternateRightType, rightType)) // pointer copy assignment
            {
                DerivedTypeOpCache& cache = derivedTypeOpCacheMap[pointerType];
                viableFunctions.insert(cache.GetCopyAssignment(typeRepository, pointerType));
            }
            else
            {
                Cm::Sym::TypeSymbol* rvalueRefRightType = typeRepository.MakeRvalueRefType(pointerType, span);
                if (Cm::Sym::TypesEqual(rvalueRefRightType, rightType)) // pointer move assignment
                {
                    DerivedTypeOpCache& cache = derivedTypeOpCacheMap[pointerType];
                    viableFunctions.insert(cache.GetMoveAssignment(typeRepository, pointerType));
                }
            }
        }
    }
}

void EqualityOpGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
    Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 2) return;
    Cm::Sym::TypeSymbol* leftType = arguments[0].Type();
    Cm::Sym::TypeSymbol* leftPlainType = typeRepository.MakePlainType(leftType);
    if (leftType->IsPointerType() || leftType->IsNullPtrType())
    {
        Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
        Cm::Sym::TypeSymbol* rightPlainType = typeRepository.MakePlainType(rightType);
        if (rightType->IsPointerType() || rightType->IsNullPtrType())
        {
            if (Cm::Sym::TypesEqual(leftPlainType, rightPlainType)) // operator==(ptr, ptr)
            {
                DerivedTypeOpCache& cache = derivedTypeOpCacheMap[leftPlainType];
                viableFunctions.insert(cache.GetOpEqual(typeRepository, leftPlainType));
            }
            else if (leftType->IsNullPtrType()) // operator==(null, ptr)
            {
                DerivedTypeOpCache& cache = derivedTypeOpCacheMap[rightType];
                viableFunctions.insert(cache.GetOpEqual(typeRepository, rightType));
            }
            else if (rightType->IsNullPtrType())    // operator==(ptr, null)
            {
                DerivedTypeOpCache& cache = derivedTypeOpCacheMap[leftType];
                viableFunctions.insert(cache.GetOpEqual(typeRepository, leftType));
            }
            else if (leftType->GetBaseType()->IsClassTypeSymbol() && rightType->GetBaseType()->IsClassTypeSymbol())
            {
                Cm::Sym::ClassTypeSymbol* leftClass = static_cast<Cm::Sym::ClassTypeSymbol*>(leftType->GetBaseType());
                Cm::Sym::ClassTypeSymbol* rightClass = static_cast<Cm::Sym::ClassTypeSymbol*>(rightType->GetBaseType());
                if (leftClass->HasBaseClass(rightClass))
                {
                    DerivedTypeOpCache& cache = derivedTypeOpCacheMap[rightPlainType];
                    viableFunctions.insert(cache.GetOpEqual(typeRepository, rightPlainType));
                }
                else if (rightClass->HasBaseClass(leftClass))
                {
                    DerivedTypeOpCache& cache = derivedTypeOpCacheMap[leftPlainType];
                    viableFunctions.insert(cache.GetOpEqual(typeRepository, leftPlainType));
                }
            }
        }
    }
}

void LessOpGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
    Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 2) return;
    Cm::Sym::TypeSymbol* leftType = arguments[0].Type();
    if (leftType->IsPointerType())
    {
        Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
        if (rightType->IsPointerType())
        {
            Cm::Sym::TypeSymbol* leftPlainType = typeRepository.MakePlainType(leftType);
            Cm::Sym::TypeSymbol* rightPlainType = typeRepository.MakePlainType(rightType);
            if (Cm::Sym::TypesEqual(leftPlainType, rightPlainType)) // operator<(ptr, ptr)
            {
                DerivedTypeOpCache& cache = derivedTypeOpCacheMap[leftPlainType];
                viableFunctions.insert(cache.GetOpLess(typeRepository, leftPlainType));
            }
            else if (leftType->GetBaseType()->IsClassTypeSymbol() && rightType->GetBaseType()->IsClassTypeSymbol())
            {
                Cm::Sym::ClassTypeSymbol* leftClass = static_cast<Cm::Sym::ClassTypeSymbol*>(leftType->GetBaseType());
                Cm::Sym::ClassTypeSymbol* rightClass = static_cast<Cm::Sym::ClassTypeSymbol*>(rightType->GetBaseType());
                if (leftClass->HasBaseClass(rightClass) || rightClass->HasBaseClass(leftClass))
                {
                    DerivedTypeOpCache& cache = derivedTypeOpCacheMap[leftPlainType];
                    viableFunctions.insert(cache.GetOpLess(typeRepository, leftPlainType));
                }
            }
        }
    }
}

void AdditiveOpGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
    Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 2) return;
    Cm::Sym::TypeSymbol* leftType = arguments[0].Type();
    if (leftType->IsPointerType())
    {
        if (leftType->GetBaseType()->IsVoidTypeSymbol()) return;
        Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
        Cm::Sym::TypeSymbol* rightPlainType = typeRepository.MakePlainType(rightType);
        if (rightPlainType->IsIntegerTypeSymbol())
        {
            DerivedTypeOpCache& cache = derivedTypeOpCacheMap[leftType];
            viableFunctions.insert(cache.GetOpAddPtrInt(typeRepository, leftType));
        }
    }
    else
    {
        Cm::Sym::TypeSymbol* leftPlainType = typeRepository.MakePlainType(leftType);
        if (leftPlainType->IsIntegerTypeSymbol())
        {
            Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
            if (rightType->IsPointerType())
            {
                if (rightType->GetBaseType()->IsVoidTypeSymbol()) return;
                DerivedTypeOpCache& cache = derivedTypeOpCacheMap[rightType];
                viableFunctions.insert(cache.GetOpAddIntPtr(typeRepository, rightType));
            }
        }
    }
}

void SubtractiveOpGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
    Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 2) return;
    Cm::Sym::TypeSymbol* leftType = arguments[0].Type();
    if (leftType->IsPointerType())
    {
        if (leftType->GetBaseType()->IsVoidTypeSymbol()) return;
        Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
        Cm::Sym::TypeSymbol* rightPlainType = typeRepository.MakePlainType(rightType);
        if (rightPlainType->IsIntegerTypeSymbol())
        {
            DerivedTypeOpCache& cache = derivedTypeOpCacheMap[leftType];
            viableFunctions.insert(cache.GetOpSubPtrInt(typeRepository, leftType));
        }
        else
        {
            if (rightType->IsPointerType())
            {
                if (Cm::Sym::TypesEqual(leftType, rightType))
                {
                    DerivedTypeOpCache& cache = derivedTypeOpCacheMap[leftType];
                    viableFunctions.insert(cache.GetOpSubPtrPtr(typeRepository, leftType));
                }
            }
        }
    }
}

void DerefOpGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
    Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 1) return;
    Cm::Sym::TypeSymbol* operandType = arguments[0].Type();
    if (operandType->IsPointerType())
    {
        if (operandType->GetBaseType()->IsVoidTypeSymbol()) return;
        DerivedTypeOpCache& cache = derivedTypeOpCacheMap[operandType];
        viableFunctions.insert(cache.GetOpDeref(typeRepository, operandType));
    }
}

void IncrementOpGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
    Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 1) return;
    Cm::Sym::TypeSymbol* operandType = arguments[0].Type();
    if (operandType->IsPointerType())
    {
        if (operandType->GetBaseType()->IsVoidTypeSymbol()) return;
        DerivedTypeOpCache& cache = derivedTypeOpCacheMap[operandType];
        viableFunctions.insert(cache.GetOpIncPtr(typeRepository, operandType));
    }
}

void DecrementOpGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
    Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 1) return;
    Cm::Sym::TypeSymbol* operandType = arguments[0].Type();
    if (operandType->IsPointerType())
    {
        if (operandType->GetBaseType()->IsVoidTypeSymbol()) return;
        DerivedTypeOpCache& cache = derivedTypeOpCacheMap[operandType];
        viableFunctions.insert(cache.GetOpDecPtr(typeRepository, operandType));
    }
}

void AddressOfOpGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
    Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 1) return;
    Cm::Sym::TypeSymbol* plainOperandType = typeRepository.MakePlainType(arguments[0].Type());
    if (plainOperandType->IsVoidTypeSymbol()) return;
    Cm::Sym::TypeSymbol* pointerType = typeRepository.MakePointerType(plainOperandType, span);
    DerivedTypeOpCache& cache = derivedTypeOpCacheMap[pointerType];
    viableFunctions.insert(cache.GetOpAddrOf(typeRepository, pointerType));
}

void ArrowOpGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
    Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 1) return;
    Cm::Sym::TypeSymbol* operandType = arguments[0].Type();
    if (operandType->IsPointerType())
    {
        if (operandType->GetBaseType()->IsVoidTypeSymbol()) return;
        DerivedTypeOpCache& cache = derivedTypeOpCacheMap[operandType];
        viableFunctions.insert(cache.GetOpArrow(typeRepository, operandType));
    }
}

DerivedTypeOpRepository::DerivedTypeOpRepository(Cm::Sym::TypeRepository& typeRepository_) : typeRepository(typeRepository_)
{
    derivedTypeOpGroupMap["@constructor"] = &constructorOpGroup;
    derivedTypeOpGroupMap["operator="] = &assignmentOpGroup;
    derivedTypeOpGroupMap["operator=="] = &equalityOpGroup;
    derivedTypeOpGroupMap["operator<"] = &lessOpGroup;
    derivedTypeOpGroupMap["operator+"] = &additiveOpGroup;
    derivedTypeOpGroupMap["operator-"] = &subtractiveOpGroup;
    derivedTypeOpGroupMap["operator*"] = &derefOpGroup;
    derivedTypeOpGroupMap["operator++"] = &incrementOpGroup;
    derivedTypeOpGroupMap["operator--"] = &decrementOpGroup;
    derivedTypeOpGroupMap["operator&"] = &addressOfOpGroup;
    derivedTypeOpGroupMap["operator->"] = &arrowOpGroup;
}

void DerivedTypeOpRepository::CollectViableFunctions(const std::string& groupName, int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
    const Cm::Parsing::Span& span, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (int(arguments.size()) != arity)
    {
        throw std::runtime_error("wrong number of arguments");
    }
    if (arity < 1 || arity > 2) return;
    DerivedTypeOpGroupMapIt i = derivedTypeOpGroupMap.find(groupName);
    if (i != derivedTypeOpGroupMap.end())
    {
        DerivedTypeOpGroup* group = i->second;
        group->CollectViableFunctions(arity, arguments, conversionTable, span, typeRepository, derivedTypeOpCacheMap, viableFunctions);
    }
}

void DerivedTypeOpRepository::InsertPointerConversionsToConversionTable(Cm::Sym::ConversionTable& conversionTable, Cm::Sym::TypeSymbol* pointerType, const Cm::Parsing::Span& span)
{
    DerivedTypeOpCache& cache = derivedTypeOpCacheMap[pointerType];
    Cm::Sym::FunctionSymbol* ptrToVoidPtrConversion = cache.GetPtrToVoidPtrConversion(typeRepository, pointerType, conversionTable, span);
    Cm::Sym::FunctionSymbol* nullPtrToPtrConversion = cache.GetNullPtrToPtrConversion(typeRepository, pointerType, conversionTable);
}

} } // namespace Cm::Core
