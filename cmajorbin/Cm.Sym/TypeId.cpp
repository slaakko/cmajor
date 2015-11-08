/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/TypeId.hpp>
#include <Cm.Util/Mt.hpp>
#include <string>
#include <cstring>

namespace Cm { namespace Sym {

TypeId::TypeId() : hashCodeValid(false), hashCode(0)
{
    SetRep(0, Cm::Util::Rand());
    SetRep(4, Cm::Util::Rand());
    SetRep(8, Cm::Util::Rand());
    SetRep(12, Cm::Util::Rand());
}

TypeId::TypeId(uint8_t leader) : hashCodeValid(false), hashCode(0)
{
    std::memset(rep, 0, typeIdRepSize);
    rep[0] = leader;
}

TypeId::TypeId(uint64_t cid): hashCodeValid(false), hashCode(0)
{
    SetRep(0, Cm::Util::Rand());
    SetRep(4, Cm::Util::Rand());
    uint32_t high = static_cast<uint32_t>(cid >> 32);
    uint32_t low = static_cast<uint32_t>(cid & 0xFFFFFFFFu);
    SetRep(8, high);
    SetRep(12, low);
}

TypeId::TypeId(uint8_t* data, int size) : hashCodeValid(false), hashCode(0)
{
#ifndef NDEBUG
    if (size != typeIdRepSize)
    {
        throw std::runtime_error("invalid type id data length (not " + std::to_string(typeIdRepSize) + ")");
    }
#endif
    std::memcpy(rep, data, size);
}

void TypeId::SetRep(int index, uint32_t r)
{
#ifndef NDEBUG
    if (index < 0 || index >= typeIdRepSize)
    {
        throw std::runtime_error("invalid type id index");
    }
#endif
    rep[index] = static_cast<uint8_t>(r >> 24u);
    rep[index + 1] = static_cast<uint8_t>(r >> 16u);
    rep[index + 2] = static_cast<uint8_t>(r >> 8u);
    rep[index + 3] = static_cast<uint8_t>(r);
}

void TypeId::ComputeHashCode() const
{
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
    static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
    const size_t offset_basis = 14695981039346656037ULL;
    const size_t prime = 1099511628211ULL;
#else /* defined(_WIN64) */
    static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
    const size_t offset_basis = 2166136261U;
    const size_t prime = 16777619U;
#endif /* defined(_WIN64) */
    size_t hashValue = offset_basis;
    for (int i = 0; i < typeIdRepSize; ++i)
    {
        hashValue ^= static_cast<size_t>(rep[i]);
        hashValue *= prime;
    }
    hashCode = hashValue;
    hashCodeValid = true;
}

bool operator==(const TypeId& left, const TypeId& right)
{
    return std::memcmp(left.Rep(), right.Rep(), typeIdRepSize) == 0;
}

bool operator<(const TypeId& left, const TypeId& right)
{
    return std::memcmp(left.Rep(), right.Rep(), typeIdRepSize) < 0;
}

} } // namespace Cm::Sym
