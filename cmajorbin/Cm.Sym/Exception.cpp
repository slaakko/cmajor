/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Exception.hpp>
#include <Cm.Parser/FileRegistry.hpp>

namespace Cm { namespace Sym {

std::string Expand(const std::string& errorMessage, const Span& span)
{
    std::string expandedMessage = "Error: " + errorMessage;
    if (span.Valid())
    {
        Cm::Parser::FileRegistry* currentFileRegistry = Cm::Parser::GetCurrentFileRegistry();
        if (currentFileRegistry)
        {
            const std::string& fileName = currentFileRegistry->GetParsedFileName(span.FileIndex());
            if (!fileName.empty())
            {
                expandedMessage.append(" (file '" + fileName + "' line " + std::to_string(span.LineNumber()) + ")");
            }
        }
    }
    return expandedMessage;
}

Exception::Exception(const std::string& message_, const Span& defined_) : std::runtime_error(Expand(message_, defined_)), defined(defined_), referenced(Span())
{
}

Exception::Exception(const std::string& message_, const Span& defined_, const Span& referenced_) : std::runtime_error(Expand(message_, defined_)), defined(defined_), referenced(referenced_)
{
}

} } // namespace Cm::Sym
