/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/BoundCompileUnitSerialization.hpp>

#ifndef CM_BOUND_TREE_FACTORY_INCLUDED
#define CM_BOUND_TREE_FACTORY_INCLUDED

namespace Cm { namespace BoundTree {

class Factory : public Cm::Sym::BcuItemFactory
{
public:
    Cm::Sym::BcuItem* CreateItem(Cm::Sym::BcuItemType itemType) const override;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_FACTORY_INCLUDED
