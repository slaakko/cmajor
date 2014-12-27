/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Emit/FunctionEmitter.hpp>
#include <Cm.BoundTree/BoundExpression.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.Sym/TypeRepository.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Emit {

std::string LocalVariableIrObjectRepository::MakeUniqueAssemblyName(const std::string& name)
{
    if (assemblyNames.find(name) == assemblyNames.end())
    {
        assemblyNames.insert(name);
        return name;
    }
    int assemblyNameCounter = 0;
    std::string assemblyName = name + Cm::IrIntf::GetPrivateSeparator() + std::to_string(assemblyNameCounter);
    while (assemblyNames.find(assemblyName) != assemblyNames.end())
    {
        ++assemblyNameCounter;
        assemblyName = name + Cm::IrIntf::GetPrivateSeparator() + std::to_string(assemblyNameCounter);
    }
    assemblyNames.insert(assemblyName);
    return assemblyName;
}

Ir::Intf::Object* LocalVariableIrObjectRepository::CreateLocalVariableIrObjectFor(Cm::Sym::LocalVariableSymbol *localVariable)
{
    Cm::Sym::TypeSymbol* type = localVariable->GetType();
    std::string assemblyName = MakeUniqueAssemblyName(localVariable->Name());
    Ir::Intf::Object* localVariableObject = nullptr;
    if (type->IsReferenceType() || type->IsRvalueRefType())
    {
        if (type->GetBaseType()->IsClassTypeSymbol())
        {
            localVariableObject = Cm::IrIntf::CreateStackVar(assemblyName, Cm::IrIntf::Pointer(type->GetIrType(), type->GetPointerCount() + 1));
        }
        else
        {
            localVariableObject = Cm::IrIntf::CreateRefVar(assemblyName, Cm::IrIntf::Pointer(type->GetIrType(), type->GetPointerCount() + 1));
        }
    }
    else
    {
        localVariableObject = Cm::IrIntf::CreateStackVar(assemblyName, Cm::IrIntf::Pointer(type->GetIrType(), type->GetPointerCount() + 1));
    }
    localVariableObjectMap[localVariable] = localVariableObject;
    ownedIrObjects.push_back(std::unique_ptr<Ir::Intf::Object>(localVariableObject));
    return localVariableObject;
}

Ir::Intf::Object* LocalVariableIrObjectRepository::GetLocalVariableIrObject(Cm::Sym::LocalVariableSymbol* localVariable)
{
    LocalVariableObjectMapIt i = localVariableObjectMap.find(localVariable);
    if (i != localVariableObjectMap.end())
    {
        return i->second;
    }
    throw std::runtime_error("local variable '" + localVariable->Name() + "' not found");
}

FunctionEmitter::FunctionEmitter(Cm::Util::CodeFormatter& codeFormatter_, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_) : 
    Cm::BoundTree::Visitor(true), codeFormatter(codeFormatter_), emitter(nullptr), genFlags(Cm::Core::GenFlags::none), typeRepository(typeRepository_), irFunctionRepository(irFunctionRepository_)
{
}

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
    Ir::Intf::Function* irFunction = irFunctionRepository.CreateIrFunction(boundFunction.GetFunctionSymbol());
    emitter.reset(new Cm::Core::Emitter(irFunction));
    irFunction->SetComment(boundFunction.GetFunctionSymbol()->FullName());
    for (Cm::Sym::LocalVariableSymbol* localVariable : boundFunction.LocalVariables())
    {
        Ir::Intf::Object* localVariableIrObject = localVariableIrObjectRepository.CreateLocalVariableIrObjectFor(localVariable);
        emitter->Emit(Cm::IrIntf::Alloca(localVariable->GetType()->GetIrType(), localVariableIrObject));
    }
}

