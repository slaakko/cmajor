/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Ir.Intf/Instruction.hpp>

namespace Ir { namespace Intf {

Instruction::Instruction(const std::string& name_): removed(false), name(name_), label(nullptr), sourcePosMetadataNode(nullptr), cdebugNode(nullptr), funCallNode(nullptr)
{
}

Instruction::~Instruction()
{
}

} } // namespace Ir::Intf
