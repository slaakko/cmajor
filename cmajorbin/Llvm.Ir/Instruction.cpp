/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Llvm.Ir/Instruction.hpp>
#include <Llvm.Ir/Function.hpp>
#include <Llvm.Ir/Metadata.hpp>
#include <Ir.Intf/Factory.hpp>

namespace Llvm { 

BinOpInstruction::BinOpInstruction(Ir::Intf::Type* type_, const std::string& name_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_):
    Ir::Intf::Instruction(name_), type(type_), result(result_), operand1(operand1_), operand2(operand2_)
{
}

std::string BinOpInstruction::ToString() const
{
    std::string s;
    std::string space(1, ' ');
    s.append(result->Name()).append(" = ").append(Name()).append(space).append(type->Name()).append(space).append(operand1->Name()).append(", ").append(operand2->Name());
    return s;
}

IntegerBinOpInstruction::IntegerBinOpInstruction(Ir::Intf::Type* type_, const std::string& name_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): 
    BinOpInstruction(type_, name_, result_, operand1_, operand2_)
{
}

bool IntegerBinOpInstruction::Valid() const
{
    Ir::Intf::Type* type = GetType();
    return type->IsIntegerType() && TypesEqual(type, GetResult()->GetType()) && TypesEqual(type, GetOperand1()->GetType()) && TypesEqual(type, GetOperand2()->GetType());
}

AddInst::AddInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): IntegerBinOpInstruction(integerType_, "add", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* Add(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new AddInst(integerType, result, operand1, operand2); }

SubInst::SubInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): IntegerBinOpInstruction(integerType_, "sub", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* Sub(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new SubInst(integerType, result, operand1, operand2); }

MulInst::MulInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): IntegerBinOpInstruction(integerType_, "mul", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* Mul(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new MulInst(integerType, result, operand1, operand2); }

UDivInst::UDivInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): IntegerBinOpInstruction(integerType_, "udiv", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* UDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new UDivInst(integerType, result, operand1, operand2); }

SDivInst::SDivInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): IntegerBinOpInstruction(integerType_, "sdiv", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* SDiv(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new SDivInst(integerType, result, operand1, operand2); }

URemInst::URemInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): IntegerBinOpInstruction(integerType_, "urem", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* URem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new URemInst(integerType, result, operand1, operand2); }

SRemInst::SRemInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): IntegerBinOpInstruction(integerType_, "srem", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* SRem(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new SRemInst(integerType, result, operand1, operand2); }

ShlInst::ShlInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): IntegerBinOpInstruction(integerType_, "shl", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* Shl(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new ShlInst(integerType, result, operand1, operand2); }

LShrInst::LShrInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): IntegerBinOpInstruction(integerType_, "lshr", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* LShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new LShrInst(integerType, result, operand1, operand2); }

AShrInst::AShrInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): IntegerBinOpInstruction(integerType_, "ashr", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* AShr(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new AShrInst(integerType, result, operand1, operand2); }

AndInst::AndInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): IntegerBinOpInstruction(integerType_, "and", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* And(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new AndInst(integerType, result, operand1, operand2); }

OrInst::OrInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): IntegerBinOpInstruction(integerType_, "or", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* Or(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new OrInst(integerType, result, operand1, operand2); }

XorInst::XorInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): IntegerBinOpInstruction(integerType_, "xor", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* Xor(Ir::Intf::Type* integerType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new XorInst(integerType, result, operand1, operand2); }

FloatingPointBinOpInstruction::FloatingPointBinOpInstruction(Ir::Intf::Type* type_, const std::string& name_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): 
    BinOpInstruction(type_, name_, result_, operand1_, operand2_)
{
}

bool FloatingPointBinOpInstruction::Valid() const
{
    Ir::Intf::Type* type = GetType();
    return type->IsFloatingPointType() && TypesEqual(type, GetResult()->GetType()) && TypesEqual(type, GetOperand1()->GetType()) && TypesEqual(type, GetOperand2()->GetType());
}

FAddInst::FAddInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): FloatingPointBinOpInstruction(floatingPointType_, "fadd", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* FAdd(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new FAddInst(floatingPointType, result, operand1, operand2); }

FSubInst::FSubInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): FloatingPointBinOpInstruction(floatingPointType_, "fsub", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* FSub(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new FSubInst(floatingPointType, result, operand1, operand2); }

FMulInst::FMulInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): FloatingPointBinOpInstruction(floatingPointType_, "fmul", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* FMul(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new FMulInst(floatingPointType, result, operand1, operand2); }

FDivInst::FDivInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): FloatingPointBinOpInstruction(floatingPointType_, "fdiv", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* FDiv(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new FDivInst(floatingPointType, result, operand1, operand2); }

FRemInst::FRemInst(Ir::Intf::Type* floatingPointType_, Ir::Intf::Object* result_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): FloatingPointBinOpInstruction(floatingPointType_, "frem", result_, operand1_, operand2_)
{
}

Ir::Intf::Instruction* FRem(Ir::Intf::Type* floatingPointType, Ir::Intf::Object* result, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new FRemInst(floatingPointType, result, operand1, operand2); }

RetInst::RetInst(): Ir::Intf::Instruction("ret"), value(nullptr)
{
}

RetInst::RetInst(Ir::Intf::Object* value_): Ir::Intf::Instruction("ret"), value(value_)
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
        ret.append(space).append(value->GetType()->Name()).append(space).append(value->Name());
    }
    else
    {
        ret.append(space).append(Ir::Intf::GetFactory()->GetVoid()->Name());
    }
    return ret;
}

