/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <C.Ir/RegVar.hpp>
#include <C.Ir/Constant.hpp>
#include <C.Ir/Global.hpp>
#include <C.Ir/Function.hpp>
#include <C.Ir/RefVar.hpp>
#include <C.Ir/StackVar.hpp>
#include <C.Ir/MemberVar.hpp>
#include <C.Ir/Util.hpp>
#include <C.Ir/Instruction.hpp>
#include <Ir.Intf/Factory.hpp>
#include <stdexcept>

namespace C {

RegVar::RegVar(const std::string& name_, Ir::Intf::Type* type_) : Ir::Intf::RegVar("_R_" + name_, type_)
{
}

void RegVar::InitTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->InitFrom(emitter, type, *this);
}

void RegVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    if (GetType()->IsPointerType())
    {
        emitter.Emit(Store(type, &constant, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
    }
    else
    {
        emitter.Emit(Set(&constant, this));
    }
}

void RegVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    Ir::Intf::Type* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    if (global.GetType()->IsTypeName())
    {
        emitter.Emit(Load(ptrType, this, &global, Ir::Intf::Indirection::none, Ir::Intf::Indirection::addr));
    }
    else
    {
        emitter.Emit(Load(ptrType, this, &global, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
    }
}

void RegVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    Ir::Intf::Type* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    if (regVar.GetType()->IsRvalueRefType() && TypesEqual(regVar.GetType(), ptrType))
    {
        Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
        emitter.Own(value);
        emitter.Emit(Load(ptrType, value, &regVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
        emitter.Emit(Store(type, value, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
    }
    else if (TypesEqual(regVar.GetType(), ptrType))
    {
        emitter.Emit(Load(ptrType, this, &regVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
    }
    else if (TypesEqual(GetType(), ptrType))
    {
        emitter.Emit(Store(type, &regVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
    }
    else
    {
        SetName(regVar.Name());
    }
}

void RegVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    Ir::Intf::Type* stackVarPtrType = MakePointerType(stackVar.GetType());
    emitter.Own(stackVarPtrType);
    Ir::Intf::Type* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    if (TypesEqual(GetType(), stackVarPtrType))
    {
        emitter.Emit(Load(ptrType, this, &stackVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::addr));
    }
    else
    {
        emitter.Emit(Load(ptrType, this, &stackVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
    }
}

void RegVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    Ir::Intf::Type* memberVarPtrType = MakePointerType(memberVar.GetType());
    emitter.Own(memberVarPtrType);
    Ir::Intf::Type* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    if (TypesEqual(GetType(), memberVarPtrType))
    {
        emitter.Emit(Load(ptrType, this, &memberVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::addr));
    }
    else
    {
        emitter.Emit(Load(ptrType, this, &memberVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
    }
}

void RegVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    if (TypesEqual(GetType(), refVar.GetType()))
    {
        emitter.Emit(Load(ptrType, this, &refVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
    }
    else
    {
        emitter.Emit(Load(type, this, &refVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
    }
}

void RegVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    emitter.Emit(Load(ptrType, this, &parameter, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void RegVar::AssignTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->AssignFrom(emitter, type, *this);
}

void RegVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    if (GetType()->IsPointerType())
    {
        emitter.Emit(Store(type, &constant, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
    }
    else
    {
        emitter.Emit(Set(&constant, this));
    }
}

void RegVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    emitter.Emit(Load(ptrType, this, &global, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void RegVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    if (regVar.GetType()->IsRvalueRefType() && TypesEqual(regVar.GetType(), ptrType))
    {
        Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
        emitter.Own(value);
        emitter.Emit(Load(ptrType, value, &regVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
        emitter.Emit(Store(type, value, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
    }
    else if (TypesEqual(regVar.GetType(), ptrType))
    {
        emitter.Emit(Load(ptrType, this, &regVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
    }
    else if (TypesEqual(GetType(), ptrType))
    {
        emitter.Emit(Store(type, &regVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
    }
    else
    {
        SetName(regVar.Name());
    }
}

void RegVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    emitter.Emit(Load(ptrType, this, &stackVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void RegVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    emitter.Emit(Load(ptrType, this, &memberVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void RegVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    emitter.Emit(Load(ptrType, this, &refVar, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
}

void RegVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    emitter.Emit(Load(ptrType, this, &parameter, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

Ir::Intf::Object* RegVar::CreateAddr(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type)
{
    return this;
}

Ir::Intf::RegVar* CreateRegVar(const std::string& name, Ir::Intf::Type* type)
{
    return new RegVar(name, type);
}

Ir::Intf::RegVar* CreateTemporaryRegVar(Ir::Intf::Type* type)
{
    return new RegVar("?", type);
}

} // namespace C
