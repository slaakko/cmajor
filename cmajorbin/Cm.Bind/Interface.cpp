/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Interface.hpp>
#include <Cm.Sym/InterfaceTypeSymbol.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Bind/Access.hpp>

namespace Cm { namespace Bind {

void BindInterface(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, Cm::Ast::InterfaceNode* interfaceNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(interfaceNode->Id()->Str(), Cm::Sym::ScopeLookup::this_and_base_and_parent, Cm::Sym::SymbolTypeSetId::lookupInterfaceSymbols);
    if (symbol)
    {
        if (symbol->IsInterfaceTypeSymbol())
        {
            Cm::Sym::InterfaceTypeSymbol* interfaceTypeSymbol = static_cast<Cm::Sym::InterfaceTypeSymbol*>(symbol);
            if (interfaceTypeSymbol->Bound()) return;
            Cm::Ast::Specifiers specifiers = interfaceNode->GetSpecifiers();
            SetAccess(interfaceTypeSymbol, specifiers, false);
            if ((specifiers & Cm::Ast::Specifiers::static_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("interface cannot be static", interfaceTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::abstract_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("interface cannot be abstract", interfaceTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::virtual_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("interface cannot be virtual", interfaceTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::override_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("interface cannot be override", interfaceTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::explicit_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("interface cannot be explicit", interfaceTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::external) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("interface cannot be external", interfaceTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::suppress) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("interface cannot be suppressed", interfaceTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::default_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("interface cannot be default", interfaceTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::inline_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("interface cannot be inline", interfaceTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::cdecl_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("interface cannot be cdecl", interfaceTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::nothrow_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("interface cannot be nothrow", interfaceTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::throw_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("interface cannot be throw", interfaceTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::new_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("interface cannot be new", interfaceTypeSymbol->GetSpan());
            }
            Cm::Sym::MemberVariableSymbol* objMemberVar = new Cm::Sym::MemberVariableSymbol(interfaceTypeSymbol->GetSpan(), "obj");
            objMemberVar->SetType(symbolTable.GetTypeRepository().MakeGenericPtrType(interfaceTypeSymbol->GetSpan()));
            objMemberVar->SetLayoutIndex(0);
            interfaceTypeSymbol->AddSymbol(objMemberVar);
            Cm::Sym::MemberVariableSymbol* itabMemberVar = new Cm::Sym::MemberVariableSymbol(interfaceTypeSymbol->GetSpan(), "itab");
            itabMemberVar->SetType(symbolTable.GetTypeRepository().MakeGenericPtrType(interfaceTypeSymbol->GetSpan()));
            itabMemberVar->SetLayoutIndex(1);
            interfaceTypeSymbol->AddSymbol(itabMemberVar);
            interfaceTypeSymbol->MakeIrType();
            interfaceTypeSymbol->SetBound();
        }
        else
        {
            throw Cm::Core::Exception("symbol '" + symbol->FullName() + "' does not denote an interface", symbol->GetSpan());
        }
    }
    else
    {
        throw Cm::Core::Exception("interface symbol '" + interfaceNode->Id()->Str() + "' not found");
    }
}

} } // namespace Cm::Bind
