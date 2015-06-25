/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_BASIC_TYPE_OP_INCLUDED
#define CM_CORE_BASIC_TYPE_OP_INCLUDED
#include <Cm.Core/GenData.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Ast/Statement.hpp>

namespace Cm { namespace Core {

class BasicTypeOp : public Cm::Sym::FunctionSymbol
{
public:
    BasicTypeOp(Cm::Sym::TypeSymbol* type_);
    bool IsBasicTypeOp() const override { return true; }
    Cm::Sym::TypeSymbol* Type() const { return type; }
    virtual void Generate(Emitter& emitter, GenResult& result) = 0;
    Ir::Intf::Type* GetIrType() const { return type->GetIrType(); }
    Ir::Intf::Object* GetDefaultIrValue() const { return type->GetDefaultIrValue(); }
private:
    Cm::Sym::TypeSymbol* type;
};

class DefaultCtor : public BasicTypeOp
{
public:
    DefaultCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Emitter& emitter, GenResult& result) override;
};

class CopyCtor : public BasicTypeOp
{
public:
    CopyCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Emitter& emitter, GenResult& result) override;
    bool IsBasicTypeCopyCtor() const override { return true; }
};

class CopyAssignment : public BasicTypeOp
{
public:
    CopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Emitter& emitter, GenResult& result) override;
};

class MoveCtor : public BasicTypeOp
{
public:
    MoveCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Emitter& emitter, GenResult& result) override;
};

class MoveAssignment : public BasicTypeOp
{
public:
    MoveAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Emitter& emitter, GenResult& result) override;
};

class OpEqual : public BasicTypeOp
{
public:
    OpEqual(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Emitter& emitter, GenResult& result) override;
};

class OpLess : public BasicTypeOp
{
public:
    OpLess(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Emitter& emitter, GenResult& result) override;
};

class BinOp : public BasicTypeOp
{
public:
    BinOp(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_, const std::string& groupName_);
    void Generate(Emitter& emitter, GenResult& result) override;
    virtual Ir::Intf::Instruction* CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const = 0;
};

class OpAdd : public BinOp
{
public:
    OpAdd(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    Ir::Intf::Instruction* CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const override;
};

class OpSub : public BinOp
{
public:
    OpSub(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    Ir::Intf::Instruction* CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const override;
};

class OpMul : public BinOp
{
public:
    OpMul(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    Ir::Intf::Instruction* CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const override;
};

class OpDiv : public BinOp
{
public:
    OpDiv(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    Ir::Intf::Instruction* CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const override;
};

class OpRem : public BinOp
{
public:
    OpRem(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    Ir::Intf::Instruction* CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const override;
};

class OpShl : public BinOp
{
public:
    OpShl(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    Ir::Intf::Instruction* CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const override;
};

class OpShr : public BinOp
{
public:
    OpShr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    Ir::Intf::Instruction* CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const override;
};

class OpBitAnd : public BinOp
{
public:
    OpBitAnd(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    Ir::Intf::Instruction* CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const override;
};

class OpBitOr : public BinOp
{
public:
    OpBitOr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    Ir::Intf::Instruction* CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const override;
};

class OpBitXor : public BinOp
{
public:
    OpBitXor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    Ir::Intf::Instruction* CreateInstruction(Ir::Intf::Type* irType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) const override;
};

class OpNot : public BasicTypeOp
{
public:
    OpNot(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Emitter& emitter, GenResult& result) override;
};

class OpUnaryPlus : public BasicTypeOp
{
public:
    OpUnaryPlus(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Emitter& emitter, GenResult& result) override;
};

class OpUnaryMinus : public BasicTypeOp
{
public:
    OpUnaryMinus(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Emitter& emitter, GenResult& result) override;
};

class OpComplement : public BasicTypeOp
{
public:
    OpComplement(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Emitter& emitter, GenResult& result) override;
};

class OpIncrement : public BasicTypeOp
{
public:
    OpIncrement(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Emitter& emitter, GenResult& result) override;
};

class OpDecrement : public BasicTypeOp
{
public:
    OpDecrement(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Emitter& emitter, GenResult& result) override;
};

enum class ConversionInst
{
    none, sext, zext, trunc, bitcast, uitofp, sitofp, fptoui, fptosi, fpext, fptrunc, ptrtoint
};

class ConvertingCtor : public BasicTypeOp
{
public:
    ConvertingCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* targetType_, Cm::Sym::TypeSymbol* sourceType_, Cm::Sym::ConversionType conversionType_, ConversionInst conversionInst_, 
        Cm::Sym::ConversionRank conversionRank_, int conversionDistance_);
    Cm::Sym::ConversionType GetConversionType() const override { return conversionType; }
    bool IsConvertingConstructor() const override { return true; }
    void Generate(Emitter& emitter, GenResult& result) override;
    Cm::Sym::TypeSymbol* GetTargetType() const override { return targetType; }
    Cm::Sym::TypeSymbol* GetSourceType() const override { return sourceType; }
    Cm::Sym::ConversionRank GetConversionRank() const override { return conversionRank; }
    int GetConversionDistance() const override { return conversionDistance; }
private:
    Cm::Sym::TypeSymbol* targetType;
    Cm::Sym::TypeSymbol* sourceType;
    Cm::Sym::ConversionType conversionType;
    ConversionInst conversionInst;
    Cm::Sym::ConversionRank conversionRank;
    int conversionDistance;
};

} } // namespace Cm::Core

#endif // CM_CORE_BASIC_TYPE_OP_INCLUDED
