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
#include <Cm.Parser/Solution.hpp>
#include <Cm.Parser/CompileUnit.hpp>
#include <Cm.Parser/FileRegistry.hpp>
#include <Cm.Parser/ToolError.hpp>
#include <Cm.Sym/DeclarationVisitor.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/Module.hpp>
#include <Cm.Sym/ExceptionTable.hpp>
#include <Cm.Sym/MutexTable.hpp>
#include <Cm.Sym/ClassCounter.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/SymbolTypeSet.hpp>
#include <Cm.Sym/Conditional.hpp>
#include <Cm.Sym/FunctionTable.hpp>
#include <Cm.Bind/Prebinder.hpp>
#include <Cm.Bind/VirtualBinder.hpp>
#include <Cm.Bind/Binder.hpp>
#include <Cm.Bind/ClassTemplateRepository.hpp>
#include <Cm.Bind/InlineFunctionRepository.hpp>
#include <Cm.Bind/SynthesizedClassFun.hpp>
#include <Cm.Bind/DelegateTypeOpRepository.hpp>
#include <Cm.Bind/ClassDelegateTypeOpRepository.hpp>
#include <Cm.Bind/ControlFlowAnalyzer.hpp>
#include <Cm.Bind/ArrayTypeOpRepository.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Core/GlobalSettings.hpp>
#include <Cm.Core/CompileUnitMap.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.Emit/LlvmEmitter.hpp>
#include <Cm.Emit/CEmitter.hpp>
#include <Cm.IrIntf/BackEnd.hpp>
#include <Cm.Util/MappedInputFile.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <Cm.Util/System.hpp>
#include <Cm.Util/Path.hpp>
#include <Llvm.Ir/Type.hpp>
#include <chrono>
#include <iostream>

namespace Cm { namespace Build {

char GetPlatformPathSeparatorChar()
{
#ifdef _WIN32
    return ';';
#else
    return ':';
#endif
}

std::string GetOs()
{
#ifdef _WIN32
    return "windows";
#else
    return "linux";
#endif
}

void GetLibraryDirectories(std::vector<std::string>& libraryDirectories)
{
    char* cmLibraryPath = getenv("CM_LIBRARY_PATH");
    if (cmLibraryPath)
    {
        libraryDirectories = Cm::Util::Split(cmLibraryPath, GetPlatformPathSeparatorChar());
    }
    else
    {
        throw std::runtime_error("please set CM_LIBRARY_PATH environment variable to contain (at least) /path/to/system directory " +
            std::string("(dirs separated by '") + std::string(1, GetPlatformPathSeparatorChar()) + "')");
    }
}

std::string ResolveLibraryReference(const boost::filesystem::path& projectOutputBase, const std::string& config, const std::vector<std::string>& libraryDirs, const std::string& libraryReferencePath)
{
    std::vector<std::string> searched;
    boost::filesystem::path lrp(libraryReferencePath);
    boost::filesystem::path libParent = lrp.parent_path();
    boost::filesystem::path projectBase = projectOutputBase.parent_path().parent_path();
    boost::filesystem::path libDir = absolute(libParent, projectBase);
    libDir /= config;
    libDir /= Cm::IrIntf::GetBackEndStr();
    boost::filesystem::path fp(absolute(lrp.filename(), libDir));
    searched.push_back(fp.generic_string());
    if (exists(fp))
    {
        return Cm::Util::GetFullPath(fp.generic_string());
    }
    for (const std::string& libraryDir : libraryDirs)
    {
        boost::filesystem::path ld(libraryDir);
        ld /= libParent;
        ld /= config;
        ld /= Cm::IrIntf::GetBackEndStr();
        boost::filesystem::path fp(absolute(lrp.filename(), ld));
        searched.push_back(fp.generic_string());
        if (exists(fp))
        {
            return Cm::Util::GetFullPath(fp.generic_string());
        }
    }
    std::string s;
    bool first = true;
    for (const std::string& srh : searched)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            s.append(";");
        }
        s.append(srh);
    }
    throw std::runtime_error("library reference '" + libraryReferencePath + "' not found (seached: " + s + ")");
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

void ImportModules(Cm::Sym::SymbolTable& symbolTable, Cm::Ast::Project* project, const std::vector<std::string>& libraryDirs, std::vector<std::string>& assemblyFilePaths,
    std::vector<std::string>& cLibs, std::vector<std::string>& allReferenceFilePaths, std::vector<std::string>& allDebugInfoFilePaths)
{
    boost::filesystem::path projectBase = project->BasePath();
    std::vector<std::string> referenceFilePaths = project->ReferenceFilePaths();
    bool quiet = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::quiet);
    if (!quiet && !referenceFilePaths.empty())
    {
        std::cout << "Importing libraries..." << std::endl;
    }
    std::unordered_set<std::string> importedModules;
    if (project->Name() != "system" && project->Name() != "support" && project->Name() != "os")
    {
        referenceFilePaths.push_back("system.cml");
    } 
    for (const std::string& referenceFilePath : referenceFilePaths)
    {
        std::string libraryReferencePath = ResolveLibraryReference(project->OutputBasePath(), Cm::Core::GetGlobalSettings()->Config(), libraryDirs, referenceFilePath);
        if (importedModules.find(libraryReferencePath) == importedModules.end())
        {
            importedModules.insert(libraryReferencePath);
            Cm::Sym::Module module(libraryReferencePath);
            module.Import(symbolTable, importedModules, assemblyFilePaths, cLibs, allReferenceFilePaths, allDebugInfoFilePaths);
            module.CheckUpToDate();
        }
    }
    symbolTable.ProcessImportedTemplateTypes();
}

void BuildSymbolTable(Cm::Sym::SymbolTable& symbolTable, Cm::Core::GlobalConceptData& globalConceptData, Cm::Ast::SyntaxTree& syntaxTree, Cm::Ast::Project* project, 
    const std::vector<std::string>& libraryDirs, std::vector<std::string>& assemblyFilePaths, std::vector<std::string>& cLibs, std::vector<std::string>& allReferenceFilePaths,
    std::vector<std::string>& allDebugInfoFilePaths)
{
    Cm::Core::InitSymbolTable(symbolTable, globalConceptData);
    ImportModules(symbolTable, project, libraryDirs, assemblyFilePaths, cLibs, allReferenceFilePaths, allDebugInfoFilePaths);
    symbolTable.InitVirtualFunctionTables();
    for (const std::unique_ptr<Cm::Ast::CompileUnitNode>& compileUnit : syntaxTree.CompileUnits())
    {
        Cm::Sym::DeclarationVisitor declarationVisitor(symbolTable);
        compileUnit->Accept(declarationVisitor);
    }
}

