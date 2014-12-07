/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_LOCAL_VARIABLE_SYMBOL_INCLUDED
#define CM_SYM_LOCAL_VARIABLE_SYMBOL_INCLUDED
#include <Cm.Sym/Symbol.hpp>
#include <Cm.Ast/Statement.hpp>

namespace Cm { namespace Sym {

class LocalVariableSymbol : public Symbol
{
public:
    LocalVariableSymbol(Cm::Ast::ConstructionStatementNode* constructionStatementNode);
};

} } // namespace Cm::Sym

#endif // CM_SYM_LOCAL_VARIABLE_SYMBOL_INCLUDED
