/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_UTIL_PRIME_INCLUDED
#define CM_UTIL_PRIME_INCLUDED
#include <stdint.h>

namespace Cm { namespace Util {

// Returns smallest prime greater than or equal to x

uint64_t NextPrime(uint64_t x);

} } // namespace Cm::Util

#endif // CM_UTIL_PRIME_INCLUDED


