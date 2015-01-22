/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_AST_BASIC_TYPE_INCLUDED
#define CM_AST_BASIC_TYPE_INCLUDED
#include <Cm.Ast/Node.hpp>

namespace Cm { namespace Ast {

class BoolNode: public Node
{
public:
    BoolNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::boolNode; }
    Node* Clone() const override;
    std::string ToString() const override { return "bool"; }
    void Accept(Visitor& visitor) override;
};

class SByteNode: public Node
{
public:
    SByteNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::sbyteNode; }
    Node* Clone() const override;
    std::string ToString() const override { return "sbyte"; }
    void Accept(Visitor& visitor) override;
};

class ByteNode: public Node
{
public:
    ByteNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::byteNode; }
    Node* Clone() const override;
    std::string ToString() const override { return "byte"; }
    void Accept(Visitor& visitor) override;
    bool IsUnsignedTypeNode() const override { return true; }
};

class ShortNode: public Node
{
public:
    ShortNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::shortNode; }
    Node* Clone() const override;
    std::string ToString() const override { return "short"; }
    void Accept(Visitor& visitor) override;
};

class UShortNode: public Node
{
public:
    UShortNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::ushortNode; }
    Node* Clone() const override;
    std::string ToString() const override { return "ushort"; }
    void Accept(Visitor& visitor) override;
    bool IsUnsignedTypeNode() const override { return true; }
};

class IntNode: public Node
{
public:
    IntNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::intNode; }
    Node* Clone() const override;
    std::string ToString() const override { return "int"; }
    void Accept(Visitor& visitor) override;
};

class UIntNode : public Node
{
public:
    UIntNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::uintNode; }
    Node* Clone() const override;
    std::string ToString() const override { return "uint"; }
    void Accept(Visitor& visitor) override;
    bool IsUnsignedTypeNode() const override { return true; }
};

class LongNode: public Node
{
public:
    LongNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::longNode; }
    Node* Clone() const override;
    std::string ToString() const override { return "long"; }
    void Accept(Visitor& visitor) override;
};

class ULongNode: public Node
{
public:
    ULongNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::ulongNode; }
    Node* Clone() const override;
    std::string ToString() const override { return "ulong"; }
    void Accept(Visitor& visitor) override;
    bool IsUnsignedTypeNode() const override { return true; }
};

class FloatNode: public Node
{
public:
    FloatNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::floatNode; }
    Node* Clone() const override;
    std::string ToString() const override { return "float"; }
    void Accept(Visitor& visitor) override;
};

class DoubleNode: public Node
{
public:
    DoubleNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::doubleNode; }
    Node* Clone() const override;
    std::string ToString() const override { return "double"; }
    void Accept(Visitor& visitor) override;
};

class CharNode: public Node
{
public:
    CharNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::charNode; }
    Node* Clone() const override;
    std::string ToString() const override { return "char"; }
    void Accept(Visitor& visitor) override;
};

class VoidNode: public Node
{
public:
    VoidNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::voidNode; }
    Node* Clone() const override;
    std::string ToString() const override { return "void"; }
    void Accept(Visitor& visitor) override;
};

} } // namespace Cm::Ast

#endif // CM_AST_BASIC_TYPE_INCLUDED
