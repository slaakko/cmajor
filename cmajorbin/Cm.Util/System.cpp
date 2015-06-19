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
#include <string.h>
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

#ifdef WIN32

unsigned long long Spawn(const std::string& filename, const std::vector<std::string>& args)
{
    if (args.size() > 15)
    {
        throw std::runtime_error("spawn: too many args");
    }
    const char* argList[16];
    int a = 0;
    argList[a++] = filename.c_str();
    for (int i = 0; i < args.size(); ++i)
    {
        argList[a++] = args[i].c_str();
    }
    argList[a] = NULL;
    intptr_t handle = _spawnvp(_P_NOWAIT, filename.c_str(), argList);
    if (handle == -1)
    {
        std::string error = strerror(errno);
        throw std::runtime_error("spawn: " + error);
    }
    return handle;
}

int Wait(unsigned long long processHandle)
{
    int exitCode = 0;
    intptr_t result = _cwait(&exitCode, processHandle, 0);
    if (result == -1)
    {
        std::string error = strerror(errno);
        throw std::runtime_error("wait: " + error);
    }
    return exitCode;
}

#elif defined(__linux) || defined(__posix) || defined(__unix)

unsigned long long Spawn(const std::string& filename, const std::vector<std::string>& args)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        std::string error = strerror(errno);
        throw std::runtime_error("fork: " + error);
    }
    if (pid == 0)   // in child...
    {
        std::string command = filename;
        for (const std::string& arg : args)
        {
            command.append(" ").append(arg);
        }
        System(command);
        exit(0);
    }
    else // in parent...
    {
        return pid;
    }
}

int Wait(unsigned long long)
{
    int exitCode = 0;
    pid_t pid = wait(&exitCode);
    if (pid == -1)
    {
        std::string error = strerror(errno);
        throw std::runtime_error("wait: " + error);
    }
    return exitCode;
}

#endif 

int ReadFromPipe(int pipeHandle, void* buffer, unsigned int count)
{
    return read(pipeHandle, buffer, count);
}

int WriteToPipe(int pipeHandle, void* buffer, unsigned int count)
{
    return write(pipeHandle, buffer, count);
}

void RedirectStdHandlesToPipes(std::vector<int>& oldHandles, std::vector<int>& pipeHandles)
{
    int phRead[2];
    if (pipe(phRead) == -1)
    {
        throw std::runtime_error("RedirectStdHandlesToPipes: pipe failed");
    }
    int old0 = dup(0);
    if (old0 == -1)
    {
        throw std::runtime_error("RedirectStdHandlesToPipes: dup(0) failed");
    }
    oldHandles.push_back(old0);
    if (dup2(phRead[0], 0) == -1)
    {
        throw std::runtime_error("RedirectStdHandlesToPipes: dup2(p0, 0)");
    }
    pipeHandles.push_back(phRead[1]);
    int phWrite[2];
    if (pipe(phWrite) == -1)
    {
        throw std::runtime_error("RedirectStdHandlesToPipes: pipe failed");
    }
    int old1 = dup(1);
    if (old1 == -1)
    {
        throw std::runtime_error("RedirectStdHandlesToPipes: dup(1) failed");
    }
    oldHandles.push_back(old1);
    if (dup2(phWrite[1], 1) == -1)
    {
        throw std::runtime_error("RedirectStdHandlesToPipes: dup2(p1, 1)");
    }
    pipeHandles.push_back(phWrite[0]);
}

void RestoreStdHandles(const std::vector<int>& oldHandles)
{
    if (dup2(oldHandles[0], 0) == -1)
    {
        throw std::runtime_error("RestoreStdHandles: dup2 0, old0 failed");
    }
    if (dup2(oldHandles[1], 1) == -1)
    {
        throw std::runtime_error("RestoreStdHandles: dup2 1, old1 failed");
    }
}

} } // namespace Cm::Util
