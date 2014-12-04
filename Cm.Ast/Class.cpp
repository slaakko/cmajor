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
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Ast {

ClassNode::ClassNode(const Span& span_) : Node(span_), parent(nullptr)
{
}

ClassNode::ClassNode(const Span& span_, Specifiers specifiers_, IdentifierNode* id_) : Node(span_), specifiers(specifiers_), id(id_), parent(nullptr)
{
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
    member->SetParent(this);
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

void ClassNode::Print(CodeFormatter& formatter) 
{
    std::string s = SpecifierStr(specifiers);
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    s.append("class ").append(id->ToString()).append(templateParameters.ToString());
    if (baseClassTypeExpr)
    { 
        s.append(" : ").append(baseClassTypeExpr->ToString());
    }
    if (constraint)
    {
        s.append(1, ' ').append(constraint->ToString());
    }
    formatter.WriteLine(s);
    formatter.WriteLine("{");
    formatter.IncIndent();
    members.Print(formatter);
    formatter.DecIndent();
    formatter.WriteLine("}");
}

Node* ClassNode::Parent() const
{
    return parent;
}

void ClassNode::SetParent(Node* parent_)
{
    parent = parent_;
}

std::string ClassNode::Name() const 
{
    return id->Str(); 
}

void ClassNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    members.Accept(visitor);
    visitor.EndVisit(*this);
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

std::string MemberInitializerNode::ToString() const
{
    return memberId->ToString() + "(" + Arguments().ToString() + ")";
}

void MemberInitializerNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
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

std::string BaseInitializerNode::ToString() const
{
    return "base(" + Arguments().ToString() + ")";
}

void BaseInitializerNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
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

std::string ThisInitializerNode::ToString() const
{
    return "this(" + Arguments().ToString() + ")";
}

void ThisInitializerNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
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

std::string InitializerNodeList::ToString() const
{
    if (initializerNodes.empty()) return std::string();
    std::string s = " : ";
    bool first = true;
    for (const std::unique_ptr<InitializerNode>& initializer : initializerNodes)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            s.append(", ");
        }
        s.append(initializer->ToString());
    }
    return s;
}

void InitializerNodeList::Accept(Visitor& visitor)
{
    for (const std::unique_ptr<InitializerNode>& initializer : initializerNodes)
    {
        initializer->Accept(visitor);
    }
}

StaticConstructorNode::StaticConstructorNode(const Span& span_) : Node(span_), parent(nullptr)
{
}

StaticConstructorNode::StaticConstructorNode(const Span& span_, Specifiers specifiers_) : Node(span_), specifiers(specifiers_), parent(nullptr)
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
    bool hasBody = reader.ReadBool();
    if (hasBody)
    {
        body.reset(reader.ReadCompoundStatementNode());
    }
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
    bool hasBody = body != nullptr;
    writer.Write(hasBody);
    if (hasBody)
    {
        writer.Write(body.get());
    }
}

void StaticConstructorNode::Print(CodeFormatter& formatter)
{
    std::string s = SpecifierStr(specifiers);
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    s.append("@static_ctor()").append(initializers.ToString());
    if (constraint)
    {
        s.append(1, ' ').append(constraint->ToString());
    }
    formatter.WriteLine(s);
    body->Print(formatter);
}

Node* StaticConstructorNode::Parent() const
{
    return parent;
}

void StaticConstructorNode::SetParent(Node* parent_)
{
    parent = parent_;
}

void StaticConstructorNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    initializers.Accept(visitor);
    body->Accept(visitor);
    visitor.EndVisit(*this);
}

ConstructorNode::ConstructorNode(const Span& span_) : Node(span_), parent(nullptr)
{
}

ConstructorNode::ConstructorNode(const Span& span_, Specifiers specifiers_) : Node(span_), specifiers(specifiers_), parent(nullptr)
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
    if (body)
    {
        clone->SetBody(static_cast<CompoundStatementNode*>(body->Clone()));
    }
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
    bool hasBody = reader.ReadBool();
    if (hasBody)
    {
        body.reset(reader.ReadCompoundStatementNode());
    }
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
    bool hasBody = body != nullptr;
    writer.Write(hasBody);
    if (hasBody)
    {
        writer.Write(body.get());
    }
}

