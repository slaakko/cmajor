/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Parsing/Scanner.hpp>
#include <Cm.Parsing/Parser.hpp>
#include <Cm.Parsing/Exception.hpp>

namespace Cm { namespace Parsing {

std::string NarrowString(const char* start, const char* end)
{
    std::string s;
    s.reserve(end - start);
    while (start != end)
    {
        if (*start != '\r')
        {
            s.append(1, *start);
        }
        ++start;
    }
    return s;
}

Scanner::Scanner(const char* start_, const char* end_, const std::string& fileName_, int fileIndex_, Parser* skipper_): 
    start(start_), end(end_), skipper(skipper_), skipping(false), tokenCounter(0), fileName(fileName_), span(fileIndex_), 
    log(nullptr), expectationCounter(0), recover(false)
{
}

void Scanner::operator++()
{
    char c = GetChar();
    ++span;
    if (c == '\n')
    {
        span.IncLineNumber();
    }
}

void Scanner::Skip()
{
    if (tokenCounter == 0 && skipper)
    {
        Span save = span;
        BeginToken();
        skipping = true;
        ObjectStack stack;
        Match match = skipper->Parse(*this, stack);
        skipping = false;
        EndToken();
        if (!match.Hit())
        {
            span = save;
        }
    }
}

int Scanner::LineEndIndex()
{
    int lineEndIndex = span.Start();
    int contentLength = static_cast<int>(end - start);
    while (lineEndIndex < contentLength && (start[lineEndIndex] != '\r' && start[lineEndIndex] != '\n'))
    {
        ++lineEndIndex;
    }
    return lineEndIndex;
}

std::string Scanner::RestOfLine()
{
    std::string restOfLine(start + span.Start(), start + (LineEndIndex() - span.Start()));
    return restOfLine;
}

void Scanner::AddException(const ExpectationFailure& exception)
{
    if (!combinedError)
    {
        combinedError.reset(new CombinedParsingError());
    }
    combinedError->Errors().push_back(exception);
}

void Scanner::Synchronize(const std::string& synchronizeCharacters)
{
    if (AtEnd()) return;
    char c = GetChar();
    while (!AtEnd() && synchronizeCharacters.find(c) == std::string::npos)
    {
        ++*this;
        c = GetChar();
    }
    if (!AtEnd())
    {
        ++*this;
    }
    expectationCounter = 0;
}

bool Scanner::HasErrors() const
{
    return combinedError && !combinedError->Errors().empty();
}

const CombinedParsingError& Scanner::GetCombinedError() const
{
    if (combinedError)
    {
        return *combinedError;
    }
    else
    {
        throw std::runtime_error("scanner exception not active");
    }
}

} } // namespace Cm::Parsing
