/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Symbol.hpp>

namespace Cm { namespace Sym {

Symbol::Symbol(): parent(nullptr), node(nullptr)
{
}

Symbol::Symbol(const std::string& name_): name(name_), parent(nullptr), node(nullptr)
{
}

Symbol::~Symbol()
{
}

} } // namespace Cm::Sym
