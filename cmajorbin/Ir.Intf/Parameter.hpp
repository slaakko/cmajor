/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef IR_INTF_PARAMETER_INCLUDED
#define IR_INTF_PARAMETER_INCLUDED
#include <Ir.Intf/Object.hpp>
#include <Ir.Intf/Type.hpp>

namespace Ir { namespace Intf {

class Parameter: public Object
{
public:
    Parameter(const std::string& name_, Type* type_);
};

} } // namespace Ir::Intf

#endif // IR_INTF_PARAMETER_INCLUDED
