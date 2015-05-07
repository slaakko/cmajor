/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_MODULE_INCLUDED
#define CM_SYM_MODULE_INCLUDED
#include <Cm.Ast/Project.hpp>
#include <unordered_set>
#include <stdexcept>
#include <string>

namespace Cm { namespace Sym {

class SymbolTable;
class Writer;
class Reader;

class ModuleFileFormatError : public std::runtime_error
{
public:
    ModuleFileFormatError(const std::string& filePath_);
};

class ModuleFileVersionMismatch : public std::runtime_error
{
public:
    ModuleFileVersionMismatch(const std::string& readVersion_, const std::string& expectedVersion_);
private:
    std::string readVersion;
    std::string expectedVersion;
};

class Module
{
public:
    Module(const std::string& filePath_);
    void SetSourceFilePaths(const std::vector<std::string>& sourceFilePaths_);
    void SetReferenceFilePaths(const std::vector<std::string>& referenceFilePaths_);
    void SetCLibraryFilePaths(const std::vector<std::string>& cLibraryFilePaths_);
    void Export(SymbolTable& symbolTable);
    void Import(SymbolTable& symbolTable, std::unordered_set<std::string>& importedModules, std::vector<std::string>& assemblyFilePaths, std::vector<std::string>& cLibs,
        std::vector<std::string>& allReferenceFilePaths);
    void Dump();
private:
    std::string filePath;
    std::vector<std::string> sourceFilePaths;
    std::vector<std::string> referenceFilePaths;
    std::vector<std::string> cLibraryFilePaths;
    void WriteModuleFileId(Writer& writer);
    void WriteSourceFilePaths(Writer& writer);
    void WriteReferenceFilePaths(Writer& writer);
    void WriteCLibraryFilePaths(Writer& writer);
    void CheckModuleFileId(Reader& reader);
    void ReadSourceFilePaths(Reader& reader);
    void ReadReferenceFilePaths(Reader& reader);
    void ReadCLibraryFilePaths(Reader& reader);
    void ExportExceptionTable(Writer& writer);
    void ImportExceptionTable(SymbolTable& symbolTable, Reader& reader);
};

} } // namespace Cm::Sym

#endif // CM_SYM_MODULE_INCLUDED

