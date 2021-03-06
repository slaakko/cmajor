/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/GenData.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <Cm.Core/CDebugInfo.hpp>
#include <Cm.Core/GlobalSettings.hpp>
#include <Ir.Intf/Label.hpp>
#include <Llvm.Ir/Instruction.hpp>
#include <iostream>
#include <stdexcept>

namespace Cm { namespace Core {

GenData::GenData() : label(nullptr)
{
}

Ir::Intf::Object* GenData::MainObject() const
{
    if (objects.empty())
    {
        throw std::runtime_error("empty gendata");
    }
    return objects.front();
}

void GenData::SetMainObject(Ir::Intf::Object* mainObject)
{
    if (objects.empty())
    {
        objects.push_back(mainObject);
    }
    else
    {
        objects[0] = mainObject;
    }
}

Ir::Intf::Object* GenData::Arg1() const
{
    if (objects.size() <= 1)
    {
        throw std::runtime_error("gendata has no arg1");
    }
    return objects[1];
}

Ir::Intf::Object* GenData::Arg2() const
{
    if (objects.size() <= 2)
    {
        throw std::runtime_error("gendata has no arg2");
    }
    return objects[2];
}

std::vector<Ir::Intf::Object*> GenData::Args() const
{
    std::vector<Ir::Intf::Object*> args;
    int n = int(objects.size());
    for (int i = 1; i < n; ++i)
    {
        args.push_back(objects[i]);
    }
    return args;
}

void GenData::AddTrueTarget(Ir::Intf::LabelObject* trueTarget)
{
    Ir::Intf::Add(trueTargets, trueTarget);
}

void GenData::AddFalseTarget(Ir::Intf::LabelObject* falseTarget)
{
    Ir::Intf::Add(falseTargets, falseTarget);
}

void GenData::AddNextTarget(Ir::Intf::LabelObject* nextTarget)
{
    Ir::Intf::Add(nextTargets, nextTarget);
}

void GenData::MergeTargets(std::vector<Ir::Intf::LabelObject*>& targets, std::vector<Ir::Intf::LabelObject*>& fromTargets)
{
    Ir::Intf::Merge(targets, fromTargets);
    fromTargets.clear();
}

void GenData::MergeData(std::shared_ptr<GenData> childData)
{
    if (!childData->IsEmpty())
    {
        objects.push_back(childData->MainObject());
    }
    Ir::Intf::Merge(nextTargets, childData->nextTargets);
    Ir::Intf::Merge(trueTargets, childData->trueTargets);
    Ir::Intf::Merge(falseTargets, childData->falseTargets);
}

Ir::Intf::LabelObject* GenData::GetLabel() const 
{ 
    return label;
}

void GenData::SetLabel(Ir::Intf::LabelObject* label_)
{
    label = label_;
}

void GenData::BackpatchTrueTargets(Ir::Intf::LabelObject* label_)
{
    if (!label_)
    {
        throw std::runtime_error("backpatch true targets got no label");
    }
    Ir::Intf::Backpatch(trueTargets, label_);
    trueTargets.clear();
}

void GenData::BackpatchFalseTargets(Ir::Intf::LabelObject* label_)
{
    if (!label_)
    {
        throw std::runtime_error("backpatch false targets got no label");
    }
    Ir::Intf::Backpatch(falseTargets, label_);
    falseTargets.clear();
}

void GenData::BackpatchNextTargets(Ir::Intf::LabelObject* label_)
{
    if (!label_)
    {
        throw std::runtime_error("backpatch next targets got no label");
    }
    Ir::Intf::Backpatch(nextTargets, label_);
    nextTargets.clear();
}

Emitter::Emitter() : irFunction(nullptr), gotoTargetLabel(nullptr), cDebugNode(nullptr), activeCfgNode(nullptr), profilingHandler(nullptr), newLlvmSyntax(false), irecPtrAllocated(false), 
    tempBoolVarAllocted(false), interfaceLookupFailed(nullptr)
{
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm && Cm::Core::GetGlobalSettings()->LlvmVersion() >= Cm::Ast::ProgramVersion(3, 7, 0, 0, ""))
    {
        newLlvmSyntax = true;
    }
}

void Emitter::RequestLabelFor(std::shared_ptr<GenData> genData)
{
    labelRequestSet.insert(genData);
}

void Emitter::RemoveLabelRequestFor(std::shared_ptr<GenData> genData)
{
    labelRequestSet.erase(genData);
}

void Emitter::Emit(Ir::Intf::Instruction* instruction)
{
    Ir::Intf::LabelObject* label = nullptr;
    if ((!labelRequestSet.empty() || !nextInstructionLabels.empty()) && !instruction->GetLabel())
    {
        if (gotoTargetLabel)
        {
            label = gotoTargetLabel;
        }
        else
        {
            label = Cm::IrIntf::CreateNextLocalLabel();
            Own(label);
        }
        if (!nextInstructionLabels.empty())
        {
            for (Ir::Intf::LabelObject* nextInstructionLabel : nextInstructionLabels)
            {
                nextInstructionLabel->Set(label);
            }
        }
        if (!profilingHandler || !instruction->IsRet())
        {
            instruction->SetLabel(label);
        }
        for (std::shared_ptr<GenData> genData : labelRequestSet)
        {
            genData->SetLabel(label);
        }
        labelRequestSet.clear();
        nextInstructionLabels.clear();
        gotoTargetLabel = nullptr;
    }
    if (!nextInstructionComment.empty())
    {
        instruction->SetComment(nextInstructionComment);
        nextInstructionComment.clear();
    }
    if (cDebugNode)
    {
        instruction->SetCDebugNode(cDebugNode);
        cDebugNode = nullptr;
    }
    if (profilingHandler && instruction->IsRet())
    {
        profilingHandler->EmitEndProfiledFun(label);
    }
    if (newLlvmSyntax && (instruction->IsLoadInst() || instruction->IsGetElementPtrInst()))
    {
        Ir::Intf::Type* ptrType = nullptr; 
        Llvm::LoadInst* loadInst = nullptr;
        Llvm::GetElementPtrInst* getElementPtrInst = nullptr;
        if (instruction->IsLoadInst())
        { 
            loadInst = static_cast<Llvm::LoadInst*>(instruction);
            ptrType = loadInst->Type();
        }
        else if (instruction->IsGetElementPtrInst())
        {
            getElementPtrInst = static_cast<Llvm::GetElementPtrInst*>(instruction);
            ptrType = getElementPtrInst->PtrType();
        }
        int numPointers = ptrType->NumPointers();
        Ir::Intf::Type* itemType = nullptr;
        if (numPointers > 1)
        {
            itemType = Cm::IrIntf::Pointer(ptrType->GetBaseType(), numPointers - 1);
        }
        else
        {
            itemType = ptrType->GetBaseType();
        }
        Own(itemType);
        if (loadInst)
        {
            loadInst->SetItemType(itemType);
        }
        else if (getElementPtrInst)
        {
            getElementPtrInst->SetItemType(itemType);
        }
    }
    irFunction->AddInstruction(instruction);
    if (instruction->IsRet())
    {
        if (activeCfgNode && activeCfgNode->Kind() == CfgNodeKind::regularNode)
        {
            activeCfgNode->SetKind(CfgNodeKind::exitNode);
        }
    }
}

void Emitter::Own(Ir::Intf::Object* object)
{
    if (object)
    {
        if (!object->Owned())
        {
            object->SetOwned();
            ownedObjects.push_back(std::unique_ptr<Ir::Intf::Object>(object));
        }
    }
}

void Emitter::Own(Ir::Intf::Type* type)
{
    if (type)
    {
        if (!type->Owned())
        {
            type->SetOwned();
            ownedTypes.push_back(std::unique_ptr<Ir::Intf::Type>(type));
        }
    }
}

void Emitter::Own(Ir::Intf::Function* fun)
{
    ownedFuns.push_back(std::unique_ptr<Ir::Intf::Function>(fun));
}

GenResult::GenResult() : emitter(nullptr), flags(), genData(new GenData())
{
}

GenResult::GenResult(Emitter* emitter_, GenFlags flags_) : emitter(emitter_), flags(flags_), genData(new GenData())
{
    emitter->RequestLabelFor(genData);
} 

GenResult::~GenResult()
{
    if (emitter)
    {
        emitter->RemoveLabelRequestFor(genData);
    }
}

void GenResult::AddChild(std::shared_ptr<GenData> child)
{
    children.push_back(child);
}

std::shared_ptr<GenData> GenResult::GetChild(int index) 
{ 
    if (int(children.size()) <= index)
    {
        throw std::runtime_error("child access out of bounds");
    }
    return children[index]; 
}

void GenResult::Merge(std::shared_ptr<GenResult> child, bool insertFirst)
{
    genData->MergeData(child->genData);
    if (insertFirst)
    {
        children.insert(children.begin(), child->genData);
    }
    else
    {
        AddChild(std::move(child->genData));
    }
}

void GenResult::Merge(std::shared_ptr<GenResult> child)
{
    genData->MergeData(child->genData);
    AddChild(child->genData);
}

Ir::Intf::LabelObject* GenResult::GetLabel() const 
{ 
    Ir::Intf::LabelObject* label = genData->GetLabel();
    if (label)
    {
        return label;
    }
    for (std::shared_ptr<GenData> child : children)
    {
        Ir::Intf::LabelObject* label = child->GetLabel();
        if (label)
        {
            return label;
        }
    }
    return nullptr;
}

void GenResult::SetLabel(Ir::Intf::LabelObject* label)
{
    if (emitter)
    {
        emitter->RemoveLabelRequestFor(genData);
    }
    genData->SetLabel(label);
}

void GenResult::SetMainObject(Cm::Sym::TypeSymbol* type, Cm::Sym::TypeRepository& typeRepository)
{
    if (type->IsRvalueRefType())
    {
        Cm::Sym::TypeSymbol* plainType = typeRepository.MakePlainType(type);
        if (plainType->IsBasicTypeSymbol() || plainType->IsEnumTypeSymbol() || plainType->IsPointerType() || plainType->IsDelegateTypeSymbol())
        {
            Ir::Intf::Type* rvalueRefType = Cm::IrIntf::RvalueRef(plainType->GetIrType());
            emitter->Own(rvalueRefType);
            Ir::Intf::RegVar* temp = Cm::IrIntf::CreateTemporaryRegVar(rvalueRefType);
            emitter->Own(temp);
            genData->SetMainObject(temp);
            return;
        }
    }
    Ir::Intf::RegVar* temp = Cm::IrIntf::CreateTemporaryRegVar(type->GetIrType()->Clone());
    emitter->Own(temp);
    genData->SetMainObject(temp);
}

} } // namespace Cm::Core
