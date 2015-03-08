/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_EXCEPTION_TABLE_INCLUDED
#define CM_CORE_EXCEPTION_TABLE_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>

namespace Cm { namespace Core {

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
    std::vector<Cm::Sym::TypeSymbol*> GetProjectExceptions() const;
    int GetExceptionId(Cm::Sym::TypeSymbol* exceptionType) const;
private:
    typedef std::unordered_map<Cm::Sym::TypeSymbol*, int> ExceptionMap;
    typedef ExceptionMap::const_iterator ExceptionMapIt;
    ExceptionMap exceptionMap;
    std::vector<ExceptionRecord> exceptions;
};

void SetExceptionTable(ExceptionTable* exceptionTable);
ExceptionTable* GetExceptionTable();

} } // namespace Cm::Core

#endif // CM_CORE_EXCEPTION_TABLE_INCLUDED
