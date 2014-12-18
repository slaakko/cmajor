/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_PROJECT_INCLUDED
#define CM_AST_PROJECT_INCLUDED
#include <Cm.Ast/CompileUnit.hpp>
#include <boost/filesystem.hpp>
#include <map>

namespace Cm { namespace Ast {

class Visitor;

enum class SourceFileType
{
    cm, asm_, c, text
};

enum class Target
{
    none, program, library
};

std::string TargetStr(Target target);

class Properties
{
public:
    void AddProperty(const std::string& name_, const std::string& value_);
    std::string GetProperty(const std::string& name_) const;
private:
    std::map<std::string, std::string> propertyMap;
};

class ProjectDeclaration
{
public:
    ProjectDeclaration(const Span& span_);
    virtual ~ProjectDeclaration();
    virtual bool IsSourceFileDeclaration() const { return false; }
    virtual bool IsTargetDeclaration() const { return false; }
    virtual bool IsAssemblyFileDeclaration() const { return false; }
    virtual bool IsExecutableFileDeclaration() const { return false; }
    virtual bool IsReferenceFileDeclaration() const { return false; }
    const Span& GetSpan() const { return span; }
private:
    Span span;
};

class SourceFileDeclaration : public ProjectDeclaration
{
public:
    SourceFileDeclaration(const Span& span_, SourceFileType fileType_, const std::string& filePath_, const boost::filesystem::path& basePath_, const Properties& properties_);
    SourceFileType FileType() const { return fileType; }
    const std::string& FilePath() const { return filePath; }
    virtual bool IsSourceFileDeclaration() const { return true; }
private:
    SourceFileType fileType;
    std::string filePath;
    Properties properties;
};

class ReferenceFileDeclaration : public ProjectDeclaration
{
public:
    ReferenceFileDeclaration(const Span& span_, const std::string& filePath_, const boost::filesystem::path& basePath_, const Properties& properties_);
    const std::string& FilePath() const { return filePath; }
    virtual bool IsReferenceFileDeclaration() const { return true; }
private:
    std::string filePath;
    Properties properties;
};

class AssemblyFileDeclaration : public ProjectDeclaration
{
public:
    AssemblyFileDeclaration(const Span& span_, const std::string& filePath_, const boost::filesystem::path& outputBasePath_);
    const std::string& FilePath() const { return filePath; }
    virtual bool IsAssemblyFileDeclaration() const { return true; }
private:
    std::string filePath;
};

class ExecutableFileDeclaration : public ProjectDeclaration
{
public:
    ExecutableFileDeclaration(const Span& span_, const std::string& filePath_, const boost::filesystem::path& outputBasePath_);
    const std::string& FilePath() const { return filePath; }
    virtual bool IsExecutableFileDeclaration() const { return true; }
private:
    std::string filePath;
};

class CLibraryDeclaration : public ProjectDeclaration
{
public:
    CLibraryDeclaration(const Span& span_, const std::string& filePath_, const Properties& properties_);
    const std::string& FilePath() const { return filePath; }
private:
    std::string filePath;
    Properties properties;
};

class TargetDeclaration : public ProjectDeclaration
{
public:
    TargetDeclaration(const Span& span_, Target target_);
    Target GetTarget() const { return target; }
    virtual bool IsTargetDeclaration() const { return true; }
private:
    Target target;
};

class Project
{
public:
    Project(const std::string& name_, const std::string& filePath_, const std::string& config_, const std::string& backend_);
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
    const std::vector<std::string>& TextFilePaths() const { return textFilePaths; }
    const std::vector<std::string>& ReferenceFilePaths() const { return referenceFilePaths; }
    const std::string& AssemblyFilePath() const { return assemblyFilePath; }
    const std::string& LibraryFilePath() const { return libraryFilePath; }
    const std::string& ExecutableFilePath() const { return executableFilePath; }
private:
    std::string name;
    std::string filePath;
    boost::filesystem::path basePath;
    boost::filesystem::path outputBasePath;
    std::string config;
    std::string backend;
    Target target;
    std::vector<std::unique_ptr<ProjectDeclaration>> declarations;
    std::vector<std::string> sourceFilePaths;
    std::vector<std::string> asmSourceFilePaths;
    std::vector<std::string> cSourceFilePaths;
    std::vector<std::string> textFilePaths;
    std::vector<std::string> referenceFilePaths;
    std::string assemblyFilePath;
    std::string libraryFilePath;
    std::string executableFilePath;
};

} } // namespace Cm::Ast

#endif // CM_AST_PROJECT_INCLUDED
