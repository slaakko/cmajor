/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_AST_NODE_INCLUDED
#define CM_AST_NODE_INCLUDED
#include <Cm.Parsing/Scanner.hpp>
#include <cstdint>

namespace Cm { namespace Ast {

using Cm::Parsing::Span;

enum class NodeType: uint8_t
{
    boolNode, sbyteNode, byteNode, shortNode, ushortNode, intNode, uintNode, longNode, ulongNode, floatNode, doubleNode, charNode, voidNode, 
    booleanLiteralNode, sbyteLiteralNode, byteLiteralNode, shortLiteralNode, ushortLiteralNode, intLiteralNode, uintLiteralNode, longLiteralNode, ulongLiteralNode, 
    floatLiteralNode, doubleLiteralNode, charLiteralNode, stringLiteralNode, nullLiteralNode,
    derivedTypeExprNode,
    identifierNode,
    maxNode
};

class Reader;
class Writer;

class Node
{
public:
    Node();
    Node(Span span_);
    virtual ~Node();
    virtual NodeType GetType() const = 0;
    virtual void Read(Reader& reader);
    virtual void Write(Writer& writer);
    const Span& GetSpan() const { return span; }
    Span& GetSpan() { return span; }
private:
    Span span;
};

} } // namespace Cm::Ast

#endif // CM_AST_NODE_INCLUDED
