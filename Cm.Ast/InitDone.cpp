/*========================================================================
Copyright (c) 2012-2015 Seppo Laakko
http://sourceforge.net/projects/cmajor/

Distributed under the GNU General Public License, version 3 (GPLv3).
(See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/InitDone.hpp>
#include <Cm.Ast/Factory.hpp>

namespace Cm { namespace Ast {

void Init()
{
    InitFactory();
}

void Done()
{
    DoneFactory();
}

} } // namespace Cm::Ast