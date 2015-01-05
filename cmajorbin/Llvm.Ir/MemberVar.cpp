/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Llvm.Ir/MemberVar.hpp>
#include <Llvm.Ir/RegVar.hpp>
#include <Llvm.Ir/Constant.hpp>
#include <Llvm.Ir/Type.hpp>
#include <Llvm.Ir/Function.hpp>
#include <Llvm.Ir/Global.hpp>
#include <Llvm.Ir/StackVar.hpp>
#include <Llvm.Ir/RefVar.hpp>
#include <Llvm.Ir/Instruction.hpp>

namespace Llvm { 

MemberVar::MemberVar(const std::string& name_, Ir::Intf::Object* ptr_, int index_, Ir::Intf::Type* type_): Ir::Intf::MemberVar(name_, ptr_, index_, type_)
{
}

void MemberVar::InitTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->InitFrom(emitter, type, *this);
}

void MemberVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* ptrReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptrReg);
    Ir::Intf::Object* zero = CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* index = CreateI32Constant(Index());
    emitter.Own(index);
    emitter.Emit(GetElementPtr(Ptr()->GetType(), ptrReg, Ptr(), zero, index));
    emitter.Emit(Store(type, &constant, ptrReg));
}

void MemberVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* ptrReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptrReg);
    Ir::Intf::Object* zero = CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* index = CreateI32Constant(Index());
    emitter.Own(index);
    emitter.Emit(GetElementPtr(Ptr()->GetType(), ptrReg, Ptr(), zero, index));
    if (global.GetType()->IsFunctionPtrPtrType())
    {
        emitter.Emit(Store(type, &global, ptrReg));
    }
    else
    {
        Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
        emitter.Own(value);
        emitter.Emit(Load(type, value, &global));
        emitter.Emit(Store(type, value, ptrReg));
    }
}

void MemberVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* ptrReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptrReg);
    Ir::Intf::Object* zero = CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* index = CreateI32Constant(Index());
    emitter.Own(index);
    emitter.Emit(GetElementPtr(Ptr()->GetType(), ptrReg, Ptr(), zero, index)); 
    if (regVar.GetType()->IsRvalueRefType() && TypesEqual(regVar.GetType(), ptrType))
    {
        Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
        emitter.Own(value);
        emitter.Emit(Load(ptrType, value, &regVar));
        emitter.Emit(Store(type, value, ptrReg));
    }
    else
    {
        emitter.Emit(Store(type, &regVar, ptrReg));
    }
}

void MemberVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* ptrReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptrReg);
    Ir::Intf::Object* zero = CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* index = CreateI32Constant(Index());
    emitter.Own(index);
    emitter.Emit(GetElementPtr(Ptr()->GetType(), ptrReg, Ptr(), zero, index));
    Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
    emitter.Own(value);
    emitter.Emit(Load(type, value, &stackVar));
    emitter.Emit(Store(type, value, ptrReg));
}

void MemberVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* ptrReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptrReg);
    Ir::Intf::Object* zero = CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* memberVarIndex = CreateI32Constant(memberVar.Index());
    emitter.Own(memberVarIndex);
    emitter.Emit(GetElementPtr(Ptr()->GetType(), ptrReg, memberVar.Ptr(), zero, memberVarIndex));
    Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
    emitter.Own(value);
    emitter.Emit(Load(ptrType, value, ptrReg));
    Ir::Intf::RegVar* ptrThis = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptrThis);
    Ir::Intf::Object* index = CreateI32Constant(Index());
    emitter.Own(index);
    emitter.Emit(GetElementPtr(Ptr()->GetType(), ptrThis, Ptr(), zero, index));
    emitter.Emit(Store(type, value, ptrThis));
}

void MemberVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* ptrReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptrReg);
    Ir::Intf::Object* zero = CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* index = CreateI32Constant(Index());
    emitter.Own(index);
    emitter.Emit(GetElementPtr(Ptr()->GetType(), ptrReg, Ptr(), zero, index));
    Ir::Intf::RegVar* refReg = CreateTemporaryRegVar(ptrType); 
    emitter.Own(refReg);
    emitter.Emit(Load(ptrType, refReg, &refVar)); 
    if (TypesEqual(GetType(), refVar.GetType()))
    {
        emitter.Emit(Store(type, refReg, ptrReg));
    }
    else
    {
        Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
        emitter.Own(value);
        emitter.Emit(Load(type, value, refReg));
        emitter.Emit(Store(type, value, ptrReg));
    }
}

void MemberVar::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* ptrReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptrReg);
    Ir::Intf::Object* zero = CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* index = CreateI32Constant(Index());
    emitter.Own(index);
    emitter.Emit(GetElementPtr(Ptr()->GetType(), ptrReg, Ptr(), zero, index));
    emitter.Emit(Store(type, &parameter, ptrReg));
}

