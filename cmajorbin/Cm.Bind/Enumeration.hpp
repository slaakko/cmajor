/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_ENUMERATION_INCLUDED
#define CM_BIND_ENUMERATION_INCLUDED
#include <Cm.Core/ClassTemplateRepository.hpp>
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Sym/SymbolTable.hpp>

namespace Cm { namespace Bind {

void BindEnumType(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Ast::EnumTypeNode* enumTypeNode);

void BindEnumType(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Ast::EnumTypeNode* enumTypeNode, Cm::Sym::EnumTypeSymbol* enumTypeSymbol);

void BindEnumConstant(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Ast::EnumConstantNode* enumConstantNode);

} } // namespace Cm::Bind

#endif // CM_BIND_ENUMERATION_INCLUDED
