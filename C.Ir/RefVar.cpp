/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <C.Ir/RefVar.hpp>
#include <C.Ir/Constant.hpp>
#include <C.Ir/RegVar.hpp>
#include <C.Ir/StackVar.hpp>
#include <C.Ir/MemberVar.hpp>
#include <C.Ir/Global.hpp>
#include <C.Ir/Function.hpp>
#include <C.Ir/Instruction.hpp>
#include <stdexcept>

namespace C {

RefVar::RefVar(const std::string& name_, Ir::Intf::Type* pointerType_) : Ir::Intf::RefVar("_F_" + name_, pointerType_)
{
}

void RefVar::InitTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->InitFrom(emitter, type, *this);
}

void RefVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    throw std::runtime_error("cannot init refvar from constant");
}

void RefVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    emitter.Emit(Load(type, this, &global, Ir::Intf::Indirection::none, Ir::Intf::Indirection::addr));
}

void RefVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    if (TypesEqual(GetType(), ptrType))
    {
        emitter.Emit(Load(type, this, &regVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::addr));
    }
    else
    {
        emitter.Emit(Load(type, this, &regVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
    }
}

void RefVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    emitter.Emit(Load(type, this, &stackVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::addr));
}

void RefVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    emitter.Emit(Load(type, this, &memberVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::addr));
}

void RefVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    emitter.Emit(Load(type, this, &refVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void RefVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    if (TypesEqual(GetType(), ptrType))
    {
        emitter.Emit(Load(type, this, &parameter, Ir::Intf::Indirection::none, Ir::Intf::Indirection::addr));
    }
    else
    {
        emitter.Emit(Load(type, this, &parameter, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
    }
}

void RefVar::AssignTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->AssignFrom(emitter, type, *this);
}

void RefVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    emitter.Emit(Store(type, &constant, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
}

void RefVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    emitter.Emit(Store(type, &global, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
}

void RefVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    emitter.Emit(Store(type, &regVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
}

void RefVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    emitter.Emit(Store(type, &stackVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
}

void RefVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    emitter.Emit(Store(type, &memberVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
}

void RefVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    emitter.Emit(Store(type, &refVar, this, Ir::Intf::Indirection::deref, Ir::Intf::Indirection::deref));
}

void RefVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot assign reference variable from parameter");
}

Ir::Intf::Object* RefVar::CreateAddr(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type)
{
    return this;
}

} // namespace C
