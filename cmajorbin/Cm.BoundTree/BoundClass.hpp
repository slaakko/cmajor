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
    void Accept(Visitor& visitor) override;
private:
    Cm::Sym::ClassTypeSymbol* classTypeSymbol;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_BOUND_CLASS_INCLUDED
