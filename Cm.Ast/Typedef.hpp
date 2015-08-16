/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_TYPEDEF_INCLUDED
#define CM_AST_TYPEDEF_INCLUDED
#include <Cm.Ast/Node.hpp>
#include <Cm.Ast/Specifier.hpp>

namespace Cm { namespace Ast {

class TypedefNode : public Node
{
public:
    TypedefNode(const Span& span_);
    TypedefNode(const Span& span_, Specifiers specifiers_, Node* typeExpr_, IdentifierNode* id_);
    NodeType GetNodeType() const override { return NodeType::typedefNode; }
    bool IsTypedefNode() const override { return true; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    std::string Name() const override;
    std::string ToString() const;
    Specifiers GetSpecifiers() const { return specifiers; }
    Node* TypeExpr() const { return typeExpr.get(); }
    IdentifierNode* Id() const { return id.get(); }
    void Accept(Visitor& visitor) override;
private:
    Specifiers specifiers;
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
};

} } // namespace Cm::Ast

#endif // CM_AST_TYPEDEF_INCLUDED
