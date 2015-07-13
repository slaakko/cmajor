/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef C_IR_INSTRUCTION_INCLUDED
#define C_IR_INSTRUCTION_INCLUDED
#include <Ir.Intf/Function.hpp>
#include <Ir.Intf/Instruction.hpp>
#include <C.Ir/Label.hpp>
#include <C.Ir/Type.hpp>
#include <string>
#include <set>

namespace C {

class BinOpInstruction : public Ir::Intf::Instruction
{
public:
    BinOpInstruction(Ir::Intf::Type* type_, const std::string& name_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
    std::string ToString() const override;
    Ir::Intf::Type* GetType() const { return type; }
    bool ReturnsResult() const override { return true; }
    Ir::Intf::Object* GetResult() const override { return result; }
    Ir::Intf::Object* GetOperand1() const { return operand1; }
    Ir::Intf::Object* GetOperand2() const { return operand2; }
private:
    Ir::Intf::Type* type;
    Ir::Intf::Object* result;
    Ir::Intf::Object* operand1;
    Ir::Intf::Object* operand2;
};

class IntegerBinOpInstruction : public BinOpInstruction
{
public:
    IntegerBinOpInstruction(Ir::Intf::Type* type_, const std::string& name_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
    bool Valid() const override;
};

class AddInst : public IntegerBinOpInstruction
{
public:
    AddInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* Add(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class SubInst : public IntegerBinOpInstruction
{
public:
    SubInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* Sub(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class MulInst : public IntegerBinOpInstruction
{
public:
    MulInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* Mul(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class UDivInst : public IntegerBinOpInstruction
{
public:
    UDivInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* UDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class SDivInst : public IntegerBinOpInstruction
{
public:
    SDivInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* SDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class URemInst : public IntegerBinOpInstruction
{
public:
    URemInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* URem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class SRemInst : public IntegerBinOpInstruction
{
public:
    SRemInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* SRem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class ShlInst : public IntegerBinOpInstruction
{
public:
    ShlInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* Shl(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class LShrInst : public IntegerBinOpInstruction
{
public:
    LShrInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* LShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class AShrInst : public IntegerBinOpInstruction
{
public:
    AShrInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* AShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class AndInst : public IntegerBinOpInstruction
{
public:
    AndInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* And(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class OrInst : public IntegerBinOpInstruction
{
public:
    OrInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* Or(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class XorInst : public IntegerBinOpInstruction
{
public:
    XorInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* Xor(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class FloatingPointBinOpInstruction : public BinOpInstruction
{
public:
    FloatingPointBinOpInstruction(Ir::Intf::Type* type_, const std::string& name_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
    bool Valid() const override;
};

class FAddInst : public FloatingPointBinOpInstruction
{
public:
    FAddInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* FAdd(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class FSubInst : public FloatingPointBinOpInstruction
{
public:
    FSubInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* FSub(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class FMulInst : public FloatingPointBinOpInstruction
{
public:
    FMulInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* FMul(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class FDivInst : public FloatingPointBinOpInstruction
{
public:
    FDivInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* FDiv(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class FRemInst : public FloatingPointBinOpInstruction
{
public:
    FRemInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* FRem(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class IndexInst : public Ir::Intf::Instruction
{
public:
    IndexInst(Ir::Intf::Object* result_, Ir::Intf::Object* array__, Ir::Intf::Object* index_);
    std::string ToString() const override;
    bool ReturnsResult() const override { return true; }
    Ir::Intf::Object* GetResult() const override { return result; }
private:
    Ir::Intf::Object* result;
    Ir::Intf::Object* array_;
    Ir::Intf::Object* index;
};

Ir::Intf::Instruction* Index(Ir::Intf::Object* result, Ir::Intf::Object* array_, Ir::Intf::Object* index);

class RetInst : public Ir::Intf::Instruction
{
public:
    RetInst();
    RetInst(Ir::Intf::Object* value_);
    std::string ToString() const override;
    bool IsTerminator() const override { return true; }
    bool IsRet() const override { return true; }
private:
    Ir::Intf::Object* value;
};

Ir::Intf::Instruction* Ret();
Ir::Intf::Instruction* Ret(Ir::Intf::Object* value);

class BrInst : public Ir::Intf::Instruction
{
public:
    BrInst(Ir::Intf::LabelObject* dest_);
    BrInst(Ir::Intf::Object* cond_, Ir::Intf::LabelObject* trueLabel_, Ir::Intf::LabelObject* falseLabel_);
    std::string ToString() const override;
    void AddTargetLabels(std::set<std::string>& targetLabels) override;
    bool IsTerminator() const override { return true; }
    Ir::Intf::LabelObject* GetTargetLabel() const override { return dest; }
    bool IsUnconditionalBr() const override;
private:
    Ir::Intf::LabelObject* dest;
    Ir::Intf::Object* cond;
    Ir::Intf::LabelObject* trueLabel;
    Ir::Intf::LabelObject* falseLabel;
};

Ir::Intf::Instruction* Br(Ir::Intf::LabelObject* dest);
Ir::Intf::Instruction* Br(Ir::Intf::Object* cond, Ir::Intf::LabelObject* trueLabel, Ir::Intf::LabelObject* falseLabel);

class SwitchInst : public Ir::Intf::Instruction
{
public:
    SwitchInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* value_, Ir::Intf::LabelObject* defaultDest_, const std::vector<std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>>& destinations_);
    std::string ToString() const override;
    void AddTargetLabels(std::set<std::string>& targetLabels) override;
    Ir::Intf::Object* GetResult() const override { return value; }
    bool IsTerminator() const override { return true; }
private:
    Ir::Intf::Type* integerType;
    Ir::Intf::Object* value;
    Ir::Intf::LabelObject* defaultDest;
    std::vector<std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>> destinations;
};

Ir::Intf::Instruction* Switch(Ir::Intf::Type* integerType, Ir::Intf::Object* value, Ir::Intf::LabelObject* defaultDest, const std::vector<std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>>& destinations);

class AllocaInst : public Ir::Intf::Instruction
{
public:
    AllocaInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_);
    AllocaInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Type* numElementsType_, int numElements_);
    void SetAlignment(int alignment_) override;
    std::string ToString() const override;
    Ir::Intf::Object* GetResult() const override { return result; }
private:
    Ir::Intf::Type* type;
    Ir::Intf::Object* result;
    Ir::Intf::Type* numElementsType;
    int numElements;
    int alignment;
};

Ir::Intf::Instruction* Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result);
Ir::Intf::Instruction* Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Type* numElementsType, int numElements);

class LoadInst : public Ir::Intf::Instruction
{
public:
    LoadInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* ptr_, Ir::Intf::Indirection leftIndirection_, Ir::Intf::Indirection rightIndirection_);
    void SetAlignment(int alignment_) override;
    std::string ToString() const override;
    bool ReturnsResult() const override { return true; }
    Ir::Intf::Object* GetResult() const override { return result; }
private:
    Ir::Intf::Type* type;
    Ir::Intf::Object* result;
    Ir::Intf::Object* ptr;
    int alignment;
    Ir::Intf::Indirection leftIndirection;
    Ir::Intf::Indirection rightIndirection;
};

Ir::Intf::Instruction* Load(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Indirection leftIndirection, Ir::Intf::Indirection rightIndirection);

class StoreInst : public Ir::Intf::Instruction
{
public:
    StoreInst(Ir::Intf::Type* type_, Ir::Intf::Object* value_, Ir::Intf::Object* ptr_, Ir::Intf::Indirection leftIndirection_, Ir::Intf::Indirection rightIndirection_);
    void SetAlignment(int alignment_) override;
    std::string ToString() const override;
private:
    Ir::Intf::Type* type;
    Ir::Intf::Object* value;
    Ir::Intf::Object* ptr;
    int alignment;
    Ir::Intf::Indirection leftIndirection;
    Ir::Intf::Indirection rightIndirection;
};

Ir::Intf::Instruction* Store(Ir::Intf::Type* type, Ir::Intf::Object* value, Ir::Intf::Object* ptr, Ir::Intf::Indirection leftIndirection, Ir::Intf::Indirection rightIndirection);

class SetInst : public Ir::Intf::Instruction
{
public:
    SetInst(Ir::Intf::Object* from_, Ir::Intf::Object* to_);
    bool ReturnsResult() const override { return true; }
    Ir::Intf::Object* GetResult() const override { return to; }
    std::string ToString() const override;
private:
    Ir::Intf::Object* from;
    Ir::Intf::Object* to;
};

Ir::Intf::Instruction* Set(Ir::Intf::Object* from, Ir::Intf::Object* to);

class CallInst : public Ir::Intf::Instruction
{
public:
    CallInst(Ir::Intf::Object* result_, Ir::Intf::Function* fun_, const std::vector<Ir::Intf::Object*>& args_);
    std::string ToString() const override;
    bool ReturnsResult() const override { return result && !result->GetType()->IsVoidType(); }
    Ir::Intf::Object* GetResult() const override { return result; }
    bool IsCall() const override { return true; }
private:
    Ir::Intf::Object* result;
    Ir::Intf::Function* fun;
    std::vector<Ir::Intf::Object*> args;
};

Ir::Intf::Instruction* Call(Ir::Intf::Object* result, Ir::Intf::Function* fun, const std::vector<Ir::Intf::Object*>& args);

class IndirectCallInst : public Ir::Intf::Instruction
{
public:
    IndirectCallInst(Ir::Intf::Object* result_, Ir::Intf::Object* funPtr_, const std::vector<Ir::Intf::Object*>& args_);
    std::string ToString() const override;
    bool ReturnsResult() const override { return result && !result->GetType()->IsVoidType(); }
    Ir::Intf::Object* GetResult() const override { return result; }
    bool IsCall() const override { return true; }
private:
    Ir::Intf::Object* result;
    Ir::Intf::Object* funPtr;
    std::vector<Ir::Intf::Object*> args;
};

Ir::Intf::Instruction* IndirectCall(Ir::Intf::Object* result, Ir::Intf::Object* funPtr, const std::vector<Ir::Intf::Object*>& args);

class ICmpInst : public Ir::Intf::Instruction
{
public:
    ICmpInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::IConditionCode cond_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
    std::string ToString() const override;
    bool ReturnsResult() const override { return true; }
    Ir::Intf::Object* GetResult() const override { return result; }
private:
    Ir::Intf::Type* type;
    Ir::Intf::Object* result;
    Ir::Intf::IConditionCode cond;
    Ir::Intf::Object* operand1;
    Ir::Intf::Object* operand2;
};

Ir::Intf::Instruction* ICmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::IConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class FCmpInst : public Ir::Intf::Instruction
{
public:
    FCmpInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::FConditionCode cond_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
    std::string ToString() const override;
    bool ReturnsResult() const override { return true; }
    Ir::Intf::Object* GetResult() const override { return result; }
private:
    Ir::Intf::Type* type;
    Ir::Intf::Object* result;
    Ir::Intf::FConditionCode cond;
    Ir::Intf::Object* operand1;
    Ir::Intf::Object* operand2;
};

Ir::Intf::Instruction* FCmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::FConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class DoNothingInst : public Ir::Intf::Instruction
{
public:
    DoNothingInst();
    std::string ToString() const override;
};

Ir::Intf::Instruction* DoNothing();

class ConversionInstruction : public Ir::Intf::Instruction
{
public:
    ConversionInstruction(const std::string& name_, Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
    std::string ToString() const override;
    bool ReturnsResult() const override { return true; }
    Ir::Intf::Object* GetResult() const override { return result; }
private:
    Ir::Intf::Type* type;
    Ir::Intf::Object* result;
    Ir::Intf::Object* value;
    Ir::Intf::Type* toType;
};

class TruncInst : public ConversionInstruction
{
public:
    TruncInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Trunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class ZextInst : public ConversionInstruction
{
public:
    ZextInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Zext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class SextInst : public ConversionInstruction
{
public:
    SextInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Sext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class FptruncInst : public ConversionInstruction
{
public:
    FptruncInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Fptrunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class FpextInst : public ConversionInstruction
{
public:
    FpextInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Fpext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class FptouiInst : public ConversionInstruction
{
public:
    FptouiInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Fptoui(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class FptosiInst : public ConversionInstruction
{
public:
    FptosiInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Fptosi(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class UitofpInst : public ConversionInstruction
{
public:
    UitofpInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Uitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class SitofpInst : public ConversionInstruction
{
public:
    SitofpInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Sitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class PtrtointInst : public ConversionInstruction
{
public:
    PtrtointInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Ptrtoint(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class InttoptrInst : public ConversionInstruction
{
public:
    InttoptrInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Inttoptr(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class BitcastInst : public ConversionInstruction
{
public:
    BitcastInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Bitcast(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class SizeOfInst : public Ir::Intf::Instruction
{
public:
    SizeOfInst(Ir::Intf::Object* result_, Ir::Intf::Type* type_);
    bool ReturnsResult() const override { return true; }
    Ir::Intf::Object* GetResult() const override { return result; }
    std::string ToString() const override;
private:
    Ir::Intf::Object* result;
    Ir::Intf::Type* type;
};

Ir::Intf::Instruction* CreateSizeOf(Ir::Intf::Object* result, Ir::Intf::Type* type);

class MemSetInst : public Ir::Intf::Instruction
{
public:
    MemSetInst(Ir::Intf::Object* dest_, Ir::Intf::Object* value_, Ir::Intf::Object* len_, int align_, bool isVolatile_);
    std::string ToString() const override;
private:
    Ir::Intf::Object* dest;
    Ir::Intf::Object* value;
    Ir::Intf::Object* len;
    int align;
    bool isVolatile;
};

Ir::Intf::Instruction* MemSet(Ir::Intf::Object* dest, Ir::Intf::Object* value, Ir::Intf::Object* len, int align, bool isVolatile);

class MemCopyInst : public Ir::Intf::Instruction
{
public:
    MemCopyInst(Ir::Intf::Object* dest_, Ir::Intf::Object* source_, Ir::Intf::Object* len_, int align_, bool isVolatile_);
    std::string ToString() const override;
private:
    Ir::Intf::Object* dest;
    Ir::Intf::Object* source;
    Ir::Intf::Object* len;
    int align;
    bool isVolatile;
};

Ir::Intf::Instruction* MemCopy(Ir::Intf::Object* dest, Ir::Intf::Object* value, Ir::Intf::Object* len, int align, bool isVolatile);

} // namespace C

#endif // C_IR_INSTRUCTION_INCLUDED
