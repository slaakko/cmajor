/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BOUND_TREE_BOUND_CONCEPT_INCLUDED
#define CM_BOUND_TREE_BOUND_CONCEPT_INCLUDED
#include <Cm.BoundTree/BoundNode.hpp>
#include <Cm.Sym/ConceptSymbol.hpp>

namespace Cm { namespace BoundTree {

class BoundConcept;

class BoundConstraint : public BoundNode
{
public:
    BoundConstraint(Cm::Ast::Node* syntaxNode_);
    virtual bool Imply(BoundConstraint* that) const = 0;
    void Accept(Visitor& visitor) override;
    virtual bool IsBinaryConstraint() const { return false; }
    virtual bool IsConjunctiveConstraint() const { return false; }
    virtual bool IsDisjunctiveConstraint() const { return false; }
    virtual bool IsBoundTypeSatisfyConceptConstraint() const { return false; }
    virtual bool IsBoundTypeIsTypeConstraint() const { return false; }
    virtual bool IsBoundMultiParamConstraint() const { return false; }
    virtual bool IsBoundConcept() const { return false; }
};

class BoundAtomicConstraint : public BoundConstraint
{
public:
    BoundAtomicConstraint(Cm::Ast::Node* syntaxNode_);
    bool Imply(BoundConstraint* that) const override { return true; }
};

class BoundBinaryConstraint : public BoundConstraint
{
public:
    BoundBinaryConstraint(Cm::Ast::Node* syntaxNode_, BoundConstraint* left_, BoundConstraint* right_);
    BoundConstraint* Left() const { return left.get(); }
    BoundConstraint* Right() const { return right.get(); }
    bool IsBinaryConstraint() const override { return true; }
private:
    std::unique_ptr<BoundConstraint> left;
    std::unique_ptr<BoundConstraint> right;
};

class BoundDisjunctiveConstraint : public BoundBinaryConstraint
{
public:
    BoundDisjunctiveConstraint(Cm::Ast::Node* syntaxNode_, BoundConstraint* left_, BoundConstraint* right_);
    bool Imply(BoundConstraint* that) const override;
    bool IsDisjunctiveConstraint() const override { return true; }
};

class BoundConjunctiveConstraint : public BoundBinaryConstraint
{
public:
    BoundConjunctiveConstraint(Cm::Ast::Node* syntaxNode_, BoundConstraint* left_, BoundConstraint* right_);
    bool Imply(BoundConstraint* that) const override;
    bool IsConjunctiveConstraint() const override { return true; }
};

class BoundTypeSatisfyConceptConstraint : public BoundConstraint
{
public:
    BoundTypeSatisfyConceptConstraint(Cm::Ast::Node* syntaxNode_, Cm::Sym::TypeSymbol* type_, BoundConcept* concept_);
    bool Imply(BoundConstraint* that) const override;
    bool IsBoundTypeSatisfyConceptConstraint() const override { return true; }
private:
    Cm::Sym::TypeSymbol* type;
    std::unique_ptr<BoundConcept> concept;
};

class BoundTypeIsTypeConstraint : public BoundConstraint
{
public:
    BoundTypeIsTypeConstraint(Cm::Ast::Node* syntaxNode_, Cm::Sym::TypeSymbol* left_, Cm::Sym::TypeSymbol* right_);
    bool Imply(BoundConstraint* that) const override;
    bool IsBoundTypeIsTypeConstraint() const override { return true; }
private:
    Cm::Sym::TypeSymbol* left;
    Cm::Sym::TypeSymbol* right;
};

class BoundMultiParamConstraint : public BoundConstraint
{
public:
    BoundMultiParamConstraint(Cm::Ast::Node* syntaxNode_, const std::vector<Cm::Sym::TypeSymbol*>& types_, Cm::BoundTree::BoundConcept* concept_);
    bool Imply(BoundConstraint* that) const override;
    bool IsBoundMultiParamConstraint() const override { return true; }
private:
    std::vector<Cm::Sym::TypeSymbol*> types;
    std::unique_ptr<BoundConcept> concept;
};

class BoundConcept : public BoundConstraint
{
public:
    BoundConcept(Cm::Ast::Node* syntaxNode_, Cm::Sym::ConceptSymbol* concept_);
    bool Imply(BoundConstraint* that) const override;
    bool IsBoundConcept() const override { return true; }
private:
    Cm::Sym::ConceptSymbol* conceptSymbol;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_BOUND_CONCEPT_INCLUDED
