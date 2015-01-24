/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef IR_INTF_STACKVAR_INCLUDED
#define IR_INTF_STACKVAR_INCLUDED
#include <Ir.Intf/Object.hpp>

namespace Ir { namespace Intf {

class StackVar: public Object
{
public:
    StackVar(const std::string& name_, Type* type_);
    bool IsStackVar() const override { return true; }
    virtual void SetTemporary() {}
};

typedef StackVar* StackVarPtr;

} } // namespace Ir::Intf

#endif // IR_INTF_STACKVAR_INCLUDED
