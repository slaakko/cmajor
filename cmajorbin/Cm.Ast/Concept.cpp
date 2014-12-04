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

BinaryConstraintNode::BinaryConstraintNode(const Span& span_) : ConstraintNode(span_)
{
}

BinaryConstraintNode::BinaryConstraintNode(const Span& span_, ConstraintNode* left_, ConstraintNode* right_) : ConstraintNode(span_), left(left_), right(right_)
{
}

void BinaryConstraintNode::Read(Reader& reader)
{
    left.reset(reader.ReadConstraintNode());
    right.reset(reader.ReadConstraintNode());
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

Node* DisjunctiveConstraintNode::Clone() const
{
    return new DisjunctiveConstraintNode(GetSpan(), static_cast<ConstraintNode*>(Left()->Clone()), static_cast<ConstraintNode*>(Right()->Clone()));
}

std::string DisjunctiveConstraintNode::ToString() const
{
    return Left()->ToString() + " or " + Right()->ToString();
}

void DisjunctiveConstraintNode::Accept(Visitor& visitor) 
{
    visitor.BeginVisit(*this);
    Left()->Accept(visitor);
    Right()->Accept(visitor);
    visitor.EndVisit(*this);
}

ConjunctiveConstraintNode::ConjunctiveConstraintNode(const Span& span_) : BinaryConstraintNode(span_)
{
}

ConjunctiveConstraintNode::ConjunctiveConstraintNode(const Span& span_, ConstraintNode* left_, ConstraintNode* right_) : BinaryConstraintNode(span_, left_, right_)
{
}

Node* ConjunctiveConstraintNode::Clone() const
{
    return new ConjunctiveConstraintNode(GetSpan(), static_cast<ConstraintNode*>(Left()->Clone()), static_cast<ConstraintNode*>(Right()->Clone()));
}

std::string ConjunctiveConstraintNode::ToString() const
{
    return Left()->ToString() + " and " + Right()->ToString();
}

void ConjunctiveConstraintNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    Left()->Accept(visitor);
    Right()->Accept(visitor);
    visitor.EndVisit(*this);
}

WhereConstraintNode::WhereConstraintNode(const Span& span_) : ConstraintNode(span_)
{
}

WhereConstraintNode::WhereConstraintNode(const Span& span_, ConstraintNode* constraint_) : ConstraintNode(span_), constraint(constraint_)
{
}

Node* WhereConstraintNode::Clone() const
{
    return new WhereConstraintNode(GetSpan(), static_cast<ConstraintNode*>(constraint->Clone()));
}

void WhereConstraintNode::Read(Reader& reader)
{
    constraint.reset(reader.ReadConstraintNode());
}

void WhereConstraintNode::Write(Writer& writer)
{
    writer.Write(constraint.get());
}

std::string WhereConstraintNode::ToString() const
{
    return "where " + constraint->ToString();
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
}

Node* IsConstraintNode::Clone() const
{
    return new IsConstraintNode(GetSpan(), typeExpr->Clone(), conceptOrTypeName->Clone());
}

void IsConstraintNode::Read(Reader& reader)
{
    typeExpr.reset(reader.ReadNode());
    conceptOrTypeName.reset(reader.ReadNode());
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

void IsConstraintNode::Accept(Visitor& visitor)
{
    return visitor.Visit(*this);
}

MultiParamConstraintNode::MultiParamConstraintNode(const Span& span_) : ConstraintNode(span_)
{
}

MultiParamConstraintNode::MultiParamConstraintNode(const Span& span_, IdentifierNode* conceptId_) : ConstraintNode(span_), conceptId(conceptId_)
{
}

void MultiParamConstraintNode::AddTypeExpr(Node* typeExpr)
{
    typeExprNodes.Add(typeExpr);
}

Node* MultiParamConstraintNode::Clone() const
{
    MultiParamConstraintNode* clone = new MultiParamConstraintNode(GetSpan(), static_cast<IdentifierNode*>(conceptId->Clone()));
    for (const std::unique_ptr<Node>& typeExpr : typeExprNodes)
    {
        clone->AddTypeExpr(typeExpr->Clone());
    }
    return clone;
}

void MultiParamConstraintNode::Read(Reader& reader)
{
    conceptId.reset(reader.ReadIdentifierNode());
    typeExprNodes.Read(reader);
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

void MultiParamConstraintNode::Accept(Visitor& visitor)
{
    return visitor.Visit(*this);
}

TypenameConstraintNode::TypenameConstraintNode(const Span& span_) : ConstraintNode(span_)
{
}

TypenameConstraintNode::TypenameConstraintNode(const Span& span_, IdentifierNode* typeId_) : ConstraintNode(span_), typeId(typeId_)
{
}

Node* TypenameConstraintNode::Clone() const
{
    return new TypenameConstraintNode(GetSpan(), static_cast<IdentifierNode*>(typeId->Clone()));
}

void TypenameConstraintNode::Read(Reader& reader)
{
    typeId.reset(reader.ReadIdentifierNode());
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

void ConstructorConstraintNode::AddParameter(ParameterNode* parameter)
{
    parameters.Add(parameter);
}

Node* ConstructorConstraintNode::Clone() const
{
    ConstructorConstraintNode* clone = new ConstructorConstraintNode(GetSpan());
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone()));
    }
    return clone;
}

