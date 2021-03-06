/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Parsing/Utility.hpp>
#include <sstream>
#include <cctype>

namespace Cm { namespace Parsing {

std::string HexEscape(char c)
{
    std::stringstream s;
    s << "\\x" << std::hex << (unsigned int)(unsigned char)c;
    return s.str();
}

std::string XmlCharStr(char c)
{
    switch (c)
    {
        case '&': return "&amp;";
        case '<': return "&lt;";
        case '>': return "&gt;";
        case '\a': return "\\a";
        case '\b': return "\\b";
        case '\f': return "\\f";
        case '\n': return "\\n";
        case '\r': return "\\r";
        case '\t': return "\\t";
        case '\v': return "\\v";
        default:
        {
            if (((int)c >= 32 && (int)c <= 126) || std::isalnum(c) || std::ispunct(c))
            {
                return std::string(1, c);
            }
            else
            {
                return HexEscape(c);
            }
        }
    }
}

std::string XmlEscape(const std::string& s)
{
    std::string result;
    result.reserve(2 * s.length());
    std::string::const_iterator e = s.end();
    for (std::string::const_iterator i = s.begin(); i != e; ++i)
    {
        result.append(XmlCharStr(*i));
    }
    return result;
}

} } // namespace Cm::Parsing
