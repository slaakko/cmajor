/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_EXTERNAL_CONSTANT_REPOSITORY_INCLUDED
#define CM_CORE_EXTERNAL_CONSTANT_REPOSITORY_INCLUDED
#include <Ir.Intf/Constant.hpp>
#include <Cm.Util/CodeFormatter.hpp>

namespace Cm { namespace Core {

class ExternalConstantRepository
{
public:
    ExternalConstantRepository();
    Ir::Intf::Global* GetExceptionBaseIdTable();
    void Write(Cm::Util::CodeFormatter& codeFormatter);
private:
    Ir::Intf::Global* exceptionBaseIdTable;
    std::vector<std::unique_ptr<Ir::Intf::Object>> ownedObjects;
};

} } // namespace Cm::Core

#endif // CM_CORE_EXTERNAL_CONSTANT_REPOSITORY_INCLUDED
