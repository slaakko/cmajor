/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_PARAMETER_INCLUDED
#define CM_BIND_PARAMETER_INCLUDED
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.Sym/SymbolTable.hpp>

namespace Cm { namespace Bind {

void BindParameter(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Ast::ParameterNode* parameterNode, int parameterIndex);
void GenerateReceives(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::BoundTree::BoundFunction* boundFunction);

} } // namespace Cm::Bind

#endif // CM_BIND_PARAMETER_INCLUDED
