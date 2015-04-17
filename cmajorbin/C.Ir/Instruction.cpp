/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <C.Ir/Instruction.hpp>
#include <C.Ir/Function.hpp>
#include <Ir.Intf/Factory.hpp>

namespace C {

BinOpInstruction::BinOpInstruction(Ir::Intf::Type* type_, const std::string& name_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) :
    Ir::Intf::Instruction(name_), type(type_), result(result_), operand1(operand1_), operand2(operand2_)
{
}

std::string BinOpInstruction::ToString() const
{
    std::string s;
    s.append(result->Name()).append(" = ").append(operand1->Name()).append(" ").append(Name()).append(" ").append(operand2->Name());
    return s;
}

IntegerBinOpInstruction::IntegerBinOpInstruction(Ir::Intf::Type* type_, const std::string& name_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) :
    BinOpInstruction(type_, name_, result_, operand1_, operand2_)
{
}

bool IntegerBinOpInstruction::Valid() const
{
    Ir::Intf::Type* type = GetType();
    return type->IsIntegerType() && TypesEqual(type, GetResult()->GetType()) && TypesEqual(type, GetOperand1()->GetType()) && TypesEqual(type, GetOperand2()->GetType());
}

AddInst::AddInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : IntegerBinOpInstruction(integerType_, "+", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* Add(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new AddInst(integerType, result, operand1, operand2); }

SubInst::SubInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : IntegerBinOpInstruction(integerType_, "-", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* Sub(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new SubInst(integerType, result, operand1, operand2); }

MulInst::MulInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : IntegerBinOpInstruction(integerType_, "*", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* Mul(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new MulInst(integerType, result, operand1, operand2); }

UDivInst::UDivInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : IntegerBinOpInstruction(integerType_, "/", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* UDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new UDivInst(integerType, result, operand1, operand2); }

SDivInst::SDivInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : IntegerBinOpInstruction(integerType_, "/", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* SDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new SDivInst(integerType, result, operand1, operand2); }

URemInst::URemInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : IntegerBinOpInstruction(integerType_, "%", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* URem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new URemInst(integerType, result, operand1, operand2); }

SRemInst::SRemInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : IntegerBinOpInstruction(integerType_, "%", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* SRem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new SRemInst(integerType, result, operand1, operand2); }

ShlInst::ShlInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : IntegerBinOpInstruction(integerType_, "<<", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* Shl(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new ShlInst(integerType, result, operand1, operand2); }

LShrInst::LShrInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : IntegerBinOpInstruction(integerType_, ">>", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* LShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new LShrInst(integerType, result, operand1, operand2); }

AShrInst::AShrInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : IntegerBinOpInstruction(integerType_, ">>", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* AShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new AShrInst(integerType, result, operand1, operand2); }

AndInst::AndInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : IntegerBinOpInstruction(integerType_, "&", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* And(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new AndInst(integerType, result, operand1, operand2); }

OrInst::OrInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : IntegerBinOpInstruction(integerType_, "|", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* Or(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new OrInst(integerType, result, operand1, operand2); }

XorInst::XorInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : IntegerBinOpInstruction(integerType_, "^", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* Xor(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new XorInst(integerType, result, operand1, operand2); }

FloatingPointBinOpInstruction::FloatingPointBinOpInstruction(Ir::Intf::Type* type_, const std::string& name_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) :
    BinOpInstruction(type_, name_, result_, operand1_, operand2_)
{
}

bool FloatingPointBinOpInstruction::Valid() const
{
    Ir::Intf::Type* type = GetType();
    return type->IsFloatingPointType() && TypesEqual(type, GetResult()->GetType()) && TypesEqual(type, GetOperand1()->GetType()) && TypesEqual(type, GetOperand2()->GetType());
}

FAddInst::FAddInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : FloatingPointBinOpInstruction(floatingPointType_, "+", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* FAdd(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new FAddInst(floatingPointType, result, operand1, operand2); }

FSubInst::FSubInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : FloatingPointBinOpInstruction(floatingPointType_, "-", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* FSub(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new FSubInst(floatingPointType, result, operand1, operand2); }

FMulInst::FMulInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : FloatingPointBinOpInstruction(floatingPointType_, "*", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* FMul(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new FMulInst(floatingPointType, result, operand1, operand2); }

FDivInst::FDivInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : FloatingPointBinOpInstruction(floatingPointType_, "/", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* FDiv(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new FDivInst(floatingPointType, result, operand1, operand2); }

FRemInst::FRemInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) : FloatingPointBinOpInstruction(floatingPointType_, "%", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* FRem(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new FRemInst(floatingPointType, result, operand1, operand2); }

RetInst::RetInst() : Ir::Intf::Instruction("return"), value(nullptr)
{
}

RetInst::RetInst(Ir::Intf::Object* value_) : Ir::Intf::Instruction("return"), value(value_)
{
}

Ir::Intf::Instruction* Ret() { return new RetInst(); }

Ir::Intf::Instruction* Ret(Ir::Intf::Object* value) { return new RetInst(value); }

std::string RetInst::ToString() const
{
    std::string ret(Name());
    std::string space(1, ' ');
    if (value)
    {
        ret.append(space).append(value->Name());
    }
    return ret;
}

BrInst::BrInst(Ir::Intf::LabelObject* dest_) : Instruction("goto"), dest(dest_), cond(nullptr), trueLabel(nullptr), falseLabel(nullptr)
{
}

BrInst::BrInst(Ir::Intf::Object* cond_, Ir::Intf::LabelObject* trueLabel_, Ir::Intf::LabelObject* falseLabel_) : Ir::Intf::Instruction("goto"), dest(nullptr), cond(cond_), trueLabel(trueLabel_), falseLabel(falseLabel_)
{
}

bool BrInst::IsUnconditionalBr() const
{
    return dest != nullptr;
}

Ir::Intf::Instruction* Br(Ir::Intf::LabelObject* dest) { return new BrInst(dest); }

Ir::Intf::Instruction* Br(Ir::Intf::Object* cond, Ir::Intf::LabelObject* trueLabel, Ir::Intf::LabelObject* falseLabel) { return new BrInst(cond, trueLabel, falseLabel); }

std::string BrInst::ToString() const
{
    std::string br;
    std::string space(1, ' ');
    std::string comma(", ");
    if (cond)
    {
        br.append("if (").append(cond->Name()).append(") ").append(Name()).append(space).append(trueLabel->Name()).append("; else ").append(Name()).append(space).append(falseLabel->Name());
    }
    else
    {
        br.append(Name()).append(space).append(dest->Name());
    }
    return br;
}

void BrInst::AddTargetLabels(std::set<std::string>& targetLabels)
{
    if (dest)
    {
        targetLabels.insert(dest->Name());
    }
    else
    {
        if (trueLabel)
        {
            targetLabels.insert(trueLabel->Name());
        }
        if (falseLabel)
        {
            targetLabels.insert(falseLabel->Name());
        }
    }
}

SwitchInst::SwitchInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* value_, Ir::Intf::LabelObject* defaultDest_, const std::vector<std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>>& destinations_) :
    Ir::Intf::Instruction("switch"), integerType(integerType_), value(value_), defaultDest(defaultDest_), destinations(destinations_)
{
}

Ir::Intf::Instruction* Switch(Ir::Intf::Type* integerType, Ir::Intf::Object* value, Ir::Intf::LabelObject* defaultDest, const std::vector<std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>>& destinations)
{
    return new SwitchInst(integerType, value, defaultDest, destinations);
}

std::string SwitchInst::ToString() const
{
    std::string s(Name());
    s.append("(").append(value->Name()).append(") { ");
    for (const std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>& dest : destinations)
    {
        s.append("case ").append(dest.first->Name()).append(": goto ").append(dest.second->Name()).append("; break; ");
    }
    s.append("default: goto ").append(defaultDest->Name()).append("; break; }");
    return s;
}

void SwitchInst::AddTargetLabels(std::set<std::string>& targetLabels)
{
    targetLabels.insert(defaultDest->Name());
    for (const std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>& dest : destinations)
    {
        targetLabels.insert(dest.second->Name());
    }
}

AllocaInst::AllocaInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_) : Instruction("alloca"), type(type_), result(result_), elementType(nullptr), numElements(0), alignment(0)
{
}

AllocaInst::AllocaInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Type* elementType_, int numElements_) : Ir::Intf::Instruction("alloca"), type(type_), result(result_), elementType(elementType_), numElements(numElements_), alignment(0)
{
}

Ir::Intf::Instruction* Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result) { return new AllocaInst(type, result); }

Ir::Intf::Instruction* Alloca(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Type* elementType, int numElements) { return new AllocaInst(type, result, elementType, numElements); }

void AllocaInst::SetAlignment(int alignment_)
{
    alignment = alignment_;
}

std::string AllocaInst::ToString() const
{
    std::string alloca;
    std::string space(1, ' ');
    if (type->IsFunctionPointerType() || type->IsFunctionPtrPtrType())
    {
        alloca.append(type->Prefix()).append(result->Name()).append(type->Suffix());
    }
    else
    {
        alloca.append(type->Name()).append(space).append(result->Name());
    }
    return alloca;
}

LoadInst::LoadInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* ptr_, Ir::Intf::Indirection leftIndirection_, Ir::Intf::Indirection rightIndirection_) : 
    Ir::Intf::Instruction("load"), type(type_), result(result_), ptr(ptr_), alignment(0), leftIndirection(leftIndirection_), rightIndirection(rightIndirection_)
{
}

Ir::Intf::Instruction* Load(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Indirection leftIndirection, Ir::Intf::Indirection rightIndirection) 
{ 
    return new LoadInst(type, result, ptr, leftIndirection, rightIndirection);
}

void LoadInst::SetAlignment(int alignment_)
{
    alignment = alignment_;
}

std::string LoadInst::ToString() const
{
    std::string load;
    if (leftIndirection == Ir::Intf::Indirection::deref)
    {
        load.append("*");
    }
    else if (leftIndirection == Ir::Intf::Indirection::addr)
    {
        load.append("&");
    }
    load.append(result->Name()).append(" = ");
    if (rightIndirection == Ir::Intf::Indirection::deref)
    {
        load.append("*");
    }
    else if (rightIndirection == Ir::Intf::Indirection::addr)
    {
        load.append("&");
    }
    load.append(ptr->Name());
    return load;
}

StoreInst::StoreInst(Ir::Intf::Type* type_, Ir::Intf::Object* value_, Ir::Intf::Object* ptr_, Ir::Intf::Indirection leftIndirection_, Ir::Intf::Indirection rightIndirection_) : 
    Ir::Intf::Instruction("store"), type(type_), value(value_), ptr(ptr_), alignment(0), leftIndirection(leftIndirection_), rightIndirection(rightIndirection_)
{
}

void StoreInst::SetAlignment(int alignment_)
{
    alignment = alignment_;
}

std::string StoreInst::ToString() const
{
    std::string store;
    if (rightIndirection == Ir::Intf::Indirection::deref)
    {
        store.append("*");
    }
    else if (rightIndirection == Ir::Intf::Indirection::addr)
    {
        store.append("&");
    }
    store.append(ptr->Name()).append(" = ");
    if (leftIndirection == Ir::Intf::Indirection::deref)
    {
        store.append("*");
    }
    else if (leftIndirection == Ir::Intf::Indirection::addr)
    {
        store.append("&");
    }
    store.append(value->Name());
    return store;
}

Ir::Intf::Instruction* Store(Ir::Intf::Type* type, Ir::Intf::Object* value, Ir::Intf::Object* ptr, Ir::Intf::Indirection leftIndirection, Ir::Intf::Indirection rightIndirection) 
{ 
    return new StoreInst(type, value, ptr, leftIndirection, rightIndirection);
}


SetInst::SetInst(Ir::Intf::Object* from_, Ir::Intf::Object* to_) : Ir::Intf::Instruction("set"), from(from_), to(to_)
{
}

Ir::Intf::Instruction* Set(Ir::Intf::Object* from, Ir::Intf::Object* to) { return new SetInst(from, to); }

std::string SetInst::ToString() const
{
    std::string set;
    set.append(to->Name() + " = " + from->Name());
    return set;
}

CallInst::CallInst(Ir::Intf::Object* result_, Ir::Intf::Function* fun_, const std::vector<Ir::Intf::Object*>& args_) : Instruction("call"), result(result_), fun(fun_), args()
{
    int n = int(args_.size());
    for (int i = 0; i < n; ++i)
    {
        args.push_back(args_[i]);
    }
}

std::string CallInst::ToString() const
{
    std::string call;
    if (result && !result->GetType()->IsVoidType())
    {
        call.append(result->Name()).append(" = ");
    }
    std::string comma(", ");
    call.append(fun->Name()).append("(");
    bool first = true;
    for (Ir::Intf::Object* arg : args)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            call.append(comma);
        }
        if (arg->IsStackVar() || arg->IsGlobal())
        {
            call.append("&");
        }
        call.append(arg->Name());
    }
    call.append(")");
    return call;
}

Ir::Intf::Instruction* Call(Ir::Intf::Object* result, Ir::Intf::Function* fun, const std::vector<Ir::Intf::Object*>& args) { return new CallInst(result, fun, args); }

IndirectCallInst::IndirectCallInst(Ir::Intf::Object* result_, Ir::Intf::Object* funPtr_, const std::vector<Ir::Intf::Object*>& args_) : Ir::Intf::Instruction("call"), result(result_), funPtr(funPtr_), args(args_)
{
}

std::string IndirectCallInst::ToString() const
{
    std::string call;
    if (result && !result->GetType()->IsVoidType())
    {
        call.append(result->Name()).append(" = ");
    }
    std::string comma(", ");
    call.append(funPtr->Name()).append("(");
    bool first = true;
    for (Ir::Intf::Object* arg : args)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            call.append(comma);
        }
        if (arg->IsStackVar() || arg->IsGlobal())
        {
            call.append("&");
        }
        call.append(arg->Name());
    }
    call.append(")");
    return call;
}

Ir::Intf::Instruction* IndirectCall(Ir::Intf::Object* result, Ir::Intf::Object* funPtr, const std::vector<Ir::Intf::Object*>& args)
{
    return new IndirectCallInst(result, funPtr, args);
}

const char* ConditionCodeStr(Ir::Intf::IConditionCode cond)
{
    switch (cond)
    {
    case Ir::Intf::IConditionCode::eq: return "==";
    case Ir::Intf::IConditionCode::ne: return "!=";
    case Ir::Intf::IConditionCode::ugt: return ">";
    case Ir::Intf::IConditionCode::uge: return ">=";
    case Ir::Intf::IConditionCode::ult: return "<";
    case Ir::Intf::IConditionCode::ule: return "<=";
    case Ir::Intf::IConditionCode::sgt: return ">";
    case Ir::Intf::IConditionCode::sge: return ">=";
    case Ir::Intf::IConditionCode::slt: return "<";
    case Ir::Intf::IConditionCode::sle: return "<=";
    }
    return "";
}

ICmpInst::ICmpInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::IConditionCode cond_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) :
    Ir::Intf::Instruction("icmp"), type(type_), result(result_), cond(cond_), operand1(operand1_), operand2(operand2_)
{
}

Ir::Intf::Instruction* ICmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::IConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new ICmpInst(type, result, cond, operand1, operand2); }

std::string ICmpInst::ToString() const
{
    std::string icmp;
    std::string space(1, ' ');
    icmp.append(result->Name()).append(" = ").append(operand1->Name()).append(space).append(ConditionCodeStr(cond)).append(space).append(operand2->Name());
    return icmp;
}

const char* ConditionCodeStr(Ir::Intf::FConditionCode cond)
{
    switch (cond)
    {
    case Ir::Intf::FConditionCode::false_: return "false";
    case Ir::Intf::FConditionCode::oeq: return "==";
    case Ir::Intf::FConditionCode::ogt: return ">";
    case Ir::Intf::FConditionCode::oge: return ">=";
    case Ir::Intf::FConditionCode::olt: return "<";
    case Ir::Intf::FConditionCode::ole: return "<=";
    case Ir::Intf::FConditionCode::one: return "!=";
    case Ir::Intf::FConditionCode::ord: return "?";
    case Ir::Intf::FConditionCode::ueq: return "==";
    case Ir::Intf::FConditionCode::ugt: return ">";
    case Ir::Intf::FConditionCode::uge: return ">=";
    case Ir::Intf::FConditionCode::ult: return "<";
    case Ir::Intf::FConditionCode::ule: return "<=";
    case Ir::Intf::FConditionCode::une: return "!=";
    case Ir::Intf::FConditionCode::uno: return "?";
    case Ir::Intf::FConditionCode::true_: return "true";
    }
    return "";
}

FCmpInst::FCmpInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::FConditionCode cond_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_) :
    Ir::Intf::Instruction("fcmp"), type(type_), result(result_), cond(cond_), operand1(operand1_), operand2(operand2_)
{
}

