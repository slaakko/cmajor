/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef LLVM_IR_INSTRUCTION_INCLUDED
#define LLVM_IR_INSTRUCTION_INCLUDED
#include <Ir.Intf/Function.hpp>
#include <Ir.Intf/Instruction.hpp>
#include <Llvm.Ir/Label.hpp>
#include <Llvm.Ir/Type.hpp>
#include <string>
#include <set>

namespace Llvm { 

class BinOpInstruction: public Ir::Intf::Instruction
{
public:
    BinOpInstruction(Ir::Intf::Type* type_, const std::string& name_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
    virtual std::string ToString() const;
    Ir::Intf::Type* GetType() const { return type; }
    virtual bool ReturnsResult() const { return true; }
    virtual Ir::Intf::Object* GetResult() const { return result; }
    Ir::Intf::Object* GetOperand1() const { return operand1; }
    Ir::Intf::Object* GetOperand2() const { return operand2; }
private:
    Ir::Intf::Type* type;
    Ir::Intf::Object* result;
    Ir::Intf::Object* operand1;
    Ir::Intf::Object* operand2;
};

class IntegerBinOpInstruction: public BinOpInstruction
{
public:
    IntegerBinOpInstruction(Ir::Intf::Type* type_, const std::string& name_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
    virtual bool Valid() const;
};

class AddInst: public IntegerBinOpInstruction
{
public:
    AddInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* Add(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class SubInst: public IntegerBinOpInstruction
{
public:
    SubInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* Sub(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class MulInst: public IntegerBinOpInstruction
{
public:
    MulInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* Mul(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class UDivInst: public IntegerBinOpInstruction
{
public:
    UDivInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* UDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class SDivInst: public IntegerBinOpInstruction
{
public:
    SDivInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* SDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class URemInst: public IntegerBinOpInstruction
{
public:
    URemInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* URem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class SRemInst: public IntegerBinOpInstruction
{
public:
    SRemInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* SRem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class ShlInst: public IntegerBinOpInstruction
{
public:
    ShlInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* Shl(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class LShrInst: public IntegerBinOpInstruction
{
public:
    LShrInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* LShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class AShrInst: public IntegerBinOpInstruction
{
public:
    AShrInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* AShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class AndInst: public IntegerBinOpInstruction
{
public:
    AndInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* And(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class OrInst: public IntegerBinOpInstruction
{
public:
    OrInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* Or(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class XorInst: public IntegerBinOpInstruction
{
public:
    XorInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* Xor(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class FloatingPointBinOpInstruction: public BinOpInstruction
{
public:
    FloatingPointBinOpInstruction(Ir::Intf::Type* type_, const std::string& name_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
    virtual bool Valid() const;
};

class FAddInst: public FloatingPointBinOpInstruction
{
public:
    FAddInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* FAdd(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class FSubInst: public FloatingPointBinOpInstruction
{
public:
    FSubInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* FSub(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class FMulInst: public FloatingPointBinOpInstruction
{
public:
    FMulInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* FMul(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class FDivInst: public FloatingPointBinOpInstruction
{
public:
    FDivInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* FDiv(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class FRemInst: public FloatingPointBinOpInstruction
{
public:
    FRemInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
};

Ir::Intf::Instruction* FRem(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class RetInst: public Ir::Intf::Instruction
{
public:
    RetInst();
    RetInst(Ir::Intf::Object* value_);
    virtual std::string ToString() const;
    virtual bool IsTerminator() const { return true; }
    virtual bool IsRet() const { return true; }
private:
    Ir::Intf::Object* value;
};

Ir::Intf::Instruction* Ret();
Ir::Intf::Instruction* Ret(Ir::Intf::Object* value);

class BrInst: public Ir::Intf::Instruction
{
public:
    BrInst(Ir::Intf::LabelObject* dest_);
    BrInst(Ir::Intf::Object* cond_, Ir::Intf::LabelObject* trueLabel_, Ir::Intf::LabelObject* falseLabel_);
    virtual std::string ToString() const;
    virtual void AddTargetLabels(std::set<std::string>& targetLabels);
    virtual bool IsTerminator() const { return true; }
    virtual Ir::Intf::LabelObject* GetTargetLabel() const { return dest; }
    virtual bool IsUnconditionalBr() const;
private:
    Ir::Intf::LabelObject* dest;
    Ir::Intf::Object* cond;
    Ir::Intf::LabelObject* trueLabel;
    Ir::Intf::LabelObject* falseLabel;
};

Ir::Intf::Instruction* Br(Ir::Intf::LabelObject* dest);
Ir::Intf::Instruction* Br(Ir::Intf::Object* cond, Ir::Intf::LabelObject* trueLabel, Ir::Intf::LabelObject* falseLabel);

class SwitchInst: public Ir::Intf::Instruction
{
public:
    SwitchInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* value_, Ir::Intf::LabelObject* defaultDest_, const std::vector<std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>>& destinations_);
    virtual std::string ToString() const;
    virtual void AddTargetLabels(std::set<std::string>& targetLabels);
    virtual Ir::Intf::Object* GetResult() const { return value; }
    virtual bool IsTerminator() const { return true; }
private:
    Ir::Intf::Type* integerType;
    Ir::Intf::Object* value;
    Ir::Intf::LabelObject* defaultDest;
    std::vector<std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>> destinations;
};

Ir::Intf::Instruction* Switch(Ir::Intf::Type* integerType, Ir::Intf::Object* value, Ir::Intf::LabelObject* defaultDest, const std::vector<std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>>& destinations);

class AllocaInst: public Ir::Intf::Instruction
{
public:
    AllocaInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_);
    AllocaInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Type* elementType_, int numElements_);
    virtual void SetAlignment(int alignment_);
    virtual std::string ToString() const;
    virtual Ir::Intf::Object* GetResult() const { return result; }
private:
    Ir::Intf::Type* type;
    Ir::Intf::Object* result;
    Ir::Intf::Type* elementType;
    int numElements;
    int alignment;
};

Ir::Intf::Instruction* Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result);
Ir::Intf::Instruction* Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Type* elementType, int numElements);

class LoadInst: public Ir::Intf::Instruction
{
public:
    LoadInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* ptr_);
    virtual void SetAlignment(int alignment_);
    virtual std::string ToString() const;
    virtual bool ReturnsResult() const { return true; }
    virtual Ir::Intf::Object* GetResult() const { return result; }
private:
    Ir::Intf::Type* type;
    Ir::Intf::Object* result;
    Ir::Intf::Object* ptr;
    int alignment;
};

Ir::Intf::Instruction* Load(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* ptr);

class StoreInst: public Ir::Intf::Instruction
{
public:
    StoreInst(Ir::Intf::Type* type_, Ir::Intf::Object* value_, Ir::Intf::Object* ptr_);
    virtual void SetAlignment(int alignment_);
    virtual std::string ToString() const;
private:
    Ir::Intf::Type* type;
    Ir::Intf::Object* value;
    Ir::Intf::Object* ptr;
    int alignment;
};

Ir::Intf::Instruction* Store(Ir::Intf::Type* type, Ir::Intf::Object* value, Ir::Intf::Object* ptr);

class GetElementPtrInst: public Ir::Intf::Instruction
{
public:
    GetElementPtrInst(Ir::Intf::Type* ptrType_, Ir::Intf::Object* result_, Ir::Intf::Object* ptr_, Ir::Intf::Object* index_);
    GetElementPtrInst(Ir::Intf::Type* ptrType_, Ir::Intf::Object* result_, Ir::Intf::Object* ptr_, Ir::Intf::Object* index_, Ir::Intf::Object* index1_);
    GetElementPtrInst(Ir::Intf::Type* ptrType_, Ir::Intf::Object* result_, Ir::Intf::Object* ptr_, Ir::Intf::Object* index_, const std::vector<Ir::Intf::Object*>& indeces_);
	~GetElementPtrInst();
    virtual void SetInbounds() { inbounds = true; }
    virtual std::string ToString() const;
    virtual bool ReturnsResult() const { return true; }
    virtual Ir::Intf::Object* GetResult() const { return result; }
private:
    Ir::Intf::Type* ptrType;
    Ir::Intf::Object* result;
    Ir::Intf::Object* ptr;
    Ir::Intf::Object* index;
    std::vector<Ir::Intf::Object*> indeces;
    bool inbounds;
};

Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index);
Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index, Ir::Intf::Object* index1);
Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index, const std::vector<Ir::Intf::Object*>& indeces_);

class CallInst: public Ir::Intf::Instruction
{
public:
    CallInst(Ir::Intf::Object* result_, Ir::Intf::Function* fun_, const std::vector<Ir::Intf::Object*>& args_);
    virtual std::string ToString() const;
    virtual bool ReturnsResult() const { return result && !result->GetType()->IsVoidType(); }
    virtual Ir::Intf::Object* GetResult() const { return result; }
    virtual bool IsCall() const { return true; }
private:
    Ir::Intf::Object* result;
    Ir::Intf::Function* fun;
    std::vector<Ir::Intf::Object*> args;
};

Ir::Intf::Instruction* Call(Ir::Intf::Object* result, Ir::Intf::Function* fun, const std::vector<Ir::Intf::Object*>& args);

class IndirectCallInst: public Ir::Intf::Instruction
{
public:
    IndirectCallInst(Ir::Intf::Object* result_, Ir::Intf::Object* funPtr_, const std::vector<Ir::Intf::Object*>& args_);
    virtual std::string ToString() const;
    virtual bool ReturnsResult() const { return result && !result->GetType()->IsVoidType(); }
    virtual Ir::Intf::Object* GetResult() const { return result; }
    virtual bool IsCall() const { return true; }
private:
    Ir::Intf::Object* result;
    Ir::Intf::Object* funPtr;
    std::vector<Ir::Intf::Object*> args;
};

Ir::Intf::Instruction* IndirectCall(Ir::Intf::Object* result, Ir::Intf::Object* funPtr, const std::vector<Ir::Intf::Object*>& args);

class ICmpInst: public Ir::Intf::Instruction
{
public:
    ICmpInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::IConditionCode cond_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
    virtual std::string ToString() const;
    virtual bool ReturnsResult() const { return true; }
    virtual Ir::Intf::Object* GetResult() const { return result; }
private:
    Ir::Intf::Type* type;
    Ir::Intf::Object* result;
    Ir::Intf::IConditionCode cond;
    Ir::Intf::Object* operand1;
    Ir::Intf::Object* operand2;
};

Ir::Intf::Instruction* ICmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::IConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class FCmpInst: public Ir::Intf::Instruction
{
public:
    FCmpInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::FConditionCode cond_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_);
    virtual std::string ToString() const;
    virtual bool ReturnsResult() const { return true; }
    virtual Ir::Intf::Object* GetResult() const { return result; }
private:
    Ir::Intf::Type* type;
    Ir::Intf::Object* result;
    Ir::Intf::FConditionCode cond;
    Ir::Intf::Object* operand1;
    Ir::Intf::Object* operand2;
};

Ir::Intf::Instruction* FCmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::FConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2);

class DoNothingInst: public Ir::Intf::Instruction
{
public:
    DoNothingInst();
    virtual std::string ToString() const;
};

Ir::Intf::Instruction* DoNothing();

class ConversionInstruction: public Ir::Intf::Instruction
{
public:
    ConversionInstruction(const std::string& name_, Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
    virtual std::string ToString() const;
    virtual bool ReturnsResult() const { return true; }
    virtual Ir::Intf::Object* GetResult() const { return result; }
private:
    Ir::Intf::Type* type;
    Ir::Intf::Object* result;
    Ir::Intf::Object* value;
    Ir::Intf::Type* toType;
};

class TruncInst: public ConversionInstruction
{
public:
    TruncInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Trunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class ZextInst: public ConversionInstruction
{
public:
    ZextInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Zext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class SextInst: public ConversionInstruction
{
public:
    SextInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Sext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class FptruncInst: public ConversionInstruction
{
public:
    FptruncInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Fptrunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class FpextInst: public ConversionInstruction
{
public:
    FpextInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Fpext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class FptouiInst: public ConversionInstruction
{
public:
    FptouiInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Fptoui(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class FptosiInst: public ConversionInstruction
{
public:
    FptosiInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Fptosi(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class UitofpInst: public ConversionInstruction
{
public:
    UitofpInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Uitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class SitofpInst: public ConversionInstruction
{
public:
    SitofpInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Sitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class PtrtointInst: public ConversionInstruction
{
public:
    PtrtointInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Ptrtoint(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class InttoptrInst: public ConversionInstruction
{
public:
    InttoptrInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Inttoptr(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class BitcastInst: public ConversionInstruction
{
public:
    BitcastInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_);
};

Ir::Intf::Instruction* Bitcast(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType);

class DbgDeclareInst: public Ir::Intf::Instruction
{
public:
    DbgDeclareInst(Ir::Intf::Object* variable_, Ir::Intf::MetadataNode* descriptor_);
    virtual std::string ToString() const;
private:
    Ir::Intf::Object* variable;
    Ir::Intf::MetadataNode* descriptor;
};

Ir::Intf::Instruction* DbgDeclare(Ir::Intf::Object* variable, Ir::Intf::MetadataNode* descriptor);

} // namespace Llvm

#endif // LLVM_IR_INSTRUCTION_INCLUDED

