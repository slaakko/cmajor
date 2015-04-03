/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/TypeExpr.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Visitor.hpp>
#include <algorithm>
#include <stdexcept>

namespace Cm { namespace Ast {

std::string DerivationStr(Derivation d)
{
    switch (d)
    {
        case Derivation::const_: return "const";
        case Derivation::reference: return "&";
        case Derivation::rvalueRef: return "&&";
        case Derivation::pointer: return "*";
        case Derivation::leftParen: return "(";
        case Derivation::rightParen: return ")";
    }
    return "";
}

DerivationList::DerivationList() : numDerivations(0)
{
    memset(&derivations[0], 0, sizeof(derivations));
}

void DerivationList::Add(Derivation derivation)
{
    if (numDerivations >= maxDerivations)
    {
        throw std::runtime_error("too many derivations");
    }
    derivations[numDerivations++] = derivation;
}

void DerivationList::InsertFront(Derivation derivation)
{
    if (numDerivations >= maxDerivations)
    {
        throw std::runtime_error("too many derivations");
    }
    std::copy_backward(&derivations[0], &derivations[numDerivations], &derivations[numDerivations + 1]);
    derivations[0] = derivation;
    ++numDerivations;
}

void DerivationList::RemoveLastPointer()
{
    for (uint8_t i = numDerivations - 1; i >= 0; --i)
    {
        if (derivations[i] == Derivation::pointer)
        {
            derivations[i] = Derivation::none;
            --numDerivations;
            return;
        }
    }
}

struct IsReference
{
    bool operator()(Derivation derivation) const
    {
        return derivation == Derivation::reference;
    }
};

void DerivationList::RemoveReference()
{
    Derivation* new_end = std::remove_if(&derivations[0], &derivations[numDerivations], IsReference());
    std::fill(new_end, &derivations[numDerivations], Derivation());
    numDerivations -= uint8_t(&derivations[numDerivations] - new_end);
}

bool operator==(const DerivationList& left, const DerivationList& right)
{
    uint8_t n = left.NumDerivations();
    if (n != right.NumDerivations()) return false;
    for (uint8_t i = 0; i < n; ++i)
    {
        if (left[i] != right[i]) return false;
    }
    return true;
}

bool operator<(const DerivationList& left, const DerivationList& right)
{
    uint8_t n = left.NumDerivations();
    uint8_t m = right.NumDerivations();
    if (n < m)
    {
        return true;
    }
    else if (n > m)
    {
        return false;
    }
    else
    {
        for (uint8_t i = 0; i < n; ++i)
        {
            if (left[i] < right[i])
            {
                return true;
            }
            else if (left[i] > right[i])
            {
                return false;
            }
        }
    }
    return false;
}

DerivedTypeExprNode::DerivedTypeExprNode(const Span& span_): Node(span_)
{
}

DerivedTypeExprNode::DerivedTypeExprNode(const Span& span_, const DerivationList& derivations_, Node* baseTypeExprNode_): Node(span_), derivations(derivations_), baseTypeExprNode(baseTypeExprNode_)
{
    baseTypeExprNode->SetParent(this);
}

Node* DerivedTypeExprNode::Clone(CloneContext& cloneContext) const
{
    return new DerivedTypeExprNode(GetSpan(), derivations, baseTypeExprNode->Clone(cloneContext));
}

void DerivedTypeExprNode::Read(Reader& reader)
{
    derivations = reader.ReadDerivationList();
    baseTypeExprNode.reset(reader.ReadNode());
    baseTypeExprNode->SetParent(this);
}

void DerivedTypeExprNode::Write(Writer& writer)
{
    writer.Write(derivations);
    writer.Write(baseTypeExprNode.get());
}

void DerivedTypeExprNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string MakeDerivedTypeName(const DerivationList& derivations, const std::string& baseTypeFullName)
{
    std::string s;
    uint8_t derivationIndex = 0;
    uint8_t n = derivations.NumDerivations();
    if (derivationIndex < n)
    {
        Derivation d = derivations[derivationIndex];
        if (d == Derivation::const_)
        {
            s.append(DerivationStr(d));
            ++derivationIndex;
        }
    }
    if (derivationIndex < n)
    {
        Derivation d = derivations[derivationIndex];
        if (d == Derivation::leftParen)
        {
            if (!s.empty())
            {
                s.append(1, ' ');
            }
            s.append(DerivationStr(d));
            ++derivationIndex;
            if (derivationIndex < n)
            {
                Derivation d = derivations[derivationIndex];
                if (d == Derivation::const_)
                {
                    s.append(DerivationStr(d));
                    ++derivationIndex;
                }
            }
        }
    }
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    s.append(baseTypeFullName);
    for (uint8_t i = derivationIndex; i < n; ++i)
    {
        s.append(DerivationStr(derivations[i]));
    }
    return s;
}

std::string DerivedTypeExprNode::ToString() const
{
    return MakeDerivedTypeName(derivations, baseTypeExprNode->ToString());
}

void DerivedTypeExprNode::Add(Derivation derivation)
{
    derivations.Add(derivation);
}

void DerivedTypeExprNode::SetBaseTypeExpr(Node* baseTypeExprNode_)
{
    baseTypeExprNode.reset(baseTypeExprNode_);
    baseTypeExprNode->SetParent(this);
}

Node* MakeTypeExprNode(DerivedTypeExprNode* derivedTypeExprNode)
{
    if (derivedTypeExprNode->Derivations().NumDerivations() == 0)
    {
        Node* baseTypeExprNode = derivedTypeExprNode->ReleaseBaseTypeExprNode();
        delete derivedTypeExprNode;
        return baseTypeExprNode;
    }
    return derivedTypeExprNode;
}

} } // namespace Cm::Ast