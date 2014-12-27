/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/Emitter.hpp>

namespace Cm { namespace Core {

Emitter::Emitter(Ir::Intf::Function* function_) : function(function_)
{
}

void Emitter::Emit(Ir::Intf::Instruction* instruction)
{
    if (!labelRequestSet.empty() || !nextInstructionLabels.empty())
    {
        Ir::Intf::LabelPtr label = nullptr;
        if (gotoTargetLabel)
        {
            label = gotoTargetLabel;
        }
        else
        {
            label = Cm::IrIntf::CreateNextLocalLabel();
            function->AddOwnedObject(label);
        }
        if (!nextInstructionLabels.empty())
        {
            for (Ir::Intf::LabelPtr nextInstructionLabel : nextInstructionLabels)
            {
                nextInstructionLabel->Set(label);
            }
        }
        instruction->SetLabel(label);
        for (GenDataPtr data : labelRequestSet)
        {
            data->SetLabel(label);
        }
        labelRequestSet.clear();
        nextInstructionLabels.clear();
        gotoTargetLabel = nullptr;
    }
    if (currentSourcePosMetadataNode)
    {
        instruction->SetSourcePosMetadataNode(currentSourcePosMetadataNode);
    }
    if (!nextInstructionComment.empty())
    {
        instruction->SetComment(nextInstructionComment);
        nextInstructionComment.clear();
    }
    if (cdebugNode)
    {
        instruction->SetCDebugNode(cdebugNode);
        cdebugNode = nullptr;
    }
    function->AddInstruction(instruction);
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

} } // namespace Cm::Core
