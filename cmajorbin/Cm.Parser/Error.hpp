/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_PARSER_ERROR_INCLUDED
#define CM_PARSER_ERROR_INCLUDED
#include <Cm.Parsing/Scanner.hpp>

namespace Cm { namespace Parser {

using Cm::Parsing::Span;

std::string Expand(const std::string& errorMessage, const Span& span);
std::string Expand(const std::string& errorMessage, const Span& primarySpan, const Span& referenceSpan);
std::string Expand(const std::string& errorMessage, const Span& span, const std::vector<Span>& references);

} } // Cm::Parser

#endif // CM_PARSER_ERROR_INCLUDED
