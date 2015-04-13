/*========================================================================
    Copyright (c) 2012-2014 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#ifdef LINUX
#include <unistd.h>
#include <sys/types.h>
#endif
#include <pthread.h>
#ifdef WIN32
#include <pthread_time.h>
#endif

typedef unsigned long long thread_t;
typedef void* mutex_t;
typedef void* cond_t;

int read_64(int fd, void* buf, uint64_t size)
{
    return read(fd, buf, (size_t)size);
}

int write_64(int fd, const void* buf, uint64_t size)
{
    return write(fd, buf, (size_t)size);
}

int get_errno()
{
    return errno;
}

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

int create_directory(const char* directoryName)
{
#if defined(WIN32)

    return mkdir(directoryName);

#elif defined(__linux) || defined(__unix) || defined(__posix)

    return mkdir(directoryName,  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

#else

    #error unknown platform

#endif
}

enum OpenFlags
{
    none = 0, readOnly = 1, writeOnly = 2, readWrite = 4, create = 8, append = 16, truncate = 32, text = 64, binary = 128
};

int open_file(const char* filename, enum OpenFlags openFlags, int pmode)
{
    int oflags = none;
    if ((openFlags & readOnly) != none)
    {
        oflags |= O_RDONLY;
    }
    if ((openFlags & writeOnly) != none)
    {
        oflags |= O_WRONLY;
    }
    if ((openFlags & readWrite) != none)
    {
        oflags |= O_RDWR;
    }
    if ((openFlags & create) != none)
    {
        oflags |= O_CREAT;  // note: in Linux 0x0040, in Windows 0x0100
    }
    if ((openFlags & append) != none)
    {
        oflags |= O_APPEND; // note: in Linux 0x0400, in Windows 0x0008
    }
    if ((openFlags & truncate) != none)
    {
        oflags |= O_TRUNC;
    }
#ifdef WIN32
    if ((openFlags & text) != none)
    {
        oflags |= O_TEXT;
    }
    if ((openFlags & binary) != none)
    {
        oflags |= O_BINARY;
    }
#endif
    return open(filename, oflags, pmode);
}

int file_exists(const char* filePath)
{
#ifdef WIN32
    struct _stat statBuf;
    int result = _stat(filePath, &statBuf);
    if (result == 0)
    {
        return (statBuf.st_mode & _S_IFDIR) == 0 ? 1 : 0;
    }
    else
    {
        return 0;
    }
#elif defined(__linux) || defined(__unix) || defined(__posix)
    struct stat statBuf;
    int result = stat(filePath, &statBuf);
    if (result == 0)
    {
        return (S_ISDIR(statBuf.st_mode)) == 0 ? 1 : 0;
    }
    else
    {
        return 0;
    }
#else
    #error unknown platform
#endif
}

int directory_exists(const char* directoryPath)
{
#ifdef WIN32
    struct _stat statBuf;
    int result = _stat(directoryPath, &statBuf);
    if (result == 0)
    {
        return (statBuf.st_mode & _S_IFDIR) != 0 ? 1 : 0;
    }
    else
    {
        return 0;
    }
#elif defined(__linux) || defined(__unix) || defined(__posix)
    struct stat statBuf;
    int result = stat(directoryPath, &statBuf);
    if (result == 0)
    {
        return (S_ISDIR(statBuf.st_mode)) != 0 ? 1 : 0;
    }
    else
    {
        return 0;
    }
#else
    #error unknown platform
#endif
}

int path_exists(const char* path)
{
#ifdef WIN32
    struct _stat statBuf;
    return _stat(path, &statBuf) == 0 ? 1 : 0;
#elif defined(__linux) || defined(__unix) || defined(__posix)
    struct stat statBuf;
    return stat(path, &statBuf) == 0 ? 1 : 0;
#else
    #error unknown platform
#endif
}

char* get_current_working_directory(char* buf, int bufSize)
{
#if defined(WIN32)

    return _getcwd(buf, bufSize);

#elif defined(__linux) || defined(__unix) || defined(__posix)

    long retval = getcwd(buf, (unsigned long)bufSize);
    if (retval == -1)
    {
        return NULL;
    }
    else
    {
        return buf;
    }

#else

    #error unknown platform

#endif
}

char* get_environment_variable(const char* varName)
{
    return getenv(varName);
}

thread_t this_thread()
{
    return pthread_self();
}

typedef void* (*thread_fun)(void*);

int create_thread(thread_t* tid, void (*start)(void*), void* arg)
{
    return pthread_create((pthread_t*)tid, NULL, (thread_fun)start, arg);
}

void exit_thread(void* value)
{
    pthread_exit(value);
}

int detach_thread(thread_t tid)
{
    return pthread_detach(*(pthread_t*)&tid);
}

int join_thread(thread_t tid, void** value)
{
    return pthread_join(*(pthread_t*)&tid, value);
}

int equal_thread(thread_t tid1, thread_t tid2)
{
    return pthread_equal(*(pthread_t*)&tid1, *(pthread_t*)&tid2) != 0 ? 1 : 0;
}

mutex_t* allocate_mutex_handle()
{
    return (mutex_t*)malloc(sizeof(pthread_mutex_t));
}

void free_mutex_handle(mutex_t* handle)
{
    free(handle);
}

int init_mutex(mutex_t* mtx)
{
    return pthread_mutex_init((pthread_mutex_t*)mtx, NULL);
}

int init_recursive_mutex(mutex_t* mtx)
{
    pthread_mutexattr_t attr;
    int attr_init_result = pthread_mutexattr_init(&attr);
    if (attr_init_result != 0)
    {
        return attr_init_result;
    }
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    int init_result = pthread_mutex_init((pthread_mutex_t*)mtx, &attr);
    if (init_result != 0)
    {
        return init_result;
    }
    int attr_destroy_result = pthread_mutexattr_destroy(&attr);
    if (attr_destroy_result != 0)
    {
        return attr_destroy_result;
    }
    return 0;
}

int destroy_mutex(mutex_t* mtx)
{
    return pthread_mutex_destroy((pthread_mutex_t*)mtx);
}

int lock_mutex(mutex_t* mtx)
{
    return pthread_mutex_lock((pthread_mutex_t*)mtx);
}

int trylock_mutex(mutex_t* mtx, _Bool* already_locked)
{
    int result = pthread_mutex_trylock((pthread_mutex_t*)mtx);
    *already_locked = result == EBUSY;
    return result;
}

int unlock_mutex(mutex_t* mtx)
{
    return pthread_mutex_unlock((pthread_mutex_t*)mtx);
}

cond_t* allocate_cond_handle()
{
    return (cond_t*)malloc(sizeof(pthread_cond_t));
}

void free_cond_handle(cond_t* handle)
{
    free(handle);
}

int init_cond(cond_t* cond)
{
    return pthread_cond_init((pthread_cond_t*)cond, NULL);
}

int destroy_cond(cond_t* cond)
{
    return pthread_cond_destroy((pthread_cond_t*)cond);
}

int broadcast_cond(cond_t* cond)
{
    return pthread_cond_broadcast((pthread_cond_t*)cond);
}

int signal_cond(cond_t* cond)
{
    return pthread_cond_signal((pthread_cond_t*)cond);
}

int wait_cond(cond_t* cond, mutex_t* mtx)
{
    return pthread_cond_wait((pthread_cond_t*)cond, (pthread_mutex_t*)mtx);
}

int timedwait_cond(cond_t* cond, mutex_t* mtx, long long secs, int nanosecs, _Bool* timedout)
{
    struct timespec abstime;
    abstime.tv_sec = secs;
    abstime.tv_nsec = nanosecs;
    *timedout = 0;
    int result = pthread_cond_timedwait((pthread_cond_t*)cond, (pthread_mutex_t*)mtx, &abstime);
    if (result == ETIMEDOUT)
    {
        *timedout = 1;
        return 0;
    }
    else 
    {
        return result;
    }
}

int time_nanosecs(long long* secs, int* nanosecs)
{
    struct timespec tp;
    int result = clock_gettime(CLOCK_REALTIME, &tp);
    if (result != 0)
    {
        return result;
    }
    *secs = tp.tv_sec;
    *nanosecs = tp.tv_nsec;
    return 0;
}

int sleep(long long secs, int nanosecs)
{
    struct timespec request;
    request.tv_sec = secs;
    request.tv_nsec = nanosecs;
    return nanosleep(&request, NULL);
}

int get_current_date(char* dateBuf)
{
    time_t rawTime;
    struct tm* timeInfo;
    time(&rawTime);
    timeInfo = localtime(&rawTime);
    return strftime(dateBuf, 11, "%Y-%m-%d", timeInfo);
}

static int traceLevel = 0;

void enter_traced_fun(const char* fun, const char* file, int line)
{
    fprintf(stderr, ">%04d:%s[%s:%d]\n", traceLevel, fun, file, line);
    ++traceLevel;
    fflush(stderr);
}

void leave_traced_fun(const char* fun, const char* file, int line)
{
    --traceLevel;
    fprintf(stderr, "<%04d:%s[%s:%d]\n", traceLevel, fun, file, line);
    fflush(stderr);
}

void enter_traced_call(const char* fun, const char* file, int line)
{
    fprintf(stderr, "+%04d:%s[%s:%d]\n", traceLevel, fun, file, line);
    ++traceLevel;
    fflush(stderr);
}

void leave_traced_call(const char* fun, const char* file, int line)
{
    --traceLevel;
    fprintf(stderr, "-%04d:%s[%s:%d]\n", traceLevel, fun, file, line);
    fflush(stderr);
}
