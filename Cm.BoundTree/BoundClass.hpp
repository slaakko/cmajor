/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BOUND_TREE_BOUND_CLASS_INCLUDED
#define CM_BOUND_TREE_BOUND_CLASS_INCLUDED
#include <Cm.BoundTree/BoundNode.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>

namespace Cm { namespace BoundTree {

class BoundClass : public BoundNode
{
public:
    BoundClass(Cm::Sym::ClassTypeSymbol* classTypeSymbol_, Cm::Ast::ClassNode* classNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuClass; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Accept(Visitor& visitor) override;
    void AddBoundNode(BoundNode* member);
    Cm::Sym::ClassTypeSymbol* Symbol() const { return classTypeSymbol; }
private:
    Cm::Sym::ClassTypeSymbol* classTypeSymbol;
    std::vector<std::unique_ptr<BoundNode>> members;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_BOUND_CLASS_INCLUDED