Ir::Intf::Instruction* FCmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::FConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new FCmpInst(type, result, cond, operand1, operand2); }

std::string FCmpInst::ToString() const
{
    std::string fcmp;
    std::string space(1, ' ');
    fcmp.append(result->Name()).append(" = ").append(operand1->Name()).append(space).append(ConditionCodeStr(cond)).append(space).append(operand2->Name());
    return fcmp;
}

DoNothingInst::DoNothingInst() : Instruction("_X_dummy = true")
{
}

std::string DoNothingInst::ToString() const
{
    std::string doNothing(Name());
    return doNothing;
}

Ir::Intf::Instruction* DoNothing()
{
    return new DoNothingInst();
}

ConversionInstruction::ConversionInstruction(const std::string& name_, Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_) :
    Ir::Intf::Instruction(name_), type(type_), result(result_), value(value_), toType(toType_)
{
}

std::string ConversionInstruction::ToString() const
{
    std::string s;
    std::string space(1, ' ');
    s.append(result->Name()).append(" = ").append("(").append(toType->Name()).append(")").append(value->Name());
    return s;
}

TruncInst::TruncInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_) :
    ConversionInstruction("trunc", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Trunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new TruncInst(type, result, value, toType); }

ZextInst::ZextInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_) :
    ConversionInstruction("zext", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Zext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new ZextInst(type, result, value, toType); }

