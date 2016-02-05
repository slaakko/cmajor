/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Llvm.Ir/Label.hpp>
#include <Llvm.Ir/Type.hpp>
#include <Llvm.Ir/Factory.hpp>
#include <stdexcept>
#include <iostream>
#include <stack>

namespace Llvm { 

LabelObject::LabelObject(): Ir::Intf::LabelObject(Ir::Intf::GetFactory()->GetLabelType())
{
}

LabelObject::LabelObject(const std::string& label_): Ir::Intf::LabelObject(Ir::Intf::GetFactory()->GetLabelType(), label_, "%" + label_)
{
}

void LabelObject::Set(Ir::Intf::LabelObject* from)
{
    SetLabelName(from->GetLabelName());
    SetName("%" + GetLabelName());
}

void LabelObject::InitTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    throw std::runtime_error("cannot init labels");
}

void LabelObject::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    throw std::runtime_error("cannot init labels");
}

void LabelObject::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    throw std::runtime_error("cannot init labels");
}

void LabelObject::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    throw std::runtime_error("cannot init labels");
}

void LabelObject::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    throw std::runtime_error("cannot init labels");
}

void LabelObject::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    throw std::runtime_error("cannot init labels");
}

void LabelObject::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    throw std::runtime_error("cannot init labels");
}

void LabelObject::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot init labels");
}

void LabelObject::AssignTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    throw std::runtime_error("cannot assign labels");
}

void LabelObject::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    throw std::runtime_error("cannot assign labels");
}

void LabelObject::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    throw std::runtime_error("cannot assign labels");
}

void LabelObject::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    throw std::runtime_error("cannot assign labels");
}

void LabelObject::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    throw std::runtime_error("cannot assign labels");
}

void LabelObject::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    throw std::runtime_error("cannot assign labels");
}

void LabelObject::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    throw std::runtime_error("cannot assign labels");
}

void LabelObject::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot assign labels");
}

Ir::Intf::LabelObject* CreateLabel()
{
    return new LabelObject();
}

Ir::Intf::LabelObject* CreateLabel(const std::string& label)
{
    return new LabelObject(label);
}

int localLabelCounter = 0;
std::string str;
std::stack<std::string> strStack;
int breaker = 0;

void SetBreaker(int x)
{
    breaker = x;
}

void PushString(const std::string& s)
{
    strStack.push(str);
    str = s;
}

void PopString()
{
    str = strStack.top();
    strStack.pop();
}

Ir::Intf::LabelObject* CreateNextLocalLabel()
{
    if (breaker == 1)
    {
        std::cout << localLabelCounter << ": " << str << std::endl;
    }
    return CreateLabel("$L" + std::to_string(localLabelCounter++));
}

void ResetLocalLabelCounter()
{
    localLabelCounter = 0;
}

} // namespace Llvm
