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
#include <Cm.Sym/BasicTypeSymbol.hpp>
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

Ir::Intf::Object* LocalVariableIrObjectRepository::CreateLocalVariableIrObjectFor(Cm::Sym::Symbol *localVariableOrParameter)
{
    Cm::Sym::TypeSymbol* type = nullptr;
    if (localVariableOrParameter->IsLocalVariableSymbol())
    {
        Cm::Sym::LocalVariableSymbol* localVariableSymbol = static_cast<Cm::Sym::LocalVariableSymbol*>(localVariableOrParameter);
        type = localVariableSymbol->GetType();
    }
    else if (localVariableOrParameter->IsParameterSymbol())
    {
        Cm::Sym::ParameterSymbol* parameterSymbol = static_cast<Cm::Sym::ParameterSymbol*>(localVariableOrParameter);
        type = parameterSymbol->GetType();
    }
    std::string assemblyName = MakeUniqueAssemblyName(localVariableOrParameter->Name());
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
    localVariableObjectMap[localVariableOrParameter] = localVariableObject;
    ownedIrObjects.push_back(std::unique_ptr<Ir::Intf::Object>(localVariableObject));
    return localVariableObject;
}

Ir::Intf::Object* LocalVariableIrObjectRepository::GetLocalVariableIrObject(Cm::Sym::Symbol* localVariableOrParameter)
{
    LocalVariableObjectMapIt i = localVariableObjectMap.find(localVariableOrParameter);
    if (i != localVariableObjectMap.end())
    {
        return i->second;
    }
    throw std::runtime_error("local variable '" + localVariableOrParameter->Name() + "' not found");
}

FunctionEmitter::FunctionEmitter(Cm::Util::CodeFormatter& codeFormatter_, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_) : 
    Cm::BoundTree::Visitor(true), codeFormatter(codeFormatter_), emitter(nullptr), genFlags(Cm::Core::GenFlags::none), typeRepository(typeRepository_), 
    irFunctionRepository(irFunctionRepository_), compoundResult(emitter.get(), Cm::Core::GenFlags::none), currentCompileUnit(nullptr)
{
}

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
    Cm::Sym::FunctionSymbol* currentFunction = boundFunction.GetFunctionSymbol();
    currentCompileUnit = currentFunction->CompileUnit();
    Ir::Intf::Function* irFunction = irFunctionRepository.CreateIrFunction(currentFunction);
    emitter.reset(new Cm::Core::Emitter(irFunction));
    irFunction->SetComment(boundFunction.GetFunctionSymbol()->FullName());
    for (Cm::Sym::ParameterSymbol* parameter : boundFunction.GetFunctionSymbol()->Parameters())
    {
        Ir::Intf::Object* localVariableIrObject = localVariableIrObjectRepository.CreateLocalVariableIrObjectFor(parameter);
        emitter->Emit(Cm::IrIntf::Alloca(parameter->GetType()->GetIrType(), localVariableIrObject));
    }
    for (Cm::Sym::LocalVariableSymbol* localVariable : boundFunction.LocalVariables())
    {
        Ir::Intf::Object* localVariableIrObject = localVariableIrObjectRepository.CreateLocalVariableIrObjectFor(localVariable);
        emitter->Emit(Cm::IrIntf::Alloca(localVariable->GetType()->GetIrType(), localVariableIrObject));
    }
}

