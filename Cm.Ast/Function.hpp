/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_FUNCTION_INCLUDED
#define CM_AST_FUNCTION_INCLUDED
#include <Cm.Ast/Specifier.hpp>
#include <Cm.Ast/Parameter.hpp>
#include <Cm.Ast/Template.hpp>

namespace Cm { namespace Ast {

class FunctionGroupIdNode : public Node
{
public:
    FunctionGroupIdNode(const Span& span_);
    FunctionGroupIdNode(const Span& span_, const std::string& functionGroupId_);
    NodeType GetNodeType() const override { return NodeType::functionGroupIdNode; }
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    std::string functionGroupId;
};

class WhereConstraintNode;
class CompoundStatementNode;

class FunctionNode : public Node
{
public:
    FunctionNode(const Span& span_);
    FunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, FunctionGroupIdNode* groupId_);
    NodeType GetNodeType() const override { return NodeType::functionNode; }
    void AddTemplateParameter(TemplateParameterNode* templateParameter) override;
    void AddParameter(ParameterNode* parameter) override;
    void SetConstraint(WhereConstraintNode* constraint_);
    void SetBody(CompoundStatementNode* body_);
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    Specifiers specifiers;
    std::unique_ptr<Node> returnTypeExpr;
    std::unique_ptr<FunctionGroupIdNode> groupId;
    TemplateParameterNodeList templateParameters;
    ParameterNodeList parameters;
    std::unique_ptr<WhereConstraintNode> constraint;
    std::unique_ptr<CompoundStatementNode> body;
};

} } // namespace Cm::Ast

#endif // CM_AST_FUNCTION_INCLUDED
