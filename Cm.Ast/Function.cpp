/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Function.hpp>
#include <Cm.Ast/Concept.hpp>
#include <Cm.Ast/Statement.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Ast {

FunctionGroupIdNode::FunctionGroupIdNode(const Span& span_) : Node(span_)
{
}

FunctionGroupIdNode::FunctionGroupIdNode(const Span& span_, const std::string& functionGroupId_) : Node(span_), functionGroupId(functionGroupId_)
{
}

Node* FunctionGroupIdNode::Clone() const
{
    return new FunctionGroupIdNode(GetSpan(), functionGroupId);
}

void FunctionGroupIdNode::Read(Reader& reader)
{
    functionGroupId = reader.ReadString();
}

void FunctionGroupIdNode::Write(Writer& writer)
{
    writer.Write(functionGroupId);
}

FunctionNode::FunctionNode(const Span& span_) : Node(span_), parent(nullptr)
{
}

FunctionNode::FunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, FunctionGroupIdNode* groupId_) :
    Node(span_), specifiers(specifiers_), returnTypeExpr(returnTypeExpr_), groupId(groupId_), parent(nullptr)
{
}

void FunctionNode::AddTemplateParameter(TemplateParameterNode* templateParameter)
{
    templateParameters.Add(templateParameter);
}

void FunctionNode::AddParameter(ParameterNode* parameter)
{
    parameters.Add(parameter);
}

void FunctionNode::SetConstraint(WhereConstraintNode* constraint_)
{
    constraint.reset(constraint_);
}

void FunctionNode::SetBody(CompoundStatementNode* body_)
{
    body.reset(body_);
}

Node* FunctionNode::Clone() const
{
    FunctionNode* clone = new FunctionNode(GetSpan(), specifiers, returnTypeExpr->Clone(), static_cast<FunctionGroupIdNode*>(groupId->Clone()));
    for (const std::unique_ptr<TemplateParameterNode>& templateParameter : templateParameters)
    {
        clone->AddTemplateParameter(static_cast<TemplateParameterNode*>(templateParameter->Clone()));
    }
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone()));
    }
    if (constraint)
    {
        clone->SetConstraint(static_cast<WhereConstraintNode*>(constraint->Clone()));
    }
    clone->SetBody(static_cast<CompoundStatementNode*>(body->Clone()));
    return clone;
}

void FunctionNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    bool hasReturnTypeExpr = reader.ReadBool();
    if (hasReturnTypeExpr)
    {
        returnTypeExpr.reset(reader.ReadNode());
    }
    groupId.reset(reader.ReadFunctionGroupIdNode());
    templateParameters.Read(reader);
    parameters.Read(reader);
    bool hasConstraint = reader.ReadBool();
    if (hasConstraint)
    {
        constraint.reset(reader.ReadWhereConstraintNode());
    }
    bool hasBody = reader.ReadBool();
    if (hasBody)
    {
        body.reset(reader.ReadCompoundStatementNode());
    }
}

void FunctionNode::Write(Writer& writer)
{
    writer.Write(specifiers);
    bool hasReturnTypeExpr = returnTypeExpr != nullptr;
    writer.Write(hasReturnTypeExpr);
    if (hasReturnTypeExpr)
    {
        writer.Write(returnTypeExpr.get());
    }
    writer.Write(groupId.get());
    templateParameters.Write(writer);
    parameters.Write(writer);
    bool hasConstraint = constraint != nullptr;
    writer.Write(hasConstraint);
    if (hasConstraint)
    {
        writer.Write(constraint.get());
    }
    bool hasBody = body != nullptr;
    writer.Write(hasBody);
    if (hasBody)
    {
        writer.Write(body.get());
    }
}

void FunctionNode::Print(CodeFormatter& formatter)
{
    std::string s = SpecifierStr(specifiers);
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    if (returnTypeExpr)
    {
        s.append(returnTypeExpr->ToString()).append(1, ' ');
    }
    s.append(groupId->ToString()).append(templateParameters.ToString()).append(parameters.ToString());
    if (constraint)
    {
        s.append(1, ' ').append(constraint->ToString());
    }
    if (body)
    {
        formatter.WriteLine(s);
        body->Print(formatter);
    }
    else
    {
        s.append(1, ';');
        formatter.WriteLine(s);
    }
}

Node* FunctionNode::Parent() const
{
    return parent;
}

void FunctionNode::SetParent(Node* parent_)
{
    parent = parent_;
}

std::string FunctionNode::Name() const 
{
    std::string name = groupId->Str();
    name.append(parameters.ToString());
    return name;
}

void FunctionNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    parameters.Accept(visitor);
    if (body)
    {
        body->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

} } // namespace Cm::Ast
