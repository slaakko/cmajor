/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Delegate.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Core/Exception.hpp>
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
        if ((specifiers & Cm::Ast::Specifiers::abstract_) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("delegate cannot be abstract", delegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::virtual_) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("delegate cannot be virtual", delegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::override_) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("delegate cannot be override", delegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::static_) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("delegate cannot be static", delegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::explicit_) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("delegate cannot be explicit", delegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::external) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("delegate cannot be external", delegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::suppress) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("delegate cannot be suppressed", delegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::default_) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("delegate cannot be default", delegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::inline_) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("delegate cannot be inline", delegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::cdecl_) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("delegate cannot be cdecl", delegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::nothrow_) != Cm::Ast::Specifiers::none)
        {
            delegateTypeSymbol->SetNothrow();
        }
        if ((specifiers & Cm::Ast::Specifiers::throw_) != Cm::Ast::Specifiers::none)
        {
            delegateTypeSymbol->SetThrow();
        }
    }
    else
    {
        throw Cm::Core::Exception("symbol '" + symbol->FullName() + "' does not denote a delegate type", symbol->GetSpan());
    }
}

void BindClassDelegate(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::ClassDelegateNode* classDelegateNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(classDelegateNode->Id()->Str());
    if (symbol->IsClassDelegateTypeSymbol())
    {
        Cm::Sym::ClassDelegateTypeSymbol* classDelegateTypeSymbol = static_cast<Cm::Sym::ClassDelegateTypeSymbol*>(symbol);
        Cm::Ast::Specifiers specifiers = classDelegateNode->GetSpecifiers();
        bool isClassMember = classDelegateNode->Parent()->IsClassNode();
        SetAccess(classDelegateTypeSymbol, specifiers, isClassMember);
        if ((specifiers & Cm::Ast::Specifiers::abstract_) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("class delegate cannot be abstract", classDelegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::virtual_) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("class delegate cannot be virtual", classDelegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::override_) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("class delegate cannot be override", classDelegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::static_) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("class delegate cannot be static", classDelegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::explicit_) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("class delegate cannot be explicit", classDelegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::external) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("class delegate cannot be external", classDelegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::suppress) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("class delegate cannot be suppressed", classDelegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::default_) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("class delegate cannot be default", classDelegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::inline_) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("class delegate cannot be inline", classDelegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::cdecl_) != Cm::Ast::Specifiers::none)
        {
            throw Cm::Core::Exception("class delegate cannot be cdecl", classDelegateTypeSymbol->GetSpan());
        }
        if ((specifiers & Cm::Ast::Specifiers::nothrow_) != Cm::Ast::Specifiers::none)
        {
            classDelegateTypeSymbol->SetNothrow();
        }
        if ((specifiers & Cm::Ast::Specifiers::throw_) != Cm::Ast::Specifiers::none)
        {
            classDelegateTypeSymbol->SetThrow();
        }
    }
}

} } // namespace Cm::Bind