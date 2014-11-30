/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_CLASS_INCLUDED
#define CM_AST_CLASS_INCLUDED
#include <Cm.Ast/Node.hpp>
#include <Cm.Ast/Specifier.hpp>
#include <Cm.Ast/Template.hpp>
#include <Cm.Ast/Parameter.hpp>

namespace Cm { namespace Ast {

class WhereConstraintNode;
class CompoundStatementNode;
class FunctionGroupIdNode;

class ClassNode : public Node
{
public:
    ClassNode(const Span& span_);
    ClassNode(const Span& span_, Specifiers specifiers_, IdentifierNode* id_);
    NodeType GetNodeType() const override { return NodeType::classNode; }
    void AddTemplateParameter(TemplateParameterNode* templateParameter) override;
    void SetBaseClassTypeExpr(Node* baseClassTypeExpr_);
    void SetConstraint(WhereConstraintNode* constraint_);
    void AddMember(Node* member);
    const std::string& Id() const;
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    Specifiers specifiers;
    std::unique_ptr<IdentifierNode> id;
    TemplateParameterNodeList templateParameters;
    std::unique_ptr<Node> baseClassTypeExpr;
    std::unique_ptr<WhereConstraintNode> constraint;
    NodeList members;
};

class InitializerNode : public Node
{
public:
    InitializerNode(const Span& span_);
    bool IsInitializerNode() const override { return true; }
    void AddArgument(Node* argument) override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    const NodeList& Arguments() const { return arguments; }
private:
    NodeList arguments;
};

class MemberInitializerNode : public InitializerNode
{
public:
    MemberInitializerNode(const Span& span_);
    MemberInitializerNode(const Span& span_, IdentifierNode* memberId_);
    NodeType GetNodeType() const override { return NodeType::memberInitializerNode; }
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    std::unique_ptr<IdentifierNode> memberId;
};

class BaseInitializerNode : public InitializerNode
{
public:
    BaseInitializerNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::baseInitializerNode; }
    Node* Clone() const override;
};

class ThisInitializerNode : public InitializerNode
{
public:
    ThisInitializerNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::thisInitializerNode; }
    Node* Clone() const override;
};

class InitializerNodeList
{
public:
    typedef std::vector<std::unique_ptr<InitializerNode>>::const_iterator const_iterator;
    InitializerNodeList();
    const_iterator begin() const { return initializerNodes.begin(); }
    const_iterator end() const { return initializerNodes.end(); }
    int Count() const { return int(initializerNodes.size()); }
    InitializerNode* operator[](int index) const { return initializerNodes[index].get(); }
    InitializerNode* Back() const { return initializerNodes.back().get(); }
    void Add(InitializerNode* initializer) { initializerNodes.push_back(std::unique_ptr<InitializerNode>(initializer)); }
    void Read(Reader& reader);
    void Write(Writer& writer);
private:
    std::vector<std::unique_ptr<InitializerNode>> initializerNodes;
};

class StaticConstructorNode : public Node
{
public:
    StaticConstructorNode(const Span& span_);
    StaticConstructorNode(const Span& span_, Specifiers specifiers_);
    NodeType GetNodeType() const override { return NodeType::staticConstructorNode; }
    void AddInitializer(InitializerNode* initializer) override;
    void SetConstraint(WhereConstraintNode* constraint_);
    void SetBody(CompoundStatementNode* body_);
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    Specifiers specifiers;
    InitializerNodeList initializers;
    std::unique_ptr<WhereConstraintNode> constraint;
    std::unique_ptr<CompoundStatementNode> body;
};

class ConstructorNode : public Node
{
public:
    ConstructorNode(const Span& span_);
    ConstructorNode(const Span& span_, Specifiers specifiers_);
    NodeType GetNodeType() const override { return NodeType::constructorNode; }
    void AddParameter(ParameterNode* parameter) override;
    void AddInitializer(InitializerNode* initializer) override;
    void SetConstraint(WhereConstraintNode* constraint_);
    void SetBody(CompoundStatementNode* body_);
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    Specifiers specifiers;
    ParameterNodeList parameters;
    InitializerNodeList initializers;
    std::unique_ptr<WhereConstraintNode> constraint;
    std::unique_ptr<CompoundStatementNode> body;
};

class DestructorNode : public Node
{
public:
    DestructorNode(const Span& span_);
    DestructorNode(const Span& span_, Specifiers specifiers_, CompoundStatementNode* body_);
    NodeType GetNodeType() const override { return NodeType::destructorNode; }
    void AddParameter(ParameterNode* parameter) override;
    void SetConstraint(WhereConstraintNode* constraint_);
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    Specifiers specifiers;
    ParameterNodeList parameters;
    std::unique_ptr<WhereConstraintNode> constraint;
    std::unique_ptr<CompoundStatementNode> body;
};

class MemberFunctionNode : public Node
{
public:
    MemberFunctionNode(const Span& span_);
    MemberFunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, FunctionGroupIdNode* groupId_);
    NodeType GetNodeType() const override { return NodeType::memberFunctionNode; }
    void AddParameter(ParameterNode* parameter) override;
    void SetConstraint(WhereConstraintNode* constraint_);
    void SetBody(CompoundStatementNode* body_);
    void SetConst();
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    Specifiers specifiers;
    std::unique_ptr<Node> returnTypeExpr;
    std::unique_ptr<FunctionGroupIdNode> groupId;
    ParameterNodeList parameters;
    std::unique_ptr<WhereConstraintNode> constraint;
    std::unique_ptr<CompoundStatementNode> body;
};

class ConversionFunctionNode : public Node
{
public:
    ConversionFunctionNode(const Span& span_);
    ConversionFunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, bool setConst_, WhereConstraintNode* constraint_, CompoundStatementNode* body_);
    NodeType GetNodeType() const override { return NodeType::conversionFunctionNode; }
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    Specifiers specifiers;
    std::unique_ptr<Node> returnTypeExpr;
    std::unique_ptr<WhereConstraintNode> constraint;
    std::unique_ptr<CompoundStatementNode> body;
};

class MemberVariableNode : public Node
{
public:
    MemberVariableNode(const Span& span_);
    MemberVariableNode(const Span& span_, Specifiers specifiers_, Node* typeExpr_, IdentifierNode* id_);
    NodeType GetNodeType() const override { return NodeType::memberVariableNode; }
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    Specifiers specifiers;
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
};

} } // namespace Cm::Ast

#endif // CM_AST_CLASS_INCLUDED
