/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Util/TextUtils.hpp>
#include <cctype>
#include <sstream>
#include <fstream>
#include <iterator>
#include <algorithm>

namespace Cm { namespace Util {

std::string Trim(const std::string& s)
{
    int b = 0;
    while (b < int(s.length()) && std::isspace(s[b])) ++b;
    int e = int(s.length()) - 1;
    while (e >= b && std::isspace(s[e])) --e;
    return s.substr(b, e - b + 1);
}

std::string TrimAll(const std::string& s)
{
    std::string result;
    result.reserve(s.length());
    int state = 0;
    std::string::const_iterator e = s.cend();
    for (std::string::const_iterator i = s.cbegin(); i != e; ++i)
    {
        char c = *i;
        switch (state)
        {
            case 0:
            {
                if (!std::isspace(c))
                {
                    result.append(1, c);
                    state = 1;
                }
                break;
            }
            case 1:
            {
                if (std::isspace(c))
                {
                    state = 2;
                }
                else
                {
                    result.append(1, c);
                }
                break;
            }
            case 2:
            {
                if (!std::isspace(c))
                {
                    result.append(1, ' ');
                    result.append(1, c);
                    state = 1;
                }
                break;
            }
        }
    }
    return result;
}

std::vector<std::string> Split(const std::string& s, char c)
{
    std::vector<std::string> v;
    int start = 0;
    int n = int(s.length());
    for (int i = 0; i < n; ++i)
    {
        if (s[i] == c)
        {
            v.push_back(s.substr(start, i - start));
            start = i + 1;
        }
    }
    if (start < n)
    {
        v.push_back(s.substr(start, n - start));
    }
    return v;
}

std::string Replace(const std::string& s, char oldChar, char newChar)
{
    std::string t(s);
    std::string::iterator e = t.end();
    for (std::string::iterator i = t.begin(); i != e; ++i)
    {
        if (*i == oldChar)
        {
            *i = newChar;
        }
    }
    return t;
}

std::string Replace(const std::string& s, const std::string& oldString, const std::string& newString)
{
    std::string r;
    std::string::size_type start = 0;
    std::string::size_type pos = s.find(oldString.c_str(), start);
    while (pos != std::string::npos)
    {
        r.append(s.substr(start, pos - start));
        r.append(newString);
        start = pos + oldString.length();
        pos = s.find(oldString.c_str(), start);
    }
    r.append(s.substr(start, s.length() - start));
    return r;
}

std::string HexEscape(char c)
{
    std::stringstream s;
    s << "\\x" << std::hex << int(c);
    return s.str();
}

std::string CharStr(char c)
{
    switch (c)
    {
        case '\'': return "\\'";
        case '\"': return "\\\"";
        case '\\': return "\\\\";
        case '\a': return "\\a";
        case '\b': return "\\b";
        case '\f': return "\\f";
        case '\n': return "\\n";
        case '\r': return "\\r";
        case '\t': return "\\t";
        case '\v': return "\\v";
        case '\0': return "\\0";
        default:
        {
            if (c >= 32 && c <= 126)
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

std::string StringStr(const std::string& s)
{
    std::string r;
    int n = int(s.length());
    for (int i = 0; i < n; ++i)
    {
        r.append(CharStr(s[i]));
    }
    return r;
}

std::string QuotedPath(const std::string& path)
{
    if (path.find(' ') != std::string::npos)
    {
        return std::string("\"") + path + "\"";
    }
    return path;
}

std::string ReadFile(const std::string& fileName)
{
    std::string content;
    content.reserve(4096);
    std::ifstream file(fileName.c_str());
    if (!file)
    {
        throw std::runtime_error("Error: Could not open input file: " + fileName);
    }
    file.unsetf(std::ios::skipws);
    std::copy(
        std::istream_iterator<char>(file),
        std::istream_iterator<char>(),
        std::back_inserter(content));
    int start = 0;
    if (content.size() >= 3)
    {
        if ((unsigned char)content[0] == (unsigned char)0xEF) ++start;
        if ((unsigned char)content[1] == (unsigned char)0xBB) ++start;
        if ((unsigned char)content[2] == (unsigned char)0xBF) ++start;
    }
    return start == 0 ? content : content.substr(start);
}

bool LastComponentsEqual(const std::string& s0, const std::string& s1, char componentSeparator)
{
    std::vector<std::string> c0 = Split(s0, componentSeparator);
    std::vector<std::string> c1 = Split(s1, componentSeparator);
    int n0 = int(c0.size());
    int n1 = int(c1.size());
    int n = std::min(n0, n1);
    for (int i = 0; i < n; ++i)
    {
        if (c0[n0 - i - 1] != c1[n1 - i - 1]) return false;
    }
    return true;
}

bool StartsWith(const std::string& s, const std::string& prefix)
{
    int n = int(prefix.length());    
    return int(s.length()) >= n && s.substr(0, n) == prefix;
}

bool EndsWith(const std::string& s, const std::string& suffix)
{
    int n = int(suffix.length());
    int m = int(s.length());
    return m >= n && s.substr(m - n, n) == suffix;
}

std::string NarrowString(const char* str, int length)
{
#if defined(__linux) || defined(__posix) || defined(__unix)
    return std::string(str, length);
#elif defined(WIN32)
    std::string narrow;
    narrow.reserve(length);
    int state = 0;
    for (int i = 0; i < length; ++i)
    {
        char c = str[i];
        switch (state)
        {
            case 0:
            {
                if (c == '\r') state = 1; else narrow.append(1, c);
                break;
            }
            case 1:
            {
                if (c == '\n') narrow.append(1, '\n'); else narrow.append(1, '\r').append(1, c);
                state = 0;
                break;
            }
        }
    }
    return narrow;
#else
    #error unknown platform
#endif
}

} } // namespace Cm::Util
