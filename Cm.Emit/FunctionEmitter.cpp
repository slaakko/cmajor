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
            localVariableObject = Cm::IrIntf::CreateStackVar(assemblyName, Cm::IrIntf::Pointer(type->GetBaseType()->GetIrType(), type->GetPointerCount() + 1));
        }
        else
        {
            localVariableObject = Cm::IrIntf::CreateRefVar(assemblyName, Cm::IrIntf::Pointer(type->GetBaseType()->GetIrType(), type->GetPointerCount() + 1));
        }
    }
    else
    {
        if (type->GetBaseType()->IsVoidTypeSymbol())
        {
            localVariableObject = Cm::IrIntf::CreateStackVar(assemblyName, Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), type->GetPointerCount() + 1));
        }
        else
        {
            localVariableObject = Cm::IrIntf::CreateStackVar(assemblyName, Cm::IrIntf::Pointer(type->GetBaseType()->GetIrType(), type->GetPointerCount() + 1));
        }
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

FunctionEmitter::FunctionEmitter(Cm::Util::CodeFormatter& codeFormatter_, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_, 
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::StringRepository& stringRepository_) : 
    Cm::BoundTree::Visitor(true), codeFormatter(codeFormatter_), emitter(nullptr), genFlags(Cm::Core::GenFlags::none), typeRepository(typeRepository_), 
    irFunctionRepository(irFunctionRepository_), irClassTypeRepository(irClassTypeRepository_), stringRepository(stringRepository_), compoundResult(), currentCompileUnit(nullptr)
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

void FunctionEmitter::Visit(Cm::BoundTree::BoundStringLiteral& boundStringLiteral)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    result.SetMainObject(boundStringLiteral.GetType());
    Ir::Intf::Object* stringConstant = stringRepository.GetStringConstant(boundStringLiteral.Id());
    Ir::Intf::Object* stringObject = stringRepository.GetStringObject(boundStringLiteral.Id());
    Ir::Intf::Object* zero = Cm::IrIntf::CreateI32Constant(0);
    emitter->Own(zero);
    Ir::Intf::Type* s = Cm::IrIntf::Pointer(stringConstant->GetType(), 1);
    emitter->Own(s);
    emitter->Emit(Cm::IrIntf::GetElementPtr(s, result.MainObject(), stringObject, zero, zero));
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
    Ir::Intf::LabelObject* resultLabel = operandResult.GetLabel();
    result.Merge(operandResult);
    Cm::Sym::FunctionSymbol* conversionFun = boundConversion.ConversionFun();
    GenerateCall(conversionFun, result);
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundCast& boundCast)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    result.SetMainObject(boundCast.GetType());
    if (boundCast.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        result.SetGenJumpingBoolCode();
    }
    Cm::Core::GenResult operandResult = resultStack.Pop();
    Ir::Intf::LabelObject* resultLabel = operandResult.GetLabel();
    result.Merge(operandResult);
    Cm::Sym::FunctionSymbol* conversionFun = boundCast.ConversionFun();
    GenerateCall(conversionFun, result);
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundUnaryOp& boundUnaryOp)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Cm::Core::GenResult operandResult = resultStack.Pop();
    result.SetMainObject(boundUnaryOp.GetType());
    if (boundUnaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        result.SetGenJumpingBoolCode();
    }
    if (boundUnaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue))
    {
        result.SetLvalue();
    }
    Ir::Intf::LabelObject* resultLabel = operandResult.GetLabel();
    result.Merge(operandResult);
    Cm::Sym::FunctionSymbol* op = boundUnaryOp.GetFunction();
    GenerateCall(op, result);
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundBinaryOp& boundBinaryOp)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Cm::Core::GenResult right = resultStack.Pop();
    Cm::Core::GenResult left = resultStack.Pop();
    result.SetMainObject(boundBinaryOp.GetType());
    if (boundBinaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        result.SetGenJumpingBoolCode();
    }
    Ir::Intf::LabelObject* resultLabel = left.GetLabel();
    if (!resultLabel)
    {
        resultLabel = right.GetLabel();
    }
    result.Merge(left);
    result.Merge(right);
    Cm::Sym::FunctionSymbol* op = boundBinaryOp.GetFunction();
    GenerateCall(op, result);
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
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
    genFlags = Cm::Core::GenFlags::none;
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

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundCompoundStatement& boundCompoundStatement)
{
    compoundResult = Cm::Core::GenResult(emitter.get(), Cm::Core::GenFlags::none);
    if (boundCompoundStatement.IsEmpty())
    {
        compoundResult.SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)));
        GenerateCall(irFunctionRepository.GetDoNothingFunction(), compoundResult, false);
    }
}

