/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Class.hpp>
#include <Cm.Ast/Clone.hpp>
#include <Cm.Ast/Concept.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Statement.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Ast {

ClassNode::ClassNode(const Span& span_) : Node(span_), compileUnit(nullptr)
{
}

ClassNode::ClassNode(const Span& span_, Specifiers specifiers_, IdentifierNode* id_) : Node(span_), specifiers(specifiers_), id(id_), compileUnit(nullptr)
{
    id->SetParent(this);
}

void ClassNode::AddTemplateParameter(TemplateParameterNode* templateParameter)
{
    templateParameter->SetParent(this);
    templateParameters.Add(templateParameter);
}

void ClassNode::SetBaseClassTypeExpr(Node* baseClassTypeExpr_)
{
    baseClassTypeExpr.reset(baseClassTypeExpr_);
    if (baseClassTypeExpr)
    {
        baseClassTypeExpr->SetParent(this);
    }
}

void ClassNode::SetConstraint(WhereConstraintNode* constraint_)
{
    constraint.reset(constraint_);
    if (constraint)
    {
        constraint->SetParent(this);
    }
}

void ClassNode::AddMember(Node* member)
{
    member->SetParent(this);
    members.Add(member);
}

Node* ClassNode::Clone(CloneContext& cloneContext) const
{
    ClassNode* clone = new ClassNode(GetSpan(), specifiers, static_cast<IdentifierNode*>(id->Clone(cloneContext)));
    if (!cloneContext.InstantiateClassNode())
    {
        for (const std::unique_ptr<TemplateParameterNode>& templateParameter : templateParameters)
        {
            clone->AddTemplateParameter(static_cast<TemplateParameterNode*>(templateParameter->Clone(cloneContext)));
        }
    }
    if (baseClassTypeExpr)
    {
        clone->SetBaseClassTypeExpr(baseClassTypeExpr->Clone(cloneContext));
    }
    if (constraint)
    {
        clone->SetConstraint(static_cast<WhereConstraintNode*>(constraint->Clone(cloneContext)));
    }
    for (const std::unique_ptr<Node>& member : members)
    {
        clone->AddMember(member->Clone(cloneContext));
    }
    return clone;
}

void ClassNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    templateParameters.Read(reader);
    templateParameters.SetParent(this);
    bool hasBaseClass = reader.ReadBool();
    if (hasBaseClass)
    {
        baseClassTypeExpr.reset(reader.ReadNode());
        baseClassTypeExpr->SetParent(this);
    }
    bool hasConstraint = reader.ReadBool();
    if (hasConstraint)
    {
        constraint.reset(reader.ReadWhereConstraintNode());
        constraint->SetParent(this);
    }
    members.Read(reader);
    members.SetParent(this);
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

std::string ClassNode::Name() const 
{
    return id->Str(); 
}

void ClassNode::SetId(IdentifierNode* id_)
{ 
    id.reset(id_); 
}

void ClassNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (!visitor.SkipContent())
    {
        templateParameters.Accept(visitor);
        members.Accept(visitor);
    }
    visitor.EndVisit(*this);
}

InitializerNode::InitializerNode(const Span& span_) : Node(span_)
{
}

void InitializerNode::AddArgument(Node* argument)
{
    argument->SetParent(this);
    arguments.Add(argument);
}

void InitializerNode::Read(Reader& reader)
{
    arguments.Read(reader);
    arguments.SetParent(this);
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
    memberId->SetParent(this);
}

Node* MemberInitializerNode::Clone(CloneContext& cloneContext) const
{
    MemberInitializerNode* clone = new MemberInitializerNode(GetSpan(), static_cast<IdentifierNode*>(memberId->Clone(cloneContext)));
    for (const std::unique_ptr<Node>& argument : Arguments())
    {
        clone->AddArgument(argument->Clone(cloneContext));
    }
    return clone;
}

