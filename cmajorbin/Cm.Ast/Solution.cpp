/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Solution.hpp>
#include <Cm.Util/Path.hpp>
#include <unordered_set>

namespace Cm { namespace Ast {

SolutionDeclaration::~SolutionDeclaration()
{
}

ProjectFileDeclaration::ProjectFileDeclaration(const Span& span_, const std::string& filePath_, const boost::filesystem::path& basePath_) : 
    span(span_), filePath(boost::filesystem::absolute(filePath_, basePath_).generic_string())
{
}

ActiveProjectDeclaration::ActiveProjectDeclaration(const std::string& activeProjectName_) : activeProjectName(activeProjectName_)
{
}

ProjectDependencyDeclaration::ProjectDependencyDeclaration(const std::string& projectName_) : projectName(projectName_)
{
}

void ProjectDependencyDeclaration::AddDependency(const std::string& dependsOn)
{
    dependsOnProjects.push_back(dependsOn);
}

Solution::Solution(const std::string& name_, const std::string& filePath_) : name(name_), filePath(filePath_), basePath(filePath)
{
    basePath.remove_filename();
}

void Solution::AddDeclaration(SolutionDeclaration* declaration)
{
    declarations.push_back(std::unique_ptr<SolutionDeclaration>(declaration));
}

void Solution::ResolveDeclarations()
{
    for (const std::unique_ptr<SolutionDeclaration>& declaration : declarations)
    {
        if (declaration->IsProjectFileDeclaration())
        {
            ProjectFileDeclaration* pfd = static_cast<ProjectFileDeclaration*>(declaration.get());
            for (const std::string& prev : projectFilePaths)
            {
                if (prev == pfd->FilePath())
                {
                    throw std::runtime_error("project file path '" + prev + "' already specified ('" + Cm::Util::GetFullPath(filePath) + "' line " + std::to_string(pfd->GetSpan().LineNumber()) + ")");
                }
            }
            projectFilePaths.push_back(pfd->FilePath());
        }
        else if (declaration->IsProjectDependencyDeclaration())
        {
            ProjectDependencyDeclaration* dependencyDeclaration = static_cast<ProjectDependencyDeclaration*>(declaration.get());
            dependencyMap[dependencyDeclaration->ProjectName()] = dependencyDeclaration;
        }
    }
}

void Solution::AddProject(std::unique_ptr<Cm::Ast::Project>&& project)
{
    projects.push_back(std::move(project));
}

void Visit(std::vector<std::string>& order, const std::string& projectName, std::unordered_set<std::string>& visited, std::unordered_set<std::string>& tempVisit, 
    const DependencyMap& dependencyMap, Solution* solution)
{
    if (tempVisit.find(projectName) == tempVisit.end())
    {
        if (visited.find(projectName) == visited.end())
        {
            tempVisit.insert(projectName);
            DependencyMapIt i = dependencyMap.find(projectName);
            if (i != dependencyMap.end())
            {
                ProjectDependencyDeclaration* dependencyDeclaration = i->second;
                for (const std::string& dependentProject : dependencyDeclaration->DependsOnProjects())
                {
                    Visit(order, dependentProject, visited, tempVisit, dependencyMap, solution);
                }
                tempVisit.erase(projectName);
                visited.insert(projectName);
                order.push_back(projectName);
            }
            else
            {
                throw std::runtime_error("project '" + projectName + "' not found in dependencies of solution '" + solution->Name() + "' (" + 
                    Cm::Util::GetFullPath(solution->FilePath()) + ")");
            }
        }
    }
    else
    {
        throw std::runtime_error("circular project dependency '" + projectName + "' detected in dependencies of solution '" + solution->Name() + "' (" + 
            Cm::Util::GetFullPath(solution->FilePath()) + ")");
    }
}

std::vector<Cm::Ast::Project*> Solution::CreateBuildOrder()
{
    std::vector<Cm::Ast::Project*> buildOrder;
    typedef std::unordered_map<std::string, Project*> ProjectMap;
    typedef ProjectMap::const_iterator ProjectMapIt;
    ProjectMap projectMap;
    for (const std::unique_ptr<Cm::Ast::Project>& project : projects)
    {
        projectMap[project->Name()] = project.get();
    }
    std::vector<std::string> order;
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> tempVisit;
    for (const std::unique_ptr<Cm::Ast::Project>& project : projects)
    {
        if (visited.find(project->Name()) == visited.end())
        {
            Visit(order, project->Name(), visited, tempVisit, dependencyMap, this);
        }
    }
    for (const std::string& projectName : order)
    {
        ProjectMapIt i = projectMap.find(projectName);
        if (i != projectMap.end())
        {
            buildOrder.push_back(i->second);
        }
        else
        {
            throw std::runtime_error("project name '" + projectName + "' not found in solution '" + Name() + "' (" + Cm::Util::GetFullPath(FilePath()) + ")");
        }
    }
    return buildOrder;
}

} } // namespace Cm::Ast
