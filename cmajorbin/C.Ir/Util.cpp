/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <C.Ir/Util.hpp>
#include <C.Ir/RegVar.hpp>
#include <C.Ir/Type.hpp>
#include <C.Ir/Instruction.hpp>
#include <C.Ir/Constant.hpp>
#include <Ir.Intf/Factory.hpp>

namespace C {

void Init(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* from, Ir::Intf::Object* to)
{
    from->InitTo(emitter, type, to);
}

void Assign(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* from, Ir::Intf::Object* to)
{
    from->AssignTo(emitter, type, to);
}

Ir::Intf::Object* SizeOf(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type)
{
    Ir::Intf::Object* result = CreateTemporaryRegVar(I64());
    emitter.Own(result);
    emitter.Emit(CreateSizeOf(result, type));
    return result;
}

} // namespace C
