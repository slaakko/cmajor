/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/TypeSymbol.hpp>

namespace Cm { namespace Sym {

TypeSymbol::TypeSymbol(const Span& span_, const std::string& name_) : ContainerSymbol(span_, name_)
{
}

TypeSymbol::TypeSymbol(const Span& span_, const std::string& name_, const Cm::Util::Uuid& id_) : ContainerSymbol(span_, name_), id(id_)
{
}

} } // namespace Cm::Sym