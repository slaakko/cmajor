/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Emit/FunctionEmitter.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.BoundTree/BoundExpression.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.BoundTree/BoundClass.hpp>
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.Core/GlobalFlags.hpp>
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
            localVariableObject = Cm::IrIntf::CreateStackVar(assemblyName, Cm::IrIntf::Pointer(type->GetBaseType()->GetIrType(), type->GetPointerCount() + 2));
        }
        else
        {
            localVariableObject = Cm::IrIntf::CreateRefVar(assemblyName, Cm::IrIntf::Pointer(type->GetBaseType()->GetIrType(), type->GetPointerCount() + 2));
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

Ir::Intf::Object* IrObjectRepository::MakeMemberVariableIrObject(Cm::BoundTree::BoundMemberVariable* boundMemberVariable, Ir::Intf::Object* ptr)
{
    Ir::Intf::MemberVar* memberVar = Cm::IrIntf::CreateMemberVar(boundMemberVariable->Symbol()->Name(), ptr, boundMemberVariable->Symbol()->LayoutIndex(), boundMemberVariable->Symbol()->GetType()->GetIrType());
    ownedIrObjects.push_back(std::unique_ptr<Ir::Intf::Object>(memberVar));
    return memberVar;
}

FunctionEmitter::FunctionEmitter(Cm::Util::CodeFormatter& codeFormatter_, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_, 
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::StringRepository& stringRepository_, Cm::BoundTree::BoundClass* currentClass_, 
    std::unordered_set<Ir::Intf::Function*>& externalFunctions_) :
    Cm::BoundTree::Visitor(true), codeFormatter(codeFormatter_), emitter(nullptr), genFlags(Cm::Core::GenFlags::none), typeRepository(typeRepository_), 
    irFunctionRepository(irFunctionRepository_), irClassTypeRepository(irClassTypeRepository_), stringRepository(stringRepository_), compoundResult(), currentCompileUnit(nullptr), 
    currentClass(currentClass_), thisParam(nullptr), externalFunctions(externalFunctions_), executingPostfixIncDecStatements(false), continueTargetStatement(nullptr), breakTargetStatement(nullptr),
    currentSwitchEmitState(SwitchEmitState::none), currentSwitchCaseConstantMap(nullptr), switchCaseLabel(nullptr)
{
}

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
    Cm::Sym::FunctionSymbol* currentFunction = boundFunction.GetFunctionSymbol();
    currentCompileUnit = currentFunction->CompileUnit();
    Ir::Intf::Function* irFunction = irFunctionRepository.CreateIrFunction(currentFunction);
    emitter.reset(new Cm::Core::Emitter(irFunction));
    irFunction->SetComment(boundFunction.GetFunctionSymbol()->FullName());
    int parameterIndex = 0;
    for (Cm::Sym::ParameterSymbol* parameter : boundFunction.GetFunctionSymbol()->Parameters())
    {
        Ir::Intf::Object* localVariableIrObject = localVariableIrObjectRepository.CreateLocalVariableIrObjectFor(parameter);
        emitter->Emit(Cm::IrIntf::Alloca(parameter->GetType()->GetIrType(), localVariableIrObject));
        if (currentFunction->IsMemberFunctionSymbol() && parameterIndex == 0)
        {
            thisParam = parameter;
        }
        ++parameterIndex;
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
    Cm::Sym::FunctionSymbol* functionSymbol = boundFunction.GetFunctionSymbol();
    bool weakOdr = functionSymbol->IsReplicated();
    bool inline_ = Cm::Core::GetGlobalFlag(Cm::Core::GlobalFlags::optimize) && functionSymbol->IsInline();
    irFunction->WriteDefinition(codeFormatter, weakOdr, inline_);
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

void FunctionEmitter::Visit(Cm::BoundTree::BoundEnumConstant& boundEnumConstant) 
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Ir::Intf::Object* enumConstantValue = boundEnumConstant.Symbol()->GetValue()->CreateIrObject();
    emitter->Own(enumConstantValue);
    result.SetMainObject(enumConstantValue);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundLocalVariable& boundLocalVariable)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Cm::Sym::TypeSymbol* type = boundLocalVariable.Symbol()->GetType();
    bool typeIsReferenceType = type->IsReferenceType() || type->IsRvalueRefType();
    bool byRefOrClassType = boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef) || boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue) || type->IsClassTypeSymbol();
    if ((boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef) || boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue)) && typeIsReferenceType)
    {
        byRefOrClassType = false; // already reference type
    }
    if (byRefOrClassType || boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::constructVariable))
    {
        result.SetMainObject(localVariableIrObjectRepository.GetLocalVariableIrObject(boundLocalVariable.Symbol()));
    }
    else
    {
        Cm::Sym::TypeSymbol* type = boundLocalVariable.Symbol()->GetType();
        result.SetMainObject(type);
        result.AddObject(localVariableIrObjectRepository.GetLocalVariableIrObject(boundLocalVariable.Symbol()));
        Cm::IrIntf::Init(*emitter, type->GetIrType(), result.Arg1(), result.MainObject());
        if (boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
        {
            GenJumpingBoolCode(result);
        }
    }
    if (boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundLocalVariable.GetType()), result);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundParameter& boundParameter)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Cm::Sym::TypeSymbol* type = boundParameter.Symbol()->GetType();
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
        if (boundParameter.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
        {
            GenJumpingBoolCode(result);
        }
    }
    if (boundParameter.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundParameter.GetType()), result);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundMemberVariable& boundMemberVariable)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Cm::BoundTree::BoundExpression* classObject = boundMemberVariable.GetClassObject();
    if (classObject)
    {
        classObject->Accept(*this);
    }
    else
    {
        if (currentClass)
        {
            Cm::Sym::ClassTypeSymbol* classType = currentClass->Symbol();
            std::unique_ptr<Cm::BoundTree::BoundParameter> boundParameter(new Cm::BoundTree::BoundParameter(nullptr, thisParam));
            boundParameter->Accept(*this);
        }
        else
        {
            throw Cm::Core::Exception("cannot use member variables in non-class context", boundMemberVariable.Symbol()->GetSpan());
        }
    }
    Cm::Core::GenResult ptrResult = resultStack.Pop();
    Cm::Sym::TypeSymbol* type = boundMemberVariable.Symbol()->GetType();
    Ir::Intf::Object* memberVariableIrObject = irObjectRepository.MakeMemberVariableIrObject(&boundMemberVariable, ptrResult.MainObject());
    if (boundMemberVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue) || boundMemberVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef))
    {
        result.SetMainObject(memberVariableIrObject->CreateAddr(*emitter, type->GetIrType()));
    }
    else
    {
        result.SetMainObject(type);
        result.AddObject(memberVariableIrObject);
        Cm::IrIntf::Init(*emitter, type->GetIrType(), result.Arg1(), result.MainObject());
    }
    if (boundMemberVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundMemberVariable.GetType()), result);
    }
    result.Merge(ptrResult);
    if (boundMemberVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        GenJumpingBoolCode(result);
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
    if (boundConversion.Operand()->GetType()->IsClassTypeSymbol() && (boundConversion.GetType()->IsPointerType() || boundConversion.GetType()->IsReferenceType() || boundConversion.GetType()->IsRvalueRefType()))
    {
        result.SetClassTypeToPointerTypeConversion();
    }
    GenerateCall(conversionFun, result);
    if (boundConversion.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundConversion.GetType()), result);
    }
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
    if (boundCast.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundCast.GetType()), result);
    }
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundUnaryOp& boundUnaryOp)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Cm::Sym::FunctionSymbol* op = boundUnaryOp.GetFunction();
    result.SetMainObject(op->GetReturnType());
    Cm::Core::GenResult operandResult = resultStack.Pop();
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
    GenerateCall(op, result);
    if (boundUnaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundUnaryOp.GetType()), result);
    }
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
    if (boundBinaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundBinaryOp.GetType()), result);
    }
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundFunctionCall& functionCall)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    if (!functionCall.GetFunction()->IsBasicTypeOp() && !functionCall.GetFunction()->IsConstructorOrDestructorSymbol())
    {
        result.SetMainObject(functionCall.GetType());
    }
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
    if (functionCall.GetFlag(Cm::BoundTree::BoundNodeFlags::genVirtualCall))
    {
        result.SetGenVirtualCall();
    }
    GenerateCall(fun, result);
    if (functionCall.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(functionCall.GetType()), result);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundDisjunction& boundDisjunction)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    if (boundDisjunction.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        boundDisjunction.Left()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundDisjunction.Left()->Accept(*this);
        Cm::Core::GenResult leftResult = resultStack.Pop();
        boundDisjunction.Right()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundDisjunction.Right()->Accept(*this);
        Cm::Core::GenResult rightResult = resultStack.Pop();
        leftResult.BackpatchFalseTargets(rightResult.GetLabel());
        result.MergeTargets(result.FalseTargets(), rightResult.FalseTargets());
        result.MergeTargets(result.TrueTargets(), leftResult.TrueTargets());
        result.MergeTargets(result.TrueTargets(), rightResult.TrueTargets());
        result.Merge(leftResult);
        result.Merge(rightResult);
    }
    else
    {
        boundDisjunction.Left()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundDisjunction.Left()->Accept(*this);
        Cm::Core::GenResult leftResult = resultStack.Pop();
        boundDisjunction.Right()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundDisjunction.Right()->Accept(*this);
        Cm::Core::GenResult rightResult = resultStack.Pop();
        leftResult.BackpatchFalseTargets(rightResult.GetLabel());
        result.SetMainObject(localVariableIrObjectRepository.GetLocalVariableIrObject(boundDisjunction.GetResultVar()));
        Ir::Intf::LabelObject* falseLabel = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(falseLabel);
        emitter->AddNextInstructionLabel(falseLabel);
        Ir::Intf::Object* false_ = Cm::IrIntf::False();
        emitter->Own(false_);
        Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI1(), false_, result.MainObject());
        rightResult.BackpatchFalseTargets(falseLabel);
        Ir::Intf::LabelObject* next = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(next);
        emitter->Emit(Cm::IrIntf::Br(next));
        result.AddArgNextTarget(next);
        Ir::Intf::LabelObject* trueLabel = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(trueLabel);
        emitter->AddNextInstructionLabel(trueLabel);
        Ir::Intf::Object* true_ = Cm::IrIntf::True();
        emitter->Own(true_);
        Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI1(), true_, result.MainObject());
        leftResult.BackpatchTrueTargets(trueLabel);
        rightResult.BackpatchTrueTargets(trueLabel);
        result.Merge(leftResult);
        result.Merge(rightResult);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundConjunction& boundConjunction)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    if (boundConjunction.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        boundConjunction.Left()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundConjunction.Left()->Accept(*this);
        Cm::Core::GenResult leftResult = resultStack.Pop();
        boundConjunction.Right()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundConjunction.Right()->Accept(*this);
        Cm::Core::GenResult rightResult = resultStack.Pop();
        leftResult.BackpatchTrueTargets(rightResult.GetLabel());
        result.MergeTargets(result.TrueTargets(), rightResult.TrueTargets());
        result.MergeTargets(result.FalseTargets(), leftResult.FalseTargets());
        result.MergeTargets(result.FalseTargets(), rightResult.FalseTargets());
    }
    else
    {
        boundConjunction.Left()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundConjunction.Left()->Accept(*this);
        Cm::Core::GenResult leftResult = resultStack.Pop();
        boundConjunction.Right()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundConjunction.Right()->Accept(*this);
        Cm::Core::GenResult rightResult = resultStack.Pop();
        leftResult.BackpatchTrueTargets(rightResult.GetLabel());
        result.SetMainObject(localVariableIrObjectRepository.GetLocalVariableIrObject(boundConjunction.GetResultVar()));
        Ir::Intf::LabelObject* trueLabel = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(trueLabel);
        Ir::Intf::Object* true_ = Cm::IrIntf::True();
        emitter->Own(true_);
        emitter->AddNextInstructionLabel(trueLabel);
        Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI1(), true_, result.MainObject());
        rightResult.BackpatchTrueTargets(trueLabel);
        Ir::Intf::LabelObject* next = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(next);
        emitter->Emit(Cm::IrIntf::Br(next));
        result.AddArgNextTarget(next);
        Ir::Intf::LabelObject* falseLabel = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(falseLabel);
        emitter->AddNextInstructionLabel(falseLabel);
        Ir::Intf::Object* false_ = Cm::IrIntf::False();
        emitter->Own(false_);
        Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI1(), false_, result.MainObject());
        leftResult.BackpatchFalseTargets(falseLabel);
        rightResult.BackpatchFalseTargets(falseLabel);
        result.Merge(leftResult);
        result.Merge(rightResult);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundPostfixIncDecExpr& boundPostfixIncDecExpr)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    boundPostfixIncDecExpr.Value()->Accept(*this);
    Cm::Core::GenResult valueResult = resultStack.Pop();
    Ir::Intf::LabelObject* resultLabel = valueResult.GetLabel();;
    result.Merge(valueResult);
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    resultStack.Push(std::move(result));
    postfixIncDecStatements.push_back(std::unique_ptr<Cm::BoundTree::BoundStatement>(boundPostfixIncDecExpr.ReleaseStatement()));
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

