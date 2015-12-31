/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/ExternalConstantRepository.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Core {

ExternalConstantRepository::ExternalConstantRepository() : exceptionBaseIdTable(nullptr), classHierarchyTable(nullptr)
{
}

ExternalConstantRepository::~ExternalConstantRepository()
{
}

Ir::Intf::Global* ExternalConstantRepository::GetExceptionBaseIdTable()
{
    exceptionBaseIdTable = Cm::IrIntf::CreateGlobal(Cm::IrIntf::GetExceptionBaseIdTableName(), Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI32(), 2));
    exceptionBaseIdTable->SetOwned();
    ownedObjects.push_back(std::unique_ptr<Ir::Intf::Object>(exceptionBaseIdTable));
    return exceptionBaseIdTable;
}

Ir::Intf::Global* ExternalConstantRepository::GetClassHierarchyTable()
{
    classHierarchyTable = Cm::IrIntf::CreateGlobal(Cm::IrIntf::GetClassHierarchyTableName(), Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI64(), 2));
    classHierarchyTable->SetOwned();
    ownedObjects.push_back(std::unique_ptr<Ir::Intf::Object>(classHierarchyTable));
    return classHierarchyTable;
}

void LlvmExternalConstantRepository::Write(Cm::Util::CodeFormatter& codeFormatter)
{
    if (ExceptionBaseIdTable())
    {
        codeFormatter.WriteLine("@" + Cm::IrIntf::GetExceptionBaseIdTableName() + " = external constant i32*");
    }
    if (ClassHierarchyTable())
    {
        codeFormatter.WriteLine("@" + Cm::IrIntf::GetClassHierarchyTableName() + " = external constant i64*");
    }
}

void CExternalConstantRepository::Write(Cm::Util::CodeFormatter& codeFormatter)
{
    if (ExceptionBaseIdTable())
    {
        codeFormatter.WriteLine("extern i32* " + Cm::IrIntf::GetExceptionBaseIdTableName() + ";");
    }
    if (ClassHierarchyTable())
    {
        codeFormatter.WriteLine("extern ui64* " + Cm::IrIntf::GetClassHierarchyTableName() + ";");
    }
}

} } // namespace Cm::Core