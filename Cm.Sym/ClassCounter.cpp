/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ClassCounter.hpp>

namespace Cm { namespace Sym {

ClassCounter::ClassCounter(): nextCid(0)
{
}

ClassCounter* classCounter = nullptr;

ClassCounter* GetClassCounter()
{
    return classCounter;
}

void SetClassCounter(ClassCounter* classCounter_)
{
    classCounter = classCounter_;
}

} } // namespace Cm::Sym
