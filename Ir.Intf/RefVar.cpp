/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Ir.Intf/RefVar.hpp>

namespace Ir { namespace Intf {

RefVar::RefVar(const std::string& name_, Type* type_): Object(name_, type_)
{
}

} } // namespace Llvm::Ir
