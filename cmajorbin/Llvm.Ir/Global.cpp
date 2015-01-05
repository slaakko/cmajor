/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Llvm.Ir/Global.hpp>
#include <Llvm.Ir/Type.hpp>
#include <Llvm.Ir/Function.hpp>
#include <Llvm.Ir/RegVar.hpp>
#include <Llvm.Ir/StackVar.hpp>
#include <Llvm.Ir/MemberVar.hpp>
#include <Llvm.Ir/RefVar.hpp>
#include <Llvm.Ir/Instruction.hpp>
#include <Llvm.Ir/Constant.hpp>

namespace Llvm { 

Global::Global(const std::string& name_, Ir::Intf::Type* pointerType_): Ir::Intf::Global("@" + name_, pointerType_)
{
}

void Global::InitTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->InitFrom(emitter, type, *this);
}

void Global::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    emitter.Emit(Store(type, &constant, this));
}

void Global::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    Ir::Intf::RegVar* reg = CreateTemporaryRegVar(type);
    emitter.Own(reg);
    emitter.Emit(Load(type, reg, &global));
    emitter.Emit(Store(type, reg, this));
}

void Global::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    emitter.Emit(Store(type, &regVar, this));
}

void Global::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    Ir::Intf::RegVar* reg = CreateTemporaryRegVar(type);
    emitter.Own(reg);
    emitter.Emit(Load(type, reg, &stackVar));
    emitter.Emit(Store(type, reg, this));
}

void Global::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* ptr = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptr);
    Ir::Intf::Object* zero = CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* index = CreateI32Constant(memberVar.Index());
    emitter.Own(index);
    emitter.Emit(GetElementPtr(memberVar.Ptr()->GetType(), ptr, memberVar.Ptr(), zero, index));
    Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
    emitter.Own(value);
    emitter.Emit(Load(type, value, ptr));
    emitter.Emit(Store(type, value, this));
}

void Global::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* refReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(refReg);
    emitter.Emit(Load(ptrType, refReg, &refVar));
    Ir::Intf::RegVar* reg = CreateTemporaryRegVar(type);
    emitter.Own(reg);
    emitter.Emit(Load(type, reg, refReg));
    emitter.Emit(Store(type, reg, this));
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
    emitter.Emit(Store(type, &constant, this));
}

void Global::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* reg = CreateTemporaryRegVar(type);
    emitter.Own(reg);
    emitter.Emit(Load(ptrType, reg, &global));
    emitter.Emit(Store(type, reg, this));
}

void Global::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    emitter.Emit(Store(type, &regVar, this));
}

void Global::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    Ir::Intf::RegVar* reg = CreateTemporaryRegVar(type);
    emitter.Own(reg);
    emitter.Emit(Load(type, reg, &stackVar));
    emitter.Emit(Store(type, reg, this));
}

void Global::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* ptr = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptr);
    Ir::Intf::Object* zero = CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* index = CreateI32Constant(memberVar.Index());
    emitter.Own(index);
    emitter.Emit(GetElementPtr(memberVar.Ptr()->GetType(), ptr, memberVar.Ptr(), zero, index));
    Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
    emitter.Own(value);
    emitter.Emit(Load(type, value, ptr));
    emitter.Emit(Store(type, value, this));
}

void Global::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* refReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(refReg);
    emitter.Emit(Load(ptrType, refReg, &refVar));
    Ir::Intf::RegVar* reg = CreateTemporaryRegVar(type);
    emitter.Own(reg);
    emitter.Emit(Load(type, reg, refReg));
    emitter.Emit(Store(type, reg, this));
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
