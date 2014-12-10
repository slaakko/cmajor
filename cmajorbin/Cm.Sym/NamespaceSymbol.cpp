/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/NamespaceSymbol.hpp>

namespace Cm { namespace Sym {

NamespaceSymbol::NamespaceSymbol(const Span& span_, const std::string& name_) : ContainerSymbol(span_, name_)
{
}

} } // namespace Cm::Sym
