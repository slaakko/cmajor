/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Expression.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>

namespace Cm { namespace Ast {

EquivalenceNode::EquivalenceNode(const Span& span_) : BinaryNode(span_)
{
}

EquivalenceNode::EquivalenceNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

ImplicationNode::ImplicationNode(const Span& span_) : BinaryNode(span_)
{
}

ImplicationNode::ImplicationNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

DisjunctionNode::DisjunctionNode(const Span& span_) : BinaryNode(span_)
{
}

DisjunctionNode::DisjunctionNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

ConjunctionNode::ConjunctionNode(const Span& span_) : BinaryNode(span_)
{
}

ConjunctionNode::ConjunctionNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

BitOrNode::BitOrNode(const Span& span_) : BinaryNode(span_)
{
}

BitOrNode::BitOrNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

BitXorNode::BitXorNode(const Span& span_) : BinaryNode(span_)
{
}

BitXorNode::BitXorNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

BitAndNode::BitAndNode(const Span& span_) : BinaryNode(span_)
{
}

BitAndNode::BitAndNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

EqualNode::EqualNode(const Span& span_) : BinaryNode(span_)
{
}

EqualNode::EqualNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

NotEqualNode::NotEqualNode(const Span& span_) : BinaryNode(span_)
{
}

NotEqualNode::NotEqualNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

LessNode::LessNode(const Span& span_) : BinaryNode(span_)
{
}

LessNode::LessNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

GreaterNode::GreaterNode(const Span& span_) : BinaryNode(span_)
{
}

GreaterNode::GreaterNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

LessOrEqualNode::LessOrEqualNode(const Span& span_) : BinaryNode(span_)
{
}

LessOrEqualNode::LessOrEqualNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

GreaterOrEqualNode::GreaterOrEqualNode(const Span& span_) : BinaryNode(span_)
{
}

GreaterOrEqualNode::GreaterOrEqualNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

ShiftLeftNode::ShiftLeftNode(const Span& span_) : BinaryNode(span_)
{
}

ShiftLeftNode::ShiftLeftNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

ShiftRightNode::ShiftRightNode(const Span& span_) : BinaryNode(span_)
{
}

ShiftRightNode::ShiftRightNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

AddNode::AddNode(const Span& span_) : BinaryNode(span_)
{
}

AddNode::AddNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

SubNode::SubNode(const Span& span_) : BinaryNode(span_)
{
}

SubNode::SubNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

MulNode::MulNode(const Span& span_) : BinaryNode(span_)
{
}

MulNode::MulNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

DivNode::DivNode(const Span& span_) : BinaryNode(span_)
{
}

DivNode::DivNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

RemNode::RemNode(const Span& span_) : BinaryNode(span_)
{
}

RemNode::RemNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

InvokeNode::InvokeNode(const Span& span_) : UnaryNode(span_)
{
}

InvokeNode::InvokeNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

void InvokeNode::AddArgument(Node* argument)
{
    arguments.Add(argument);
}

void InvokeNode::Read(Reader& reader) 
{
    UnaryNode::Read(reader);
    arguments.Read(reader);
}

void InvokeNode::Write(Writer& writer) 
{
    UnaryNode::Write(writer);
    arguments.Write(writer);
}

IndexNode::IndexNode(const Span& span_): UnaryNode(span_)
{
}

IndexNode::IndexNode(const Span& span_, Node* subject_, Node* index_) : UnaryNode(span_, subject_), index(index_)
{
}

void IndexNode::Read(Reader& reader)
{
    UnaryNode::Read(reader);
    index.reset(reader.ReadNode());
}

void IndexNode::Write(Writer& writer)
{
    UnaryNode::Write(writer);
    writer.Write(index.get());
}

DotNode::DotNode(const Span& span_) : UnaryNode(span_)
{
}

DotNode::DotNode(const Span& span_, Node* subject_, IdentifierNode* memberId_) : UnaryNode(span_, subject_), memberId(memberId_)
{
}

void DotNode::Read(Reader& reader)
{
    UnaryNode::Read(reader);
    memberId.reset(reader.ReadIdentifierNode());
}

void DotNode::Write(Writer& writer)
{
    UnaryNode::Write(writer);
    writer.Write(memberId.get());
}

ArrowNode::ArrowNode(const Span& span_) : UnaryNode(span_)
{
}

ArrowNode::ArrowNode(const Span& span_, Node* subject_, IdentifierNode* memberId_) : UnaryNode(span_, subject_), memberId(memberId_)
{
}

void ArrowNode::Read(Reader& reader)
{
    UnaryNode::Read(reader);
    memberId.reset(reader.ReadIdentifierNode());
}

void ArrowNode::Write(Writer& writer)
{
    UnaryNode::Write(writer);
    writer.Write(memberId.get());
}

PostfixIncNode::PostfixIncNode(const Span& span_) : UnaryNode(span_)
{
}

PostfixIncNode::PostfixIncNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

PostfixDecNode::PostfixDecNode(const Span& span_) : UnaryNode(span_)
{
}

PostfixDecNode::PostfixDecNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

DerefNode::DerefNode(const Span& span_) : UnaryNode(span_)
{
}

DerefNode::DerefNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

AddrOfNode::AddrOfNode(const Span& span_) : UnaryNode(span_)
{
}

AddrOfNode::AddrOfNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

NotNode::NotNode(const Span& span_) : UnaryNode(span_)
{
}

NotNode::NotNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

UnaryPlusNode::UnaryPlusNode(const Span& span_) : UnaryNode(span_)
{
}

UnaryPlusNode::UnaryPlusNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

UnaryMinusNode::UnaryMinusNode(const Span& span_) : UnaryNode(span_)
{
}

UnaryMinusNode::UnaryMinusNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

ComplementNode::ComplementNode(const Span& span_) : UnaryNode(span_)
{
}

ComplementNode::ComplementNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

PrefixIncNode::PrefixIncNode(const Span& span_) : UnaryNode(span_)
{
}

PrefixIncNode::PrefixIncNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

PrefixDecNode::PrefixDecNode(const Span& span_) : UnaryNode(span_)
{
}

PrefixDecNode::PrefixDecNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

SizeOfNode::SizeOfNode(const Span& span_) : UnaryNode(span_)
{
}

SizeOfNode::SizeOfNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

TypeNameNode::TypeNameNode(const Span& span_) : UnaryNode(span_)
{
}

TypeNameNode::TypeNameNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

CastNode::CastNode(const Span& span_) : Node(span_)
{
}

CastNode::CastNode(const Span& span_, Node* targetTypeExpr_, Node* sourceExpr_) : Node(span_), targetTypeExpr(targetTypeExpr_), sourceExpr(sourceExpr_)
{
}

void CastNode::Read(Reader& reader)
{
    targetTypeExpr.reset(reader.ReadNode());
    sourceExpr.reset(reader.ReadNode());
}

void CastNode::Write(Writer& writer)
{
    writer.Write(targetTypeExpr.get());
    writer.Write(sourceExpr.get());
}

NewNode::NewNode(const Span& span_) : Node(span_)
{
}

NewNode::NewNode(const Span& span_, Node* typeExpr_) : Node(span_), typeExpr(typeExpr_)
{
}

void NewNode::AddArgument(Node* argument)
{
    arguments.Add(argument);
}

void NewNode::Read(Reader& reader)
{
    typeExpr.reset(reader.ReadNode());
    arguments.Read(reader);
}

void NewNode::Write(Writer& writer)
{
    writer.Write(typeExpr.get());
    arguments.Write(writer);
}

ConstructNode::ConstructNode(const Span& span_) : Node(span_)
{
}

ConstructNode::ConstructNode(const Span& span_, Node* typeExpr_) : Node(span_), typeExpr(typeExpr_)
{
}

void ConstructNode::AddArgument(Node* argument)
{
    arguments.Add(argument);
}

void ConstructNode::Read(Reader& reader)
{
    typeExpr.reset(reader.ReadNode());
    arguments.Read(reader);
}

void ConstructNode::Write(Writer& writer)
{
    writer.Write(typeExpr.get());
    arguments.Write(writer);
}

ThisNode::ThisNode(const Span& span_) : Node(span_)
{
}

BaseNode::BaseNode(const Span& span_) : Node(span_)
{
}

} } // namespace Cm::Ast
