/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Compiler/IdeError.hpp>
#include <Cm.Build/Build.hpp>
#include <Cm.Parser/FileRegistry.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <Cm.Util/MappedInputFile.hpp>

namespace Cm { namespace Compiler {

std::string GetFilePath(int fileIndex)
{
    return Cm::Parser::GetCurrentFileRegistry()->GetParsedFileName(fileIndex);
}

int GetColumn(int fileIndex, int pos)
{
    if (pos >= 0)
    {
        std::string filePath = GetFilePath(fileIndex);
        Cm::Util::MappedInputFile file(filePath);
        const char* start = file.Begin();
        const char* p = file.Begin() + pos;
        if (p < file.End())
        {
            const char* q = p;
            while (q > start && *q != '\n' && *q != '\r')
            {
                --q;
            }
            if (*q == '\n' || *q == '\r')
            {
                ++q;
            }
            int column = int(p - q + 1);
            return column;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

IdeError::IdeError(const Cm::Parsing::ExpectationFailure& ex) :
    tool("cmc.syntax"), project(Cm::Build::GetCurrentProjectName()), description(ex.Message()), file(ex.FileName()), line(ex.GetSpan().LineNumber()), 
    startColumn(GetColumn(ex.GetSpan().FileIndex(), ex.GetSpan().Start())),
    endColumn(GetColumn(ex.GetSpan().FileIndex(), ex.GetSpan().End()))
{
}

IdeError::IdeError(const Cm::Sym::Exception& ex) :
    tool("cmc.symbols"), project(Cm::Build::GetCurrentProjectName()), description(ex.Message()), file(GetFilePath(ex.Defined().FileIndex())), line(ex.Defined().LineNumber()),
    startColumn(GetColumn(ex.Defined().FileIndex(), ex.Defined().Start())),
    endColumn(GetColumn(ex.Defined().FileIndex(), ex.Defined().End()))
{
}

IdeError::IdeError(const Cm::Core::Exception& ex) :
    tool("cmc.bind"), project(Cm::Build::GetCurrentProjectName()), description(ex.Message()), file(GetFilePath(ex.Defined().FileIndex())), line(ex.Defined().LineNumber()),
    startColumn(GetColumn(ex.Defined().FileIndex(), ex.Defined().Start())),
    endColumn(GetColumn(ex.Defined().FileIndex(), ex.Defined().End()))
{
}

IdeError::IdeError(const Cm::Core::ToolErrorExcecption& ex) :
    tool(ex.Error().ToolName()), project(Cm::Build::GetCurrentProjectName()), description(ex.Error().Message()), file(ex.Error().FilePath()), line(ex.Error().Line()),
    startColumn(ex.Error().Column()), endColumn(0)
{
}

IdeError::IdeError(const std::exception& ex) :
    tool("cmc.internal"), project(Cm::Build::GetCurrentProjectName()), description(ex.what()), file(), line(0), startColumn(0), endColumn(0)
{
}

IdeError::IdeError(const std::string& message) :
    tool("cmc.internal"), project(Cm::Build::GetCurrentProjectName()), description(message), file(), line(0), startColumn(0), endColumn(0)
{
}

IdeError::IdeError(const Cm::Parsing::Span& reference) :
    tool("cmc.reference"), project(""), description("see reference to:"), file(GetFilePath(reference.FileIndex())), line(reference.LineNumber()), 
    startColumn(GetColumn(reference.FileIndex(), reference.Start())),
    endColumn(GetColumn(reference.FileIndex(), reference.End()))
{
}

std::ostream& operator<<(std::ostream& s, const IdeError& error)
{
    return s << "{ \"tool\" : \"" << error.Tool() << "\", \"project\" : \"" << error.Project() << "\", \"description\" : \"" << Cm::Util::StringStr(error.Description()) << 
        "\", \"file\" : \"" << error.File() << "\", " << "\"line\" : " << error.Line() << ", \"startColumn\" : " << error.StartColumn() << ", \"endColumn\" : " << error.EndColumn() <<
        " }";
}

IdeErrorCollection::IdeErrorCollection(const Cm::Parsing::CombinedParsingError& ex)
{
    for (const Cm::Parsing::ExpectationFailure& exp : ex.Errors())
    {
        errors.push_back(IdeError(exp));
    }
}

IdeErrorCollection::IdeErrorCollection(const Cm::Sym::Exception& ex)
{
    errors.push_back(IdeError(ex));
    if (ex.Referenced().Valid())
    {
        errors.push_back(IdeError(ex.Referenced()));
    }
}

IdeErrorCollection::IdeErrorCollection(const Cm::Core::Exception& ex)
{
    errors.push_back(IdeError(ex));
    if (ex.Referenced().Valid())
    {
        errors.push_back(IdeError(ex.Referenced()));
    }
}

IdeErrorCollection::IdeErrorCollection(const Cm::Core::ToolErrorExcecption& ex)
{
    errors.push_back(IdeError(ex));
}

IdeErrorCollection::IdeErrorCollection(const std::exception& ex)
{
    errors.push_back(IdeError(ex));
}

IdeErrorCollection::IdeErrorCollection(const std::string& message)
{
    errors.push_back(IdeError(message));
}

std::ostream& operator<<(std::ostream& s, const IdeErrorCollection& errorCollection)
{
    s << "[ "; 
    bool first = true;
    for (const IdeError& error : errorCollection.Errors())
    {
        if (first)
        {
            first = false;
        }
        else
        {
            s << ", ";
        }
        s << error;
    }
    return s << " ]";
}

} } // namespace Cm::Compiler