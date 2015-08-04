/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Concept.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Ast {

AxiomStatementNodeList::AxiomStatementNodeList()
{
}

void AxiomStatementNodeList::Read(Reader& reader)
{
    uint32_t n = reader.ReadUInt();
    for (uint32_t i = 0; i < n; ++i)
    {
        axiomStatementNodes.push_back(std::unique_ptr<AxiomStatementNode>(reader.ReadAxiomStatementNode()));
    }
}

void AxiomStatementNodeList::Write(Writer& writer)
{
    uint32_t n = static_cast<uint32_t>(axiomStatementNodes.size());
    writer.Write(n);
    for (uint32_t i = 0; i < n; ++i)
    {
        writer.Write(axiomStatementNodes[i].get());
    }
}

void AxiomStatementNodeList::Print(CodeFormatter& formatter)
{
    for (const std::unique_ptr<AxiomStatementNode>& axiomStatement : axiomStatementNodes)
    {
        formatter.WriteLine(axiomStatement->ToString());
    }
}

void AxiomStatementNodeList::Accept(Visitor& visitor)
{
    for (const std::unique_ptr<AxiomStatementNode>& axiomStatement : axiomStatementNodes)
    {
        axiomStatement->Accept(visitor);
    }
}

void AxiomStatementNodeList::SetParent(Node* parent)
{
    for (const std::unique_ptr<AxiomStatementNode>& axiomStatement : axiomStatementNodes)
    {
        axiomStatement->SetParent(parent);
    }
}

ConstraintNode::ConstraintNode(const Span& span_) : Node(span_)
{
}

ConstraintNodeList::ConstraintNodeList()
{
}

void ConstraintNodeList::Read(Reader& reader)
{
    uint32_t n = reader.ReadUInt();
    for (uint32_t i = 0; i < n; ++i)
    {
        constraintNodes.push_back(std::unique_ptr<ConstraintNode>(reader.ReadConstraintNode()));
    }
}

void ConstraintNodeList::Write(Writer& writer)
{
    uint32_t n = static_cast<uint32_t>(constraintNodes.size());
    writer.Write(n);
    for (uint32_t i = 0; i < n; ++i)
    {
        writer.Write(constraintNodes[i].get());
    }
}

void ConstraintNodeList::Print(CodeFormatter& formatter)
{
    for (const std::unique_ptr<ConstraintNode>& constraint : constraintNodes)
    {
        formatter.WriteLine(constraint->ToString());
    }
}

void ConstraintNodeList::Accept(Visitor& visitor)
{
    for (const std::unique_ptr<ConstraintNode>& constraint : constraintNodes)
    {
        constraint->Accept(visitor);
    }
}

void ConstraintNodeList::SetParent(Node* parent)
{
    for (const std::unique_ptr<ConstraintNode>& constraint : constraintNodes)
    {
        constraint->SetParent(parent);
    }
}

BinaryConstraintNode::BinaryConstraintNode(const Span& span_) : ConstraintNode(span_)
{
}

BinaryConstraintNode::BinaryConstraintNode(const Span& span_, ConstraintNode* left_, ConstraintNode* right_) : ConstraintNode(span_), left(left_), right(right_)
{
    left->SetParent(this);
    right->SetParent(this);
}

void BinaryConstraintNode::Read(Reader& reader)
{
    left.reset(reader.ReadConstraintNode());
    left->SetParent(this);
    right.reset(reader.ReadConstraintNode());
    right->SetParent(this);
}

void BinaryConstraintNode::Write(Writer& writer)
{
    writer.Write(left.get());
    writer.Write(right.get());
}

DisjunctiveConstraintNode::DisjunctiveConstraintNode(const Span& span_) : BinaryConstraintNode(span_)
{
}

DisjunctiveConstraintNode::DisjunctiveConstraintNode(const Span& span_, ConstraintNode* left_, ConstraintNode* right_) : BinaryConstraintNode(span_, left_, right_)
{
}

Node* DisjunctiveConstraintNode::Clone(CloneContext& cloneContext) const
{
    return new DisjunctiveConstraintNode(GetSpan(), static_cast<ConstraintNode*>(Left()->Clone(cloneContext)), static_cast<ConstraintNode*>(Right()->Clone(cloneContext)));
}

std::string DisjunctiveConstraintNode::ToString() const
{
    return Left()->ToString() + " or " + Right()->ToString();
}

std::string DisjunctiveConstraintNode::DocId() const
{
    return Left()->DocId() + ".or." + Right()->DocId();
}

void DisjunctiveConstraintNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ConjunctiveConstraintNode::ConjunctiveConstraintNode(const Span& span_) : BinaryConstraintNode(span_)
{
}

ConjunctiveConstraintNode::ConjunctiveConstraintNode(const Span& span_, ConstraintNode* left_, ConstraintNode* right_) : BinaryConstraintNode(span_, left_, right_)
{
}

Node* ConjunctiveConstraintNode::Clone(CloneContext& cloneContext) const
{
    return new ConjunctiveConstraintNode(GetSpan(), static_cast<ConstraintNode*>(Left()->Clone(cloneContext)), static_cast<ConstraintNode*>(Right()->Clone(cloneContext)));
}

std::string ConjunctiveConstraintNode::ToString() const
{
    return Left()->ToString() + " and " + Right()->ToString();
}

std::string ConjunctiveConstraintNode::DocId() const
{
    return Left()->DocId() + ".and." + Right()->DocId();
}

void ConjunctiveConstraintNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

WhereConstraintNode::WhereConstraintNode(const Span& span_) : ConstraintNode(span_)
{
}

WhereConstraintNode::WhereConstraintNode(const Span& span_, ConstraintNode* constraint_) : ConstraintNode(span_), constraint(constraint_)
{
    constraint->SetParent(this);
}

Node* WhereConstraintNode::Clone(CloneContext& cloneContext) const
{
    return new WhereConstraintNode(GetSpan(), static_cast<ConstraintNode*>(constraint->Clone(cloneContext)));
}

void WhereConstraintNode::Read(Reader& reader)
{
    constraint.reset(reader.ReadConstraintNode());
    constraint->SetParent(this);
}

void WhereConstraintNode::Write(Writer& writer)
{
    writer.Write(constraint.get());
}

std::string WhereConstraintNode::ToString() const
{
    return "where " + constraint->ToString();
}

std::string WhereConstraintNode::DocId() const
{
    return "where." + constraint->DocId();
}

void WhereConstraintNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    constraint->Accept(visitor);
    visitor.EndVisit(*this);
}

