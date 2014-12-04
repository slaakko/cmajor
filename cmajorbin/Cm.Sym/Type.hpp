/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_TYPE_INCLUDED
#define CM_SYM_TYPE_INCLUDED
#include <Cm.Sym/Symbol.hpp>

namespace Cm { namespace Sym {

class TypeSymbol : public Symbol
{
public:
    TypeSymbol(const std::string& name_);
};

} } // namespace Cm::Sym

#endif // CM_SYM_TYPE_INCLUDED
