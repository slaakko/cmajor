/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_SOLUTION_INCLUDED
#define CM_AST_SOLUTION_INCLUDED
#include <Cm.Ast/Project.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <unordered_map>

namespace Cm { namespace Ast {

class SolutionDeclaration
{
public:
    virtual ~SolutionDeclaration();
    virtual bool IsProjectFileDeclaration() const { return false; }
    virtual bool IsProjectDependencyDeclaration() const { return false; }
};

class ProjectFileDeclaration : public SolutionDeclaration
{
public:
    ProjectFileDeclaration(const Span& span_, const std::string& filePath_, const boost::filesystem::path& basePath_);
    virtual bool IsProjectFileDeclaration() const { return true; }
    const Span& GetSpan() const { return span; }
    const std::string& FilePath() const { return filePath; }
private:
    Span span;
    std::string filePath;
};

class ActiveProjectDeclaration : public SolutionDeclaration
{
public:
    ActiveProjectDeclaration(const std::string& activeProjectName_);
private:
    std::string activeProjectName;
};

class ProjectDependencyDeclaration : public SolutionDeclaration
{
public:
    ProjectDependencyDeclaration(const std::string& projectName_);
    void AddDependency(const std::string& dependsOn);
    virtual bool IsProjectDependencyDeclaration() const { return true; }
    const std::string& ProjectName() const { return projectName; }
    const std::vector<std::string>& DependsOnProjects() const { return dependsOnProjects; }
private:
    std::string projectName;
    std::vector<std::string> dependsOnProjects;
};

typedef std::unordered_map<std::string, ProjectDependencyDeclaration*> DependencyMap;
typedef DependencyMap::const_iterator DependencyMapIt;

class Solution
{
public:
    Solution(const std::string& name_, const std::string& filePath_);
    const std::string& Name() const { return name; }
    const std::string& FilePath() const { return filePath; }
    const boost::filesystem::path& BasePath() const { return basePath; }
    void AddDeclaration(SolutionDeclaration* declaration);
    void ResolveDeclarations();
    const std::vector<std::string>& ProjectFilePaths() const { return projectFilePaths; }
    void AddProject(std::unique_ptr<Cm::Ast::Project>&& project);
    std::vector<Cm::Ast::Project*> CreateBuildOrder();
private:
    std::string name;
    std::string filePath;
    boost::filesystem::path basePath;
    std::vector<std::unique_ptr<SolutionDeclaration>> declarations;
    std::vector<std::string> projectFilePaths;
    std::vector<std::unique_ptr<Cm::Ast::Project>> projects;
    DependencyMap dependencyMap;
};

} } // namespace Cm::Ast

#endif // CM_AST_SOLUTION_INCLUDED
