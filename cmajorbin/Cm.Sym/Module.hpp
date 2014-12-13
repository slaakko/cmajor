/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_MODULE_INCLUDED
#define CM_SYM_MODULE_INCLUDED
#include <Cm.Ast/Project.hpp>
#include <stdexcept>
#include <string>

namespace Cm { namespace Sym {

class SymbolTable;
class Writer;
class Reader;

class ModuleFileVersionMismatch : std::runtime_error
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
    void Export(SymbolTable& symbolTable);
    void ImportTo(SymbolTable& symbolTable);
    void Dump();
private:
    std::string filePath;
    std::vector<std::string> sourceFilePaths;
    void WriteModuleFileId(Writer& writer);
    void WriteSourceFilePaths(Writer& writer);
    void CheckModuleFileId(Reader& reader);
    void ReadSourceFilePaths(Reader& reader);
};

} } // namespace Cm::Sym

#endif // CM_SYM_MODULE_INCLUDED


