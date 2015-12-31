/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
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

Node* EquivalenceNode::Clone(CloneContext& cloneContext) const
{
    return new EquivalenceNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* ImplicationNode::Clone(CloneContext& cloneContext) const
{
    return new ImplicationNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* DisjunctionNode::Clone(CloneContext& cloneContext) const
{
    return new DisjunctionNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* ConjunctionNode::Clone(CloneContext& cloneContext) const
{
    return new ConjunctionNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* BitOrNode::Clone(CloneContext& cloneContext) const
{
    return new BitOrNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* BitXorNode::Clone(CloneContext& cloneContext) const
{
    return new BitXorNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* BitAndNode::Clone(CloneContext& cloneContext) const
{
    return new BitAndNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* EqualNode::Clone(CloneContext& cloneContext) const
{
    return new EqualNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* NotEqualNode::Clone(CloneContext& cloneContext) const
{
    return new NotEqualNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* LessNode::Clone(CloneContext& cloneContext) const
{
    return new LessNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* GreaterNode::Clone(CloneContext& cloneContext) const
{
    return new GreaterNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* LessOrEqualNode::Clone(CloneContext& cloneContext) const
{
    return new LessOrEqualNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* GreaterOrEqualNode::Clone(CloneContext& cloneContext) const
{
    return new GreaterOrEqualNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* ShiftLeftNode::Clone(CloneContext& cloneContext) const
{
    return new ShiftLeftNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* ShiftRightNode::Clone(CloneContext& cloneContext) const
{
    return new ShiftRightNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* AddNode::Clone(CloneContext& cloneContext) const
{
    return new AddNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* SubNode::Clone(CloneContext& cloneContext) const
{
    return new SubNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* MulNode::Clone(CloneContext& cloneContext) const
{
    return new MulNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* DivNode::Clone(CloneContext& cloneContext) const
{
    return new DivNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* RemNode::Clone(CloneContext& cloneContext) const
{
    return new RemNode(GetSpan(), Left()->Clone(cloneContext), Right()->Clone(cloneContext));
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

Node* InvokeNode::Clone(CloneContext& cloneContext) const
{
    InvokeNode* clone = new InvokeNode(GetSpan(), Child()->Clone(cloneContext));
    for (const std::unique_ptr<Node>& argument : arguments)
    {
        clone->AddArgument(argument->Clone(cloneContext));
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

Node* IndexNode::Clone(CloneContext& cloneContext) const
{
    return new IndexNode(GetSpan(), Child()->Clone(cloneContext), index->Clone(cloneContext));
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

Node* DotNode::Clone(CloneContext& cloneContext) const
{
    return new DotNode(GetSpan(), Child()->Clone(cloneContext), static_cast<IdentifierNode*>(memberId->Clone(cloneContext)));
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

std::string DotNode::DocId() const
{
    return Child()->DocId() + "." + memberId->DocId();
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

Node* ArrowNode::Clone(CloneContext& cloneContext) const
{
    return new ArrowNode(GetSpan(), Child()->Clone(cloneContext), static_cast<IdentifierNode*>(memberId->Clone(cloneContext)));
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

Node* PostfixIncNode::Clone(CloneContext& cloneContext) const
{
    return new PostfixIncNode(GetSpan(), Child()->Clone(cloneContext));
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

Node* PostfixDecNode::Clone(CloneContext& cloneContext) const
{
    return new PostfixDecNode(GetSpan(), Child()->Clone(cloneContext));
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

Node* DerefNode::Clone(CloneContext& cloneContext) const
{
    return new DerefNode(GetSpan(), Child()->Clone(cloneContext));
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

Node* AddrOfNode::Clone(CloneContext& cloneContext) const
{
    return new AddrOfNode(GetSpan(), Child()->Clone(cloneContext));
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

Node* NotNode::Clone(CloneContext& cloneContext) const
{
    return new NotNode(GetSpan(), Child()->Clone(cloneContext));
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

Node* UnaryPlusNode::Clone(CloneContext& cloneContext) const
{
    return new UnaryPlusNode(GetSpan(), Child()->Clone(cloneContext));
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

Node* UnaryMinusNode::Clone(CloneContext& cloneContext) const
{
    return new UnaryMinusNode(GetSpan(), Child()->Clone(cloneContext));
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

Node* ComplementNode::Clone(CloneContext& cloneContext) const
{
    return new ComplementNode(GetSpan(), Child()->Clone(cloneContext));
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

Node* PrefixIncNode::Clone(CloneContext& cloneContext) const
{
    return new PrefixIncNode(GetSpan(), Child()->Clone(cloneContext));
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

Node* PrefixDecNode::Clone(CloneContext& cloneContext) const
{
    return new PrefixDecNode(GetSpan(), Child()->Clone(cloneContext));
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

Node* SizeOfNode::Clone(CloneContext& cloneContext) const
{
    return new SizeOfNode(GetSpan(), Child()->Clone(cloneContext));
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

Node* TypeNameNode::Clone(CloneContext& cloneContext) const
{
    return new TypeNameNode(GetSpan(), Child()->Clone(cloneContext));
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

Node* CastNode::Clone(CloneContext& cloneContext) const
{
    return new CastNode(GetSpan(), targetTypeExpr->Clone(cloneContext), sourceExpr->Clone(cloneContext));
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

IsNode::IsNode(const Span& span_) : Node(span_)
{
}

IsNode::IsNode(const Span& span_, Node* expr_, Node* typeExpr_) : Node(span_), expr(expr_), typeExpr(typeExpr_)
{
    expr->SetParent(this);
    typeExpr->SetParent(this);
}

Node* IsNode::Clone(CloneContext& cloneContext) const
{
    return new IsNode(GetSpan(), expr->Clone(cloneContext), typeExpr->Clone(cloneContext));
}

void IsNode::Read(Reader& reader)
{
    expr.reset(reader.ReadNode());
    expr->SetParent(this);
    typeExpr.reset(reader.ReadNode());
    typeExpr->SetParent(this);
}

void IsNode::Write(Writer& writer)
{
    writer.Write(expr.get());
    writer.Write(typeExpr.get());
}

std::string IsNode::ToString() const
{
    std::string s = expr->ToString();
    s.append(" is ").append(typeExpr->ToString());
    return s;
}

void IsNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

AsNode::AsNode(const Span& span_) : Node(span_)
{
}

AsNode::AsNode(const Span& span_, Node* expr_, Node* typeExpr_) : Node(span_), expr(expr_), typeExpr(typeExpr_)
{
    expr->SetParent(this);
    typeExpr->SetParent(this);
}

Node* AsNode::Clone(CloneContext& cloneContext) const
{
    return new AsNode(GetSpan(), expr->Clone(cloneContext), typeExpr->Clone(cloneContext));
}

void AsNode::Read(Reader& reader)
{
    expr.reset(reader.ReadNode());
    expr->SetParent(this);
    typeExpr.reset(reader.ReadNode());
    typeExpr->SetParent(this);
}

void AsNode::Write(Writer& writer)
{
    writer.Write(expr.get());
    writer.Write(typeExpr.get());
}

std::string AsNode::ToString() const
{
    std::string s = expr->ToString();
    s.append(" as ").append(typeExpr->ToString());
    return s;
}

void AsNode::Accept(Visitor& visitor)
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

Node* NewNode::Clone(CloneContext& cloneContext) const
{
    NewNode* clone = new NewNode(GetSpan(), typeExpr->Clone(cloneContext));
    for (const std::unique_ptr<Node>& argument : arguments)
    {
        clone->AddArgument(argument->Clone(cloneContext));
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
    visitor.Visit(*this);
}

ConstructNode::ConstructNode(const Span& span_) : Node(span_)
{
}

ConstructNode::ConstructNode(const Span& span_, Node* typeExpr_) : Node(span_), typeExpr(typeExpr_)
{
    typeExpr->SetParent(this);
}

Node* ConstructNode::Clone(CloneContext& cloneContext) const
{
    ConstructNode* clone = new ConstructNode(GetSpan(), typeExpr->Clone(cloneContext));
    for (const std::unique_ptr<Node>& argument : arguments)
    {
        clone->AddArgument(argument->Clone(cloneContext));
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
    visitor.Visit(*this);
}

ThisNode::ThisNode(const Span& span_) : Node(span_)
{
}

Node* ThisNode::Clone(CloneContext& cloneContext) const
{
    return new ThisNode(GetSpan());
}

void ThisNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BaseNode::BaseNode(const Span& span_) : Node(span_)
{
}

Node* BaseNode::Clone(CloneContext& cloneContext) const
{
    return new BaseNode(GetSpan());
}

void BaseNode::Accept(Visitor& visitor)
{
    return visitor.Visit(*this);
}

} } // namespace Cm::Ast
