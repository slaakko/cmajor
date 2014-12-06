/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/DelegateSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

DelegateSymbol::DelegateSymbol(Cm::Ast::DelegateNode* delegateNode) : ContainerSymbol(delegateNode->Id()->Str())
{
    SetNode(delegateNode);
}

ClassDelegateSymbol::ClassDelegateSymbol(Cm::Ast::ClassDelegateNode* classDelegateNode) : ContainerSymbol(classDelegateNode->Id()->Str())
{
    SetNode(classDelegateNode);
}

} } // namespace Cm::Sym