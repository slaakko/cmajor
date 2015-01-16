/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_DELEGATE_INCLUDED
#define CM_BIND_DELEGATE_INCLUDED
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Ast/Delegate.hpp>

namespace Cm { namespace Bind {

void BindDelegate(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::DelegateNode* delegateNode);
void BindClassDelegate(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::ClassDelegateNode* classDelegateNode);

} } // namespace Cm::Bind

#endif // CM_BIND_DELEGATE_INCLUDED
