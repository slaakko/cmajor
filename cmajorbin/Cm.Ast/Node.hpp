/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef NODE_INCLUDED
#define NODE_INCLUDED
#include <memory>
#include <ostream>
#include <stdint.h>

namespace Cm { namespace Ast {

enum class NodeType: uint8_t
{
    boolNode, sbyteNode, byteNode, shortNode, ushortNode, intNode, uintNode, longNode, ulongNode, floatNode, doubleNode, charNode, voidNode, 
    maxNode
};

std::ostream& operator<<(std::ostream& s, NodeType nt);

class Node
{
public:
    Node();
    virtual std::ostream& Write(std::ostream& s) = 0;
};

std::ostream& operator<<(std::ostream& s, Node& node);

class UnaryNode: Node
{
public: 
    UnaryNode(Node* child_);
    virtual std::ostream& Write(std::ostream& s);
private:
    std::unique_ptr<Node> child;
};

class BinaryNode: Node
{
public:
    BinaryNode(Node* left_, Node* right_);
    virtual std::ostream& Write(std::ostream& s);
private:
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};

} } // namespace Cm::Ast

#endif // NODE_INCLUDED
