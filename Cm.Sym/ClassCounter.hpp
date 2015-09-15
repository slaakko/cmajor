/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_CLASS_COUNTER_INCLUDED
#define CM_SYM_CLASS_COUNTER_INCLUDED
#include <stdint.h>

namespace Cm { namespace Sym {

class ClassCounter
{
public:
    ClassCounter();
    uint32_t GetCid() { return nextCid++; }
    void SetNextCid(uint32_t nextCid_) { nextCid = nextCid_; }
    uint32_t GetNextCid() const { return nextCid; }
private:
    uint32_t nextCid;
};

ClassCounter* GetClassCounter();

void SetClassCounter(ClassCounter* classCounter_);

} } // namespace Cm::Sym

#endif // CM_SYM_CLASS_COUNTER_INCLUDED
