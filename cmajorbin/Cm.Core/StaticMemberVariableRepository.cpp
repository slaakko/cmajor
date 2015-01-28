/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/StaticMemberVariableRepository.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/NameMangling.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Core {

void StaticMemberVariableRepository::Add(Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol)
{
    std::string assemblyName = Cm::Sym::MakeAssemblyName(staticMemberVariableSymbol->Class()->FullName() + "." + staticMemberVariableSymbol->Name());
    Ir::Intf::Object* irObject = Cm::IrIntf::CreateGlobal(assemblyName, Cm::IrIntf::Pointer(staticMemberVariableSymbol->GetType()->GetIrType(), 1));
    StaticMemberVariableMapIt i = staticMemberVariableMap.find(staticMemberVariableSymbol);
    if (i != staticMemberVariableMap.end())
    {
        throw Cm::Core::Exception("static member variable '" + staticMemberVariableSymbol->FullName() + "' already added to repository", staticMemberVariableSymbol->GetSpan());
    }
    staticMemberVariableMap[staticMemberVariableSymbol] = irObject;
    ownedIrObjects.push_back(std::unique_ptr<Ir::Intf::Object>(irObject));
}

Ir::Intf::Object* StaticMemberVariableRepository::GetStaticMemberVariableIrObject(Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol)
{
    StaticMemberVariableMapIt i = staticMemberVariableMap.find(staticMemberVariableSymbol);
    if (i != staticMemberVariableMap.end())
    {
        return i->second;
    }
    throw Cm::Core::Exception("static member variable '" + staticMemberVariableSymbol->FullName() + "' not found in repository", staticMemberVariableSymbol->GetSpan());
}

void StaticMemberVariableRepository::Write(Cm::Util::CodeFormatter& codeFormatter)
{
    for (const std::pair<Cm::Sym::MemberVariableSymbol*, Ir::Intf::Object*>& p : staticMemberVariableMap)
    {
        Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol = p.first;
        Ir::Intf::Object* irObject = p.second;
        std::string declaration = irObject->Name();
        declaration.append(" = global ").append(staticMemberVariableSymbol->GetType()->GetIrType()->Name());
        Ir::Intf::Object* defaultValue = staticMemberVariableSymbol->GetType()->GetDefaultIrValue();
        if (defaultValue)
        {
            declaration.append(1, ' ').append(defaultValue->Name());
        }
        else
        {
            declaration.append(" zeroinitializer");
        }
        codeFormatter.WriteLine(declaration);
    }
}

} } // namespace Cm::Core