void FunctionEmitter::Visit(Cm::BoundTree::BoundInitClassObjectStatement& boundInitClassObjectStatement)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    boundInitClassObjectStatement.FunctionCall()->Accept(*this);
    Cm::Core::GenResult callResult = resultStack.Pop();
    result.Merge(callResult);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundInitVPtrStatement& boundInitVPtrStatement)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 1);
    emitter->Own(i8Ptr);
    Ir::Intf::Type* i8PtrPtr = Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 2);
    emitter->Own(i8PtrPtr);
    Cm::Sym::ClassTypeSymbol* classType = boundInitVPtrStatement.ClassType();
    Cm::BoundTree::BoundParameter boundThisParam(nullptr, thisParam);
    boundThisParam.Accept(*this);
    Cm::Core::GenResult thisResult = resultStack.Pop();
    int16_t vptrIndex = classType->VPtrIndex();
    Ir::Intf::Object* vptrContainerPtr = thisResult.MainObject();
    if (vptrIndex == -1)
    {
        Cm::Sym::ClassTypeSymbol* vptrContainingType = classType->VPtrContainerClass();
        vptrIndex = vptrContainingType->VPtrIndex();
        Ir::Intf::Type* vptrContainingTypeIrType = vptrContainingType->GetIrType();
        Ir::Intf::Type* vptrContainingTypePtrType = Cm::IrIntf::Pointer(vptrContainingTypeIrType, 1);
        emitter->Own(vptrContainingTypePtrType);
        Ir::Intf::RegVar* containerPtr = Cm::IrIntf::CreateTemporaryRegVar(vptrContainingTypePtrType);
        emitter->Own(containerPtr);
        Ir::Intf::Type* classTypeIrType = classType->GetIrType();
        Ir::Intf::Type* classTypePtrType = Cm::IrIntf::Pointer(classTypeIrType, 1);
        emitter->Own(classTypePtrType);
        emitter->Emit(Cm::IrIntf::Bitcast(classTypePtrType, containerPtr, thisResult.MainObject(), vptrContainingTypePtrType));
        vptrContainerPtr = containerPtr;
    }
    Ir::Intf::MemberVar* vptr = Cm::IrIntf::CreateMemberVar(Cm::IrIntf::GetVPtrVarName(), vptrContainerPtr, vptrIndex, i8PtrPtr);
    emitter->Own(vptr);
    Ir::Intf::RegVar* vtblAddrAsI8PtrPtr = Cm::IrIntf::CreateTemporaryRegVar(i8PtrPtr);
    emitter->Own(vtblAddrAsI8PtrPtr);
    Ir::Intf::Type* vtblAddrType = Cm::IrIntf::Pointer(Cm::IrIntf::Array(i8Ptr->Clone(), int(classType->Vtbl().size())), 1);
    emitter->Own(vtblAddrType);
    std::string vtblName = Cm::IrIntf::MakeAssemblyName(classType->FullName() + Cm::IrIntf::GetPrivateSeparator() + "vtbl");
    Ir::Intf::Object* vtblObject = Cm::IrIntf::CreateGlobal(vtblName, vtblAddrType);
    emitter->Own(vtblObject);
    emitter->Emit(Cm::IrIntf::Bitcast(vtblAddrType, vtblAddrAsI8PtrPtr, vtblObject, i8PtrPtr));
    Cm::IrIntf::Assign(*emitter, i8PtrPtr, vtblAddrAsI8PtrPtr, vptr);
    result.Merge(thisResult);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundInitMemberVariableStatement& boundInitMemberVariableStatement)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    int n = boundInitMemberVariableStatement.Arguments().Count();
    Ir::Intf::LabelObject* resultLabel = nullptr;
    for (int i = 0; i < n; ++i)
    {
        const std::unique_ptr<Cm::BoundTree::BoundExpression>& arg = boundInitMemberVariableStatement.Arguments()[i];
        arg->Accept(*this);
        Cm::Core::GenResult argResult = resultStack.Pop();
        if (!resultLabel)
        {
            resultLabel = argResult.GetLabel();
        }
        result.Merge(argResult);
    }
    Cm::Sym::FunctionSymbol* ctor = boundInitMemberVariableStatement.Constructor();
    GenerateCall(ctor, result);
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundFunctionCallStatement& boundFunctionCallStatement) 
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Cm::Sym::FunctionSymbol* function = boundFunctionCallStatement.Function();
    if (!function->IsBasicTypeOp() && !function->IsConstructorOrDestructorSymbol())
    {
        result.SetMainObject(function->GetReturnType());
    }
    int n = boundFunctionCallStatement.Arguments().Count();
    Ir::Intf::LabelObject* resultLabel = nullptr;
    for (int i = 0; i < n; ++i)
    {
        const std::unique_ptr<Cm::BoundTree::BoundExpression>& arg = boundFunctionCallStatement.Arguments()[i];
        arg->Accept(*this);
        Cm::Core::GenResult argResult = resultStack.Pop();
        if (!resultLabel)
        {
            resultLabel = argResult.GetLabel();
        }
        result.Merge(argResult);
    }
    GenerateCall(function, result);
    ExecutePostfixIncDecStatements(result);
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
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
        Ir::Intf::LabelObject* exprNext = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(exprNext);
        emitter->AddNextInstructionLabel(exprNext);
        GenerateCall(ctor, result);
        result.BackpatchArgNextTargets(exprNext);
        ExecutePostfixIncDecStatements(result);
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
    Ir::Intf::LabelObject* exprNext = Cm::IrIntf::CreateNextLocalLabel();
    emitter->Own(exprNext);
    emitter->AddNextInstructionLabel(exprNext);
    GenerateCall(ctor, result);
    result.BackpatchArgNextTargets(exprNext);
    ExecutePostfixIncDecStatements(result);
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundAssignmentStatement& boundAssignmentStatement)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Cm::Sym::FunctionSymbol* assignment = boundAssignmentStatement.Assignment();
    if (!assignment->IsBasicTypeOp())
    {
        result.SetMainObject(assignment->GetReturnType());
    }
    Cm::Core::GenResult sourceResult = resultStack.Pop();
    Cm::Core::GenResult targetResult = resultStack.Pop(); 
    Ir::Intf::LabelObject* resultLabel = targetResult.GetLabel();
    if (!resultLabel)
    {
        resultLabel = sourceResult.GetLabel();
    }
    result.Merge(targetResult);
    result.Merge(sourceResult);
    Ir::Intf::LabelObject* exprNext = Cm::IrIntf::CreateNextLocalLabel();
    emitter->Own(exprNext);
    emitter->AddNextInstructionLabel(exprNext);
    GenerateCall(assignment, result);
    result.BackpatchArgNextTargets(exprNext);
    ExecutePostfixIncDecStatements(result);
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
        ExecutePostfixIncDecStatements(result);
    }
    else
    {
        result.SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)));
        GenerateCall(irFunctionRepository.GetDoNothingFunction(), result, false);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundBreakStatement& boundBreakStatement)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Ir::Intf::LabelObject* breakTargetLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter->Own(breakTargetLabel);
    breakTargetStatement->AddBreakTargetLabel(breakTargetLabel);
    emitter->Emit(Cm::IrIntf::Br(breakTargetLabel));
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundContinueStatement& boundContinueStatement) 
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Ir::Intf::LabelObject* continueTargetLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter->Own(continueTargetLabel);
    continueTargetStatement->AddContinueTargetLabel(continueTargetLabel);
    emitter->Emit(Cm::IrIntf::Br(continueTargetLabel));
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
    Ir::Intf::LabelObject* resultLabel = conditionResult.GetLabel();
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
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
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
    PushBreakTargetStatement(&boundWhileStatement);
    PushContinueTargetStatement(&boundWhileStatement);
    BeginVisitStatement(*statement);
    statement->Accept(*this);
    Cm::Core::GenResult statementResult = resultStack.Pop();
    emitter->Emit(Cm::IrIntf::Br(conditionResult.GetLabel()));
    conditionResult.BackpatchTrueTargets(statementResult.GetLabel());
    statementResult.BackpatchNextTargets(conditionResult.GetLabel());
    result.MergeTargets(result.NextTargets(), conditionResult.FalseTargets());
    result.MergeTargets(result.NextTargets(), boundWhileStatement.BreakTargetLabels());
    Ir::Intf::Backpatch(boundWhileStatement.ContinueTargetLabels(), conditionResult.GetLabel());
    PopContinueTargetStatement();
    PopBreakTargetStatement();
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
    PushBreakTargetStatement(&boundDoStatement);
    PushContinueTargetStatement(&boundDoStatement);
    BeginVisitStatement(*statement);
    statement->Accept(*this);
    Cm::Core::GenResult statementResult = resultStack.Pop();
    Cm::BoundTree::BoundExpression* condition = boundDoStatement.Condition();
    condition->Accept(*this);
    Cm::Core::GenResult conditionResult = resultStack.Pop();
    statementResult.BackpatchNextTargets(conditionResult.GetLabel());
    conditionResult.BackpatchTrueTargets(statementResult.GetLabel());
    result.MergeTargets(result.NextTargets(), conditionResult.FalseTargets());
    result.MergeTargets(result.NextTargets(), boundDoStatement.BreakTargetLabels());
    Ir::Intf::Backpatch(boundDoStatement.ContinueTargetLabels(), conditionResult.GetLabel());
    PopContinueTargetStatement();
    PopBreakTargetStatement();
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
    PushBreakTargetStatement(&boundForStatement);
    PushContinueTargetStatement(&boundForStatement);
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
    result.MergeTargets(result.NextTargets(), boundForStatement.BreakTargetLabels());
    Ir::Intf::Backpatch(boundForStatement.ContinueTargetLabels(), conditionResult.GetLabel());
    PopContinueTargetStatement();
    PopBreakTargetStatement();
    result.Merge(initResult);
    result.Merge(conditionResult);
    result.Merge(actionResult);
    result.Merge(incrementResult);
    result.SetLabel(initLabel);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundSwitchStatement& boundSwitchStatement)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    PushBreakTargetStatement(&boundSwitchStatement);
    boundSwitchStatement.Condition()->Accept(*this);
    Cm::Core::GenResult conditionResult = resultStack.Pop();
    Ir::Intf::LabelObject* resultLabel = conditionResult.GetLabel();
    result.Merge(conditionResult);
    std::vector<Ir::Intf::LabelObject*> caseLabels;
    std::unordered_map<std::string, Ir::Intf::LabelObject*> caseConstantMap;
    std::vector<std::pair<Ir::Intf::Object*, Ir::Intf::LabelObject*>> destinations;
    switchEmitStateStack.push(currentSwitchEmitState);
    currentSwitchEmitState = SwitchEmitState::createSwitchTargets;
    switchCaseConstantMapStack.push(currentSwitchCaseConstantMap);
    currentSwitchCaseConstantMap = &caseConstantMap;
    for (std::unique_ptr<Cm::BoundTree::BoundStatement>& caseStatement : boundSwitchStatement.CaseStatements())
    {
        switchCaseConstants.clear();
        caseStatement->Accept(*this);
        Ir::Intf::LabelObject* caseLabel = switchCaseLabel;
        for (Ir::Intf::Object* caseConstant : switchCaseConstants)
        {
            destinations.push_back(std::make_pair(caseConstant, caseLabel));
        }
        caseLabels.push_back(caseLabel);
    }
    Ir::Intf::LabelObject* defaultDest = nullptr;
    if (boundSwitchStatement.DefaultStatement())
    {
        boundSwitchStatement.DefaultStatement()->Accept(*this);
        defaultDest = switchCaseLabel;
    }
    else
    {
        defaultDest = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(defaultDest);
        result.AddNextTarget(defaultDest);
    }
    currentSwitchCaseConstantMap = switchCaseConstantMapStack.top();
    switchCaseConstantMapStack.pop();
    currentSwitchEmitState = switchEmitStateStack.top();
    switchEmitStateStack.pop();
    Ir::Intf::Instruction* switchInst = Cm::IrIntf::Switch(result.MainObject()->GetType(), result.MainObject(), defaultDest, destinations);
    emitter->Emit(switchInst);
    int index = 0;
    for (std::unique_ptr<Cm::BoundTree::BoundStatement>& caseStatement : boundSwitchStatement.CaseStatements())
    {
        switchEmitStateStack.push(currentSwitchEmitState);
        currentSwitchEmitState = SwitchEmitState::emitStatements;
        switchCaseConstantMapStack.push(currentSwitchCaseConstantMap);
        currentSwitchCaseConstantMap = &caseConstantMap;
        switchCaseLabel = caseLabels[index];
        caseStatement->Accept(*this);
        currentSwitchCaseConstantMap = switchCaseConstantMapStack.top();
        switchCaseConstantMapStack.pop();
        currentSwitchEmitState = switchEmitStateStack.top();
        switchEmitStateStack.pop();
        Cm::Core::GenResult caseResult = resultStack.Pop();
        result.Merge(caseResult);
        ++index;
    }
    if (boundSwitchStatement.DefaultStatement())
    {
        switchEmitStateStack.push(currentSwitchEmitState);
        currentSwitchEmitState = SwitchEmitState::emitStatements;
        switchCaseConstantMapStack.push(currentSwitchCaseConstantMap);
        currentSwitchCaseConstantMap = &caseConstantMap;
        switchCaseLabel = defaultDest;
        boundSwitchStatement.DefaultStatement()->Accept(*this);
        currentSwitchCaseConstantMap = switchCaseConstantMapStack.top();
        switchCaseConstantMapStack.pop();
        currentSwitchEmitState = switchEmitStateStack.top();
        switchEmitStateStack.pop();
        Cm::Core::GenResult defaultResult = resultStack.Pop();
        result.Merge(defaultResult);
    }
    result.MergeTargets(result.NextTargets(), boundSwitchStatement.BreakTargetLabels());
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    PopBreakTargetStatement();
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundCaseStatement& boundCaseStatement)
{
    if (currentSwitchEmitState == SwitchEmitState::createSwitchTargets)
    {
        Ir::Intf::LabelObject* label = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(label);
        for (const std::unique_ptr<Cm::Sym::Value>& value : boundCaseStatement.Values())
        {
            Ir::Intf::Object* caseConstant = value->CreateIrObject();
            emitter->Own(caseConstant);
            if (currentSwitchCaseConstantMap->find(caseConstant->Name()) != currentSwitchCaseConstantMap->end())
            {
                throw Cm::Core::Exception("duplicate case constant '" + caseConstant->Name() + "'", boundCaseStatement.SyntaxNode()->GetSpan());
            }
            currentSwitchCaseConstantMap->insert(std::make_pair(caseConstant->Name(), label));
            switchCaseConstants.push_back(caseConstant);
        }
        switchCaseLabel = label;
    }
    else if (currentSwitchEmitState == SwitchEmitState::emitStatements)
    {
        Cm::Core::GenResult result(emitter.get(), genFlags);
        emitter->AddNextInstructionLabel(switchCaseLabel);
        for (std::unique_ptr<Cm::BoundTree::BoundStatement>& statement : boundCaseStatement.Statements())
        {
            statement->Accept(*this);
            Cm::Core::GenResult statementResult = resultStack.Pop();
            result.Merge(statementResult);
        }
        resultStack.Push(std::move(result));
    }
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundDefaultStatement& boundDefaultStatement)
{
    if (currentSwitchEmitState == SwitchEmitState::createSwitchTargets)
    {
        Ir::Intf::LabelObject* label = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(label);
        switchCaseLabel = label;
        currentSwitchCaseConstantMap->insert(std::make_pair("@default", label));
    }
    else if (currentSwitchEmitState == SwitchEmitState::emitStatements)
    {
        Cm::Core::GenResult result(emitter.get(), genFlags);
        emitter->AddNextInstructionLabel(switchCaseLabel);
        for (std::unique_ptr<Cm::BoundTree::BoundStatement>& statement : boundDefaultStatement.Statements())
        {
            statement->Accept(*this);
            Cm::Core::GenResult statementResult = resultStack.Pop();
            result.Merge(statementResult);
        }
        resultStack.Push(std::move(result));
    }
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundGotoCaseStatement& boundGotoCaseStatement)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    Cm::Sym::Value* value = boundGotoCaseStatement.Value();
    Ir::Intf::Object* caseConstant = value->CreateIrObject();
    emitter->Own(caseConstant);
    SwitchCaseConstantMapIt i = currentSwitchCaseConstantMap->find(caseConstant->Name());
    if (i != currentSwitchCaseConstantMap->end())
    {
        Ir::Intf::LabelObject* target = i->second;
        emitter->Emit(Cm::IrIntf::Br(target));
    }
    else
    {
        throw Cm::Core::Exception("goto case statement target not found", boundGotoCaseStatement.SyntaxNode()->GetSpan());
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundGotoDefaultStatement& boundGotoDefaultStatement)
{
    Cm::Core::GenResult result(emitter.get(), genFlags);
    SwitchCaseConstantMapIt i = currentSwitchCaseConstantMap->find("@default");
    if (i != currentSwitchCaseConstantMap->end())
    {
        Ir::Intf::LabelObject* target = i->second;
        emitter->Emit(Cm::IrIntf::Br(target));
    }
    else
    {
        throw Cm::Core::Exception("goto default statement target not found", boundGotoDefaultStatement.SyntaxNode()->GetSpan());
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::PushBreakTargetStatement(Cm::BoundTree::BoundStatement* statement)
{
    breakTargetStatementStack.push(breakTargetStatement);
    breakTargetStatement = statement;
}

void FunctionEmitter::PopBreakTargetStatement()
{
    breakTargetStatement = breakTargetStatementStack.top();
    breakTargetStatementStack.pop();
}

void FunctionEmitter::PushContinueTargetStatement(Cm::BoundTree::BoundStatement* statement)
{
    continueTargetStatementStack.push(continueTargetStatement);
    continueTargetStatement = statement;
}

void FunctionEmitter::PopContinueTargetStatement()
{
    continueTargetStatement = continueTargetStatementStack.top();
    continueTargetStatementStack.pop();
}

void FunctionEmitter::MakePlainValueResult(Cm::Sym::TypeSymbol* plainType, Cm::Core::GenResult& result)
{
    Ir::Intf::Type* plainIrType = plainType->GetIrType();
    Ir::Intf::Object* plainObject = Cm::IrIntf::CreateTemporaryRegVar(plainIrType);
    emitter->Own(plainObject);
    Cm::IrIntf::Assign(*emitter, plainIrType, result.MainObject(), plainObject);
    result.SetMainObject(plainObject);
}

void FunctionEmitter::ExecutePostfixIncDecStatements(Cm::Core::GenResult& result)
{
    if (executingPostfixIncDecStatements) return;
    if (!postfixIncDecStatements.empty())
    {
        executingPostfixIncDecStatements = true;
        for (const std::unique_ptr<Cm::BoundTree::BoundStatement>& statement : postfixIncDecStatements)
        {
            statement->Accept(*this);
            Cm::Core::GenResult statementResult = resultStack.Pop();
            result.Merge(statementResult);
        }
        postfixIncDecStatements.clear();
        executingPostfixIncDecStatements = false;
    }
}

void FunctionEmitter::GenerateCall(Ir::Intf::Function* fun, Cm::Core::GenResult& result, bool constructorOrDestructorCall)
{
    if (constructorOrDestructorCall)
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

void FunctionEmitter::GenerateVirtualCall(Cm::Sym::FunctionSymbol* fun, Cm::Core::GenResult& result)
{
    Cm::Core::GenResult memberFunctionResult(emitter.get(), result.Flags());
    if (fun->IsConstructorOrDestructorSymbol())
    {
        memberFunctionResult.SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)));
    }
    for (Ir::Intf::Object* object : result.Objects())
    {
        memberFunctionResult.AddObject(object);
    }
    Ir::Intf::Object* objectPtr = memberFunctionResult.Arg1();
    Cm::Sym::ClassTypeSymbol* classType = fun->Class();
    Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 1);
    emitter->Own(i8Ptr);
    Ir::Intf::Type* i8PtrPtr = Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 2);
    emitter->Own(i8PtrPtr);
    Ir::Intf::Object* vptrContainerPtr = objectPtr;
    int16_t vptrIndex = classType->VPtrIndex();
    if (vptrIndex == -1)
    {
        Cm::Sym::ClassTypeSymbol* vptrContainingType = classType->VPtrContainerClass();
        vptrIndex = vptrContainingType->VPtrIndex();
        Ir::Intf::Type* vptrContainingPtrIrType = Cm::IrIntf::Pointer(vptrContainingType->GetIrType(), 1);
        emitter->Own(vptrContainingPtrIrType);
        Ir::Intf::RegVar* containerPtr = Cm::IrIntf::CreateTemporaryRegVar(vptrContainingPtrIrType);
        emitter->Own(containerPtr);
        Ir::Intf::Type* classTypePtrIrType = Cm::IrIntf::Pointer(classType->GetIrType(), 1);
        emitter->Own(classTypePtrIrType);
        emitter->Emit(Cm::IrIntf::Bitcast(classTypePtrIrType, containerPtr, objectPtr, vptrContainingPtrIrType));
        vptrContainerPtr = containerPtr;
    }
    Ir::Intf::MemberVar* vptr = Cm::IrIntf::CreateMemberVar(Cm::IrIntf::GetVPtrVarName(), vptrContainerPtr, vptrIndex, i8PtrPtr);
    emitter->Own(vptr);
    Ir::Intf::RegVar* loadedVptr = Cm::IrIntf::CreateTemporaryRegVar(i8PtrPtr);
    emitter->Own(loadedVptr);
    Cm::IrIntf::Assign(*emitter, i8PtrPtr, vptr, loadedVptr);
    Ir::Intf::RegVar* functionI8PtrPtr = Cm::IrIntf::CreateTemporaryRegVar(i8PtrPtr);
    emitter->Own(functionI8PtrPtr);
    Ir::Intf::Object* functionIndex = Cm::IrIntf::CreateI16Constant(fun->VtblIndex());
    emitter->Own(functionIndex);
    emitter->Emit(Cm::IrIntf::GetElementPtr(i8PtrPtr, functionI8PtrPtr, loadedVptr, functionIndex));
    Ir::Intf::RegVar* loadedFunctionI8Ptr = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
    emitter->Own(loadedFunctionI8Ptr);
    Cm::IrIntf::Assign(*emitter, i8Ptr, functionI8PtrPtr, loadedFunctionI8Ptr);
    Ir::Intf::Type* functionPtrType = irFunctionRepository.GetFunPtrIrType(fun);
    Ir::Intf::RegVar* loadedFunctionPtr = Cm::IrIntf::CreateTemporaryRegVar(functionPtrType);
    emitter->Own(loadedFunctionPtr);
    emitter->Emit(Cm::IrIntf::Bitcast(i8Ptr, loadedFunctionPtr, loadedFunctionI8Ptr, functionPtrType));
    emitter->Emit(Cm::IrIntf::IndirectCall(memberFunctionResult.MainObject(), loadedFunctionPtr, memberFunctionResult.Args()));
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
        if (result.GenerateVirtualCall())
        {
            GenerateVirtualCall(fun, result);
        }
        else
        {
            Ir::Intf::Function* irFunction = irFunctionRepository.CreateIrFunction(fun);
            if (fun->CompileUnit() != currentCompileUnit && !fun->IsReplicated())
            {
                externalFunctions.insert(irFunction);
            }
            GenerateCall(irFunction, result, fun->IsConstructorOrDestructorSymbol());
        }
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
