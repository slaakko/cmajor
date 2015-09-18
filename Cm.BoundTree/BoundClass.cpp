/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/BoundClass.hpp>
#include <Cm.BoundTree/Visitor.hpp>

namespace Cm { namespace BoundTree {

BoundClass::BoundClass(Cm::Sym::ClassTypeSymbol* classTypeSymbol_, Cm::Ast::ClassNode* classNode_) : BoundNode(classNode_), classTypeSymbol(classTypeSymbol_)
{
}

void BoundClass::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(classTypeSymbol);
    writer.GetBinaryWriter().Write(int(members.size()));
    for (const std::unique_ptr<BoundNode>& member : members)
    {
        writer.Write(member.get());
    }
}

void BoundClass::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    for (const std::unique_ptr<BoundNode>& member : members)
    {
        member->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

void BoundClass::AddBoundNode(BoundNode* member)
{
    members.push_back(std::unique_ptr<BoundNode>(member));
}

} } // namespace Cm::BoundTree
