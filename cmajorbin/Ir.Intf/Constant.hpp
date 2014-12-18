/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef IR_INTF_CONSTANT_INCLUDED
#define IR_INTF_CONSTANT_INCLUDED
#include <Ir.Intf/Object.hpp>

namespace Ir { namespace Intf {

class Constant: public Object
{
public:
    Constant(const std::string& valueName_, Type* type_);
    virtual bool IsConstant() const { return true; }
};

} } // namespace Ir::Intf

#endif // IR_INTF_CONSTANT_INCLUDED