void FunctionEmitter::EndVisit(Cm::BoundTree::BoundCompoundStatement& boundCompoundStatement)
{
    resultStack.Push(std::move(compoundResult));
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
        result.SetMainObject(boundReturnStatement.GetReturnType());
        Cm::Core::GenResult retValResult = resultStack.Pop();
        Ir::Intf::LabelObject* resultLabel = retValResult.GetLabel();
        result.Merge(retValResult);
        GenerateCall(ctor, result);
        emitter->Emit(Cm::IrIntf::Ret(result.MainObject()));
        if (resultLabel)
        {
            result.SetLabel(resultLabel);
        }
    }
    else
    {
        emitter->Emit(Cm::IrIntf::Ret());
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundConstructionStatement& boundConstructionStatement)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    int n = boundConstructionStatement.Arguments().Count();
    Ir::Intf::LabelObject* resultLabel = nullptr;
    for (int i = 0; i < n; ++i)
    {
        const std::unique_ptr<Cm::BoundTree::BoundExpression>& arg = boundConstructionStatement.Arguments()[i];
        arg->Accept(*this);
        Cm::Core::GenResult argResult = resultStack.Pop();
        if (!resultLabel)
        {
            resultLabel = argResult.GetLabel();
        }
        result.Merge(argResult);
    }
    Cm::Sym::FunctionSymbol* ctor = boundConstructionStatement.Constructor();
    GenerateCall(ctor, result);
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundAssignmentStatement& boundAssignmentStatement)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Cm::Core::GenResult sourceResult = resultStack.Pop();
    Cm::Core::GenResult targetResult = resultStack.Pop(); 
    Ir::Intf::LabelObject* resultLabel = targetResult.GetLabel();
    if (!resultLabel)
    {
        resultLabel = sourceResult.GetLabel();
    }
    result.Merge(targetResult);
    result.Merge(sourceResult);
    Cm::Sym::FunctionSymbol* assignment = boundAssignmentStatement.Assignment();
    GenerateCall(assignment, result);
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundSimpleStatement& boundSimpleStatement)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    if (boundSimpleStatement.HasExpression())
    {
        Cm::Core::GenResult expressionResult = resultStack.Pop();
        result.SetLabel(expressionResult.GetLabel());
        result.Merge(expressionResult);
    }
    else
    {
        result.SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)));
        GenerateCall(irFunctionRepository.GetDoNothingFunction(), result, false);
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
    BeginVisitStatement(*thenS);
    thenS->Accept(*this);
    Cm::Core::GenResult thenResult = resultStack.Pop();
    conditionResult.BackpatchTrueTargets(thenResult.GetLabel());
    result.Merge(thenResult);
    Cm::BoundTree::BoundStatement* elseS = boundConditionalStatement.ElseS();
    if (elseS)
    {
        BeginVisitStatement(*elseS);
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
    BeginVisitStatement(*statement);
    statement->Accept(*this);
    Cm::Core::GenResult statementResult = resultStack.Pop();
    emitter->Emit(Cm::IrIntf::Br(conditionResult.GetLabel()));
    conditionResult.BackpatchTrueTargets(statementResult.GetLabel());
    statementResult.BackpatchNextTargets(conditionResult.GetLabel());
    result.MergeTargets(result.NextTargets(), conditionResult.FalseTargets());
    result.SetLabel(conditionResult.GetLabel());
    result.Merge(conditionResult);
    result.Merge(statementResult);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundDoStatement& boundDoStatement)
{
    PushSkipContent();
}

