/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.IrIntf/BackEnd.hpp>
#include <Cm.IrIntf/LlvmBackEnd.hpp>
#include <Cm.IrIntf/CBackEnd.hpp>
#include <Ir.Intf/Factory.hpp>

namespace Cm { namespace IrIntf {

BackEnd globalBackEnd = BackEnd::llvm;

BackEndIntf* backEndImpl = GetLlvmBackEnd();

BackEndIntf* GetBackEndImpl()
{
    return backEndImpl;
}

void SetBackEnd(BackEnd backEnd)
{
    globalBackEnd = backEnd;
    switch (globalBackEnd)
    {
        case BackEnd::llvm: 
        {
            backEndImpl = GetLlvmBackEnd(); 
            Ir::Intf::SetFactory(GetLlvmFactory());
            break;
        }
        case BackEnd::c: 
        {
            backEndImpl = GetCBackEnd();
            Ir::Intf::SetFactory(GetCFactory());
            break;
        }
    }
}

BackEnd GetBackEnd()
{
    return globalBackEnd;
}

std::string GetBackEndStr()
{
    switch (globalBackEnd)
    {
        case BackEnd::llvm: return "llvm";
        case BackEnd::c: return "c";
    }
    return "";
}

BackEndIntf::~BackEndIntf()
{
}

} } // namespace Cm::IrIntf
