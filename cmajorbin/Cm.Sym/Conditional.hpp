/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_CONDITIONAL_INCLUDED
#define CM_SYM_CONDITIONAL_INCLUDED
#include <string>
#include <unordered_set>

namespace Cm { namespace Sym {

void Define(const std::unordered_set<std::string>& symbols);
bool IsSymbolDefined(const std::string& symbol);

void InitConditional();
void DoneConditional();

} } // namespace Cm::Sym

#endif // CM_SYM_CONDITIONAL_INCLUDED
