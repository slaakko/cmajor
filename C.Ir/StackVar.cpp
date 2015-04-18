/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <C.Ir/StackVar.hpp>
#include <C.Ir/Function.hpp>
#include <C.Ir/Constant.hpp>
#include <C.Ir/Global.hpp>
#include <C.Ir/RegVar.hpp>
#include <C.Ir/RefVar.hpp>
#include <C.Ir/MemberVar.hpp>
#include <C.Ir/Instruction.hpp>

namespace C {

StackVar::StackVar(const std::string& name_, Ir::Intf::Type* pointerType_) : Ir::Intf::StackVar("_V_" + name_, pointerType_)
{
}

void StackVar::InitTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->InitFrom(emitter, type, *this);
}

void StackVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    emitter.Emit(Store(type, &constant, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void StackVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    emitter.Emit(Store(type, &global, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void StackVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    if (TypesEqual(regVar.GetType(), ptrType))
    {
        emitter.Emit(Store(type, &regVar, this, Ir::Intf::Indirection::deref, Ir::Intf::Indirection::none));
    }
    else
    {
        emitter.Emit(Store(type, &regVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
    }
}

void StackVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    PointerType* ptrType = MakePointerType(stackVar.GetType());
    emitter.Own(ptrType);
    if (TypesEqual(GetType(), ptrType))
    {
        emitter.Emit(Load(type, this, &stackVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::addr));
    }
    else
    {
        emitter.Emit(Store(type, &stackVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
    }
}

void StackVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    emitter.Emit(Store(type, &memberVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void StackVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    emitter.Emit(Store(type, &refVar, this, Ir::Intf::Indirection::deref, Ir::Intf::Indirection::none));
}

void StackVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    emitter.Emit(Store(type, &parameter, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void StackVar::AssignTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->AssignFrom(emitter, type, *this);
}

void StackVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    emitter.Emit(Store(type, &constant, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void StackVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    emitter.Emit(Store(type, &global, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void StackVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    emitter.Emit(Store(type, &regVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void StackVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    PointerType* ptrType = MakePointerType(stackVar.GetType());
    emitter.Own(ptrType);
    if (TypesEqual(GetType(), ptrType))
    {
        emitter.Emit(Store(type, &stackVar, this, Ir::Intf::Indirection::deref, Ir::Intf::Indirection::none));
    }
    else
    {
        emitter.Emit(Store(type, &stackVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
    }
}

void StackVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    emitter.Emit(Store(type, &memberVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void StackVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    emitter.Emit(Store(type, &refVar, this, Ir::Intf::Indirection::deref, Ir::Intf::Indirection::none));
}

void StackVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    emitter.Emit(Store(type, &parameter, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

Ir::Intf::Object* StackVar::CreateAddr(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type)
{
    return this;
}

} // namespace C
