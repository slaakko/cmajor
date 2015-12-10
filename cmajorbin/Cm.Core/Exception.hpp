/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_EXCEPTION_INCLUDED
#define CM_CORE_EXCEPTION_INCLUDED
#include <Cm.Ast/Exception.hpp>
#include <Cm.Parsing/Scanner.hpp>
#include <Cm.Util/ToolError.hpp>
#include <stdexcept>

namespace Cm { namespace Core {

using Cm::Parsing::Span;

class Exception : public Cm::Ast::Exception
{
public:
    Exception(const std::string& message_);
    Exception(const std::string& message_, const Span& defined_);
    Exception(const std::string& message_, const Span& defined_, const Span& referenced_);
    Exception(const std::string& message_, const Span& defined_, const Span& referenced1_, const Span& referenced2_);
    Exception(const std::string& message_, const Span& defined_, const std::vector<Span>& references_);
    const Span& Defined() const { return defined; }
    const std::vector<Span>& References() const { return references; }
    const std::string& Message() const { return message; }
    void AddReference(const Span& span) override;
private:
    std::string message;
    Span defined;
    std::vector<Span> references;
};

class ToolErrorExcecption
{
public:
    ToolErrorExcecption(const Cm::Util::ToolError& toolError_);
    const Cm::Util::ToolError& Error() const { return toolError; }
private:
    Cm::Util::ToolError toolError;
};

class ConceptCheckException : public Exception
{
public:
    ConceptCheckException();
    ConceptCheckException(const std::string& message_);
    ConceptCheckException(const std::string& message_, const Span& defined_);
};

} } // namespace Cm::Core

#endif // CM_CORE_EXCEPTION_INCLUDED
