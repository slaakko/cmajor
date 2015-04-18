/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/ExternalConstantRepository.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Core {

ExternalConstantRepository::ExternalConstantRepository() : exceptionBaseIdTable(nullptr)
{
}

ExternalConstantRepository::~ExternalConstantRepository()
{
}

Ir::Intf::Global* ExternalConstantRepository::GetExceptionBaseIdTable()
{
    exceptionBaseIdTable = Cm::IrIntf::CreateGlobal(Cm::IrIntf::GetExceptionBaseIdTableName(), Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI32(), 2));
    ownedObjects.push_back(std::unique_ptr<Ir::Intf::Object>(exceptionBaseIdTable));
    return exceptionBaseIdTable;
}

void LlvmExternalConstantRepository::Write(Cm::Util::CodeFormatter& codeFormatter)
{
    if (ExceptionBaseIdTable())
    {
        codeFormatter.WriteLine("@" + Cm::IrIntf::GetExceptionBaseIdTableName() + " = external constant i32*");
    }
}

void CExternalConstantRepository::Write(Cm::Util::CodeFormatter& codeFormatter)
{
    if (ExceptionBaseIdTable())
    {
        codeFormatter.WriteLine("extern i32* " + Cm::IrIntf::GetExceptionBaseIdTableName() + ";");
    }
}

} } // namespace Cm::Core