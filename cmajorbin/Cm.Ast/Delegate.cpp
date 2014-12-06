/*========================================================================
Copyright (c) 2012-2015 Seppo Laakko
http://sourceforge.net/projects/cmajor/

Distributed under the GNU General Public License, version 3 (GPLv3).
(See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Delegate.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Parameter.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Ast {

DelegateNode::DelegateNode(const Span& span_) : Node(span_), specifiers(Specifiers::none), parent(nullptr)
{
}

DelegateNode::DelegateNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, IdentifierNode* id_) : 
    Node(span_), specifiers(specifiers_), returnTypeExpr(returnTypeExpr_), id(id_), parent(nullptr)
{
}

Node* DelegateNode::Clone() const
{
    DelegateNode* clone = new DelegateNode(GetSpan(), specifiers, returnTypeExpr->Clone(), static_cast<IdentifierNode*>(id->Clone()));
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone()));
    }
    return clone;
}

void DelegateNode::AddParameter(ParameterNode* parameter)
{
    parameters.Add(parameter);
}

void DelegateNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    returnTypeExpr.reset(reader.ReadNode());
    id.reset(reader.ReadIdentifierNode());
    parameters.Read(reader);
}

void DelegateNode::Write(Writer& writer)
{
    writer.Write(specifiers);
    writer.Write(returnTypeExpr.get());
    writer.Write(id.get());
    parameters.Write(writer);
}

void DelegateNode::Print(CodeFormatter& formatter)
{
    std::string s = SpecifierStr(specifiers);
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    s.append("delegate ").append(returnTypeExpr->ToString()).append(1, ' ').append(id->ToString()).append(parameters.ToString()).append(1, ';');
    formatter.WriteLine(s);
}

Node* DelegateNode::Parent() const
{
    return parent;
}

void DelegateNode::SetParent(Node* parent_)
{
    parent = parent_;
}

std::string DelegateNode::Name() const 
{ 
    return id->Str(); 
}

void DelegateNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        parameter->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

ClassDelegateNode::ClassDelegateNode(const Span& span_) : Node(span_), specifiers(Specifiers::none), parent(nullptr)
{
}

ClassDelegateNode::ClassDelegateNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, IdentifierNode* id_) : 
    Node(span_), specifiers(specifiers_), returnTypeExpr(returnTypeExpr_), id(id_), parent(nullptr)
{
}

Node* ClassDelegateNode::Clone() const
{
    ClassDelegateNode* clone = new ClassDelegateNode(GetSpan(), specifiers, returnTypeExpr->Clone(), static_cast<IdentifierNode*>(id->Clone()));
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone()));
    }
    return clone;
}

void ClassDelegateNode::AddParameter(ParameterNode* parameter)
{
    parameters.Add(parameter);
}

void ClassDelegateNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    returnTypeExpr.reset(reader.ReadNode());
    id.reset(reader.ReadIdentifierNode());
    parameters.Read(reader);
}

void ClassDelegateNode::Write(Writer& writer)
{
    writer.Write(specifiers);
    writer.Write(returnTypeExpr.get());
    writer.Write(id.get());
    parameters.Write(writer);
}

void ClassDelegateNode::Print(CodeFormatter& formatter)
{
    std::string s = SpecifierStr(specifiers);
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    s.append("class delegate ").append(returnTypeExpr->ToString()).append(1, ' ').append(id->ToString()).append(parameters.ToString()).append(1, ';');
    formatter.WriteLine(s);
}

Node* ClassDelegateNode::Parent() const
{
    return parent;
}

void ClassDelegateNode::SetParent(Node* parent_)
{
    parent = parent_;
}

std::string ClassDelegateNode::Name() const 
{ 
    return id->Str(); 
}

void ClassDelegateNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        parameter->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

} } // namespace Cm::Ast