IsConstraintNode::IsConstraintNode(const Span& span_) : ConstraintNode(span_)
{
}

IsConstraintNode::IsConstraintNode(const Span& span_, Node* typeExpr_, Node* conceptOrTypeName_) : ConstraintNode(span_), typeExpr(typeExpr_), conceptOrTypeName(conceptOrTypeName_)
{
    typeExpr->SetParent(this);
    conceptOrTypeName->SetParent(this);
}

Node* IsConstraintNode::Clone(CloneContext& cloneContext) const
{
    return new IsConstraintNode(GetSpan(), typeExpr->Clone(cloneContext), conceptOrTypeName->Clone(cloneContext));
}

void IsConstraintNode::Read(Reader& reader)
{
    typeExpr.reset(reader.ReadNode());
    typeExpr->SetParent(this);
    conceptOrTypeName.reset(reader.ReadNode());
    conceptOrTypeName->SetParent(this);
}

void IsConstraintNode::Write(Writer& writer)
{
    writer.Write(typeExpr.get());
    writer.Write(conceptOrTypeName.get());
}

std::string IsConstraintNode::ToString() const
{
    return typeExpr->ToString() + " is " + conceptOrTypeName->ToString();
}

std::string IsConstraintNode::DocId() const
{
    return typeExpr->DocId() + ".is." + conceptOrTypeName->DocId();
}

void IsConstraintNode::Accept(Visitor& visitor)
{
    return visitor.Visit(*this);
}

MultiParamConstraintNode::MultiParamConstraintNode(const Span& span_) : ConstraintNode(span_)
{
}

MultiParamConstraintNode::MultiParamConstraintNode(const Span& span_, IdentifierNode* conceptId_) : ConstraintNode(span_), conceptId(conceptId_)
{
    conceptId->SetParent(this);
}

void MultiParamConstraintNode::AddTypeExpr(Node* typeExpr)
{
    typeExpr->SetParent(this);
    typeExprNodes.Add(typeExpr);
}

