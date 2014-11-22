/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_UTIL_CODEFORMATTER_INCLUDED
#define CM_UTIL_CODEFORMATTER_INCLUDED
#include <ostream>

namespace Cm { namespace Util {

class CodeFormatter
{
public:
    CodeFormatter(std::ostream& stream_): stream(stream_), indent(0), indentSize(4), atBeginningOfLine(true) {}
    int Indent() const { return indent; }
    int IndentSize() const { return indentSize; }
    int& IndentSize() { return indentSize; }
    int CurrentIndent() const { return indentSize * indent; }
    void Write(const std::string& text);
    void WriteLine(const std::string& text);
    void NewLine();
    void WriteLine() { NewLine(); }
    void IncIndent()
    {
        ++indent;
    }
    void DecIndent()
    {
        --indent;
    }
private:
    std::ostream& stream;
    int indent;
    int indentSize;
    bool atBeginningOfLine;
};

} } // namespace Cm::Util

#endif // CM_UTIL_CODEFORMATTER_INCLUDED
