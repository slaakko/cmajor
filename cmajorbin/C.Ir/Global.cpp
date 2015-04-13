/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <C.Ir/Global.hpp>
#include <C.Ir/Type.hpp>
#include <C.Ir/Function.hpp>
#include <C.Ir/RegVar.hpp>
#include <C.Ir/StackVar.hpp>
#include <C.Ir/MemberVar.hpp>
#include <C.Ir/RefVar.hpp>
#include <C.Ir/Instruction.hpp>
#include <C.Ir/Constant.hpp>
#include <stdexcept>

namespace C {

Global::Global(const std::string& name_, Ir::Intf::Type* pointerType_) : Ir::Intf::Global(name_, pointerType_)
{
}

void Global::InitTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->InitFrom(emitter, type, *this);
}

void Global::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    emitter.Emit(Store(type, &constant, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void Global::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    emitter.Emit(Store(type, &global, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void Global::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    emitter.Emit(Store(type, &regVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void Global::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    emitter.Emit(Store(type, &stackVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void Global::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    emitter.Emit(Store(type, &memberVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void Global::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    emitter.Emit(Store(type, &refVar, this, Ir::Intf::Indirection::deref, Ir::Intf::Indirection::none));
}

void Global::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot init global from parameter");
}

void Global::AssignTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->AssignFrom(emitter, type, *this);
}

void Global::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    emitter.Emit(Store(type, &constant, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void Global::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    emitter.Emit(Store(type, &global, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void Global::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    emitter.Emit(Store(type, &regVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void Global::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    emitter.Emit(Store(type, &stackVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void Global::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    emitter.Emit(Store(type, &memberVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
}

void Global::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    emitter.Emit(Store(type, &refVar, this, Ir::Intf::Indirection::deref, Ir::Intf::Indirection::none));
}

void Global::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot assign global from parameter");
}

Ir::Intf::Object* Global::CreateAddr(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type)
{
    return this;
}

Ir::Intf::Global* CreateGlobal(const std::string& name, Ir::Intf::Type* pointerType)
{
    return new Global(name, pointerType);
}

} // namespace Llvm