void MemberInitializerNode::Read(Reader& reader)
{
    InitializerNode::Read(reader);
    memberId.reset(reader.ReadIdentifierNode());
    memberId->SetParent(this);
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

const std::string& MemberInitializerNode::MemberVariableName() const
{
    return memberId->Str();
}

void MemberInitializerNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BaseInitializerNode::BaseInitializerNode(const Span& span_) : InitializerNode(span_)
{
}

Node* BaseInitializerNode::Clone(CloneContext& cloneContext) const
{
    BaseInitializerNode* clone = new BaseInitializerNode(GetSpan());
    for (const std::unique_ptr<Node>& argument : Arguments())
    {
        clone->AddArgument(argument->Clone(cloneContext));
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

Node* ThisInitializerNode::Clone(CloneContext& cloneContext) const
{
    ThisInitializerNode* clone = new ThisInitializerNode(GetSpan());
    for (const std::unique_ptr<Node>& argument : Arguments())
    {
        clone->AddArgument(argument->Clone(cloneContext));
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

void InitializerNodeList::SetParent(Node* parent)
{
    for (const std::unique_ptr<InitializerNode>& initializer : initializerNodes)
    {
        initializer->SetParent(parent);
    }
}

StaticConstructorNode::StaticConstructorNode(const Span& span_) : FunctionNode(span_)
{
}

StaticConstructorNode::StaticConstructorNode(const Span& span_, Specifiers specifiers_) : FunctionNode(span_, specifiers_, nullptr, new FunctionGroupIdNode(span_, "@static_constructor"))
{
}

void StaticConstructorNode::AddInitializer(InitializerNode* initializer)
{
    initializer->SetParent(this);
    initializers.Add(initializer);
}

Node* StaticConstructorNode::Clone(CloneContext& cloneContext) const
{
    StaticConstructorNode* clone = new StaticConstructorNode(GetSpan(), GetSpecifiers());
    for (const std::unique_ptr<InitializerNode>& initializer : initializers)
    {
        clone->AddInitializer(static_cast<InitializerNode*>(initializer->Clone(cloneContext)));
    }
    if (Constraint())
    {
        clone->SetConstraint(static_cast<WhereConstraintNode*>(Constraint()->Clone(cloneContext)));
    }
    if (Body())
    {
        if (cloneContext.InstantiateClassNode())
        {
            Cm::Ast::CloneContext bodyCloneContext;
            clone->SetBodySource(static_cast<Cm::Ast::CompoundStatementNode*>(Body()->Clone(bodyCloneContext)));
        }
        else
        {
            clone->SetBody(static_cast<CompoundStatementNode*>(Body()->Clone(cloneContext)));
        }
    }
    return clone;
}

void StaticConstructorNode::Read(Reader& reader)
{
    FunctionNode::Read(reader);
    initializers.Read(reader);
    initializers.SetParent(this);
}

void StaticConstructorNode::Write(Writer& writer)
{
    FunctionNode::Write(writer);
    initializers.Write(writer);
}

void StaticConstructorNode::Print(CodeFormatter& formatter)
{
    std::string s = SpecifierStr(GetSpecifiers());
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    if (Parent() && Parent()->IsClassNode())
    {
        s.append(Parent()->Name()).append("()");
    }
    else
    {
        s.append("@static_constructor()");
    }
    s.append(initializers.ToString());
    if (Constraint())
    {
        s.append(1, ' ').append(Constraint()->ToString());
    }
    formatter.WriteLine(s);
    if (Body())
    {
        Body()->Print(formatter);
    }
}

void StaticConstructorNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (visitor.VisitBodies())
    {
        if (Body())
        {
            Body()->Accept(visitor);
        }
    }
    visitor.EndVisit(*this);
}

ConstructorNode::ConstructorNode(const Span& span_) : FunctionNode(span_)
{
}

ConstructorNode::ConstructorNode(const Span& span_, Specifiers specifiers_) : FunctionNode(span_, specifiers_, nullptr, new FunctionGroupIdNode(GetSpan(), "@constructor"))
{
}

void ConstructorNode::AddInitializer(InitializerNode* initializer)
{
    initializer->SetParent(this);
    initializers.Add(initializer);
}

Node* ConstructorNode::Clone(CloneContext& cloneContext) const
{
    ConstructorNode* clone = new ConstructorNode(GetSpan(), GetSpecifiers());
    for (const std::unique_ptr<ParameterNode>& parameter : Parameters())
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone(cloneContext)));
    }
    for (const std::unique_ptr<InitializerNode>& initializer : initializers)
    {
        clone->AddInitializer(static_cast<InitializerNode*>(initializer->Clone(cloneContext)));
    }
    if (Constraint())
    {
        clone->SetConstraint(static_cast<WhereConstraintNode*>(Constraint()->Clone(cloneContext)));
    }
    if (Body())
    {
        if (cloneContext.InstantiateClassNode())
        {
            Cm::Ast::CloneContext bodyCloneContext;
            clone->SetBodySource(static_cast<Cm::Ast::CompoundStatementNode*>(Body()->Clone(bodyCloneContext)));
        }
        else
        {
            clone->SetBody(static_cast<CompoundStatementNode*>(Body()->Clone(cloneContext)));
        }
    }
    return clone;
}

void ConstructorNode::Read(Reader& reader)
{
    FunctionNode::Read(reader);
    initializers.Read(reader);
    initializers.SetParent(this);
}

void ConstructorNode::Write(Writer& writer)
{
    FunctionNode::Write(writer);
    initializers.Write(writer);
}

void ConstructorNode::Print(CodeFormatter& formatter)
{
    std::string s = SpecifierStr(GetSpecifiers());
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    if (Parent() && Parent()->IsClassNode())
    {
        s.append(Parent()->Name());
    }
    else
    {
        s.append("@constructor");
    }
    s.append(Parameters().ToString());
    s.append(initializers.ToString());
    if (Constraint())
    {
        s.append(1, ' ').append(Constraint()->ToString());
    }
    if (Body())
    {
        formatter.WriteLine(s);
        Body()->Print(formatter);
    }
    else
    {
        s.append(1, ';');
        formatter.WriteLine(s);
    }
}

void ConstructorNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    const_cast<ParameterNodeList&>(Parameters()).Accept(visitor);
    if (visitor.VisitBodies())
    {
        if (Body())
        {
            Body()->Accept(visitor);
        }
    }
    visitor.EndVisit(*this);
}

