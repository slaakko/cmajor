/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_CONCEPT_INCLUDED
#define CM_AST_CONCEPT_INCLUDED
#include <Cm.Ast/Function.hpp>

namespace Cm { namespace Ast {

class AxiomStatementNode;

class AxiomStatementNodeList
{
public:
    typedef std::vector<std::unique_ptr<AxiomStatementNode>>::const_iterator const_iterator;
    AxiomStatementNodeList();
    const_iterator begin() const { return axiomStatementNodes.begin(); }
    const_iterator end() const { return axiomStatementNodes.end(); }
    int Count() const { return int(axiomStatementNodes.size()); }
    AxiomStatementNode* operator[](int index) const { return axiomStatementNodes[index].get(); }
    AxiomStatementNode* Back() const { return axiomStatementNodes.back().get(); }
    void Add(AxiomStatementNode* statement) { axiomStatementNodes.push_back(std::unique_ptr<AxiomStatementNode>(statement)); }
    void Read(Reader& reader);
    void Write(Writer& writer);
    void Print(CodeFormatter& formatter);
    void Accept(Visitor& visitor);
    void SetParent(Node* parent);
private:
    std::vector<std::unique_ptr<AxiomStatementNode>> axiomStatementNodes;
};

class ConstraintNode: public Node
{
public:
    ConstraintNode(const Span& span_);
    virtual bool IsConstraintNode() const { return true; }
};

class ConstraintNodeList
{
public:
    typedef std::vector<std::unique_ptr<ConstraintNode>>::const_iterator const_iterator;
    ConstraintNodeList();
    const_iterator begin() const { return constraintNodes.begin(); }
    const_iterator end() const { return constraintNodes.end(); }
    int Count() const { return int(constraintNodes.size()); }
    ConstraintNode* operator[](int index) const { return constraintNodes[index].get(); }
    ConstraintNode* Back() const { return constraintNodes.back().get(); }
    void Add(ConstraintNode* constraint) { constraintNodes.push_back(std::unique_ptr<ConstraintNode>(constraint)); }
    void Read(Reader& reader);
    void Write(Writer& writer);
    void Print(CodeFormatter& formatter);
    void Accept(Visitor& visitor);
    void SetParent(Node* parent);
private:
    std::vector<std::unique_ptr<ConstraintNode>> constraintNodes;
};

class BinaryConstraintNode : public ConstraintNode
{
public:
    BinaryConstraintNode(const Span& span_);
    BinaryConstraintNode(const Span& span_, ConstraintNode* left_, ConstraintNode* right_);
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    ConstraintNode* Left() const { return left.get(); }
    ConstraintNode* Right() const { return right.get(); }
private:
    std::unique_ptr<ConstraintNode> left;
    std::unique_ptr<ConstraintNode> right;
};

class DisjunctiveConstraintNode : public BinaryConstraintNode
{
public:
    DisjunctiveConstraintNode(const Span& span_);
    DisjunctiveConstraintNode(const Span& span_, ConstraintNode* left_, ConstraintNode* right_);
    NodeType GetNodeType() const override { return NodeType::disjunctiveConstraintNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
};

class ConjunctiveConstraintNode : public BinaryConstraintNode
{
public:
    ConjunctiveConstraintNode(const Span& span_);
    ConjunctiveConstraintNode(const Span& span_, ConstraintNode* left_, ConstraintNode* right_);
    NodeType GetNodeType() const override { return NodeType::conjunctiveConstraintNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
};

class WhereConstraintNode : public ConstraintNode
{
public:
    WhereConstraintNode(const Span& span_);
    WhereConstraintNode(const Span& span_, ConstraintNode* constraint_);
    NodeType GetNodeType() const override { return NodeType::whereConstraintNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<ConstraintNode> constraint;
};

class IsConstraintNode : public ConstraintNode
{
public:
    IsConstraintNode(const Span& span_);
    IsConstraintNode(const Span& span_, Node* typeExpr_, Node* conceptOrTypeName_);
    NodeType GetNodeType() const override { return NodeType::isConstraintNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    Node* TypeExpr() const { return typeExpr.get(); }
    Node* ConceptOrTypeName() const { return conceptOrTypeName.get();  }
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<Node> conceptOrTypeName;
};

class MultiParamConstraintNode : public ConstraintNode
{
public:
    MultiParamConstraintNode(const Span& span_);
    MultiParamConstraintNode(const Span& span_, IdentifierNode* conceptId_);
    NodeType GetNodeType() const override { return NodeType::multiParamConstraintNode; }
    void AddTypeExpr(Node* typeExpr);
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
    IdentifierNode* ConceptId() const { return conceptId.get(); }
    const NodeList& TypeExprNodes() const { return typeExprNodes; }
private:
    std::unique_ptr<IdentifierNode> conceptId;
    NodeList typeExprNodes;
};

class TypenameConstraintNode : public ConstraintNode
{
public:
    TypenameConstraintNode(const Span& span_);
    TypenameConstraintNode(const Span& span_, Node* typeId_);
    NodeType GetNodeType() const override { return NodeType::typenameConstraintNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    Node* TypeId() const { return typeId.get(); }
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<Node> typeId;
};

class SignatureConstraintNode : public ConstraintNode
{
public:
    SignatureConstraintNode(const Span& span_);
};

class ConstructorConstraintNode : public SignatureConstraintNode
{
public:
    ConstructorConstraintNode(const Span& span_);
    void AddParameter(ParameterNode* parameter) override;
    NodeType GetNodeType() const override { return NodeType::constructorConstraintNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
    const ParameterNodeList& Parameters() const { return parameters; }
private:
    ParameterNodeList parameters;
};

class DestructorConstraintNode : public SignatureConstraintNode
{
public:
    DestructorConstraintNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::destructorConstraintNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
};

class MemberFunctionConstraintNode : public SignatureConstraintNode
{
public:
    MemberFunctionConstraintNode(const Span& span_);
    MemberFunctionConstraintNode(const Span& span_, Node* returnTypeExpr_, IdentifierNode* typeParamId_, FunctionGroupIdNode* functionGroupId_);
    NodeType GetNodeType() const override { return NodeType::memberFunctionConstraintNode; }
    void AddParameter(ParameterNode* parameter) override;
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
    Node* ReturnTypeExpr() const { return returnTypeExpr.get(); }
    IdentifierNode* TypeParamId() const { return typeParamId.get(); }
    FunctionGroupIdNode* GroupId() const { return functionGroupId.get(); }
    const ParameterNodeList& Parameters() const { return parameters; }
private:
    std::unique_ptr<Node> returnTypeExpr;
    std::unique_ptr<IdentifierNode> typeParamId;
    std::unique_ptr<FunctionGroupIdNode> functionGroupId;
    ParameterNodeList parameters;
};

class FunctionConstraintNode : public SignatureConstraintNode
{
public:
    FunctionConstraintNode(const Span& span_);
    FunctionConstraintNode(const Span& span_, Node* returnTypeExpr_, FunctionGroupIdNode* functionGroupId_);
    NodeType GetNodeType() const override { return NodeType::functionConstraintNode; }
    void AddParameter(ParameterNode* parameter) override;
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
    Node* ReturnTypeExpr() const { return returnTypeExpr.get(); }
    FunctionGroupIdNode* GroupId() const { return functionGroupId.get(); }
    const ParameterNodeList& Parameters() const { return parameters; }
private:
    std::unique_ptr<Node> returnTypeExpr;
    std::unique_ptr<FunctionGroupIdNode> functionGroupId;
    ParameterNodeList parameters;
};

class AxiomStatementNode : public Node
{
public:
    AxiomStatementNode(const Span& span_);
    AxiomStatementNode(const Span& span_, Node* expression_);
    NodeType GetNodeType() const override { return NodeType::axiomStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
    Node* Expression() const { return expression.get(); }
private:
    std::unique_ptr<Node> expression;
};

class AxiomNode : public Node
{ 
public:
    AxiomNode(const Span& span_);
    AxiomNode(const Span& span_, IdentifierNode* id_);
    NodeType GetNodeType() const override { return NodeType::axiomNode; }
    void AddParameter(ParameterNode* parameter) override;
    void AddStatement(AxiomStatementNode* statement);
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter);
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<IdentifierNode> id;
    ParameterNodeList parameters;
    AxiomStatementNodeList axiomStatements;
};

class ConceptIdNode : public Node
{
public:
    ConceptIdNode(const Span& span_);
    ConceptIdNode(const Span& span_, IdentifierNode* id_);
    NodeType GetNodeType() const override { return NodeType::conceptIdNode; }
    void AddTypeParameter(Node* typeParameter);
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
    IdentifierNode* Id() const { return id.get(); }
    const NodeList& TypeParameters() const { return typeParameters; }
private:
    std::unique_ptr<IdentifierNode> id;
    NodeList typeParameters;
};

class ConceptNode : public Node
{
public:
    ConceptNode(const Span& span_);
    ConceptNode(const Span& span_, Specifiers specifiers_, IdentifierNode* id_);
    NodeType GetNodeType() const override { return NodeType::conceptNode; }
    bool IsConceptNode() const override { return true; }
    const std::string& FirstTypeParameter() const;
    void AddTypeParameter(Node* typeParameter);
    void SetRefinement(ConceptIdNode* refinement_);
    void AddConstraint(ConstraintNode* constraint);
    void AddAxiom(AxiomNode* axiom);
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    std::string Name() const override;
    void Accept(Visitor& visitor) override;
    Specifiers GetSpecifiers() const { return specifiers; }
    IdentifierNode* Id() const { return id.get(); }
    const NodeList& TypeParameters() const { return typeParameters; }
    ConceptIdNode* Refinement() const { return refinement.get(); }
    ConstraintNodeList& Constraints() { return constraints; }
    NodeList& Axioms() { return axioms; }
    virtual bool IsCommonConceptNode() const { return false; }
private:
    Specifiers specifiers;
    std::unique_ptr<IdentifierNode> id;
    NodeList typeParameters;
    std::unique_ptr<ConceptIdNode> refinement;
    ConstraintNodeList constraints;
    NodeList axioms;
};

} } // namespace Cm::Ast

#endif // CM_AST_CONCEPT_INCLUDED
