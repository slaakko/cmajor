/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/StringRepository.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Core {

int StringRepository::Install(const std::string& str)
{
    StringIntMapIt i = stringIntMap.find(str);
    if (i != stringIntMap.end())
    {
        return i->second;
    }
    int id = int(stringObjects.size());
    Ir::Intf::Object* stringConstant = Cm::IrIntf::CreateStringConstant(str);
    stringConstants.push_back(std::unique_ptr<Ir::Intf::Object>(stringConstant));
    std::string stringObjectName = ".s" + std::to_string(id);
    stringObjects.push_back(std::unique_ptr<Ir::Intf::Object>(Cm::IrIntf::CreateGlobal(stringObjectName, stringConstant->GetType())));
    stringIntMap[str] = id;
    return id;
}

Ir::Intf::Object* StringRepository::GetStringConstant(int id) const
{
    return stringConstants[id].get();
}

Ir::Intf::Object* StringRepository::GetStringObject(int id) const
{
    return stringObjects[id].get();
}

void StringRepository::Write(Cm::Util::CodeFormatter& codeFormatter)
{
    int n = int(stringObjects.size());
    for (int i = 0; i < n; ++i)
    {
        Ir::Intf::Object* stringObject = stringObjects[i].get();
        Ir::Intf::Object* stringConstant = stringConstants[i].get();
        codeFormatter.WriteLine(stringObject->Name() + " = private unnamed_addr constant " + stringConstant->GetType()->Name() + " " + stringConstant->Name());
    }
}

} } // namespace Cm::Core
