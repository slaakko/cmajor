/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_UTIL_TOOL_ERROR_INCLUDED
#define CM_UTIL_TOOL_ERROR_INCLUDED
#include <string>

namespace Cm { namespace Util {

class ToolError
{
public:
    ToolError() : toolName(), filePath(), line(0), column(0), message() {}
    ToolError(const std::string& toolName_, const std::string& filePath_, int line_, int column_, const std::string& message_);
    const std::string& ToolName() const { return toolName; }
    const std::string& FilePath() const { return filePath; }
    int Line() const { return line; }
    int Column() const { return column; }
    const std::string& Message() const { return message; }
    bool ContainsErrorKeyword() const;
private:
    std::string toolName;
    std::string filePath;
    int line;
    int column;
    std::string message;
};

} } // namespace Cm::Util

#endif // CM_UTIL_TOOL_ERROR_INCLUDED
