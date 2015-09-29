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
    NodeType GetNodeType() const override { return NodeType::equivalenceNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::equivalence; }
    virtual std::string GetOpStr() const { return "<=>"; }
    void Accept(Visitor& visitor) override;
};

class ImplicationNode : public BinaryNode
{
public:
    ImplicationNode(const Span& span_);
    ImplicationNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::implicationNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::implication; }
    virtual std::string GetOpStr() const { return "=>"; }
    void Accept(Visitor& visitor) override;
};

class DisjunctionNode : public BinaryNode
{
public:
    DisjunctionNode(const Span& span_);
    DisjunctionNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::disjunctionNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::disjunction; }
    virtual std::string GetOpStr() const { return "||"; }
    void Accept(Visitor& visitor) override;
};

class ConjunctionNode : public BinaryNode
{
public:
    ConjunctionNode(const Span& span_);
    ConjunctionNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::conjunctionNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::conjunction; }
    virtual std::string GetOpStr() const { return "&&"; }
    void Accept(Visitor& visitor) override;
};

class BitOrNode : public BinaryNode
{
public:
    BitOrNode(const Span& span_);
    BitOrNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::bitOrNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::bitOr; }
    virtual std::string GetOpStr() const { return "|"; }
    void Accept(Visitor& visitor) override;
};

class BitXorNode : public BinaryNode
{
public:
    BitXorNode(const Span& span_);
    BitXorNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::bitXorNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::bitXor; }
    virtual std::string GetOpStr() const { return "^"; }
    void Accept(Visitor& visitor) override;
};

class BitAndNode : public BinaryNode
{
public:
    BitAndNode(const Span& span_);
    BitAndNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::bitAndNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::bitAnd; }
    virtual std::string GetOpStr() const { return "&"; }
    void Accept(Visitor& visitor) override;
};

class EqualNode : public BinaryNode
{
public:
    EqualNode(const Span& span_);
    EqualNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::equalNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::equality; }
    virtual std::string GetOpStr() const { return "=="; }
    void Accept(Visitor& visitor) override;
};

class NotEqualNode : public BinaryNode
{
public:
    NotEqualNode(const Span& span_);
    NotEqualNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::notEqualNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::equality; }
    virtual std::string GetOpStr() const { return "!="; }
    void Accept(Visitor& visitor) override;
};

class LessNode : public BinaryNode
{
public:
    LessNode(const Span& span_);
    LessNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::lessNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::relational; }
    virtual std::string GetOpStr() const { return "<"; }
    void Accept(Visitor& visitor) override;
};

class GreaterNode : public BinaryNode
{
public:
    GreaterNode(const Span& span_);
    GreaterNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::greaterNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::relational; }
    virtual std::string GetOpStr() const { return ">"; }
    void Accept(Visitor& visitor) override;
};

class LessOrEqualNode : public BinaryNode
{
public:
    LessOrEqualNode(const Span& span_);
    LessOrEqualNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::lessOrEqualNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::relational; }
    virtual std::string GetOpStr() const { return "<="; }
    void Accept(Visitor& visitor) override;
};

class GreaterOrEqualNode : public BinaryNode
{
public:
    GreaterOrEqualNode(const Span& span_);
    GreaterOrEqualNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::greaterOrEqualNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::relational; }
    virtual std::string GetOpStr() const { return ">="; }
    void Accept(Visitor& visitor) override;
};

class ShiftLeftNode : public BinaryNode
{
public:
    ShiftLeftNode(const Span& span_);
    ShiftLeftNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::shiftLeftNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::shift; }
    virtual std::string GetOpStr() const { return "<<"; }
    void Accept(Visitor& visitor) override;
};

class ShiftRightNode : public BinaryNode
{
public:
    ShiftRightNode(const Span& span_);
    ShiftRightNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::shiftRightNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::shift; }
    virtual std::string GetOpStr() const { return ">>"; }
    void Accept(Visitor& visitor) override;
};

class AddNode : public BinaryNode
{
public:
    AddNode(const Span& span_);
    AddNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::addNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::additive; }
    virtual std::string GetOpStr() const { return "+"; }
    void Accept(Visitor& visitor) override;
};

class SubNode : public BinaryNode
{
public:
    SubNode(const Span& span_);
    SubNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::subNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::additive; }
    virtual std::string GetOpStr() const { return "-"; }
    void Accept(Visitor& visitor) override;
};

class MulNode : public BinaryNode
{
public:
    MulNode(const Span& span_);
    MulNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::mulNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::multiplicative; }
    virtual std::string GetOpStr() const { return "*"; }
    void Accept(Visitor& visitor) override;
};

