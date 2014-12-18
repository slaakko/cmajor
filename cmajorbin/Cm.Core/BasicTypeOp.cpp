/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/TypeRepository.hpp>

namespace Cm { namespace Core {

using Cm::Parsing::Span;

DefaultCtor::DefaultCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*default_ctor*"), type(type_)
{
    SetGroupName("@constructor");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(type, Span(), true));
    AddSymbol(thisParam);
    ComputeName();
}

CopyCtor::CopyCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*copy_ctor*"), type(type_)
{
    SetGroupName("@constructor");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(type, Span(), true));
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* thatParam(new Cm::Sym::ParameterSymbol(Span(), "that"));
    thatParam->SetType(type);
    AddSymbol(thatParam);
    ComputeName();
}

CopyAssignment::CopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*copy_assignment*"), type(type_)
{
    SetGroupName("operator=");
    Cm::Sym::TypeSymbol* voidType = typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId));
    SetReturnType(voidType);
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(type, Span(), true));
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* thatParam(new Cm::Sym::ParameterSymbol(Span(), "that"));
    thatParam->SetType(type);
    AddSymbol(thatParam);
    ComputeName();
}

MoveCtor::MoveCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*move_ctor*"), type(type_)
{
    SetGroupName("@constructor");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(type, Span(), true));
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* thatParam(new Cm::Sym::ParameterSymbol(Span(), "that"));
    thatParam->SetType(typeRepository.MakeRvalueRefType(type, Span(), true));
    AddSymbol(thatParam);
    ComputeName();
}

MoveAssignment::MoveAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*move_assignment*"), type(type_)
{
    SetGroupName("operator=");
    Cm::Sym::TypeSymbol* voidType = typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId));
    SetReturnType(voidType);
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(type, Span(), true));
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* thatParam(new Cm::Sym::ParameterSymbol(Span(), "that"));
    thatParam->SetType(typeRepository.MakeRvalueRefType(type, Span(), true));
    AddSymbol(thatParam);
    ComputeName();
}

