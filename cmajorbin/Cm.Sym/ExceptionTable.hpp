/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_EXCEPTION_TABLE_INCLUDED
#define CM_SYM_EXCEPTION_TABLE_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>

namespace Cm { namespace Sym {

class ExceptionRecord
{
public:
    ExceptionRecord(Cm::Sym::TypeSymbol* exceptionType_, Cm::Sym::SymbolSource source_);
    Cm::Sym::TypeSymbol* ExceptionType() const { return exceptionType; }
    Cm::Sym::SymbolSource Source() const { return source; }
private:
    Cm::Sym::TypeSymbol* exceptionType;
    Cm::Sym::SymbolSource source;
};

class ExceptionTable
{
public:
    void AddLibraryException(Cm::Sym::TypeSymbol* exceptionType);
    void AddProjectException(Cm::Sym::TypeSymbol* exceptionType);
    std::vector<Cm::Sym::TypeSymbol*> GetProjectExceptions();
    int GetExceptionId(Cm::Sym::TypeSymbol* exceptionType);
    int GetNumberOfExceptions() const { return int(exceptions.size()); }
    void GenerateExceptionTableUnit(const std::string& exceptionTableFilePath);
private:
    typedef std::unordered_map<Cm::Sym::TypeSymbol*, int> ExceptionMap;
    typedef ExceptionMap::const_iterator ExceptionMapIt;
    ExceptionMap exceptionMap;
    std::vector<ExceptionRecord> exceptions;
};

void EraseExceptionIdFile(const std::string& irFilePath);
void WriteExceptionIdToFile(const std::string& irFilePath, const std::string& exceptionTypeFullName);
void ProcessExceptionIdFile(const std::string& irFilePath, SymbolTable& symbolTable);

void SetExceptionTable(ExceptionTable* exceptionTable);
ExceptionTable* GetExceptionTable();

} } // namespace Cm::Sym

#endif // CM_SYM_EXCEPTION_TABLE_INCLUDED