void FunctionEmitter::EndVisit(Cm::BoundTree::BoundDoStatement& boundDoStatement)
{
    PopSkipContent();
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Cm::BoundTree::BoundStatement* statement = boundDoStatement.Statement();
    BeginVisitStatement(*statement);
    statement->Accept(*this);
    Cm::Core::GenResult statementResult = resultStack.Pop();
    Cm::BoundTree::BoundExpression* condition = boundDoStatement.Condition();
    condition->Accept(*this);
    Cm::Core::GenResult conditionResult = resultStack.Pop();
    statementResult.BackpatchNextTargets(conditionResult.GetLabel());
    conditionResult.BackpatchTrueTargets(statementResult.GetLabel());
    result.MergeTargets(result.NextTargets(), conditionResult.FalseTargets());
    result.SetLabel(statementResult.GetLabel());
    result.Merge(statementResult);
    result.Merge(conditionResult);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundForStatement& boundForStatement)
{
    PushSkipContent();
}

void FunctionEmitter::EndVisit(Cm::BoundTree::BoundForStatement& boundForStatement)
{
    PopSkipContent();
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Cm::BoundTree::BoundStatement* initS = boundForStatement.InitS();
    BeginVisitStatement(*initS);
    initS->Accept(*this);
    Cm::Core::GenResult initResult = resultStack.Pop();
    Ir::Intf::LabelObject* initLabel = initResult.GetLabel();
    Cm::BoundTree::BoundExpression* condition = boundForStatement.Condition();
    condition->Accept(*this);
    Cm::Core::GenResult conditionResult = resultStack.Pop();
    initResult.BackpatchNextTargets(conditionResult.GetLabel());
    Cm::BoundTree::BoundStatement* action = boundForStatement.Action();
    BeginVisitStatement(*action);
    action->Accept(*this);
    Cm::Core::GenResult actionResult = resultStack.Pop();
    conditionResult.BackpatchTrueTargets(actionResult.GetLabel());
    result.MergeTargets(result.NextTargets(), conditionResult.FalseTargets());
    Cm::BoundTree::BoundExpression* increment = boundForStatement.Increment();
    increment->Accept(*this);
    Cm::Core::GenResult incrementResult = resultStack.Pop();
    actionResult.BackpatchNextTargets(incrementResult.GetLabel());
    incrementResult.BackpatchNextTargets(conditionResult.GetLabel());
    emitter->Emit(Cm::IrIntf::Br(conditionResult.GetLabel()));
    result.Merge(initResult);
    result.Merge(conditionResult);
    result.Merge(actionResult);
    result.Merge(incrementResult);
    result.SetLabel(initLabel);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::GenerateCall(Ir::Intf::Function* fun, Cm::Core::GenResult& result, bool memberFunctionCall)
{
    if (memberFunctionCall)
    {
        Cm::Core::GenResult memberFunctionResult(emitter.get(), result.Flags());
        memberFunctionResult.SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)));
        for (Ir::Intf::Object* object : result.Objects())
        {
            memberFunctionResult.AddObject(object);
        }
        Ir::Intf::Instruction* callInst = Cm::IrIntf::Call(memberFunctionResult.MainObject(), fun, memberFunctionResult.Args());
        emitter->Emit(callInst);
    }
    else
    {
        Ir::Intf::Instruction* callInst = Cm::IrIntf::Call(result.MainObject(), fun, result.Args());
        emitter->Emit(callInst);
    }
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
        GenerateCall(irFunction, result, fun->IsMemberFunctionSymbol());
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