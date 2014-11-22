/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Ast/Node.hpp>

namespace Cm { namespace Ast {

std::ostream& operator<<(std::ostream& s, NodeType nt)
{
    return s << static_cast<uint8_t>(nt);
}

Node::Node()
{
}

std::ostream& operator<<(std::ostream& s, Node& node)
{
    return node.Write(s);
}

UnaryNode::UnaryNode(Node* child_): child(child_)
{
}

std::ostream& UnaryNode::Write(std::ostream& s)
{
    return child->Write(s);
}

BinaryNode::BinaryNode(Node* left_, Node* right_): left(left_), right(right_)
{
}

std::ostream& BinaryNode::Write(std::ostream& s)
{    
    left->Write(s);
    return right->Write(s);
}

} } // namespace Cm::Ast

