/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_UNIT_TEST_INCLUDED
#define CM_UNIT_TEST_INCLUDED
#include <string>
#include <unordered_set>

namespace Cm { namespace Unit {

bool TestSolution(const std::string& solutionFilePath, const std::string& fileName, const std::string& testName, const std::unordered_set<std::string>& defines);
bool TestProject(const std::string& projectFilePath, const std::string& fileName, const std::string& testName, const std::unordered_set<std::string>& defines);

} } // Cm::Unit

#endif // CM_UNIT_TEST_INCLUDED
