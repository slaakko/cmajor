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
#include <Cm.Parser/ToolError.hpp>
#include <Cm.Sym/DeclarationVisitor.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/Module.hpp>
#include <Cm.Sym/ExceptionTable.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Bind/Prebinder.hpp>
#include <Cm.Bind/VirtualBinder.hpp>
#include <Cm.Bind/Binder.hpp>
#include <Cm.Bind/ClassTemplateRepository.hpp>
#include <Cm.Bind/SynthesizedClassFun.hpp>
#include <Cm.Core/Exception.hpp>
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

std::string GetOs()
{
#ifdef WIN32
    return "windows";
#else
    return "linux";
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
    if (project->Name() != "system" && project->Name() != "support" && project->Name() != "os")
    {
        referenceFilePaths.insert(referenceFilePaths.begin(), "system.cml");
    }
    if (project->Name() != "support" && project->Name() != "os")
    {
        referenceFilePaths.insert(referenceFilePaths.begin(), "support.cml");
    }
    if (project->Name() != "os")
    {
        referenceFilePaths.insert(referenceFilePaths.begin(), "os.cml");
    }
    std::unordered_set<std::string> importedModules;
    for (const std::string& referenceFilePath : referenceFilePaths)
    {
        std::string libraryReferencePath = ResolveLibraryReference(project->OutputBasePath(), "debug", libraryDirs, referenceFilePath);
        if (importedModules.find(libraryReferencePath) == importedModules.end())
        {
            importedModules.insert(libraryReferencePath);
            Cm::Sym::Module module(libraryReferencePath);
            module.Import(symbolTable);
            boost::filesystem::path afp = libraryReferencePath;
            afp.replace_extension(".cma");
            assemblyFilePaths.push_back(Cm::Util::GetFullPath(afp.generic_string()));
        }
    }
}

void BuildSymbolTable(Cm::Sym::SymbolTable& symbolTable, Cm::Core::GlobalConceptData& globalConceptData, Cm::Ast::SyntaxTree& syntaxTree, Cm::Ast::Project* project, const std::vector<std::string>& libraryDirs, std::vector<std::string>& assemblyFilePaths)
{
    Cm::Core::InitSymbolTable(symbolTable, globalConceptData);
    ImportModules(symbolTable, project, libraryDirs, assemblyFilePaths);
    symbolTable.InitVirtualFunctionTables();
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
            throw Cm::Core::Exception("already has main() function", boundUserMainFunction->GetSpan(), userMainFunction->GetSpan());
        }
        else
        {
            userMainFunction = boundUserMainFunction;
        }
    }
}

void Emit(Cm::Sym::TypeRepository& typeRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit)
{
    Cm::Emit::EmittingVisitor emittingVisitor(boundCompileUnit.IrFilePath(), typeRepository, boundCompileUnit.IrFunctionRepository(), boundCompileUnit.IrClassTypeRepository(), boundCompileUnit.StringRepository());
    boundCompileUnit.Accept(emittingVisitor);
}

Cm::Parser::ToolErrorGrammar* toolErrorGrammar = nullptr;

void GenerateObjectCode(Cm::BoundTree::BoundCompileUnit& boundCompileUnit)
{
    std::string llErrorFilePath = Cm::Util::GetFullPath(boost::filesystem::path(boundCompileUnit.IrFilePath()).replace_extension(".ll.error").generic_string());
    std::string command = "llc";
    command.append(" -O=").append("0");
    command.append(" -filetype=obj").append(" -o ").append(Cm::Util::QuotedPath(boundCompileUnit.ObjectFilePath())).append(" ").append(Cm::Util::QuotedPath(boundCompileUnit.IrFilePath()));
    try
    {
        Cm::Util::System(command, 2, llErrorFilePath);
    }
    catch (const std::exception&)
    {
        Cm::Util::MappedInputFile file(llErrorFilePath);
        try
        {
            Cm::Util::ToolError toolError = toolErrorGrammar->Parse(file.Begin(), file.End(), 0, llErrorFilePath);
            throw Cm::Core::ToolErrorExcecption(toolError);
        }
        catch (const std::exception&)
        {
            std::string errorText(file.Begin(), file.End());
            throw std::runtime_error(errorText);
        }
    }
    boost::filesystem::remove(llErrorFilePath);
}

