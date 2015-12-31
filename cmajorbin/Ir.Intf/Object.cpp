/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Ir.Intf/Object.hpp>
#include <Ir.Intf/Type.hpp>
#include <stdexcept>

namespace Ir { namespace Intf {

Object::Object(const std::string& name_, Type* type_): name(name_), type(type_), owned(false)
{
    if (!type->Owned())
    {
        type->SetOwned();
        ownedType.reset(type);
    }
}

Object::~Object()
{
}

void Object::SetType(Type* type_)
{
    type = type_;
}

Object* Object::CreateAddr(Emitter& emitter, Type* type_)
{
    throw std::runtime_error("CreateAddr() not supported by this object type");
}

} } // namespace Ir::Intf
