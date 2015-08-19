/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Compiler/IdeError.hpp>
#include <Cm.Core/GlobalSettings.hpp>
#include <Cm.Emit/SourceFileCache.hpp>
#include <Cm.Parser/FileRegistry.hpp>
#include <Cm.Util/TextUtils.hpp>

namespace Cm { namespace Compiler {

std::string GetFilePath(int fileIndex)
{
    return Cm::Parser::FileRegistry::Instance()->GetParsedFileName(fileIndex);
}

Cm::Core::SourceSpan GetSourceSpan(const Cm::Parsing::Span& fromSpan)
{
    if (!fromSpan.Valid()) return Cm::Core::SourceSpan();
    std::string filePath = GetFilePath(fromSpan.FileIndex());
    Cm::Emit::SourceFile& file = Cm::Emit::SourceFileCache::Instance().GetSourceFile(filePath);
    const char* start = file.Begin();
    const char* end = file.End();
    return Cm::Core::FromSpan(start, end, fromSpan);
}

IdeError::IdeError(const Cm::Parsing::ExpectationFailure& ex) :
    tool("cmc.syntax"), category("error"), project(Cm::Core::GetGlobalSettings()->CurrentProjectName()), description(ex.Message()), file(ex.FileName()), sourceSpan(GetSourceSpan(ex.GetSpan()))
{
}

IdeError::IdeError(const Cm::Sym::Exception& ex) :
    tool("cmc.symbols"), category("error"), project(Cm::Core::GetGlobalSettings()->CurrentProjectName()), description(ex.Message()), file(GetFilePath(ex.Defined().FileIndex())),
    sourceSpan(GetSourceSpan(ex.Defined()))
{
}

IdeError::IdeError(const Cm::Core::Exception& ex) :
    tool("cmc.bind"), category("error"), project(Cm::Core::GetGlobalSettings()->CurrentProjectName()), description(ex.Message()), file(GetFilePath(ex.Defined().FileIndex())),
    sourceSpan(GetSourceSpan(ex.Defined()))
{
}

IdeError::IdeError(const Cm::Core::ToolErrorExcecption& ex) :
    tool(ex.Error().ToolName()), category("error"), project(Cm::Core::GetGlobalSettings()->CurrentProjectName()), description(ex.Error().Message()), file(ex.Error().FilePath()),
    sourceSpan(ex.Error().Line(), ex.Error().Column(), ex.Error().Column())
{
}

IdeError::IdeError(const std::exception& ex) :
    tool("cmc.internal"), category("error"), project(Cm::Core::GetGlobalSettings()->CurrentProjectName()), description(ex.what()), file(), sourceSpan()
{
}

IdeError::IdeError(const std::string& message) :
    tool("cmc.internal"), category("error"), project(Cm::Core::GetGlobalSettings()->CurrentProjectName()), description(message), file(), sourceSpan()
{
}

IdeError::IdeError(const Cm::Parsing::Span& reference) :
    tool("cmc.reference"), category("info"), project(""), description("see reference to:"), file(GetFilePath(reference.FileIndex())), sourceSpan(GetSourceSpan(reference))
{
}

IdeError::IdeError(const Cm::Sym::Warning& warning) : 
    tool("cmc"), category("warning"), project(warning.Project()), description(warning.Message()), file(GetFilePath(warning.Defined().FileIndex())), sourceSpan(GetSourceSpan(warning.Defined()))
{
}


std::ostream& operator<<(std::ostream& s, const IdeError& error)
{
    return s << "{ \"tool\" : \"" << error.Tool() << "\", \"category\" : \"" << error.Category() << "\", \"project\" : \"" << error.Project() << "\", \"description\" : \"" << 
        Cm::Util::StringStr(error.Description()) << "\", \"file\" : \"" << error.File() << "\", " << "\"line\" : " << error.Line() << ", \"startColumn\" : " << 
        error.StartColumn() << ", \"endColumn\" : " << error.EndColumn() << " }";
}

IdeErrorCollection::IdeErrorCollection()
{
}

IdeErrorCollection::IdeErrorCollection(const Cm::Parsing::ExpectationFailure& ex)
{
    errors.push_back(IdeError(ex));
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

void IdeErrorCollection::AddWarnings(const std::vector<Cm::Sym::Warning>& warnings)
{
    for (const Cm::Sym::Warning& warning : warnings)
    {
        IdeError error(warning);
        errors.push_back(error);
        if (warning.Referenced().Valid())
        {
            errors.push_back(IdeError(warning.Referenced()));
        }
    }
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
