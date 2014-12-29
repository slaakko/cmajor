/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Llvm.Ir/RegVar.hpp>
#include <Llvm.Ir/Constant.hpp>
#include <Llvm.Ir/Global.hpp>
#include <Llvm.Ir/Function.hpp>
#include <Llvm.Ir/RefVar.hpp>
#include <Llvm.Ir/StackVar.hpp>
#include <Llvm.Ir/MemberVar.hpp>
#include <Llvm.Ir/Temporary.hpp>
#include <Llvm.Ir/Util.hpp>
#include <Llvm.Ir/Instruction.hpp>
#include <stdexcept>

namespace Llvm { 

RegVar::RegVar(const std::string& name_, Ir::Intf::Type* type_): Ir::Intf::RegVar("%" + name_, type_)
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
        emitter.Emit(Store(type, &constant, this));
    }
    else
    {
        Ir::Intf::Object* defaultValue = type->CreateDefaultValue();
        emitter.Own(defaultValue);
        emitter.Emit(Or(type, this, &constant, defaultValue)); 
    }
}

void RegVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    emitter.Emit(Load(ptrType, this, &global));
}

void RegVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    if (regVar.GetType()->IsRvalueRefType() && TypesEqual(regVar.GetType(), ptrType))
    {
        Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
        emitter.Own(value);
        emitter.Emit(Load(ptrType, value, &regVar));
        emitter.Emit(Store(type, value, this));
    }
    else if (TypesEqual(regVar.GetType(), ptrType))
    {
        emitter.Emit(Load(ptrType, this, &regVar));
    }
    else if (TypesEqual(GetType(), ptrType))
    {
        emitter.Emit(Store(type, &regVar, this));
    }
    else
    {
        SetName(regVar.Name());
    }
}

void RegVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    emitter.Emit(Load(ptrType, this, &stackVar));
}

void RegVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    PointerType* ptrType = MakePointerType(memberVar.GetType());
    emitter.Own(ptrType);
    if (TypesEqual(type, ptrType))
    {
        Assign(emitter, type, memberVar.CreateAddr(emitter, memberVar.GetType()), this);
    }
    else
    {
        PointerType* ptrType = MakePointerType(type);
        emitter.Own(ptrType);
        Ir::Intf::RegVar* ptr = CreateTemporaryRegVar(ptrType);
        emitter.Own(ptr);
        emitter.Emit(GetElementPtr(memberVar.Ptr()->GetType(), ptr, memberVar.Ptr(), CreateI32Constant(0), CreateI32Constant(memberVar.Index())));
        emitter.Emit(Load(ptrType, this, ptr));
    }
}

void RegVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    if (TypesEqual(ptrType, refVar.GetType()))
    {
        emitter.Emit(Load(ptrType, this, &refVar));
    }
    else
    {
        Ir::Intf::RegVar* refReg = CreateTemporaryRegVar(ptrType);
        emitter.Own(refReg);
        emitter.Emit(Load(ptrType, refReg, &refVar));
        emitter.Emit(Load(type, this, refReg));
    }
}

void RegVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot init register variable from parameter");
}

void RegVar::AssignTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->AssignFrom(emitter, type, *this);
}

void RegVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    if (GetType()->IsPointerType())
    {
        emitter.Emit(Store(type, &constant, this));
    }
    else
    {
        Ir::Intf::Object* defaultValue = type->CreateDefaultValue();
        emitter.Own(defaultValue);
        emitter.Emit(Or(type, this, &constant, defaultValue));
    }
}

void RegVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    emitter.Emit(Load(ptrType, this, &global));
}

void RegVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    if (regVar.GetType()->IsRvalueRefType() && TypesEqual(regVar.GetType(), ptrType))
    {
        Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
        emitter.Own(value);
        emitter.Emit(Load(ptrType, value, &regVar));
        emitter.Emit(Store(type, value, this));
    }
    else if (TypesEqual(regVar.GetType(), ptrType))
    {
        emitter.Emit(Load(ptrType, this, &regVar));
    }
    else if (TypesEqual(GetType(), ptrType))
    {
        emitter.Emit(Store(type, &regVar, this));
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
    emitter.Emit(Load(ptrType, this, &stackVar));
}

void RegVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* ptr = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptr);
    emitter.Emit(GetElementPtr(memberVar.Ptr()->GetType(), ptr, memberVar.Ptr(), CreateI32Constant(0), CreateI32Constant(memberVar.Index())));
    emitter.Emit(Load(ptrType, this, ptr));
}

void RegVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* refReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(refReg);
    PointerType* ptrPtrType = MakePointerType(ptrType);
    emitter.Own(ptrPtrType);
    emitter.Emit(Load(ptrPtrType, refReg, &refVar));
    emitter.Emit(Load(ptrType, this, refReg));
}

void RegVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot assign register variable from parameter");
}

Ir::Intf::Object* RegVar::CreateAddr(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type)
{
    return this;
}

Ir::Intf::RegVar* CreateRegVar(const std::string& name,  Ir::Intf::Type* type)
{
    return new RegVar(name, type);
}

Ir::Intf::RegVar* CreateTemporaryRegVar(Ir::Intf::Type* type)
{
    return new RegVar("%", type);
}

} // namespace Llvm
