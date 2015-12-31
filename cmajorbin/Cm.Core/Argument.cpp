/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/Argument.hpp>

namespace Cm { namespace Core {

Argument::Argument(ArgumentCategory category_, Cm::Sym::TypeSymbol* type_) : category(category_), type(type_), bindToRvalueRef(false)
{
}

} } // namespace Cm::Core
