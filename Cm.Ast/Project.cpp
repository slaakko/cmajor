/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
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

VersionParser* projectVersionParser = nullptr;

void SetVersionParser(VersionParser* versionParser)
{
    projectVersionParser = versionParser;
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
    ProjectDeclaration(span_, properties_), filePath(filePath_)
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

AddLibraryPathDeclaration::AddLibraryPathDeclaration(const Span& span_, const std::string& filePath_, const boost::filesystem::path& basePath_, const Properties& properties_) :
    ProjectDeclaration(span_, properties_), filePath(boost::filesystem::absolute(filePath_, basePath_).generic_string())
{
}

CLibraryDeclaration::CLibraryDeclaration(const Span& span_, const std::string& filePath_, const Properties& properties_) : ProjectDeclaration(span_, properties_), filePath(filePath_)
{
}

TargetDeclaration::TargetDeclaration(const Span& span_, Target target_) : ProjectDeclaration(span_, Properties()), target(target_)
{
}

StackSizeDeclaration::StackSizeDeclaration(const Span& span_, uint64_t stackReserveSize_, uint64_t stackCommitSize_) : ProjectDeclaration(span_, Properties()), 
    stackReserveSize(stackReserveSize_), stackCommitSize(stackCommitSize_)
{
}

Properties::Properties()
{
}

void Properties::AddProperty(const std::string& name_, RelOp rel_, const std::string& value_)
{
    propertyMap[name_] = std::make_pair(rel_, value_);
}

std::pair<RelOp, std::string> Properties::GetProperty(const std::string& name_) const
{
    std::map<std::string, std::pair<RelOp, std::string>>::const_iterator i = propertyMap.find(name_);
    if (i != propertyMap.cend())
    {
        return i->second;
    }
    return std::make_pair(RelOp::equal, std::string());
}

bool Compare(const std::string& propertyValue, const std::pair<RelOp, std::string>& declaredValue)
{
    switch (declaredValue.first)
    {
        case RelOp::equal: return propertyValue == declaredValue.second;
        case RelOp::notEq: return propertyValue != declaredValue.second;
        case RelOp::less: return propertyValue < declaredValue.second;
        case RelOp::greater: return propertyValue > declaredValue.second;
        case RelOp::lessEq: return propertyValue <= declaredValue.second;
        case RelOp::greaterEq: return propertyValue >= declaredValue.second;
    }
    return false;
}

bool Compare(int propertyValue, const std::pair<RelOp, std::string>& declaredValue)
{
    switch (declaredValue.first)
    {
        case RelOp::equal: return propertyValue == std::stoi(declaredValue.second);
        case RelOp::notEq: return propertyValue != std::stoi(declaredValue.second);
        case RelOp::less: return propertyValue < std::stoi(declaredValue.second);
        case RelOp::greater: return propertyValue > std::stoi(declaredValue.second);
        case RelOp::lessEq: return propertyValue <= std::stoi(declaredValue.second);
        case RelOp::greaterEq: return propertyValue >= std::stoi(declaredValue.second);
    }
    return false;
}

bool Compare(const ProgramVersion& propertyValue, const std::pair<RelOp, std::string>& declaredValue)
{
    ProgramVersion declaredVersion = projectVersionParser->Parse(declaredValue.second);
    switch (declaredValue.first)
    {
        case RelOp::equal: return propertyValue == declaredVersion;
        case RelOp::notEq: return propertyValue != declaredVersion;
        case RelOp::less: return propertyValue < declaredVersion;
        case RelOp::greater: return propertyValue > declaredVersion;
        case RelOp::lessEq: return propertyValue <= declaredVersion;
        case RelOp::greaterEq: return propertyValue >= declaredVersion;
    }
    return false;
}

Project::Project(const std::string& name_, const std::string& filePath_, const std::string& config_, const std::string& backend_, const std::string& os_, int bits_, const ProgramVersion& llvmVersion_) :
    name(name_), filePath(filePath_), basePath(filePath), config(config_), backend(backend_), os(os_), bits(bits_), llvmVersion(llvmVersion_), target(Target::none), stackSize(0, 0)
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
        std::pair<RelOp, std::string> declBackEnd = properties.GetProperty("backend");
        if (!declBackEnd.second.empty())
        {
            if (!Compare(backend, declBackEnd)) continue;
        }
        std::pair<RelOp, std::string> declOs = properties.GetProperty("os");
        if (!declOs.second.empty())
        {
            if (!Compare(os, declOs)) continue;
        }
        std::pair<RelOp, std::string> declBits = properties.GetProperty("bits");
        if (!declBits.second.empty())
        {
            if (!Compare(bits, declBits)) continue;
        }
        std::pair<RelOp, std::string> declLlvmVersion = properties.GetProperty("llvm_version");
        if (!declLlvmVersion.second.empty())
        {
            if (!Compare(llvmVersion, declLlvmVersion)) continue;
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
                case SourceFileType::cpp:
                {
                    for (const std::string& prev : cppSourceFilePaths)
                    {
                        if (prev == sfd->FilePath())
                        {
                            throw std::runtime_error("C++ source file '" + prev + "' already specified ('" + Cm::Util::GetFullPath(FilePath()) + "' line " + std::to_string(sfd->GetSpan().LineNumber()) + ")");
                        }
                    }
                    cppSourceFilePaths.push_back(sfd->FilePath());
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
        else if (declaration->IsAddLibraryPathDeclaration())
        {
            AddLibraryPathDeclaration* lsp = static_cast<AddLibraryPathDeclaration*>(declaration.get());
            librarySearchPaths.push_back(lsp->FilePath());
        }
        else if (declaration->IsCLibraryDeclaration())
        {
            CLibraryDeclaration* clib = static_cast<CLibraryDeclaration*>(declaration.get());
            for (const std::string& prev : cLibraryFilePaths)
            {
                if (prev == clib->FilePath())
                {
                    throw std::runtime_error("C library file '" + prev + "' already specified ('" + Cm::Util::GetFullPath(FilePath()) + "' line " + std::to_string(clib->GetSpan().LineNumber()) + ")");
                }
            }
            cLibraryFilePaths.push_back(clib->FilePath());
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
        else if (declaration->IsStackSizeDeclaration())
        {
            StackSizeDeclaration* s = static_cast<StackSizeDeclaration*>(declaration.get());
            stackSize = std::make_pair(s->StackReserveSize(), s->StackCommitSize());
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

ProgramVersion::ProgramVersion() : majorVersion(0), minorVersion(0), revision(0), build(0), versionText()
{
}

ProgramVersion::ProgramVersion(int major_, int minor_, int revision_, int build_, const std::string& versionText_) : 
    majorVersion(major_), minorVersion(minor_), revision(revision_), build(build_), versionText(versionText_)
{
}

std::string ProgramVersion::ToString() const
{
    return std::to_string(majorVersion) + "." + std::to_string(minorVersion) + "." + std::to_string(revision) + "." + std::to_string(build);
}

bool operator==(const ProgramVersion& left, const ProgramVersion& right)
{
    return left.Major() == right.Major() && left.Minor() == right.Minor() && left.Revision() == right.Revision() && left.Build() == right.Build();
}

bool operator<(const ProgramVersion& left, const ProgramVersion& right)
{
    if (left.Major() < right.Major()) return true;
    if (left.Major() > right.Major()) return false;
    if (left.Minor() < right.Minor()) return true;
    if (left.Minor() > right.Minor()) return false;
    if (left.Revision() < right.Revision()) return true;
    if (left.Revision() > right.Revision()) return false;
    if (left.Build() < right.Build()) return true;
    if (left.Build() > right.Build()) return false;
    return false;
}

} } // namespace Cm::Ast
