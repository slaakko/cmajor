/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_UTIL_TEXTUTILS_INCLUDED
#define CM_UTIL_TEXTUTILS_INCLUDED
#include <string>
#include <vector>

namespace Cm { namespace Util {

std::string Trim(const std::string& s);
std::string TrimAll(const std::string& s);
std::vector<std::string> Split(const std::string& s, char c);
std::string Replace(const std::string& s, char oldChar, char newChar);
std::string Replace(const std::string& s, const std::string& oldString, const std::string& newString);
std::string HexEscape(char c);
std::string CharStr(char c);
std::string StringStr(const std::string& s);
std::string QuotedPath(const std::string& path);
std::string ReadFile(const std::string& fileName);
bool LastComponentsEqual(const std::string& s0, const std::string& s1, char componentSeparator);
bool StartsWith(const std::string& s, const std::string& prefix);
bool EndsWith(const std::string& s, const std::string& suffix);
std::string NarrowString(const char* str, int length);
std::string ToUpper(const std::string& s);
std::string ToLower(const std::string& s);
std::string ToString(double x);
std::string ToString(double x, int maxNumDecimals);
std::string ToString(double x, int minNumDecimals, int maxNumDecimals);

} } // namespace Cm::Util

#endif //
