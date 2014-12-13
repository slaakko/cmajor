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
    BoundFunction(Cm::Ast::Node* syntaxNode_);
    void SetBody(BoundCompoundStatement* body_);
private:
    std::unique_ptr<BoundCompoundStatement> body;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_FUNCTION_INCLUDED
