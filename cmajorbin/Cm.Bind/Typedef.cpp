/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Typedef.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Sym/TypedefSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Bind {

void BindTypedef(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Ast::TypedefNode* typedefNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(typedefNode->Id()->Str(), Cm::Sym::SymbolTypeSetId::lookupTypedef);
    if (!symbol)
    {
        for (const std::unique_ptr<Cm::Sym::FileScope>& fileScope : fileScopes)
        {
            symbol = fileScope->Lookup(typedefNode->Id()->Str(), Cm::Sym::SymbolTypeSetId::lookupTypedef);
            if (symbol) break;
        }
    }
    if (symbol)
    {
        if (symbol->IsTypedefSymbol())
        {
            Cm::Sym::TypedefSymbol* typedefSymbol = static_cast<Cm::Sym::TypedefSymbol*>(symbol);
            BindTypedef(symbolTable, containerScope, fileScopes, classTemplateRepository, boundCompileUnit, typedefNode, typedefSymbol);
        }
        else
        {
            throw Cm::Core::Exception("symbol '" + symbol->FullName() + "' does not denote a typedef", typedefNode->GetSpan(), symbol->GetSpan());
        }
    }
    else
    {
        throw Cm::Core::Exception("typedef symbol '" + typedefNode->Id()->Str() + "' not found", typedefNode->GetSpan());
    }
}

void BindTypedef(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Ast::TypedefStatementNode* typedefStatementNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(typedefStatementNode->Id()->Str(), Cm::Sym::SymbolTypeSetId::lookupTypedef);
    if (!symbol)
    {
        for (const std::unique_ptr<Cm::Sym::FileScope>& fileScope : fileScopes)
        {
            symbol = fileScope->Lookup(typedefStatementNode->Id()->Str(), Cm::Sym::SymbolTypeSetId::lookupTypedef);
            if (symbol) break;
        }
    }
    if (symbol)
    {
        if (symbol->IsTypedefSymbol())
        {
            Cm::Sym::TypedefSymbol* typedefSymbol = static_cast<Cm::Sym::TypedefSymbol*>(symbol);
            BindTypedef(symbolTable, containerScope, fileScopes, classTemplateRepository, boundCompileUnit, typedefStatementNode, typedefSymbol);
        }
        else
        {
            throw Cm::Core::Exception("symbol '" + symbol->FullName() + "' does not denote a typedef", symbol->GetSpan());
        }
    }
    else
    {
        throw Cm::Core::Exception("typedef symbol '" + typedefStatementNode->Id()->Str() + "' not found");
    }

}

void BindTypedef(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Ast::TypedefNode* typedefNode, Cm::Sym::TypedefSymbol* typedefSymbol)
{
    if (typedefSymbol->Evaluating())
    {
        throw Cm::Core::Exception("cyclic typedef definitions detected", typedefSymbol->GetSpan());
    }
    if (typedefSymbol->Bound())
    {
        return;
    }
    Cm::Ast::Specifiers specifiers = typedefNode->GetSpecifiers();
    bool isClassMember = typedefNode->Parent()->IsClassNode();
    SetAccess(typedefSymbol, specifiers, isClassMember);
    if ((specifiers & Cm::Ast::Specifiers::abstract_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("typedef cannot be abstract", typedefSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::virtual_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("typedef cannot be virtual", typedefSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::override_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("typedef cannot be override", typedefSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::static_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("typedef cannot be static", typedefSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::explicit_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("typedef cannot be explicit", typedefSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::external) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("typedef cannot be external", typedefSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::suppress) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("typedef cannot be suppressed", typedefSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::default_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("typedef cannot be default", typedefSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::inline_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("typedef cannot be inline", typedefSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::constexpr_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("typedef cannot be constexpr", typedefSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::cdecl_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("typedef cannot be cdecl", typedefSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::nothrow_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("typedef cannot be nothrow", typedefSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::throw_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("typedef cannot be throw", typedefSymbol->GetSpan());
    }
    if (typedefSymbol->Parent()->IsClassTemplateSymbol())
    {
        typedefSymbol->SetBound();
        return;
    }
    typedefSymbol->SetEvaluating();
    Cm::Sym::TypeSymbol* type = ResolveType(symbolTable, containerScope, fileScopes, classTemplateRepository, boundCompileUnit, typedefNode->TypeExpr());
    typedefSymbol->ResetEvaluating();
    typedefSymbol->SetType(type);
    typedefSymbol->SetBound();
}

void BindTypedef(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Ast::TypedefStatementNode* typedefStatementNode, Cm::Sym::TypedefSymbol* typedefSymbol)
{
    if (typedefSymbol->Evaluating())
    {
        throw Cm::Core::Exception("cyclic typedef definitions detected", typedefSymbol->GetSpan());
    }
    if (typedefSymbol->Bound())
    {
        return;
    }
    SetAccess(typedefSymbol, Cm::Ast::Specifiers::public_, false);
    typedefSymbol->SetEvaluating();
    Cm::Sym::TypeSymbol* type = ResolveType(symbolTable, containerScope, fileScopes, classTemplateRepository, boundCompileUnit, typedefStatementNode->TypeExpr());
    typedefSymbol->ResetEvaluating();
    typedefSymbol->SetType(type);
    typedefSymbol->SetBound();
}

} } // namespace Cm::Bind