/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_EXPRESSION_INCLUDED
#define CM_AST_EXPRESSION_INCLUDED
#include <Cm.Ast/Node.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Ast {

class EquivalenceNode : public BinaryNode
{
public:
    EquivalenceNode(const Span& span_);
    EquivalenceNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::equivalenceNode; }
};

class ImplicationNode : public BinaryNode
{
public:
    ImplicationNode(const Span& span_);
    ImplicationNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::implicationNode; }
};

class DisjunctionNode : public BinaryNode
{
public:
    DisjunctionNode(const Span& span_);
    DisjunctionNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::disjunctionNode; }
};

class ConjunctionNode : public BinaryNode
{
public:
    ConjunctionNode(const Span& span_);
    ConjunctionNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::conjunctionNode; }
};

class BitOrNode : public BinaryNode
{
public:
    BitOrNode(const Span& span_);
    BitOrNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::bitOrNode; }
};

class BitXorNode : public BinaryNode
{
public:
    BitXorNode(const Span& span_);
    BitXorNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::bitXorNode; }
};

class BitAndNode : public BinaryNode
{
public:
    BitAndNode(const Span& span_);
    BitAndNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::bitAndNode; }
};

class EqualNode : public BinaryNode
{
public:
    EqualNode(const Span& span_);
    EqualNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::equalNode; }
};

class NotEqualNode : public BinaryNode
{
public:
    NotEqualNode(const Span& span_);
    NotEqualNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::notEqualNode; }
};

class LessNode : public BinaryNode
{
public:
    LessNode(const Span& span_);
    LessNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::lessNode; }
};

class GreaterNode : public BinaryNode
{
public:
    GreaterNode(const Span& span_);
    GreaterNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::greaterNode; }
};

class LessOrEqualNode : public BinaryNode
{
public:
    LessOrEqualNode(const Span& span_);
    LessOrEqualNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::lessOrEqualNode; }
};

class GreaterOrEqualNode : public BinaryNode
{
public:
    GreaterOrEqualNode(const Span& span_);
    GreaterOrEqualNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::greaterOrEqualNode; }
};

class ShiftLeftNode : public BinaryNode
{
public:
    ShiftLeftNode(const Span& span_);
    ShiftLeftNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::shiftLeftNode; }
};

class ShiftRightNode : public BinaryNode
{
public:
    ShiftRightNode(const Span& span_);
    ShiftRightNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::shiftRightNode; }
};

class AddNode : public BinaryNode
{
public:
    AddNode(const Span& span_);
    AddNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::addNode; }
};

class SubNode : public BinaryNode
{
public:
    SubNode(const Span& span_);
    SubNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::subNode; }
};

class MulNode : public BinaryNode
{
public:
    MulNode(const Span& span_);
    MulNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::mulNode; }
};

class DivNode : public BinaryNode
{
public:
    DivNode(const Span& span_);
    DivNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::divNode; }
};

class RemNode : public BinaryNode
{
public:
    RemNode(const Span& span_);
    RemNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetType() const override { return NodeType::remNode; }
};

class InvokeNode : public UnaryNode
{
public:
    InvokeNode(const Span& span_);
    InvokeNode(const Span& span_, Node* subject_);
    NodeType GetType() const override { return NodeType::invokeNode; }
    void AddArgument(Node* argument) override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    NodeList arguments;
};

