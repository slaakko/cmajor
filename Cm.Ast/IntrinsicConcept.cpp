/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/IntrinsicConcept.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Visitor.hpp>
#include <stdexcept>

namespace Cm { namespace Ast {

IntrinsicConstraintNode::IntrinsicConstraintNode() : ConstraintNode(Cm::Parsing::Span())
{
}

SameConstraintNode::SameConstraintNode() : IntrinsicConstraintNode()
{
}

void SameConstraintNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

Node* SameConstraintNode::Clone(CloneContext& cloneContext) const
{
    return new SameConstraintNode();
}

DerivedConstraintNode::DerivedConstraintNode() : IntrinsicConstraintNode()
{
}

void DerivedConstraintNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

Node* DerivedConstraintNode::Clone(CloneContext& cloneContext) const
{
    return new DerivedConstraintNode();
}

ConvertibleConstraintNode::ConvertibleConstraintNode() : IntrinsicConstraintNode()
{
}

void ConvertibleConstraintNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

Node* ConvertibleConstraintNode::Clone(CloneContext& cloneContext) const
{
    return new ConvertibleConstraintNode();
}

ExplicitlyConvertibleConstraintNode::ExplicitlyConvertibleConstraintNode() : IntrinsicConstraintNode()
{
}

void ExplicitlyConvertibleConstraintNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

Node* ExplicitlyConvertibleConstraintNode::Clone(CloneContext& cloneContext) const
{
    return new ExplicitlyConvertibleConstraintNode();
}

CommonConstraintNode::CommonConstraintNode() : IntrinsicConstraintNode()
{
}

void CommonConstraintNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

Node* CommonConstraintNode::Clone(CloneContext& cloneContext) const
{
    return new CommonConstraintNode();
}

NonReferenceTypeConstraintNode::NonReferenceTypeConstraintNode() : IntrinsicConstraintNode()
{
}

void NonReferenceTypeConstraintNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

Node* NonReferenceTypeConstraintNode::Clone(CloneContext& cloneContext) const
{
    return new NonReferenceTypeConstraintNode();
}

SameConceptNode::SameConceptNode() : ConceptNode(Cm::Parsing::Span(), Specifiers::public_, new IdentifierNode(Cm::Parsing::Span(), "Same"))
{
    AddTypeParameter(new IdentifierNode(Cm::Parsing::Span(), "T"));
    AddTypeParameter(new IdentifierNode(Cm::Parsing::Span(), "U"));
    AddConstraint(new SameConstraintNode());
}

DerivedConceptNode::DerivedConceptNode() : ConceptNode(Cm::Parsing::Span(), Specifiers::public_, new IdentifierNode(Cm::Parsing::Span(), "Derived"))
{
    AddTypeParameter(new IdentifierNode(Cm::Parsing::Span(), "T"));
    AddTypeParameter(new IdentifierNode(Cm::Parsing::Span(), "U"));
    AddConstraint(new DerivedConstraintNode());
}

ConvertibleConceptNode::ConvertibleConceptNode() : ConceptNode(Cm::Parsing::Span(), Specifiers::public_, new IdentifierNode(Cm::Parsing::Span(), "Convertible"))
{
    AddTypeParameter(new IdentifierNode(Cm::Parsing::Span(), "T"));
    AddTypeParameter(new IdentifierNode(Cm::Parsing::Span(), "U"));
    AddConstraint(new ConvertibleConstraintNode());
}

ExplicitlyConvertibleConceptNode::ExplicitlyConvertibleConceptNode() : ConceptNode(Cm::Parsing::Span(), Specifiers::public_, new IdentifierNode(Cm::Parsing::Span(), "ExplicitlyConvertible"))
{
    AddTypeParameter(new IdentifierNode(Cm::Parsing::Span(), "T"));
    AddTypeParameter(new IdentifierNode(Cm::Parsing::Span(), "U"));
    AddConstraint(new ExplicitlyConvertibleConstraintNode());
}

CommonConceptNode::CommonConceptNode() : ConceptNode(Cm::Parsing::Span(), Specifiers::public_, new IdentifierNode(Cm::Parsing::Span(), "Common"))
{
    AddTypeParameter(new IdentifierNode(Cm::Parsing::Span(), "T"));
    AddTypeParameter(new IdentifierNode(Cm::Parsing::Span(), "U"));
    AddConstraint(new CommonConstraintNode());
}

NonReferenceTypeConceptNode::NonReferenceTypeConceptNode() : ConceptNode(Cm::Parsing::Span(), Specifiers::public_, new IdentifierNode(Cm::Parsing::Span(), "NonReferenceType"))
{ 
    AddTypeParameter(new IdentifierNode(Cm::Parsing::Span(), "T"));
    AddConstraint(new NonReferenceTypeConstraintNode());
}

} } // namespace Cm::Ast
