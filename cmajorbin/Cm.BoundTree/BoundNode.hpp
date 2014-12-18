/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BOUND_TREE_BOUND_NODE_INCLUDED
#define CM_BOUND_TREE_BOUND_NODE_INCLUDED
#include <Cm.Ast/Node.hpp>

namespace Cm { namespace BoundTree {

class BoundNode
{
public:
    BoundNode(Cm::Ast::Node* syntaxNode_);
    virtual ~BoundNode();
    Cm::Ast::Node* SyntaxNode() const { return syntaxNode; }
    virtual bool IsBoundExpressionNode() const { return false;  }
private:
    Cm::Ast::Node* syntaxNode;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_BOUND_NODE_INCLUDED
