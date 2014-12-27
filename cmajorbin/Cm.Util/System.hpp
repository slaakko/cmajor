/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_UTIL_SYSTEM_INCLUDED
#define CM_UTIL_SYSTEM_INCLUDED
#include <string>

namespace Cm { namespace Util {

void System(const std::string& command);
void System(const std::string& command, int redirectFd, const std::string& toFile);

} } // namespace Cm::Util

#endif // CM_UTIL_SYSTEM_INCLUDED
