/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Emit/SourceFileCache.hpp>
#include <limits>

namespace Cm { namespace Emit {

SourceFile::SourceFile() : used(std::chrono::system_clock::now())
{
}

SourceFile::SourceFile(const std::string& filePath_) : filePath(filePath_), file(new Cm::Util::MappedInputFile(filePath)), used(std::chrono::system_clock::now())
{
}

void SourceFile::Use()
{
    used = std::chrono::system_clock::now();
}

SourceFileCache::SourceFileCache() : maxCachedFiles(64)
{
}

SourceFile& SourceFileCache::GetSourceFile(const std::string& filePath)
{
    FileMapIt i = fileMap.find(filePath);
    if (i != fileMap.end())
    {
        SourceFile& sourceFile = i->second;
        sourceFile.Use();
        return sourceFile;
    }
    if (int(fileMap.size()) >= maxCachedFiles)
    {
        std::chrono::time_point<std::chrono::system_clock> minTp = std::chrono::time_point<std::chrono::system_clock>::max();
        FileMapIt leastRecentlyUsed = fileMap.end();
        FileMapIt e = fileMap.end();
        for (FileMapIt i = fileMap.begin(); i != e; ++i)
        {
            const SourceFile& sourceFile = i->second;
            if (sourceFile.Used() < minTp)
            {
                minTp = sourceFile.Used();
                leastRecentlyUsed = i;
            }
        }
        fileMap.erase(leastRecentlyUsed);
    }
    fileMap[filePath] = SourceFile(filePath);
    return fileMap[filePath];
}

std::unique_ptr<SourceFileCache> SourceFileCache::instance;

void SourceFileCache::Init()
{
    instance.reset(new SourceFileCache());
}

void SourceFileCache::Done()
{
    instance.reset();
}

SourceFileCache& SourceFileCache::Instance()
{
    return *instance;
}

void SourceFileCacheInit()
{
    SourceFileCache::Init();
}

void SourceFileCacheDone()
{
    SourceFileCache::Done();
}

} } // namespace Cm::Emit
