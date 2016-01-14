/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_PROJECT_INCLUDED
#define CM_AST_PROJECT_INCLUDED
#include <Cm.Ast/CompileUnit.hpp>
#include <boost/filesystem.hpp>
#include <map>
#include <utility>

namespace Cm { namespace Ast {

class Visitor;

enum class SourceFileType
{
    cm, asm_, c, cpp, text
};

enum class Target
{
    none, program, library
};

std::string TargetStr(Target target);

class Properties
{
public:
    Properties();
    void AddProperty(const std::string& name_, const std::string& value_);
    std::string GetProperty(const std::string& name_) const;
private:
    std::map<std::string, std::string> propertyMap;
};

class ProjectDeclaration
{
public:
    ProjectDeclaration(const Span& span_, const Properties& properties_);
    virtual ~ProjectDeclaration();
    virtual bool IsSourceFileDeclaration() const { return false; }
    virtual bool IsTargetDeclaration() const { return false; }
    virtual bool IsStackSizeDeclaration() const { return false; }
    virtual bool IsAssemblyFileDeclaration() const { return false; }
    virtual bool IsExecutableFileDeclaration() const { return false; }
    virtual bool IsAddLibraryPathDeclaration() const { return false; }
    virtual bool IsReferenceFileDeclaration() const { return false; }
    virtual bool IsCLibraryDeclaration() const { return false; }
    const Span& GetSpan() const { return span; }
    const Properties& GetProperties() const { return properties; }
private:
    Span span;
    Properties properties;
};

class SourceFileDeclaration : public ProjectDeclaration
{
public:
    SourceFileDeclaration(const Span& span_, SourceFileType fileType_, const std::string& filePath_, const boost::filesystem::path& basePath_, const Properties& properties_);
    SourceFileType FileType() const { return fileType; }
    const std::string& FilePath() const { return filePath; }
    bool IsSourceFileDeclaration() const override { return true; }
private:
    SourceFileType fileType;
    std::string filePath;
};

class ReferenceFileDeclaration : public ProjectDeclaration
{
public:
    ReferenceFileDeclaration(const Span& span_, const std::string& filePath_, const boost::filesystem::path& basePath_, const Properties& properties_);
    const std::string& FilePath() const { return filePath; }
    bool IsReferenceFileDeclaration() const override { return true; }
private:
    std::string filePath;
};

class AssemblyFileDeclaration : public ProjectDeclaration
{
public:
    AssemblyFileDeclaration(const Span& span_, const std::string& filePath_, const boost::filesystem::path& outputBasePath_, const Properties& properties_);
    const std::string& FilePath() const { return filePath; }
    bool IsAssemblyFileDeclaration() const override { return true; }
private:
    std::string filePath;
};

class ExecutableFileDeclaration : public ProjectDeclaration
{
public:
    ExecutableFileDeclaration(const Span& span_, const std::string& filePath_, const boost::filesystem::path& outputBasePath_);
    const std::string& FilePath() const { return filePath; }
    bool IsExecutableFileDeclaration() const override { return true; }
private:
    std::string filePath;
};

class AddLibraryPathDeclaration : public ProjectDeclaration
{
public:
    AddLibraryPathDeclaration(const Span& span_, const std::string& filePath_, const boost::filesystem::path& basePath_, const Properties& properties_);
    const std::string& FilePath() const { return filePath; }
    bool IsAddLibraryPathDeclaration() const override { return true; }
private:
    std::string filePath;
};

class CLibraryDeclaration : public ProjectDeclaration
{
public:
    CLibraryDeclaration(const Span& span_, const std::string& filePath_, const Properties& properties_);
    const std::string& FilePath() const { return filePath; }
    bool IsCLibraryDeclaration() const override { return true; }
private:
    std::string filePath;
};

class TargetDeclaration : public ProjectDeclaration
{
public:
    TargetDeclaration(const Span& span_, Target target_);
    Target GetTarget() const { return target; }
    bool IsTargetDeclaration() const override { return true; }
private:
    Target target;
};

class StackSizeDeclaration : public ProjectDeclaration
{
public:
    StackSizeDeclaration(const Span& span_, uint64_t stackSize_);
    uint64_t StackSize() const { return stackSize; }
    bool IsStackSizeDeclaration() const override { return true; }
private:
    uint64_t stackSize;
};

class Project
{
public:
    Project(const std::string& name_, const std::string& filePath_, const std::string& config_, const std::string& backend_, const std::string& os_, int bits_);
    void AddDeclaration(ProjectDeclaration* declaration);
    void ResolveDeclarations();
    const std::string& Name() const { return name; }
    const std::string& FilePath() const { return filePath; }
    const boost::filesystem::path& BasePath() const { return basePath; }
    const boost::filesystem::path& OutputBasePath() const { return outputBasePath; }
    Target GetTarget() const { return target; }
    const std::vector<std::string>& SourceFilePaths() const { return sourceFilePaths; }
    const std::vector<std::string>& AsmSourceFilePaths() const { return asmSourceFilePaths; }
    const std::vector<std::string>& CSourceFilePaths() const { return cSourceFilePaths; }
    const std::vector<std::string>& CppSourceFilePaths() const { return cppSourceFilePaths; }
    const std::vector<std::string>& TextFilePaths() const { return textFilePaths; }
    const std::vector<std::string>& ReferenceFilePaths() const { return referenceFilePaths; }
    const std::vector<std::string>& LibrarySearchPaths() const { return librarySearchPaths; }
    void AddReferenceFilePath(const std::string& referenceFilePath) { referenceFilePaths.push_back(referenceFilePath); }
    const std::vector<std::string>& CLibraryFilePaths() const { return cLibraryFilePaths; }
    const std::string& AssemblyFilePath() const { return assemblyFilePath; }
    const std::string& LibraryFilePath() const { return libraryFilePath; }
    const std::string& ExecutableFilePath() const { return executableFilePath; }
    uint64_t StackSize() const { return stackSize; }
private:
    std::string name;
    std::string filePath;
    boost::filesystem::path basePath;
    boost::filesystem::path outputBasePath;
    std::string config;
    std::string backend;
    std::string os;
    int bits;
    Target target;
    uint64_t stackSize;
    std::vector<std::unique_ptr<ProjectDeclaration>> declarations;
    std::vector<std::string> sourceFilePaths;
    std::vector<std::string> asmSourceFilePaths;
    std::vector<std::string> cSourceFilePaths;
    std::vector<std::string> cppSourceFilePaths;
    std::vector<std::string> textFilePaths;
    std::vector<std::string> referenceFilePaths;
    std::vector<std::string> cLibraryFilePaths;
    std::vector<std::string> librarySearchPaths;
    std::string assemblyFilePath;
    std::string libraryFilePath;
    std::string executableFilePath;
};

class ProgramVersion
{
public:
    ProgramVersion();
    ProgramVersion(int major_, int minor_, int revision_, int build_, const std::string& versionText_);
    int Major() const { return major; }
    int Minor() const { return minor; }
    int Revision() const { return revision; }
    int Build() const { return build; }
    const std::string& VersionText() const { return versionText; }
    std::string ToString() const;
private:
    int major;
    int minor;
    int revision;
    int build;
    std::string versionText;
};

bool operator==(const ProgramVersion& left, const ProgramVersion& right);
bool operator<(const ProgramVersion& left, const ProgramVersion& right);

inline bool operator!=(const ProgramVersion& left, const ProgramVersion& right)
{
    return std::rel_ops::operator!=(left, right);
}

inline bool operator>(const ProgramVersion& left, const ProgramVersion& right)
{
    return std::rel_ops::operator>(left, right);
}

inline bool operator>=(const ProgramVersion& left, const ProgramVersion& right)
{
    return std::rel_ops::operator>=(left, right);
}

inline bool operator<=(const ProgramVersion& left, const ProgramVersion& right)
{
    return std::rel_ops::operator<=(left, right);
}

} } // namespace Cm::Ast

#endif // CM_AST_PROJECT_INCLUDED
