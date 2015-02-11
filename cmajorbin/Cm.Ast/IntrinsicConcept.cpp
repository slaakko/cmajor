/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/IntrinsicConcept.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Ast {

IntrinsicConstraintNode::IntrinsicConstraintNode() : ConstraintNode(Cm::Parsing::Span())
{
}

Node* IntrinsicConstraintNode::Clone() const
{
    throw std::runtime_error("member function not applicable");
}

SameConstraintNode::SameConstraintNode() : IntrinsicConstraintNode()
{
}

void SameConstraintNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

DerivedConstraintNode::DerivedConstraintNode() : IntrinsicConstraintNode()
{
}

void DerivedConstraintNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ConvertibleConstraintNode::ConvertibleConstraintNode() : IntrinsicConstraintNode()
{
}

void ConvertibleConstraintNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ExplicitlyConvertibleConstraintNode::ExplicitlyConvertibleConstraintNode() : IntrinsicConstraintNode()
{
}

void ExplicitlyConvertibleConstraintNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
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

} } // namespace Cm::Ast
