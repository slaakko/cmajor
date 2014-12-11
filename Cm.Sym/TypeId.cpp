/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/TypeId.hpp>

namespace Cm { namespace Sym {

TypeId::TypeId() : baseTypeId(), derivations()
{
}

TypeId::TypeId(const Cm::Util::Uuid& baseTypeId_) : baseTypeId(baseTypeId_), derivations()
{
}

TypeId::TypeId(const Cm::Util::Uuid& baseTypeId_, const Cm::Ast::DerivationList& derivations_) : baseTypeId(baseTypeId_), derivations(derivations_)
{
}

bool operator==(const TypeId& left, const TypeId& right)
{
    return left.BaseTypeId() == right.BaseTypeId() && left.Derivations() == right.Derivations();
}

bool operator<(const TypeId& left, const TypeId& right)
{
    if (left.BaseTypeId() < right.BaseTypeId())
    {
        return true;
    }
    else if (left.BaseTypeId() > right.BaseTypeId())
    {
        return false;
    }
    else
    {
        return left.Derivations() < right.Derivations();
    }
}

} } // namespace Cm::Sym
