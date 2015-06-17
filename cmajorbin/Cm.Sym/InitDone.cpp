/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/InitDone.hpp>
#include <Cm.Sym/Factory.hpp>
#include <Cm.Sym/NameMangling.hpp>
#include <Cm.Sym/Warning.hpp>

namespace Cm { namespace Sym {

void Init()
{
    InitFactory();
    InitNameMangling();
    InitWarning();
}

void Done()
{
    DoneWarning();
    DoneNameMangling();
    DoneFactory();
}

} } // namespace Cm::Sym