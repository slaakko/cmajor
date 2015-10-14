/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include "class_hierarchy.hpp"
#include <unordered_map>

std::unordered_map<uint64_t, uint64_t> classHierarchy;
const uint64_t noCid = -1;

extern "C" void init_class_hierarchy(uint64_t* classHierarchyTable, int32_t numEntries)
{
    for (int32_t i = 0; i < numEntries / 2; ++i)
    {
        uint64_t derivedCid = classHierarchyTable[2 * i];
        uint64_t baseCid = classHierarchyTable[2 * i + 1];
        classHierarchy[derivedCid] = baseCid;
    }
}

extern "C" bool is_class_same_or_derived_from(uint64_t derivedCid, uint64_t baseCid)
{
    if (derivedCid == baseCid)
    {
        return true;
    }
    else
    {
        std::unordered_map<uint64_t, uint64_t>::const_iterator i = classHierarchy.find(derivedCid);
        if (i != classHierarchy.cend())
        {
            uint64_t bcid = i->second;
            while (bcid != noCid)
            {
                if (bcid == baseCid)
                {
                    return true;
                }
                else
                {
                    i = classHierarchy.find(bcid);
                    if (i != classHierarchy.cend())
                    {
                        bcid = i->second;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
    }
    return false;
}
