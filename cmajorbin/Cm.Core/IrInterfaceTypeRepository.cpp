/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/IrInterfaceTypeRepository.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Core {

IrInterfaceTypeRepository::~IrInterfaceTypeRepository()
{
}

void IrInterfaceTypeRepository::Add(Cm::Sym::InterfaceTypeSymbol* interfaceType)
{
    interfaceTypes.insert(interfaceType);
}

void LlvmIrInterfaceTypeRepository::Write(Cm::Util::CodeFormatter& formatter)
{
    for (Cm::Sym::InterfaceTypeSymbol* intf : InterfaceTypes())
    {
        std::string tagName = intf->GetMangleId() + "_";
        std::vector<Ir::Intf::Type*> memberTypes;
        std::vector<std::string> memberNames;
        for (const Cm::Sym::MemberVariableSymbol* memberVar : intf->MemberVars())
        {
            memberTypes.push_back(memberVar->GetType()->GetIrType()->Clone());
            memberNames.push_back(memberVar->Name());
        }
        std::unique_ptr<Ir::Intf::Type> irTypeDeclaration(Cm::IrIntf::Structure(tagName, memberTypes, memberNames));
        std::string typeDeclaration = intf->GetIrType()->Name() + " = type " + irTypeDeclaration->Name();
        formatter.WriteLine(typeDeclaration);
    }
}

void CIrInterfaceTypeRepository::Write(Cm::Util::CodeFormatter& formatter)
{
    for (Cm::Sym::InterfaceTypeSymbol* intf : InterfaceTypes())
    {
        std::string tagName = intf->GetMangleId() + "_";
        std::vector<Ir::Intf::Type*> memberTypes;
        std::vector<std::string> memberNames;
        for (const Cm::Sym::MemberVariableSymbol* memberVar : intf->MemberVars())
        {
            memberTypes.push_back(memberVar->GetType()->GetIrType()->Clone());
            memberNames.push_back(memberVar->Name());
        }
        std::unique_ptr<Ir::Intf::Type> irTypeDeclaration(Cm::IrIntf::Structure(tagName, memberTypes, memberNames));
        std::unique_ptr<Ir::Intf::Type> typeName(Cm::IrIntf::CreateClassTypeName(intf->FullName()));
        formatter.WriteLine("typedef struct " + intf->GetMangleId() + "_ " + typeName->Name() + ";");
        formatter.WriteLine("typedef " + irTypeDeclaration->Name() + " " + typeName->Name() + ";");
    }
}

} } // namespace Cm::Core
