/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
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
    BoundConstraint();
    virtual bool Imply(BoundConstraint* that) const = 0;
    virtual bool IsAtomicConstraint() const { return false; }
    virtual bool IsBinaryConstraint() const { return false; }
    virtual bool IsConjunctiveConstraint() const { return false; }
    virtual bool IsDisjunctiveConstraint() const { return false; }
    virtual BoundConstraint* Clone() const = 0;
};

class BoundAtomicConstraint : public BoundConstraint
{
public:
    BoundAtomicConstraint(bool satisfied_);
    bool IsAtomicConstraint() const override { return true; }
    bool Imply(BoundConstraint* that) const override;
    void Accept(Visitor& visitor) override;
    bool Satisfied() const { return satisfied; }
    BoundConstraint* Clone() const override { return new BoundAtomicConstraint(*this); }
    void SetConcept(Cm::Sym::ConceptSymbol* concept_) { concept = concept_; }
private:
    bool satisfied;
    Cm::Sym::ConceptSymbol* concept;
};

class BoundBinaryConstraint : public BoundConstraint
{
public:
    BoundBinaryConstraint(BoundConstraint* left_, BoundConstraint* right_);
    BoundBinaryConstraint(const BoundBinaryConstraint& that);
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
    BoundDisjunctiveConstraint(BoundConstraint* left_, BoundConstraint* right_);
    BoundDisjunctiveConstraint(const BoundDisjunctiveConstraint& that);
    bool Imply(BoundConstraint* that) const override;
    bool IsDisjunctiveConstraint() const override { return true; }
    void Accept(Visitor& visitor) override;
    BoundConstraint* Clone() const override { return new BoundDisjunctiveConstraint(*this); }
};

class BoundConjunctiveConstraint : public BoundBinaryConstraint
{
public:
    BoundConjunctiveConstraint(BoundConstraint* left_, BoundConstraint* right_);
    BoundConjunctiveConstraint(const BoundConjunctiveConstraint& that);
    bool Imply(BoundConstraint* that) const override;
    bool IsConjunctiveConstraint() const override { return true; }
    void Accept(Visitor& visitor) override;
    BoundConstraint* Clone() const override { return new BoundConjunctiveConstraint(*this); }
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_BOUND_CONCEPT_INCLUDED
