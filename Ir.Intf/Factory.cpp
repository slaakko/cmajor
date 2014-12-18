/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Ir.Intf/Factory.hpp>

namespace Ir { namespace Intf {

Factory* globalFactory = nullptr;

Factory::~Factory()
{
}

void SetFactory(Factory* factory)
{
    globalFactory = factory;
}

Factory* GetFactory()
{
    return globalFactory;
}

} } // namespace Ir::Intf

