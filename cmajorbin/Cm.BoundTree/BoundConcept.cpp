/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/BoundConcept.hpp>

namespace Cm { namespace BoundTree {

BoundConstraint::BoundConstraint(Cm::Ast::Node* syntaxNode_) : BoundNode(syntaxNode_)
{
}

void BoundConstraint::Accept(Visitor& visitor)
{
    throw std::runtime_error("member function not applicable");
}

BoundAtomicConstraint::BoundAtomicConstraint(Cm::Ast::Node* syntaxNode_) : BoundConstraint(syntaxNode_)
{
}

BoundBinaryConstraint::BoundBinaryConstraint(Cm::Ast::Node* syntaxNode_, BoundConstraint* left_, BoundConstraint* right_) : BoundConstraint(syntaxNode_), left(left_), right(right_)
{
}

BoundDisjunctiveConstraint::BoundDisjunctiveConstraint(Cm::Ast::Node* syntaxNode_, BoundConstraint* left_, BoundConstraint* right_) : BoundBinaryConstraint(syntaxNode_, left_, right_)
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

BoundConjunctiveConstraint::BoundConjunctiveConstraint(Cm::Ast::Node* syntaxNode_, BoundConstraint* left_, BoundConstraint* right_) : BoundBinaryConstraint(syntaxNode_, left_, right_)
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

BoundTypeSatisfyConceptConstraint::BoundTypeSatisfyConceptConstraint(Cm::Ast::Node* syntaxNode_, Cm::Sym::TypeSymbol* type_, BoundConcept* concept_) :
    BoundConstraint(syntaxNode_), type(type_), concept(concept_)
{
}

bool BoundTypeSatisfyConceptConstraint::Imply(BoundConstraint* that) const
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
    else if (!that->IsBoundTypeSatisfyConceptConstraint())
    {
        return false;
    }
    else
    {
        BoundTypeSatisfyConceptConstraint* thatTypeSatisfyConceptConstraint = static_cast<BoundTypeSatisfyConceptConstraint*>(that);
        if (!Cm::Sym::TypesEqual(type, thatTypeSatisfyConceptConstraint->type))
        {
            return false;
        }
        return concept->Imply(thatTypeSatisfyConceptConstraint->concept.get());
    }
}

BoundTypeIsTypeConstraint::BoundTypeIsTypeConstraint(Cm::Ast::Node* syntaxNode_, Cm::Sym::TypeSymbol* left_, Cm::Sym::TypeSymbol* right_) : BoundConstraint(syntaxNode_), left(left_), right(right_)
{
}

bool BoundTypeIsTypeConstraint::Imply(BoundConstraint* that) const
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
    else if (!that->IsBoundTypeIsTypeConstraint())
    {
        return false;
    }
    else
    {
        BoundTypeIsTypeConstraint* thatTypeIsTypeConstraint = static_cast<BoundTypeIsTypeConstraint*>(that);
        if (!Cm::Sym::TypesEqual(left, thatTypeIsTypeConstraint->left))
        {
            return false;
        }
        if (!Cm::Sym::TypesEqual(right, thatTypeIsTypeConstraint->right))
        {
            return false;
        }
        return true;
    }
}

BoundMultiParamConstraint::BoundMultiParamConstraint(Cm::Ast::Node* syntaxNode_, const std::vector<Cm::Sym::TypeSymbol*>& types_, Cm::BoundTree::BoundConcept* concept_) :
    BoundConstraint(syntaxNode_), types(types_), concept(concept_)
{
}

bool BoundMultiParamConstraint::Imply(BoundConstraint* that) const
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
    else if (!that->IsBoundMultiParamConstraint())
    {
        return false;
    }
    else
    {
        BoundMultiParamConstraint* thatMultiParamConstraint = static_cast<BoundMultiParamConstraint*>(that);
        int n = int(types.size());
        if (n != int(thatMultiParamConstraint->types.size())) return false;
        for (int i = 0; i < n; ++i)
        {
            Cm::Sym::TypeSymbol* left = types[i];
            Cm::Sym::TypeSymbol* right = thatMultiParamConstraint->types[i];
            if (!Cm::Sym::TypesEqual(left, right)) return false;
        }
        return (concept->Imply(thatMultiParamConstraint->concept.get()));
    }
}

BoundConcept::BoundConcept(Cm::Ast::Node* syntaxNode_, Cm::Sym::ConceptSymbol* concept_) :
    BoundConstraint(syntaxNode_), conceptSymbol(concept_)
{
}

bool BoundConcept::Imply(BoundConstraint* that) const
{
    if (!that->IsBoundConcept()) return false;
    BoundConcept* thatConcept = static_cast<BoundConcept*>(that);
    if (conceptSymbol == thatConcept->conceptSymbol) return true;
    Cm::Sym::ConceptSymbol* refined = conceptSymbol->RefinedConcept();
    while (refined)
    {
        if (refined == thatConcept->conceptSymbol)
        {
            return true;
        }
    }
    return false;
}

} } // namespace Cm::BoundTree