SextInst::SextInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_) :
    ConversionInstruction("sext", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Sext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new SextInst(type, result, value, toType); }

FptruncInst::FptruncInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_) :
    ConversionInstruction("fptrunc", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Fptrunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new FptruncInst(type, result, value, toType); }

FpextInst::FpextInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_) :
    ConversionInstruction("fpext", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Fpext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new FpextInst(type, result, value, toType); }

FptouiInst::FptouiInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_) :
    ConversionInstruction("fptoui", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Fptoui(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new FptouiInst(type, result, value, toType); }

FptosiInst::FptosiInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_) :
    ConversionInstruction("fptosi", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Fptosi(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new FptosiInst(type, result, value, toType); }

UitofpInst::UitofpInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_) :
    ConversionInstruction("uitofp", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Uitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new UitofpInst(type, result, value, toType); }

SitofpInst::SitofpInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_) :
    ConversionInstruction("sitofp", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Sitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new SitofpInst(type, result, value, toType); }

PtrtointInst::PtrtointInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_) :
    ConversionInstruction("ptrtoint", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Ptrtoint(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new PtrtointInst(type, result, value, toType); }

InttoptrInst::InttoptrInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_) :
    ConversionInstruction("inttoptr", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Inttoptr(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new InttoptrInst(type, result, value, toType); }

BitcastInst::BitcastInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_) :
    ConversionInstruction("bitcast", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Bitcast(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new BitcastInst(type, result, value, toType); }

SizeOfInst::SizeOfInst(Ir::Intf::Object* result_, Ir::Intf::Type* type_) :
    Instruction("sizeof"), result(result_), type(type_)
{
}

std::string SizeOfInst::ToString() const
{
    std::string sizeOf;
    sizeOf.append(result->Name()).append(" = ").append(Name()).append("(").append(type->Name()).append(")");
    return sizeOf;
}

Ir::Intf::Instruction* CreateSizeOf(Ir::Intf::Object* result, Ir::Intf::Type* type) { return new SizeOfInst(result, type); }

} // namespace C
