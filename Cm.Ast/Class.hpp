/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_CLASS_INCLUDED
#define CM_AST_CLASS_INCLUDED
#include <Cm.Ast/Function.hpp>

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
    virtual bool IsClassNode() const { return true; }
    void AddTemplateParameter(TemplateParameterNode* templateParameter) override;
    void SetBaseClassTypeExpr(Node* baseClassTypeExpr_);
    void SetConstraint(WhereConstraintNode* constraint_);
    void AddMember(Node* member);
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    Node* Parent() const override;
    void SetParent(Node* parent_) override;
    std::string Name() const override;
    Specifiers GetSpecifiers() const { return specifiers; }
    IdentifierNode* Id() const { return id.get(); }
    const TemplateParameterNodeList& TemplateParameters() const { return templateParameters; }
    WhereConstraintNode* Constraint() const { return constraint.get(); }
    void Accept(Visitor& visitor) override;
private:
    Specifiers specifiers;
    std::unique_ptr<IdentifierNode> id;
    TemplateParameterNodeList templateParameters;
    std::unique_ptr<Node> baseClassTypeExpr;
    std::unique_ptr<WhereConstraintNode> constraint;
    NodeList members;
    Node* parent;
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
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<IdentifierNode> memberId;
};

class BaseInitializerNode : public InitializerNode
{
public:
    BaseInitializerNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::baseInitializerNode; }
    Node* Clone() const override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
};

class ThisInitializerNode : public InitializerNode
{
public:
    ThisInitializerNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::thisInitializerNode; }
    Node* Clone() const override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
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
    std::string ToString() const;
    void Accept(Visitor& visitor);
private:
    std::vector<std::unique_ptr<InitializerNode>> initializerNodes;
};

class StaticConstructorNode : public FunctionNode
{
public:
    StaticConstructorNode(const Span& span_);
    StaticConstructorNode(const Span& span_, Specifiers specifiers_);
    NodeType GetNodeType() const override { return NodeType::staticConstructorNode; }
    void AddInitializer(InitializerNode* initializer) override;
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    InitializerNodeList initializers;
};

class ConstructorNode : public FunctionNode
{
public:
    ConstructorNode(const Span& span_);
    ConstructorNode(const Span& span_, Specifiers specifiers_);
    NodeType GetNodeType() const override { return NodeType::constructorNode; }
    void AddInitializer(InitializerNode* initializer) override;
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    InitializerNodeList initializers;
};

class DestructorNode : public FunctionNode
{
public:
    DestructorNode(const Span& span_);
    DestructorNode(const Span& span_, Specifiers specifiers_, CompoundStatementNode* body_);
    NodeType GetNodeType() const override { return NodeType::destructorNode; }
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
};

class MemberFunctionNode : public FunctionNode
{
public:
    MemberFunctionNode(const Span& span_);
    MemberFunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, FunctionGroupIdNode* groupId_);
    NodeType GetNodeType() const override { return NodeType::memberFunctionNode; }
    void SetConst();
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    bool IsConst() const { return isConst; }
private:
    bool isConst;
};

class ConversionFunctionNode : public MemberFunctionNode
{
public:
    ConversionFunctionNode(const Span& span_);
    ConversionFunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, bool setConst_, WhereConstraintNode* constraint_, CompoundStatementNode* body_);
    NodeType GetNodeType() const override { return NodeType::conversionFunctionNode; }
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
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
    void Print(CodeFormatter& formatter) override;
    Node* Parent() const override;
    void SetParent(Node* parent_) override;
    void Accept(Visitor& visitor) override;
    Specifiers GetSpecifiers() const { return specifiers; }
    Node* TypeExpr() const { return typeExpr.get(); }
    IdentifierNode* Id() const { return id.get(); }
private:
    Specifiers specifiers;
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
    Node* parent;
};

} } // namespace Cm::Ast

#endif // CM_AST_CLASS_INCLUDED
