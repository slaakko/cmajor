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
#include <Cm.Sym/NamespaceSymbol.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <stdexcept>

namespace Cm { namespace Core {

IrFunctionRepository::IrFunctionRepository() : exceptionCodeParam(nullptr)
{
}

Ir::Intf::Parameter* IrFunctionRepository::CreateIrParameter(Cm::Sym::ParameterSymbol* parameter)
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
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        parameterName.append(Cm::IrIntf::GetPrivateSeparator()).append("p");
    }
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
        Ir::Intf::Function* irFun = i->second;
        return irFun;
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
        if (function->CanThrow())
        {
            Ir::Intf::Type* exceptionCodeParamType = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI32(), 1);
            Own(exceptionCodeParamType);
            exceptionCodeParam = Cm::IrIntf::CreateParameter(Cm::IrIntf::GetExceptionCodeParamName(), exceptionCodeParamType);
            Own(exceptionCodeParam);
            irParameters.push_back(exceptionCodeParam);
            irParameterTypes.push_back(exceptionCodeParamType->Clone());
        }
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
        if (!Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::unit_test))
        {
            if (functionGroupName.empty())
            {
                functionGroupName = "main";
            }
            else if (functionGroupName == "main")
            {
                functionGroupName = "user" + Cm::IrIntf::GetPrivateSeparator() + "main";
            }
        }
        functionName = functionGroupName;
    }
    if (!function->IsCDecl() && !function->IsStaticConstructor())
    {
        if (function->IsConversionFunction())
        {
            functionName = Cm::Sym::MangleName(function->Ns()->FullName(), "cv_" + Cm::Sym::MakeAssemblyName(function->GetReturnType()->FullName()), function->TypeArguments(),  function->Parameters());
        }
        else if (function->IsStatic())
        {
            functionName = Cm::Sym::MangleName(function->Class()->FullName(), functionGroupName, function->TypeArguments(), function->Parameters());
        }
        else
        {
            functionName = Cm::Sym::MangleName(function->Ns()->FullName(), functionGroupName, function->TypeArguments(), function->Parameters());
        }
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

Ir::Intf::Object* IrFunctionRepository::GetFunctionId(Cm::Sym::FunctionSymbol* function, Cm::Sym::TypeSymbol* functionPtrPtrType)
{
    FunctionIdMapIt i = functionIdMap.find(function);
    if (i != functionIdMap.end())
    {
        return i->second;
    }
    std::string functionIdName = Cm::Sym::MangleName(function->Ns()->FullName(), function->GroupName(), function->TypeArguments(), function->Parameters());
    Ir::Intf::Object* functionId = Cm::IrIntf::CreateGlobal(functionIdName, functionPtrPtrType->GetIrType());
    functionIdMap[function] = functionId;
    Own(functionId);
    return functionId;
}

Ir::Intf::Type* IrFunctionRepository::CreateIrPointerToDelegateType(Cm::Sym::DelegateTypeSymbol* delegateType)
{
    Cm::Sym::TypeSymbol* returnType = delegateType->GetReturnType();
    Ir::Intf::Type* irReturnType = returnType->GetIrType()->Clone();
    std::vector<Ir::Intf::Type*> irParameterTypes;
    for (Cm::Sym::ParameterSymbol* parameter : delegateType->Parameters())
    {
        Cm::Sym::TypeSymbol* parameterType = parameter->GetType();
        Ir::Intf::Type* irParameterType = parameterType->GetIrType()->Clone();
        irParameterTypes.push_back(irParameterType);
    }
    if (!delegateType->IsNothrow())
    {
        Ir::Intf::Type* exceptionCodeParamType = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI32(), 1);
        irParameterTypes.push_back(exceptionCodeParamType);
    }
    Ir::Intf::Type* irFunctionType = Cm::IrIntf::CreateFunctionType(irReturnType, irParameterTypes);
    Own(irFunctionType);
    Ir::Intf::Type* delegatePointerType = Cm::IrIntf::Pointer(irFunctionType, 2);
    Own(delegatePointerType);
    return delegatePointerType;
}

Ir::Intf::Type* IrFunctionRepository::GetFunPtrIrType(Cm::Sym::FunctionSymbol* fun)
{
    CreateIrFunction(fun);
    IrFunTypeMapIt i = irFunPtrMap.find(fun);
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

void IrFunctionRepository::Own(Ir::Intf::Object* object)
{
    if (!object->Owned())
    {
        object->SetOwned();
        ownedObjects.push_back(std::unique_ptr<Ir::Intf::Object>(object));
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
