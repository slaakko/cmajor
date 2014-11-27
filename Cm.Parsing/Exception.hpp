/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_PARSING_EXCEPTION_INCLUDED
#define CM_PARSING_EXCEPTION_INCLUDED
#include <Cm.Parsing/Scanner.hpp>
#include <stdexcept>

namespace Cm { namespace Parsing {

void SetFileInfo(const std::vector<std::string>& files_);
std::string GetErrorLines(const char* start, const char* end, const char* pos);
void ThrowException(const std::string& message, const Span& span);

class ParsingException : public std::runtime_error
{
public:
    ParsingException(const std::string& message_, const std::string& fileName_, const Span& span_, const char* start_, const char* end_);
    const std::string& Message() const { return message; }
    const std::string& FileName() const { return fileName; }
    const Span& GetSpan() const { return span; }
    const char* Start() const { return start; }
    const char* End() const { return end; }
private:
    std::string message;
    std::string fileName;
    Span span;
    const char* start;
    const char* end;
};

class ExpectationFailure : public ParsingException
{
public:
    ExpectationFailure(const std::string& info_, const std::string& fileName_, const Span& span_, const char* start, const char* end);
    const std::string& Info() const { return info; }
    void CombineInfo(const std::string& parentInfo);
private:
    std::string info;
};

class CombinedParsingError : public std::runtime_error
{
public:
    CombinedParsingError();
    const std::vector<ExpectationFailure>& Errors() const { return errors; }
    std::vector<ExpectationFailure>& Errors() { return errors; }
private:
    std::vector<ExpectationFailure> errors;
};

} } // namespace Cm::Parsing

#endif // CM_PARSING_EXCEPTION_INCLUDED
