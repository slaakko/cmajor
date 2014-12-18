/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/Function.hpp>

namespace Cm { namespace BoundTree {

BoundFunction::BoundFunction(Cm::Ast::Node* syntaxNode_, Cm::Sym::FunctionSymbol* functionSymbol_) : BoundNode(syntaxNode_), functionSymbol(functionSymbol_)
{
}

void BoundFunction::SetBody(BoundCompoundStatement* body_)
{
    body.reset(body_);
}

} } // namespace Cm::BoundTree