Node* MultiParamConstraintNode::Clone(CloneContext& cloneContext) const
{
    MultiParamConstraintNode* clone = new MultiParamConstraintNode(GetSpan(), static_cast<IdentifierNode*>(conceptId->Clone(cloneContext)));
    for (const std::unique_ptr<Node>& typeExpr : typeExprNodes)
    {
        clone->AddTypeExpr(typeExpr->Clone(cloneContext));
    }
    return clone;
}

void MultiParamConstraintNode::Read(Reader& reader)
{
    conceptId.reset(reader.ReadIdentifierNode());
    conceptId->SetParent(this);
    typeExprNodes.Read(reader);
    typeExprNodes.SetParent(this);
}

void MultiParamConstraintNode::Write(Writer& writer)
{
    writer.Write(conceptId.get());
    typeExprNodes.Write(writer);
}

std::string MultiParamConstraintNode::ToString() const
{
    std::string s = conceptId->ToString();
    s.append(1, '<');
    bool first = true;
    for (const std::unique_ptr<Node>& typeExpr : typeExprNodes)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            s.append(", ");
        }
        s.append(typeExpr->ToString());
    }
    s.append(1, '>');
    return s;
}

std::string MultiParamConstraintNode::DocId() const
{
    std::string docId = conceptId->ToString();
    for (const std::unique_ptr<Node>& typeExpr : typeExprNodes)
    {
        docId.append(1, '.').append(typeExpr->DocId());
    }
    return docId;
}

void MultiParamConstraintNode::Accept(Visitor& visitor)
{
    return visitor.Visit(*this);
}

TypenameConstraintNode::TypenameConstraintNode(const Span& span_) : ConstraintNode(span_)
{
}

TypenameConstraintNode::TypenameConstraintNode(const Span& span_, Node* typeId_) : ConstraintNode(span_), typeId(typeId_)
{
    typeId->SetParent(this);
}

Node* TypenameConstraintNode::Clone(CloneContext& cloneContext) const
{
    return new TypenameConstraintNode(GetSpan(), static_cast<Node*>(typeId->Clone(cloneContext)));
}

void TypenameConstraintNode::Read(Reader& reader)
{
    typeId.reset(reader.ReadNode());
    typeId->SetParent(this);
}

void TypenameConstraintNode::Write(Writer& writer)
{
    writer.Write(typeId.get());
}

std::string TypenameConstraintNode::ToString() const
{
    return "typename " + typeId->ToString() + ";";
}

void TypenameConstraintNode::Accept(Visitor& visitor)
{
    return visitor.Visit(*this);
}

SignatureConstraintNode::SignatureConstraintNode(const Span& span_) : ConstraintNode(span_)
{
}

ConstructorConstraintNode::ConstructorConstraintNode(const Span& span_) : SignatureConstraintNode(span_)
{
}

ConstructorConstraintNode::ConstructorConstraintNode(const Span& span_, IdentifierNode* typeParamId_) : SignatureConstraintNode(span_), typeParamId(typeParamId_)
{
}

void ConstructorConstraintNode::AddParameter(ParameterNode* parameter)
{
    parameter->SetParent(this);
    parameters.Add(parameter);
}

Node* ConstructorConstraintNode::Clone(CloneContext& cloneContext) const
{
    ConstructorConstraintNode* clone = new ConstructorConstraintNode(GetSpan(), static_cast<IdentifierNode*>(typeParamId->Clone(cloneContext)));
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone(cloneContext)));
    }
    return clone;
}

void ConstructorConstraintNode::Read(Reader& reader)
{
    typeParamId.reset(reader.ReadIdentifierNode());
    typeParamId->SetParent(this);
    parameters.Read(reader);
    parameters.SetParent(this);
}

void ConstructorConstraintNode::Write(Writer& writer)
{
    writer.Write(typeParamId.get());
    parameters.Write(writer);
}

std::string ConstructorConstraintNode::ToString() const
{
    std::string text(typeParamId->ToString());
    text.append(parameters.ToString()).append(";");
    return text;
}

void ConstructorConstraintNode::Accept(Visitor& visitor)
{
    return visitor.Visit(*this);
}

DestructorConstraintNode::DestructorConstraintNode(const Span& span_) : SignatureConstraintNode(span_)
{
}

DestructorConstraintNode::DestructorConstraintNode(const Span& span_, IdentifierNode* typeParamId_) : SignatureConstraintNode(span_), typeParamId(typeParamId_)
{
}

