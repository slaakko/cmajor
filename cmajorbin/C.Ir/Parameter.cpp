/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <C.Ir/Parameter.hpp>
#include <C.Ir/Function.hpp>
#include <C.Ir/Global.hpp>
#include <C.Ir/RegVar.hpp>
#include <C.Ir/StackVar.hpp>
#include <C.Ir/Instruction.hpp>
#include <C.Ir/Constant.hpp>
#include <stdexcept>

namespace C {

Parameter::Parameter(const std::string& name_, Ir::Intf::Type* type_) : Ir::Intf::Parameter("_P_" + name_, type_)
{
}

void Parameter::InitTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->InitFrom(emitter, type, *this);
}

void Parameter::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    throw std::runtime_error("cannot init parameter from constant");
}

void Parameter::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    throw std::runtime_error("cannot init parameter from global");
}

void Parameter::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    throw std::runtime_error("cannot init parameter from register variable");
}

void Parameter::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    throw std::runtime_error("cannot init parameter from stack variable");
}

void Parameter::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    throw std::runtime_error("cannot init parameter from member variable");
}

void Parameter::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    throw std::runtime_error("cannot init parameter from reference variabe");
}

void Parameter::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot init parameter from parameter");
}

void Parameter::AssignTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->AssignFrom(emitter, type, *this);
}

void Parameter::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    Ir::Intf::Type* ptrType = MakePointerType(constant.GetType(), 1);
    emitter.Own(ptrType);
    if (TypesEqual(GetType(), ptrType))
    {
        emitter.Emit(Store(type, &constant, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
    }
    else
    {
        emitter.Emit(Store(type, &constant, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
    }
}

void Parameter::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    Ir::Intf::Type* ptrType = MakePointerType(global.GetType(), 1);
    emitter.Own(ptrType);
    if (TypesEqual(GetType(), ptrType))
    {
        emitter.Emit(Store(type, &global, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
    }
    else
    {
        emitter.Emit(Store(type, &global, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
    }
}

void Parameter::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    Ir::Intf::Type* ptrType = MakePointerType(regVar.GetType(), 1);
    emitter.Own(ptrType);
    if (TypesEqual(GetType(), ptrType))
    {
        emitter.Emit(Store(type, &regVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
    }
    else
    {
        emitter.Emit(Store(type, &regVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
    }
}

void Parameter::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    Ir::Intf::Type* ptrType = MakePointerType(stackVar.GetType(), 1);
    emitter.Own(ptrType);
    if (TypesEqual(GetType(), ptrType))
    {
        emitter.Emit(Store(type, &stackVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::deref));
    }
    else
    {
        emitter.Emit(Store(type, &stackVar, this, Ir::Intf::Indirection::none, Ir::Intf::Indirection::none));
    }
}

void Parameter::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    throw std::runtime_error("cannot assign parameter from member variable");
}

void Parameter::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    throw std::runtime_error("cannot assign parameter from reference variable");
}

void Parameter::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot assign parameter from parameter");
}

Ir::Intf::Object* Parameter::CreateAddr(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type)
{
    return this;
}

Ir::Intf::Parameter* CreateParameter(const std::string& name, Ir::Intf::Type* type)
{
    return new Parameter(name, type);
}

} // namespace C
