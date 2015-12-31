/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef PROFILE_INCLUDED
#define PROFILE_INCLUDED
#include <chrono>
#include <stdint.h>

const uint32_t startFunEvent = 0;
const uint32_t endFunEvent = 1;
const uint32_t flushFid = (uint32_t)-1;

struct ProfileRec
{
    ProfileRec() : elapsed(), fid(), evnt() {}
    ProfileRec(const std::chrono::steady_clock::duration& elapsed_, uint32_t fid_, uint32_t evnt_) : elapsed(elapsed_), fid(fid_), evnt(evnt_) {}
    std::chrono::steady_clock::duration elapsed;
    uint32_t fid;
    uint32_t evnt;
};

extern "C" void start_profiling(const char* profDataFilePath);
extern "C" void end_profiling();
extern "C" void start_profiled_fun(uint32_t fid);
extern "C" void end_profiled_fun(uint32_t fid);

#endif // PROFILE_INCLUDED
