/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Delegate.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/MemberVariable.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Sym/DelegateSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Bind {

Cm::Sym::DelegateTypeSymbol* BindDelegate(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Ast::DelegateNode* delegateNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(delegateNode->Id()->Str(), Cm::Sym::SymbolTypeSetId::lookupDelegate);
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
        return delegateTypeSymbol;
    }
    else
    {
        throw Cm::Core::Exception("symbol '" + symbol->FullName() + "' does not denote a delegate type", symbol->GetSpan());
    }
}

void CompleteBindDelegate(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Sym::DelegateTypeSymbol* delegateTypeSymbol, Cm::Ast::DelegateNode* delegateNode)
{
    Cm::Sym::TypeSymbol* returnType = ResolveType(symbolTable, containerScope, fileScopes, classTemplateRepository, delegateNode->ReturnTypeExpr());
    delegateTypeSymbol->SetReturnType(returnType);
    delegateTypeSymbol->MakeIrType();
}

Cm::Sym::ClassDelegateTypeSymbol* BindClassDelegate(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, Cm::Ast::ClassDelegateNode* classDelegateNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(classDelegateNode->Id()->Str(), Cm::Sym::SymbolTypeSetId::lookupClassDelegate);
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
        return classDelegateTypeSymbol;
    }
    else
    {
        throw Cm::Core::Exception("symbol '" + symbol->FullName() + "' does not denote a class delegate type", symbol->GetSpan());
    }
}

void CompleBindClassDelegate(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Sym::ClassDelegateTypeSymbol* classDelegateTypeSymbol, Cm::Ast::ClassDelegateNode* classDelegateNode)
{
    Cm::Sym::TypeSymbol* returnType = ResolveType(symbolTable, containerScope, fileScopes, classTemplateRepository, classDelegateNode->ReturnTypeExpr());
    classDelegateTypeSymbol->SetReturnType(returnType);
    Cm::Sym::MemberVariableSymbol* obj = new Cm::Sym::MemberVariableSymbol(classDelegateNode->GetSpan(), "obj");
    obj->SetType(symbolTable.GetTypeRepository().MakeGenericPtrType(classDelegateNode->GetSpan()));
    classDelegateTypeSymbol->AddSymbol(obj);
    Cm::Sym::MemberVariableSymbol* dlg = new Cm::Sym::MemberVariableSymbol(classDelegateNode->GetSpan(), "dlg");
    Cm::Sym::DelegateTypeSymbol* dlgType = new Cm::Sym::DelegateTypeSymbol(classDelegateNode->GetSpan(), "dlgType");
    dlgType->SetReturnType(returnType);
    Cm::Sym::ParameterSymbol* dlgTypeObjParam = new Cm::Sym::ParameterSymbol(classDelegateNode->GetSpan(), "obj");
    dlgTypeObjParam->SetType(symbolTable.GetTypeRepository().MakeGenericPtrType(classDelegateNode->GetSpan()));
    dlgType->AddSymbol(dlgTypeObjParam);
    for (Cm::Sym::ParameterSymbol* classDelegateParam : classDelegateTypeSymbol->Parameters())
    {
        Cm::Sym::ParameterSymbol* dlgTypeParam = new Cm::Sym::ParameterSymbol(classDelegateNode->GetSpan(), classDelegateParam->Name());
        dlgTypeParam->SetType(classDelegateParam->GetType());
        dlgType->AddSymbol(dlgTypeParam);
    }
    dlgType->MakeIrType();
    classDelegateTypeSymbol->AddSymbol(dlgType);
    dlg->SetType(dlgType);
    classDelegateTypeSymbol->AddSymbol(dlg);
    classDelegateTypeSymbol->MakeIrType();
}

} } // namespace Cm::Bind