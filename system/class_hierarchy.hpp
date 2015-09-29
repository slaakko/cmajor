/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CLASS_HIERARCHY_INCLUDED
#define CLASS_HIERARCHY_INCLUDED
#include <stdint.h>

extern "C" void init_class_hierarchy(uint64_t* classHierarchyTable, int32_t numEntries);
extern "C" bool is_class_same_or_derived_from(uint64_t derivedCid, uint64_t baseCid);

#endif // CLASS_HIERARCHY_INCLUDED
