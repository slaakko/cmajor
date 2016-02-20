/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Parser/FileRegistry.hpp>
#include <iostream>
#include <mutex>

namespace Cm { namespace Parser {

std::mutex mtx;

int FileRegistry::RegisterParsedFile(const std::string& filePath)
{
    std::lock_guard<std::mutex> lock(mtx);
    int fileIndex = int(parsedFiles.size());
    parsedFiles.push_back(filePath);
    return fileIndex;
}

const std::string& FileRegistry::GetParsedFileName(int parsedFileIndex) const
{
    std::lock_guard<std::mutex> lock(mtx);
    static std::string emptyFileName;
    if (parsedFileIndex >= 0 && parsedFileIndex < int(parsedFiles.size()))
    {
        return parsedFiles[parsedFileIndex];
    }
    return emptyFileName;
}

int FileRegistry::GetNumberOfParsedFiles() const
{
    std::lock_guard<std::mutex> lock(mtx);
    return int(parsedFiles.size());
}

void FileRegistry::Init()
{
    instance.reset(new FileRegistry());
}

void FileRegistry::Done()
{
    instance.reset();
}

FileRegistry* FileRegistry::Instance()
{
    return instance.get();
}

std::unique_ptr<FileRegistry> FileRegistry::instance;

} } // namespace Cm::Parser
