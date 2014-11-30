/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Ast/Node.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>

namespace Cm { namespace Ast {

Node::Node()
{
}

Node::Node(const Span& span_): span(span_)
{
}

Node::~Node()
{
}

Node* Node::GetValue() const 
{ 
    throw std::runtime_error("member function not applicable");
    return nullptr; 
}

void Node::AddArgument(Node* argument) 
{
    throw std::runtime_error("member function not applicable");
}

void Node::AddParameter(ParameterNode* parameter)
{
    throw std::runtime_error("member function not applicable");
}

void Node::AddTemplateParameter(TemplateParameterNode* templateParameter)
{
    throw std::runtime_error("member function not applicable");
}

void Node::AddInitializer(InitializerNode* initializer)
{
    throw std::runtime_error("member function not applicable");
}

void Node::Read(Reader& reader)
{
}

void Node::Write(Writer& writer)
{
}

UnaryNode::UnaryNode(const Span& span_): Node(span_)
{
}

UnaryNode::UnaryNode(const Span& span_, Node* child_): Node(span_), child(child_)
{
}

void UnaryNode::Read(Reader& reader)
{
    child.reset(reader.ReadNode());
}

void UnaryNode::Write(Writer& writer) 
{
    writer.Write(child.get());
}

BinaryNode::BinaryNode(const Span& span_) : Node(span_)
{
}

BinaryNode::BinaryNode(const Span& span_, Node* left_, Node* right_) : Node(span_), left(left_), right(right_)
{
}

void BinaryNode::Read(Reader& reader)
{
    left.reset(reader.ReadNode());
    right.reset(reader.ReadNode());
}

void BinaryNode::Write(Writer& writer)
{
    writer.Write(left.get());
    writer.Write(right.get());
}

NodeList::NodeList()
{
}

void NodeList::Read(Reader& reader)
{
    uint32_t n = reader.ReadUInt();
    for (uint32_t i = 0; i < n; ++i)
    {
        nodes.push_back(std::unique_ptr<Node>(reader.ReadNode()));
    }
}

void NodeList::Write(Writer& writer)
{
    uint32_t n = static_cast<uint32_t>(nodes.size());
    writer.Write(n);
    for (uint32_t i = 0; i < n; ++i)
    {
        writer.Write(nodes[i].get());
    }
}

} } // namespace Cm::Ast

