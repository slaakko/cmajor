/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_TYPE_ID_INCLUDED
#define CM_SYM_TYPE_ID_INCLUDED
#include <Cm.Ast/TypeExpr.hpp>
#include <Cm.Util/Uuid.hpp>

namespace Cm { namespace Sym {

class TypeId
{
public:
    TypeId();
    TypeId(const Cm::Util::Uuid& baseTypeId_);
    TypeId(const Cm::Util::Uuid& baseTypeId_, const Cm::Ast::DerivationList& derivations_);
    const Cm::Util::Uuid& BaseTypeId() const { return baseTypeId; }
    Cm::Util::Uuid& BaseTypeId() { return baseTypeId; }
    const Cm::Ast::DerivationList& Derivations() const { return derivations; }
    void SetDerivations(const Cm::Ast::DerivationList& derivations_) { derivations = derivations_; hashCodeValid = false; }
    size_t GetHashCode() const { if (!hashCodeValid) ComputeHashCode(); return hashCode; }
private:
    Cm::Util::Uuid baseTypeId;
    Cm::Ast::DerivationList derivations;
    mutable bool hashCodeValid;
    mutable size_t hashCode;
    void ComputeHashCode() const;
};

bool operator==(const TypeId& left, const TypeId& right);
bool operator<(const TypeId& left, const TypeId& right);

inline bool operator!=(const TypeId& left, const TypeId& right)
{
    return std::rel_ops::operator!=(left, right);
}

inline bool operator>(const TypeId& left, const TypeId& right)
{
    return std::rel_ops::operator>(left, right);
}

inline bool operator>=(const TypeId& left, const TypeId& right)
{
    return std::rel_ops::operator>=(left, right);
}

inline bool operator<=(const TypeId& left, const TypeId& right)
{
    return std::rel_ops::operator<=(left, right);
}

struct TypeIdHash
{
    size_t operator()(const TypeId& typeId) const
    {
        return typeId.GetHashCode();
    }
};

} } // namespace Cm::Sym

#endif // CM_SYM_TYPE_ID_INCLUDED
