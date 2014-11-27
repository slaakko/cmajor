/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_ENUMERATION_INCLUDED
#define CM_AST_ENUMERATION_INCLUDED
#include <Cm.Ast/Node.hpp>
#include <Cm.Ast/Specifier.hpp>

namespace Cm { namespace Ast {

class IdentifierNode;

class EnumTypeNode : public Node
{
public:
    EnumTypeNode(const Span& span_);
    EnumTypeNode(const Span& span_, Specifiers specifiers_, IdentifierNode* id_);
    NodeType GetNodeType() const override { return NodeType::enumTypeNode; }
    Node* Clone() const override;
    void AddConstant(Node* constant);
    Node* GetLastConstant();
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    Specifiers specifiers;
    std::unique_ptr<IdentifierNode> id;
    NodeList constants;
};

class EnumConstantNode : public Node
{
public:
    EnumConstantNode(const Span& span_);
    EnumConstantNode(const Span& span_, IdentifierNode* id_, Node* value_);
    NodeType GetNodeType() const override { return NodeType::enumConstantNode; }
    Node* Clone() const override;
    Node* GetValue() const override { return value.get(); }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<Node> value;
};

Node* MakeNextEnumConstantValue(const Span& span, EnumTypeNode* enumType);

} } // namespace Cm::Ast

#endif // CM_AST_ENUMERATION_INCLUDED
