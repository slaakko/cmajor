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

class AliasNode : public Node
{
public:
    AliasNode(const Span& span_);
    AliasNode(const Span& span_, IdentifierNode* id_, IdentifierNode* qid_);
    NodeType GetNodeType() const override { return NodeType::aliasNode; }
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<IdentifierNode> qid;
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
private:
    std::unique_ptr<IdentifierNode> ns;
};

} } // namespace Cm::Ast

#endif // CM_AST_NAMESPACE_INCLUDED
