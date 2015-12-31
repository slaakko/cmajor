/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_TYPE_ID_INCLUDED
#define CM_SYM_TYPE_ID_INCLUDED
#include <stdexcept>
#include <utility>

namespace Cm { namespace Sym {

const int typeIdRepSize = 16;

class TypeId
{
public:
    TypeId();
    TypeId(uint8_t leader);
    TypeId(uint64_t cid);
    TypeId(uint8_t* data, int size);
    size_t GetHashCode() const { if (!hashCodeValid) ComputeHashCode(); return hashCode; }
    void InvalidateHashCode() { hashCodeValid = false; }
    const uint8_t* Rep() const { return &rep[0]; }
    uint8_t* Rep() { return &rep[0]; }
    uint8_t operator[](int index) const
    {
#ifndef NDEBUG
        if (index < 0 || index >= typeIdRepSize)
        {
            throw std::runtime_error("invalid type id index");
        }
#endif
        return rep[index];
    }
    void SetByte(int index, uint8_t b)
    {
#ifndef NDEBUG
        if (index < 0 || index >= typeIdRepSize)
        {
            throw std::runtime_error("invalid type id index");
        }
#endif
        hashCodeValid = false;
        rep[index] = b;
    }
private:
    uint8_t rep[typeIdRepSize];
    mutable bool hashCodeValid;
    mutable size_t hashCode;
    void ComputeHashCode() const;
    void SetRep(int index, uint32_t r);
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

inline TypeId operator^(const TypeId& left, const TypeId& right)
{
    TypeId result = left;
    for (int i = 0; i < typeIdRepSize; ++i)
    {
        result.SetByte(i, result[i] ^ right[i]);
    }
    return result;
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
