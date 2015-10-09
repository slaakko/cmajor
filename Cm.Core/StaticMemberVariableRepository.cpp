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

StaticMemberVariableRepository::StaticMemberVariableRepository() : classTypeMap(nullptr)
{
}

StaticMemberVariableRepository::~StaticMemberVariableRepository()
{
}

void StaticMemberVariableRepository::Add(Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol)
{
    if (staticMemberVariableSymbol->Name() == "table")
    {
        int x = 0;
    }
    std::string assemblyName = Cm::Sym::MakeAssemblyName(staticMemberVariableSymbol->Class()->FullName() + "." + staticMemberVariableSymbol->Name());
    Ir::Intf::Object* irObject = Cm::IrIntf::CreateGlobal(assemblyName, Cm::IrIntf::Pointer(staticMemberVariableSymbol->GetType()->GetIrType(), 1));
    StaticMemberVariableMapIt i = staticMemberVariableMap.find(staticMemberVariableSymbol);
    if (i != staticMemberVariableMap.end())
    {
        throw Cm::Core::Exception("static member variable '" + staticMemberVariableSymbol->FullName() + "' already added to repository", staticMemberVariableSymbol->GetSpan());
    }
    nameMap[staticMemberVariableSymbol->FullName()] = staticMemberVariableSymbol;
    staticMemberVariableMap[staticMemberVariableSymbol] = irObject;
    ownedIrObjects.push_back(std::unique_ptr<Ir::Intf::Object>(irObject));
    if (staticMemberVariableSymbol->GetType()->IsClassTypeSymbol())
    {
        Cm::Sym::ClassTypeSymbol* classTypeSymbol = nullptr;
        if (classTypeMap)
        {
            std::unordered_map<std::string, Cm::Sym::ClassTypeSymbol*>::const_iterator i = classTypeMap->find(staticMemberVariableSymbol->GetType()->FullName());
            if (i != classTypeMap->cend())
            {
                classTypeSymbol = i->second;
            }
        }
        if (!classTypeSymbol)
        {
            classTypeSymbol = static_cast<Cm::Sym::ClassTypeSymbol*>(staticMemberVariableSymbol->GetType());
        }
        if (classTypeSymbol->Destructor())
        {
            Ir::Intf::Object* destructionNode = Cm::IrIntf::CreateGlobal(Cm::IrIntf::MakeDestructionNodeName(assemblyName),
                Cm::IrIntf::Pointer(Cm::IrIntf::CreateTypeName(Cm::IrIntf::GetDestructionNodeTypeName(), false), 1));
            ownedIrObjects.push_back(std::unique_ptr<Ir::Intf::Object>(destructionNode));
            destructionNodeMap[staticMemberVariableSymbol] = destructionNode;
        }
    }
}

Ir::Intf::Object* StaticMemberVariableRepository::GetStaticMemberVariableIrObject(Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol) const
{
    if (staticMemberVariableSymbol->Name() == "table")
    {
        int x = 0;
    }
    StaticMemberVariableMapIt i = staticMemberVariableMap.find(staticMemberVariableSymbol);
    if (i != staticMemberVariableMap.end())
    {
        return i->second;
    }
    std::unordered_map<std::string, Cm::Sym::MemberVariableSymbol*>::const_iterator j = nameMap.find(staticMemberVariableSymbol->FullName());
    if (j != nameMap.cend())
    {
        i = staticMemberVariableMap.find(j->second);
    }
    if (i != staticMemberVariableMap.end())
    {
        return i->second;
    }
    throw Cm::Core::Exception("static member variable '" + staticMemberVariableSymbol->FullName() + "' not found in repository", staticMemberVariableSymbol->GetSpan());
}

Ir::Intf::Object* StaticMemberVariableRepository::GetDestructionNode(Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol) const
{
    if (staticMemberVariableSymbol->Name() == "table")
    {
        int x = 0;
    }
    StaticMemberVariableMapIt i = destructionNodeMap.find(staticMemberVariableSymbol);
    if (i != destructionNodeMap.end())
    {
        return i->second;
    }
    std::unordered_map<std::string, Cm::Sym::MemberVariableSymbol*>::const_iterator j = nameMap.find(staticMemberVariableSymbol->FullName());
    if (j != nameMap.cend())
    {
        i = destructionNodeMap.find(j->second);
    }
    if (i != destructionNodeMap.end())
    {
        return i->second;
    }
    throw Cm::Core::Exception("destruction node for '" + staticMemberVariableSymbol->FullName() + "' not found in repository", staticMemberVariableSymbol->GetSpan());
}