class DivNode : public BinaryNode
{
public:
    DivNode(const Span& span_);
    DivNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::divNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::multiplicative; }
    virtual std::string GetOpStr() const { return "/"; }
    void Accept(Visitor& visitor) override;
};

class RemNode : public BinaryNode
{
public:
    RemNode(const Span& span_);
    RemNode(const Span& span_, Node* left_, Node* right_);
    NodeType GetNodeType() const override { return NodeType::remNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::multiplicative; }
    virtual std::string GetOpStr() const { return "%"; }
    void Accept(Visitor& visitor) override;
};

class InvokeNode : public UnaryNode
{
public:
    InvokeNode(const Span& span_);
    InvokeNode(const Span& span_, Node* subject_);
    NodeType GetNodeType() const override { return NodeType::invokeNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void AddArgument(Node* argument) override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    Rank GetRank() const override { return Rank::postfix; }
    std::string ToString() const override;
    Node* Subject() const { return Child(); }
    void Accept(Visitor& visitor) override;
private:
    NodeList arguments;
};

class IndexNode : public UnaryNode
{
public:
    IndexNode(const Span& span_);
    IndexNode(const Span& span_, Node* subject_, Node* index_);
    NodeType GetNodeType() const override { return NodeType::indexNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    Rank GetRank() const override { return Rank::postfix; }
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
    Node* Subject() const { return Child(); }
    Node* Index() const { return index.get(); }
private:
    std::unique_ptr<Node> index;
};

class DotNode : public UnaryNode
{
public:
    DotNode(const Span& span_);
    DotNode(const Span& span_, Node* subject_, IdentifierNode* memberId_);
    NodeType GetNodeType() const override { return NodeType::dotNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::postfix; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    std::string DocId() const override;
    Node* Subject() const { return Child(); }
    IdentifierNode* MemberId() const { return memberId.get(); }
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<IdentifierNode> memberId;
};

class ArrowNode : public UnaryNode
{
public:
    ArrowNode(const Span& span_);
    ArrowNode(const Span& span_, Node* subject_, IdentifierNode* memberId_);
    NodeType GetNodeType() const override { return NodeType::arrowNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    Rank GetRank() const override { return Rank::postfix; }
    std::string ToString() const override;
    Node* Subject() const { return Child(); }
    IdentifierNode* MemberId() const { return memberId.get(); }
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<IdentifierNode> memberId;
};

class PostfixIncNode : public UnaryNode
{
public:
    PostfixIncNode(const Span& span_);
    PostfixIncNode(const Span& span_, Node* subject_);
    NodeType GetNodeType() const override { return NodeType::postfixIncNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::postfix; }
    std::string ToString() const override;
    Node* Subject() const { return Child(); }
    void Accept(Visitor& visitor) override;
};

class PostfixDecNode : public UnaryNode
{
public:
    PostfixDecNode(const Span& span_);
    PostfixDecNode(const Span& span_, Node* subject_);
    NodeType GetNodeType() const override { return NodeType::postfixDecNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::postfix; }
    std::string ToString() const override;
    Node* Subject() const { return Child(); }
    void Accept(Visitor& visitor) override;
};

class DerefNode : public UnaryNode
{
public:
    DerefNode(const Span& span_);
    DerefNode(const Span& span_, Node* subject_);
    NodeType GetNodeType() const override { return NodeType::derefNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::prefix; }
    virtual std::string GetOpStr() const { return "*";  }
    Node* Subject() const { return Child(); }
    void Accept(Visitor& visitor) override;
};

class AddrOfNode : public UnaryNode
{
public:
    AddrOfNode(const Span& span_);
    AddrOfNode(const Span& span_, Node* subject_);
    NodeType GetNodeType() const override { return NodeType::addrOfNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::prefix; }
    virtual std::string GetOpStr() const { return "&"; }
    Node* Subject() const { return Child(); }
    void Accept(Visitor& visitor) override;
};

class NotNode : public UnaryNode
{
public:
    NotNode(const Span& span_);
    NotNode(const Span& span_, Node* subject_);
    NodeType GetNodeType() const override { return NodeType::notNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::prefix; }
    virtual std::string GetOpStr() const { return "!"; }
    void Accept(Visitor& visitor) override;
};

class UnaryPlusNode : public UnaryNode
{
public:
    UnaryPlusNode(const Span& span_);
    UnaryPlusNode(const Span& span_, Node* subject_);
    NodeType GetNodeType() const override { return NodeType::unaryPlusNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::prefix; }
    virtual std::string GetOpStr() const { return "+"; }
    void Accept(Visitor& visitor) override;
};

class UnaryMinusNode : public UnaryNode
{
public:
    UnaryMinusNode(const Span& span_);
    UnaryMinusNode(const Span& span_, Node* subject_);
    NodeType GetNodeType() const override { return NodeType::unaryMinusNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::prefix; }
    virtual std::string GetOpStr() const { return "-"; }
    void Accept(Visitor& visitor) override;
};

class ComplementNode : public UnaryNode
{
public:
    ComplementNode(const Span& span_);
    ComplementNode(const Span& span_, Node* subject_);
    NodeType GetNodeType() const override { return NodeType::complementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::prefix; }
    virtual std::string GetOpStr() const { return "~"; }
    void Accept(Visitor& visitor) override;
};

class PrefixIncNode : public UnaryNode
{
public:
    PrefixIncNode(const Span& span_);
    PrefixIncNode(const Span& span_, Node* subject_);
    NodeType GetNodeType() const override { return NodeType::prefixIncNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::prefix; }
    virtual std::string GetOpStr() const { return "++"; }
    void Accept(Visitor& visitor) override;
};

class PrefixDecNode : public UnaryNode
{
public:
    PrefixDecNode(const Span& span_);
    PrefixDecNode(const Span& span_, Node* subject_);
    NodeType GetNodeType() const override { return NodeType::prefixDecNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    Rank GetRank() const override { return Rank::prefix; }
    virtual std::string GetOpStr() const { return "--"; }
    void Accept(Visitor& visitor) override;
};

class SizeOfNode : public UnaryNode
{
public:
    SizeOfNode(const Span& span_);
    SizeOfNode(const Span& span_, Node* subject_);
    NodeType GetNodeType() const override { return NodeType::sizeOfNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    std::string ToString() const override { return "sizeof(" + Child()->ToString() + ")";  }
    Node* Subject() const { return Child(); }
    void Accept(Visitor& visitor) override;
};

class TypeNameNode : public UnaryNode
{
public:
    TypeNameNode(const Span& span_);
    TypeNameNode(const Span& span_, Node* subject_);
    NodeType GetNodeType() const override { return NodeType::typeNameNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    std::string ToString() const override { return "typename(" + Child()->ToString() + ")"; }
    Node* Subject() const { return Child(); }
    void Accept(Visitor& visitor) override;
};

class CastNode : public Node
{
public:
    CastNode(const Span& span_);
    CastNode(const Span& span_, Node* targetTypeExpr_, Node* sourceExpr_);
    NodeType GetNodeType() const override { return NodeType::castNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    Node* TargetTypeExpr() const { return targetTypeExpr.get(); }
    Node* SourceExpr() const { return sourceExpr.get(); }
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<Node> targetTypeExpr;
    std::unique_ptr<Node> sourceExpr;
};

class IsNode : public Node
{
public:
    IsNode(const Span& span_);
    IsNode(const Span& span_, Node* expr_, Node* typeExpr_);
    NodeType GetNodeType() const override { return NodeType::isNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
    Node* Expr() const { return expr.get(); }
    Node* TypeExpr() const { return typeExpr.get(); }
private:
    std::unique_ptr<Node> expr;
    std::unique_ptr<Node> typeExpr;
};

class AsNode : public Node
{
public:
    AsNode(const Span& span_);
    AsNode(const Span& span_, Node* expr_, Node* typeExpr_);
    NodeType GetNodeType() const override { return NodeType::isNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
    Node* Expr() const { return expr.get(); }
    Node* TypeExpr() const { return typeExpr.get(); }
private:
    std::unique_ptr<Node> expr;
    std::unique_ptr<Node> typeExpr;
};

class NewNode : public Node
{
public:
    NewNode(const Span& span_);
    NewNode(const Span& span_, Node* typeExpr_);
    NodeType GetNodeType() const override { return NodeType::newNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void AddArgument(Node* argument) override;
    NodeList& Arguments() { return arguments; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    Node* TypeExpr() const { return typeExpr.get(); }
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<Node> typeExpr;
    NodeList arguments;
};

class ConstructNode : public Node
{
public:
    ConstructNode(const Span& span_);
    ConstructNode(const Span& span_, Node* typeExpr_);
    NodeType GetNodeType() const override { return NodeType::constructNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void AddArgument(Node* argument) override;
    NodeList& Arguments() { return arguments; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    Node* TypeExpr() const { return typeExpr.get(); }
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<Node> typeExpr;
    NodeList arguments;
};

class ThisNode : public Node
{
public:
    ThisNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::thisNode; }
    bool IsThisNode() const override { return true; }
    Node* Clone(CloneContext& cloneContext) const override;
    std::string ToString() const override { return "this"; }
    void Accept(Visitor& visitor) override;
};

class BaseNode : public Node
{
public:
    BaseNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::baseNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    std::string ToString() const override { return "base";  }
    void Accept(Visitor& visitor) override;
};

} } // namespace Cm::Ast

#endif //CM_AST_EXPRESSION_INCLUDED
