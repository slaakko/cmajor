/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/TypeExpr.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>

namespace Cm { namespace Ast {

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

DerivedTypeExprNode::DerivedTypeExprNode(const Span& span_): Node(span_)
{
}

DerivedTypeExprNode::DerivedTypeExprNode(const Span& span_, const DerivationList& derivations_, Node* baseTypeExprNode_): Node(span_), derivations(derivations_), baseTypeExprNode(baseTypeExprNode_)
{
}

Node* DerivedTypeExprNode::Clone() const
{
    return new DerivedTypeExprNode(GetSpan(), derivations, baseTypeExprNode->Clone());
}

void DerivedTypeExprNode::Read(Reader& reader)
{
    derivations = reader.ReadDerivationList();
    baseTypeExprNode.reset(reader.ReadNode());
}

void DerivedTypeExprNode::Write(Writer& writer)
{
    writer.Write(derivations);
    writer.Write(baseTypeExprNode.get());
}

void DerivedTypeExprNode::Add(Derivation derivation)
{
    derivations.Add(derivation);
}

void DerivedTypeExprNode::SetBaseTypeExpr(Node* baseTypeExprNode_)
{
    baseTypeExprNode.reset(baseTypeExprNode_);
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
