/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_INTRINSIC_CONCEPT_INCLUDED
#define CM_AST_INTRINSIC_CONCEPT_INCLUDED
#include <Cm.Ast/Concept.hpp>

namespace Cm { namespace Ast {

class IntrinsicConstraintNode : public ConstraintNode
{
public:
    IntrinsicConstraintNode();
    NodeType GetNodeType() const override { return NodeType::intrinsicConstraintNode; }
    Node* Clone() const override;
};

class SameConstraintNode : public IntrinsicConstraintNode
{
public:
    SameConstraintNode();
    void Accept(Visitor& visitor) override;
};

class DerivedConstraintNode : public IntrinsicConstraintNode
{
public:
    DerivedConstraintNode();
    void Accept(Visitor& visitor) override;
};

class ConvertibleConstraintNode : public IntrinsicConstraintNode
{
public:
    ConvertibleConstraintNode();
    void Accept(Visitor& visitor) override;
};

class ExplicitlyConvertibleConstraintNode : public IntrinsicConstraintNode
{
public:
    ExplicitlyConvertibleConstraintNode();
    void Accept(Visitor& visitor) override;
};

class SameConceptNode : public ConceptNode
{
public:
    SameConceptNode();
};

class DerivedConceptNode : public ConceptNode
{
public:
    DerivedConceptNode();
};

class ConvertibleConceptNode : public ConceptNode
{
public:
    ConvertibleConceptNode();
};

class ExplicitlyConvertibleConceptNode : public ConceptNode
{
public:
    ExplicitlyConvertibleConceptNode();
};

} } // namespace Cm::Ast

#endif // CM_AST_INTRINSIC_CONCEPT_INCLUDED
