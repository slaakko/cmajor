/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Build/Build.hpp>
#include <Cm.Build/Main.hpp>
#include <Cm.Core/InitSymbolTable.hpp>
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.Ast/SyntaxTree.hpp>
#include <Cm.Parser/Project.hpp>
#include <Cm.Parser/CompileUnit.hpp>
#include <Cm.Parser/FileRegistry.hpp>
#include <Cm.Sym/DeclarationVisitor.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/Module.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Bind/Prebinder.hpp>
#include <Cm.Bind/Binder.hpp>
#include <Cm.Bind/Exception.hpp>
#include <Cm.Emit/EmittingVisitor.hpp>
#include <Cm.IrIntf/BackEnd.hpp>
#include <Cm.Util/MappedInputFile.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <Cm.Util/System.hpp>
#include <Cm.Util/Path.hpp>
#include <chrono>
#include <iostream>

namespace Cm { namespace Build {

char GetPlatformPathSeparatorChar()
{
#ifdef WIN32
    return ';';
#else
    return ':';
#endif
}

void GetLibraryDirectories(std::vector<std::string>& libraryDirectories)
{
    char* cmLibraryPath = getenv("CMBIN_LIBRARY_PATH");
    if (cmLibraryPath)
    {
        libraryDirectories = Cm::Util::Split(cmLibraryPath, GetPlatformPathSeparatorChar());
    }
    else
    {
        throw std::runtime_error("please set CMBIN_LIBRARY_PATH environment variable to contain (at least) /path/to/system directory " +
            std::string("(dirs separated by '") + std::string(1, GetPlatformPathSeparatorChar()) + "')");
    }
}

std::string ResolveLibraryReference(const boost::filesystem::path& projectOutputBase, const std::string& config, const std::vector<std::string>& libraryDirs, const std::string& libraryReferencePath)
{
    boost::filesystem::path lrp(libraryReferencePath);
    boost::filesystem::path libParent = lrp.parent_path();
    boost::filesystem::path projectBase = projectOutputBase.parent_path().parent_path();
    boost::filesystem::path libDir = absolute(libParent, projectBase);
    libDir /= config;
    libDir /= Cm::IrIntf::GetBackEndStr();
    boost::filesystem::path fp(absolute(lrp.filename(), libDir));
    if (exists(fp))
    {
        return Cm::Util::GetFullPath(fp.generic_string());
    }
    for (const std::string& libraryDir : libraryDirs)
    {
        boost::filesystem::path ld(libraryDir);
        ld /= config;
        ld /= Cm::IrIntf::GetBackEndStr();
        boost::filesystem::path fp(absolute(lrp, ld));
        if (exists(fp))
        {
            return Cm::Util::GetFullPath(fp.generic_string());
        }
    }
    throw std::runtime_error("library reference '" + libraryReferencePath + "' not found");
}

Cm::Ast::SyntaxTree ParseSources(Cm::Parser::FileRegistry& fileRegistry, const std::vector<std::string>& sourceFilePaths)
{
    Cm::Parser::CompileUnitGrammar* compileUnitGrammar = Cm::Parser::CompileUnitGrammar::Create();
    Cm::Ast::SyntaxTree syntaxTree;
    for (const std::string& sourceFilePath : sourceFilePaths)
    {
        Cm::Util::MappedInputFile sourceFile(sourceFilePath);
        int sourceFileIndex = fileRegistry.RegisterParsedFile(sourceFilePath);
        Cm::Parser::ParsingContext ctx;
        Cm::Ast::CompileUnitNode* compileUnit = compileUnitGrammar->Parse(sourceFile.Begin(), sourceFile.End(), sourceFileIndex, sourceFilePath, &ctx);
        syntaxTree.AddCompileUnit(compileUnit);
    }
    return syntaxTree;
}

void ImportModules(Cm::Sym::SymbolTable& symbolTable, Cm::Ast::Project* project, const std::vector<std::string>& libraryDirs, std::vector<std::string>& assemblyFilePaths)
{
    boost::filesystem::path projectBase = project->BasePath();
    std::vector<std::string> referenceFilePaths = project->ReferenceFilePaths();
    if (project->Name() != "os")
    {
        referenceFilePaths.push_back("os.cml");
    }
    for (const std::string& referenceFilePath : referenceFilePaths)
    {
        boost::filesystem::path rfp = ResolveLibraryReference(project->OutputBasePath(), "debug", libraryDirs, referenceFilePath);
        rfp.replace_extension(".mc");
        Cm::Sym::Module module(Cm::Util::GetFullPath(rfp.generic_string()));
        module.ImportTo(symbolTable);
        boost::filesystem::path afp = rfp;
        afp.replace_extension(".cma");
        assemblyFilePaths.push_back(Cm::Util::GetFullPath(afp.generic_string()));
    }
}

void BuildSymbolTable(Cm::Sym::SymbolTable& symbolTable, Cm::Ast::SyntaxTree& syntaxTree, Cm::Ast::Project* project, const std::vector<std::string>& libraryDirs, std::vector<std::string>& assemblyFilePaths)
{
    Cm::Core::InitSymbolTable(symbolTable);
    ImportModules(symbolTable, project, libraryDirs, assemblyFilePaths);
    for (const std::unique_ptr<Cm::Ast::CompileUnitNode>& compileUnit : syntaxTree.CompileUnits())
    {
        Cm::Sym::DeclarationVisitor declarationVisitor(symbolTable);
        compileUnit->Accept(declarationVisitor);
    }
}

void Bind(Cm::Ast::CompileUnitNode* compileUnit, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::FunctionSymbol*& userMainFunction)
{
    Cm::Bind::Binder binder(boundCompileUnit);
    compileUnit->Accept(binder);
    Cm::Sym::FunctionSymbol* boundUserMainFunction = binder.GetUserMainFunction();
    if (boundUserMainFunction)
    {
        if (userMainFunction)
        {
            throw Cm::Bind::Exception("already has main() function", boundUserMainFunction->GetSpan(), userMainFunction->GetSpan());
        }
        else
        {
            userMainFunction = boundUserMainFunction;
        }
    }
}

void Emit(Cm::Sym::TypeRepository& typeRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit)
{
    Cm::Emit::EmittingVisitor emittingVisitor(boundCompileUnit.IrFilePath(), typeRepository, boundCompileUnit.IrFunctionRepository());
    boundCompileUnit.Accept(emittingVisitor);
}

void GenerateObjectCode(Cm::BoundTree::BoundCompileUnit& boundCompileUnit)
{
    std::string llErrorFilePath = Cm::Util::GetFullPath(boost::filesystem::path(boundCompileUnit.IrFilePath()).replace_extension(".ll.error").generic_string());
    std::string command = "llc";
    command.append(" -O=").append("0");
    command.append(" -filetype=obj").append(" -o ").append(Cm::Util::QuotedPath(boundCompileUnit.ObjectFilePath())).append(" ").append(Cm::Util::QuotedPath(boundCompileUnit.IrFilePath()));
    Cm::Util::System(command, 2, llErrorFilePath);
    boost::filesystem::remove(llErrorFilePath);
}

void Compile(Cm::Sym::SymbolTable& symbolTable, Cm::Ast::SyntaxTree& syntaxTree, const std::string& outputBasePath, Cm::Sym::FunctionSymbol*& userMainFunction, std::vector<std::string>& objectFilePaths)
{
    boost::filesystem::path outputBase(outputBasePath);
    std::vector<Cm::Sym::FileScope*> fileScopes;
    for (const std::unique_ptr<Cm::Ast::CompileUnitNode>& compileUnit : syntaxTree.CompileUnits())
    {
        Cm::Bind::Prebinder prebinder(symbolTable);
        compileUnit->Accept(prebinder);
        fileScopes.push_back(prebinder.GetFileScope());
    }
    int index = 0;
    for (const std::unique_ptr<Cm::Ast::CompileUnitNode>& compileUnit : syntaxTree.CompileUnits())
    {
        std::string compileUnitIrFilePath = Cm::Util::GetFullPath((outputBase / boost::filesystem::path(compileUnit->FilePath()).filename().replace_extension(".ll")).generic_string());
        Cm::BoundTree::BoundCompileUnit boundCompileUnit(compileUnitIrFilePath, symbolTable);
        boundCompileUnit.SetFileScope(fileScopes[index]);
        Bind(compileUnit.get(), boundCompileUnit, userMainFunction);
        Emit(symbolTable.GetTypeRepository(), boundCompileUnit);
        GenerateObjectCode(boundCompileUnit);
        objectFilePaths.push_back(boundCompileUnit.ObjectFilePath());
        ++index;
    }
}

void Archive(const std::vector<std::string>& objectFilePaths, const std::string& assemblyFilePath)
{
    std::string arErrorFilePath = Cm::Util::GetFullPath(boost::filesystem::path(assemblyFilePath).replace_extension(".ar.error").generic_string());
    std::string command = "ar";
    command.append(" q ").append(Cm::Util::QuotedPath(assemblyFilePath));
    for (const std::string& objectFilePath : objectFilePaths)
    {
        command.append(1, ' ').append(objectFilePath);
    }
    Cm::Util::System(command, 2, arErrorFilePath);
    boost::filesystem::remove(arErrorFilePath);
}

void Link(const std::vector<std::string>& assemblyFilePaths, const std::string& executableFilePath)
{
    std::string ccCommand = "gcc";
    ccCommand.append(" -pthread -Xlinker --allow-multiple-definition");
/*/
    for (const std::string& objectFile : objectFiles)
    {
        ccCommand.append(" ").append(QuotedPath(objectFile));
        if (genOutput)
        {
            std::cout << "> " << Soul::Util::GetFullPath(objectFile) << std::endl;
        }
    }
*/
    ccCommand.append(" -Xlinker --start-group");
    for (const std::string& assemblyFilePath : assemblyFilePaths)
    {
        ccCommand.append(" ").append(Cm::Util::QuotedPath(assemblyFilePath));
/*
        if (genOutput)
        {
            std::cout << "> " << Soul::Util::GetFullPath(assemblyFile) << std::endl;
        }
*/
    }
/*
    for (const std::string& clib : cLibs)
    {
        ccCommand.append(" -l").append(clib);
        if (genOutput)
        {
            std::cout << "> " << clib << std::endl;
        }
    }
*/
    ccCommand.append(" -Xlinker --end-group");
    ccCommand.append(" -o ").append(Cm::Util::QuotedPath(executableFilePath));
    std::string exeErrorFilePath = Cm::Util::GetFullPath(boost::filesystem::path(executableFilePath).replace_extension(".exe.error").generic_string());
/*
    if (genOutput && (GetGlobalFlags() & GlobalFlags::verbose) != GlobalFlags::none)
    {
        std::cout << ccCommand << std::endl;
    }
*/
    Cm::Util::System(ccCommand, 2, exeErrorFilePath);
    boost::filesystem::remove(exeErrorFilePath);
/*
    if (genOutput)
    {
        std::cout << "=> " << Soul::Util::GetFullPath((executableFilePath.empty() ? exePath : executableFilePath)) << std::endl;
    }
*/
}

void Build(const std::string& projectFilePath)
{
    auto start = std::chrono::system_clock::now();
    Cm::IrIntf::SetBackEnd(Cm::IrIntf::BackEnd::llvm);
    Cm::Util::MappedInputFile projectFile(projectFilePath);
    Cm::Parser::FileRegistry fileRegistry;
    Cm::Parser::SetCurrentFileRegistry(&fileRegistry);
    Cm::Parser::ProjectGrammar* projectGrammar = Cm::Parser::ProjectGrammar::Create();
    int projectFileIndex = fileRegistry.RegisterParsedFile(projectFilePath);
    std::unique_ptr<Cm::Ast::Project> project(projectGrammar->Parse(projectFile.Begin(), projectFile.End(), projectFileIndex, projectFilePath, "debug", "llvm"));
    project->ResolveDeclarations();
    Cm::Ast::SyntaxTree syntaxTree = ParseSources(fileRegistry, project->SourceFilePaths());
    std::vector<std::string> assemblyFilePaths;
    assemblyFilePaths.push_back(project->AssemblyFilePath());
    Cm::Sym::SymbolTable symbolTable;
    std::vector<std::string> libraryDirs;
    GetLibraryDirectories(libraryDirs);
    BuildSymbolTable(symbolTable, syntaxTree, project.get(), libraryDirs, assemblyFilePaths);
    boost::filesystem::create_directories(project->OutputBasePath());
    Cm::Sym::FunctionSymbol* userMainFunction = nullptr;
    std::vector<std::string> objectFilePaths;
    Compile(symbolTable, syntaxTree, project->OutputBasePath().generic_string(), userMainFunction, objectFilePaths);
    if (project->GetTarget() == Cm::Ast::Target::program)
    {
        GenerateMainCompileUnit(symbolTable, project->OutputBasePath().generic_string(), userMainFunction, objectFilePaths);
    }
    Archive(objectFilePaths, project->AssemblyFilePath());
    if (project->GetTarget() == Cm::Ast::Target::program)
    {
        Link(assemblyFilePaths, project->ExecutableFilePath());
    }
    boost::filesystem::path outputBasePath = project->OutputBasePath();
    std::string mcFilePath = Cm::Util::GetFullPath((outputBasePath / boost::filesystem::path(project->FilePath()).filename().replace_extension(".mc")).generic_string());
    Cm::Sym::Module projectModule(mcFilePath);
    projectModule.SetSourceFilePaths(project->SourceFilePaths());
    projectModule.Export(symbolTable);
    Cm::Parser::SetCurrentFileRegistry(nullptr);
    auto end = std::chrono::system_clock::now();
    auto dur = end - start;
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    std::cout << ms << std::endl;
}

} } // namespace Bm::Build
