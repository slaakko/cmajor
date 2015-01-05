/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/IrClassTypeRepository.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Core {

void IrClassTypeRepository::AddClassType(Cm::Sym::ClassTypeSymbol* classTypeSymbol)
{
    classTypes.insert(classTypeSymbol);
}

void IrClassTypeRepository::Write(Cm::Util::CodeFormatter& codeFormatter)
{
    for (Cm::Sym::ClassTypeSymbol* classType : classTypes)
    {
        Write(classType, codeFormatter);
    }
}

void IrClassTypeRepository::Write(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter)
{
    std::vector<Ir::Intf::Type*> memberTypes;
    std::vector<std::string> memberNames;
    std::string tagName = classType->GetMangleId() + "_";
    if (classType->BaseClass())
    {
        memberTypes.push_back(classType->BaseClass()->GetIrType()->Clone());
        memberNames.push_back("__base");
    }
    for (Cm::Sym::MemberVariableSymbol* memberVariable : classType->MemberVariables())
    {
        memberTypes.push_back(memberVariable->GetType()->GetIrType()->Clone());
        memberNames.push_back(memberVariable->Name());
    }
    std::unique_ptr<Ir::Intf::Type> irTypeDeclaration(Cm::IrIntf::Structure(tagName, memberTypes, memberNames));
    std::string typeDeclaration = classType->GetIrType()->Name() + " = type " + irTypeDeclaration->Name();
    codeFormatter.WriteLine(typeDeclaration);
}

} } // namespace Cm::Core
