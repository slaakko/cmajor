/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/CompileUnit.hpp>

namespace Cm { namespace Ast {

CompileUnit::CompileUnit(const std::string& filePath_) : filePath(filePath_)
{
}

void CompileUnit::AddNode(Node* node)
{
    nodes.Add(node);
}

} } // namespace Cm::Ast