/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_PARSER_PARSINGCONTEXT
#define CM_PARSER_PARSINGCONTEXT
#include <stack>

namespace Cm { namespace Parser {

class ParsingContext
{
public:
    ParsingContext();
    bool ParsingTypeExpr() const { return parsingTypeExpr; }
    void BeginParsingTypeExpr();
    void EndParsingTypeExpr();
    bool ParsingConcept() const { return parsingConcept; }
    void BeginParsingConcept();
    void EndParsingConcept();
    bool ParsingTemplateId() const { return parsingTemplateId; }
    void BeginParsingTemplateId();
    void EndParsingTemplateId();
    bool ParsingSimpleStatement() const { return parsingSimpleStatement; }
    void BeginParsingSimpleStatement();
    void EndParsingSimpleStatement();
    bool ParsingLvalue() const { return parsingLvalue; }
    void BeginParsingLvalue();
    void EndParsingLvalue();
    bool ParsingArguments() const { return parsingArguments; }
    void BeginParsingArguments();
    void EndParsingArguments();
private:
    bool parsingTypeExpr;
    std::stack<bool> parsingTypeExprStack;
    bool parsingConcept;
    std::stack<bool> parsingConceptStack;
    bool parsingTemplateId;
    std::stack<bool> parsingTemplateIdStack;
    bool parsingSimpleStatement;
    std::stack<bool> parsingSimpleStatementStack;
    bool parsingLvalue;
    std::stack<bool> parsingLvalueStack;
    bool parsingArguments;
    std::stack<bool> parsingArgumentsStack;
};

} } // namespace Cm::Parser

#endif // CM_PARSER_PARSINGCONTEXT
