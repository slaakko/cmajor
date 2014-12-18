/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef LLVM_IR_REGVAR_INCLUDED
#define LLVM_IR_REGVAR_INCLUDED
#include <Ir.Intf/RegVar.hpp>

namespace Llvm { 

class RegVar : public Ir::Intf::RegVar
{
public:
    RegVar(const std::string& name_, Ir::Intf::Type* type_);
    virtual void InitTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to);
    virtual void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant);
    virtual void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global);
    virtual void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar);
    virtual void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar);
    virtual void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar);
    virtual void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar);
    virtual void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter);
    virtual void AssignTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to);
    virtual void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant);
    virtual void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global);
    virtual void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar);
    virtual void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar);
    virtual void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar);
    virtual void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar);
    virtual void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter);
    virtual Ir::Intf::Object* CreateAddr(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type);
};

typedef RegVar* RegVarPtr;

Ir::Intf::RegVar* CreateRegVar(const std::string& name,  Ir::Intf::Type* type);
Ir::Intf::RegVar* CreateTemporaryRegVar(Ir::Intf::Type* type);

} // namespace Llvm

#endif // LLVM_IR_REGVAR_INCLUDED

