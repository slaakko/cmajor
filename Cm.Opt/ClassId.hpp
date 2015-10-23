/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_OPT_CLASS_ID_INCLUDED
#define CM_OPT_CLASS_ID_INCLUDED
#include <Cm.Sym/ClassTypeSymbol.hpp>

namespace Cm { namespace Opt {

void ProcessClasses(const std::unordered_set<Cm::Sym::ClassTypeSymbol*>& classes);

} } // namespace Cm::Opt

#endif // CM_OPT_CLASS_ID_INCLUDED

