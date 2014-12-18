/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BOUND_TREE_FUNCTION_INCLUDED
#define CM_BOUND_TREE_FUNCTION_INCLUDED
#include <Cm.BoundTree/Statement.hpp>

namespace Cm { namespace BoundTree {

class BoundFunction : public BoundNode
{
public:
    BoundFunction(Cm::Ast::Node* syntaxNode_, Cm::Sym::FunctionSymbol* functionSymbol_);
    Cm::Sym::FunctionSymbol* GetFunctionSymbol() const { return functionSymbol; }
    void SetBody(BoundCompoundStatement* body_);
private:
    std::unique_ptr<BoundCompoundStatement> body;
    Cm::Sym::FunctionSymbol* functionSymbol;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_FUNCTION_INCLUDED
