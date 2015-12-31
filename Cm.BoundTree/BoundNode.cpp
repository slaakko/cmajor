/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/BoundNode.hpp>

namespace Cm { namespace BoundTree {

BoundNode::BoundNode() : flags(BoundNodeFlags::none), syntaxNode(nullptr)
{
}

BoundNode::BoundNode(Cm::Ast::Node* syntaxNode_) : flags(BoundNodeFlags::none), syntaxNode(syntaxNode_)
{
}

void BoundNode::Write(Cm::Sym::BcuWriter& writer)
{
    writer.GetBinaryWriter().Write(uint16_t(flags));
}

void BoundNode::Read(Cm::Sym::BcuReader& reader)
{
    flags = BoundNodeFlags(reader.GetBinaryReader().ReadUShort());
}

} } // namespace Cm::BoundTree
