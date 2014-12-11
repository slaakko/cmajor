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
    const Cm::Ast::DerivationList& Derivations() const { return derivations; }
private:
    Cm::Util::Uuid baseTypeId;
    Cm::Ast::DerivationList derivations;
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
#if defined(_WIN64)
    static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
    const size_t offset_basis = 14695981039346656037ULL;
    const size_t prime = 1099511628211ULL;
#else /* defined(_WIN64) */
    static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
    const size_t offset_basis = 2166136261U;
    const size_t prime = 16777619U;
#endif /* defined(_WIN64) */
    size_t operator()(const TypeId& typeId) const
    {
        size_t hashValue = offset_basis;
        int n = int(typeId.BaseTypeId().Tag().size());
        for (int i = 0; i < n; ++i)
        {
            hashValue ^= static_cast<size_t>(typeId.BaseTypeId().Tag().data[i]);
            hashValue *= prime;
        }
        int nd = typeId.Derivations().NumDerivations();
        if (nd > 0)
        {
            for (int i = 0; i < nd; ++i)
            {
                hashValue ^= static_cast<size_t>(typeId.Derivations()[i]);
                hashValue *= prime;
            }
        }
        return hashValue;
    }
};

} } // namespace Cm::Sym

#endif // CM_SYM_TYPE_ID_INCLUDED