void FunctionEmitter::EndVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
    Ir::Intf::Function* irFunction = emitter->GetIrFunction();
    Cm::Core::GenResult result = resultStack.Pop();
    if (!irFunction->LastInstructionIsRet())
    {
        Cm::Sym::TypeSymbol* returnType = boundFunction.GetFunctionSymbol()->GetReturnType();
        Ir::Intf::LabelObject* retLabel = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(retLabel);
        emitter->AddNextInstructionLabel(retLabel);
        if (!returnType || returnType->IsVoidTypeSymbol())
        {
            emitter->Emit(Cm::IrIntf::Ret());
        }
        else
        {
            Ir::Intf::Object* retval = returnType->GetIrType()->CreateDefaultValue();
            emitter->Own(retval);
            emitter->Emit(Cm::IrIntf::Ret(retval));
        }
        result.BackpatchNextTargets(retLabel);
    }
    irFunction->Clean();
    irFunction->WriteDefinition(codeFormatter, false, false);
    for (Ir::Intf::Function* externalFunction : externalFunctions)
    {
        externalFunction->WriteDeclaration(codeFormatter, false, false);
    }
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundLiteral& boundLiteral)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Ir::Intf::Object* literalValue = boundLiteral.GetValue()->CreateIrObject();
    emitter->Own(literalValue);
    result.SetMainObject(literalValue);
    if (boundLiteral.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        GenJumpingBoolCode(result);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundConstant& boundConstant)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Ir::Intf::Object* constantValue = boundConstant.Symbol()->GetValue()->CreateIrObject();
    emitter->Own(constantValue);
    result.SetMainObject(constantValue);
    if (boundConstant.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        GenJumpingBoolCode(result);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundLocalVariable& boundLocalVariable)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    if (boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef) || boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue))
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
        if (boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
        {
            GenJumpingBoolCode(result);
        }
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundParameter& boundParameter)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    if (boundParameter.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef) || boundParameter.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue))
    {
        result.SetMainObject(localVariableIrObjectRepository.GetLocalVariableIrObject(boundParameter.Symbol()));
    }
    else
    {
        Cm::Sym::TypeSymbol* type = boundParameter.Symbol()->GetType();
        result.SetMainObject(type);
        result.AddObject(localVariableIrObjectRepository.GetLocalVariableIrObject(boundParameter.Symbol()));
        Cm::IrIntf::Init(*emitter, type->GetIrType(), result.Arg1(), result.MainObject());
        Cm::Sym::TypeSymbol* plainType = typeRepository.MakePlainType(type);
        if (boundParameter.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode) && plainType->IsBoolTypeSymbol())
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
    if (boundConversion.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        result.SetGenJumpingBoolCode();
    }
    Cm::Core::GenResult operandResult = resultStack.Pop();
    result.Merge(operandResult);
    Cm::Sym::FunctionSymbol* conversionFun = boundConversion.ConversionFun();
    GenerateCall(conversionFun, result);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundUnaryOp& boundUnaryOp)
{
    Cm::Core::GenResult operandResult = resultStack.Pop();
    Cm::Core::GenResult result(emitter.get(), genFlags);
    result.SetMainObject(boundUnaryOp.GetType());
    if (boundUnaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        result.SetGenJumpingBoolCode();
    }
    result.Merge(operandResult);
    Cm::Sym::FunctionSymbol* op = boundUnaryOp.GetFunction();
    GenerateCall(op, result);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundBinaryOp& boundBinaryOp)
{
    Cm::Core::GenResult right = resultStack.Pop();
    Cm::Core::GenResult left = resultStack.Pop();
    Cm::Core::GenResult result(emitter.get(), genFlags);
    result.SetMainObject(boundBinaryOp.GetType());
    if (boundBinaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        result.SetGenJumpingBoolCode();
    }
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
    if (functionCall.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        result.SetGenJumpingBoolCode();
    }
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

void FunctionEmitter::BeginVisitStatement(Cm::BoundTree::BoundStatement& statement)
{
    if (statement.RequiresLabel())
    {
        genFlags = Cm::Core::GenFlags::label;
    }
    else
    {
        genFlags = Cm::Core::GenFlags::none;
    }
}

void FunctionEmitter::EndVisitStatement(Cm::BoundTree::BoundStatement& statement)
{
    Cm::Core::GenResult statementResult = resultStack.Pop();
    if (statementResult.GetLabel())
    {
        compoundResult.BackpatchNextTargets(statementResult.GetLabel());
    }
    compoundResult.Merge(statementResult);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundCompoundStatement& boundCompoundStatement)
{
    resultStack.Push(std::move(compoundResult));
    compoundResult = Cm::Core::GenResult(emitter.get(), Cm::Core::GenFlags::none);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundReceiveStatement& boundReceiveStatement)
{
    Cm::Sym::ParameterSymbol* parameterSymbol = boundReceiveStatement.GetParameterSymbol();
    Cm::Core::GenResult result(emitter.get(), genFlags);
    result.SetMainObject(localVariableIrObjectRepository.GetLocalVariableIrObject(parameterSymbol));
    Ir::Intf::Parameter* irParameter = Cm::Core::CreateIrParameter(parameterSymbol);
    emitter->Own(irParameter);
    result.AddObject(irParameter);
    Cm::Sym::FunctionSymbol* ctor = boundReceiveStatement.Constructor();
    GenerateCall(ctor, result);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundReturnStatement& boundReturnStatement) 
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    if (boundReturnStatement.ReturnsValue())
    {
        Cm::Sym::FunctionSymbol* ctor = boundReturnStatement.Constructor();
        Cm::Core::GenResult result(emitter.get(), genFlags);
        result.SetMainObject(boundReturnStatement.GetReturnType());
        Cm::Core::GenResult retValResult = resultStack.Pop();
        result.Merge(retValResult);
        GenerateCall(ctor, result);
        emitter->Emit(Cm::IrIntf::Ret(result.MainObject()));
    }
    else
    {
        emitter->Emit(Cm::IrIntf::Ret());
    }
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

void FunctionEmitter::Visit(Cm::BoundTree::BoundAssignmentStatement& boundAssignmentStatement)
{
    Cm::Core::GenResult sourceResult = resultStack.Pop();
    Cm::Core::GenResult result = resultStack.Pop(); // todo: lvalue genFlag
    result.Merge(sourceResult);
    Cm::Sym::FunctionSymbol* assignment = boundAssignmentStatement.Assignment();
    GenerateCall(assignment, result);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundSimpleStatement& boundSimpleStatement)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    if (boundSimpleStatement.HasExpression())
    {
        Cm::Core::GenResult expressionResult = resultStack.Pop();
        result.Merge(expressionResult);
    }
    else
    {
        result.SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)));
        GenerateCall(irFunctionRepository.GetDoNothingFunction(), result);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundConditionalStatement& boundConditionalStatement)
{
    PushSkipContent();
}