void CompileAsmSources(Cm::Ast::Project* project, std::vector<std::string>& objectFilePaths)
{
    for (const std::string& asmSourceFilePath : project->AsmSourceFilePaths())
    {
        std::string llErrorFilePath = Cm::Util::GetFullPath(boost::filesystem::path(asmSourceFilePath).replace_extension(".ll.error").generic_string());
        std::string objectFilePath = Cm::Util::GetFullPath((project->OutputBasePath() / boost::filesystem::path(asmSourceFilePath).filename().replace_extension(".o")).generic_string());
        objectFilePaths.push_back(objectFilePath);
        std::string command = "llc";
        command.append(" -O=").append("0");
        command.append(" -filetype=obj").append(" -o ").append(Cm::Util::QuotedPath(objectFilePath)).append(" ").append(Cm::Util::QuotedPath(asmSourceFilePath));
        try
        {
            Cm::Util::System(command, 2, llErrorFilePath);
        }
        catch (const std::exception&)
        {
            Cm::Util::MappedInputFile file(llErrorFilePath);
            try
            {
                Cm::Util::ToolError toolError = toolErrorGrammar->Parse(file.Begin(), file.End(), 0, llErrorFilePath);
                throw Cm::Core::ToolErrorExcecption(toolError);
            }
            catch (const std::exception&)
            {
                std::string errorText(file.Begin(), file.End());
                throw std::runtime_error(errorText);
            }
        }
        boost::filesystem::remove(llErrorFilePath);
    }
}

void CompileCFiles(Cm::Ast::Project* project, std::vector<std::string>& objectFilePaths)
{
    for (const std::string& cSourceFilePath : project->CSourceFilePaths())
    {
        std::string objectFilePath = Cm::Util::GetFullPath((boost::filesystem::path(project->OutputBasePath()) / boost::filesystem::path(cSourceFilePath).filename().replace_extension(".o")).generic_string());
        std::string ccCommand = "gcc";
        ccCommand.append(" -pthread -c ").append(Cm::Util::QuotedPath(cSourceFilePath)).append(" -o ").append(Cm::Util::QuotedPath(objectFilePath));
        objectFilePaths.push_back(objectFilePath);
        std::string ccErrorFilePath = Cm::Util::GetFullPath(boost::filesystem::path(objectFilePath).replace_extension(".c.error").generic_string());
        try
        {
            Cm::Util::System(ccCommand, 2, ccErrorFilePath);
        }
        catch (const std::exception&)
        {
            Cm::Util::MappedInputFile file(ccErrorFilePath);
            try
            {
                Cm::Util::ToolError toolError = toolErrorGrammar->Parse(file.Begin(), file.End(), 0, ccErrorFilePath);
                throw Cm::Core::ToolErrorExcecption(toolError);
            }
            catch (const std::exception&)
            {
                std::string errorText(file.Begin(), file.End());
                throw std::runtime_error(errorText);
            }
        }
        boost::filesystem::remove(ccErrorFilePath);
    }
}

