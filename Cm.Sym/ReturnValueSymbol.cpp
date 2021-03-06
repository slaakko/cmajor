/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ReturnValueSymbol.hpp>

namespace Cm { namespace Sym {

ReturnValueSymbol::ReturnValueSymbol(const Span& span_) : VariableSymbol(span_, "@returns")
{
}

ReturnValueSymbol::ReturnValueSymbol(const Span& span_, const std::string& name_) : VariableSymbol(span_, name_)
{
}

} } // namespace Cm::Sym
