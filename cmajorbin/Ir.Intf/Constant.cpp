/*========================================================================
Copyright (c) 2012-2015 Seppo Laakko
http://sourceforge.net/projects/cmajor/

Distributed under the GNU General Public License, version 3 (GPLv3).
(See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Ir.Intf/Constant.hpp>

namespace Ir { namespace Intf {

Constant::Constant(const std::string& valueName_, Type* type_): Object(valueName_, type_)
{
}

} } // namespace Ir::Intf
