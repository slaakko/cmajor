/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Build/Build.hpp>
#include <Cm.Parser/Project.hpp>
#include <Cm.Parser/CompileUnit.hpp>
#include <Cm.Parser/FileRegistry.hpp>
#include <Cm.Sym/DeclarationVisitor.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/Module.hpp>
#include <Cm.Bind/BindingVisitor.hpp>
#include <Cm.Util/MappedInputFile.hpp>
#include <chrono>
#include <iostream>

namespace Cm { namespace Build {

void Build(const std::string& projectFilePath)
{
    auto start = std::chrono::system_clock::now();
    Cm::Util::MappedInputFile projectFile(projectFilePath);
    Cm::Parser::FileRegistry fileRegistry;
    Cm::Parser::SetCurrentFileRegistry(&fileRegistry);
    Cm::Parser::ProjectGrammar* projectGrammar = Cm::Parser::ProjectGrammar::Create();
    Cm::Parser::CompileUnitGrammar* compileUnitGrammar = Cm::Parser::CompileUnitGrammar::Create();
    int projectFileIndex = fileRegistry.RegisterParsedFile(projectFilePath);
    std::unique_ptr<Cm::Ast::Project> project(projectGrammar->Parse(projectFile.Begin(), projectFile.End(), projectFileIndex, projectFilePath, "debug", "llvm"));
    project->ResolveDeclarations();
    for (const std::string& sourceFilePath : project->SourceFilePaths())
    {
        Cm::Util::MappedInputFile sourceFile(sourceFilePath);
        int sourceFileIndex = fileRegistry.RegisterParsedFile(sourceFilePath);
        Cm::Parser::ParsingContext ctx;
        Cm::Ast::CompileUnitNode* compileUnit = compileUnitGrammar->Parse(sourceFile.Begin(), sourceFile.End(), sourceFileIndex, sourceFilePath, &ctx);
        project->AddCompileUnit(compileUnit);
    }
    Cm::Sym::SymbolTable symbolTable;
    boost::filesystem::path projectBase = project->BasePath();
    for (const std::string& referenceFilePath : project->ReferenceFilePaths())
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
    Cm::Sym::DeclarationVisitor declarationVisitor(symbolTable);
    project->VisitCompileUnits(declarationVisitor);
    Cm::Bind::BindingVisitor bindingVisitor(symbolTable);
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
