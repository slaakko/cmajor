/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/Visitor.hpp>
#include <Cm.BoundTree/BoundStatement.hpp>

namespace Cm { namespace BoundTree {

Visitor::Visitor(bool visitFunctionBody_) : visitFunctionBody(visitFunctionBody_), skipContent(false)
{
}

Visitor::~Visitor()
{
}

void Visitor::VisitStatement(BoundStatement& statement)
{
    BeginVisitStatement(statement);
    statement.Accept(*this);
    EndVisitStatement(statement);
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

} } // namespace Cm::BoundTree