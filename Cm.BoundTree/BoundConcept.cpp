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

bool BoundAtomicConstraint::Imply(BoundConstraint* that) const
{
    if (that->IsBinaryConstraint())
    {
        BoundBinaryConstraint* thatBinaryConstraint = static_cast<BoundBinaryConstraint*>(that);
        BoundConstraint* thatLeft = thatBinaryConstraint->Left();
        BoundConstraint* thatRight = thatBinaryConstraint->Right();
        bool implyLeft = Imply(thatLeft);
        bool implyRight = Imply(thatRight);
        if (that->IsConjunctiveConstraint())
        {
            return implyLeft && implyRight;
        }
        else if (that->IsDisjunctiveConstraint())
        {
            return implyLeft || implyRight;
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
            return false;
        }
        else
        {
            if (concept && !thatAtomic->concept)
            {
                return false;
            }
            else if (!concept && thatAtomic->concept)
            {
                return true;
            }
            else if (concept && thatAtomic->concept)
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
            return true;
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

bool BoundDisjunctiveConstraint::Imply(BoundConstraint* that) const
{
    BoundConstraint* left = Left();
    BoundConstraint* right = Right();
    if (that->IsBinaryConstraint())
    {
        BoundBinaryConstraint* thatBinaryConstraint = static_cast<BoundBinaryConstraint*>(that);
        BoundConstraint* thatLeft = thatBinaryConstraint->Left();
        BoundConstraint* thatRight = thatBinaryConstraint->Right();
        bool leftImplyThatLeft = left->Imply(thatLeft);
        bool leftImplyThatRight = left->Imply(thatRight);
        bool rightImplyThatLeft = right->Imply(thatLeft);
        bool rightImplyThatRight = right->Imply(thatRight);
        bool leftImplyThatLeftOrThatRight = leftImplyThatLeft || leftImplyThatRight;
        bool rightImplyThatLeftOrThatRight = rightImplyThatLeft || rightImplyThatRight;
        if (that->IsConjunctiveConstraint())
        {
            return leftImplyThatLeftOrThatRight && rightImplyThatLeftOrThatRight;
        }
        else if (that->IsDisjunctiveConstraint())
        {
            return leftImplyThatLeftOrThatRight || rightImplyThatLeftOrThatRight;
        }
        else // assert(false)
        {
            return false;
        }
    }
    else
    {
        bool leftImplyThat = left->Imply(that);
        bool rightImplyThat = right->Imply(that);
        return leftImplyThat && rightImplyThat;
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

bool BoundConjunctiveConstraint::Imply(BoundConstraint* that) const
{
    BoundConstraint* left = Left();
    BoundConstraint* right = Right();
    if (that->IsBinaryConstraint())
    {
        BoundBinaryConstraint* thatBinaryConstraint = static_cast<BoundBinaryConstraint*>(that);
        BoundConstraint* thatLeft = thatBinaryConstraint->Left();
        BoundConstraint* thatRight = thatBinaryConstraint->Right();
        bool leftImplyThatLeft = left->Imply(thatLeft);
        bool rightImplyThatLeft = right->Imply(thatLeft);
        bool leftImplyThatRight = left->Imply(thatRight);
        bool rightImplyThatRight = right->Imply(thatRight);
        bool leftOrRightImplyThatLeft = leftImplyThatLeft || rightImplyThatLeft;
        bool leftOrRightImplyThatRight = leftImplyThatRight || rightImplyThatRight;
        if (that->IsConjunctiveConstraint())
        {
            return leftOrRightImplyThatLeft && leftOrRightImplyThatRight;
        }
        else if (that->IsDisjunctiveConstraint())
        {
            return leftOrRightImplyThatLeft || leftOrRightImplyThatRight;
        }
        else // assert(false)
        {
            return false;
        }
    }
    else
    {
        bool leftImplyThat = left->Imply(that);
        bool righImplyThat = right->Imply(that);
        return leftImplyThat || righImplyThat;
    }
}

void BoundConjunctiveConstraint::Accept(Visitor& visitor)
{
    Left()->Accept(visitor);
    Right()->Accept(visitor);
    visitor.Visit(*this);
}

} } // namespace Cm::BoundTree
