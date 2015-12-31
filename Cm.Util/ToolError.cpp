/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Util/ToolError.hpp>

namespace Cm { namespace Util {

ToolError::ToolError(const std::string& toolName_, const std::string& filePath_, int line_, int column_, const std::string& message_) :
    toolName(toolName_), filePath(filePath_), line(line_), column(column_), message(message_)
{
}

bool ToolError::ContainsErrorKeyword() const
{
    return message.find("error") != std::string::npos;
}

} } // namespace Cm::Util
