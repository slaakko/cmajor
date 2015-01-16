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
    const std::string& Str() const { return functionGroupId; }
    std::string ToString() const override { return functionGroupId; }
private:
    std::string functionGroupId;
};

class WhereConstraintNode;
class CompoundStatementNode;
class CompileUnitNode;

class FunctionNode : public Node
{
public:
    FunctionNode(const Span& span_);
    FunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, FunctionGroupIdNode* groupId_);
    NodeType GetNodeType() const override { return NodeType::functionNode; }
    bool IsFunctionNode() const override { return true; }
    void AddTemplateParameter(TemplateParameterNode* templateParameter) override;
    void AddParameter(ParameterNode* parameter) override;
    void SetConstraint(WhereConstraintNode* constraint_);
    void SetBody(CompoundStatementNode* body_);
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    std::string Name() const override;
    void Accept(Visitor& visitor) override;
    Specifiers GetSpecifiers() const { return specifiers; }
    Node* ReturnTypeExpr() const { return returnTypeExpr.get(); }
    FunctionGroupIdNode* GroupId() const { return groupId.get(); }
    const TemplateParameterNodeList& TemplateParameters() const { return templateParameters; }
    const ParameterNodeList& Parameters() const { return parameters; }
    WhereConstraintNode* Constraint() const { return constraint.get(); }
    CompoundStatementNode* Body() const { return body.get(); }
    bool HasBody() const { return body != nullptr; }
    void SetCompileUnit(CompileUnitNode* compileUnit_) { compileUnit = compileUnit_; }
    CompileUnitNode* GetCompileUnit() const { return compileUnit; }
private:
    Specifiers specifiers;
    std::unique_ptr<Node> returnTypeExpr;
    std::unique_ptr<FunctionGroupIdNode> groupId;
    TemplateParameterNodeList templateParameters;
    ParameterNodeList parameters;
    std::unique_ptr<WhereConstraintNode> constraint;
    std::unique_ptr<CompoundStatementNode> body;
    CompileUnitNode* compileUnit;
};

} } // namespace Cm::Ast

#endif // CM_AST_FUNCTION_INCLUDED
