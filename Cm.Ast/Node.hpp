/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_AST_NODE_INCLUDED
#define CM_AST_NODE_INCLUDED
#include <Cm.Ast/Rank.hpp>
#include <Cm.Parsing/Scanner.hpp>
#include <Cm.Util/CodeFormatter.hpp>
#include <memory>
#include <cstdint>

namespace Cm { namespace Ast {

using Cm::Parsing::Span;
using Cm::Util::CodeFormatter;

enum class NodeType: uint8_t
{
    boolNode, sbyteNode, byteNode, shortNode, ushortNode, intNode, uintNode, longNode, ulongNode, floatNode, doubleNode, charNode, voidNode,
    booleanLiteralNode, sbyteLiteralNode, byteLiteralNode, shortLiteralNode, ushortLiteralNode, intLiteralNode, uintLiteralNode, longLiteralNode, ulongLiteralNode,
    floatLiteralNode, doubleLiteralNode, charLiteralNode, stringLiteralNode, nullLiteralNode,
    derivedTypeExprNode,
    equivalenceNode, implicationNode, disjunctionNode, conjunctionNode, bitOrNode, bitXorNode, bitAndNode, equalNode, notEqualNode, lessNode, greaterNode, lessOrEqualNode, greaterOrEqualNode,
    shiftLeftNode, shiftRightNode, addNode, subNode, mulNode, divNode, remNode, invokeNode, indexNode, dotNode, arrowNode, postfixIncNode, postfixDecNode, derefNode, addrOfNode,
    notNode, unaryPlusNode, unaryMinusNode, complementNode, prefixIncNode, prefixDecNode, sizeOfNode, typeNameNode,
    castNode, newNode, constructNode, thisNode, baseNode,
    identifierNode, templateIdNode,
    enumTypeNode, enumConstantNode, constantNode, parameterNode, delegateNode, classDelegateNode, typedefNode,
    labelNode,
    simpleStatementNode, returnStatementNode, conditionalStatementNode, switchStatementNode, caseStatementNode, defaultStatementNode, gotoCaseStatementNode, gotoDefaultStatementNode,
    whileStatementNode, doStatementNode, rangeForStatementNode, forStatementNode, compoundStatementNode, breakStatementNode, continueStatemetNode, gotoStatementNode, typedefStatementNode,
    assignmentStatementNode, constructionStatementNode, deleteStatementNode, destroyStatementNode, throwStatementNode, tryStatementNode, catchNode, assertStatementNode,
    condCompDisjunctionNode, condCompConjunctionNode, condCompNotNode, condCompPrimaryNode, condCompSymbolNode, condCompilationPartNode, condCompStatementNode,
    disjunctiveConstraintNode, conjunctiveConstraintNode, whereConstraintNode, isConstraintNode, multiParamConstraintNode, typenameConstraintNode,
    constructorConstraintNode, destructorConstraintNode, memberFunctionConstraintNode, functionConstraintNode, axiomNode, axiomStatementNode, conceptIdNode, conceptNode,
    functionGroupIdNode, templateParameterNode, functionNode, 
    classNode, memberInitializerNode, baseInitializerNode, thisInitializerNode, staticConstructorNode, constructorNode, destructorNode, memberFunctionNode, conversionFunctionNode, memberVariableNode,
    aliasNode, namespaceImportNode, namespaceNode, compileUnitNode,
    maxNode
};

class Reader;
class Writer;
class IdentifierNode;
class ParameterNode;
class TemplateParameterNode;
class InitializerNode;
class FunctionNode;
class Visitor;

class Node
{
public:
    Node();
    Node(const Span& span_);
    virtual ~Node();
    virtual NodeType GetNodeType() const = 0;
    virtual Node* Clone() const = 0;
    const Span& GetSpan() const { return span; }
    Span& GetSpan() { return span; }
    virtual void Read(Reader& reader);
    virtual void Write(Writer& writer);
    virtual void Print(CodeFormatter& formatter);
    virtual Node* GetValue() const;
    virtual void AddArgument(Node* argument);
    virtual void AddParameter(ParameterNode* parameter);
    virtual void AddTemplateParameter(TemplateParameterNode* templateParameter);
    virtual void AddInitializer(InitializerNode* initializer);
    virtual bool IsClassNode() const { return false; }
    virtual bool IsStatementNode() const { return false; }
    virtual bool IsCompoundStatementNode() const { return false; }
    virtual bool IsFunctionNode() const { return false; }
    virtual bool IsCondCompExprNode() const { return false; }
    virtual bool IsCondCompSymbolNode() const { return false; }
    virtual bool IsCondCompPartNode() const { return false; }
    virtual bool IsConstraintNode() const { return false; }
    virtual bool IsInitializerNode() const { return false; }
    virtual Rank GetRank() const { return Rank::primary; }
    virtual std::string ToString() const { return std::string(); }
    virtual std::string GetOpStr() const { return std::string(); }
    virtual Node* Parent() const;
    virtual void SetParent(Node* parent_);
    virtual std::string Name() const;
    std::string FullName() const;
    virtual void Accept(Visitor& visitor);
    virtual FunctionNode* GetFunction() const;
private:
    Span span;
};

class UnaryNode : public Node
{
public:
    UnaryNode(const Span& span_);
    UnaryNode(const Span& span_, Node* child_);
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    Node* Child() const { return child.get(); }
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<Node> child;
};


class BinaryNode : public Node
{
public:
    BinaryNode(const Span& span_);
    BinaryNode(const Span& span_, Node* left_, Node* right_);
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    Node* Left() const { return left.get(); }
    Node* Right() const { return right.get(); }
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};

class NodeList
{
public:
    typedef std::vector<std::unique_ptr<Node>>::const_iterator const_iterator;
    NodeList();
    const_iterator begin() const { return nodes.begin(); }
    const_iterator end() const { return nodes.end(); }
    int Count() const { return int(nodes.size()); }
    Node* operator[](int index) const { return nodes[index].get(); }
    Node* Back() const { return nodes.back().get(); }
    void Add(Node* node) { nodes.push_back(std::unique_ptr<Node>(node)); }
    void Read(Reader& reader);
    void Write(Writer& writer);
    void Print(CodeFormatter& formatter);
    std::string ToString() const;
    void Accept(Visitor& visitor);
private:
    std::vector<std::unique_ptr<Node>> nodes;
};

} } // namespace Cm::Ast

#endif // CM_AST_NODE_INCLUDED
