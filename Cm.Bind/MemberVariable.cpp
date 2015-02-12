/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/MemberVariable.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Bind {

void BindMemberVariable(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Ast::MemberVariableNode* memberVariableNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(memberVariableNode->Id()->Str());
    if (symbol)
    {
        if (symbol->IsMemberVariableSymbol())
        {
            Cm::Sym::MemberVariableSymbol* memberVariableSymbol = static_cast<Cm::Sym::MemberVariableSymbol*>(symbol);
            BindMemberVariable(symbolTable, containerScope, fileScopes, memberVariableNode, memberVariableSymbol);
        }
        else
        {
            throw Cm::Core::Exception("symbol '" + symbol->FullName() + "' does not denote a member variable", symbol->GetSpan());
        }
    }
    else
    {
        throw Cm::Core::Exception("symbol '" + memberVariableNode->Id()->Str() + "' not found");
    }
}

void BindMemberVariable(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Ast::MemberVariableNode* memberVariableNode, Cm::Sym::MemberVariableSymbol* memberVariableSymbol)
{
    if (memberVariableSymbol->Bound())
    {
        return;
    }
    Cm::Ast::Specifiers specifiers = memberVariableNode->GetSpecifiers();
    bool isClassMember = true;
    SetAccess(memberVariableSymbol, specifiers, isClassMember);
    if ((specifiers & Cm::Ast::Specifiers::abstract_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("member variable cannot be abstract", memberVariableSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::virtual_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("member variable cannot be virtual", memberVariableSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::override_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("member variable cannot be override", memberVariableSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::explicit_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("member variable cannot be explicit", memberVariableSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::external) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("member variable cannot be external", memberVariableSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::suppress) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("member variable cannot be suppressed", memberVariableSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::default_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("member variable cannot be default", memberVariableSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::inline_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("member variable cannot be inline", memberVariableSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::cdecl_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("member variable cannot be cdecl", memberVariableSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::nothrow_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("member variable cannot be nothrow", memberVariableSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::throw_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("member variable cannot be throw", memberVariableSymbol->GetSpan());
    }
    Cm::Sym::TypeSymbol* type = ResolveType(symbolTable, containerScope, fileScopes, memberVariableNode->TypeExpr());
    if (type->Access() < memberVariableSymbol->Access())
    {
        throw Cm::Core::Exception("type of a member variable must be at least as accessible as the member variable itself", type->GetSpan(), memberVariableSymbol->GetSpan());
    }
    memberVariableSymbol->SetType(type);
    memberVariableSymbol->SetBound();
}

} } // namespace Cm::Bind
