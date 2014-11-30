/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_COMPILE_UNIT_INCLUDED
#define CM_AST_COMPILE_UNIT_INCLUDED
#include <Cm.Ast/Node.hpp>

namespace Cm { namespace Ast {

class CompileUnit
{
public:
    CompileUnit(const std::string& filePath_);
    void AddNode(Node* node);
private:
    std::string filePath;

    NodeList nodes;
};

} } // namespace Cm::Ast

#endif // CM_AST_COMPILE_UNIT_INCLUDED
