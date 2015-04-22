/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_UNIT_TEST_INCLUDED
#define CM_UNIT_TEST_INCLUDED
#include <string>

namespace Cm { namespace Unit {

bool TestSolution(const std::string& solutionFilePath, const std::string& fileName, const std::string& testName);
bool TestProject(const std::string& projectFilePath, const std::string& fileName, const std::string& testName);

} } // Cm::Unit

#endif // CM_UNIT_TEST_INCLUDED