void ImportFunctionTables(const std::vector<std::string>& allReferenceFilePaths)
{
    for (const std::string& referenceFilePath : allReferenceFilePaths)
    {
        std::string functionTableFileName = boost::filesystem::path(referenceFilePath).replace_extension(".fid").generic_string();
        Cm::Sym::FunctionTable::Instance()->Import(functionTableFileName);
    }
}

void ExportFunctionTable(const std::string& cmlFilePath)
{
    std::string functionTableFileName = boost::filesystem::path(cmlFilePath).replace_extension(".fid").generic_string();
    Cm::Sym::FunctionTable::Instance()->Export(functionTableFileName);
}

void Bind(Cm::Ast::CompileUnitNode* compileUnit, Cm::BoundTree::BoundCompileUnit& boundCompileUnit)
{
    Cm::Sym::EraseExceptionIdFile(boundCompileUnit.IrFilePath());
    Cm::Bind::Binder binder(boundCompileUnit);
    compileUnit->Accept(binder);
}

void AnalyzeControlFlow(Cm::BoundTree::BoundCompileUnit& boundCompileUnit)
{
    Cm::Bind::ControlFlowAnalyzer controlFlowAnalyzer;
    boundCompileUnit.Accept(controlFlowAnalyzer);
}

void Emit(Cm::Sym::TypeRepository& typeRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit)
{
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        Cm::Emit::LlvmEmitter emitter(boundCompileUnit.IrFilePath(), typeRepository, boundCompileUnit.IrFunctionRepository(), boundCompileUnit.IrClassTypeRepository(),
            boundCompileUnit.StringRepository(), boundCompileUnit.ExternalConstantRepository());
        boundCompileUnit.Accept(emitter);
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        Cm::Emit::CEmitter emitter(boundCompileUnit.IrFilePath(), typeRepository, boundCompileUnit.IrFunctionRepository(), boundCompileUnit.IrClassTypeRepository(),
            boundCompileUnit.StringRepository(), boundCompileUnit.ExternalConstantRepository());
        boundCompileUnit.Accept(emitter);
    }
    else
    {
        throw std::runtime_error("emitter not set");
    }
}

Cm::Parser::ToolErrorGrammar* toolErrorGrammar = nullptr;

void GenerateObjectCode(Cm::BoundTree::BoundCompileUnit& boundCompileUnit)
{
    Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
    std::string llErrorFilePath = backend == Cm::IrIntf::BackEnd::llvm ? Cm::Util::GetFullPath(boost::filesystem::path(boundCompileUnit.IrFilePath()).replace_extension(".ll.error").generic_string()) :
        backend == Cm::IrIntf::BackEnd::c ? Cm::Util::GetFullPath(boost::filesystem::path(boundCompileUnit.IrFilePath()).replace_extension(".c.error").generic_string()) :
        "";
    std::string command = backend == Cm::IrIntf::BackEnd::llvm ? "llc" : backend == Cm::IrIntf::BackEnd::c ? "gcc" : "";
    command.append(" -O").append(std::to_string(Cm::Core::GetGlobalSettings()->OptimizationLevel()));
    if (backend == Cm::IrIntf::BackEnd::llvm)
    {
        command.append(" -filetype=obj");
    }
    else if (backend == Cm::IrIntf::BackEnd::c)
    {
        command.append(" -c");
        if (Cm::Core::GetGlobalSettings()->Config() == "debug")
        {
            command.append(" -g");
        }
    }
    command.append(" -o ").append(Cm::Util::QuotedPath(boundCompileUnit.ObjectFilePath())).append(" ").append(Cm::Util::QuotedPath(boundCompileUnit.IrFilePath()));
    try
    {
        Cm::Util::System(command, 2, llErrorFilePath);
    }
    catch (const std::exception&)
    {
        Cm::Util::MappedInputFile file(llErrorFilePath);
        try
        {
            if (!toolErrorGrammar)
            {
                toolErrorGrammar = Cm::Parser::ToolErrorGrammar::Create();
            }
            Cm::Util::ToolError toolError = toolErrorGrammar->Parse(file.Begin(), file.End(), 0, llErrorFilePath);
            throw Cm::Core::ToolErrorExcecption(toolError);
        }
        catch (...)
        {
            std::string errorText(file.Begin(), file.End());
            throw std::runtime_error(errorText);
        }
    }
    boost::filesystem::remove(llErrorFilePath);
}

void GenerateOptimizedLlvmCodeFile(Cm::BoundTree::BoundCompileUnit& boundCompileUnit)
{
    std::string optllErrorFilePath = Cm::Util::GetFullPath(boost::filesystem::path(boundCompileUnit.IrFilePath()).replace_extension(".opt.ll.error").generic_string());
    std::string command = "opt";
    command.append(" -O").append(std::to_string(Cm::Core::GetGlobalSettings()->OptimizationLevel()));
    command.append(" -S").append(" -o ").append(Cm::Util::QuotedPath(boundCompileUnit.OptIrFilePath())).append(" ").append(Cm::Util::QuotedPath(boundCompileUnit.IrFilePath()));
    try
    {
        Cm::Util::System(command, 2, optllErrorFilePath);
    }
    catch (const std::exception&)
    {
        Cm::Util::MappedInputFile file(optllErrorFilePath);
        try
        {
            if (!toolErrorGrammar)
            {
                toolErrorGrammar = Cm::Parser::ToolErrorGrammar::Create();
            }
            Cm::Util::ToolError toolError = toolErrorGrammar->Parse(file.Begin(), file.End(), 0, optllErrorFilePath);
            throw Cm::Core::ToolErrorExcecption(toolError);
        }
        catch (const std::exception&)
        {
            std::string errorText(file.Begin(), file.End());
            throw std::runtime_error(errorText);
        }
    }
    boost::filesystem::remove(optllErrorFilePath);
}