void ConstructorNode::Print(CodeFormatter& formatter)
{
    std::string s = SpecifierStr(specifiers);
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    s.append("@constructor").append(parameters.ToString()).append(initializers.ToString());
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

Node* ConstructorNode::Parent() const
{
    return parent;
}

void ConstructorNode::SetParent(Node* parent_)
{
    parent = parent_;
}

void ConstructorNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    initializers.Accept(visitor);
    if (body)
    {
        body->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

DestructorNode::DestructorNode(const Span& span_) : Node(span_), parent(nullptr)
{
}

DestructorNode::DestructorNode(const Span& span_, Specifiers specifiers_, CompoundStatementNode* body_) : Node(span_), specifiers(specifiers_), body(body_), parent(nullptr)
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
    CompoundStatementNode* clonedBody = nullptr;
    if (body)
    {
        clonedBody = static_cast<CompoundStatementNode*>(body->Clone());
    }
    DestructorNode* clone = new DestructorNode(GetSpan(), specifiers, clonedBody);
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
    bool hasBody = reader.ReadBool();
    if (hasBody)
    {
        body.reset(reader.ReadCompoundStatementNode());
    }
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
    bool hasBody = body != nullptr;
    writer.Write(hasBody);
    if (hasBody)
    {
        writer.Write(body.get());
    }
}

void DestructorNode::Print(CodeFormatter& formatter)
{
    std::string s = SpecifierStr(specifiers);
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    s.append("@destructor").append("()");
    if (constraint)
    {
        s.append(1, ' ').append(constraint->ToString());
    }
    if (body)
    {
        formatter.WriteLine(s);
        formatter.WriteLine("{");
        formatter.IncIndent();
        body->Print(formatter);
        formatter.DecIndent();
        formatter.WriteLine("}");
    }
    else
    {
        s.append(1, ';');
        formatter.WriteLine(s);
    }
}

Node* DestructorNode::Parent() const
{
    return parent;
}

void DestructorNode::SetParent(Node* parent_)
{
    parent = parent_;
}

void DestructorNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (body)
    {
        body->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

MemberFunctionNode::MemberFunctionNode(const Span& span_) : Node(span_), parent(nullptr)
{
}

MemberFunctionNode::MemberFunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, FunctionGroupIdNode* groupId_) : 
    Node(span_), specifiers(specifiers_), returnTypeExpr(returnTypeExpr_), groupId(groupId_), parent(nullptr)
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
    if (body)
    {
        clone->SetBody(static_cast<CompoundStatementNode*>(body->Clone()));
    }
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
    bool hasBody = reader.ReadBool();
    if (hasBody)
    {
        body.reset(reader.ReadCompoundStatementNode());
    }
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
    bool hasBody = body != nullptr;
    writer.Write(hasBody);
    if (hasBody)
    {
        writer.Write(body.get());
    }
}

void MemberFunctionNode::Print(CodeFormatter& formatter)
{
    std::string s = SpecifierStr(specifiers);
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    s.append(returnTypeExpr->ToString()).append(1, ' ').append(groupId->ToString()).append(parameters.ToString());
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

Node* MemberFunctionNode::Parent() const
{
    return parent;
}

void MemberFunctionNode::SetParent(Node* parent_)
{
    parent = parent_;
}

void MemberFunctionNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (body)
    {
        body->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

ConversionFunctionNode::ConversionFunctionNode(const Span& span_) : Node(span_), parent(nullptr)
{
}

ConversionFunctionNode::ConversionFunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, bool setConst_, WhereConstraintNode* constraint_, CompoundStatementNode* body_) :
    Node(span_), specifiers(specifiers_), returnTypeExpr(returnTypeExpr_), constraint(constraint_), body(body_), parent(nullptr)
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
    bool hasBody = reader.ReadBool();
    if (hasBody)
    {
        body.reset(reader.ReadCompoundStatementNode());
    }
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
    bool hasBody = body != nullptr;
    writer.Write(hasBody);
    if (hasBody)
    {
        writer.Write(body.get());
    }
}

void ConversionFunctionNode::Print(CodeFormatter& formatter)
{
    std::string s = SpecifierStr(specifiers);
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    s.append("operator ").append(returnTypeExpr->ToString()).append("()");
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

Node* ConversionFunctionNode::Parent() const
{
    return parent;
}

void ConversionFunctionNode::SetParent(Node* parent_)
{
    parent = parent_;
}

void ConversionFunctionNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (body)
    {
        body->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

MemberVariableNode::MemberVariableNode(const Span& span_) : Node(span_), parent(nullptr)
{
}

MemberVariableNode::MemberVariableNode(const Span& span_, Specifiers specifiers_, Node* typeExpr_, IdentifierNode* id_) : 
    Node(span_), specifiers(specifiers_), typeExpr(typeExpr_), id(id_), parent(nullptr)
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

void MemberVariableNode::Print(CodeFormatter& formatter)
{
    std::string s = SpecifierStr(specifiers);
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    s.append(typeExpr->ToString()).append(1, ' ').append(id->ToString()).append(1, ';');
    formatter.WriteLine(s);
}

Node* MemberVariableNode::Parent() const
{
    return parent;
}

void MemberVariableNode::SetParent(Node* parent_)
{
    parent = parent_;
}

void MemberVariableNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace Cm::Ast
