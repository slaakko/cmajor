/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_PARAMETER_INCLUDED
#define CM_BIND_PARAMETER_INCLUDED
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.Core/ClassConversionTable.hpp>
#include <Cm.Core/PointerOpRepository.hpp>
#include <Cm.Sym/SymbolTable.hpp>

namespace Cm { namespace Bind {

void BindParameter(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::ParameterNode* parameterNode, int parameterIndex);
void GenerateReceives(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ConversionTable& conversionTable, Cm::Core::ClassConversionTable& classConversionTable, Cm::Core::PointerOpRepository& pointerOpRepository,
    Cm::BoundTree::BoundFunction* boundFunction);

} } // namespace Cm::Bind

#endif CM_BIND_PARAMETER_INCLUDED
