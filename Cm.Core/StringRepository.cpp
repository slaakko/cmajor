/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/StringRepository.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Core {

StringRepository::~StringRepository()
{
}

int StringRepository::Install(const std::string& str)
{
    StringIntMapIt i = stringIntMap.find(str);
    if (i != stringIntMap.end())
    {
        return i->second;
    }
    int id = int(stringObjects.size());
    Ir::Intf::Object* stringConstant = Cm::IrIntf::CreateStringConstant(str);
	stringConstant->SetOwned();
    stringConstants.push_back(std::unique_ptr<Ir::Intf::Object>(stringConstant));
    std::string stringObjectName = Cm::IrIntf::GetStringValuePrefix() + std::to_string(id);
	Ir::Intf::Object* stringObject = Cm::IrIntf::CreateGlobal(stringObjectName, stringConstant->GetType());
	stringObject->SetOwned();
    stringObjects.push_back(std::unique_ptr<Ir::Intf::Object>(stringObject));
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

void LlvmStringRepository::Write(Cm::Util::CodeFormatter& codeFormatter)
{
    int n = int(StringObjects().size());
    for (int i = 0; i < n; ++i)
    {
        Ir::Intf::Object* stringObject = StringObjects()[i].get();
        Ir::Intf::Object* stringConstant = StringConstants()[i].get();
        codeFormatter.WriteLine(stringObject->Name() + " = private unnamed_addr constant " + stringConstant->GetType()->Name() + " " + stringConstant->Name());
    }
}

void CStringRepository::Write(Cm::Util::CodeFormatter& codeFormatter)
{
    int n = int(StringObjects().size());
    for (int i = 0; i < n; ++i)
    {
        Ir::Intf::Object* stringObject = StringObjects()[i].get();
        Ir::Intf::Object* stringConstant = StringConstants()[i].get();
        codeFormatter.WriteLine("static " + stringConstant->GetType()->Name() + " " + stringObject->Name() + " = " + stringConstant->Name() + ";");
    }
}

} } // namespace Cm::Core