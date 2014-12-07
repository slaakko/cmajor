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
#include <Cm.Bind/BindingVisitor.hpp>
#include <Cm.Util/MappedInputFile.hpp>
#include <iostream>

namespace Cm { namespace Build {

void Build(const std::string& projectFilePath)
{
    Cm::Util::MappedInputFile projectFile(projectFilePath);
    Cm::Parser::FileRegistry fileRegistry;
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
    Cm::Sym::DeclarationVisitor declarationVisitor(symbolTable);
    project->VisitCompileUnits(declarationVisitor);
    Cm::Bind::BindingVisitor bindingVisitor(symbolTable);
    project->VisitCompileUnits(bindingVisitor);
    std::cout << "enter" << std::endl;
    char c;
    std::cin >> c;
}

} } // namespace Bm::Build