BrInst::BrInst(Ir::Intf::LabelObject* dest_): Instruction("br"), dest(dest_), cond(nullptr), trueLabel(nullptr), falseLabel(nullptr)
{
}

BrInst::BrInst(Ir::Intf::Object* cond_, Ir::Intf::LabelObject* trueLabel_, Ir::Intf::LabelObject* falseLabel_): Ir::Intf::Instruction("br"), dest(nullptr), cond(cond_), trueLabel(trueLabel_), falseLabel(falseLabel_)
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
    std::string br(Name());
    std::string space(1, ' ');
    std::string comma(", ");
    Ir::Intf::Type* labelType = Ir::Intf::GetFactory()->GetLabelType();
    if (cond)
    {
        br.append(space).append(Ir::Intf::GetFactory()->GetI1()->Name()).append(space).append(cond->Name()).append(comma).append(labelType->Name()).append(space).append(
            trueLabel->Name()).append(comma).append(labelType->Name()).append(space).append(falseLabel->Name());
    }
    else
    {
        br.append(space).append(labelType->Name()).append(space).append(dest->Name());
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

SwitchInst::SwitchInst(Ir::Intf::Type* integerType_, Ir::Intf::Object* value_, Ir::Intf::LabelObject* defaultDest_, const std::vector<std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>>& destinations_): 
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
    std::string space(1, ' ');
    std::string comma(", ");
    Ir::Intf::Type* labelType = Ir::Intf::GetFactory()->GetLabelType();
    s.append(space).append(integerType->Name()).append(space).append(value->Name()).append(comma).append(labelType->Name()).append(space).append(defaultDest->Name()).append(space).append("[");
    for (const std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>& dest : destinations)
    {
        s.append(space).append(integerType->Name()).append(space).append(dest.first->Name()).append(comma).append(labelType->Name()).append(space).append(dest.second->Name());
    }
    s.append(" ]");
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

AllocaInst::AllocaInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_): Instruction("alloca"), type(type_), result(result_), elementType(nullptr), numElements(0), alignment(0)
{
}

AllocaInst::AllocaInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Type* elementType_, int numElements_): Ir::Intf::Instruction("alloca"), type(type_), result(result_), elementType(elementType_), numElements(numElements_), alignment(0)
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
    std::string comma(", ");
    alloca.append(result->Name()).append(" = ").append(Name()).append(space).append(type->Name());
    if (numElements > 0)
    {
        alloca.append(comma).append(elementType->Name()).append(space).append(std::to_string(numElements));
    }
    if (alignment > 0)
    {
        alloca.append(comma).append("align ").append(std::to_string(alignment));
    }
    return alloca;
}

LoadInst::LoadInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* ptr_): Ir::Intf::Instruction("load"), type(type_), result(result_), ptr(ptr_), alignment(0)
{
}

Ir::Intf::Instruction* Load(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* ptr) { return new LoadInst(type, result, ptr); }

void LoadInst::SetAlignment(int alignment_)
{
    alignment = alignment_;
}

