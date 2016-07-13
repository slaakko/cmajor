/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/BoundConcept.hpp>
#include <Cm.BoundTree/Visitor.hpp>
#include <stdexcept>

namespace Cm { namespace BoundTree {

BoundConstraint::BoundConstraint() : BoundNode(nullptr)
{
}

BoundAtomicConstraint::BoundAtomicConstraint(bool satisfied_) : BoundConstraint(), satisfied(satisfied_), concept(nullptr)
{
}

bool BoundAtomicConstraint::Subsume(BoundConstraint* that) const
{
    if (that->IsBinaryConstraint())
    {
        BoundBinaryConstraint* thatBinaryConstraint = static_cast<BoundBinaryConstraint*>(that);
        BoundConstraint* thatLeft = thatBinaryConstraint->Left();
        BoundConstraint* thatRight = thatBinaryConstraint->Right();
        bool subsumeLeft = Subsume(thatLeft);
        bool subsumeRight = Subsume(thatRight);
        if (that->IsConjunctiveConstraint())
        {
            return subsumeLeft && subsumeRight;
        }
        else if (that->IsDisjunctiveConstraint())
        {
            return subsumeLeft || subsumeRight;
        }
        else // assert(false)
        {
            return false;
        }
    }
    else if (that->IsAtomicConstraint())
    {
        BoundAtomicConstraint* thatAtomic = static_cast<BoundAtomicConstraint*>(that);
        if (satisfied && !thatAtomic->Satisfied())
        {
            return true;
        }
        else if (!satisfied && thatAtomic->Satisfied())
        {
            return false;
        }
        else
        {
            if (concept && !thatAtomic->concept)
            {
                return true;
            }
            else if (!concept && thatAtomic->concept)
            {
                return false;
            }
            else if (!concept && !thatAtomic->concept)
            {
                return true;
            }
            else 
            {
                if (concept == thatAtomic->concept)
                {
                    return true;
                }
                Cm::Sym::ConceptSymbol* refinedConcept = concept->RefinedConcept();
                while (refinedConcept)
                {
                    if (refinedConcept == thatAtomic->concept)
                    {
                        return true;
                    }
                    else
                    {
                        refinedConcept = refinedConcept->RefinedConcept();
                    }
                }
                return false;
            }
        }
    }
    else
    {
        return false;
    }
}

void BoundAtomicConstraint::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundBinaryConstraint::BoundBinaryConstraint(BoundConstraint* left_, BoundConstraint* right_) : BoundConstraint(), left(left_), right(right_)
{
}

BoundBinaryConstraint::BoundBinaryConstraint(const BoundBinaryConstraint& that): BoundConstraint(that), left(that.left->Clone()), right(that.right->Clone())
{
}

BoundDisjunctiveConstraint::BoundDisjunctiveConstraint(BoundConstraint* left_, BoundConstraint* right_) : BoundBinaryConstraint(left_, right_)
{
}

BoundDisjunctiveConstraint::BoundDisjunctiveConstraint(const BoundDisjunctiveConstraint& that) : BoundBinaryConstraint(that)
{
}

bool BoundDisjunctiveConstraint::Subsume(BoundConstraint* that) const
{
    BoundConstraint* left = Left();
    BoundConstraint* right = Right();
    if (that->IsBinaryConstraint())
    {
        BoundBinaryConstraint* thatBinaryConstraint = static_cast<BoundBinaryConstraint*>(that);
        BoundConstraint* thatLeft = thatBinaryConstraint->Left();
        BoundConstraint* thatRight = thatBinaryConstraint->Right();
        bool leftSubsumeThatLeft = left->Subsume(thatLeft);
        bool leftSubsumeThatRight = left->Subsume(thatRight);
        bool rightSubsumeThatLeft = right->Subsume(thatLeft);
        bool rightSubsumeThatRight = right->Subsume(thatRight);
        bool leftSubsumeThatLeftOrThatRight = leftSubsumeThatLeft || leftSubsumeThatRight;
        bool rightSubsumeThatLeftOrThatRight = rightSubsumeThatLeft || rightSubsumeThatRight;
        if (that->IsConjunctiveConstraint())
        {
            return leftSubsumeThatLeftOrThatRight && rightSubsumeThatLeftOrThatRight;
        }
        else if (that->IsDisjunctiveConstraint())
        {
            return leftSubsumeThatLeftOrThatRight || rightSubsumeThatLeftOrThatRight;
        }
        else // assert(false)
        {
            return false;
        }
    }
    else
    {
        bool leftSubsumeThat = left->Subsume(that);
        bool rightSubsumeThat = right->Subsume(that);
        return leftSubsumeThat && rightSubsumeThat;
    }
}

void BoundDisjunctiveConstraint::Accept(Visitor& visitor)
{
    Left()->Accept(visitor);
    Right()->Accept(visitor);
    visitor.Visit(*this);
}

BoundConjunctiveConstraint::BoundConjunctiveConstraint(BoundConstraint* left_, BoundConstraint* right_) : BoundBinaryConstraint(left_, right_)
{
}

BoundConjunctiveConstraint::BoundConjunctiveConstraint(const BoundConjunctiveConstraint& that) : BoundBinaryConstraint(that)
{
}

bool BoundConjunctiveConstraint::Subsume(BoundConstraint* that) const
{
    BoundConstraint* left = Left();
    BoundConstraint* right = Right();
    if (that->IsBinaryConstraint())
    {
        BoundBinaryConstraint* thatBinaryConstraint = static_cast<BoundBinaryConstraint*>(that);
        BoundConstraint* thatLeft = thatBinaryConstraint->Left();
        BoundConstraint* thatRight = thatBinaryConstraint->Right();
        bool leftSubsumeThatLeft = left->Subsume(thatLeft);
        bool rightSubsumeThatLeft = right->Subsume(thatLeft);
        bool leftSubsumeThatRight = left->Subsume(thatRight);
        bool rightSubsumeThatRight = right->Subsume(thatRight);
        bool leftOrRightSubsumeThatLeft = leftSubsumeThatLeft || rightSubsumeThatLeft;
        bool leftOrRightSubsumeThatRight = leftSubsumeThatRight || rightSubsumeThatRight;
        if (that->IsConjunctiveConstraint())
        {
            return leftOrRightSubsumeThatLeft && leftOrRightSubsumeThatRight;
        }
        else if (that->IsDisjunctiveConstraint())
        {
            return leftOrRightSubsumeThatLeft || leftOrRightSubsumeThatRight;
        }
        else // assert(false)
        {
            return false;
        }
    }
    else
    {
        bool leftSubsumeThat = left->Subsume(that);
        bool righSubsumeThat = right->Subsume(that);
        return leftSubsumeThat || righSubsumeThat;
    }
}

void BoundConjunctiveConstraint::Accept(Visitor& visitor)
{
    Left()->Accept(visitor);
    Right()->Accept(visitor);
    visitor.Visit(*this);
}

} } // namespace Cm::BoundTree
