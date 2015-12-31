/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Ir.Intf/MemberVar.hpp>

namespace Ir { namespace Intf {

MemberVar::MemberVar(const std::string& name_, Object* ptr_, int index_, Type* type_): Object(name_, type_), ptr(ptr_), index(index_)
{
}

} } // namespace Ir::Intf
