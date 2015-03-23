/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_GLOBAL_FLAGS_INCLUDED
#define CM_CORE_GLOBAL_FLAGS_INCLUDED
#include <stdint.h>

namespace Cm { namespace Core {

enum class GlobalFlags : uint8_t
{
    none = 0,
    optimize = 1 << 0,
    quiet = 1 << 1
};

inline GlobalFlags operator|(GlobalFlags left, GlobalFlags right)
{
    return GlobalFlags(uint8_t(left) | uint8_t(right));
}

inline GlobalFlags operator&(GlobalFlags left, GlobalFlags right)
{
    return GlobalFlags(uint8_t(left) & uint8_t(right));
}

bool GetGlobalFlag(GlobalFlags flag);
void SetGlobalFlag(GlobalFlags flag);

} } // namespace Cm::Core

#endif // CM_CORE_GLOBAL_FLAGS_INCLUDED
