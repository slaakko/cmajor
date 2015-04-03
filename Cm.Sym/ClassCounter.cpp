/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ClassCounter.hpp>

namespace Cm { namespace Sym {

ClassCounter::ClassCounter(): numClasses(0), numClassesInThisProject(0)
{
}

void ClassCounter::AddLibryClasses(uint32_t numLibraryClasses)
{
    numClasses = numClasses + numLibraryClasses;
}

uint32_t ClassCounter::GetNextClassNumber()
{
    ++numClassesInThisProject;
    return ++numClasses;
}

uint32_t ClassCounter::GetNumberOfClasses() const
{
    return numClasses;
}

uint32_t ClassCounter::GetNumberOfClassesInThisProject() const
{
    return numClassesInThisProject;
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
