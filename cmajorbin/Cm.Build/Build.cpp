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
#include <Cm.Bind/BindingVisitor.hpp>
#include <Cm.IrIntf/BackEnd.hpp>
#include <Cm.Util/MappedInputFile.hpp>
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
        Cm::Sym::Module module(rfp.generic_string());
        module.ImportTo(symbolTable);
    }
}

Cm::Sym::SymbolTable BuildSymbolTable(Cm::Ast::SyntaxTree& syntaxTree)
{
    Cm::Sym::SymbolTable symbolTable;
    Cm::Core::InitSymbolTable(symbolTable);
    for (const std::unique_ptr<Cm::Ast::CompileUnitNode>& compileUnit : syntaxTree.CompileUnits())
    {
        Cm::Sym::DeclarationVisitor declarationVisitor(symbolTable);
        compileUnit->Accept(declarationVisitor);
    }
    return symbolTable;
}

Cm::BoundTree::BoundCompileUnit Bind(Cm::Sym::SymbolTable& symbolTable, Cm::Ast::CompileUnitNode* compileUnit)
{
    Cm::Bind::BindingVisitor bindingVisitor(symbolTable);
    compileUnit->Accept(bindingVisitor);
    return bindingVisitor.Result();
}

void Compile(Cm::Sym::SymbolTable& symbolTable, Cm::Ast::SyntaxTree& syntaxTree)
{
    for (const std::unique_ptr<Cm::Ast::CompileUnitNode>& compileUnit : syntaxTree.CompileUnits())
    {
        Cm::BoundTree::BoundCompileUnit boundCompileUnit = Bind(symbolTable, compileUnit.get());
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
    Cm::Sym::SymbolTable symbolTable = BuildSymbolTable(syntaxTree);
    boost::filesystem::path projectBase = project->BasePath();
    ImportModules(symbolTable, project->ReferenceFilePaths(), projectBase);
    project->VisitCompileUnits(bindingVisitor);
    boost::filesystem::create_directories(project->OutputBasePath());
    boost::filesystem::path moduleFilePath = project->OutputBasePath();
    boost::filesystem::path mcFilePath = moduleFilePath / boost::filesystem::path(project->FilePath()).filename().replace_extension(".mc");
    Cm::Sym::Module projectModule(mcFilePath.generic_string());
    projectModule.SetSourceFilePaths(project->SourceFilePaths());
    projectModule.Export(symbolTable);
    Cm::Parser::SetCurrentFileRegistry(nullptr);
    auto end = std::chrono::system_clock::now();
    auto dur = end - start;
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    std::cout << ms << std::endl;
}

} } // namespace Bm::Build