class IndexNode : public UnaryNode
{
public:
    IndexNode(const Span& span_);
    IndexNode(const Span& span_, Node* subject_, Node* index_);
    NodeType GetType() const override { return NodeType::indexNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    std::unique_ptr<Node> index;
};

class DotNode : public UnaryNode
{
public:
    DotNode(const Span& span_);
    DotNode(const Span& span_, Node* subject_, IdentifierNode* memberId_);
    NodeType GetType() const override { return NodeType::dotNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    std::unique_ptr<IdentifierNode> memberId;
};

class ArrowNode : public UnaryNode
{
public:
    ArrowNode(const Span& span_);
    ArrowNode(const Span& span_, Node* subject_, IdentifierNode* memberId_);
    NodeType GetType() const override { return NodeType::arrowNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    std::unique_ptr<IdentifierNode> memberId;
};

class PostfixIncNode : public UnaryNode
{
public:
    PostfixIncNode(const Span& span_);
    PostfixIncNode(const Span& span_, Node* subject_);
    NodeType GetType() const override { return NodeType::postfixIncNode; }
};

class PostfixDecNode : public UnaryNode
{
public:
    PostfixDecNode(const Span& span_);
    PostfixDecNode(const Span& span_, Node* subject_);
    NodeType GetType() const override { return NodeType::postfixDecNode; }
};

class DerefNode : public UnaryNode
{
public:
    DerefNode(const Span& span_);
    DerefNode(const Span& span_, Node* subject_);
    NodeType GetType() const override { return NodeType::derefNode; }
};

class AddrOfNode : public UnaryNode
{
public:
    AddrOfNode(const Span& span_);
    AddrOfNode(const Span& span_, Node* subject_);
    NodeType GetType() const override { return NodeType::addrOfNode; }
};

class NotNode : public UnaryNode
{
public:
    NotNode(const Span& span_);
    NotNode(const Span& span_, Node* subject_);
    NodeType GetType() const override { return NodeType::notNode; }
};

class UnaryPlusNode : public UnaryNode
{
public:
    UnaryPlusNode(const Span& span_);
    UnaryPlusNode(const Span& span_, Node* subject_);
    NodeType GetType() const override { return NodeType::unaryPlusNode; }
};

class UnaryMinusNode : public UnaryNode
{
public:
    UnaryMinusNode(const Span& span_);
    UnaryMinusNode(const Span& span_, Node* subject_);
    NodeType GetType() const override { return NodeType::unaryMinusNode; }
};

class ComplementNode : public UnaryNode
{
public:
    ComplementNode(const Span& span_);
    ComplementNode(const Span& span_, Node* subject_);
    NodeType GetType() const override { return NodeType::complementNode; }
};

class PrefixIncNode : public UnaryNode
{
public:
    PrefixIncNode(const Span& span_);
    PrefixIncNode(const Span& span_, Node* subject_);
    NodeType GetType() const override { return NodeType::prefixIncNode; }
};

class PrefixDecNode : public UnaryNode
{
public:
    PrefixDecNode(const Span& span_);
    PrefixDecNode(const Span& span_, Node* subject_);
    NodeType GetType() const override { return NodeType::prefixDecNode; }
};

class SizeOfNode : public UnaryNode
{
public:
    SizeOfNode(const Span& span_);
    SizeOfNode(const Span& span_, Node* subject_);
    NodeType GetType() const override { return NodeType::sizeOfNode; }
};

class TypeNameNode : public UnaryNode
{
public:
    TypeNameNode(const Span& span_);
    TypeNameNode(const Span& span_, Node* subject_);
    NodeType GetType() const override { return NodeType::typeNameNode; }
};

class CastNode : public Node
{
public:
    CastNode(const Span& span_);
    CastNode(const Span& span_, Node* targetTypeExpr_, Node* sourceExpr_);
    NodeType GetType() const override { return NodeType::castNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    std::unique_ptr<Node> targetTypeExpr;
    std::unique_ptr<Node> sourceExpr;
};

class NewNode : public Node
{
public:
    NewNode(const Span& span_);
    NewNode(const Span& span_, Node* typeExpr_);
    NodeType GetType() const override { return NodeType::newNode; }
    void AddArgument(Node* argument) override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    std::unique_ptr<Node> typeExpr;
    NodeList arguments;
};

class ConstructNode : public Node
{
public:
    ConstructNode(const Span& span_);
    ConstructNode(const Span& span_, Node* typeExpr_);
    NodeType GetType() const override { return NodeType::constructNode; }
    void AddArgument(Node* argument) override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    std::unique_ptr<Node> typeExpr;
    NodeList arguments;
};

class ThisNode : public Node
{
public:
    ThisNode(const Span& span_);
    NodeType GetType() const override { return NodeType::thisNode; }
};

class BaseNode : public Node
{
public:
    BaseNode(const Span& span_);
    NodeType GetType() const override { return NodeType::baseNode; }
};

} } // namespace Cm::Ast

#endif //CM_AST_EXPRESSION_INCLUDED
