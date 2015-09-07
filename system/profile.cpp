/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <chrono>
#include <stdio.h>
#include <stdint.h>

namespace 
{
    const uint32_t startFunEvent = 0;
    const uint32_t endFunEvent = 1;
    const uint32_t flushFid = (uint32_t)-1;

    struct ProfileRec
    {
        ProfileRec() : timestamp(), fid(), evnt() {}
        ProfileRec(uint32_t fid_, uint32_t evnt_) : timestamp(std::chrono::steady_clock::now()), fid(fid_), evnt(evnt_) {}
        std::chrono::steady_clock::time_point timestamp;
        uint32_t fid;
        uint32_t evnt;
    };

    ProfileRec* profileBuf = nullptr;
    ProfileRec* profileBufPtr = nullptr;
    ProfileRec* profileBufEnd = nullptr;
    FILE* profileData = nullptr;

    inline void AppendProfileEvent(uint32_t fid, uint32_t evnt)
    {
        *profileBufPtr++ = ProfileRec(fid, evnt);
    }

    void FlushProfileBuf(bool last)
    {
        if (!profileData) return;
        if (!last)
        {
            AppendProfileEvent(flushFid, startFunEvent);
        }
        fwrite(profileBuf, sizeof(ProfileRec), profileBufPtr - profileBuf, profileData);
        profileBufPtr = profileBuf;
        if (!last)
        {
            AppendProfileEvent(flushFid, endFunEvent);
        }
    }
}

extern "C" void start_profiling(const char* profile)
{
    profileBuf = new ProfileRec[(1024 * 1024) / sizeof(ProfileRec)];
    profileBufPtr = profileBuf;
    profileBufEnd = profileBuf + (1024 * 1024) / sizeof(ProfileRec) - 1;
    profileData = fopen(profile, "wb");
}

extern "C" void end_profiling()
{
    if (profileData)
    {
        FlushProfileBuf(true);
        fclose(profileData);
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