void MemberVar::AssignTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->AssignFrom(emitter, type, *this);
}

void MemberVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* ptrReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptrReg);
    Ir::Intf::Object* zero = CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* index = CreateI32Constant(Index());
    emitter.Own(index);
    emitter.Emit(GetElementPtr(Ptr()->GetType(), ptrReg, Ptr(), zero, index));
    emitter.Emit(Store(type, &constant, ptrReg));
}

void MemberVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* ptrReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptrReg);
    Ir::Intf::Object* zero = CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* index = CreateI32Constant(Index());
    emitter.Own(index);
    emitter.Emit(GetElementPtr(Ptr()->GetType(), ptrReg, Ptr(), zero, index));
    if (global.GetType()->IsFunctionPtrPtrType())
    {
        emitter.Emit(Store(type, &global, ptrReg));
    }
    else
    {
        Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
        emitter.Own(value);
        emitter.Emit(Load(ptrType, value, &global));
        emitter.Emit(Store(type, value, ptrReg));
    }
}

void MemberVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* ptrReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptrReg);
    Ir::Intf::Object* zero = CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* index = CreateI32Constant(Index());
    emitter.Own(index);
    emitter.Emit(GetElementPtr(Ptr()->GetType(), ptrReg, Ptr(), zero, index));
    if (regVar.GetType()->IsRvalueRefType() && TypesEqual(regVar.GetType(), ptrType))
    {
        Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
        emitter.Own(value);
        emitter.Emit(Load(ptrType, value, &regVar));
        emitter.Emit(Store(type, value, ptrReg));
    }
    else
    {
        emitter.Emit(Store(type, &regVar, ptrReg));
    }
}

void MemberVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* ptrReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptrReg);
    Ir::Intf::Object* zero = CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* index = CreateI32Constant(Index());
    emitter.Own(index);
    emitter.Emit(GetElementPtr(Ptr()->GetType(), ptrReg, Ptr(), zero, index));
    Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
    emitter.Own(value);
    emitter.Emit(Load(type, value, &stackVar));
    emitter.Emit(Store(type, value, ptrReg));
}

void MemberVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* ptrReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptrReg);
    Ir::Intf::Object* zero = CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* memberVarIndex = CreateI32Constant(memberVar.Index());
    emitter.Own(memberVarIndex);
    emitter.Emit(GetElementPtr(Ptr()->GetType(), ptrReg, memberVar.Ptr(), zero, memberVarIndex));
    Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
    emitter.Own(value);
    emitter.Emit(Load(ptrType, value, ptrReg));
    Ir::Intf::RegVar* ptrThis = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptrThis);
    Ir::Intf::Object* index = CreateI32Constant(Index());
    emitter.Own(index);
    emitter.Emit(GetElementPtr(Ptr()->GetType(), ptrThis, Ptr(), zero, index));
    emitter.Emit(Store(type, value, ptrThis));
}

void MemberVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* ptrReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptrReg);
    emitter.Emit(GetElementPtr(Ptr()->GetType(), ptrReg, Ptr(), CreateI32Constant(0), CreateI32Constant(Index())));
    Ir::Intf::RegVar* refReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(refReg);
    emitter.Emit(Load(ptrType, refReg, &refVar));
    Ir::Intf::RegVar* value = CreateTemporaryRegVar(type);
    emitter.Own(value);
    emitter.Emit(Load(type, value, refReg));
    emitter.Emit(Store(type, value, ptrReg));
}

void MemberVar::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* ptrReg = CreateTemporaryRegVar(ptrType);
    emitter.Own(ptrReg);
    Ir::Intf::Object* zero = CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* index = CreateI32Constant(Index());
    emitter.Own(index);
    emitter.Emit(GetElementPtr(Ptr()->GetType(), ptrReg, Ptr(), zero, index));
    emitter.Emit(Store(type, &parameter, ptrReg));
}

Ir::Intf::Object* MemberVar::CreateAddr(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type)
{
    PointerType* ptrType = MakePointerType(type);
    emitter.Own(ptrType);
    Ir::Intf::RegVar* addr = CreateTemporaryRegVar(ptrType);
    emitter.Own(addr);
    Ir::Intf::Object* zero = CreateI32Constant(0);
    emitter.Own(zero);
    Ir::Intf::Object* index = CreateI32Constant(Index());
    emitter.Own(index);
    emitter.Emit(GetElementPtr(Ptr()->GetType(), addr, Ptr(), zero, index));
    return addr;
}

} // namespace Llvm
