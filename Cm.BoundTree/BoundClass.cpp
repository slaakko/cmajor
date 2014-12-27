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

void BoundClass::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace Cm::BoundTree
