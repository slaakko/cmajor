/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include "profile.hpp"
#include <stdio.h>

ProfileRec* profileBuf = nullptr;
ProfileRec* profileBufPtr = nullptr;
ProfileRec* profileBufEnd = nullptr;
FILE* profDataFile = nullptr;

inline void AppendProfileEvent(uint32_t fid, uint32_t evnt)
{
    *profileBufPtr++ = ProfileRec(fid, evnt);
}

void FlushProfileBuf(bool last)
{
    if (!profDataFile) return;
    if (!last)
    {
        AppendProfileEvent(flushFid, startFunEvent);
    }
    fwrite(profileBuf, sizeof(ProfileRec), profileBufPtr - profileBuf, profDataFile);
    profileBufPtr = profileBuf;
    if (!last)
    {
        AppendProfileEvent(flushFid, endFunEvent);
    }
}

extern "C" void start_profiling(const char* profDataFilePath)
{
    profileBuf = new ProfileRec[(1024 * 1024) / sizeof(ProfileRec)];
    profileBufPtr = profileBuf;
    profileBufEnd = profileBuf + (1024 * 1024) / sizeof(ProfileRec) - 1;
    profDataFile = fopen(profDataFilePath, "wb");
}

extern "C" void end_profiling()
{
    if (profDataFile)
    {
        FlushProfileBuf(true);
        fclose(profDataFile);
    }
}

extern "C" void start_profiled_fun(uint32_t fid)
{
    if (profileBufPtr == profileBufEnd)
    {
        FlushProfileBuf(false);
    }
    AppendProfileEvent(fid, startFunEvent);
}

extern "C" void end_profiled_fun(uint32_t fid)
{
    if (profileBufPtr == profileBufEnd)
    {
        FlushProfileBuf(false);
    }
    AppendProfileEvent(fid, endFunEvent);
}
