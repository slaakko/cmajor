/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_EVALUATOR_INCLUDED
#define CM_BIND_EVALUATOR_INCLUDED
#include <Cm.Sym/Value.hpp>
#include <Cm.Sym/Scope.hpp>
#include <Cm.Ast/Visitor.hpp>
#include <stack>
#include <memory>

namespace Cm { namespace Bind {

Cm::Sym::Value* Evaluate(Cm::Sym::ValueType targetType, bool cast, Cm::Ast::Node* value, Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* currentContainerScope, Cm::Sym::FileScope* fileScope);

} } // namespace Cm::Bind

#endif // CM_BIND_EVALUATOR_INCLUDED