Node* DestructorConstraintNode::Clone(CloneContext& cloneContext) const
{
    return new DestructorConstraintNode(GetSpan(), static_cast<IdentifierNode*>(typeParamId->Clone(cloneContext)));
}

std::string DestructorConstraintNode::ToString() const 
{
    std::string text("~" + typeParamId->ToString());
    text.append("();");
    return text;
}
void DestructorConstraintNode::Accept(Visitor& visitor)
{
    return visitor.Visit(*this);
}

void DestructorConstraintNode::Read(Reader& reader)
{
    typeParamId.reset(reader.ReadIdentifierNode());
    typeParamId->SetParent(this);
}

void DestructorConstraintNode::Write(Writer& writer)
{
    writer.Write(typeParamId.get());
}

MemberFunctionConstraintNode::MemberFunctionConstraintNode(const Span& span_) : SignatureConstraintNode(span_)
{
}

MemberFunctionConstraintNode::MemberFunctionConstraintNode(const Span& span_, Node* returnTypeExpr_, IdentifierNode* typeParamId_, FunctionGroupIdNode* functionGroupId_) :
    SignatureConstraintNode(span_), returnTypeExpr(returnTypeExpr_), typeParamId(typeParamId_), functionGroupId(functionGroupId_)
{
    returnTypeExpr->SetParent(this);
    typeParamId->SetParent(this);
    functionGroupId->SetParent(this);
}

void MemberFunctionConstraintNode::AddParameter(ParameterNode* parameter) 
{
    parameter->SetParent(this);
    parameters.Add(parameter);
}

Node* MemberFunctionConstraintNode::Clone(CloneContext& cloneContext) const
{
    MemberFunctionConstraintNode* clone = new MemberFunctionConstraintNode(GetSpan(), returnTypeExpr->Clone(cloneContext), static_cast<IdentifierNode*>(typeParamId->Clone(cloneContext)),
        static_cast<FunctionGroupIdNode*>(functionGroupId->Clone(cloneContext)));
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone(cloneContext)));
    }
    return clone;
}

void MemberFunctionConstraintNode::Read(Reader& reader)
{
    returnTypeExpr.reset(reader.ReadNode());
    returnTypeExpr->SetParent(this);
    typeParamId.reset(reader.ReadIdentifierNode());
    typeParamId->SetParent(this);
    functionGroupId.reset(reader.ReadFunctionGroupIdNode());
    functionGroupId->SetParent(this);
    parameters.Read(reader);
    parameters.SetParent(this);
}

void MemberFunctionConstraintNode::Write(Writer& writer)
{
    writer.Write(returnTypeExpr.get());
    writer.Write(typeParamId.get());
    writer.Write(functionGroupId.get());
    parameters.Write(writer);
}

std::string MemberFunctionConstraintNode::ToString() const
{
    std::string s = returnTypeExpr->ToString();
    s.append(1, ' ').append(typeParamId->ToString()).append(1, '.').append(functionGroupId->ToString()).append(parameters.ToString()).append(1, ';');
    return s;
}

void MemberFunctionConstraintNode::Accept(Visitor& visitor)
{
    return visitor.Visit(*this);
}

FunctionConstraintNode::FunctionConstraintNode(const Span& span_) : SignatureConstraintNode(span_)
{
}

FunctionConstraintNode::FunctionConstraintNode(const Span& span_, Node* returnTypeExpr_, FunctionGroupIdNode* functionGroupId_) :
    SignatureConstraintNode(span_), returnTypeExpr(returnTypeExpr_), functionGroupId(functionGroupId_)
{
    returnTypeExpr->SetParent(this);
    functionGroupId->SetParent(this);
}

void FunctionConstraintNode::AddParameter(ParameterNode* parameter)
{
    parameter->SetParent(this);
    parameters.Add(parameter);
}

Node* FunctionConstraintNode::Clone(CloneContext& cloneContext) const
{
    FunctionConstraintNode* clone = new FunctionConstraintNode(GetSpan(), returnTypeExpr->Clone(cloneContext), static_cast<FunctionGroupIdNode*>(functionGroupId->Clone(cloneContext)));
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone(cloneContext)));
    }
    return clone;
}

void FunctionConstraintNode::Read(Reader& reader)
{
    returnTypeExpr.reset(reader.ReadNode());
    returnTypeExpr->SetParent(this);
    functionGroupId.reset(reader.ReadFunctionGroupIdNode());
    functionGroupId->SetParent(this);
    parameters.Read(reader);
    parameters.SetParent(this);
}

