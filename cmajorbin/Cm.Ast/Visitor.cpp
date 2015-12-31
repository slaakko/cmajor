/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Ast {

Visitor::Visitor(bool visitBodies_, bool visitExpressions_) : visitBodies(visitBodies_), visitExpressions(visitExpressions_), skipContent(false)
{
}

Visitor::~Visitor()
{
}

void Visitor::PushSkipContent()
{
    skipContentStack.push(skipContent);
    skipContent = true;
}

void Visitor::PopSkipContent()
{
    skipContent = skipContentStack.top();
    skipContentStack.pop();
}

} } // namespace Cm::Ast