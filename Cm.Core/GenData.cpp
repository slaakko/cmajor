/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/GenData.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <Ir.Intf/Label.hpp>

namespace Cm { namespace Core {

GenData::GenData() : labelHolder(new LabelHolder())
{
}

GenData::GenData(GenData&& that) : labelHolder(std::move(that.labelHolder)), objects(std::move(that.objects)), nextTargets(std::move(that.nextTargets)), trueTargets(std::move(that.trueTargets)), falseTargets(std::move(that.falseTargets))
{
}

GenData& GenData::operator=(GenData&& that)
{
    std::swap(labelHolder, that.labelHolder);
    std::swap(objects, that.objects);
    std::swap(nextTargets, that.nextTargets);
    std::swap(trueTargets, that.trueTargets);
    std::swap(falseTargets, that.falseTargets);
    return *this;
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

void GenData::MergeTargets(std::vector<Ir::Intf::LabelObject*>& targets, std::vector<Ir::Intf::LabelObject*>& fromTargets)
{
    Ir::Intf::Merge(targets, fromTargets);
    fromTargets.clear();
}

void GenData::MergeData(GenData& childData)
{
    if (!GetLabel() && childData.GetLabel())
    {
        labelHolder->SetLabel(childData.labelHolder->GetLabel());
    }
    if (!childData.IsEmpty())
    {
        objects.push_back(childData.MainObject());
    }
    Ir::Intf::Merge(nextTargets, childData.nextTargets);
    Ir::Intf::Merge(trueTargets, childData.trueTargets);
    Ir::Intf::Merge(falseTargets, childData.falseTargets);
}

void GenData::BackpatchTrueTargets(Ir::Intf::LabelObject* label)
{
    if (!label)
    {
        throw std::runtime_error("backpatch true targets got no label");
    }
    Ir::Intf::Backpatch(trueTargets, label);
    trueTargets.clear();
}

void GenData::BackpatchFalseTargets(Ir::Intf::LabelObject* label)
{
    if (!label)
    {
        throw std::runtime_error("backpatch false targets got no label");
    }
    Ir::Intf::Backpatch(falseTargets, label);
    falseTargets.clear();
}

void GenData::BackpatchNextTargets(Ir::Intf::LabelObject* label)
{
    if (!label)
    {
        throw std::runtime_error("backpatch next targets got no label");
    }
    Ir::Intf::Backpatch(nextTargets, label);
    nextTargets.clear();
}

Emitter::Emitter(Ir::Intf::Function* irFunction_) : irFunction(irFunction_), gotoTargetLabel(nullptr)
{
}

void Emitter::RequestLabelFor(GenData& genData)
{
    labelRequestSet.insert(genData.GetLabelHolder());
}

void Emitter::Emit(Ir::Intf::Instruction* instruction)
{
    if (!labelRequestSet.empty() || !nextInstructionLabels.empty())
    {
        Ir::Intf::LabelObject* label = nullptr;
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
        instruction->SetLabel(label);
        for (LabelHolder* labelHolder : labelRequestSet)
        {
            labelHolder->SetLabel(label);
        }
        labelRequestSet.clear();
        nextInstructionLabels.clear();
        gotoTargetLabel = nullptr;
    }
    /*
    if (currentSourcePosMetadataNode)
    {
    instruction->SetSourcePosMetadataNode(currentSourcePosMetadataNode);
    }
    */
    if (!nextInstructionComment.empty())
    {
        instruction->SetComment(nextInstructionComment);
        nextInstructionComment.clear();
    }
    /*
    if (cdebugNode)
    {
    instruction->SetCDebugNode(cdebugNode);
    cdebugNode = nullptr;
    }
    */
    irFunction->AddInstruction(instruction);
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

GenResult::GenResult(Emitter* emitter_, GenFlags flags_) : emitter(emitter_), flags(flags_)
{
    if (GetFlag(GenFlags::label, flags))
    {
        Ir::Intf::LabelObject* label = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(label);
        genData.GetLabelHolder()->SetLabel(label);
        emitter->RequestLabelFor(genData);
    }
}

GenResult::GenResult(GenResult&& that) : emitter(that.emitter), flags(that.flags), genData(std::move(that.genData)), children(std::move(that.children))
{
    that.emitter = nullptr;
}

GenResult& GenResult::operator=(GenResult&& that)
{
    std::swap(emitter, that.emitter);
    std::swap(flags, that.flags);
    std::swap(genData, that.genData);
    std::swap(children, that.children);
    return *this;
}

void GenResult::AddChild(GenData&& child)
{
    children.push_back(std::move(child));
}

GenData& GenResult::GetChild(int index) 
{ 
    if (int(children.size()) <= index)
    {
        throw std::runtime_error("child access out of bounds");
    }
    return children[index]; 
}

void GenResult::Merge(GenResult& child)
{
    genData.MergeData(child.genData);
    AddChild(std::move(child.genData));
}

void GenResult::SetMainObject(Cm::Sym::TypeSymbol* type)
{
    if (type->IsRvalueRefType())
    {
        Cm::Sym::TypeSymbol* baseType = type->GetBaseType();
        if (baseType->IsBasicTypeSymbol() || baseType->IsEnumTypeSymbol() || baseType->IsPointerType() || baseType->IsDelegateTypeSymbol())
        {
            Ir::Intf::Type* rvalueRefType = Cm::IrIntf::RvalueRef(baseType->GetIrType());
            emitter->Own(rvalueRefType);
            Ir::Intf::RegVarPtr temp = Cm::IrIntf::CreateTemporaryRegVar(rvalueRefType);
            emitter->Own(temp);
            genData.SetMainObject(temp);
            return;
        }
    }
    Ir::Intf::RegVar* temp = Cm::IrIntf::CreateTemporaryRegVar(type->GetIrType());
    emitter->Own(temp);
    genData.SetMainObject(temp);
}

} } // namespace Cm::Core