void FunctionEmitter::EndVisit(Cm::BoundTree::BoundConditionalStatement& boundConditionalStatement)
{
    PopSkipContent();
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Cm::Core::GenResult conditionResult = resultStack.Pop();
    Cm::BoundTree::BoundStatement* thenS = boundConditionalStatement.ThenS();
    thenS->Accept(*this);
    Cm::Core::GenResult thenResult = resultStack.Pop();
    conditionResult.BackpatchTrueTargets(thenResult.GetLabel());
    result.Merge(thenResult);
    Cm::BoundTree::BoundStatement* elseS = boundConditionalStatement.ElseS();
    if (elseS)
    {
        elseS->Accept(*this);
        Cm::Core::GenResult elseResult = resultStack.Pop();
        conditionResult.BackpatchFalseTargets(elseResult.GetLabel());
        result.Merge(elseResult);
    }
    else
    {
        result.MergeTargets(result.NextTargets(), conditionResult.FalseTargets());
    }
    result.Merge(conditionResult);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundWhileStatement& boundWhileStatement)
{
    PushSkipContent();
}

void FunctionEmitter::EndVisit(Cm::BoundTree::BoundWhileStatement& boundWhileStatement)
{
    PopSkipContent();
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Cm::Core::GenResult conditionResult = resultStack.Pop();
    Cm::BoundTree::BoundStatement* statement = boundWhileStatement.Statement();
    statement->Accept(*this);
    Cm::Core::GenResult statementResult = resultStack.Pop();
    emitter->Emit(Cm::IrIntf::Br(conditionResult.GetLabel()));
    conditionResult.BackpatchTrueTargets(statementResult.GetLabel());
    statementResult.BackpatchNextTargets(conditionResult.GetLabel());
    result.MergeTargets(result.NextTargets(), conditionResult.FalseTargets());
    result.Merge(statementResult);
    result.Merge(conditionResult);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::GenerateCall(Ir::Intf::Function* fun, Cm::Core::GenResult& result)
{
    Ir::Intf::Instruction* callInst = Cm::IrIntf::Call(result.MainObject(), fun, result.Args());
    emitter->Emit(callInst);
    if (fun->IsDoNothingFunction())
    {
        externalFunctions.insert(fun);
    }
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
        if (fun->CompileUnit() != currentCompileUnit)
        {
            externalFunctions.insert(irFunction);
        }
        GenerateCall(irFunction, result);
        bool boolResult = false;
        Cm::Sym::TypeSymbol* returnType = fun->GetReturnType();
        if (returnType)
        {
            Cm::Sym::TypeSymbol* plainReturnType = typeRepository.MakePlainType(returnType);
            boolResult = plainReturnType->IsBoolTypeSymbol();
        }
        if (result.GenJumpingBoolCode() && boolResult)
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
    Ir::Intf::RegVar* flag = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI1());
    emitter->Own(flag);
    Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI1(), result.MainObject(), flag);
    emitter->Emit(Cm::IrIntf::Br(flag, trueLabel, falseLabel));
    result.AddTrueTarget(trueLabel);
    result.AddFalseTarget(falseLabel);
    Cm::Core::ResetFlag(Cm::Core::GenFlags::genJumpingBoolCode, genFlags);
}

} } // namespace Cm::Emit