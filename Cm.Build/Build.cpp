/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Build/Build.hpp>
#include <Cm.Core/InitSymbolTable.hpp>
#include <Cm.Ast/SyntaxTree.hpp>
#include <Cm.Parser/Project.hpp>
#include <Cm.Parser/CompileUnit.hpp>
#include <Cm.Parser/FileRegistry.hpp>
#include <Cm.Sym/DeclarationVisitor.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/Module.hpp>
#include <Cm.Bind/Prebinder.hpp>
#include <Cm.Bind/Binder.hpp>
#include <Cm.Emit/EmittingVisitor.hpp>
#include <Cm.IrIntf/BackEnd.hpp>
#include <Cm.Util/MappedInputFile.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <Cm.Util/System.hpp>
#include <Cm.Util/Path.hpp>
#include <chrono>
#include <iostream>

namespace Cm { namespace Build {

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

void ImportModules(Cm::Sym::SymbolTable& symbolTable, const std::vector<std::string>& referenceFilePaths, boost::filesystem::path& projectBase)
{
    for (const std::string& referenceFilePath : referenceFilePaths)
    {
        boost::filesystem::path lrp = referenceFilePath;
        boost::filesystem::path libParent = lrp.parent_path();
        boost::filesystem::path libDir = boost::filesystem::absolute(libParent, projectBase);
        libDir /= "debug";
        libDir /= "llvm";
        boost::filesystem::path rfp = boost::filesystem::absolute(lrp.filename(), libDir);
        rfp.replace_extension(".mc");
        Cm::Sym::Module module(Cm::Util::GetFullPath(rfp.generic_string()));
        module.ImportTo(symbolTable);
    }
}

void BuildSymbolTable(Cm::Sym::SymbolTable& symbolTable, Cm::Ast::SyntaxTree& syntaxTree, Cm::Ast::Project* project)
{
    Cm::Core::InitSymbolTable(symbolTable);
    boost::filesystem::path projectBase = project->BasePath(); 
    ImportModules(symbolTable, project->ReferenceFilePaths(), projectBase);
    for (const std::unique_ptr<Cm::Ast::CompileUnitNode>& compileUnit : syntaxTree.CompileUnits())
    {
        Cm::Sym::DeclarationVisitor declarationVisitor(symbolTable);
        compileUnit->Accept(declarationVisitor);
    }
}

void Bind(Cm::Ast::CompileUnitNode* compileUnit, Cm::BoundTree::BoundCompileUnit& boundCompileUnit)
{
    Cm::Bind::Binder binder(boundCompileUnit);
    compileUnit->Accept(binder);
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

void Compile(Cm::Sym::SymbolTable& symbolTable, Cm::Ast::SyntaxTree& syntaxTree, const std::string& outputBasePath)
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
        Bind(compileUnit.get(), boundCompileUnit);
        Emit(symbolTable.GetTypeRepository(), boundCompileUnit);
        GenerateObjectCode(boundCompileUnit);
        ++index;
    }
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
    Cm::Sym::SymbolTable symbolTable;
    BuildSymbolTable(symbolTable, syntaxTree, project.get());
    boost::filesystem::create_directories(project->OutputBasePath());
    Compile(symbolTable, syntaxTree, project->OutputBasePath().generic_string());
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