std::string LoadInst::ToString() const
{
    std::string load;
    std::string space(1, ' ');
    std::string comma(", ");
    load.append(result->Name()).append(" = ").append(Name()).append(space).append(type->Name()).append(space).append(ptr->Name());
    if (alignment > 0)
    {
        load.append(comma).append("align ").append(std::to_string(alignment));
    }
    return load;
}

StoreInst::StoreInst(Ir::Intf::Type* type_, Ir::Intf::Object* value_, Ir::Intf::Object* ptr_): Ir::Intf::Instruction("store"), type(type_), value(value_), ptr(ptr_), alignment(0)
{
}

Ir::Intf::Instruction* Store(Ir::Intf::Type* type, Ir::Intf::Object* value, Ir::Intf::Object* ptr) { return new StoreInst(type, value, ptr); }

void StoreInst::SetAlignment(int alignment_)
{
    alignment = alignment_;
}

std::string StoreInst::ToString() const
{
    std::string store(Name());
    std::string space(1, ' ');
    std::string comma(", ");
    store.append(space).append(type->Name()).append(space).append(value->Name()).append(comma).append(ptr->GetType()->Name()).append(space).append(ptr->Name());
    if (alignment > 0)
    {
        store.append(comma).append("align ").append(std::to_string(alignment));
    }
    return store;
}

GetElementPtrInst::GetElementPtrInst(Ir::Intf::Type* ptrType_, Ir::Intf::Object* result_, Ir::Intf::Object* ptr_, Ir::Intf::Object* index_): 
    Ir::Intf::Instruction("getelementptr"), ptrType(ptrType_), result(result_), ptr(ptr_), index(index_), indeces(), inbounds(false)
{
}

GetElementPtrInst::GetElementPtrInst(Ir::Intf::Type* ptrType_, Ir::Intf::Object* result_, Ir::Intf::Object* ptr_, Ir::Intf::Object* index_, Ir::Intf::Object* index1_): 
    Ir::Intf::Instruction("getelementptr"), ptrType(ptrType_), result(result_), ptr(ptr_), index(index_), indeces(), inbounds(false)
{
	indeces.push_back(index1_);
	if (index1_->GetType()->IsLabelType())
	{
		int x = 0;
	}
}

GetElementPtrInst::GetElementPtrInst(Ir::Intf::Type* ptrType_, Ir::Intf::Object* result_, Ir::Intf::Object* ptr_, Ir::Intf::Object* index_, const std::vector<Ir::Intf::Object*>& indeces_): 
    Ir::Intf::Instruction("getelementptr"), ptrType(ptrType_), result(result_), ptr(ptr_), index(index_), indeces(indeces_), inbounds(false)
{
}

GetElementPtrInst::~GetElementPtrInst()
{
	if (debug)
	{
		int x = 0;
	}
}

Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index)
{
    return new GetElementPtrInst(ptrType, result, ptr, index);
}

Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index, Ir::Intf::Object* index1)
{
    return new GetElementPtrInst(ptrType, result, ptr, index, index1);
}

Ir::Intf::Instruction* GetElementPtr(Ir::Intf::Type* ptrType, Ir::Intf::Object* result, Ir::Intf::Object* ptr, Ir::Intf::Object* index, const std::vector<Ir::Intf::Object*>& indeces)
{
    return new GetElementPtrInst(ptrType, result, ptr, index, indeces);
}

std::string GetElementPtrInst::ToString() const
{
    std::string getElementPtr;
    std::string space(1, ' ');
    std::string comma(", ");
    getElementPtr.append(result->Name()).append(" = ").append(Name()).append(space);
    if (inbounds)
    {
        getElementPtr.append("inbounds").append(space);
    }
    getElementPtr.append(ptrType->Name()).append(space).append(ptr->Name()).append(comma).append(index->GetType()->Name()).append(space).append(index->Name());   
    for (Ir::Intf::Object* idx : indeces)
    {
		if (idx->GetType()->IsLabelType())
		{
			int x = 0;
		}
        getElementPtr.append(comma).append(idx->GetType()->Name()).append(space).append(idx->Name());
    }
    return getElementPtr;
}

