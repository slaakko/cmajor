/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Ir.Intf/Type.hpp>

namespace Ir { namespace Intf {

Type::Type(const std::string& name_) : name(name_), owned(false)
{
}

Type::~Type()
{
}

} } // namespace Ir::Intf
