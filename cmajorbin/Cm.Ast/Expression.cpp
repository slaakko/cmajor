/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Expression.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Ast {

EquivalenceNode::EquivalenceNode(const Span& span_) : BinaryNode(span_)
{
}

EquivalenceNode::EquivalenceNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* EquivalenceNode::Clone() const
{
    return new EquivalenceNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void EquivalenceNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

ImplicationNode::ImplicationNode(const Span& span_) : BinaryNode(span_)
{
}

ImplicationNode::ImplicationNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* ImplicationNode::Clone() const
{
    return new ImplicationNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void ImplicationNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

DisjunctionNode::DisjunctionNode(const Span& span_) : BinaryNode(span_)
{
}

DisjunctionNode::DisjunctionNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* DisjunctionNode::Clone() const
{
    return new DisjunctionNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void DisjunctionNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

ConjunctionNode::ConjunctionNode(const Span& span_) : BinaryNode(span_)
{
}

ConjunctionNode::ConjunctionNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* ConjunctionNode::Clone() const
{
    return new ConjunctionNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void ConjunctionNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

BitOrNode::BitOrNode(const Span& span_) : BinaryNode(span_)
{
}

BitOrNode::BitOrNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* BitOrNode::Clone() const
{
    return new BitOrNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void BitOrNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

BitXorNode::BitXorNode(const Span& span_) : BinaryNode(span_)
{
}

BitXorNode::BitXorNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* BitXorNode::Clone() const
{
    return new BitXorNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void BitXorNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

BitAndNode::BitAndNode(const Span& span_) : BinaryNode(span_)
{
}

BitAndNode::BitAndNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* BitAndNode::Clone() const
{
    return new BitAndNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void BitAndNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

EqualNode::EqualNode(const Span& span_) : BinaryNode(span_)
{
}

EqualNode::EqualNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* EqualNode::Clone() const
{
    return new EqualNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void EqualNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

NotEqualNode::NotEqualNode(const Span& span_) : BinaryNode(span_)
{
}

NotEqualNode::NotEqualNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* NotEqualNode::Clone() const
{
    return new NotEqualNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void NotEqualNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

LessNode::LessNode(const Span& span_) : BinaryNode(span_)
{
}

LessNode::LessNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* LessNode::Clone() const
{
    return new LessNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void LessNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

GreaterNode::GreaterNode(const Span& span_) : BinaryNode(span_)
{
}

GreaterNode::GreaterNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* GreaterNode::Clone() const
{
    return new GreaterNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void GreaterNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

LessOrEqualNode::LessOrEqualNode(const Span& span_) : BinaryNode(span_)
{
}

LessOrEqualNode::LessOrEqualNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* LessOrEqualNode::Clone() const
{
    return new LessOrEqualNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void LessOrEqualNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

GreaterOrEqualNode::GreaterOrEqualNode(const Span& span_) : BinaryNode(span_)
{
}

GreaterOrEqualNode::GreaterOrEqualNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* GreaterOrEqualNode::Clone() const
{
    return new GreaterOrEqualNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void GreaterOrEqualNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

ShiftLeftNode::ShiftLeftNode(const Span& span_) : BinaryNode(span_)
{
}

ShiftLeftNode::ShiftLeftNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* ShiftLeftNode::Clone() const
{
    return new ShiftLeftNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void ShiftLeftNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

ShiftRightNode::ShiftRightNode(const Span& span_) : BinaryNode(span_)
{
}

ShiftRightNode::ShiftRightNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* ShiftRightNode::Clone() const
{
    return new ShiftRightNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void ShiftRightNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

AddNode::AddNode(const Span& span_) : BinaryNode(span_)
{
}

AddNode::AddNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* AddNode::Clone() const
{
    return new AddNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void AddNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

SubNode::SubNode(const Span& span_) : BinaryNode(span_)
{
}

SubNode::SubNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* SubNode::Clone() const
{
    return new SubNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void SubNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

MulNode::MulNode(const Span& span_) : BinaryNode(span_)
{
}

MulNode::MulNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* MulNode::Clone() const
{
    return new MulNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void MulNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

DivNode::DivNode(const Span& span_) : BinaryNode(span_)
{
}

DivNode::DivNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* DivNode::Clone() const
{
    return new DivNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void DivNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

RemNode::RemNode(const Span& span_) : BinaryNode(span_)
{
}

RemNode::RemNode(const Span& span_, Node* left_, Node* right_) : BinaryNode(span_, left_, right_)
{
}

Node* RemNode::Clone() const
{
    return new RemNode(GetSpan(), Left()->Clone(), Right()->Clone());
}

void RemNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    BinaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

InvokeNode::InvokeNode(const Span& span_) : UnaryNode(span_)
{
}

InvokeNode::InvokeNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

Node* InvokeNode::Clone() const
{
    InvokeNode* clone = new InvokeNode(GetSpan(), Child()->Clone());
    for (const std::unique_ptr<Node>& argument : arguments)
    {
        clone->AddArgument(argument->Clone());
    }
    return clone;
}

void InvokeNode::AddArgument(Node* argument)
{
    argument->SetParent(this);
    arguments.Add(argument);
}

void InvokeNode::Read(Reader& reader) 
{
    UnaryNode::Read(reader);
    arguments.Read(reader);
    arguments.SetParent(this);
}

void InvokeNode::Write(Writer& writer) 
{
    UnaryNode::Write(writer);
    arguments.Write(writer);
}

std::string InvokeNode::ToString() const
{
    std::string s = Child()->ToString();
    s.append(1, '(');
    s.append(arguments.ToString());
    s.append(1, ')');
    return s;
}

void InvokeNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    arguments.Accept(visitor);
    visitor.EndVisit(*this);
}

IndexNode::IndexNode(const Span& span_): UnaryNode(span_)
{
}

IndexNode::IndexNode(const Span& span_, Node* subject_, Node* index_) : UnaryNode(span_, subject_), index(index_)
{
    index->SetParent(this);
}

Node* IndexNode::Clone() const
{
    return new IndexNode(GetSpan(), Child()->Clone(), index->Clone());
}

void IndexNode::Read(Reader& reader)
{
    UnaryNode::Read(reader);
    index.reset(reader.ReadNode());
    index->SetParent(this);
}

void IndexNode::Write(Writer& writer)
{
    UnaryNode::Write(writer);
    writer.Write(index.get());
}

std::string IndexNode::ToString() const
{
    std::string s;
    if (Child()->GetRank() > GetRank())
    {
        s.append(1, '(');
    }
    s.append(Child()->ToString());
    if (Child()->GetRank() > GetRank())
    {
        s.append(1, ')');
    }
    s.append(1, '[').append(index->ToString()).append(1, ']');
    return s;
}

void IndexNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

DotNode::DotNode(const Span& span_) : UnaryNode(span_)
{
}

DotNode::DotNode(const Span& span_, Node* subject_, IdentifierNode* memberId_) : UnaryNode(span_, subject_), memberId(memberId_)
{
    memberId->SetParent(this);
}

Node* DotNode::Clone() const
{
    return new DotNode(GetSpan(), Child()->Clone(), static_cast<IdentifierNode*>(memberId->Clone()));
}

void DotNode::Read(Reader& reader)
{
    UnaryNode::Read(reader);
    memberId.reset(reader.ReadIdentifierNode());
    memberId->SetParent(this);
}

void DotNode::Write(Writer& writer)
{
    UnaryNode::Write(writer);
    writer.Write(memberId.get());
}

std::string DotNode::ToString() const
{
    std::string s;
    if (Child()->GetRank() > GetRank())
    {
        s.append(1, '(');
    }
    s.append(Child()->ToString());
    if (Child()->GetRank() > GetRank())
    {
        s.append(1, ')');
    }
    s.append(1, '.').append(memberId->ToString());
    return s;
}

void DotNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    Subject()->Accept(visitor);
    visitor.EndVisit(*this);
}

ArrowNode::ArrowNode(const Span& span_) : UnaryNode(span_)
{
}

ArrowNode::ArrowNode(const Span& span_, Node* subject_, IdentifierNode* memberId_) : UnaryNode(span_, subject_), memberId(memberId_)
{
    memberId->SetParent(this);
}

Node* ArrowNode::Clone() const
{
    return new ArrowNode(GetSpan(), Child()->Clone(), static_cast<IdentifierNode*>(memberId->Clone()));
}

void ArrowNode::Read(Reader& reader)
{
    UnaryNode::Read(reader);
    memberId.reset(reader.ReadIdentifierNode());
    memberId->SetParent(this);
}

void ArrowNode::Write(Writer& writer)
{
    UnaryNode::Write(writer);
    writer.Write(memberId.get());
}

std::string ArrowNode::ToString() const
{
    std::string s;
    if (Child()->GetRank() > GetRank())
    {
        s.append(1, '(');
    }
    s.append(Child()->ToString());
    if (Child()->GetRank() > GetRank())
    {
        s.append(1, ')');
    }
    s.append("->").append(memberId->ToString());
    return s;
}

void ArrowNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

PostfixIncNode::PostfixIncNode(const Span& span_) : UnaryNode(span_)
{
}

PostfixIncNode::PostfixIncNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

Node* PostfixIncNode::Clone() const
{
    return new PostfixIncNode(GetSpan(), Child()->Clone());
}

std::string PostfixIncNode::ToString() const
{
    std::string s;
    if (Child()->GetRank() > GetRank())
    {
        s.append(1, '(');
    }
    s.append(Child()->ToString());
    if (Child()->GetRank() > GetRank())
    {
        s.append(1, ')');
    }
    s.append("++");
    return s;
}

void PostfixIncNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

PostfixDecNode::PostfixDecNode(const Span& span_) : UnaryNode(span_)
{
}

PostfixDecNode::PostfixDecNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

Node* PostfixDecNode::Clone() const
{
    return new PostfixDecNode(GetSpan(), Child()->Clone());
}

std::string PostfixDecNode::ToString() const
{
    std::string s;
    if (Child()->GetRank() > GetRank())
    {
        s.append(1, '(');
    }
    s.append(Child()->ToString());
    if (Child()->GetRank() > GetRank())
    {
        s.append(1, ')');
    }
    s.append("--");
    return s;
}

void PostfixDecNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

DerefNode::DerefNode(const Span& span_) : UnaryNode(span_)
{
}

DerefNode::DerefNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

Node* DerefNode::Clone() const
{
    return new DerefNode(GetSpan(), Child()->Clone());
}

void DerefNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

AddrOfNode::AddrOfNode(const Span& span_) : UnaryNode(span_)
{
}

AddrOfNode::AddrOfNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

Node* AddrOfNode::Clone() const
{
    return new AddrOfNode(GetSpan(), Child()->Clone());
}

void AddrOfNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

NotNode::NotNode(const Span& span_) : UnaryNode(span_)
{
}

NotNode::NotNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

Node* NotNode::Clone() const
{
    return new NotNode(GetSpan(), Child()->Clone());
}

void NotNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    UnaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

UnaryPlusNode::UnaryPlusNode(const Span& span_) : UnaryNode(span_)
{
}

UnaryPlusNode::UnaryPlusNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

Node* UnaryPlusNode::Clone() const
{
    return new UnaryPlusNode(GetSpan(), Child()->Clone());
}

void UnaryPlusNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    UnaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

UnaryMinusNode::UnaryMinusNode(const Span& span_) : UnaryNode(span_)
{
}

UnaryMinusNode::UnaryMinusNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

Node* UnaryMinusNode::Clone() const
{
    return new UnaryMinusNode(GetSpan(), Child()->Clone());
}

void UnaryMinusNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    UnaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

ComplementNode::ComplementNode(const Span& span_) : UnaryNode(span_)
{
}

ComplementNode::ComplementNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

Node* ComplementNode::Clone() const
{
    return new ComplementNode(GetSpan(), Child()->Clone());
}

void ComplementNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    UnaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

PrefixIncNode::PrefixIncNode(const Span& span_) : UnaryNode(span_)
{
}

PrefixIncNode::PrefixIncNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

Node* PrefixIncNode::Clone() const
{
    return new PrefixIncNode(GetSpan(), Child()->Clone());
}

void PrefixIncNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    UnaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

PrefixDecNode::PrefixDecNode(const Span& span_) : UnaryNode(span_)
{
}

PrefixDecNode::PrefixDecNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

Node* PrefixDecNode::Clone() const
{
    return new PrefixDecNode(GetSpan(), Child()->Clone());
}

void PrefixDecNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    UnaryNode::Accept(visitor);
    visitor.EndVisit(*this);
}

SizeOfNode::SizeOfNode(const Span& span_) : UnaryNode(span_)
{
}

SizeOfNode::SizeOfNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

Node* SizeOfNode::Clone() const
{
    return new SizeOfNode(GetSpan(), Child()->Clone());
}

void SizeOfNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

TypeNameNode::TypeNameNode(const Span& span_) : UnaryNode(span_)
{
}

TypeNameNode::TypeNameNode(const Span& span_, Node* subject_) : UnaryNode(span_, subject_)
{
}

Node* TypeNameNode::Clone() const
{
    return new TypeNameNode(GetSpan(), Child()->Clone());
}

void TypeNameNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

CastNode::CastNode(const Span& span_) : Node(span_)
{
}

CastNode::CastNode(const Span& span_, Node* targetTypeExpr_, Node* sourceExpr_) : Node(span_), targetTypeExpr(targetTypeExpr_), sourceExpr(sourceExpr_)
{
    targetTypeExpr->SetParent(this);
    sourceExpr->SetParent(this);
}

Node* CastNode::Clone() const
{
    return new CastNode(GetSpan(), targetTypeExpr->Clone(), sourceExpr->Clone());
}

void CastNode::Read(Reader& reader)
{
    targetTypeExpr.reset(reader.ReadNode());
    targetTypeExpr->SetParent(this);
    sourceExpr.reset(reader.ReadNode());
    sourceExpr->SetParent(this);
}

void CastNode::Write(Writer& writer)
{
    writer.Write(targetTypeExpr.get());
    writer.Write(sourceExpr.get());
}

std::string CastNode::ToString() const
{
    std::string s = "cast<";
    s.append(targetTypeExpr->ToString());
    s.append(">(");
    s.append(sourceExpr->ToString());
    s.append(1, ')');
    return s;
}

void CastNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

NewNode::NewNode(const Span& span_) : Node(span_)
{
}

NewNode::NewNode(const Span& span_, Node* typeExpr_) : Node(span_), typeExpr(typeExpr_)
{
    typeExpr->SetParent(this);
}

Node* NewNode::Clone() const
{
    NewNode* clone = new NewNode(GetSpan(), typeExpr->Clone());
    for (const std::unique_ptr<Node>& argument : arguments)
    {
        clone->AddArgument(argument->Clone());
    }
    return clone;
}

void NewNode::AddArgument(Node* argument)
{
    argument->SetParent(this);
    arguments.Add(argument);
}

void NewNode::Read(Reader& reader)
{
    typeExpr.reset(reader.ReadNode());
    typeExpr->SetParent(this);
    arguments.Read(reader);
    arguments.SetParent(this);
}

void NewNode::Write(Writer& writer)
{
    writer.Write(typeExpr.get());
    arguments.Write(writer);
}

std::string NewNode::ToString() const
{
    std::string s = "new ";
    s.append(typeExpr->ToString());
    s.append(1, '(');
    s.append(arguments.ToString());
    s.append(1, ')');
    return s;
}

void NewNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    arguments.Accept(visitor);
    visitor.EndVisit(*this);
}

ConstructNode::ConstructNode(const Span& span_) : Node(span_)
{
}

ConstructNode::ConstructNode(const Span& span_, Node* typeExpr_) : Node(span_), typeExpr(typeExpr_)
{
    typeExpr->SetParent(this);
}

Node* ConstructNode::Clone() const
{
    ConstructNode* clone = new ConstructNode(GetSpan(), typeExpr->Clone());
    for (const std::unique_ptr<Node>& argument : arguments)
    {
        clone->AddArgument(argument->Clone());
    }
    return clone;
}

void ConstructNode::AddArgument(Node* argument)
{
    argument->SetParent(this);
    arguments.Add(argument);
}

void ConstructNode::Read(Reader& reader)
{
    typeExpr.reset(reader.ReadNode());
    typeExpr->SetParent(this);
    arguments.Read(reader);
    arguments.SetParent(this);
}

void ConstructNode::Write(Writer& writer)
{
    writer.Write(typeExpr.get());
    arguments.Write(writer);
}

std::string ConstructNode::ToString() const
{
    std::string s = "construct<";
    s.append(typeExpr->ToString());
    s.append(">(");
    s.append(arguments.ToString());
    s.append(1, ')');
    return s;
}

void ConstructNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    arguments.Accept(visitor);
    visitor.EndVisit(*this);
}

ThisNode::ThisNode(const Span& span_) : Node(span_)
{
}

Node* ThisNode::Clone() const
{
    return new ThisNode(GetSpan());
}

void ThisNode::Accept(Visitor& visitor)
{
    return visitor.Visit(*this);
}

BaseNode::BaseNode(const Span& span_) : Node(span_)
{
}

Node* BaseNode::Clone() const
{
    return new BaseNode(GetSpan());
}

void BaseNode::Accept(Visitor& visitor)
{
    return visitor.Visit(*this);
}

} } // namespace Cm::Ast
