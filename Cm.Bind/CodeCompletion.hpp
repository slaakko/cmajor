/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_CODE_COMPLETION_INCLUDED
#define CM_BIND_CODE_COMPLETION_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.Ast/CompileUnit.hpp>
#include <Cm.Sym/Scope.hpp>

namespace Cm { namespace Bind {

void DoCodeCompletion(Cm::Ast::CompileUnitNode& ccUnit, Cm::Sym::SymbolTable& symbolTable, Cm::Sym::FileScope* fileScope, const std::string& ccResultFilePath, 
    Cm::BoundTree::BoundCompileUnit& boundCompileUnit);

} } // namespace Cm::Bind

#endif // CM_BIND_CODE_COMPLETION_INCLUDED


