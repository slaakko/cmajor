/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_DECLARATION_BLOCK_INCLUDED
#define CM_SYM_DECLARATION_BLOCK_INCLUDED
#include <Cm.Sym/ContainerSymbol.hpp>
#include <Cm.Ast/Statement.hpp>

namespace Cm { namespace Sym {

class DeclarationBlock : public ContainerSymbol
{
public:
    DeclarationBlock(Cm::Ast::StatementNode* statementNode);
};

} } // namespace Cm::Sym

#endif // CM_SYM_DECLARATION_BLOCK_INCLUDED
