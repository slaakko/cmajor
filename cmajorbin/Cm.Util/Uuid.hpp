/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_UTIL_UUID_INCLUDED
#define CM_UTIL_UUID_INCLUDED
#include <utility>
#include <boost/uuid/uuid.hpp>
#include <stdint.h>

namespace Cm { namespace Util {

class Uuid
{
public:
    Uuid();             // construct a random uuid
    Uuid(uint8_t id);   // construct a predefined uuid (first byte is id, others are 0)
    const boost::uuids::uuid& Tag() const { return tag; }
    boost::uuids::uuid& Tag() { return tag; }
private:
    boost::uuids::uuid tag;
};

inline bool operator==(const Uuid& left, const Uuid& right)
{
    return left.Tag() == right.Tag();
}

inline bool operator!=(const Uuid& left, const Uuid& right)
{
    return std::rel_ops::operator!=(left, right);
}

inline bool operator<(const Uuid& left, const Uuid& right)
{
    return left.Tag() < right.Tag();
}

inline bool operator>(const Uuid& left, const Uuid& right)
{
    return std::rel_ops::operator>(left, right);
}

inline bool operator>=(const Uuid& left, const Uuid& right)
{
    return std::rel_ops::operator>=(left, right);
}

inline bool operator<=(const Uuid& left, const Uuid& right)
{
    return std::rel_ops::operator<=(left, right);
}

Uuid operator^(const Uuid& left, const Uuid& right);

} } // namespace Cm::Util

#endif // CM_UTIL_UUID_INCLUDED
