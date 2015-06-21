/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_TYPEDEF_INCLUDED
#define CM_BIND_TYPEDEF_INCLUDED
#include <Cm.Core/ClassTemplateRepository.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/Scope.hpp>
#include <Cm.Sym/TypedefSymbol.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Ast/Typedef.hpp>

namespace Cm { namespace Bind {

void BindTypedef(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Ast::TypedefNode* typedefNode);
void BindTypedef(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Ast::TypedefNode* typedefNode, Cm::Sym::TypedefSymbol* typedefSymbol);
void BindTypedef(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Ast::TypedefStatementNode* typedefStatementNode);
void BindTypedef(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Ast::TypedefStatementNode* typedefStatementNode, Cm::Sym::TypedefSymbol* typedefSymbol);

} } // namespace Cm::Bind

#endif // CM_BIND_TYPEDEF_INCLUDED
