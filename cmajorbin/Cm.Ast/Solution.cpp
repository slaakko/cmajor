/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Solution.hpp>

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
                    throw std::runtime_error("project file path '" + prev + "' already specified ('" + filePath + "' line " + std::to_string(pfd->GetSpan().LineNumber()) + ")");
                }
            }
            projectFilePaths.push_back(pfd->FilePath());
        }
        else if (declaration->IsProjectDependencyDeclaration())
        {
            // todo
        }
    }
}

void Solution::AddProject(Project* project)
{
    projects.push_back(std::unique_ptr<Project>(project));
}

} } // namespace Cm::Ast
