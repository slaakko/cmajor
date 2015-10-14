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
#include <Cm.Opt/TypePropagationGraph.hpp>

namespace Cm { namespace Build {

void BuildSolution(const std::string& solutionFilePath, bool rebuild, const std::vector<std::string>& compileFileNames, const std::unordered_set<std::string>& defines);
void BuildProject(const std::string& projectFilePath, bool rebuild, const std::vector<std::string>& compileFileNames, const std::unordered_set<std::string>& defines);
void Emit(Cm::Sym::TypeRepository& typeRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Opt::TpGraph* tpGraph);
void GenerateObjectCode(Cm::BoundTree::BoundCompileUnit& boundCompileUnit);
void GetLibraryDirectories(std::vector<std::string>& libraryDirectories);
void ImportModules(Cm::Sym::SymbolTable& symbolTable, Cm::Ast::Project* project, const std::vector<std::string>& libraryDirs, std::vector<std::string>& assemblyFilePaths,
    std::vector<std::string>& cLibs, std::vector<std::string>& allReferenceFilePaths, std::vector<std::string>& allDebugInfoFilePaths, std::vector<std::string>& allNativeObjectFilePaths, 
    std::vector<std::string>& allBcuPaths, std::vector<uint64_t>& classHierarchyTable);
void Bind(Cm::Ast::CompileUnitNode* compileUnit, Cm::BoundTree::BoundCompileUnit& boundCompileUnit);
void AnalyzeControlFlow(Cm::BoundTree::BoundCompileUnit& boundCompileUnit);
bool GenerateExceptionTableUnit(Cm::Sym::SymbolTable& symbolTable, const std::string& projectOutputBasePath, std::vector<std::string>& objectFilePaths, bool changed);
void AddPlatformAndConfigDefines(std::unordered_set<std::string>& defines);

} } // namespace Bm::Build

#endif // CM_BUILD_BUILD_INCLUDED