void FunctionEmitter::EndVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
    Ir::Intf::Function* irFunction = emitter->GetIrFunction();
    if (!irFunction->LastInstructionIsRet())
    {
        emitter->Emit(Cm::IrIntf::Ret());
    }
    irFunction->WriteDefinition(codeFormatter, false, false);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundLiteral& boundLiteral)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Ir::Intf::Object* literalValue = boundLiteral.GetValue()->CreateIrObject();
    emitter->Own(literalValue);
    result.SetMainObject(literalValue);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundConstant& boundConstant)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Ir::Intf::Object* constantValue = boundConstant.Symbol()->GetValue()->CreateIrObject();
    emitter->Own(constantValue);
    result.SetMainObject(constantValue);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundLocalVariable& boundLocalVariable)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    if (boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef))
    {
        result.SetMainObject(localVariableIrObjectRepository.GetLocalVariableIrObject(boundLocalVariable.Symbol()));
    }
    else
    {
        Cm::Sym::TypeSymbol* type = boundLocalVariable.Symbol()->GetType();
        result.SetMainObject(type);
        result.AddObject(localVariableIrObjectRepository.GetLocalVariableIrObject(boundLocalVariable.Symbol()));
        Cm::IrIntf::Init(*emitter, type->GetIrType(), result.Arg1(), result.MainObject());
        Cm::Sym::TypeSymbol* plainType = typeRepository.MakePlainType(type);
        if (Cm::Core::GetFlag(Cm::Core::GenFlags::genJumpingBoolCode, genFlags) && plainType->IsBoolTypeSymbol())
        {
            GenJumpingBoolCode(result);
        }
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundConversion& boundConversion)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    result.SetMainObject(boundConversion.GetType());
    Cm::Core::GenResult operandResult = resultStack.Pop();
    result.Merge(operandResult);
    Cm::Sym::FunctionSymbol* conversionFun = boundConversion.ConversionFun();
    GenerateCall(conversionFun, result);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundBinaryOp& boundBinaryOp)
{
    Cm::Core::GenResult right = resultStack.Pop();
    Cm::Core::GenResult left = resultStack.Pop();
    Cm::Core::GenResult result(emitter.get(), genFlags);
    result.SetMainObject(boundBinaryOp.GetType());
    result.Merge(left);
    result.Merge(right);
    Cm::Sym::FunctionSymbol* op = boundBinaryOp.GetFunction();
    GenerateCall(op, result);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundFunctionCall& functionCall)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    result.SetMainObject(functionCall.GetType());
    for (std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : functionCall.Arguments())
    {
        argument->Accept(*this);
        Cm::Core::GenResult argResult = resultStack.Pop();
        result.Merge(argResult);
    }
    Cm::Sym::FunctionSymbol* fun = functionCall.GetFunction();
    GenerateCall(fun, result);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundConstructionStatement& boundConstructionStatement)
{
    Cm::Core::GenResult argResult = resultStack.Pop();
    Cm::Core::GenResult result = resultStack.Pop();
    result.Merge(argResult);
    Cm::Sym::FunctionSymbol* ctor = boundConstructionStatement.Constructor();
    GenerateCall(ctor, result);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::GenerateCall(Cm::Sym::FunctionSymbol* fun, Cm::Core::GenResult& result)
{
    if (fun->IsBasicTypeOp())
    {
        Cm::Core::BasicTypeOp* op = static_cast<Cm::Core::BasicTypeOp*>(fun);
        op->Generate(*emitter, result);
    }
    else
    {
        Ir::Intf::Function* irFunction = irFunctionRepository.CreateIrFunction(fun);
        Ir::Intf::Instruction* callInst = Cm::IrIntf::Call(result.MainObject(), irFunction, result.Args());
        emitter->Emit(callInst);
        bool boolResult = false;
        Cm::Sym::TypeSymbol* returnType = fun->GetReturnType();
        if (returnType)
        {
            Cm::Sym::TypeSymbol* plainReturnType = typeRepository.MakePlainType(returnType);
            boolResult = plainReturnType->IsBoolTypeSymbol();
        }
        if (Cm::Core::GetFlag(Cm::Core::GenFlags::genJumpingBoolCode, genFlags) && boolResult)
        {
            GenJumpingBoolCode(result);
        }
    }
}

void FunctionEmitter::GenJumpingBoolCode(Cm::Core::GenResult& result)
{
    Ir::Intf::LabelObject* trueLabel = Cm::IrIntf::CreateLabel();
    emitter->Own(trueLabel);
    Ir::Intf::LabelObject* falseLabel = Cm::IrIntf::CreateLabel();
    emitter->Own(falseLabel);
    Ir::Intf::RegVar* flag = Cm::IrIntf::CreateTemporaryRegVar(Cm::IrIntf::I1());
    emitter->Own(flag);
    Cm::IrIntf::Assign(*emitter, Cm::IrIntf::I1(), result.MainObject(), flag);
    emitter->Emit(Cm::IrIntf::Br(flag, trueLabel, falseLabel));
    result.AddTrueTarget(trueLabel);
    result.AddFalseTarget(falseLabel);
    Cm::Core::ResetFlag(Cm::Core::GenFlags::genJumpingBoolCode, genFlags);
}

} } // namespace Cm::Emit