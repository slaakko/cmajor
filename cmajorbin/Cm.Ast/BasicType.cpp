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

SByteNode::SByteNode(const Span& span_): Node(span_)
{
}

ByteNode::ByteNode(const Span& span_): Node(span_)
{
}

ShortNode::ShortNode(const Span& span_): Node(span_)
{
}

UShortNode::UShortNode(const Span& span_): Node(span_)
{
}

IntNode::IntNode(const Span& span_): Node(span_)
{
}

UIntNode::UIntNode(const Span& span_): Node(span_)
{
}

LongNode::LongNode(const Span& span_): Node(span_)
{
}

ULongNode::ULongNode(const Span& span_): Node(span_)
{
}

FloatNode::FloatNode(const Span& span_): Node(span_)
{
}

DoubleNode::DoubleNode(const Span& span_): Node(span_)
{
}

CharNode::CharNode(const Span& span_): Node(span_)
{
}

VoidNode::VoidNode(const Span& span_): Node(span_)
{
}

} } // namespace Cm::Ast
