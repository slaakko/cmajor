/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_SYNTAX_TREE_INCLUDED
#define CM_AST_SYNTAX_TREE_INCLUDED
#include <Cm.Ast/CompileUnit.hpp>

namespace Cm { namespace Ast {

class SyntaxTree
{
public:
    void AddCompileUnit(CompileUnitNode* compileUnit);
    const std::vector<std::unique_ptr<CompileUnitNode>>& CompileUnits() const { return compileUnits; }
    void Write(Writer& writer);
private:
    std::vector<std::unique_ptr<CompileUnitNode>> compileUnits;
};

} } // namespace Cm::Ast

#endif // CM_AST_SYNTAX_TREE_INCLUDED
