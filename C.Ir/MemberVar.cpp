/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <C.Ir/MemberVar.hpp>
#include <C.Ir/RegVar.hpp>
#include <C.Ir/Constant.hpp>
#include <C.Ir/Type.hpp>
#include <C.Ir/Function.hpp>
#include <C.Ir/Global.hpp>
#include <C.Ir/StackVar.hpp>
#include <C.Ir/RefVar.hpp>
#include <C.Ir/Instruction.hpp>

namespace C {

MemberVar::MemberVar(const std::string& name_, Ir::Intf::Object* ptr_, int index_, Ir::Intf::Type* type_) : 
    Ir::Intf::MemberVar(ptr_->Name() + "->" + name_, ptr_, index_, type_), memberName(name_)
{
}

void MemberVar::SetDotMember()
{
    SetName(Ptr()->Name() + "." + memberName);
}

void MemberVar::InitTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->InitFrom(emitter, type, *this);
}

void MemberVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    emitter.Emit(Store(type, &constant, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void MemberVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    emitter.Emit(Store(type, &global, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void MemberVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    Ir::Intf::Type* ptrType = MakePointerType(type, 1);
    emitter.Own(ptrType);
    if (regVar.GetType()->IsRvalueRefType() && TypesEqual(regVar.GetType(), ptrType))
    {
        Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
        emitter.Own(value);
        emitter.Emit(Load(ptrType, value, &regVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
        emitter.Emit(Store(type, value, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
    }
    else
    {
        emitter.Emit(Store(type, &regVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
    }
}

void MemberVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    emitter.Emit(Store(type, &stackVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void MemberVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    emitter.Emit(Store(type, &memberVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void MemberVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    emitter.Emit(Store(type, &refVar, this, Ir::Intf::Indirection::deref, Ir::Intf::Indirection::none));
}

void MemberVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    emitter.Emit(Store(type, &parameter, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void MemberVar::AssignTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->AssignFrom(emitter, type, *this);
}

void MemberVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    emitter.Emit(Store(type, &constant, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void MemberVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    emitter.Emit(Store(type, &global, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void MemberVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    if (regVar.GetType()->IsRvalueRefType() && TypesEqual(regVar.GetType(), ptrType))
    {
        Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
        emitter.Own(value);
        emitter.Emit(Load(ptrType, value, &regVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
        emitter.Emit(Store(type, value, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
    }
    else
    {
        emitter.Emit(Store(type, &regVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
    }
}

void MemberVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    emitter.Emit(Store(type, &stackVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void MemberVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    emitter.Emit(Store(type, &memberVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void MemberVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    emitter.Emit(Store(type, &refVar, this, Ir::Intf::Indirection::deref, Ir::Intf::Indirection::none));
}

void MemberVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    emitter.Emit(Store(type, &parameter, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

Ir::Intf::Object* MemberVar::CreateAddr(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* addr = CreateTemporaryRegVar(ptrType);
    emitter.Own(addr);
    emitter.Emit(Load(GetType(), addr, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::addr));
    return addr;
}

} // namespace C
