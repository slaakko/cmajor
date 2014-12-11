/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/TypeId.hpp>

namespace Cm { namespace Sym {

TypeId::TypeId() : baseTypeId(), derivations(), hashCodeValid(false), hashCode(0)
{
}

TypeId::TypeId(const Cm::Util::Uuid& baseTypeId_) : baseTypeId(baseTypeId_), derivations(), hashCodeValid(false), hashCode(0)
{
}

TypeId::TypeId(const Cm::Util::Uuid& baseTypeId_, const Cm::Ast::DerivationList& derivations_) : baseTypeId(baseTypeId_), derivations(derivations_), hashCodeValid(false), hashCode(0)
{
}

void TypeId::ComputeHashCode() const
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
    size_t hashValue = offset_basis;
    int n = int(baseTypeId.Tag().size());
    for (int i = 0; i < n; ++i)
    {
        hashValue ^= static_cast<size_t>(baseTypeId.Tag().data[i]);
        hashValue *= prime;
    }
    int nd = derivations.NumDerivations();
    if (nd > 0)
    {
        for (int i = 0; i < nd; ++i)
        {
            hashValue ^= static_cast<size_t>(derivations[i]);
            hashValue *= prime;
        }
    }
    hashCode = hashValue;
    hashCodeValid = true;
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