bool CompileAsmSources(Cm::Ast::Project* project, std::vector<std::string>& objectFilePaths)
{
    bool changed = false;
    for (const std::string& asmSourceFilePath : project->AsmSourceFilePaths())
    {
        std::string objectFilePath = Cm::Util::GetFullPath((project->OutputBasePath() / boost::filesystem::path(asmSourceFilePath).filename().replace_extension(".o")).generic_string());
        boost::filesystem::path ofp = objectFilePath;
        objectFilePaths.push_back(objectFilePath);
        if (!boost::filesystem::exists(ofp))
        {
            changed = true;
        }
        else
        {
            boost::filesystem::path afp = asmSourceFilePath;
            if (boost::filesystem::last_write_time(afp) > boost::filesystem::last_write_time(ofp))
            {
                changed = true;
                break;
            }
        }
    }
    if (!changed) return false;
    bool quiet = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::quiet);
    if (!quiet && !project->AsmSourceFilePaths().empty())
    {
        std::cout << "Compiling assembly sources..." << std::endl;
    }
    for (const std::string& asmSourceFilePath : project->AsmSourceFilePaths())
    {
        std::string llErrorFilePath = Cm::Util::GetFullPath(boost::filesystem::path(asmSourceFilePath).replace_extension(".ll.error").generic_string());
        std::string objectFilePath = Cm::Util::GetFullPath((project->OutputBasePath() / boost::filesystem::path(asmSourceFilePath).filename().replace_extension(".o")).generic_string());
        std::string command = "llc";
        command.append(" -O").append(std::to_string(Cm::Core::GetGlobalSettings()->OptimizationLevel()));
        command.append(" -filetype=obj").append(" -o ").append(Cm::Util::QuotedPath(objectFilePath)).append(" ").append(Cm::Util::QuotedPath(asmSourceFilePath));
        try
        {
            if (!quiet)
            {
                std::cout << "> " << Cm::Util::GetFullPath(asmSourceFilePath) << std::endl;
            }
            Cm::Util::System(command, 2, llErrorFilePath);
        }
        catch (const std::exception&)
        {
            Cm::Util::MappedInputFile file(llErrorFilePath);
            try
            {
                if (!toolErrorGrammar)
                {
                    toolErrorGrammar = Cm::Parser::ToolErrorGrammar::Create();
                }
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
    return true;
}

bool CompileCFiles(Cm::Ast::Project* project, std::vector<std::string>& objectFilePaths)
{
    bool changed = false;
    for (const std::string& cSourceFilePath : project->CSourceFilePaths())
    {
        std::string objectFilePath = Cm::Util::GetFullPath((boost::filesystem::path(project->OutputBasePath()) / boost::filesystem::path(cSourceFilePath).filename().replace_extension(".o")).generic_string());
        objectFilePaths.push_back(objectFilePath);
        boost::filesystem::path ofp = objectFilePath;
        if (!boost::filesystem::exists(ofp))
        {
            changed = true;
        }
        else
        {
            boost::filesystem::path cfp = cSourceFilePath;
            if (boost::filesystem::last_write_time(cfp) > boost::filesystem::last_write_time(ofp))
            {
                changed = true;
                break;
            }
        }
    }
    if (!changed) return false;
    bool quiet = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::quiet);
    if (!quiet && !project->CSourceFilePaths().empty())
    {
        std::cout << "Compiling C files..." << std::endl;
    }
    std::string gFlag;
    if (Cm::Core::GetGlobalSettings()->Config() == "debug")
    {
        gFlag = "-g ";
    }
    for (const std::string& cSourceFilePath : project->CSourceFilePaths())
    {
        std::string objectFilePath = Cm::Util::GetFullPath((boost::filesystem::path(project->OutputBasePath()) / boost::filesystem::path(cSourceFilePath).filename().replace_extension(".o")).generic_string());
        std::string ccCommand = "gcc " + gFlag + "-O" + std::to_string(Cm::Core::GetGlobalSettings()->OptimizationLevel());
        ccCommand.append(" -pthread -c ").append(Cm::Util::QuotedPath(cSourceFilePath)).append(" -o ").append(Cm::Util::QuotedPath(objectFilePath));
        std::string ccErrorFilePath = Cm::Util::GetFullPath(boost::filesystem::path(objectFilePath).replace_extension(".c.error").generic_string());
        try
        {
            if (!quiet)
            {
                std::cout << "> " << Cm::Util::GetFullPath(cSourceFilePath) << std::endl;
            }
            Cm::Util::System(ccCommand, 2, ccErrorFilePath);
        }
        catch (const std::exception&)
        {
            Cm::Util::MappedInputFile file(ccErrorFilePath);
            try
            {
                if (!toolErrorGrammar)
                {
                    toolErrorGrammar = Cm::Parser::ToolErrorGrammar::Create();
                }
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
    return true;
}

bool CompileCppFiles(Cm::Ast::Project* project, std::vector<std::string>& objectFilePaths)
{
    bool changed = false;
    for (const std::string& cppSourceFilePath : project->CppSourceFilePaths())
    {
        std::string objectFilePath = Cm::Util::GetFullPath((boost::filesystem::path(project->OutputBasePath()) / boost::filesystem::path(cppSourceFilePath).filename().replace_extension(".o")).generic_string());
        objectFilePaths.push_back(objectFilePath);
        boost::filesystem::path ofp = objectFilePath;
        if (!boost::filesystem::exists(ofp))
        {
            changed = true;
        }
        else
        {
            boost::filesystem::path cfp = cppSourceFilePath;
            if (boost::filesystem::last_write_time(cfp) > boost::filesystem::last_write_time(ofp))
            {
                changed = true;
                break;
            }
        }
    }
    if (!changed) return false;
    bool quiet = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::quiet);
    if (!quiet && !project->CppSourceFilePaths().empty())
    {
        std::cout << "Compiling C++ files..." << std::endl;
    }
    std::string gFlag;
    if (Cm::Core::GetGlobalSettings()->Config() == "debug")
    {
        gFlag = "-g ";
    }
    for (const std::string& cppSourceFilePath : project->CppSourceFilePaths())
    {
        std::string objectFilePath = Cm::Util::GetFullPath((boost::filesystem::path(project->OutputBasePath()) / boost::filesystem::path(cppSourceFilePath).filename().replace_extension(".o")).generic_string());
        std::string cxxCommand = "g++ " + gFlag + "-O" + std::to_string(Cm::Core::GetGlobalSettings()->OptimizationLevel());
        cxxCommand.append(" -std=c++11 -pthread -c ").append(Cm::Util::QuotedPath(cppSourceFilePath)).append(" -o ").append(Cm::Util::QuotedPath(objectFilePath));
        std::string cxxErrorFilePath = Cm::Util::GetFullPath(boost::filesystem::path(objectFilePath).replace_extension(".cpp.error").generic_string());
        try
        {
            if (!quiet)
            {
                std::cout << "> " << Cm::Util::GetFullPath(cppSourceFilePath) << std::endl;
            }
            Cm::Util::System(cxxCommand, 2, cxxErrorFilePath);
        }
        catch (const std::exception&)
        {
            Cm::Util::MappedInputFile file(cxxErrorFilePath);
            try
            {
                if (!toolErrorGrammar)
                {
                    toolErrorGrammar = Cm::Parser::ToolErrorGrammar::Create();
                }
                Cm::Util::ToolError toolError = toolErrorGrammar->Parse(file.Begin(), file.End(), 0, cxxErrorFilePath);
                throw Cm::Core::ToolErrorExcecption(toolError);
            }
            catch (const std::exception&)
            {
                std::string errorText(file.Begin(), file.End());
                throw std::runtime_error(errorText);
            }
        }
        boost::filesystem::remove(cxxErrorFilePath);
    }
    return true;
}

bool Compile(const std::string& projectName, Cm::Sym::SymbolTable& symbolTable, Cm::Ast::SyntaxTree& syntaxTree, const std::string& outputBasePath, std::vector<std::string>& objectFilePaths, 
    bool rebuild, const std::vector<std::string>& compileFileNames, std::vector<std::string>& debugInfoFilePaths)
{
    bool changed = false;
    if (syntaxTree.CompileUnits().empty()) return changed;
    bool quiet = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::quiet);
    boost::filesystem::path outputBase(outputBasePath);
    std::string ext;
    Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
    if (backend == Cm::IrIntf::BackEnd::llvm)
    {
        ext = ".ll";
    }
    else if (backend == Cm::IrIntf::BackEnd::c)
    {
        ext = ".c";
    }
    std::string prebindCompileUnitIrFilePath = Cm::Util::GetFullPath((outputBase / boost::filesystem::path("__prebind__" + ext)).generic_string());
    Cm::BoundTree::BoundCompileUnit prebindCompileUnit(syntaxTree.CompileUnits().front().get(), prebindCompileUnitIrFilePath, symbolTable);
    prebindCompileUnit.SetPrebindCompileUnit();
    prebindCompileUnit.SetClassTemplateRepository(new Cm::Bind::ClassTemplateRepository(prebindCompileUnit));
    prebindCompileUnit.SetInlineFunctionRepository(new Cm::Bind::InlineFunctionRepository(prebindCompileUnit));
    prebindCompileUnit.SetSynthesizedClassFunRepository(new Cm::Bind::SynthesizedClassFunRepository(prebindCompileUnit));
    prebindCompileUnit.SetDelegateTypeOpRepository(new Cm::Bind::DelegateTypeOpRepository(prebindCompileUnit));
    prebindCompileUnit.SetClassDelegateTypeOpRepository(new Cm::Bind::ClassDelegateTypeOpRepository(prebindCompileUnit));
    prebindCompileUnit.SetArrayTypeOpRepository(new Cm::Bind::ArrayTypeOpRepository(prebindCompileUnit));
    std::vector<std::unique_ptr<Cm::Sym::FileScope>> fileScopes;
    for (const std::unique_ptr<Cm::Ast::CompileUnitNode>& compileUnit : syntaxTree.CompileUnits())
    {
        Cm::Bind::Prebinder prebinder(symbolTable, prebindCompileUnit.ClassTemplateRepository());
        compileUnit->Accept(prebinder);
        fileScopes.push_back(std::unique_ptr<Cm::Sym::FileScope>(prebinder.ReleaseFileScope()));
    }
    for (const std::unique_ptr<Cm::Ast::CompileUnitNode>& compileUnit : syntaxTree.CompileUnits())
    {
        Cm::Bind::VirtualBinder virtualBinder(symbolTable, compileUnit.get(), prebindCompileUnit);
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
    std::set<Cm::BoundTree::BoundCompileUnit*> buildSet;
    std::vector<std::unique_ptr<Cm::BoundTree::BoundCompileUnit>> boundCompileUnits;
    Cm::Core::CompileUnitMap compileUnitMap;
    Cm::Core::SetCompileUnitMap(&compileUnitMap);
    for (const std::unique_ptr<Cm::Ast::CompileUnitNode>& compileUnit : syntaxTree.CompileUnits())
    {
        Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
        std::string ext = backend == Cm::IrIntf::BackEnd::llvm ? ".ll" : backend == Cm::IrIntf::BackEnd::c ? ".c" : "";
        std::string compileUnitIrFilePath = Cm::Util::GetFullPath((outputBase / boost::filesystem::path(compileUnit->FilePath()).filename().replace_extension(ext)).generic_string());
        std::unique_ptr<Cm::BoundTree::BoundCompileUnit> boundCompileUnit(new Cm::BoundTree::BoundCompileUnit(compileUnit.get(), compileUnitIrFilePath, symbolTable));
        compileUnitMap.MapCompileUnit(compileUnit.get(), boundCompileUnit.get());
        boundCompileUnits.push_back(std::move(boundCompileUnit));
    }
    if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::generate_debug_info))
    {
        for (const std::unique_ptr<Cm::BoundTree::BoundCompileUnit>& boundCompileUnit : boundCompileUnits)
        {
            debugInfoFilePaths.push_back(boundCompileUnit->CDebugInfoFilePath());
        }
    }
    if (!rebuild)
    {
        if (!compileFileNames.empty())
        {
            for (const std::unique_ptr<Cm::BoundTree::BoundCompileUnit>& boundCompileUnit : boundCompileUnits)
            {
                boundCompileUnit->ReadDependencyFile();
                for (const std::string& compileFileName : compileFileNames)
                {
                    std::string cfn = Cm::Util::Path::MakeCanonical(compileFileName);
                    if (Cm::Util::LastComponentsEqual(cfn, boundCompileUnit->SyntaxUnit()->FilePath(), '/'))
                    {
                        boundCompileUnit->WriteChangedFile();
                        buildSet.insert(boundCompileUnit.get());
                    }
                }
            }
        }
        else
        {
            for (const std::unique_ptr<Cm::BoundTree::BoundCompileUnit>& boundCompileUnit : boundCompileUnits)
            {
                boundCompileUnit->ReadDependencyFile();
                if (boundCompileUnit->HasChangedFile())
                {
                    for (Cm::BoundTree::BoundCompileUnit* dependentUnit : boundCompileUnit->GetDependentUnits())
                    {
                        buildSet.insert(dependentUnit);
                    }
                    boundCompileUnit->RemoveChangedFile();
                }
                else if (boundCompileUnit->Changed())
                {
                    buildSet.insert(boundCompileUnit.get());
                    for (Cm::BoundTree::BoundCompileUnit* dependentUnit : boundCompileUnit->GetDependentUnits())
                    {
                        buildSet.insert(dependentUnit);
                    }
                }
            }
        }
    }
    int index = 0;
    bool first = true;
    for (const std::unique_ptr<Cm::BoundTree::BoundCompileUnit>& boundCompileUnit : boundCompileUnits)
    {
        if (rebuild || buildSet.find(boundCompileUnit.get()) != buildSet.end())
        {
            if (!quiet && first)
            {
                first = false;
                std::cout << "Compiling..." << std::endl;
            }
            if (!quiet)
            {
                std::cout << "> " << Cm::Util::GetFullPath(boundCompileUnit->SyntaxUnit()->FilePath()) << std::endl;
            }
            changed = true;
            boundCompileUnit->SetClassTemplateRepository(new Cm::Bind::ClassTemplateRepository(*boundCompileUnit));
            boundCompileUnit->SetInlineFunctionRepository(new Cm::Bind::InlineFunctionRepository(*boundCompileUnit));
            boundCompileUnit->SetSynthesizedClassFunRepository(new Cm::Bind::SynthesizedClassFunRepository(*boundCompileUnit));
            boundCompileUnit->SetDelegateTypeOpRepository(new Cm::Bind::DelegateTypeOpRepository(*boundCompileUnit));
            boundCompileUnit->SetClassDelegateTypeOpRepository(new Cm::Bind::ClassDelegateTypeOpRepository(*boundCompileUnit));
            boundCompileUnit->SetArrayTypeOpRepository(new Cm::Bind::ArrayTypeOpRepository(*boundCompileUnit));
            boundCompileUnit->AddFileScope(fileScopes[index].release());
            Bind(boundCompileUnit->SyntaxUnit(), *boundCompileUnit);
            if (boundCompileUnit->HasGotos())
            {
                AnalyzeControlFlow(*boundCompileUnit);
            }
            Emit(symbolTable.GetTypeRepository(), *boundCompileUnit);
            GenerateObjectCode(*boundCompileUnit);
            if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::emitOpt))
            {
                GenerateOptimizedLlvmCodeFile(*boundCompileUnit);
            }
        }
        else
        {
            Cm::Sym::ProcessExceptionIdFile(boundCompileUnit->IrFilePath(), symbolTable);
        }
        objectFilePaths.push_back(boundCompileUnit->ObjectFilePath());
        ++index;
    }
    for (const std::unique_ptr<Cm::BoundTree::BoundCompileUnit>& boundCompileUnit : boundCompileUnits)
    {
        boundCompileUnit->WriteDependencyFile();
    }
    Cm::Core::SetCompileUnitMap(nullptr);
    return changed;
}

