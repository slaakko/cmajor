/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_PARSER_FILE_REGISTRY
#define CM_PARSER_FILE_REGISTRY
#include <string>
#include <vector>

namespace Cm { namespace Parser {

class FileRegistry
{
public:
    int RegisterParsedFile(const std::string& filePath);
    const std::string& GetParsedFileName(int parsedFileIndex) const;
    int GetNumberOfParsedFiles() const { return int(parsedFiles.size()); }
private:
    std::vector<std::string> parsedFiles;
};

FileRegistry* GetCurrentFileRegistry();
void SetCurrentFileRegistry(FileRegistry* fileRegistry);

} } // namespace Cm::Parser

#endif // CM_PARSER_FILE_REGISTRY