void StaticMemberVariableRepository::SetClassTypeMap(std::unordered_map<std::string, Cm::Sym::ClassTypeSymbol*>& classTypeMap_)
{
    classTypeMap = &classTypeMap_;
}

void StaticMemberVariableRepository::Own(Ir::Intf::Type* type)
{
    ownedIrTypes.push_back(std::unique_ptr<Ir::Intf::Type>(type));
}

void LlvmStaticMemberVariableRepository::Write(Cm::Util::CodeFormatter& codeFormatter)
{
    for (const std::pair<Cm::Sym::MemberVariableSymbol*, Ir::Intf::Object*>& p : GetStaticMemberVariableMap())
    {
        Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol = p.first;
        Ir::Intf::Object* irObject = p.second;
        std::string declaration = irObject->Name();
        Cm::Sym::TypeSymbol* staticMemberVarType = staticMemberVariableSymbol->GetType();
        if (staticMemberVarType->IsArrayType())
        {
            Ir::Intf::Type* staticMemberVarIrArrayType = Cm::IrIntf::Array(staticMemberVarType->GetBaseType()->GetIrType(), staticMemberVarType->GetLastArrayDimension());
            Own(staticMemberVarIrArrayType);
            declaration.append(" = global ").append(staticMemberVarIrArrayType->Name());
        }
        else
        {
            declaration.append(" = global ").append(staticMemberVarType->GetIrType()->Name());
        }
        Ir::Intf::Object* defaultValue = staticMemberVarType->GetDefaultIrValue();
        if (defaultValue && !staticMemberVarType->IsArrayType())
        {
            declaration.append(1, ' ').append(defaultValue->Name());
        }
        else
        {
            declaration.append(" zeroinitializer");
        }
        codeFormatter.WriteLine(declaration);
    }
    for (const std::pair<Cm::Sym::MemberVariableSymbol*, Ir::Intf::Object*>& p : GetDestructionNodeMap())
    {
        Ir::Intf::Object* destructionNode = p.second;
        std::string destructionNodeDeclaration(destructionNode->Name());
        destructionNodeDeclaration.append(" = global %" + Cm::IrIntf::GetDestructionNodeTypeName() + " zeroinitializer");
        codeFormatter.WriteLine(destructionNodeDeclaration);
    }
}

void CStaticMemberVariableRepository::Write(Cm::Util::CodeFormatter& codeFormatter)
{
    for (const std::pair<Cm::Sym::MemberVariableSymbol*, Ir::Intf::Object*>& p : GetStaticMemberVariableMap())
    {
        Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol = p.first;
        Ir::Intf::Object* irObject = p.second;
        std::string declaration;
        Cm::Sym::TypeSymbol* staticMemberVarType = staticMemberVariableSymbol->GetType();
        if (staticMemberVarType->IsArrayType())
        {
            declaration.append(staticMemberVarType->GetBaseType()->GetIrType()->Name());
        }
        else
        {
            declaration.append(staticMemberVarType->GetIrType()->Name());
        }
        declaration.append(" ").append(irObject->Name());
        if (staticMemberVarType->IsArrayType())
        {
            declaration.append("[").append(std::to_string(staticMemberVarType->GetLastArrayDimension())).append("]");
        }
        declaration.append(";");
        codeFormatter.WriteLine(declaration);
    }
    for (const std::pair<Cm::Sym::MemberVariableSymbol*, Ir::Intf::Object*>& p : GetDestructionNodeMap())
    {
        Ir::Intf::Object* destructionNode = p.second;
        std::string destructionNodeDeclaration = Cm::IrIntf::GetDestructionNodeTypeName();
        destructionNodeDeclaration.append(" ").append(destructionNode->Name()).append(";");
        codeFormatter.WriteLine(destructionNodeDeclaration);
    }
}

} } // namespace Cm::Core
