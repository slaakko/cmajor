/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Ast/BasicType.hpp>

namespace Cm { namespace Ast {

BoolNode::BoolNode()
{
}

std::ostream& BoolNode::Write(std::ostream& s)
{
    return s << NodeType::boolNode;
}

SByteNode::SByteNode()
{
}

std::ostream& SByteNode::Write(std::ostream& s)
{
    return s << NodeType::sbyteNode;
}

ByteNode::ByteNode()
{
}

std::ostream& ByteNode::Write(std::ostream& s)
{
    return s << NodeType::byteNode;
}

ShortNode::ShortNode()
{
}

std::ostream& ShortNode::Write(std::ostream& s)
{
    return s << NodeType::shortNode;
}

UShortNode::UShortNode()
{
}

std::ostream& UShortNode::Write(std::ostream& s)
{
    return s << NodeType::ushortNode;
}

IntNode::IntNode()
{
}

std::ostream& IntNode::Write(std::ostream& s)
{
    return s << NodeType::intNode;
}

UIntNode::UIntNode()
{
}

std::ostream& UIntNode::Write(std::ostream& s)
{
    return s << NodeType::uintNode;
}

LongNode::LongNode()
{
}

std::ostream& LongNode::Write(std::ostream& s)
{
    return s << NodeType::longNode;
}

ULongNode::ULongNode()
{
}

std::ostream& ULongNode::Write(std::ostream& s)
{
    return s << NodeType::ulongNode;
}

FloatNode::FloatNode()
{
}

std::ostream& FloatNode::Write(std::ostream& s)
{
    return s << NodeType::floatNode;
}

DoubleNode::DoubleNode()
{
}

std::ostream& DoubleNode::Write(std::ostream& s)
{
    return s << NodeType::doubleNode;
}

CharNode::CharNode()
{
}

std::ostream& CharNode::Write(std::ostream& s)
{
    return s << NodeType::charNode;
}

VoidNode::VoidNode()
{
}

std::ostream& VoidNode::Write(std::ostream& s)
{
    return s << NodeType::voidNode;
}

} } // namespace Cm::Ast