CallInst::CallInst(Ir::Intf::Object* result_, Ir::Intf::Function* fun_, const std::vector<Ir::Intf::Object*>& args_): Instruction("call"), result(result_), fun(fun_), args()
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
    std::string space(1, ' ');
    std::string comma(", ");
    call.append(Name()).append(space).append(fun->ReturnType()->Name()).append(" @").append(fun->Name()).append("(");
    bool first = true;
    for (Ir::Intf::Object* arg : args)
    {
		if (arg->GetType()->Name() == "label")
		{
			int x = 0;
		}
        if (first)
        {
            first = false;
        }
        else
        {
            call.append(comma);
        }
        call.append(arg->GetType()->Name()).append(space).append(arg->Name());
    }
    call.append(")");
    return call;
}

Ir::Intf::Instruction* Call(Ir::Intf::Object* result, Ir::Intf::Function* fun, const std::vector<Ir::Intf::Object*>& args) { return new CallInst(result, fun, args); }

IndirectCallInst::IndirectCallInst(Ir::Intf::Object* result_, Ir::Intf::Object* funPtr_, const std::vector<Ir::Intf::Object*>& args_): Ir::Intf::Instruction("call"), result(result_), funPtr(funPtr_), args(args_)
{
}

std::string IndirectCallInst::ToString() const
{
    std::string call;
    std::string returnTypeName = "void";
    if (result && !result->GetType()->IsVoidType())
    {
        call.append(result->Name()).append(" = ");
        returnTypeName = result->GetType()->Name();
    }
    std::string space(1, ' ');
    std::string comma(", ");
    call.append(Name()).append(space).append(returnTypeName).append(space).append(funPtr->Name()).append("(");
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
        call.append(arg->GetType()->Name()).append(space).append(arg->Name());
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
        case Ir::Intf::IConditionCode::eq: return "eq";
        case Ir::Intf::IConditionCode::ne: return "ne";
        case Ir::Intf::IConditionCode::ugt: return "ugt";
        case Ir::Intf::IConditionCode::uge: return "uge";
        case Ir::Intf::IConditionCode::ult: return "ult";
        case Ir::Intf::IConditionCode::ule: return "ule";
        case Ir::Intf::IConditionCode::sgt: return "sgt";
        case Ir::Intf::IConditionCode::sge: return "sge";
        case Ir::Intf::IConditionCode::slt: return "slt";
        case Ir::Intf::IConditionCode::sle: return "sle";
    }
    return "";
}

ICmpInst::ICmpInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::IConditionCode cond_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): 
    Ir::Intf::Instruction("icmp"), type(type_), result(result_), cond(cond_), operand1(operand1_), operand2(operand2_)
{
}

Ir::Intf::Instruction* ICmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::IConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new ICmpInst(type, result, cond, operand1, operand2); }

std::string ICmpInst::ToString() const
{
    std::string icmp;
    std::string space(1, ' ');
    std::string comma(", ");
    icmp.append(result->Name()).append(" = ").append(Name()).append(space).append(ConditionCodeStr(cond)).append(space).append(
        type->Name()).append(space).append(operand1->Name()).append(comma).append(operand2->Name());
    return icmp;
}

const char* ConditionCodeStr(Ir::Intf::FConditionCode cond)
{
    switch (cond)
    {
        case Ir::Intf::FConditionCode::false_: return "false";
        case Ir::Intf::FConditionCode::oeq: return "oeq";
        case Ir::Intf::FConditionCode::ogt: return "ogt";
        case Ir::Intf::FConditionCode::oge: return "oge";
        case Ir::Intf::FConditionCode::olt: return "olt";
        case Ir::Intf::FConditionCode::ole: return "ole";
        case Ir::Intf::FConditionCode::one: return "one";
        case Ir::Intf::FConditionCode::ord: return "ord";
        case Ir::Intf::FConditionCode::ueq: return "ueq";
        case Ir::Intf::FConditionCode::ugt: return "ugt";
        case Ir::Intf::FConditionCode::uge: return "uge";
        case Ir::Intf::FConditionCode::ult: return "ult";
        case Ir::Intf::FConditionCode::ule: return "ule";
        case Ir::Intf::FConditionCode::une: return "une";
        case Ir::Intf::FConditionCode::uno: return "uno";
        case Ir::Intf::FConditionCode::true_: return "true";
    }
    return "";
}

FCmpInst::FCmpInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::FConditionCode cond_, Ir::Intf::Object* operand1_, Ir::Intf::Object* operand2_): 
    Ir::Intf::Instruction("fcmp"), type(type_), result(result_), cond(cond_), operand1(operand1_), operand2(operand2_)
{
}