void ConstructorConstraintNode::Read(Reader& reader)
{
    parameters.Read(reader);
}

void ConstructorConstraintNode::Write(Writer& writer)
{
    parameters.Write(writer);
}

std::string ConstructorConstraintNode::ToString() const
{
    return "@constructor" + parameters.ToString() + ";";
}

void ConstructorConstraintNode::Accept(Visitor& visitor)
{
    return visitor.Visit(*this);
}

DestructorConstraintNode::DestructorConstraintNode(const Span& span_) : SignatureConstraintNode(span_)
{
}

Node* DestructorConstraintNode::Clone() const
{
    return new DestructorConstraintNode(GetSpan());
}

std::string DestructorConstraintNode::ToString() const 
{
    return "@destructor();";
    
}
void DestructorConstraintNode::Accept(Visitor& visitor)
{
    return visitor.Visit(*this);
}

MemberFunctionConstraintNode::MemberFunctionConstraintNode(const Span& span_) : SignatureConstraintNode(span_)
{
}

MemberFunctionConstraintNode::MemberFunctionConstraintNode(const Span& span_, Node* returnTypeExpr_, IdentifierNode* typeParamId_, FunctionGroupIdNode* functionGroupId_) :
    SignatureConstraintNode(span_), returnTypeExpr(returnTypeExpr_), typeParamId(typeParamId_), functionGroupId(functionGroupId_)
{
}

void MemberFunctionConstraintNode::AddParameter(ParameterNode* parameter) 
{
    parameters.Add(parameter);
}

Node* MemberFunctionConstraintNode::Clone() const
{
    MemberFunctionConstraintNode* clone = new MemberFunctionConstraintNode(GetSpan(), returnTypeExpr->Clone(), static_cast<IdentifierNode*>(typeParamId->Clone()),
        static_cast<FunctionGroupIdNode*>(functionGroupId->Clone()));
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone()));
    }
    return clone;
}

void MemberFunctionConstraintNode::Read(Reader& reader)
{
    returnTypeExpr.reset(reader.ReadNode());
    typeParamId.reset(reader.ReadIdentifierNode());
    functionGroupId.reset(reader.ReadFunctionGroupIdNode());
    parameters.Read(reader);
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
}

void FunctionConstraintNode::AddParameter(ParameterNode* parameter)
{
    parameters.Add(parameter);
}

Node* FunctionConstraintNode::Clone() const
{
    FunctionConstraintNode* clone = new FunctionConstraintNode(GetSpan(), returnTypeExpr->Clone(), static_cast<FunctionGroupIdNode*>(functionGroupId->Clone()));
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone()));
    }
    return clone;
}

