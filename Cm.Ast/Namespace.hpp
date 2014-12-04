/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_NAMESPACE_INCLUDED
#define CM_AST_NAMESPACE_INCLUDED
#include <Cm.Ast/Node.hpp>

namespace Cm { namespace Ast {

class NamespaceNode : public Node
{
public:
    NamespaceNode(const Span& span_);
    NamespaceNode(const Span& span_, IdentifierNode* id_);
    NodeType GetNodeType() const override { return NodeType::namespaceNode; }
    void AddMember(Node* member);
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    Node* Parent() const override;
    void SetParent(Node* parent_) override;
    void Accept(Visitor& visitor) override;
    IdentifierNode* Id() const { return id.get(); }
private:
    std::unique_ptr<IdentifierNode> id;
    NodeList members;
    Node* parent;
};

class AliasNode : public Node
{
public:
    AliasNode(const Span& span_);
    AliasNode(const Span& span_, IdentifierNode* id_, IdentifierNode* qid_);
    NodeType GetNodeType() const override { return NodeType::aliasNode; }
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    Node* Parent() const override;
    void SetParent(Node* parent_) override;
    void Accept(Visitor& visitor) override;
    IdentifierNode* Id() const { return id.get(); }
    IdentifierNode* Qid() const { return qid.get(); }
private:
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<IdentifierNode> qid;
    Node* parent;
};

class NamespaceImportNode : public Node
{
public:
    NamespaceImportNode(const Span& span_);
    NamespaceImportNode(const Span& span_, IdentifierNode* ns_);
    NodeType GetNodeType() const override { return NodeType::namespaceImportNode; }
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    Node* Parent() const override;
    void SetParent(Node* parent_) override;
    void Accept(Visitor& visitor) override;
    IdentifierNode* Ns() const { return ns.get(); }
private:
    std::unique_ptr<IdentifierNode> ns;
    Node* parent;
};

} } // namespace Cm::Ast

#endif // CM_AST_NAMESPACE_INCLUDED