Ir::Intf::Instruction* FCmp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::FConditionCode cond, Ir::Intf::Object* operand1, Ir::Intf::Object* operand2) { return new FCmpInst(type, result, cond, operand1, operand2); }

std::string FCmpInst::ToString() const
{
    std::string fcmp;
    std::string space(1, ' ');
    std::string comma(", ");
    fcmp.append(result->Name()).append(" = ").append(Name()).append(space).append(ConditionCodeStr(cond)).append(space).append(
        type->Name()).append(space).append(operand1->Name()).append(comma).append(operand2->Name());
    return fcmp;
}

DoNothingInst::DoNothingInst(): Instruction("call void @llvm.donothing()")
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

ConversionInstruction::ConversionInstruction(const std::string& name_, Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_): 
    Ir::Intf::Instruction(name_), type(type_), result(result_), value(value_), toType(toType_)
{
}

std::string ConversionInstruction::ToString() const
{
    std::string s;
    std::string space(1, ' ');
    s.append(result->Name()).append(" = ").append(Name()).append(space).append(type->Name()).append(space).append(value->Name()).append(" to ").append(toType->Name());
    return s;
}

TruncInst::TruncInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_): 
    ConversionInstruction("trunc", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Trunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new TruncInst(type, result, value, toType); }

ZextInst::ZextInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_): 
    ConversionInstruction("zext", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Zext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new ZextInst(type, result, value, toType); }

SextInst::SextInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_): 
    ConversionInstruction("sext", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Sext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new SextInst(type, result, value, toType); }

FptruncInst::FptruncInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_): 
    ConversionInstruction("fptrunc", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Fptrunc(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new FptruncInst(type, result, value, toType); }

FpextInst::FpextInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_): 
    ConversionInstruction("fpext", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Fpext(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new FpextInst(type, result, value, toType); }

FptouiInst::FptouiInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_): 
    ConversionInstruction("fptoui", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Fptoui(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new FptouiInst(type, result, value, toType); }

FptosiInst::FptosiInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_): 
    ConversionInstruction("fptosi", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Fptosi(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new FptosiInst(type, result, value, toType); }

UitofpInst::UitofpInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_): 
    ConversionInstruction("uitofp", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Uitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new UitofpInst(type, result, value, toType); }

SitofpInst::SitofpInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_): 
    ConversionInstruction("sitofp", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Sitofp(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new SitofpInst(type, result, value, toType); }

PtrtointInst::PtrtointInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_): 
    ConversionInstruction("ptrtoint", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Ptrtoint(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new PtrtointInst(type, result, value, toType); }

InttoptrInst::InttoptrInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_): 
    ConversionInstruction("inttoptr", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Inttoptr(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new InttoptrInst(type, result, value, toType); }

BitcastInst::BitcastInst(Ir::Intf::Type* type_, Ir::Intf::Object* result_, Ir::Intf::Object* value_, Ir::Intf::Type* toType_): 
    ConversionInstruction("bitcast", type_, result_, value_, toType_)
{
}

Ir::Intf::Instruction* Bitcast(Ir::Intf::Type* type, Ir::Intf::Object* result, Ir::Intf::Object* value, Ir::Intf::Type* toType) { return new BitcastInst(type, result, value, toType); }

DbgDeclareInst::DbgDeclareInst(Ir::Intf::Object* variable_, Ir::Intf::MetadataNode* descriptor_): Ir::Intf::Instruction("dbgdeclare"), variable(variable_), descriptor(descriptor_)
{
}

std::string DbgDeclareInst::ToString() const
{
    std::string dbgdeclare;
    std::string space(1, ' ');
    std::string comma(", ");
    dbgdeclare.append("call void @llvm.dbg.declare(metadata !{").append(variable->GetType()->Name()).append(space).append(variable->Name()).append("}");
    dbgdeclare.append(comma).append(space).append("metadata !" + std::to_string(descriptor->Id())).append(")");
    return dbgdeclare;
}

Ir::Intf::Instruction* DbgDeclare(Ir::Intf::Object* variable, Ir::Intf::MetadataNode* descriptor) { return new DbgDeclareInst(variable, descriptor); }

} // namespace Llvm
