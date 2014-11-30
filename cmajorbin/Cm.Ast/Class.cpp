/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Class.hpp>
#include <Cm.Ast/Concept.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Statement.hpp>

namespace Cm { namespace Ast {

ClassNode::ClassNode(const Span& span_) : Node(span_)
{
}

ClassNode::ClassNode(const Span& span_, Specifiers specifiers_, IdentifierNode* id_) : Node(span_), specifiers(specifiers_), id(id_)
{
}

const std::string& ClassNode::Id() const 
{ 
    return id->Str(); 
}

void ClassNode::AddTemplateParameter(TemplateParameterNode* templateParameter)
{
    templateParameters.Add(templateParameter);
}

void ClassNode::SetBaseClassTypeExpr(Node* baseClassTypeExpr_)
{
    baseClassTypeExpr.reset(baseClassTypeExpr_);
}

void ClassNode::SetConstraint(WhereConstraintNode* constraint_)
{
    constraint.reset(constraint_);
}

void ClassNode::AddMember(Node* member)
{
    members.Add(member);
}

Node* ClassNode::Clone() const
{
    ClassNode* clone = new ClassNode(GetSpan(), specifiers, static_cast<IdentifierNode*>(id->Clone()));
    for (const std::unique_ptr<TemplateParameterNode>& templateParameter : templateParameters)
    {
        clone->AddTemplateParameter(static_cast<TemplateParameterNode*>(templateParameter->Clone()));
    }
    if (baseClassTypeExpr)
    {
        clone->SetBaseClassTypeExpr(baseClassTypeExpr->Clone());
    }
    if (constraint)
    {
        clone->SetConstraint(static_cast<WhereConstraintNode*>(constraint->Clone()));
    }
    for (const std::unique_ptr<Node>& member : members)
    {
        clone->AddMember(member->Clone());
    }
    return clone;
}

void ClassNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    id.reset(reader.ReadIdentifierNode());
    templateParameters.Read(reader);
    bool hasBaseClass = reader.ReadBool();
    if (hasBaseClass)
    {
        baseClassTypeExpr.reset(reader.ReadNode());
    }
    bool hasConstraint = reader.ReadBool();
    if (hasConstraint)
    {
        constraint.reset(reader.ReadWhereConstraintNode());
    }
    members.Read(reader);
}

void ClassNode::Write(Writer& writer)
{
    writer.Write(specifiers);
    writer.Write(id.get());
    templateParameters.Write(writer);
    bool hasBasClass = baseClassTypeExpr != nullptr;
    writer.Write(hasBasClass);
    if (hasBasClass)
    {
        writer.Write(baseClassTypeExpr.get());
    }
    bool hasConstraint = constraint != nullptr;
    writer.Write(hasConstraint);
    if (hasConstraint)
    {
        writer.Write(constraint.get());
    }
    members.Write(writer);
}

InitializerNode::InitializerNode(const Span& span_) : Node(span_)
{
}

void InitializerNode::AddArgument(Node* argument)
{
    arguments.Add(argument);
}

void InitializerNode::Read(Reader& reader)
{
    arguments.Read(reader);
}

void InitializerNode::Write(Writer& writer)
{
    arguments.Write(writer);
}

MemberInitializerNode::MemberInitializerNode(const Span& span_) : InitializerNode(span_)
{
}

MemberInitializerNode::MemberInitializerNode(const Span& span_, IdentifierNode* memberId_) : InitializerNode(span_), memberId(memberId_)
{
}

Node* MemberInitializerNode::Clone() const
{
    MemberInitializerNode* clone = new MemberInitializerNode(GetSpan(), static_cast<IdentifierNode*>(memberId->Clone()));
    for (const std::unique_ptr<Node>& argument : Arguments())
    {
        clone->AddArgument(argument->Clone());
    }
    return clone;
}

void MemberInitializerNode::Read(Reader& reader)
{
    InitializerNode::Read(reader);
    memberId.reset(reader.ReadIdentifierNode());
}

void MemberInitializerNode::Write(Writer& writer)
{
    InitializerNode::Write(writer);
    writer.Write(memberId.get());
}

BaseInitializerNode::BaseInitializerNode(const Span& span_) : InitializerNode(span_)
{
}

Node* BaseInitializerNode::Clone() const
{
    BaseInitializerNode* clone = new BaseInitializerNode(GetSpan());
    for (const std::unique_ptr<Node>& argument : Arguments())
    {
        clone->AddArgument(argument->Clone());
    }
    return clone;
}

ThisInitializerNode::ThisInitializerNode(const Span& span_) : InitializerNode(span_)
{
}

Node* ThisInitializerNode::Clone() const
{
    ThisInitializerNode* clone = new ThisInitializerNode(GetSpan());
    for (const std::unique_ptr<Node>& argument : Arguments())
    {
        clone->AddArgument(argument->Clone());
    }
    return clone;
}

InitializerNodeList::InitializerNodeList()
{
}

void InitializerNodeList::Read(Reader& reader)
{
    uint32_t n = reader.ReadUInt();
    for (uint32_t i = 0; i < n; ++i)
    {
        initializerNodes.push_back(std::unique_ptr<InitializerNode>(reader.ReadInitializerNode()));
    }
}

void InitializerNodeList::Write(Writer& writer)
{
    uint32_t n = static_cast<uint32_t>(initializerNodes.size());
    writer.Write(n);
    for (uint32_t i = 0; i < n; ++i)
    {
        writer.Write(initializerNodes[i].get());
    }
}

StaticConstructorNode::StaticConstructorNode(const Span& span_) : Node(span_)
{
}

StaticConstructorNode::StaticConstructorNode(const Span& span_, Specifiers specifiers_) : Node(span_), specifiers(specifiers_)
{
}

void StaticConstructorNode::AddInitializer(InitializerNode* initializer)
{
    initializers.Add(initializer);
}

void StaticConstructorNode::SetConstraint(WhereConstraintNode* constraint_)
{
    constraint.reset(constraint_);
}

void StaticConstructorNode::SetBody(CompoundStatementNode* body_)
{
    body.reset(body_);
}

Node* StaticConstructorNode::Clone() const
{
    StaticConstructorNode* clone = new StaticConstructorNode(GetSpan(), specifiers);
    for (const std::unique_ptr<InitializerNode>& initializer : initializers)
    {
        clone->AddInitializer(static_cast<InitializerNode*>(initializer->Clone()));
    }
    if (constraint)
    {
        clone->SetConstraint(static_cast<WhereConstraintNode*>(constraint->Clone()));
    }
    clone->SetBody(static_cast<CompoundStatementNode*>(body->Clone()));
    return clone;
}

void StaticConstructorNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    initializers.Read(reader);
    bool hasConstraint = reader.ReadBool();
    if (hasConstraint)
    {
        constraint.reset(reader.ReadWhereConstraintNode());
    }
    body.reset(reader.ReadCompoundStatementNode());
}

