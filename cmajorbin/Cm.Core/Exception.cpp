/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/Exception.hpp>
#include <Cm.Parser/Error.hpp>

namespace Cm { namespace Core {

Exception::Exception(const std::string& message_) : Cm::Ast::Exception(message_), message(message_), defined()
{
}

Exception::Exception(const std::string& message_, const Span& defined_) : Cm::Ast::Exception(Cm::Parser::Expand(message_, defined_)), message(message_), defined(defined_)
{
}

Exception::Exception(const std::string& message_, const Span& defined_, const Span& referenced_) : 
    Cm::Ast::Exception(Cm::Parser::Expand(message_, defined_, referenced_)), message(message_), defined(defined_)
{
    references.push_back(referenced_);
}

std::vector<Span> MakeReferences(const Span& referenced1_, const Span& referenced2_)
{
    std::vector<Span> references;
    references.push_back(referenced1_);
    references.push_back(referenced2_);
    return references;
}

Exception::Exception(const std::string& message_, const Span& defined_, const Span& referenced1_, const Span& referenced2_) :
    Cm::Ast::Exception(Cm::Parser::Expand(message_, defined_, MakeReferences(referenced1_, referenced2_))), message(message_), defined(defined_)
{
    references.push_back(referenced1_);
    references.push_back(referenced2_);
}

Exception::Exception(const std::string& message_, const Span& defined_, const std::vector<Span>& references_) : 
    Cm::Ast::Exception(Cm::Parser::Expand(message_, defined_, references_)), message(message_), defined(defined_), references(references_)
{
}

void Exception::AddReference(const Span& span)
{
    references.push_back(span);
}

ToolErrorExcecption::ToolErrorExcecption(const Cm::Util::ToolError& toolError_) : toolError(toolError_)
{
}

ConceptCheckException::ConceptCheckException() : Exception("")
{
}

ConceptCheckException::ConceptCheckException(const std::string& message_) : Exception(message_)
{
}

ConceptCheckException::ConceptCheckException(const std::string& message_, const Span& defined_) : Exception(message_, defined_)
{
}

} } // namespace Cm::Core
