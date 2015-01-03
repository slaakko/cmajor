/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Delegate.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Bind/Exception.hpp>
#include <Cm.Sym/DelegateSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Bind {

void BindDelegate(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::DelegateNode* delegateNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(delegateNode->Id()->Str());
    if (symbol->IsDelegateTypeSymbol())
    {
        Cm::Sym::DelegateTypeSymbol* delegateTypeSymbol = static_cast<Cm::Sym::DelegateTypeSymbol*>(symbol);
        Cm::Ast::Specifiers specifiers = delegateNode->GetSpecifiers();
        bool isClassMember = delegateNode->Parent()->IsClassNode();
        SetAccess(delegateTypeSymbol, specifiers, isClassMember);
    }
    else
    {
        throw Exception("symbol '" + symbol->FullName() + "' does not denote a delegate type", symbol->GetSpan());
    }
}

} } // namespace Cm::Bind