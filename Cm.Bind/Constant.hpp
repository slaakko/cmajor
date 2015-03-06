/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_CONSTANT_INCLUDED
#define CM_BIND_CONSTANT_INCLUDED
#include <Cm.Core/ClassTemplateRepository.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/ConstantSymbol.hpp>
#include <Cm.Ast/Constant.hpp>

namespace Cm { namespace Bind {

void BindConstant(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Ast::ConstantNode* constantNode);
void BindConstant(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Ast::ConstantNode* constantNode, Cm::Sym::ConstantSymbol* constantSymbol);

} } // namespace Cm::Bind

#endif // CM_BIND_CONSTANT_INCLUDED
