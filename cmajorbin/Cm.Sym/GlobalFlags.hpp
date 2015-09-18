/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_GLOBAL_FLAGS_INCLUDED
#define CM_SYM_GLOBAL_FLAGS_INCLUDED
#include <stdint.h>

namespace Cm { namespace Sym {

enum class GlobalFlags : uint16_t
{
    none = 0,
    optimize = 1 << 0,
    wpo = 1 << 1,
    quiet = 1 << 2,
    emitOpt = 1 << 3,
    ide = 1 << 4,
    trace = 1 << 5,
    debug_heap = 1 << 6,
    clean = 1 << 7,
    unit_test = 1 << 8,
    generate_debug_info = 1 << 9,
    generate_docs = 1 << 10,
    no_call_stacks = 1 << 11
};

inline GlobalFlags operator|(GlobalFlags left, GlobalFlags right)
{
    return GlobalFlags(uint16_t(left) | uint16_t(right));
}

inline GlobalFlags operator&(GlobalFlags left, GlobalFlags right)
{
    return GlobalFlags(uint16_t(left) & uint16_t(right));
}

bool GetGlobalFlag(GlobalFlags flag);
void SetGlobalFlag(GlobalFlags flag);

} } // namespace Cm::Sym

#endif // CM_SYM_GLOBAL_FLAGS_INCLUDED