void StaticConstructorNode::Write(Writer& writer)
{
    writer.Write(specifiers);
    initializers.Write(writer);
    bool hasConstraint = constraint != nullptr;
    writer.Write(hasConstraint);
    if (hasConstraint)
    {
        writer.Write(constraint.get());
    }
    writer.Write(body.get());
}


ConstructorNode::ConstructorNode(const Span& span_) : Node(span_)
{
}

ConstructorNode::ConstructorNode(const Span& span_, Specifiers specifiers_) : Node(span_), specifiers(specifiers_)
{
}

void ConstructorNode::AddParameter(ParameterNode* parameter)
{
    parameters.Add(parameter);
}

void ConstructorNode::AddInitializer(InitializerNode* initializer)
{
    initializers.Add(initializer);
}

void ConstructorNode::SetConstraint(WhereConstraintNode* constraint_)
{
    constraint.reset(constraint_);
}

void ConstructorNode::SetBody(CompoundStatementNode* body_)
{
    body.reset(body_);
}

Node* ConstructorNode::Clone() const
{
    ConstructorNode* clone = new ConstructorNode(GetSpan(), specifiers);
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone()));
    }
    for (const std::unique_ptr<InitializerNode>& initializer : initializers)
    {
        clone->AddInitializer(static_cast<InitializerNode*>(initializer->Clone()));
    }
    if (constraint)
    {
        clone->SetConstraint(static_cast<WhereConstraintNode*>(constraint->Clone()));
    }
    clone->SetBody(static_cast<CompoundStatementNode*>(body->Clone()));
    return clone;
}

void ConstructorNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    parameters.Read(reader);
    initializers.Read(reader);
    bool hasConstraint = reader.ReadBool();
    if (hasConstraint)
    {
        constraint.reset(reader.ReadWhereConstraintNode());
    }
    body.reset(reader.ReadCompoundStatementNode());
}

void ConstructorNode::Write(Writer& writer)
{
    writer.Write(specifiers);
    parameters.Write(writer);
    initializers.Write(writer);
    bool hasConstraint = constraint != nullptr;
    writer.Write(hasConstraint);
    if (hasConstraint)
    {
        writer.Write(constraint.get());
    }
    writer.Write(body.get());
}


DestructorNode::DestructorNode(const Span& span_) : Node(span_)
{
}

DestructorNode::DestructorNode(const Span& span_, Specifiers specifiers_, CompoundStatementNode* body_) : Node(span_), specifiers(specifiers_), body(body_)
{
}

void DestructorNode::AddParameter(ParameterNode* parameter)
{
    parameters.Add(parameter);
}

void DestructorNode::SetConstraint(WhereConstraintNode* constraint_)
{
    constraint.reset(constraint_);
}

