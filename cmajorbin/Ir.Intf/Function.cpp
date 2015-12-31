/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Ir.Intf/Function.hpp>

namespace Ir { namespace Intf {

TempTypedefProvider* currentTempTypedefProvider = nullptr;

TempTypedefProvider* GetCurrentTempTypedefProvider()
{
    return currentTempTypedefProvider;
}

void SetCurrentTempTypedefProvider(TempTypedefProvider* tempTypedefProvider)
{
    currentTempTypedefProvider = tempTypedefProvider;
}

Function::Function(const std::string& name_, Type* returnType_, const std::vector<Parameter*>& parameters_): 
    name(name_), returnType(returnType_), parameters(parameters_), instNumber(0)
{
    for (Parameter* parameter : parameters)
    {
        if (!parameter->Owned())
        {
            parameter->SetOwned();
            ownedParameters.push_back(std::unique_ptr<Parameter>(parameter));
        }
    }
}

Function::~Function()
{
}

int Function::GetNextInstNumber() 
{
    return ++instNumber;
}

void Function::DoAddInstruction(Instruction* instruction)
{
    instructions.push_back(instruction);
}

void Function::Own(Instruction* instruction)
{
    ownedInstructions.push_back(std::unique_ptr<Instruction>(instruction));
}

void Function::Own(Object* object)
{
    ownedObjects.push_back(std::unique_ptr<Object>(object));
}

void Function::Own(Function* fun)
{
    ownedFunctions.push_back(std::unique_ptr<Function>(fun));
}

void Function::SetLastInstructionLabel(LabelObject* label)
{
    instructions.back()->SetLabel(label);
}

void Function::Clean()
{
    std::set<std::string> targetLabels;
    for (Instruction* inst : instructions)
    {
        inst->AddTargetLabels(targetLabels);
    }
    Instruction* prev = nullptr;
    Instruction* prevPrev = nullptr;
    for (Instruction* inst : instructions)
    {
        LabelObject* label = inst->GetLabel();
        if (label)
        {
            if (!prev || prev && !prev->IsTerminator())
            {
                if (targetLabels.find(label->Name()) == targetLabels.end()) // if label not used
                {
                    inst->RemoveLabel(); // remove the label
                }
            }
            if (prevPrev && prevPrev->IsTerminator() && prev && prev->IsUnconditionalBr() && !prev->GetLabel()) // if previous br begins a basic block and it has no label...
            {
                LabelObject* prevTarget = prev->GetTargetLabel();
                if (prevTarget && prevTarget->Name() == label->Name()) // ...and previous br jumps to this inst
                {
                    prev->Remove(); // remove the br
                }
            }
        }
        prevPrev = prev;
        prev = inst;
    }
    std::set<std::string> existingTargets;
    for (Instruction* inst : instructions)
    {
        LabelObject* label = inst->GetLabel();
        if (label)
        {
            existingTargets.insert(label->Name());
        }
    }
    for (Instruction* inst : instructions)
    {
        LabelObject* target = inst->GetTargetLabel();
        if (target && existingTargets.find(target->Name()) == existingTargets.end()) // if br target not found
        {
            inst->Remove(); // remove this br
        }
    }
}

void Function::Release()
{
    instructions.clear();
    ownedInstructions.clear();
    ownedObjects.clear();
    ownedFunctions.clear();
}

void Function::WriteInlineDefinition(CodeFormatter& formatter)
{ 
    WriteDefinition(formatter, true, true); 
}


bool Function::LastInstructionIsRet() const
{
    if (!instructions.empty())
    {
        return instructions.back()->IsRet();
    }
    return false;
}

} } // namespace Ir::Intf
