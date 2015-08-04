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
};

class SameConstraintNode : public IntrinsicConstraintNode
{
public:
    SameConstraintNode();
    void Accept(Visitor& visitor) override;
    Node* Clone(CloneContext& cloneContext) const override;
};

class DerivedConstraintNode : public IntrinsicConstraintNode
{
public:
    DerivedConstraintNode();
    void Accept(Visitor& visitor) override;
    Node* Clone(CloneContext& cloneContext) const override;
};

class ConvertibleConstraintNode : public IntrinsicConstraintNode
{
public:
    ConvertibleConstraintNode();
    void Accept(Visitor& visitor) override;
    Node* Clone(CloneContext& cloneContext) const override;
};

class ExplicitlyConvertibleConstraintNode : public IntrinsicConstraintNode
{
public:
    ExplicitlyConvertibleConstraintNode();
    void Accept(Visitor& visitor) override;
    Node* Clone(CloneContext& cloneContext) const override;
};

class CommonConstraintNode : public IntrinsicConstraintNode
{
public:
    CommonConstraintNode();
    void Accept(Visitor& visitor) override;
    Node* Clone(CloneContext& cloneContext) const override;
};

class SameConceptNode : public ConceptNode
{
public:
    SameConceptNode();
    bool IsIntrinsicConceptNode() const { return true; }
};

class DerivedConceptNode : public ConceptNode
{
public:
    DerivedConceptNode();
    bool IsIntrinsicConceptNode() const { return true; }
};

class ConvertibleConceptNode : public ConceptNode
{
public:
    ConvertibleConceptNode();
    bool IsIntrinsicConceptNode() const { return true; }
};

class ExplicitlyConvertibleConceptNode : public ConceptNode
{
public:
    ExplicitlyConvertibleConceptNode();
    bool IsIntrinsicConceptNode() const { return true; }
};

class CommonConceptNode : public ConceptNode
{
public:
    CommonConceptNode();
    bool IsCommonConceptNode() const override { return true; }
    bool IsIntrinsicConceptNode() const { return true; }
};

} } // namespace Cm::Ast

#endif // CM_AST_INTRINSIC_CONCEPT_INCLUDED
