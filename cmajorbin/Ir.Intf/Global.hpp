/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef IR_INTF_GLOBAL_INCLUDED
#define IR_INTF_GLOBAL_INCLUDED
#include <Ir.Intf/Object.hpp>

namespace Ir { namespace Intf {

class Global: public Object
{
public:
    Global(const std::string& name_, Type* type_);
    bool IsGlobal() const override { return true; }
};

typedef Global* GlobalPtr;

} } // namespace Ir::Intf

#endif // IR_INTF_GLOBAL_INCLUDED
