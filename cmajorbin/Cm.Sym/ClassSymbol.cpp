/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ClassSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

ClassSymbol::ClassSymbol(Cm::Ast::ClassNode* classNode) : ContainerSymbol(classNode->Id()->Str())
{
    SetNode(classNode);
}

} } // namespace Cm::Sym
