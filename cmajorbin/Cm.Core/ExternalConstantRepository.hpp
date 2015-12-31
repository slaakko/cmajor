/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
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
    virtual ~ExternalConstantRepository();
    Ir::Intf::Global* GetExceptionBaseIdTable();
    Ir::Intf::Global* GetClassHierarchyTable();
    virtual void Write(Cm::Util::CodeFormatter& codeFormatter) = 0;
protected:
    Ir::Intf::Global* ExceptionBaseIdTable() const { return exceptionBaseIdTable; }
    Ir::Intf::Global* ClassHierarchyTable() const { return classHierarchyTable; }
private:
    Ir::Intf::Global* exceptionBaseIdTable;
    Ir::Intf::Global* classHierarchyTable;
    std::vector<std::unique_ptr<Ir::Intf::Object>> ownedObjects;
};

class LlvmExternalConstantRepository : public ExternalConstantRepository
{
public:
    void Write(Cm::Util::CodeFormatter& codeFormatter) override;
};

class CExternalConstantRepository : public ExternalConstantRepository
{
public:
    void Write(Cm::Util::CodeFormatter& codeFormatter) override;
};


} } // namespace Cm::Core

#endif // CM_CORE_EXTERNAL_CONSTANT_REPOSITORY_INCLUDED
