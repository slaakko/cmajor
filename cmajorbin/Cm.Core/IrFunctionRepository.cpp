/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/IrFunctionRepository.hpp>
#include <Cm.Sym/NameMangling.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/Namespace.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Core {

Ir::Intf::Parameter* CreateIrParameter(Cm::Sym::ParameterSymbol* parameter)
{
    Ir::Intf::Type* irParameterType = nullptr;
    if (parameter->GetType()->IsClassTypeSymbol())
    {
        irParameterType = Cm::IrIntf::Pointer(parameter->GetType()->GetIrType(), 1);
    }
    else
    {
        irParameterType = parameter->GetType()->GetIrType();
    }
    std::string parameterName = parameter->Name();
    parameterName.append(Cm::IrIntf::GetPrivateSeparator()).append("p");
    Ir::Intf::Parameter* irParameter = Cm::IrIntf::CreateParameter(parameterName, irParameterType);
    return irParameter;
}

IrFunctionRepository::IrFunctionRepository() : exceptionCodeParam(nullptr)
{
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
    std::string functionName;
    std::string functionGroupName;
    Ir::Intf::Type* irReturnType = nullptr;
    std::vector<Ir::Intf::Parameter*> irParameters;
    std::vector<Ir::Intf::Type*> irParameterTypes;
    if (function->IsStaticConstructor())
    {
        functionName = "__sc_" + function->Class()->GetMangleId();
        irReturnType = Cm::IrIntf::Void();
        Own(irReturnType);
    }
    else
    {
        bool returnsClassObjectByValue = function->ReturnsClassObjectByValue();
        if (!returnsClassObjectByValue)
        {
            Cm::Sym::TypeSymbol* returnType = function->GetReturnType();
            if (returnType)
            {
                irReturnType = returnType->GetIrType();
            }
        }
        if (!irReturnType)
        {
            irReturnType = Cm::IrIntf::Void();
            Own(irReturnType);
        }
        for (Cm::Sym::ParameterSymbol* parameter : function->Parameters())
        {
            Ir::Intf::Parameter* irParameter = CreateIrParameter(parameter);
            Own(irParameter);
            irParameters.push_back(irParameter);
            irParameterTypes.push_back(irParameter->GetType()->Clone());
        }
        if (returnsClassObjectByValue)
        {
            Ir::Intf::Type* classObjectResultParamType = Cm::IrIntf::Pointer(function->GetReturnType()->GetIrType(), 1);
            Own(classObjectResultParamType);
            Ir::Intf::Parameter* irClassObjectParameter = Cm::IrIntf::CreateParameter(Cm::IrIntf::GetClassObjectResultParamName(), classObjectResultParamType);
            Own(irClassObjectParameter);
            irParameters.push_back(irClassObjectParameter);
            irParameterTypes.push_back(classObjectResultParamType->Clone());
            function->SetClassObjectResultIrParam(irClassObjectParameter);
        }
        if (function->CanThrow())
        {
            Ir::Intf::Type* exceptionCodeParamType = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI32(), 1);
            Own(exceptionCodeParamType);
            exceptionCodeParam = Cm::IrIntf::CreateParameter(Cm::IrIntf::GetExceptionCodeParamName(), exceptionCodeParamType);
            Own(exceptionCodeParam);
            irParameters.push_back(exceptionCodeParam);
            irParameterTypes.push_back(exceptionCodeParamType->Clone());
        }
        functionGroupName = function->GroupName();
        if (functionGroupName.empty())
        {
            functionGroupName = "main";
        }
        else if (functionGroupName == "main")
        {
            functionGroupName = "user" + Cm::IrIntf::GetPrivateSeparator() + "main";
        }
        functionName = functionGroupName;
    }
    if (!function->IsCDecl() && !function->IsStaticConstructor())
    {
        functionName = Cm::Sym::MangleName(function->Ns()->FullName(), functionGroupName, function->TypeArguments(), function->Parameters());
    }
    Ir::Intf::Function* irFunction = Cm::IrIntf::CreateFunction(functionName, irReturnType, irParameters);
    ownedIrFunctions.push_back(std::unique_ptr<Ir::Intf::Function>(irFunction));
    Ir::Intf::Type* irFunPtrType = Cm::IrIntf::Pointer(Cm::IrIntf::CreateFunctionType(irReturnType->Clone(), irParameterTypes), 1);
    ownedIrTypes.push_back(std::unique_ptr<Ir::Intf::Type>(irFunPtrType));
    irFunPtrType->SetOwned();
    irFunPtrMap[function] = irFunPtrType;
    irFunctionMap[function] = irFunction;
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
