/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Util/CodeFormatter.hpp>
#include <string>

namespace Cm { namespace Util {

void CodeFormatter::Write(const std::string& text)
{
    if (atBeginningOfLine)
    {
        if (indent != 0)
        {
            stream << std::string(indentSize * indent, ' ');
            atBeginningOfLine = false;
        }
    }
    stream << text;
}

void CodeFormatter::WriteLine(const std::string& text)
{
    Write(text);
    NewLine();
}

void CodeFormatter::NewLine()
{
    stream << "\n";
    atBeginningOfLine = true;
    ++line;
}

} } // namespace Cm::Util
