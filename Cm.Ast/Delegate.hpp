/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_DELEGATE_INCLUDED
#define CM_AST_DELEGATE_INCLUDED
#include <Cm.Ast/Node.hpp>
#include <Cm.Ast/Specifier.hpp>

namespace Cm { namespace Ast {

class IdentifierNode;

class DelegateNode : public Node
{
public:
    DelegateNode(const Span& span_);
    DelegateNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, IdentifierNode* id_);
    NodeType GetNodeType() const override { return NodeType::delegateNode; }
    Node* Clone() const override;
    void AddParameter(const Span& span_, Node* paramTypeExpr, IdentifierNode* paramId) override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    Specifiers specifiers;
    std::unique_ptr<Node> returnTypeExpr;
    std::unique_ptr<IdentifierNode> id;
    NodeList parameters;
};

class ClassDelegateNode : public Node
{
public:
    ClassDelegateNode(const Span& span_);
    ClassDelegateNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, IdentifierNode* id_);
    NodeType GetNodeType() const override { return NodeType::classDelegateNode; }
    Node* Clone() const override;
    void AddParameter(const Span& span_, Node* paramTypeExpr, IdentifierNode* paramId) override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    Specifiers specifiers;
    std::unique_ptr<Node> returnTypeExpr;
    std::unique_ptr<IdentifierNode> id;
    NodeList parameters;
};

} } // namespace Cm::Ast

#endif // CM_AST_DELEGATE_INCLUDED
