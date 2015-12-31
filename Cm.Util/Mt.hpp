/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

// Mersenne Twister pseudo random number engine.
// See: http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/mt19937ar.c

#include <stdint.h>

namespace Cm { namespace Util {

//  Note: In general you do not have to call the InitMt function,
//  only if you want predictable sequence of pseudo random numbers,
//  call InitMt.

void InitMt(uint32_t seed);

uint32_t Rand();

} } // namespace Cm::Util
