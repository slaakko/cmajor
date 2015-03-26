/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Util/System.hpp>
#include <Cm.Util/Handle.hpp>
#include <stdexcept>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef WIN32
#include <process.h>
#include <Windows.h>
#include <io.h>
#elif defined(__linux) || defined(__posix) || defined(__unix)
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/select.h>
#endif

namespace Cm { namespace Util {

#ifdef WIN32

int dup2(int oldFd, int newFd)
{
    return _dup2(oldFd, newFd);
}

int pipe(int pipefd[2])
{
    unsigned int psize = 4096;
    int textmode = _O_BINARY;
    return _pipe(pipefd, psize, textmode);
}

#endif

int get_default_pmode()
{
#if defined(WIN32)

    return S_IREAD | S_IWRITE;

#elif defined(__linux) || defined(__unix) || defined(__posix)

    return S_IRUSR | S_IWUSR | S_IRGRP;

#else

#error unknown platform

#endif
}

void System(const std::string& command)
{
    int retVal = system(command.c_str());
    if (retVal != 0)
    {
        throw std::runtime_error("'" + command + "' returned " + std::to_string(retVal));
    }
}

void System(const std::string& command, int redirectFd, const std::string& toFile)
{
    Handle old = dup(redirectFd);
    if (old == -1)
    {
        throw std::runtime_error("System redirect: could not duplicate handle " + std::to_string(redirectFd));
    }
    Handle fd = creat(toFile.c_str(), get_default_pmode());
    if (fd == -1)
    {
        throw std::runtime_error("System: could not create file '" + toFile + "'");
    }
    if (dup2(fd, redirectFd) == -1)
    {
        throw std::runtime_error("System redirect: dup2 failed");
    }
    try
    {
        System(command);
        dup2(old, redirectFd);
    }
    catch (...)
    {
        dup2(old, redirectFd);
        throw;
    }
}

} } // namespace Cm::Util
