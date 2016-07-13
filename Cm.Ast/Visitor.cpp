/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Ast {

Visitor::Visitor(bool visitBodies_, bool visitExpressions_) : visitBodies(visitBodies_), visitExpressions(visitExpressions_), skipContent(false), skipArguments(false)
{
}

Visitor::~Visitor()
{
}

void Visitor::PushSkipContent(bool skip)
{
    skipContentStack.push(skipContent);
    skipContent = skip;
}

void Visitor::PopSkipContent()
{
    skipContent = skipContentStack.top();
    skipContentStack.pop();
}

void Visitor::PushVisitExpressions(bool visit)
{
    visitExpressionsStack.push(visitExpressions);
    visitExpressions = visit;
}

void Visitor::PopVisitExpressions()
{
    visitExpressions = visitExpressionsStack.top();
    visitExpressionsStack.pop();
}

void Visitor::PushSkipArguments(bool skip)
{
    skipArgumentsStack.push(skipArguments);
    skipArguments = skip;
}

void Visitor::PopSkipArguments()
{
    skipArguments = skipArgumentsStack.top();
    skipArgumentsStack.pop();
}


} } // namespace Cm::Ast