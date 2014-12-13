/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_CONSTANT_INCLUDED
#define CM_AST_CONSTANT_INCLUDED
#include <Cm.Ast/Node.hpp>
#include <Cm.Ast/Specifier.hpp>

namespace Cm { namespace Ast {

class IdentifierNode;

class ConstantNode : public Node
{
public:
    ConstantNode(const Span& span_);
    ConstantNode(const Span& span_, Specifiers specifiers_, Node* typeExpr_, IdentifierNode* id_, Node* value_);
    NodeType GetNodeType() const override { return NodeType::constantNode; }
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    std::string Name() const override;
    Specifiers GetSpecifiers() const { return specifiers; }
    Node* TypeExpr() const { return typeExpr.get(); }
    IdentifierNode* Id() const { return id.get(); }
    Node* Value() const { return value.get(); }
    void Accept(Visitor& visitor) override;
    bool IsConstantNode() const override { return true; }
private:
    Specifiers specifiers;
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<Node> value;
};

} } // namespace Cm::Ast

#endif // CM_AST_CONSTANT_INCLUDED
