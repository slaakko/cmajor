/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Specifier.hpp>

namespace Cm { namespace Ast {

bool HasStaticSpecifier(Specifiers s)
{
    return (s & Specifiers::static_) != Specifiers::none;
}

} } // namespace Cm::Ast