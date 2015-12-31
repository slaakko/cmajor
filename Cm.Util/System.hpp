/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_UTIL_SYSTEM_INCLUDED
#define CM_UTIL_SYSTEM_INCLUDED
#include <string>
#include <vector>

namespace Cm { namespace Util {

void System(const std::string& command);
void System(const std::string& command, int redirectFd, const std::string& toFile);
void System(const std::string& command, const std::vector<std::pair<int, std::string>>& redirections);
unsigned long long Spawn(const std::string& filename, const std::vector<std::string>& args);
int Wait(unsigned long long processHandle);
int ReadFromPipe(int pipeHandle, void* buffer, unsigned int count);
int WriteToPipe(int pipeHandle, void* buffer, unsigned int count);
void RedirectStdHandlesToPipes(std::vector<int>& oldHandles, std::vector<int>& pipeHandles);
void RestoreStdHandles(const std::vector<int>& oldHandles);

} } // namespace Cm::Util

#endif // CM_UTIL_SYSTEM_INCLUDED