void FunctionConstraintNode::Read(Reader& reader)
{
    returnTypeExpr.reset(reader.ReadNode());
    functionGroupId.reset(reader.ReadFunctionGroupIdNode());
    parameters.Read(reader);
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

AxiomStatementNode::AxiomStatementNode(const Span& span_, Node* expression_) : Node(span_), expression(expression_)
{
}

Node* AxiomStatementNode::Clone() const
{
    return new AxiomStatementNode(GetSpan(), expression->Clone());
}

void AxiomStatementNode::Read(Reader& reader)
{
    expression.reset(reader.ReadNode());
}

void AxiomStatementNode::Write(Writer& writer)
{
    writer.Write(expression.get());
}


std::string AxiomStatementNode::ToString() const
{
    return expression->ToString() + ";";
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
}

void AxiomNode::AddParameter(ParameterNode* parameter)
{
    parameters.Add(parameter);
}

void AxiomNode::AddStatement(AxiomStatementNode* statement)
{
    axiomStatements.Add(statement);
}

Node* AxiomNode::Clone() const
{
    IdentifierNode* clonedId = nullptr;
    if (id)
    {
        clonedId = static_cast<IdentifierNode*>(id->Clone());
    }
    AxiomNode* clone = new AxiomNode(GetSpan(), clonedId);
    for (const std::unique_ptr<ParameterNode>& parameter : parameters)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameter->Clone()));
    }
    for (const std::unique_ptr<AxiomStatementNode>& axiomStatement : axiomStatements)
    {
        clone->AddStatement(static_cast<AxiomStatementNode*>(axiomStatement->Clone()));
    }
    return clone;
}

void AxiomNode::Read(Reader& reader)
{
    bool hasId = reader.ReadBool();
    if (hasId)
    {
        id.reset(reader.ReadIdentifierNode());
    }
    parameters.Read(reader);
    axiomStatements.Read(reader);
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
}

void ConceptIdNode::AddTypeParameter(Node* typeParameter)
{
    typeParameters.Add(typeParameter);
}

Node* ConceptIdNode::Clone() const
{
    ConceptIdNode* clone = new ConceptIdNode(GetSpan(), static_cast<IdentifierNode*>(id->Clone()));
    for (const std::unique_ptr<Node>& typeParameter : typeParameters)
    {
        clone->AddTypeParameter(typeParameter->Clone());
    }
    return clone;
}

void ConceptIdNode::Read(Reader& reader)
{
    id.reset(reader.ReadIdentifierNode());
    typeParameters.Read(reader);
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

ConceptNode::ConceptNode(const Span& span_) : Node(span_), parent(nullptr)
{
}

ConceptNode::ConceptNode(const Span& span_, Specifiers specifiers_, IdentifierNode* id_) : Node(span_), specifiers(specifiers_), id(id_), parent(nullptr)
{
}

const std::string& ConceptNode::FirstTypeParameter() const
{
    return static_cast<const IdentifierNode*>(typeParameters[0])->Str();
}

void ConceptNode::AddTypeParameter(Node* typeParameter)
{
    typeParameters.Add(typeParameter);
}

void ConceptNode::SetRefinement(ConceptIdNode* refinement_)
{
    refinement.reset(refinement_);
}

void ConceptNode::AddConstraint(ConstraintNode* constraint)
{
    constraints.Add(constraint);
}

void ConceptNode::AddAxiom(AxiomNode* axiom)
{
    axioms.Add(axiom);
}

Node* ConceptNode::Clone() const
{
    ConceptNode* clone = new ConceptNode(GetSpan(), specifiers, static_cast<IdentifierNode*>(id->Clone()));
    for (const std::unique_ptr<Node>& typeParameter : typeParameters)
    {
        clone->AddTypeParameter(typeParameter->Clone());
    }
    if (refinement)
    {
        clone->SetRefinement(static_cast<ConceptIdNode*>(refinement->Clone()));
    }
    for (const std::unique_ptr<ConstraintNode>& constraint : constraints)
    {
        clone->AddConstraint(static_cast<ConstraintNode*>(constraint->Clone()));
    }
    for (const std::unique_ptr<Node>& axiom : axioms)
    {
        clone->AddAxiom(static_cast<AxiomNode*>(axiom->Clone()));
    }
    return clone;
}

void ConceptNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    id.reset(reader.ReadIdentifierNode());
    typeParameters.Read(reader);
    bool hasRefinement = reader.ReadBool();
    if (hasRefinement)
    {
        refinement.reset(reader.ReadConceptIdNode());
    }
    constraints.Read(reader);
    axioms.Read(reader);
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

Node* ConceptNode::Parent() const
{
    return parent;
}

void ConceptNode::SetParent(Node* parent_)
{
    parent = parent_;
}

std::string ConceptNode::Name() const
{ 
    return id->Str(); 
}

void ConceptNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    constraints.Accept(visitor);
    axioms.Accept(visitor);
    visitor.EndVisit(*this);
}

} } // namespace Cm::Ast