void FunctionConstraintNode::Write(Writer& writer)
{
    writer.Write(returnTypeExpr.get());
    writer.Write(functionGroupId.get());
    parameters.Write(writer);
}

std::string FunctionConstraintNode::ToString() const
{
    std::string s = returnTypeExpr->ToString();
    s.append(1, ' ').append(functionGroupId->ToString()).append(parameters.ToString()).append(1, ';');
    return s;
}

void FunctionConstraintNode::Accept(Visitor& visitor)
{
    return visitor.Visit(*this);
}

AxiomStatementNode::AxiomStatementNode(const Span& span_) : Node(span_)
{
}

AxiomStatementNode::AxiomStatementNode(const Span& span_, Node* expression_, const std::string& text_) : Node(span_), expression(expression_), text(text_)
{
    expression->SetParent(this);
}

Node* AxiomStatementNode::Clone(CloneContext& cloneContext) const
{
    return new AxiomStatementNode(GetSpan(), expression->Clone(cloneContext), text);
}

void AxiomStatementNode::Read(Reader& reader)
{
    expression.reset(reader.ReadNode());
    text = reader.ReadString();
}

void AxiomStatementNode::Write(Writer& writer)
{
    writer.Write(expression.get());
    writer.Write(text);
}

std::string AxiomStatementNode::ToString() const
{
    return text;
}

void AxiomStatementNode::Accept(Visitor& visitor)
{
    return visitor.Visit(*this);
}

AxiomNode::AxiomNode(const Span& span_) : Node(span_)
{
}

AxiomNode::AxiomNode(const Span& span_, IdentifierNode* id_) : Node(span_), id(id_) 
{
    if (id)
    {
        id->SetParent(this);
    }
}

std::string AxiomNode::Name() const
{
    if (id)
    {
        return id->Str();
    }
    return "axiom";
}

void AxiomNode::AddParameter(ParameterNode* parameter)
{
    parameter->SetParent(this);
    parameters.Add(parameter);
}

void AxiomNode::AddStatement(AxiomStatementNode* statement)
{
    statement->SetParent(this);
    axiomStatements.Add(statement);
}

Node* AxiomNode::Clone(CloneContext& cloneContext) const
{
    IdentifierNode* clonedId = nullptr;
    if (id)
    {
        clonedId = static_cast<IdentifierNode*>(id->Clone(cloneContext));
    }
    AxiomNode* clone = new AxiomNode(GetSpan(), clonedId);
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone(cloneContext)));
    }
    for (const std::unique_ptr<AxiomStatementNode>& axiomStatement : axiomStatements)
    {
        clone->AddStatement(static_cast<AxiomStatementNode*>(axiomStatement->Clone(cloneContext)));
    }
    return clone;
}

void AxiomNode::Read(Reader& reader)
{
    bool hasId = reader.ReadBool();
    if (hasId)
    {
        id.reset(reader.ReadIdentifierNode());
        id->SetParent(this);
    }
    parameters.Read(reader);
    parameters.SetParent(this);
    axiomStatements.Read(reader);
    axiomStatements.SetParent(this);
}

void AxiomNode::Write(Writer& writer)
{
    bool hasId = id != nullptr;
    writer.Write(hasId);
    if (hasId)
    {
        writer.Write(id.get());
    }   
    parameters.Write(writer);
    axiomStatements.Write(writer);
}

void AxiomNode::Print(CodeFormatter& formatter)
{
    std::string s = "axiom";
    if (id)
    {
        s.append(1, ' ').append(id->ToString());
    }
    s.append(parameters.ToString());
    formatter.WriteLine(s);
    formatter.WriteLine("{");
    formatter.IncIndent();
    axiomStatements.Print(formatter);
    formatter.DecIndent();
    formatter.WriteLine("}");
}

void AxiomNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    axiomStatements.Accept(visitor);
    visitor.EndVisit(*this);
}

ConceptIdNode::ConceptIdNode(const Span& span_) : Node(span_)
{
}

ConceptIdNode::ConceptIdNode(const Span& span_, IdentifierNode* id_) : Node(span_), id(id_)
{
    id->SetParent(this);
}

void ConceptIdNode::AddTypeParameter(Node* typeParameter)
{
    typeParameter->SetParent(this);
    typeParameters.Add(typeParameter);
}

