/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Ast/Node.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Function.hpp>
#include <stdexcept>

namespace Cm { namespace Ast {

Node::Node() : span(), parent(nullptr)
{
}

Node::Node(const Span& span_): span(span_), parent(nullptr)
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

void Node::Print(CodeFormatter& formatter)
{
    throw std::runtime_error("member function not applicable");
}

std::string Node::Name() const
{
    throw std::runtime_error("member function not applicable");
}

std::string Node::FullName() const
{
    std::string parentFullName;
    Node* parent = Parent();
    if (parent)
    {
        parentFullName = parent->FullName();
    }
    if (parentFullName.empty())
    {
        return Name();
    }
    else
    {
        return parentFullName + "." + Name(); 
    }
}

void Node::Accept(Visitor& visitor)
{
    throw std::runtime_error("member function not applicable");
}

FunctionNode* Node::GetFunction() const
{
    if (IsFunctionNode())
    {
        return const_cast<FunctionNode*>(static_cast<const FunctionNode*>(this));
    }
    else
    {
        Node* parent = Parent();
        if (parent)
        {
            return parent->GetFunction();
        }
        else
        {
            throw std::runtime_error("node not found");
        }
    }
}

UnaryNode::UnaryNode(const Span& span_): Node(span_)
{
}

UnaryNode::UnaryNode(const Span& span_, Node* child_): Node(span_), child(child_)
{
    child->SetParent(this);
}

void UnaryNode::Read(Reader& reader)
{
    child.reset(reader.ReadNode());
    child->SetParent(this);
}

void UnaryNode::Write(Writer& writer) 
{
    writer.Write(child.get());
}

std::string UnaryNode::ToString() const
{
    std::string s = GetOpStr();
    if (child->GetRank() > GetRank())
    {
        s.append(1, '(');
    }
    s.append(child->ToString());
    if (child->GetRank() > GetRank())
    {
        s.append(1, ')');
    }
    return s;

}

void UnaryNode::Accept(Visitor& visitor)
{
    child->Accept(visitor);
}

BinaryNode::BinaryNode(const Span& span_) : Node(span_)
{
}

BinaryNode::BinaryNode(const Span& span_, Node* left_, Node* right_) : Node(span_), left(left_), right(right_)
{
    left->SetParent(this);
    right->SetParent(this);
}

void BinaryNode::Read(Reader& reader)
{
    left.reset(reader.ReadNode());
    left->SetParent(this);
    right.reset(reader.ReadNode());
    right->SetParent(this);
}

void BinaryNode::Write(Writer& writer)
{
    writer.Write(left.get());
    writer.Write(right.get());
}

std::string BinaryNode::ToString() const
{
    std::string s;
    if (left->GetRank() > GetRank())
    {
        s.append(1, '(');
    }
    s.append(left->ToString());
    if (left->GetRank() > GetRank())
    {
        s.append(1, ')');
    }
    s.append(1, ' ').append(GetOpStr()).append(1, ' ');
    if (right->GetRank() > GetRank())
    {
        s.append(1, '(');
    }
    s.append(right->ToString());
    if (right->GetRank() > GetRank())
    {
        s.append(1, ')');
    }
    return s;
}

void BinaryNode::Accept(Visitor& visitor)
{
    left->Accept(visitor);
    right->Accept(visitor);
}

NodeList::NodeList()
{
}

void NodeList::SetParent(Node* parent)
{
    for (const std::unique_ptr<Node>& node : nodes)
    {
        node->SetParent(parent);
    }
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

void NodeList::Print(CodeFormatter& formatter)
{
    for (const std::unique_ptr<Node>& node : nodes)
    {
        node->Print(formatter);
    }
}

std::string NodeList::ToString() const
{
    std::string s;
    bool first = true;
    for (const std::unique_ptr<Node>& node : nodes)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            s.append(", ");
        }
        s.append(node->ToString());
    }
    return s;
}

void NodeList::Accept(Visitor& visitor)
{
    for (const std::unique_ptr<Node>& node : nodes)
    {
        node->Accept(visitor);
    }
}

} } // namespace Cm::Ast

