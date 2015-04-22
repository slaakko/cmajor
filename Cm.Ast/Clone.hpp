/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_CLONE_INCLUDED
#define CM_AST_CLONE_INCLUDED
#include <vector>
#include <memory>

namespace Cm { namespace Ast {

class FunctionNode;

class CloneContext
{
public:
    CloneContext();
    void SetInstantiateClassNode() { instantiateClassNode = true; }
    bool InstantiateClassNode() const { return instantiateClassNode; }
    void SetMakeTestUnits() { makeTestUnits = true; }
    bool MakeTestUnits() const { return makeTestUnits; }
    void AddUnitTestFunction(Cm::Ast::FunctionNode* unitTestFunction);
    std::vector<std::unique_ptr<FunctionNode>>& UnitTestFunctions();
private:
    bool instantiateClassNode;
    bool makeTestUnits;
    std::vector<std::unique_ptr<FunctionNode>> unitTestFunctions;
};

} } // namespace Cm::Ast

#endif // CM_AST_CLONE_INCLUDED