Node* ConceptIdNode::Clone(CloneContext& cloneContext) const
{
    ConceptIdNode* clone = new ConceptIdNode(GetSpan(), static_cast<IdentifierNode*>(id->Clone(cloneContext)));
    for (const std::unique_ptr<Node>& typeParameter : typeParameters)
    {
        clone->AddTypeParameter(typeParameter->Clone(cloneContext));
    }
    return clone;
}

void ConceptIdNode::Read(Reader& reader)
{
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    typeParameters.Read(reader);
    typeParameters.SetParent(this);
}

void ConceptIdNode::Write(Writer& writer)
{
    writer.Write(id.get());
    typeParameters.Write(writer);
}

std::string ConceptIdNode::ToString() const 
{ 
    return id->ToString() + "<" + typeParameters.ToString() + ">";
}

void ConceptIdNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ConceptNode::ConceptNode(const Span& span_) : Node(span_)
{
}

ConceptNode::ConceptNode(const Span& span_, Specifiers specifiers_, IdentifierNode* id_) : Node(span_), specifiers(specifiers_), id(id_)
{
    id->SetParent(this);
}

const std::string& ConceptNode::FirstTypeParameter() const
{
    return static_cast<const IdentifierNode*>(typeParameters[0])->Str();
}

void ConceptNode::AddTypeParameter(Node* typeParameter)
{
    typeParameter->SetParent(this);
    typeParameters.Add(typeParameter);
}

void ConceptNode::SetRefinement(ConceptIdNode* refinement_)
{
    if (refinement)
    {
        refinement->SetParent(this);
    }
    refinement.reset(refinement_);
}

void ConceptNode::AddConstraint(ConstraintNode* constraint)
{
    constraint->SetParent(this);
    constraints.Add(constraint);
}

void ConceptNode::AddAxiom(AxiomNode* axiom)
{
    axiom->SetParent(this);
    axioms.Add(axiom);
}

Node* ConceptNode::Clone(CloneContext& cloneContext) const
{
    ConceptNode* clone = new ConceptNode(GetSpan(), specifiers, static_cast<IdentifierNode*>(id->Clone(cloneContext)));
    for (const std::unique_ptr<Node>& typeParameter : typeParameters)
    {
        clone->AddTypeParameter(typeParameter->Clone(cloneContext));
    }
    if (refinement)
    {
        clone->SetRefinement(static_cast<ConceptIdNode*>(refinement->Clone(cloneContext)));
    }
    for (const std::unique_ptr<ConstraintNode>& constraint : constraints)
    {
        clone->AddConstraint(static_cast<ConstraintNode*>(constraint->Clone(cloneContext)));
    }
    for (const std::unique_ptr<Node>& axiom : axioms)
    {
        clone->AddAxiom(static_cast<AxiomNode*>(axiom->Clone(cloneContext)));
    }
    return clone;
}

void ConceptNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    typeParameters.Read(reader);
    typeParameters.SetParent(this);
    bool hasRefinement = reader.ReadBool();
    if (hasRefinement)
    {
        refinement.reset(reader.ReadConceptIdNode());
        refinement->SetParent(this);
    }
    constraints.Read(reader);
    constraints.SetParent(this);
    axioms.Read(reader);
    axioms.SetParent(this);
}

void ConceptNode::Write(Writer& writer)
{
    writer.Write(specifiers);
    writer.Write(id.get());
    typeParameters.Write(writer);
    bool hasRefinement = refinement != nullptr;
    writer.Write(hasRefinement);
    if (hasRefinement)
    {
        writer.Write(refinement.get());
    }
    constraints.Write(writer);
    axioms.Write(writer);
}

void ConceptNode::Print(CodeFormatter& formatter) 
{
    std::string s = SpecifierStr(specifiers);
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    s.append("concept ").append(id->ToString()).append(1, '<').append(typeParameters.ToString()).append(1, '>');
    if (refinement)
    {
        s.append(" : ").append(refinement->ToString());
    }
    formatter.WriteLine(s);
    formatter.WriteLine("{");
    formatter.IncIndent();
    constraints.Print(formatter);
    axioms.Print(formatter);
    formatter.DecIndent();
    formatter.WriteLine("}");
}

std::string ConceptNode::Name() const
{ 
    return id->Str(); 
}

void ConceptNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace Cm::Ast