bool Archive(const std::vector<std::string>& objectFilePaths, const std::string& assemblyFilePath)
{
    bool changed = false;
    boost::filesystem::path afp = assemblyFilePath;
    if (!boost::filesystem::exists(afp))
    {
        changed = true;
    }
    else
    {
        for (const std::string& objectFilePath : objectFilePaths)
        {
            boost::filesystem::path ofp = objectFilePath;
            if (boost::filesystem::last_write_time(ofp) > boost::filesystem::last_write_time(afp))
            {
                changed = true;
                break;
            }
        }
    }
    if (!changed) return false;
    boost::filesystem::remove(assemblyFilePath);
    bool quiet = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::quiet);
    if (!quiet && !objectFilePaths.empty())
    {
        std::cout << "Archiving..." << std::endl;
    }
    std::string arErrorFilePath = Cm::Util::GetFullPath(boost::filesystem::path(assemblyFilePath).replace_extension(".ar.error").generic_string());
    std::string command = "ar";
    command.append(" q ").append(Cm::Util::QuotedPath(assemblyFilePath));
    for (const std::string& objectFilePath : objectFilePaths)
    {
        if (!quiet)
        {
            std::cout << "> " << Cm::Util::GetFullPath(objectFilePath) << std::endl;
        }
        command.append(1, ' ').append(objectFilePath);
    }
    try
    {
        Cm::Util::System(command, 2, arErrorFilePath);
        if (!quiet && !objectFilePaths.empty())
        {
            std::cout << "=> " << Cm::Util::GetFullPath(assemblyFilePath) << std::endl;
        }
    }
    catch (const std::exception&)
    {
        Cm::Util::MappedInputFile file(arErrorFilePath);
        try
        {
            if (!toolErrorGrammar)
            {
                toolErrorGrammar = Cm::Parser::ToolErrorGrammar::Create();
            }
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
    return true;
}

bool Link(const std::vector<std::string>& assemblyFilePaths, const std::vector<std::string>& cLibs, const std::string& executableFilePath)
{
    bool changed = false;
#ifdef _WIN32
    std::string exePath = Cm::Util::GetFullPath(boost::filesystem::path(executableFilePath).replace_extension(".exe").generic_string());
#else
    std::string exePath = Cm::Util::GetFullPath(executableFilePath);
#endif    
    boost::filesystem::path efp = exePath;
    if (!boost::filesystem::exists(efp))
    {
        changed = true;
    }
    else 
    {
        for (const std::string& assemblyFilePath : assemblyFilePaths)
        {
            boost::filesystem::path afp = assemblyFilePath;
            if (boost::filesystem::last_write_time(afp) > boost::filesystem::last_write_time(efp))
            {
                changed = true;
                break;
            }
        }
    }
    if (!changed) return false;
    bool quiet = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::quiet);
    if (!quiet && !assemblyFilePaths.empty())
    {
        std::cout << "Linking..." << std::endl;
    }
    std::string ccCommand = "gcc";
    ccCommand.append(" -pthread -Xlinker --allow-multiple-definition");
    ccCommand.append(" -Xlinker --start-group");
    for (const std::string& assemblyFilePath : assemblyFilePaths)
    {
        ccCommand.append(" ").append(Cm::Util::QuotedPath(assemblyFilePath));
        if (!quiet)
        {
            std::cout << "> " << Cm::Util::GetFullPath(assemblyFilePath) << std::endl;
        }
    }
    for (const std::string& clib : cLibs)
    {
        ccCommand.append(" -l").append(clib);
        if (!quiet)
        {
            std::cout << "> " << clib << std::endl;
        }
    }
    ccCommand.append(" -Xlinker --end-group");
    ccCommand.append(" -o ").append(Cm::Util::QuotedPath(executableFilePath));
    std::string exeErrorFilePath = Cm::Util::GetFullPath(boost::filesystem::path(executableFilePath).replace_extension(".exe.error").generic_string());
    try
    {
        Cm::Util::System(ccCommand, 2, exeErrorFilePath);
    }
    catch (const std::exception&)
    {
        Cm::Util::MappedInputFile file(exeErrorFilePath);
        try
        {
            if (!toolErrorGrammar)
            {
                toolErrorGrammar = Cm::Parser::ToolErrorGrammar::Create();
            }
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
    if (!quiet)
    {
        std::cout << "=> " << exePath << std::endl;
    }
    return true;
}

bool CreateDebugInfoFile(const std::string& executableFilePath, const std::vector<std::string>& allDebugInfoFilePaths)
{
    bool changed = false;
    std::string cmdbFilePath = Cm::Util::GetFullPath(boost::filesystem::path(executableFilePath).replace_extension(".cmdb").generic_string());
    boost::filesystem::path cfp = cmdbFilePath;
    if (!boost::filesystem::exists(cfp))
    {
        changed = true;
    }
    else
    {
        for (const std::string& debugInfoFilePath : allDebugInfoFilePaths)
        {
            boost::filesystem::path dfp = debugInfoFilePath;
            if (boost::filesystem::last_write_time(dfp) > boost::filesystem::last_write_time(cfp))
            {
                changed = true;
                break;
            }
        }
    }
    if (!changed) return false;
    bool quiet = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::quiet);
    if (!quiet)
    {
        std::cout << "Creating debug info file..." << std::endl;
    }
    std::vector<std::string> nonEmptyDebugInfoFilePaths;
    for (const std::string& debugInfoFilePath : allDebugInfoFilePaths)
    {
        boost::filesystem::path difp = debugInfoFilePath;
        if (boost::filesystem::exists(difp))
        {
            if (boost::filesystem::file_size(difp) != 0)
            {
                nonEmptyDebugInfoFilePaths.push_back(debugInfoFilePath);
            }
        }
    }
    Cm::Ser::BinaryWriter writer(cmdbFilePath);
    writer.Write(int32_t(nonEmptyDebugInfoFilePaths.size()));
    for (const std::string& nonEmptyDebugInfoFilePath : nonEmptyDebugInfoFilePaths)
    {
        if (!quiet)
        {
            std::cout << "> " << nonEmptyDebugInfoFilePath << std::endl;
        }
        Cm::Ser::BinaryReader reader(nonEmptyDebugInfoFilePath);
        Cm::Core::CDebugInfoFile debugInfoFile;
        debugInfoFile.Read(reader);
        debugInfoFile.Write(writer);
    }
    if (!quiet)
    {
        std::cout << "=> " << cmdbFilePath << std::endl;
    }
    return true;
}

void CreateCmProfFile(const std::string& cmlFilePath, const std::vector<std::string>& allReferenceFilePaths)
{
    std::string cmProfFile = boost::filesystem::path(cmlFilePath).replace_extension(".cmprof").generic_string();
    Cm::Sym::FunctionTable globalFunctionTable;
    for (const std::string& referenceFilePath : allReferenceFilePaths)
    {
        std::string fidFilePath = boost::filesystem::path(referenceFilePath).replace_extension(".fid").generic_string();
        globalFunctionTable.ReadFunctionsById(fidFilePath);
    }
    std::string fidFilePath = boost::filesystem::path(cmlFilePath).replace_extension(".fid").generic_string();
    globalFunctionTable.ReadFunctionsById(fidFilePath);
    globalFunctionTable.WriteFunctionsById(cmProfFile);
}

bool GenerateExceptionTableUnit(Cm::Sym::SymbolTable& symbolTable, const std::string& projectOutputBasePath, std::vector<std::string>& objectFilePaths, bool changed)
{
    boost::filesystem::path outputBase(projectOutputBasePath);
    Cm::Parsing::Span span;
    Cm::Ast::CompileUnitNode syntaxUnit(span);
    std::string ext;
    Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
    if (backend == Cm::IrIntf::BackEnd::llvm)
    {
        ext = ".ll";
    }
    else if (backend == Cm::IrIntf::BackEnd::c)
    {
        ext = ".c";
    }
    std::string exceptionTableCompileUnitIrFilePath = Cm::Util::GetFullPath((outputBase / boost::filesystem::path("__exception_table__" + ext)).generic_string());
    Cm::BoundTree::BoundCompileUnit exceptionTableCompileUnit(&syntaxUnit, exceptionTableCompileUnitIrFilePath, symbolTable);
    objectFilePaths.push_back(exceptionTableCompileUnit.ObjectFilePath());
    if (!changed)
    {
        boost::filesystem::path ifp = exceptionTableCompileUnit.IrFilePath();
        boost::filesystem::path ofp = exceptionTableCompileUnit.ObjectFilePath();
        if (!boost::filesystem::exists(ifp))
        {
            changed = true;
        }
        else if (!boost::filesystem::exists(ofp))
        {
            changed = true;
        }
        else if (boost::filesystem::last_write_time(ifp) > boost::filesystem::last_write_time(ofp))
        {
            changed = true;
        }
    }
    if (!changed) return false;
    Cm::Sym::GetExceptionTable()->GenerateExceptionTableUnit(exceptionTableCompileUnitIrFilePath);
    GenerateObjectCode(exceptionTableCompileUnit);
    return true;
}

void CleanProject(Cm::Ast::Project* project)
{
    bool quiet = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::quiet);
    if (!quiet)
    {
        std::cout << "Cleaning project '" << project->Name() << "' (" << Cm::Util::GetFullPath(project->OutputBasePath().generic_string()) << ")" << std::endl;
    }
    boost::filesystem::remove_all(project->OutputBasePath());
    if (!quiet)
    {
        std::cout << "Project '" << project->Name() << "' (" << Cm::Util::GetFullPath(project->OutputBasePath().generic_string()) << ") cleaned successfully" << std::endl;
    }
}

