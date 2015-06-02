/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_COMPILER_IDE_ERROR_INCLUDED
#define CM_COMPILER_IDE_ERROR_INCLUDED
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Sym/Exception.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Core/CDebugInfo.hpp>
#include <ostream>
#include <stdexcept>

namespace Cm { namespace Compiler {  

class IdeError
{
public:
    IdeError(const Cm::Parsing::ExpectationFailure& ex);
    IdeError(const Cm::Sym::Exception& ex);
    IdeError(const Cm::Core::Exception& ex);
    IdeError(const Cm::Core::ToolErrorExcecption& ex);
    IdeError(const std::exception& ex);
    IdeError(const std::string& message);
    IdeError(const Cm::Parsing::Span& reference);
    const std::string& Tool() const { return tool; }
    const std::string& Project() const { return project; }
    const std::string& Description() const { return description; }
    const std::string& File() const { return file; }
    int Line() const { return sourceSpan.Line(); }
    int StartColumn() const { return sourceSpan.StartCol(); }
    int EndColumn() const { return sourceSpan.EndCol(); }
private:
    std::string tool;
    std::string project;
    std::string description;
    std::string file;
    Cm::Core::SourceSpan sourceSpan;
};

std::ostream& operator<<(std::ostream& s, const IdeError& error);

class IdeErrorCollection
{
public:
    IdeErrorCollection(const Cm::Parsing::CombinedParsingError& ex);
    IdeErrorCollection(const Cm::Sym::Exception& ex);
    IdeErrorCollection(const Cm::Core::Exception& ex);
    IdeErrorCollection(const Cm::Core::ToolErrorExcecption& ex);
    IdeErrorCollection(const std::exception& ex);
    IdeErrorCollection(const std::string& message);
    const std::vector<IdeError>& Errors() const { return errors; }
private:
    std::vector<IdeError> errors;
};

std::ostream& operator<<(std::ostream& s, const IdeErrorCollection& errorCollection);

} } // namespace Cm::Compiler

#endif // CM_COMPILER_IDE_ERROR_INCLUDED
