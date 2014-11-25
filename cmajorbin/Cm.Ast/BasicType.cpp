/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Ast/BasicType.hpp>

namespace Cm { namespace Ast {

BoolNode::BoolNode(const Span& span_): Node(span_)
{
}

Node* BoolNode::Clone() const
{
    return new BoolNode(GetSpan());
}

SByteNode::SByteNode(const Span& span_): Node(span_)
{
}

Node* SByteNode::Clone() const
{
    return new SByteNode(GetSpan());
}

ByteNode::ByteNode(const Span& span_): Node(span_)
{
}

Node* ByteNode::Clone() const
{
    return new ByteNode(GetSpan());
}

ShortNode::ShortNode(const Span& span_): Node(span_)
{
}

Node* ShortNode::Clone() const
{
    return new ShortNode(GetSpan());
}

UShortNode::UShortNode(const Span& span_): Node(span_)
{
}

Node* UShortNode::Clone() const
{
    return new UShortNode(GetSpan());
}

IntNode::IntNode(const Span& span_): Node(span_)
{
}

Node* IntNode::Clone() const
{
    return new IntNode(GetSpan());
}

UIntNode::UIntNode(const Span& span_): Node(span_)
{
}

Node* UIntNode::Clone() const
{
    return new UIntNode(GetSpan());
}

LongNode::LongNode(const Span& span_): Node(span_)
{
}

Node* LongNode::Clone() const
{
    return new LongNode(GetSpan());
}

ULongNode::ULongNode(const Span& span_): Node(span_)
{
}

Node* ULongNode::Clone() const
{
    return new ULongNode(GetSpan());
}

FloatNode::FloatNode(const Span& span_): Node(span_)
{
}

Node* FloatNode::Clone() const
{
    return new FloatNode(GetSpan());
}

DoubleNode::DoubleNode(const Span& span_): Node(span_)
{
}

Node* DoubleNode::Clone() const
{
    return new DoubleNode(GetSpan());
}

CharNode::CharNode(const Span& span_): Node(span_)
{
}

Node* CharNode::Clone() const
{
    return new CharNode(GetSpan());
}

VoidNode::VoidNode(const Span& span_): Node(span_)
{
}

Node* VoidNode::Clone() const
{
    return new VoidNode(GetSpan());
}

} } // namespace Cm::Ast