void Compile(const std::string& projectName, Cm::Sym::SymbolTable& symbolTable, Cm::Ast::SyntaxTree& syntaxTree, const std::string& outputBasePath, Cm::Sym::FunctionSymbol*& userMainFunction, 
    std::vector<std::string>& objectFilePaths)
{
    boost::filesystem::path outputBase(outputBasePath);
    std::string prebindCompileUnitIrFilePath = Cm::Util::GetFullPath((outputBase / boost::filesystem::path("__prebind__.ll")).generic_string());
    Cm::BoundTree::BoundCompileUnit prebindCompileUnit(syntaxTree.CompileUnits().front().get(), prebindCompileUnitIrFilePath, symbolTable);
    prebindCompileUnit.SetClassTemplateRepostory(new Cm::Bind::ClassTemplateRepository(prebindCompileUnit));
    prebindCompileUnit.SetSynthesizedClassFunRepository(new Cm::Bind::SynthesizedClassFunRepository(prebindCompileUnit));
    std::vector<std::unique_ptr<Cm::Sym::FileScope>> fileScopes;
    for (const std::unique_ptr<Cm::Ast::CompileUnitNode>& compileUnit : syntaxTree.CompileUnits())
    {
        Cm::Bind::Prebinder prebinder(symbolTable, prebindCompileUnit.ClassTemplateRepository());
        compileUnit->Accept(prebinder);
        fileScopes.push_back(std::unique_ptr<Cm::Sym::FileScope>(prebinder.ReleaseFileScope()));
    }
    for (const std::unique_ptr<Cm::Ast::CompileUnitNode>& compileUnit : syntaxTree.CompileUnits())
    {
        Cm::Bind::VirtualBinder virtualBinder(symbolTable, compileUnit.get());
        compileUnit->Accept(virtualBinder);
    }
    if (projectName == "system")
    {
        Cm::Sym::ExceptionTable* exceptionTable = Cm::Sym::GetExceptionTable();
        Cm::Sym::Symbol* systemExceptionSymbol = symbolTable.GlobalScope()->Lookup("System.Exception");
        if (systemExceptionSymbol)
        {
            if (systemExceptionSymbol->IsTypeSymbol())
            {
                Cm::Sym::TypeSymbol* systemExceptionType = static_cast<Cm::Sym::TypeSymbol*>(systemExceptionSymbol);
                exceptionTable->AddProjectException(systemExceptionType);
            }
            else
            {
                throw std::runtime_error("System.Exception not a type");
            }
        }
        else
        {
            throw std::runtime_error("System.Exception not found");
        }
    }
    int index = 0;
    for (const std::unique_ptr<Cm::Ast::CompileUnitNode>& compileUnit : syntaxTree.CompileUnits())
    {
        std::string compileUnitIrFilePath = Cm::Util::GetFullPath((outputBase / boost::filesystem::path(compileUnit->FilePath()).filename().replace_extension(".ll")).generic_string());
        Cm::BoundTree::BoundCompileUnit boundCompileUnit(compileUnit.get(), compileUnitIrFilePath, symbolTable);
        boundCompileUnit.SetClassTemplateRepostory(new Cm::Bind::ClassTemplateRepository(boundCompileUnit));
        boundCompileUnit.SetSynthesizedClassFunRepository(new Cm::Bind::SynthesizedClassFunRepository(boundCompileUnit));
        boundCompileUnit.AddFileScope(fileScopes[index].release());
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
    try
    {
        Cm::Util::System(command, 2, arErrorFilePath);
    }
    catch (const std::exception&)
    {
        Cm::Util::MappedInputFile file(arErrorFilePath);
        try
        {
            Cm::Util::ToolError toolError = toolErrorGrammar->Parse(file.Begin(), file.End(), 0, arErrorFilePath);
            throw Cm::Core::ToolErrorExcecption(toolError);
        }
        catch (const std::exception&)
        {
            std::string errorText(file.Begin(), file.End());
            throw std::runtime_error(errorText);
        }
    }
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
    try
    {
        Cm::Util::System(ccCommand, 2, exeErrorFilePath);
    }
    catch (const std::exception&)
    {
        Cm::Util::MappedInputFile file(exeErrorFilePath);
        try
        {
            Cm::Util::ToolError toolError = toolErrorGrammar->Parse(file.Begin(), file.End(), 0, exeErrorFilePath);
            throw Cm::Core::ToolErrorExcecption(toolError);
        }
        catch (const std::exception&)
        {
            std::string errorText(file.Begin(), file.End());
            throw std::runtime_error(errorText);
        }
    }
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
    toolErrorGrammar = Cm::Parser::ToolErrorGrammar::Create();
    Cm::IrIntf::SetBackEnd(Cm::IrIntf::BackEnd::llvm);
    Cm::Util::MappedInputFile projectFile(projectFilePath);
    Cm::Parser::FileRegistry fileRegistry;
    Cm::Parser::SetCurrentFileRegistry(&fileRegistry);
    Cm::Parser::ProjectGrammar* projectGrammar = Cm::Parser::ProjectGrammar::Create();
    std::unique_ptr<Cm::Ast::Project> project(projectGrammar->Parse(projectFile.Begin(), projectFile.End(), 0, projectFilePath, "debug", "llvm", GetOs()));
    project->ResolveDeclarations();
    Cm::Ast::SyntaxTree syntaxTree = ParseSources(fileRegistry, project->SourceFilePaths());
    std::vector<std::string> assemblyFilePaths;
    assemblyFilePaths.push_back(project->AssemblyFilePath());
    Cm::Core::GlobalConceptData globalConceptData;
    Cm::Core::SetGlobalConceptData(&globalConceptData);
    Cm::Sym::SymbolTable symbolTable;
    Cm::Sym::ExceptionTable exceptionTable;
    Cm::Sym::SetExceptionTable(&exceptionTable);
    std::vector<std::string> libraryDirs;
    GetLibraryDirectories(libraryDirs);
    BuildSymbolTable(symbolTable, globalConceptData, syntaxTree, project.get(), libraryDirs, assemblyFilePaths);
    boost::filesystem::create_directories(project->OutputBasePath());
    Cm::Sym::FunctionSymbol* userMainFunction = nullptr;
    std::vector<std::string> objectFilePaths;
    CompileCFiles(project.get(), objectFilePaths);
    CompileAsmSources(project.get(), objectFilePaths);
    Compile(project->Name(), symbolTable, syntaxTree, project->OutputBasePath().generic_string(), userMainFunction, objectFilePaths);
    if (project->GetTarget() == Cm::Ast::Target::program)
    {
        GenerateMainCompileUnit(symbolTable, project->OutputBasePath().generic_string(), userMainFunction, objectFilePaths);
    }
    boost::filesystem::remove(project->AssemblyFilePath());
    Archive(objectFilePaths, project->AssemblyFilePath());
    if (project->GetTarget() == Cm::Ast::Target::program)
    {
        Link(assemblyFilePaths, project->ExecutableFilePath());
    }
    boost::filesystem::path outputBasePath = project->OutputBasePath();
    std::string cmlFilePath = Cm::Util::GetFullPath((outputBasePath / boost::filesystem::path(project->FilePath()).filename().replace_extension(".cml")).generic_string());
    Cm::Sym::Module projectModule(cmlFilePath);
    projectModule.SetSourceFilePaths(project->SourceFilePaths());
    projectModule.Export(symbolTable);
    Cm::Parser::SetCurrentFileRegistry(nullptr);
    Cm::Core::SetGlobalConceptData(nullptr);
    Cm::Sym::SetExceptionTable(nullptr);
    auto end = std::chrono::system_clock::now();
    auto dur = end - start;
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    std::cout << ms << std::endl;
}

} } // namespace Bm::Build
