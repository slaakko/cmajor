/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

FunctionSymbol::FunctionSymbol(Cm::Ast::FunctionNode* functionNode) : ContainerSymbol(functionNode->Name())
{
    SetNode(functionNode);
}

} } // namespace Cm::Sym