/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ParameterSymbol.hpp>

namespace Cm { namespace Sym {

ParameterSymbol::ParameterSymbol(Cm::Ast::ParameterNode* parameterNode) : Symbol(parameterNode->ToString())
{
    SetNode(parameterNode);
}

} } // namespace Cm::Sym