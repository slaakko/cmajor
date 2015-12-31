/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Llvm.Ir/RefVar.hpp>
#include <Llvm.Ir/Constant.hpp>
#include <Llvm.Ir/RegVar.hpp>
#include <Llvm.Ir/StackVar.hpp>
#include <Llvm.Ir/MemberVar.hpp>
#include <Llvm.Ir/Global.hpp>
#include <Llvm.Ir/Function.hpp>
#include <Llvm.Ir/Instruction.hpp>
#include <stdexcept>

namespace Llvm { 

RefVar::RefVar(const std::string& name_, Ir::Intf::Type* pointerType_): Ir::Intf::RefVar("%" + name_, pointerType_)
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
    emitter.Emit(Store(type, &global, this));
}

void RefVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    if (TypesEqual(GetType(), ptrType))
    {
        emitter.Emit(Store(type, &regVar, this));
    }
    else
    {
        Ir::Intf::RegVar* reg = CreateTemporaryRegVar(ptrType);
        emitter.Own(reg);
        emitter.Emit(Load(type, reg, this));
        emitter.Emit(Store(type, &regVar, reg));
    }
}

void RefVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    emitter.Emit(Store(type, &stackVar, this));
}

void RefVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
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
    emitter.Emit(Store(type, ptr, this));
}

void RefVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* refReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(refReg);
    emitter.Emit(Load(ptrType, refReg, &refVar));
    emitter.Emit(Store(type, refReg, this));
}

void RefVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    emitter.Emit(Store(type, &parameter, this));
}

void RefVar::AssignTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->AssignFrom(emitter, type, *this);
}

void RefVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* reg = CreateTemporaryRegVar(ptrType);
    emitter.Own(reg);
    emitter.Emit(Load(type, reg, this));
    emitter.Emit(Store(type, &constant, reg));
}

void RefVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* reg = CreateTemporaryRegVar(type);
    emitter.Own(reg);
    emitter.Emit(Load(ptrType, reg, &global));
    Ir::Intf::RegVar* ptr = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptr);
    emitter.Emit(Load(type, ptr, this));
    emitter.Emit(Store(type, reg, ptr));
}

void RefVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* reg = CreateTemporaryRegVar(ptrType);
    emitter.Own(reg);
    PointerType* ptrPtrType = MakePointerType(ptrType);
    emitter.Own(ptrPtrType);
    emitter.Emit(Load(ptrPtrType, reg, this));
    emitter.Emit(Store(type, &regVar, reg));
}

void RefVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* reg = CreateTemporaryRegVar(type);
    emitter.Own(reg);
    emitter.Emit(Load(type, reg, &stackVar));
    Ir::Intf::RegVar* ptr = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptr);
    emitter.Emit(Load(type, ptr, this));
    emitter.Emit(Store(type, reg, ptr));
}

void RefVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
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
    Ir::Intf::RegVar* refPtr = CreateTemporaryRegVar(ptrType);
    emitter.Own(refPtr);
    emitter.Emit(Load(ptrType, refPtr, this));
    emitter.Emit(Store(type, value, refPtr));
}

void RefVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* refReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(refReg);
    emitter.Emit(Load(ptrType, refReg, &refVar));
    Ir::Intf::RegVar* reg = CreateTemporaryRegVar(type);
    emitter.Own(reg);
    emitter.Emit(Load(type, reg, refReg));
    Ir::Intf::RegVar* ptr = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptr);
    emitter.Emit(Load(ptrType, ptr, this));
    emitter.Emit(Store(type, reg, ptr));
}

void RefVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot assign reference variable from parameter");
}

Ir::Intf::Object* RefVar::CreateAddr(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* refReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(refReg);
    emitter.Emit(Load(ptrType, refReg, this));
    return refReg;
}

} // namespace Llvm
