/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/BoundNode.hpp>

namespace Cm { namespace BoundTree {

BoundNode::BoundNode(Cm::Ast::Node* syntaxNode_) : syntaxNode(syntaxNode_)
{
}

BoundNode::~BoundNode()
{
}

} } // namespace Cm::BoundTree