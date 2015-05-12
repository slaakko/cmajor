/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BUILD_BUILD_INCLUDED
#define CM_BUILD_BUILD_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.Ast/Project.hpp>

namespace Cm { namespace Build {

std::string GetCurrentProjectName();
void BuildSolution(const std::string& solutionFilePath, bool rebuild, const std::vector<std::string>& compileFileNames);
bool BuildProject(const std::string& projectFilePath, bool rebuild, const std::vector<std::string>& compileFileNames);
void Emit(Cm::Sym::TypeRepository& typeRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit);
void GenerateObjectCode(Cm::BoundTree::BoundCompileUnit& boundCompileUnit);
void GetLibraryDirectories(std::vector<std::string>& libraryDirectories);
void ImportModules(Cm::Sym::SymbolTable& symbolTable, Cm::Ast::Project* project, const std::vector<std::string>& libraryDirs, std::vector<std::string>& assemblyFilePaths,
    std::vector<std::string>& cLibs, std::vector<std::string>& allReferenceFilePaths);
void Bind(Cm::Ast::CompileUnitNode* compileUnit, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::FunctionSymbol*& userMainFunction);
void AnalyzeControlFlow(Cm::BoundTree::BoundCompileUnit& boundCompileUnit);
void GenerateExceptionTableUnit(Cm::Sym::SymbolTable& symbolTable, const std::string& projectOutputBasePath, std::vector<std::string>& objectFilePaths);

} } // namespace Bm::Build

#endif // CM_BUILD_BUILD_INCLUDED
