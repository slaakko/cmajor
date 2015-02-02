/*========================================================================
    Copyright (c) 2011-2015 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_UTIL_ALGORITHM_INCLUDED
#define CM_UTIL_ALGORITHM_INCLUDED
#include <vector>
#include <set>
#include <stdexcept>

namespace Cm { namespace Util {

//  Topological Sort
//  ================
//  Requires that SortablePtr is a pointer-like object to an item that has two member functions:
//  o   const std::set<SortablePtr>& DependsOn():   returns a set of pointers to items that this item depends on
//  o   std::string FullName();                 :   returns the full name of the item
//  Also requires that SortablePtr is less-than-comparable.

template<typename SortablePtr>
void Visit(std::vector<SortablePtr>& order, const SortablePtr& item, std::set<SortablePtr>& visited, std::set<SortablePtr>& tempVisit, bool denyCircularDependency)
{
    if (tempVisit.find(item) == tempVisit.end())
    {
        if (visited.find(item) == visited.end())
        {
            tempVisit.insert(item);
            const std::set<SortablePtr>& dependsOn = item->DependsOn();
            typename std::set<SortablePtr>::const_iterator e = dependsOn.end();
            for (typename std::set<SortablePtr>::const_iterator i = dependsOn.begin(); i != e; ++i)
            {
                SortablePtr dependent = *i;
                Visit(order, dependent, visited, tempVisit, denyCircularDependency);
            }
            tempVisit.erase(item);
            visited.insert(item);
            order.push_back(item);
        }
    }
    else if (denyCircularDependency)
    {
        throw std::runtime_error("circular dependency detected: '" + item->FullName() + "'");
    }
}

template<typename SortablePtr>
std::vector<SortablePtr> TopologicalSort(const std::set<SortablePtr>& items, bool denyCircularDependency)
{
    std::vector<SortablePtr> order;
    std::set<SortablePtr> visited;
    std::set<SortablePtr> tempVisit;
    typename std::set<SortablePtr>::const_iterator e = items.end();
    for (typename std::set<SortablePtr>::const_iterator i = items.begin(); i != e; ++i)
    {
        SortablePtr item = *i;
        if (visited.find(item) == visited.end())
        {
            Visit(order, item, visited, tempVisit, denyCircularDependency);
        }
    }
    return order;
}

template<typename SortablePtr>
std::vector<SortablePtr> TopologicalSort(const std::set<SortablePtr>& items)
{
    return TopologicalSort(items, true);
}

} } // namespace Cm::Util

#endif // CM_UTIL_ALGORITHM_INCLUDED
