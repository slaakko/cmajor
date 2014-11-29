/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_AST_NODE_INCLUDED
#define CM_AST_NODE_INCLUDED
#include <Cm.Parsing/Scanner.hpp>
#include <memory>
#include <cstdint>

namespace Cm { namespace Ast {

using Cm::Parsing::Span;

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
    functionGroupIdNode, 
    maxNode
};

class Reader;
class Writer;
class IdentifierNode;
class ParameterNode;

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
    virtual Node* GetValue() const;
    virtual void AddArgument(Node* argument);
    virtual void AddParameter(ParameterNode* parameter);
    virtual bool IsStatementNode() const { return false; }
    virtual bool IsCompoundStatementNode() const { return false; }
    virtual bool IsCondCompExprNode() const { return false; }
    virtual bool IsCondCompSymbolNode() const { return false; }
    virtual bool IsCondCompPartNode() const { return false; }
    virtual bool IsConstraintNode() const { return false; }
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
private:
    std::vector<std::unique_ptr<Node>> nodes;
};

} } // namespace Cm::Ast

#endif // CM_AST_NODE_INCLUDED