DestructorNode::DestructorNode(const Span& span_) : FunctionNode(span_)
{
}

DestructorNode::DestructorNode(const Span& span_, Specifiers specifiers_, CompoundStatementNode* body_) : FunctionNode(span_, specifiers_, nullptr, new FunctionGroupIdNode(GetSpan(), "@destructor"))
{
    SetBody(body_);
}

Node* DestructorNode::Clone(CloneContext& cloneContext) const
{
    CompoundStatementNode* clonedBody = nullptr;
    if (Body())
    {
        if (!cloneContext.InstantiateClassNode())
        {
            clonedBody = static_cast<CompoundStatementNode*>(Body()->Clone(cloneContext));
        }
    }
    DestructorNode* clone = new DestructorNode(GetSpan(), GetSpecifiers(), clonedBody);
    if (Constraint())
    {
        clone->SetConstraint(static_cast<WhereConstraintNode*>(Constraint()->Clone(cloneContext)));
    }
    if (cloneContext.InstantiateClassNode())
    {
        if (Body())
        {
            Cm::Ast::CloneContext bodyCloneContext;
            clone->SetBodySource(static_cast<Cm::Ast::CompoundStatementNode*>(Body()->Clone(bodyCloneContext)));
        }
    }
    return clone;
}

void DestructorNode::Read(Reader& reader)
{
    FunctionNode::Read(reader);
}

void DestructorNode::Write(Writer& writer)
{
    FunctionNode::Write(writer);
}

void DestructorNode::Print(CodeFormatter& formatter)
{
    std::string s = SpecifierStr(GetSpecifiers());
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    if (Parent() && Parent()->IsClassNode())
    {
        s.append("~").append(Parent()->Name()).append("()");
    }
    else
    {
        s.append("@destructor").append("()");
    }
    if (Constraint())
    {
        s.append(1, ' ').append(Constraint()->ToString());
    }
    if (Body())
    {
        formatter.WriteLine(s);
        Body()->Print(formatter);
    }
    else
    {
        s.append(1, ';');
        formatter.WriteLine(s);
    }
}

void DestructorNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (visitor.VisitBodies())
    {
        if (Body())
        {
            Body()->Accept(visitor);
        }
    }
    visitor.EndVisit(*this);
}

MemberFunctionNode::MemberFunctionNode(const Span& span_) : FunctionNode(span_)
{
}

MemberFunctionNode::MemberFunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, FunctionGroupIdNode* groupId_) : 
    FunctionNode(span_, specifiers_, returnTypeExpr_, groupId_), isConst(false)
{
}

void MemberFunctionNode::SetConst()
{
    isConst = true;
}

Node* MemberFunctionNode::Clone(CloneContext& cloneContext) const
{
    MemberFunctionNode* clone = new MemberFunctionNode(GetSpan(), GetSpecifiers(), ReturnTypeExpr()->Clone(cloneContext), static_cast<FunctionGroupIdNode*>(GroupId()->Clone(cloneContext)));
    for (const std::unique_ptr<ParameterNode>& parameter : Parameters())
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone(cloneContext)));
    }
    if (Constraint())
    {
        clone->SetConstraint(static_cast<WhereConstraintNode*>(Constraint()->Clone(cloneContext)));
    }
    if (Body())
    {
        if (cloneContext.InstantiateClassNode())
        {
            Cm::Ast::CloneContext bodyCloneContext;
            clone->SetBodySource(static_cast<Cm::Ast::CompoundStatementNode*>(Body()->Clone(bodyCloneContext)));
        }
        else
        {
            clone->SetBody(static_cast<CompoundStatementNode*>(Body()->Clone(cloneContext)));
        }
    }
    if (isConst)
    {
        clone->SetConst();
    }
    return clone;
}

