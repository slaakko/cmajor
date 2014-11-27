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
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    Specifiers specifiers;
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
};

} } // namespace Cm::Ast

#endif // CM_AST_TYPEDEF_INCLUDED
