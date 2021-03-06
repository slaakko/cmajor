/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
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
    bool IsEnumTypeNode() const override { return true; }
    Node* Clone(CloneContext& cloneContext) const override;
    void SetUnderlyingType(Node* underlyingType_);
    Node* GetUnderlyingType() const { return underlyingType.get(); }
    void AddConstant(Node* constant);
    Node* GetLastConstant();
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    std::string Name() const override;
    std::string ToString() const;
    Specifiers GetSpecifiers() const { return specifiers; }
    IdentifierNode* Id() const { return id.get(); }
    void Accept(Visitor& visitor) override;
private:
    Specifiers specifiers;
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<Node> underlyingType;
    NodeList constants;
};

class EnumConstantNode : public Node
{
public:
    EnumConstantNode(const Span& span_);
    EnumConstantNode(const Span& span_, IdentifierNode* id_, Node* value_);
    NodeType GetNodeType() const override { return NodeType::enumConstantNode; }
    bool IsEnumConstantNode() const override { return true; }
    Node* Clone(CloneContext& cloneContext) const override;
    Node* GetValue() const override { return value.get(); }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    std::string Name() const override;
    IdentifierNode* Id() const { return id.get(); }
    Node* Value() const { return value.get(); }
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<Node> value;
};

Node* MakeNextEnumConstantValue(const Span& span, EnumTypeNode* enumType);

} } // namespace Cm::Ast

#endif // CM_AST_ENUMERATION_INCLUDED
