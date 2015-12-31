/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Llvm.Ir/Util.hpp>
#include <Llvm.Ir/RegVar.hpp>
#include <Llvm.Ir/Type.hpp>
#include <Llvm.Ir/Instruction.hpp>
#include <Llvm.Ir/Constant.hpp>
#include <Ir.Intf/Factory.hpp>

namespace Llvm { 

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
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::Object* ptrSize(CreateTemporaryRegVar(ptrType));
    emitter.Own(ptrSize);
    Ir::Intf::Object* null = Null(ptrType);
    emitter.Own(null);
    Ir::Intf::Object* one = CreateI64Constant(1);
    emitter.Own(one);
    emitter.Emit(GetElementPtr(ptrType, ptrSize, null, one));
    Ir::Intf::Object* size(CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI64()));
    emitter.Own(size);
    emitter.Emit(Ptrtoint(ptrType, size, ptrSize, Ir::Intf::GetFactory()->GetI64()));
    return size;
}

} // namespace Llvm
