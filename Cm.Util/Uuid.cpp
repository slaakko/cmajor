/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Util/Uuid.hpp>
#include <boost/uuid/random_generator.hpp>

namespace Cm { namespace Util {

bool IsPredefined(const boost::uuids::uuid& id)
{
    int n = int(id.size());
    for (int i = 1; i < n; ++i)
    {
        if (id.data[i] != 0)
        {
            return false;
        }
    }
    return true;
}

Uuid::Uuid() : tag(boost::uuids::random_generator()())
{
    while (IsPredefined(tag)) // while tag is predefined (first byte is 0 .. 255 and others are 0) keep generating new tag...
    {
        tag = boost::uuids::random_generator()();
    }
}

Uuid::Uuid(uint8_t id): tag()
{
    tag.data[0] = id;
}

} } // namespace Cm::Util
