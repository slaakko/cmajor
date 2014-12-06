/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_DELEGATE_SYMBOL_INCLUDED
#define CM_SYM_DELEGATE_SYMBOL_INCLUDED
#include <Cm.Sym/ContainerSymbol.hpp>
#include <Cm.Ast/Delegate.hpp>

namespace Cm { namespace Sym {

class DelegateSymbol : public ContainerSymbol
{
public:
    DelegateSymbol(Cm::Ast::DelegateNode* delegateNode);
};

class ClassDelegateSymbol : public ContainerSymbol
{
public:
    ClassDelegateSymbol(Cm::Ast::ClassDelegateNode* classDelegateNode);
};

} } // namespace Cm::Sym

#endif // CM_SYM_DELEGATE_SYMBOL_INCLUDED