void ReadIdeDefines(std::unordered_set<std::string>& defines, Cm::Ast::Project* project)
{
    boost::filesystem::path projectDefinePath = project->OutputBasePath();
    projectDefinePath /= boost::filesystem::path(project->FilePath()).filename().replace_extension(".cmp.sym");
    std::string cmpSymFilePath = Cm::Util::GetFullPath(projectDefinePath.generic_string());
    std::ifstream cmpSymFile(cmpSymFilePath);
    std::string symbol;
    while (std::getline(cmpSymFile, symbol))
    {
        defines.insert(symbol);
    }
}

void AddPlatformAndConfigDefines(std::unordered_set<std::string>& defines)
{
#ifdef _WIN32
    defines.insert("WINDOWS");
#else
    defines.insert("LINUX");
#endif
    if (Cm::Core::GetGlobalSettings()->Config() == "debug")
    {
        defines.insert("DEBUG");
    }
    else
    {
        defines.insert("RELEASE");
    }
}

bool BuildProject(Cm::Ast::Project* project, bool rebuild, const std::vector<std::string>& compileFileNames, const std::unordered_set<std::string>& defines)
{
    bool changed = false;
    if (Cm::Core::GetGlobalSettings()->Config() == "profile")
    {
        rebuild = true;
    }
    Cm::Core::GetGlobalSettings()->SetCurrentProjectName(project->Name());
    bool quiet = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::quiet);
    if (!quiet)
    {
        std::cout << (rebuild ? "Rebuilding" : "Building") << " project '" << project->Name() << "' (" << Cm::Util::GetFullPath(project->FilePath()) << 
            ") using " << Cm::Core::GetGlobalSettings()->Config() << " configuration..." << std::endl;
    }
    std::unordered_set<std::string> allDefines = defines;
    if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::ide))
    {
        ReadIdeDefines(allDefines, project);
    }
    AddPlatformAndConfigDefines(allDefines);
    Cm::Sym::Define(allDefines);
    Cm::Parser::FileRegistry::Init();
    Cm::Ast::SyntaxTree syntaxTree = ParseSources(*Cm::Parser::FileRegistry::Instance(), project->SourceFilePaths());
    std::vector<std::string> assemblyFilePaths;
    assemblyFilePaths.push_back(project->AssemblyFilePath());
    std::vector<std::string> cLibs;
    cLibs.insert(cLibs.end(), project->CLibraryFilePaths().begin(), project->CLibraryFilePaths().end());
    Cm::Core::GlobalConceptData globalConceptData;
    Cm::Core::SetGlobalConceptData(&globalConceptData);
    Cm::Sym::SymbolTable symbolTable;
    Cm::Sym::FunctionTable functionTable;
    Cm::Sym::FunctionTable::SetInstance(&functionTable);
    Cm::Sym::SymbolTypeSetCollection symbolTypeSetCollection;
    Cm::Sym::SetSymbolTypeSetCollection(&symbolTypeSetCollection);
    Cm::Sym::ExceptionTable exceptionTable;
    Cm::Sym::SetExceptionTable(&exceptionTable);
    Cm::Sym::MutexTable mutexTable;
    Cm::Sym::SetMutexTable(&mutexTable);
    Cm::Sym::ClassCounter classCounter;
    Cm::Sym::SetClassCounter(&classCounter);
    std::vector<std::string> libraryDirs;
    GetLibraryDirectories(libraryDirs);
    std::vector<std::string> allReferenceFilePaths;
    std::vector<std::string> allDebugInfoFilePaths;
    boost::filesystem::path outputBasePath = project->OutputBasePath();
    std::string cmlFilePath = Cm::Util::GetFullPath((outputBasePath / boost::filesystem::path(project->FilePath()).filename().replace_extension(".cml")).generic_string());
    if (!rebuild && boost::filesystem::exists(cmlFilePath))
    {
        Cm::Sym::Module module(cmlFilePath);
        module.CheckFileVersion();
    }
    BuildSymbolTable(symbolTable, globalConceptData, syntaxTree, project, libraryDirs, assemblyFilePaths, cLibs, allReferenceFilePaths, allDebugInfoFilePaths);
    if (Cm::Core::GetGlobalSettings()->Config() == "profile")
    {
        ImportFunctionTables(allReferenceFilePaths);
    }
    boost::filesystem::create_directories(project->OutputBasePath());
    std::vector<std::string> objectFilePaths;
    bool cFilesChanged = CompileCFiles(project, objectFilePaths);
    if (!changed)
    {
        changed = cFilesChanged;
    }
    bool cppFilesChanged = CompileCppFiles(project, objectFilePaths);
    if (!changed)
    {
        changed = cppFilesChanged;
    }
    bool asmSourcesChanged = CompileAsmSources(project, objectFilePaths);
    if (!changed)
    {
        changed = asmSourcesChanged;
    }
    std::vector<std::string> debugInfoFilePaths;
    if (Compile(project->Name(), symbolTable, syntaxTree, project->OutputBasePath().generic_string(), objectFilePaths, rebuild, compileFileNames, debugInfoFilePaths))
    {
        changed = true;
    }
    if (project->GetTarget() == Cm::Ast::Target::program)
    {
        bool mainCompileUnitGenerated = GenerateMainCompileUnit(symbolTable, project->OutputBasePath().generic_string(), 
            boost::filesystem::path(project->AssemblyFilePath()).replace_extension(".profdata").generic_string(), objectFilePaths, changed || rebuild);
        if (!changed)
        {
            changed = mainCompileUnitGenerated;
        }
        bool exceptionTableUnitGenerated = GenerateExceptionTableUnit(symbolTable, project->OutputBasePath().generic_string(), objectFilePaths, changed || rebuild);
        if (!changed)
        {
            changed = exceptionTableUnitGenerated;
        }
    }
    bool objectFilesChanged = Archive(objectFilePaths, project->AssemblyFilePath());
    if (!changed)
    {
        changed = objectFilesChanged;
    }
    if (project->GetTarget() == Cm::Ast::Target::program)
    {
        bool linked = Link(assemblyFilePaths, cLibs, project->ExecutableFilePath());
        if (!changed)
        {
            changed = linked;
        }
    }
    if (changed)
    {
        if (!quiet)
        {
            std::cout << "Generating library file..." << std::endl;
            std::cout << "=> " << cmlFilePath << std::endl;
        }
        Cm::Sym::Module projectModule(cmlFilePath);
        projectModule.SetName(project->Name());
        projectModule.SetSourceFilePaths(project->SourceFilePaths());
        projectModule.SetReferenceFilePaths(allReferenceFilePaths);
        projectModule.SetCLibraryFilePaths(project->CLibraryFilePaths());
        projectModule.SetDebugInfoFilePaths(debugInfoFilePaths);
        projectModule.Export(symbolTable);
        if (Cm::Core::GetGlobalSettings()->Config() == "profile")
        {
            ExportFunctionTable(cmlFilePath);
        }
    }
    if (project->GetTarget() == Cm::Ast::Target::program && Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::generate_debug_info))
    {
        allDebugInfoFilePaths.insert(allDebugInfoFilePaths.end(), debugInfoFilePaths.begin(), debugInfoFilePaths.end());
        bool debugInfoFileCreated = CreateDebugInfoFile(project->ExecutableFilePath(), allDebugInfoFilePaths);
        if (!changed)
        {
            changed = debugInfoFileCreated;
        }
    }
    if (project->GetTarget() == Cm::Ast::Target::program && Cm::Core::GetGlobalSettings()->Config() == "profile")
    {
        CreateCmProfFile(cmlFilePath, allReferenceFilePaths);
    }
    Cm::Core::SetGlobalConceptData(nullptr);
    Cm::Sym::SetExceptionTable(nullptr);
    Cm::Sym::SetMutexTable(nullptr);
    Cm::Sym::SetClassCounter(nullptr);
    Cm::Sym::FunctionTable::SetInstance(nullptr);
        if (!quiet)
    {
        if (!changed)
        {
            std::cout << "Project '" << project->Name() << "' (" << Cm::Util::GetFullPath(project->FilePath()) << ") is up-to-date" << std::endl;
        }
        else
        {
            std::cout << "Project '" << project->Name() << "' (" << Cm::Util::GetFullPath(project->FilePath()) << ") built successfully" << std::endl;
        }
    }
    Cm::Core::GetGlobalSettings()->SetCurrentProjectName("");
    return changed;
}