void MemberFunctionNode::Read(Reader& reader)
{
    FunctionNode::Read(reader);
    isConst = reader.ReadBool();
}

void MemberFunctionNode::Write(Writer& writer)
{
    FunctionNode::Write(writer);
    writer.Write(isConst);
}

void MemberFunctionNode::Print(CodeFormatter& formatter)
{
    std::string s = SpecifierStr(GetSpecifiers());
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    s.append(ReturnTypeExpr()->ToString()).append(1, ' ').append(GroupId()->ToString()).append(Parameters().ToString());
    if (Constraint())
    {
        s.append(1, ' ').append(Constraint()->ToString());
    }
    if (Body())
    {
        formatter.WriteLine(s);
        Body()->Print(formatter);
    }
    else
    {
        s.append(1, ';');
        formatter.WriteLine(s);
    }
}

void MemberFunctionNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    const_cast<ParameterNodeList&>(Parameters()).Accept(visitor);
    if (visitor.VisitBodies())
    {
        if (Body())
        {
            Body()->Accept(visitor);
        }
    }
    visitor.EndVisit(*this);
}

ConversionFunctionNode::ConversionFunctionNode(const Span& span_) : MemberFunctionNode(span_)
{
}

ConversionFunctionNode::ConversionFunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, bool setConst_, WhereConstraintNode* constraint_, CompoundStatementNode* body_) :
    MemberFunctionNode(span_, specifiers_, returnTypeExpr_, new FunctionGroupIdNode(GetSpan(), "@operator_conv"))
{
    SetConstraint(constraint_);
    SetBody(body_);
    if (setConst_)
    {
        SetConst();
    }
}

Node* ConversionFunctionNode::Clone(CloneContext& cloneContext) const
{
    WhereConstraintNode* clonedConstraint = nullptr;
    if (Constraint())
    {
        clonedConstraint = static_cast<WhereConstraintNode*>(Constraint()->Clone(cloneContext));
    }
    CompoundStatementNode* clonedBody = nullptr;
    if (Body())
    {
        if (!cloneContext.InstantiateClassNode())
        {
            clonedBody = static_cast<CompoundStatementNode*>(Body()->Clone(cloneContext));
        }
    }
    ConversionFunctionNode* clone = new ConversionFunctionNode(GetSpan(), GetSpecifiers(), ReturnTypeExpr()->Clone(cloneContext), IsConst(), clonedConstraint, clonedBody);
    if (cloneContext.InstantiateClassNode())
    {
        if (Body())
        {
            Cm::Ast::CloneContext bodyCloneContext;
            clone->SetBodySource(static_cast<Cm::Ast::CompoundStatementNode*>(Body()->Clone(bodyCloneContext)));
        }
    }
    if (IsConst())
    {
        clone->SetConst();
    }
    return clone;
}

void ConversionFunctionNode::Read(Reader& reader)
{
    MemberFunctionNode::Read(reader);
}

void ConversionFunctionNode::Write(Writer& writer)
{
    MemberFunctionNode::Write(writer);
}

void ConversionFunctionNode::Print(CodeFormatter& formatter)
{
    std::string s = SpecifierStr(GetSpecifiers());
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    s.append("operator ").append(ReturnTypeExpr()->ToString()).append("()");
    if (Constraint())
    {
        s.append(1, ' ').append(Constraint()->ToString());
    }
    if (Body())
    {
        formatter.WriteLine(s);
        Body()->Print(formatter);
    }
    else
    {
        s.append(1, ';');
        formatter.WriteLine(s);
    }
}

void ConversionFunctionNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (visitor.VisitBodies())
    {
        if (Body())
        {
            Body()->Accept(visitor);
        }
    }
    visitor.EndVisit(*this);
}

MemberVariableNode::MemberVariableNode(const Span& span_) : Node(span_)
{
}

MemberVariableNode::MemberVariableNode(const Span& span_, Specifiers specifiers_, Node* typeExpr_, IdentifierNode* id_) : 
    Node(span_), specifiers(specifiers_), typeExpr(typeExpr_), id(id_)
{
    typeExpr->SetParent(this);
    id->SetParent(this);
}

Node* MemberVariableNode::Clone(CloneContext& cloneContext) const
{
    return new MemberVariableNode(GetSpan(), specifiers, typeExpr->Clone(cloneContext), static_cast<IdentifierNode*>(id->Clone(cloneContext)));
}

void MemberVariableNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    typeExpr.reset(reader.ReadNode());
    typeExpr->SetParent(this);
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
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

void MemberVariableNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace Cm::Ast
