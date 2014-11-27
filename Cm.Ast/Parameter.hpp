/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_PARAMETER_INCLUDED
#define CM_AST_PARAMETER_INCLUDED
#include <Cm.Ast/Node.hpp>

namespace Cm { namespace Ast {

class IdentifierNode;

class ParameterNode : public Node
{
public:
    ParameterNode(const Span& span_);
    ParameterNode(const Span& span_, Node* typeExpr_, IdentifierNode* id_);
    NodeType GetNodeType() const override { return NodeType::parameterNode; }
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    Node* TypeExpr() const { return typeExpr.get(); }
    IdentifierNode* Id() const { return id.get(); }
private:
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
};

} } // namespace Cm::Ast

#endif // CM_AST_PARAMETER_INCLUDED
