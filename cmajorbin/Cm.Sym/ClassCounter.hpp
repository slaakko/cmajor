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
    void AddLibryClasses(uint32_t numLibraryClasses);
    uint32_t GetNextClassNumber();
    uint32_t GetNumberOfClasses() const;
    uint32_t GetNumberOfClassesInThisProject() const;
private:
    uint32_t numClasses;
    uint32_t numClassesInThisProject;
};

ClassCounter* GetClassCounter();

void SetClassCounter(ClassCounter* classCounter_);

} } // namespace Cm::Sym

#endif // CM_SYM_CLASS_COUNTER_INCLUDED