Node* DestructorNode::Clone() const
{
    DestructorNode* clone = new DestructorNode(GetSpan(), specifiers, static_cast<CompoundStatementNode*>(body->Clone()));
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone()));
    }
    if (constraint)
    {
        clone->SetConstraint(static_cast<WhereConstraintNode*>(constraint->Clone()));
    }
    return clone;
}

void DestructorNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    parameters.Read(reader);
    bool hasConstraint = reader.ReadBool();
    if (hasConstraint)
    {
        constraint.reset(reader.ReadWhereConstraintNode());
    }
    body.reset(reader.ReadCompoundStatementNode());
}

void DestructorNode::Write(Writer& writer)
{
    writer.Write(specifiers);
    parameters.Write(writer);
    bool hasConstraint = constraint != nullptr;
    writer.Write(hasConstraint);
    if (hasConstraint)
    {
        writer.Write(constraint.get());
    }
    writer.Write(body.get());
}

MemberFunctionNode::MemberFunctionNode(const Span& span_) : Node(span_)
{
}

MemberFunctionNode::MemberFunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, FunctionGroupIdNode* groupId_) : 
    Node(span_), specifiers(specifiers_), returnTypeExpr(returnTypeExpr_), groupId(groupId_)
{
}

void MemberFunctionNode::AddParameter(ParameterNode* parameter)
{
    parameters.Add(parameter);
}

void MemberFunctionNode::SetConstraint(WhereConstraintNode* constraint_)
{
    constraint.reset(constraint_);
}

void MemberFunctionNode::SetBody(CompoundStatementNode* body_)
{
    body.reset(body_);
}

void MemberFunctionNode::SetConst()
{
    // todo
}

Node* MemberFunctionNode::Clone() const
{
    MemberFunctionNode* clone = new MemberFunctionNode(GetSpan(), specifiers, returnTypeExpr->Clone(), static_cast<FunctionGroupIdNode*>(groupId->Clone()));
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

void MemberFunctionNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    returnTypeExpr.reset(reader.ReadNode());
    groupId.reset(reader.ReadFunctionGroupIdNode());
    parameters.Read(reader);
    bool hasConstraint = reader.ReadBool();
    if (hasConstraint)
    {
        constraint.reset(reader.ReadWhereConstraintNode());
    }
    body.reset(reader.ReadCompoundStatementNode());
}

void MemberFunctionNode::Write(Writer& writer)
{
    writer.Write(specifiers);
    writer.Write(returnTypeExpr.get());
    writer.Write(groupId.get());
    parameters.Write(writer);
    bool hasConstraint = constraint != nullptr;
    writer.Write(hasConstraint);
    if (hasConstraint)
    {
        writer.Write(constraint.get());
    }
    writer.Write(body.get());
}

ConversionFunctionNode::ConversionFunctionNode(const Span& span_) : Node(span_)
{
}

ConversionFunctionNode::ConversionFunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, bool setConst_, WhereConstraintNode* constraint_, CompoundStatementNode* body_) :
    Node(span_), specifiers(specifiers_), returnTypeExpr(returnTypeExpr_), constraint(constraint_), body(body_)
{
    // setConst todo
}

Node* ConversionFunctionNode::Clone() const
{
    WhereConstraintNode* clonedConstraint = nullptr;
    if (constraint)
    {
        clonedConstraint = static_cast<WhereConstraintNode*>(constraint->Clone());
    }
    return new ConversionFunctionNode(GetSpan(), specifiers, returnTypeExpr->Clone(), false, clonedConstraint, static_cast<CompoundStatementNode*>(body->Clone()));
}

void ConversionFunctionNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    returnTypeExpr.reset(reader.ReadNode());
    bool hasConstraint = reader.ReadBool();
    if (hasConstraint)
    {
        constraint.reset(reader.ReadWhereConstraintNode());
    }
    body.reset(reader.ReadCompoundStatementNode());
}

void ConversionFunctionNode::Write(Writer& writer)
{
    writer.Write(specifiers);
    writer.Write(returnTypeExpr.get());
    bool hasConstraint = constraint != nullptr;
    writer.Write(hasConstraint);
    if (hasConstraint)
    {
        writer.Write(constraint.get());
    }
    writer.Write(body.get());
}

MemberVariableNode::MemberVariableNode(const Span& span_) : Node(span_)
{
}

MemberVariableNode::MemberVariableNode(const Span& span_, Specifiers specifiers_, Node* typeExpr_, IdentifierNode* id_) : Node(span_), specifiers(specifiers_), typeExpr(typeExpr_), id(id_)
{
}

Node* MemberVariableNode::Clone() const
{
    return new MemberVariableNode(GetSpan(), specifiers, typeExpr->Clone(), static_cast<IdentifierNode*>(id->Clone()));
}

void MemberVariableNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    typeExpr.reset(reader.ReadNode());
    id.reset(reader.ReadIdentifierNode());
}

void MemberVariableNode::Write(Writer& writer)
{
    writer.Write(specifiers);
    writer.Write(typeExpr.get());
    writer.Write(id.get());
}

} } // namespace Cm::Ast
