/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Parser/Error.hpp>
#include <Cm.Parser/FileRegistry.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Util/MappedInputFile.hpp>

namespace Cm { namespace Parser {

std::string Expand(const std::string& errorMessage, const Span& span)
{
    std::vector<Span> references;
    return Expand(errorMessage, span, references);
}

std::string Expand(const std::string& errorMessage, const Span& primarySpan, const Span& referenceSpan)
{
    std::vector<Span> references(1, referenceSpan);
    return Expand(errorMessage, primarySpan, references);
}

std::string Expand(const std::string& errorMessage, const Span& span, const std::vector<Span>& references)
{
    std::string expandedMessage = "Error: " + errorMessage;
    if (span.Valid())
    {
        FileRegistry* currentFileRegistry = GetCurrentFileRegistry();
        if (currentFileRegistry)
        {
            const std::string& fileName = currentFileRegistry->GetParsedFileName(span.FileIndex());
            if (!fileName.empty())
            {
                expandedMessage.append(" (file '" + fileName + "', line " + std::to_string(span.LineNumber()) + ")");
                Cm::Util::MappedInputFile file(fileName);
                expandedMessage.append(":\n").append(Cm::Parsing::GetErrorLines(file.Begin(), file.End(), span));
            }
            for (const Span& referenceSpan : references)
            {
                const std::string& fileName = currentFileRegistry->GetParsedFileName(referenceSpan.FileIndex());
                if (!fileName.empty())
                {
                    expandedMessage.append("\nsee reference to file '" + fileName + "', line " + std::to_string(referenceSpan.LineNumber()));
                    Cm::Util::MappedInputFile file(fileName);
                    expandedMessage.append(":\n").append(Cm::Parsing::GetErrorLines(file.Begin(), file.End(), referenceSpan));
                }
            }
        }
    }
    return expandedMessage;
}

} } // Cm::Parser