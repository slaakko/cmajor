/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Parser/FileRegistry.hpp>

namespace Cm { namespace Parser {

int FileRegistry::RegisterParsedFile(const std::string& filePath)
{
    int fileIndex = int(parsedFiles.size());
    parsedFiles.push_back(filePath);
    return fileIndex;
}

} } // namespace Cm::Parser