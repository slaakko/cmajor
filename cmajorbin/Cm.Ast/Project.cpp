/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Project.hpp>
#include <Cm.Util/Path.hpp>

namespace Cm { namespace Ast {

std::string TargetStr(Target target)
{
    switch (target)
    {
        case Target::none: return "none";
        case Target::program: return "program";
        case Target::library: return "library";
    }
    return "";
}

ProjectDeclaration::ProjectDeclaration(const Span& span_, const Properties& properties_) : span(span_), properties(properties_)
{
}

ProjectDeclaration::~ProjectDeclaration()
{
}

SourceFileDeclaration::SourceFileDeclaration(const Span& span_, SourceFileType fileType_, const std::string& filePath_, const boost::filesystem::path& basePath_, const Properties& properties_) :
    ProjectDeclaration(span_, properties_), fileType(fileType_), filePath(boost::filesystem::absolute(filePath_, basePath_).generic_string())
{
}

ReferenceFileDeclaration::ReferenceFileDeclaration(const Span& span_, const std::string& filePath_, const boost::filesystem::path& basePath_, const Properties& properties_) :
    ProjectDeclaration(span_, properties_), filePath(boost::filesystem::absolute(filePath_, basePath_).generic_string())
{
}

AssemblyFileDeclaration::AssemblyFileDeclaration(const Span& span_, const std::string& filePath_, const boost::filesystem::path& outputBasePath_, const Properties& properties_) :
    ProjectDeclaration(span_, properties_), filePath(boost::filesystem::absolute(filePath_, outputBasePath_).generic_string())
{
}

ExecutableFileDeclaration::ExecutableFileDeclaration(const Span& span_, const std::string& filePath_, const boost::filesystem::path& outputBasePath_) :
    ProjectDeclaration(span_, Properties()), filePath(boost::filesystem::absolute(filePath_, outputBasePath_).generic_string())
{
}

CLibraryDeclaration::CLibraryDeclaration(const Span& span_, const std::string& filePath_, const Properties& properties_) : ProjectDeclaration(span_, properties_), filePath(filePath_)
{
}

TargetDeclaration::TargetDeclaration(const Span& span_, Target target_) : ProjectDeclaration(span_, Properties()), target(target_)
{
}

Properties::Properties()
{
}

void Properties::AddProperty(const std::string& name_, const std::string& value_)
{
    propertyMap[name_] = value_;
}

std::string Properties::GetProperty(const std::string& name_) const
{
    std::map<std::string, std::string>::const_iterator i = propertyMap.find(name_);
    if (i != propertyMap.cend())
    {
        return i->second;
    }
    return std::string();
}

Project::Project(const std::string& name_, const std::string& filePath_, const std::string& config_, const std::string& backend_, const std::string& os_) :
    name(name_), filePath(filePath_), basePath(filePath), config(config_), backend(backend_), os(os_), target(Target::none)
{
    basePath.remove_filename();
    outputBasePath = basePath;
    outputBasePath /= config;
    outputBasePath /= backend;
}

void Project::ResolveDeclarations()
{
    for (const std::unique_ptr<ProjectDeclaration>& declaration : declarations)
    {
        const Properties& properties = declaration->GetProperties();
        std::string declBackEnd = properties.GetProperty("backend");
        if (!declBackEnd.empty())
        {
            if (declBackEnd != backend) continue;
        }
        std::string declOs = properties.GetProperty("os");
        if (!declOs.empty())
        {
            if (declOs != os) continue;
        }
        if (declaration->IsSourceFileDeclaration())
        {
            SourceFileDeclaration* sfd = static_cast<SourceFileDeclaration*>(declaration.get());
            switch (sfd->FileType())
            {
                case SourceFileType::cm: 
                { 
                    for (const std::string& prev : sourceFilePaths)
                    {
                        if (prev == sfd->FilePath())
                        {
                            throw std::runtime_error("source file '" + prev + "' already specified ('" + Cm::Util::GetFullPath(FilePath()) + "' line " + std::to_string(sfd->GetSpan().LineNumber()) + ")");
                        }
                    }
                    sourceFilePaths.push_back(sfd->FilePath());
                    break;
                }
                case SourceFileType::asm_:
                {
                    for (const std::string& prev : asmSourceFilePaths)
                    {
                        if (prev == sfd->FilePath())
                        {
                            throw std::runtime_error("assembly source file '" + prev + "' already specified ('" + Cm::Util::GetFullPath(FilePath()) + "' line " + std::to_string(sfd->GetSpan().LineNumber()) + ")");
                        }
                    }
                    asmSourceFilePaths.push_back(sfd->FilePath());
                    break;
                }
                case SourceFileType::c:
                {
                    for (const std::string& prev : cSourceFilePaths)
                    {
                        if (prev == sfd->FilePath())
                        {
                            throw std::runtime_error("C source file '" + prev + "' already specified ('" + Cm::Util::GetFullPath(FilePath()) + "' line " + std::to_string(sfd->GetSpan().LineNumber()) + ")");
                        }
                    }
                    cSourceFilePaths.push_back(sfd->FilePath());
                    break;
                }
                case SourceFileType::text:
                {
                    for (const std::string& prev : textFilePaths)
                    {
                        if (prev == sfd->FilePath())
                        {
                            throw std::runtime_error("text file '" + prev + "' already specified ('" + Cm::Util::GetFullPath(FilePath()) + "' line " + std::to_string(sfd->GetSpan().LineNumber()) + ")");
                        }
                    }
                    textFilePaths.push_back(sfd->FilePath());
                    break;
                }
            }
        }
        else if (declaration->IsReferenceFileDeclaration())
        {
            ReferenceFileDeclaration* rfd = static_cast<ReferenceFileDeclaration*>(declaration.get());
            for (const std::string& prev : referenceFilePaths)
            {
                if (prev == rfd->FilePath())
                {
                    throw std::runtime_error("reference file '" + prev + "' already specified ('" + Cm::Util::GetFullPath(FilePath()) + "' line " + std::to_string(rfd->GetSpan().LineNumber()) + ")");
                }
            }
            referenceFilePaths.push_back(rfd->FilePath());
        }
        else if (declaration->IsTargetDeclaration())
        {
            TargetDeclaration* td = static_cast<TargetDeclaration*>(declaration.get());
            if (target == Target::none)
            {
                target = td->GetTarget();
            }
            else
            {
                throw std::runtime_error("target '" + TargetStr(td->GetTarget()) + "' already specified ('" + Cm::Util::GetFullPath(FilePath()) + "' line " + std::to_string(declaration->GetSpan().LineNumber()) + ")");
            }
        }
        else if (declaration->IsAssemblyFileDeclaration())
        {
            if (assemblyFilePath.empty())
            {
                assemblyFilePath = static_cast<AssemblyFileDeclaration*>(declaration.get())->FilePath();
            }
            else
            {
                throw std::runtime_error("assembly '" + assemblyFilePath + "' already specified ('" + Cm::Util::GetFullPath(FilePath()) + "' line " + std::to_string(declaration->GetSpan().LineNumber()) + ")");
            }
        }
        else if (declaration->IsExecutableFileDeclaration())
        {
            if (executableFilePath.empty())
            {
                executableFilePath = static_cast<ExecutableFileDeclaration*>(declaration.get())->FilePath();
            }
            else
            {
                throw std::runtime_error("executable '" + executableFilePath + "' already specified ('" + Cm::Util::GetFullPath(FilePath()) + "' line " + std::to_string(declaration->GetSpan().LineNumber()) + ")");
            }
        }
    }
    if (target == Target::none)
    {
        throw std::runtime_error("target ('program' | 'library') not specified in file '" + Cm::Util::GetFullPath(FilePath()) + "'");
    }
    if (assemblyFilePath.empty())
    {
        throw std::runtime_error("assembly not specified in file '" + Cm::Util::GetFullPath(FilePath()) + "'");
    }
    libraryFilePath = boost::filesystem::path(assemblyFilePath).replace_extension(".cml").generic_string();
    if (target == Target::program && executableFilePath.empty())
    {
        executableFilePath = boost::filesystem::path(assemblyFilePath).replace_extension().generic_string();
    }
}

void Project::AddDeclaration(ProjectDeclaration* declaration)
{
    declarations.push_back(std::unique_ptr<ProjectDeclaration>(declaration));
}

} } // namespace Cm::Ast
