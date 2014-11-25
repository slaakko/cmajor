/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Parser/ParsingContext.hpp>

namespace Cm { namespace Parser {

ParsingContext::ParsingContext(): parsingTypeExpr(false), parsingConcept(false), parsingTemplateId(false), parsingSimpleStatement(false), parsingLvalue(false), parsingArguments(false)
{
}

void ParsingContext::BeginParsingTypeExpr()
{
    parsingTypeExprStack.push(parsingTypeExpr);
    parsingTypeExpr = true;
}

void ParsingContext::EndParsingTypeExpr()
{
    parsingTypeExpr = parsingTypeExprStack.top();
    parsingTypeExprStack.pop();
}

void ParsingContext::BeginParsingConcept()
{
    parsingConceptStack.push(parsingConcept);
    parsingConcept = true;
}

void ParsingContext::EndParsingConcept()
{
    parsingConcept = parsingConceptStack.top();
    parsingConceptStack.pop();
}

void ParsingContext::BeginParsingTemplateId()
{
    parsingTemplateIdStack.push(parsingTemplateId);
    parsingTemplateId = true;
}

void ParsingContext::EndParsingTemplateId()
{
    parsingTemplateId = parsingTemplateIdStack.top();
    parsingTemplateIdStack.pop();
}

void ParsingContext::BeginParsingSimpleStatement()
{
    parsingSimpleStatementStack.push(parsingSimpleStatement);
    parsingSimpleStatement = true;
}

void ParsingContext::EndParsingSimpleStatement() 
{
    parsingSimpleStatement = parsingSimpleStatementStack.top();
    parsingSimpleStatementStack.pop();
}

void ParsingContext::BeginParsingLvalue()
{
    parsingLvalueStack.push(parsingLvalue);
    parsingLvalue = true;
}

void ParsingContext::EndParsingLvalue()
{
    parsingLvalue = parsingLvalueStack.top();
    parsingLvalueStack.pop();
}

void ParsingContext::BeginParsingArguments()
{
    parsingArgumentsStack.push(parsingArguments);
    parsingArguments = true;
}

void ParsingContext::EndParsingArguments()
{
    parsingArguments = parsingArgumentsStack.top();
    parsingArgumentsStack.pop();
}

} } // namespace Cm::Parser
