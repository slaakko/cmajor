/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_EMIT_SOURCE_FILE_CACHE_INCLUDED
#define CM_EMIT_SOURCE_FILE_CACHE_INCLUDED
#include <Cm.Util/MappedInputFile.hpp>
#include <unordered_map>
#include <memory>
#include <chrono>

namespace Cm { namespace Emit {

class SourceFile
{
public:
    SourceFile();
    SourceFile(const std::string& filePath_);
    const std::string& FilePath() const { return filePath; }
    const char* Begin() const { return file->Begin(); }
    const char* End() const { return file->End(); }
    void Use();
    std::chrono::time_point<std::chrono::system_clock> Used() const { return used; }
private:
    std::string filePath;
    std::unique_ptr<Cm::Util::MappedInputFile> file;
    std::chrono::time_point<std::chrono::system_clock> used;
};

class SourceFileCache
{
public:
    SourceFileCache();
    SourceFile& GetSourceFile(const std::string& filePath);
    int MaxCachedFiles() const { return maxCachedFiles; }
    void SetMaxCachedFiles(int maxCachedFiles_) { maxCachedFiles = maxCachedFiles_; }
    static void Init();
    static void Done();
    static SourceFileCache& Instance();
private:
    int maxCachedFiles;
    typedef std::unordered_map<std::string, SourceFile> FileMap;
    typedef FileMap::iterator FileMapIt;
    FileMap fileMap;
    static std::unique_ptr<SourceFileCache> instance;
};

void SourceFileCacheInit();
void SourceFileCacheDone();

} } // namespace Cm::Emit

#endif // CM_EMIT_SOURCE_FILE_CACHE_INCLUDED
