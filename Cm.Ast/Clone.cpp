/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Clone.hpp>
#include <Cm.Ast/Function.hpp>

namespace Cm { namespace Ast {

CloneContext::CloneContext() : instantiateClassNode(false) , makeTestUnits(false)
{
}

void CloneContext::AddUnitTestFunction(Cm::Ast::FunctionNode* unitTestFunction)
{
    unitTestFunctions.push_back(std::unique_ptr<Cm::Ast::FunctionNode>(unitTestFunction));
}

std::vector<std::unique_ptr<FunctionNode>>& CloneContext::UnitTestFunctions()
{ 
    return unitTestFunctions; 
}

} } // namespace Cm::Ast