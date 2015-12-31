/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Llvm.Ir/Parameter.hpp>
#include <Llvm.Ir/Function.hpp>
#include <Llvm.Ir/Global.hpp>
#include <Llvm.Ir/RegVar.hpp>
#include <Llvm.Ir/StackVar.hpp>
#include <Llvm.Ir/Instruction.hpp>
#include <Llvm.Ir/Constant.hpp>
#include <stdexcept>

namespace Llvm { 

Parameter::Parameter(const std::string& name_, Ir::Intf::Type* type_): Ir::Intf::Parameter("%" + name_, type_)
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
    emitter.Emit(Store(type, &constant, this));
}

void Parameter::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* reg = CreateTemporaryRegVar(type);
    emitter.Own(reg);
    emitter.Emit(Load(ptrType, reg, &global));
    emitter.Emit(Store(type, reg, this));
}

void Parameter::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    emitter.Emit(Store(type, &regVar, this));
}

void Parameter::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* reg = CreateTemporaryRegVar(type);
    emitter.Own(reg);
    emitter.Emit(Load(ptrType, reg, &stackVar));
    emitter.Emit(Store(type, reg, this));
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

} // namespace Llvm
