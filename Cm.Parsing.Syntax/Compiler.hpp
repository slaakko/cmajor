/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_PARSING_SYNTAX_COMPILER_INCLUDED
#define CM_PARSING_SYNTAX_COMPILER_INCLUDED

#include <string>
#include <vector>

namespace Cm { namespace Parsing { namespace Syntax {

void Compile(const std::string& projectFilePath, const std::vector<std::string>& libraryDirectories);

} } } // namespace Cm::Parsing::Syntax

#endif // CM_PARSING_SYNTAX_COMPILER_INCLUDED

