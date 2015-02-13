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

DelegateNode::DelegateNode(const Span& span_) : Node(span_), specifiers(Specifiers::none)
{
}

DelegateNode::DelegateNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, IdentifierNode* id_) : 
    Node(span_), specifiers(specifiers_), returnTypeExpr(returnTypeExpr_), id(id_)
{
    returnTypeExpr->SetParent(this);
    id->SetParent(this);
}

Node* DelegateNode::Clone(CloneContext& cloneContext) const
{
    DelegateNode* clone = new DelegateNode(GetSpan(), specifiers, returnTypeExpr->Clone(cloneContext), static_cast<IdentifierNode*>(id->Clone(cloneContext)));
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone(cloneContext)));
    }
    return clone;
}

void DelegateNode::AddParameter(ParameterNode* parameter)
{
    parameter->SetParent(this);
    parameters.Add(parameter);
}

void DelegateNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    returnTypeExpr.reset(reader.ReadNode());
    returnTypeExpr->SetParent(this);
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    parameters.Read(reader);
    parameters.SetParent(this);
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

ClassDelegateNode::ClassDelegateNode(const Span& span_) : Node(span_), specifiers(Specifiers::none)
{
}

ClassDelegateNode::ClassDelegateNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, IdentifierNode* id_) : 
    Node(span_), specifiers(specifiers_), returnTypeExpr(returnTypeExpr_), id(id_)
{
    returnTypeExpr->SetParent(this);
    id->SetParent(this);
}

Node* ClassDelegateNode::Clone(CloneContext& cloneContext) const
{
    ClassDelegateNode* clone = new ClassDelegateNode(GetSpan(), specifiers, returnTypeExpr->Clone(cloneContext), static_cast<IdentifierNode*>(id->Clone(cloneContext)));
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone(cloneContext)));
    }
    return clone;
}

void ClassDelegateNode::AddParameter(ParameterNode* parameter)
{
    parameter->SetParent(this);
    parameters.Add(parameter);
}

void ClassDelegateNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    returnTypeExpr.reset(reader.ReadNode());
    returnTypeExpr->SetParent(this);
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    parameters.Read(reader);
    parameters.SetParent(this);
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