Cm::Parser::ProjectGrammar* projectGrammar = nullptr;

void BuildProject(const std::string& projectFilePath, bool rebuild, const std::vector<std::string>& compileFileNames, const std::unordered_set<std::string>& defines)
{
    if (!boost::filesystem::exists(projectFilePath))
    {
        throw std::runtime_error("project file '" + projectFilePath + "' not found");
    }
    Cm::Util::MappedInputFile projectFile(projectFilePath);
    if (!projectGrammar)
    {
        projectGrammar = Cm::Parser::ProjectGrammar::Create();
    }
    std::unique_ptr<Cm::Ast::Project> project(projectGrammar->Parse(projectFile.Begin(), projectFile.End(), 0, projectFilePath, Cm::Core::GetGlobalSettings()->Config(), 
        Cm::IrIntf::GetBackEndStr(), GetOs()));
    project->ResolveDeclarations();
    if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::clean))
    {
        CleanProject(project.get());
    }
    else
    {
        BuildProject(project.get(), rebuild, compileFileNames, defines);
    }
}

Cm::Parser::SolutionGrammar* solutionGrammar = nullptr;

void BuildSolution(const std::string& solutionFilePath, bool rebuild, const std::vector<std::string>& compileFileNames, const std::unordered_set<std::string>& defines)
{
    int built = 0;
    int uptodate = 0;
    if (!boost::filesystem::exists(solutionFilePath))
    {
        throw std::runtime_error("solution file '" + solutionFilePath + "' not found");
    }
    if (!solutionGrammar)
    {
        solutionGrammar = Cm::Parser::SolutionGrammar::Create();
    }
    Cm::Util::MappedInputFile solutionFile(solutionFilePath);
    std::unique_ptr<Cm::Ast::Solution> solution(solutionGrammar->Parse(solutionFile.Begin(), solutionFile.End(), 0, solutionFilePath));
    bool quiet = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::quiet);
    bool clean = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::clean);
    if (!quiet)
    {
        std::string work = rebuild ? "Rebuilding" : "Building";
        if (clean)
        {
            work = "Cleaning";
        }
        std::cout << work << " solution '" << solution->Name() << "' (" << Cm::Util::GetFullPath(solution->FilePath()) << 
            ") using " << Cm::Core::GetGlobalSettings()->Config() << " configuration..." << std::endl;
    }
    solution->ResolveDeclarations();
    if (!projectGrammar)
    {
        projectGrammar = Cm::Parser::ProjectGrammar::Create();
    }
    std::vector<std::unique_ptr<Cm::Ast::Project>> projects;
    for (const std::string& projectFilePath : solution->ProjectFilePaths())
    {
        if (!boost::filesystem::exists(projectFilePath))
        {
            throw std::runtime_error("project file '" + projectFilePath + "' not found");
        }
        Cm::Util::MappedInputFile projectFile(projectFilePath);
        std::unique_ptr<Cm::Ast::Project> project(projectGrammar->Parse(projectFile.Begin(), projectFile.End(), 0, projectFilePath, Cm::Core::GetGlobalSettings()->Config(), 
            Cm::IrIntf::GetBackEndStr(), GetOs()));
        project->ResolveDeclarations();
        solution->AddProject(std::move(project));
    }
    std::vector<Cm::Ast::Project*> buildOrder = solution->CreateBuildOrder();
    for (Cm::Ast::Project* project : buildOrder)
    {
        if (clean)
        {
            CleanProject(project);
        }
        else
        {
            bool projectChanged = BuildProject(project, rebuild, compileFileNames, defines);
            if (projectChanged)
            {
                rebuild = true;
                ++built;
            }
            else
            {
                ++uptodate;
            }
        }
    }
    if (!quiet)
    {
        if (clean)
        {
            std::cout << "Solution '" << solution->Name() + "' (" << Cm::Util::GetFullPath(solution->FilePath()) << ") cleaned successfully" << std::endl;
        }
        else
        {
            std::cout << built << " project" << (built != 1 ? "s" : "") << " built, " << uptodate << " project" << (uptodate != 1 ? "s" : "") << " up-to-date" << std::endl;
            std::cout << "Solution '" << solution->Name() + "' (" << Cm::Util::GetFullPath(solution->FilePath()) << ") built successfully" << std::endl;
        }
    }
}

} } // namespace Bm::Build
