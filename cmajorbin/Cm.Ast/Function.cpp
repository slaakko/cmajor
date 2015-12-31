/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Function.hpp>
#include <Cm.Ast/Concept.hpp>
#include <Cm.Ast/Statement.hpp>
#include <Cm.Ast/Namespace.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Visitor.hpp>
#include <Cm.Ast/Exception.hpp>

namespace Cm { namespace Ast {

FunctionGroupIdNode::FunctionGroupIdNode(const Span& span_) : Node(span_)
{
}

FunctionGroupIdNode::FunctionGroupIdNode(const Span& span_, const std::string& functionGroupId_) : Node(span_), functionGroupId(functionGroupId_)
{
}

Node* FunctionGroupIdNode::Clone(CloneContext& cloneContext) const
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

FunctionNode::FunctionNode(const Span& span_) : Node(span_), compileUnit(nullptr), bodySource(nullptr)
{
}

FunctionNode::FunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, FunctionGroupIdNode* groupId_) :
    Node(span_), specifiers(specifiers_), returnTypeExpr(returnTypeExpr_), groupId(groupId_), compileUnit(nullptr), bodySource(nullptr)
{
    if (returnTypeExpr)
    {
        returnTypeExpr->SetParent(this);
    }
    groupId->SetParent(this);
}

void FunctionNode::AddTemplateParameter(TemplateParameterNode* templateParameter)
{
    templateParameter->SetParent(this);
    templateParameters.Add(templateParameter);
}

void FunctionNode::AddParameter(ParameterNode* parameter)
{
    parameter->SetParent(this);
    parameters.Add(parameter);
}

void FunctionNode::SetConstraint(WhereConstraintNode* constraint_)
{
    constraint.reset(constraint_);
    if (constraint)
    {
        constraint->SetParent(this);
    }
}

void FunctionNode::SetBody(CompoundStatementNode* body_)
{
    body.reset(body_);
    if (body)
    {
        body->SetParent(this);
    }
}

Node* FunctionNode::Clone(CloneContext& cloneContext) const
{
    FunctionNode* clone = new FunctionNode(GetSpan(), specifiers, returnTypeExpr->Clone(cloneContext), static_cast<FunctionGroupIdNode*>(groupId->Clone(cloneContext)));
    for (const std::unique_ptr<TemplateParameterNode>& templateParameter : templateParameters)
    {
        clone->AddTemplateParameter(static_cast<TemplateParameterNode*>(templateParameter->Clone(cloneContext)));
    }
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone(cloneContext)));
    }
    if (constraint)
    {
        clone->SetConstraint(static_cast<WhereConstraintNode*>(constraint->Clone(cloneContext)));
    }
    clone->SetBody(static_cast<CompoundStatementNode*>(body->Clone(cloneContext)));
    return clone;
}

void FunctionNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    bool hasReturnTypeExpr = reader.ReadBool();
    if (hasReturnTypeExpr)
    {
        returnTypeExpr.reset(reader.ReadNode());
        returnTypeExpr->SetParent(this);
    }
    groupId.reset(reader.ReadFunctionGroupIdNode());
    groupId->SetParent(this);
    templateParameters.Read(reader);
    templateParameters.SetParent(this);
    parameters.Read(reader);
    parameters.SetParent(this);
    bool hasConstraint = reader.ReadBool();
    if (hasConstraint)
    {
        constraint.reset(reader.ReadWhereConstraintNode());
        constraint->SetParent(this);
    }
    bool hasBody = reader.ReadBool();
    if (hasBody)
    {
        body.reset(reader.ReadCompoundStatementNode());
        body->SetParent(this);
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

std::string FunctionNode::Name() const 
{
    std::string name = groupId->Str();
    name.append(parameters.ToString());
    return name;
}

std::string FunctionNode::FullGroupName() const
{
    if (Parent()->IsNamespaceNode())
    {
        return static_cast<NamespaceNode*>(Parent())->FullName() + "." + groupId->Str();
    }
    else
    {
        return groupId->Str();
    }
}

void FunctionNode::Accept(Visitor& visitor)
{
    try
    {
        visitor.BeginVisit(*this);
        if (!visitor.SkipContent())
        {
            templateParameters.Accept(visitor);
            parameters.Accept(visitor);
            if (visitor.VisitBodies())
            {
                if (body)
                {
                    body->Accept(visitor);
                }
            }
        }
        visitor.EndVisit(*this);
    }
    catch (Cm::Ast::Exception& ex)
    {
        ex.AddReference(GetSpan());
        throw;
    }
}

void FunctionNode::SetBodySource(CompoundStatementNode* bodySource_)
{
    bodySource.reset(bodySource_);
}

} } // namespace Cm::Ast
