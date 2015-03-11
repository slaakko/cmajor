/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ExceptionTable.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Util/CodeFormatter.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <fstream>

namespace Cm { namespace Sym {

ExceptionRecord::ExceptionRecord(Cm::Sym::TypeSymbol* exceptionType_, Cm::Sym::SymbolSource source_) : exceptionType(exceptionType_), source(source_)
{
}

void ExceptionTable::AddLibraryException(Cm::Sym::TypeSymbol* exceptionType)
{
    ExceptionMapIt i = exceptionMap.find(exceptionType);
    if (i == exceptionMap.end())
    {
        int exceptionId = int(exceptions.size()) + 1;
        exceptions.push_back(ExceptionRecord(exceptionType, Cm::Sym::SymbolSource::library));
        exceptionMap[exceptionType] = exceptionId;
    }
}

void ExceptionTable::AddProjectException(Cm::Sym::TypeSymbol* exceptionType)
{
    ExceptionMapIt i = exceptionMap.find(exceptionType);
    if (i == exceptionMap.end())
    {
        int exceptionId = int(exceptions.size()) + 1;
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

void ExceptionTable::GenerateExceptionTableUnit(const std::string& exceptionTableFilePath)
{
    std::ofstream exceptionTableFile(exceptionTableFilePath);
    Cm::Util::CodeFormatter formatter(exceptionTableFile);
    int n = int(exceptions.size());
    std::unique_ptr<Ir::Intf::Type> exceptionBaseIdArrayType(Cm::IrIntf::Array(Cm::IrIntf::I32(), n));
    exceptionBaseIdArrayType->SetOwned();
    formatter.WriteLine("@$exception$base$id$table = constant " + exceptionBaseIdArrayType->Name());
    formatter.WriteLine("[");
    formatter.IncIndent();
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::TypeSymbol* exceptionType = exceptions[i].ExceptionType();
        if (!exceptionType->IsClassTypeSymbol())
        {
            throw std::runtime_error("exception type not class type");
        }
        Cm::Sym::ClassTypeSymbol* exceptionClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(exceptionType);
        int baseId = 0;
        Cm::Sym::ClassTypeSymbol* baseClassType = exceptionClassType->BaseClass();
        if (baseClassType)
        {
            baseId = GetExceptionId(baseClassType);
        }
        std::string baseIdStr;
        baseIdStr.append(Ir::Intf::GetFactory()->GetI32()->Name()).append(" ").append(std::to_string(baseId));
        if (i < n - 1)
        {
            baseIdStr.append(",");
        }
        formatter.WriteLine(baseIdStr);
    }
    formatter.DecIndent();
    formatter.WriteLine("]");
    std::unique_ptr<Ir::Intf::Type> pointerToExceptionTable(Cm::IrIntf::Pointer(exceptionBaseIdArrayType.get(), 1));
    pointerToExceptionTable->SetOwned();
    formatter.WriteLine("@$exception$base$id$table$addr = constant i32* bitcast (" + pointerToExceptionTable->Name() + " @$exception$base$id$table to i32*)");
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

} } // namespace Cm::Sym
