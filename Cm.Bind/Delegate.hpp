/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_DELEGATE_INCLUDED
#define CM_BIND_DELEGATE_INCLUDED
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/DelegateSymbol.hpp>
#include <Cm.Core/ClassTemplateRepository.hpp>
#include <Cm.Ast/Delegate.hpp>

namespace Cm { namespace Bind {

Cm::Sym::DelegateTypeSymbol* BindDelegate(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Ast::DelegateNode* delegateNode);

void BindDelegate(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Ast::DelegateNode* delegateNode, Cm::Sym::DelegateTypeSymbol* delegateTypeSymbol);

void CompleteBindDelegate(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Sym::DelegateTypeSymbol* delegateTypeSymbol, Cm::Ast::DelegateNode* delegateNode);

Cm::Sym::ClassDelegateTypeSymbol* BindClassDelegate(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, Cm::Ast::ClassDelegateNode* classDelegateNode);

void BindClassDelegate(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope,
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, Cm::Ast::ClassDelegateNode* classDelegateNode, Cm::Sym::ClassDelegateTypeSymbol* classDelegateTypeSymbol);

void CompleBindClassDelegate(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Sym::ClassDelegateTypeSymbol* classDelegateTypeSymbol, Cm::Ast::ClassDelegateNode* classDelegateNode);

} } // namespace Cm::Bind

#endif // CM_BIND_DELEGATE_INCLUDED
