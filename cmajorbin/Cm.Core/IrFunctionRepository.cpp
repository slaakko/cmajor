/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/IrFunctionRepository.hpp>
#include <Cm.Sym/NameMangling.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/Namespace.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Core {

Ir::Intf::Parameter* CreateIrParameter(Cm::Sym::ParameterSymbol* parameter)
{
    Ir::Intf::Type* irParameterType = parameter->GetType()->GetIrType();
    std::string parameterName = parameter->Name();
    parameterName.append(Cm::IrIntf::GetPrivateSeparator()).append("p");
    Ir::Intf::Parameter* irParameter = Cm::IrIntf::CreateParameter(parameterName, irParameterType);
    return irParameter;
}

Ir::Intf::Function* IrFunctionRepository::GetDoNothingFunction()
{
    if (!doNothingFunction)
    {
        doNothingFunction.reset(Cm::IrIntf::CreateDoNothingFunction());
    }
    return doNothingFunction.get();
}

Ir::Intf::Function* IrFunctionRepository::CreateIrFunction(Cm::Sym::FunctionSymbol* function)
{
    IrFunctionMapIt i = irFunctionMap.find(function);
    if (i != irFunctionMap.end())
    {
        return i->second;
    }
    Cm::Sym::TypeSymbol* returnType = function->GetReturnType();
    Ir::Intf::Type* irReturnType = returnType ? returnType->GetIrType() : nullptr;
    if (!irReturnType)
    {
        irReturnType = Cm::IrIntf::Void();
        Own(irReturnType);
    }
    std::vector<Ir::Intf::Parameter*> irParameters;
    std::vector<Ir::Intf::Type*> irParameterTypes;
    for (Cm::Sym::ParameterSymbol* parameter : function->Parameters())
    {
        Ir::Intf::Parameter* irParameter = CreateIrParameter(parameter);
        Own(irParameter);
        irParameters.push_back(irParameter);
        irParameterTypes.push_back(irParameter->GetType()->Clone());
    }
    std::string functionGroupName = function->GroupName();
    if (functionGroupName.empty())
    {
        functionGroupName = "main";
    }
    else if (functionGroupName == "main")
    {
        functionGroupName = "user" + Cm::IrIntf::GetPrivateSeparator() + "main";
    }
    std::string functionName = functionGroupName;
    if (!function->IsCDecl())
    {
        functionName = Cm::Sym::MangleName(function->Ns()->FullName(), functionGroupName, std::vector<Cm::Sym::TypeSymbol*>(), function->Parameters());
    }
    Ir::Intf::Function* irFunction = Cm::IrIntf::CreateFunction(functionName, irReturnType, irParameters);
    Ir::Intf::Type* irFunPtrType = Cm::IrIntf::Pointer(Cm::IrIntf::CreateFunctionType(irReturnType->Clone(), irParameterTypes), 1);
    ownedIrFunctions.push_back(std::unique_ptr<Ir::Intf::Function>(irFunction));
    ownedIrTypes.push_back(std::unique_ptr<Ir::Intf::Type>(irFunPtrType));
    irFunPtrType->SetOwned();
    irFunctionMap[function] = irFunction;
    irFunPtrMap[function] = irFunPtrType;
    return irFunction;
}

Ir::Intf::Type* IrFunctionRepository::GetFunPtrIrType(Cm::Sym::FunctionSymbol* fun)
{
    CreateIrFunction(fun);
    IrFunPtrMapIt i = irFunPtrMap.find(fun);
    if (i != irFunPtrMap.end())
    {
        return i->second;
    }
    throw std::runtime_error("ir function pointer type not created");
}

void IrFunctionRepository::Own(Ir::Intf::Type* type)
{
    if (!type->Owned())
    {
        type->SetOwned();
        ownedIrTypes.push_back(std::unique_ptr<Ir::Intf::Type>(type));
    }
}

void IrFunctionRepository::Own(Ir::Intf::Parameter* parameter)
{
    if (!parameter->Owned())
    {
        parameter->SetOwned();
        ownedIrParameters.push_back(std::unique_ptr<Ir::Intf::Parameter>(parameter));
    }
}

} } // namespace Cm::Core
