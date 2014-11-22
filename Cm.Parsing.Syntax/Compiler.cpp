/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Parsing.Syntax/Compiler.hpp>
#include <Cm.Parsing.Syntax/ProjectFile.hpp>
#include <Cm.Parsing.Syntax/LibraryFile.hpp>
#include <Cm.Parsing.Syntax/ParserFile.hpp>
#include <Cm.Parsing.Syntax/Library.hpp>
#include <Cm.Parsing.Syntax/CodeGenerator.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <Cm.Util/MappedInputFile.hpp>
#include <Cm.Parsing/Linking.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

namespace Cm { namespace Parsing { namespace Syntax {

using namespace Cm::Util;
using namespace boost::filesystem;

std::string ResolveReferenceFilePath(const std::string& relativeReferenceFilePath, const std::string& projectBasePath, const std::vector<std::string>& libraryDirectories)
{
    path base(projectBasePath);
    path rrfp(relativeReferenceFilePath);
    path referenceFilePath = absolute(rrfp, base);
    if (exists(referenceFilePath))
    {
        return referenceFilePath.generic_string();
    }
    int n = int(libraryDirectories.size());
    for (int i = 0; i < n; ++i)
    {
        base = path(libraryDirectories[i]);
        referenceFilePath = absolute(rrfp, base);
        if (exists(referenceFilePath))
        {
            return referenceFilePath.generic_string();
        }
    }
    throw std::runtime_error("library file path '" + relativeReferenceFilePath + "' not found");
}

void Compile(const std::string& projectFilePath, const std::vector<std::string>& libraryDirectories)
{
    std::cout << "Parsing project file " << projectFilePath << "...\n";
    ProjectFileGrammar* projectFileGrammar = ProjectFileGrammar::Create();
    LibraryFileGrammar* libraryFileGrammar = LibraryFileGrammar::Create();
    ParserFileGrammar* parserFileGrammar = ParserFileGrammar::Create();
    Cm::Util::MappedInputFile projectFile(projectFilePath);
    std::unique_ptr<Project> project(projectFileGrammar->Parse(projectFile.Begin(), projectFile.End(), 0, projectFilePath));
    std::cout << "Compiling project '" << project->Name() << "'...\n";
    std::unique_ptr<Cm::Parsing::ParsingDomain> parsingDomain(new Cm::Parsing::ParsingDomain());
    std::cout << "Parsing library files...\n";
    const std::vector<std::string>& referenceFiles = project->ReferenceFiles();
    int nr = int(referenceFiles.size());
    for (int i = 0; i < nr; ++i)
    {
        std::string relativeReferenceFilePath = referenceFiles[i];
        std::string referenceFilePath = ResolveReferenceFilePath(relativeReferenceFilePath, project->BasePath(), libraryDirectories);
        std::cout << "> " << referenceFilePath << "\n";
        Cm::Util::MappedInputFile libraryFile(referenceFilePath);
        libraryFileGrammar->Parse(libraryFile.Begin(), libraryFile.End(), i, referenceFilePath, parsingDomain.get());
    }
    std::cout << "Parsing source files...\n";
    std::vector<std::unique_ptr<ParserFileContent>> parserFiles;
    const std::vector<std::string>& sourceFiles = project->SourceFiles();
    Cm::Parsing::SetFileInfo(sourceFiles);
    int n = int(sourceFiles.size());
    for (int i = 0; i < n; ++i)
    {
        std::string sourceFilePath = sourceFiles[i];
        std::cout << "> " << sourceFilePath << "\n";
        Cm::Util::MappedInputFile sourceFile(sourceFilePath);
        std::unique_ptr<ParserFileContent> parserFileContent(parserFileGrammar->Parse(sourceFile.Begin(), sourceFile.End(), i, sourceFilePath, i, parsingDomain.get()));
        parserFiles.push_back(std::move(parserFileContent));
    }
    std::cout << "Linking...\n";
    Cm::Parsing::Link(parsingDomain.get());
    std::cout << "Expanding code...\n";
    Cm::Parsing::ExpandCode(parsingDomain.get());
    std::cout << "Generating source code...\n";
    GenerateCode(parserFiles);
    std::cout << "Generating library file...\n";
    GenerateLibraryFile(project.get(), parsingDomain.get());
}

} } } // namespace Cm::Parsing::Syntax