OpEqual::OpEqual(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_equal*"), type(type_)
{
    SetGroupName("operator==");
    Cm::Sym::TypeSymbol* boolType = typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
    SetReturnType(boolType);
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(type);
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(type);
    AddSymbol(rightParam);
    ComputeName();
}

OpLess::OpLess(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_less*"), type(type_)
{
    SetGroupName("operator<");
    Cm::Sym::TypeSymbol* boolType = typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
    SetReturnType(boolType);
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(type);
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(type);
    AddSymbol(rightParam);
    ComputeName();
}

OpAdd::OpAdd(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_add*"), type(type_)
{
    SetGroupName("operator+");
    SetReturnType(type);
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(type);
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(type);
    AddSymbol(rightParam);
    ComputeName();
}

OpSub::OpSub(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_sub*"), type(type_)
{
    SetGroupName("operator-");
    SetReturnType(type);
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(type);
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(type);
    AddSymbol(rightParam);
    ComputeName();
}

OpMul::OpMul(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_mul*"), type(type_)
{
    SetGroupName("operator*");
    SetReturnType(type);
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(type);
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(type);
    AddSymbol(rightParam);
    ComputeName();
}

OpDiv::OpDiv(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_mul*"), type(type_)
{
    SetGroupName("operator/");
    SetReturnType(type);
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(type);
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(type);
    AddSymbol(rightParam);
    ComputeName();
}

OpRem::OpRem(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_rem*"), type(type_)
{
    SetGroupName("operator%");
    SetReturnType(type);
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(type);
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(type);
    AddSymbol(rightParam);
    ComputeName();
}

OpShl::OpShl(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_shl*"), type(type_)
{
    SetGroupName("operator<<");
    SetReturnType(type);
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(type);
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(type);
    AddSymbol(rightParam);
    ComputeName();
}

OpShr::OpShr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_shr*"), type(type_)
{
    SetGroupName("operator>>");
    SetReturnType(type);
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(type);
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(type);
    AddSymbol(rightParam);
    ComputeName();
}

OpBitAnd::OpBitAnd(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_bit_and*"), type(type_)
{
    SetGroupName("operator&");
    SetReturnType(type);
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(type);
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(type);
    AddSymbol(rightParam);
    ComputeName();
}

OpBitOr::OpBitOr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_bit_or*"), type(type_)
{
    SetGroupName("operator|");
    SetReturnType(type);
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(type);
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(type);
    AddSymbol(rightParam);
    ComputeName();
}

OpBitXor::OpBitXor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_bit_xor*"), type(type_)
{
    SetGroupName("operator^");
    SetReturnType(type);
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(type);
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(type);
    AddSymbol(rightParam);
    ComputeName();
}

OpNot::OpNot(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_not*"), type(type_)
{
    SetGroupName("operator!");
    Cm::Sym::TypeSymbol* boolType = typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
    SetReturnType(boolType);
    Cm::Sym::ParameterSymbol* operandParam(new Cm::Sym::ParameterSymbol(Span(), "operand"));
    operandParam->SetType(type);
    AddSymbol(operandParam);
    ComputeName();
}

OpUnaryPlus::OpUnaryPlus(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_unary_plus*"), type(type_)
{
    SetGroupName("operator+");
    SetReturnType(type);
    Cm::Sym::ParameterSymbol* operandParam(new Cm::Sym::ParameterSymbol(Span(), "operand"));
    operandParam->SetType(type);
    AddSymbol(operandParam);
    ComputeName();
}

OpUnaryMinus::OpUnaryMinus(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_unary_minus*"), type(type_)
{
    SetGroupName("operator-");
    SetReturnType(type);
    Cm::Sym::ParameterSymbol* operandParam(new Cm::Sym::ParameterSymbol(Span(), "operand"));
    operandParam->SetType(type);
    AddSymbol(operandParam);
    ComputeName();
}

OpComplement::OpComplement(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_complement*"), type(type_)
{
    SetGroupName("operator~");
    SetReturnType(type);
    Cm::Sym::ParameterSymbol* operandParam(new Cm::Sym::ParameterSymbol(Span(), "operand"));
    operandParam->SetType(type);
    AddSymbol(operandParam);
    ComputeName();
}

OpIncrement::OpIncrement(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_increment*"), type(type_)
{
    SetGroupName("operator++");
    SetReturnType(type);
    Cm::Sym::ParameterSymbol* operandParam(new Cm::Sym::ParameterSymbol(Span(), "operand"));
    operandParam->SetType(type);
    AddSymbol(operandParam);
    ComputeName();
}

OpDecrement::OpDecrement(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : Cm::Sym::FunctionSymbol(Span(), "*op_decrement*"), type(type_)
{
    SetGroupName("operator--");
    SetReturnType(type);
    Cm::Sym::ParameterSymbol* operandParam(new Cm::Sym::ParameterSymbol(Span(), "operand"));
    operandParam->SetType(type);
    AddSymbol(operandParam);
    ComputeName();
}

ConvertingCtor::ConvertingCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* targetType_, Cm::Sym::TypeSymbol* sourceType_, ConversionType conversionType_, ConversionInst conversionInst_,
    ConversionRank conversionRank_, int conversionDistance_) :
    Cm::Sym::FunctionSymbol(Span(), "*converting_ctor*"), targetType(targetType_), sourceType(sourceType_), conversionType(conversionType_), conversionInst(conversionInst_), conversionRank(conversionRank_),
    conversionDistance(conversionDistance_)
{
    SetGroupName("@constructor");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(targetType, Span(), true));
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* thatParam(new Cm::Sym::ParameterSymbol(Span(), "that"));
    thatParam->SetType(sourceType);
    AddSymbol(thatParam);
    ComputeName();
}

} } // namespace Cm::Core
