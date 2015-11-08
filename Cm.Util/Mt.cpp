/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

// Mersenne Twister pseudo random number engine.
// See: http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/mt19937ar.c

#include <Cm.Util/Mt.hpp>

#if defined(_WIN32)
#define _CRT_RAND_S
#include <stdlib.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif

#include <stdexcept>

#ifdef _WIN32

uint32_t get_random_seed_from_system()
{
    uint32_t seed = 0;
    errno_t retval = rand_s(&seed);
    if (retval != 0)
    {
        throw std::runtime_error("get_random_seed_from_system() failed");
    }
    return seed;
}

#elif defined(__linux) || defined(__unix) || defined(__posix)

uint32_t get_random_seed_from_system()
{
    uint32_t seed = 0;
    int fn = open("/dev/urandom", O_RDONLY);
    if (fn == -1)
    {
        throw std::runtime_error("get_random_seed_from_system() failed");
    }
    if (read(fn, &seed, 4) != 4)
    {
        throw std::runtime_error("get_random_seed_from_system() failed");
    }
    close(fn);
    return seed;
}

#else

#error unknown platform

#endif

namespace Cm { namespace Util {

class MT
{
private:
    static const int32_t n = 624;
    static const int32_t m = 397;
    static const uint32_t  matrixA = 0x9908b0dfu;
    static const uint32_t upperMask = 0x80000000u;
    static const uint32_t lowerMask = 0x7fffffffu;
public:
    static bool Initialized()
    {
        return initialized;
    }
    static void InitWithRandomSeed()
    {
        uint32_t seed = get_random_seed_from_system();
        Init(seed);
    }
    static void Init(uint32_t seed)
    {
        initialized = true;
        mt[0] = seed;
        for (mti = 1; mti < n; ++mti)
        {
            mt[mti] = 1812433253u * (mt[mti - 1] ^ (mt[mti - 1] >> 30u)) + static_cast<uint32_t>(mti);
        }
        mag[0] = 0u;
        mag[1] = matrixA;
    }
    static uint32_t GenRand()
    {
        uint32_t y = 0u;
        if (mti >= n)
        {
            int32_t kk;
            for (kk = 0; kk < n - m; ++kk)
            {
                y = (mt[kk] & upperMask) | (mt[kk + 1] & lowerMask);
                mt[kk] = mt[kk + m] ^ (y >> 1u) ^ mag[static_cast<int32_t>(y & 0x01u)];
            }
            for (; kk < n - 1; ++kk)
            {
                y = (mt[kk] & upperMask) | (mt[kk + 1] & lowerMask);
                mt[kk] = mt[kk + (m - n)] ^ (y >> 1u) ^ mag[static_cast<int32_t>(y & 0x01u)];
            }
            y = (mt[n - 1] & upperMask) | (mt[0] & lowerMask);
            mt[n - 1] = mt[m - 1] ^ (y >> 1u) ^ mag[static_cast<int32_t>(y & 0x01u)];
            mti = 0;
        }
        y = mt[mti++];
        y = y ^ (y >> 11u);
        y = y ^ ((y << 7u) & 0x9d2c5680u);
        y = y ^ ((y << 15u) & 0xefc60000u);
        y = y ^ (y >> 18u);
        return y;
    }
private:
    static int32_t mti;
    static uint32_t mt[n];
    static uint32_t mag[2];
    static bool initialized;
};

int32_t MT::mti = 0;
uint32_t MT::mt[n];
uint32_t MT::mag[2];
bool MT::initialized = false;

void InitMt(uint32_t seed)
{
    MT::Init(seed);
}

uint32_t Rand()
{
    if (!MT::Initialized())
    {
        MT::InitWithRandomSeed();
    }
    return MT::GenRand();
}

} } // namespace Cm::Util
