/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/ExceptionTable.hpp>

namespace Cm { namespace Core {

ExceptionRecord::ExceptionRecord(Cm::Sym::TypeSymbol* exceptionType_, Cm::Sym::SymbolSource source_) : exceptionType(exceptionType_), source(source_)
{
}

void ExceptionTable::AddLibraryException(Cm::Sym::TypeSymbol* exceptionType)
{
    ExceptionMapIt i = exceptionMap.find(exceptionType);
    if (i == exceptionMap.end())
    {
        int exceptionId = int(exceptions.size());
        exceptions.push_back(ExceptionRecord(exceptionType, Cm::Sym::SymbolSource::library));
        exceptionMap[exceptionType] = exceptionId;
    }
}

void ExceptionTable::AddProjectException(Cm::Sym::TypeSymbol* exceptionType)
{
    ExceptionMapIt i = exceptionMap.find(exceptionType);
    if (i == exceptionMap.end())
    {
        int exceptionId = int(exceptions.size());
        exceptions.push_back(ExceptionRecord(exceptionType, Cm::Sym::SymbolSource::project));
        exceptionMap[exceptionType] = exceptionId;
    }
}

std::vector<Cm::Sym::TypeSymbol*> ExceptionTable::GetProjectExceptions() const
{
    std::vector<Cm::Sym::TypeSymbol*> projectExceptions;
    for (const ExceptionRecord& exceptionRec : exceptions)
    {
        if (exceptionRec.Source() == Cm::Sym::SymbolSource::project)
        {
            projectExceptions.push_back(exceptionRec.ExceptionType());
        }
    }
    return projectExceptions;
}

int ExceptionTable::GetExceptionId(Cm::Sym::TypeSymbol* exceptionType) const
{
    ExceptionMapIt i = exceptionMap.find(exceptionType);
    if (i != exceptionMap.end())
    {
        return i->second;
    }
    throw std::runtime_error("exception id for exception type '" + exceptionType->FullName() + "' not found");
}

ExceptionTable* globalExceptionTable = nullptr;

void SetExceptionTable(ExceptionTable* exceptionTable)
{
    globalExceptionTable = exceptionTable;
}

ExceptionTable* GetExceptionTable()
{
    return globalExceptionTable;
}

} } // namespace Cm::Core
