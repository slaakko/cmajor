/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef LLVM_IR_STACKVAR_INCLUDED
#define LLVM_IR_STACKVAR_INCLUDED
#include <Ir.Intf/StackVar.hpp>

namespace Llvm { 

class StackVar : public Ir::Intf::StackVar
{
public:
    StackVar(const std::string& name_, Ir::Intf::Type* pointerType_);
    void InitTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to) override;
    void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant) override;
    void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global) override;
    void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar) override;
    void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar) override;
    void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar) override;
    void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar) override;
    void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter) override;
    void AssignTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to) override;
    void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant) override;
    void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global) override;
    void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar) override;
    void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar) override;
    void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar) override;
    void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar) override;
    void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter) override;
    Ir::Intf::Object* CreateAddr(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type) override;
};

} // namespace Llvm

#endif // LLVM_IR_STACKVAR_INCLUDED
