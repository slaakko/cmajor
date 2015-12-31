/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Ir.Intf/Type.hpp>
#include <stdexcept>

namespace Ir { namespace Intf {

Type::Type(const std::string& name_) : name(name_), owned(false)
{
}

Type::~Type()
{
}

Object* Type::CreateDefaultValue() const
{
    throw std::runtime_error("member function not applicable");
}

Object* Type::CreateMinusOne() const
{
    throw std::runtime_error("member function not applicable");
}

Object* Type::CreatePlusOne() const
{
    throw std::runtime_error("member function not applicable");
}

} } // namespace Ir::Intf
