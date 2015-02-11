/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_INIT_SYMBOL_TABLE_INCLUDED
#define CM_CORE_INIT_SYMBOL_TABLE_INCLUDED
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Core/ConceptRepository.hpp>

namespace Cm { namespace Core {

void InitSymbolTable(Cm::Sym::SymbolTable& symbolTable, GlobalConceptData& globalConceptData);

} } // namespace Cm::Core

#endif // CM_CORE_INIT_SYMBOL_TABLE_INCLUDED
