/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Emit/FunctionEmitter.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/DelegateTypeOpRepository.hpp>
#include <Cm.Bind/MemberVariable.hpp>
#include <Cm.BoundTree/BoundExpression.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.BoundTree/BoundClass.hpp>
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.Core/CompileUnitMap.hpp>
#include <Cm.Core/GlobalSettings.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.Sym/TypeRepository.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/FunctionTable.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <Llvm.Ir/Type.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <Llvm.Ir/Label.hpp>

namespace Cm { namespace Emit {

LocalVariableIrObjectRepository::LocalVariableIrObjectRepository(Cm::Core::IrFunctionRepository* irFunctionRepository_) : irFunctionRepository(irFunctionRepository_), exceptionCodeVariable(nullptr)
{
}

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
    Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
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
    if (!type->IrTypeMade())
    {
        type->MakeIrType();
    }
    std::string assemblyName = MakeUniqueAssemblyName(localVariableOrParameter->Name());
    Ir::Intf::Object* localVariableObject = nullptr;
    Ir::Intf::Type* baseTypeIrType = type->GetBaseType()->GetIrType();
    if (type->GetBaseType()->IsVoidTypeSymbol())
    {
        if (backend == Cm::IrIntf::BackEnd::llvm)
        {
            baseTypeIrType = Ir::Intf::GetFactory()->GetI8();
        }
        else if (backend == Cm::IrIntf::BackEnd::c)
        {
            baseTypeIrType = Ir::Intf::GetFactory()->GetVoid();
        }
    }
    if (type->IsArrayType())
    {
        baseTypeIrType = Cm::IrIntf::Array(baseTypeIrType, type->GetLastArrayDimension());
    }
    if (type->IsReferenceType() || type->IsRvalueRefType())
    {
        if (type->GetBaseType()->IsClassTypeSymbol() || type->GetBaseType()->IsInterfaceTypeSymbol() || type->GetBaseType()->IsDelegateTypeSymbol())
        {
            if (backend == Cm::IrIntf::BackEnd::llvm)
            {
                localVariableObject = Cm::IrIntf::CreateStackVar(assemblyName, Cm::IrIntf::Pointer(baseTypeIrType, type->GetPointerCount() + 2));
            }
            else if (backend == Cm::IrIntf::BackEnd::c)
            {
                localVariableObject = Cm::IrIntf::CreateStackVar(assemblyName, Cm::IrIntf::Pointer(baseTypeIrType, type->GetPointerCount() + 1));
            }
        }
        else
        {
            if (type->GetBaseType()->IsVoidTypeSymbol())
            {
                if (backend == Cm::IrIntf::BackEnd::llvm)
                {
                    localVariableObject = Cm::IrIntf::CreateRefVar(assemblyName, Cm::IrIntf::Pointer(baseTypeIrType, type->GetPointerCount() + 2));
                }
                else if (backend == Cm::IrIntf::BackEnd::c)
                {
                    localVariableObject = Cm::IrIntf::CreateRefVar(assemblyName, Cm::IrIntf::Pointer(baseTypeIrType, type->GetPointerCount() + 1));
                }
            }
            else
            {
                if (backend == Cm::IrIntf::BackEnd::llvm)
                {
                    localVariableObject = Cm::IrIntf::CreateRefVar(assemblyName, Cm::IrIntf::Pointer(baseTypeIrType, type->GetPointerCount() + 2));
                }
                else if (backend == Cm::IrIntf::BackEnd::c)
                {
                    localVariableObject = Cm::IrIntf::CreateRefVar(assemblyName, Cm::IrIntf::Pointer(baseTypeIrType, type->GetPointerCount() + 1));
                }
            }
        }
    }
    else
    {
        if (type->GetBaseType()->IsVoidTypeSymbol())
        {
            if (backend == Cm::IrIntf::BackEnd::llvm)
            {
                localVariableObject = Cm::IrIntf::CreateStackVar(assemblyName, Cm::IrIntf::Pointer(baseTypeIrType, type->GetPointerCount() + 1));
            }
            else if (backend == Cm::IrIntf::BackEnd::c)
            {
                localVariableObject = Cm::IrIntf::CreateStackVar(assemblyName, Cm::IrIntf::Pointer(baseTypeIrType, type->GetPointerCount()));
            }
        }
        else if (type->IsDelegateTypeSymbol())
        {
            Cm::Sym::DelegateTypeSymbol* delegateType = static_cast<Cm::Sym::DelegateTypeSymbol*>(type);
            Ir::Intf::Type* irPtrToDelegateType = irFunctionRepository->CreateIrPointerToDelegateType(delegateType);
            localVariableObject = Cm::IrIntf::CreateStackVar(assemblyName, irPtrToDelegateType);
        }
        else
        {
            if (backend == Cm::IrIntf::BackEnd::llvm)
            {
                localVariableObject = Cm::IrIntf::CreateStackVar(assemblyName, Cm::IrIntf::Pointer(baseTypeIrType, type->GetPointerCount() + 1));
            }
            else if (backend == Cm::IrIntf::BackEnd::c)
            {
                localVariableObject = Cm::IrIntf::CreateStackVar(assemblyName, type->GetIrType());
            }
        }
    }
    localVariableObjectMap[localVariableOrParameter->Sid()] = localVariableObject;
    ownedIrObjects.push_back(std::unique_ptr<Ir::Intf::Object>(localVariableObject));
    return localVariableObject;
}

Ir::Intf::Object* LocalVariableIrObjectRepository::GetLocalVariableIrObject(Cm::Sym::Symbol* localVariableOrParameter)
{
    LocalVariableObjectMapIt i = localVariableObjectMap.find(localVariableOrParameter->Sid());
    if (i != localVariableObjectMap.end())
    {
        return i->second;
    }
    throw std::runtime_error("local variable '" + localVariableOrParameter->Name() + "' not found");
}

void LocalVariableIrObjectRepository::GetFunctionPtrTypes(std::unordered_set<Ir::Intf::Type*>& functionPtrTypes)
{
    for (const std::pair<uint32_t, Ir::Intf::Object*>& p : localVariableObjectMap)
    {
        Ir::Intf::Type* type = p.second->GetType();
        type->GetFunctionPtrTypes(functionPtrTypes);
    }
}

void LocalVariableIrObjectRepository::ReplaceFunctionPtrTypes(const std::unordered_map<Ir::Intf::Type*, Ir::Intf::Type*>& tdfMap)
{
    for (const std::pair<uint32_t, Ir::Intf::Object*>& p : localVariableObjectMap)
    {
        Ir::Intf::Type* type = p.second->GetType();
        type->ReplaceFunctionPtrTypes(tdfMap);
    }
}

IrObjectRepository::IrObjectRepository() 
{
}

Ir::Intf::MemberVar* IrObjectRepository::MakeMemberVariableIrObject(Cm::BoundTree::BoundMemberVariable* boundMemberVariable, Ir::Intf::Object* ptr)
{
    Ir::Intf::MemberVar* memberVar = Cm::IrIntf::CreateMemberVar(boundMemberVariable->Symbol()->Name(), ptr, boundMemberVariable->Symbol()->LayoutIndex(),
        boundMemberVariable->Symbol()->GetType()->GetIrType());
    ownedIrObjects.push_back(std::unique_ptr<Ir::Intf::Object>(memberVar));
    return memberVar;
}

void CompoundDestructionStack::Push(Cm::BoundTree::BoundDestructionStatement* destructionStatement)
{
    destructionStatements.push_back(std::unique_ptr<Cm::BoundTree::BoundDestructionStatement>(destructionStatement));
}

std::unique_ptr<Cm::BoundTree::BoundDestructionStatement> CompoundDestructionStack::Pop()
{
    std::unique_ptr<Cm::BoundTree::BoundDestructionStatement> top = std::move(destructionStatements.back());
    destructionStatements.pop_back();
    return top;
}

void FunctionDestructionStack::Push(CompoundDestructionStack&& compoundDestructionStack)
{
    compoundDestructionStacks.push_back(std::move(compoundDestructionStack));
}

CompoundDestructionStack FunctionDestructionStack::Pop()
{
    CompoundDestructionStack top = std::move(compoundDestructionStacks.back());
    compoundDestructionStacks.pop_back();
    return top;
}

FunctionEmitter::FunctionEmitter(Cm::Util::CodeFormatter& codeFormatter_, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_,
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::StringRepository& stringRepository_, Cm::BoundTree::BoundClass* currentClass_, 
    std::unordered_set<std::string>& internalFunctionNames_, std::unordered_set<Ir::Intf::Function*>& externalFunctions_, 
    Cm::Core::StaticMemberVariableRepository& staticMemberVariableRepository_, Cm::Core::ExternalConstantRepository& externalConstantRepository_, Cm::Ast::CompileUnitNode* currentCompileUnit_, 
    Cm::Sym::FunctionSymbol* enterFrameFun_, Cm::Sym::FunctionSymbol* leaveFrameFun_, Cm::Sym::FunctionSymbol* enterTracedCallFun_, Cm::Sym::FunctionSymbol* leaveTracedCallFun_,
    Cm::Sym::FunctionSymbol* interfaceLookupFailed_, bool generateDebugInfo_, bool profile_) :
    Cm::BoundTree::Visitor(true), emitter(new Cm::Core::Emitter()), codeFormatter(codeFormatter_), genFlags(Cm::Core::GenFlags::none), typeRepository(typeRepository_),
    irFunctionRepository(irFunctionRepository_), irClassTypeRepository(irClassTypeRepository_), stringRepository(stringRepository_), localVariableIrObjectRepository(&irFunctionRepository), 
    compoundResult(), currentCompileUnit(currentCompileUnit_),
    currentClass(currentClass_), currentFunction(nullptr), fid(uint32_t(-1)), profile(profile_), thisParam(nullptr), internalFunctionNames(internalFunctionNames_), externalFunctions(externalFunctions_), 
    staticMemberVariableRepository(staticMemberVariableRepository_), externalConstantRepository(externalConstantRepository_),
    executingPostfixIncDecStatements(false), continueTargetStatement(nullptr), breakTargetStatement(nullptr), currentSwitchEmitState(SwitchEmitState::none), 
    currentSwitchCaseConstantMap(nullptr), switchCaseLabel(nullptr), firstStatementInCompound(false), currentCatchId(-1), enterFrameFun(enterFrameFun_), leaveFrameFun(leaveFrameFun_),
    enterTracedCallFun(enterTracedCallFun_), leaveTracedCallFun(leaveTracedCallFun_), interfaceLookupFailed(interfaceLookupFailed_), generateDebugInfo(generateDebugInfo_), symbolTable(nullptr), 
    endProfiledFunLabel(nullptr), tpGraph(nullptr)
{
    emitter->SetInterfaceLookupFailed(irFunctionRepository.CreateIrFunction(interfaceLookupFailed));
}

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
    currentFunction = &boundFunction;
    Cm::IrIntf::ResetLocalLabelCounter();
    Ir::Intf::Function* irFunction = irFunctionRepository.CreateIrFunction(currentFunction->GetFunctionSymbol());
    externalFunctions.insert(irFunction);
    MapIrFunToFun(irFunction, boundFunction.GetFunctionSymbol());
    internalFunctionNames.insert(irFunction->Name());
    emitter->SetIrFunction(irFunction);

    irFunction->SetComment(boundFunction.GetFunctionSymbol()->FullName());

    if (profile)
    {
        emitter->SetProfilingHandler(this);
        EmitStartProfiledFun(boundFunction.GetFunctionSymbol()->FullName());
    }

    Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
    Ir::Intf::Object* exceptionCodeVariable = nullptr;
    if (backend == Cm::IrIntf::BackEnd::llvm)
    {
        exceptionCodeVariable = Cm::IrIntf::CreateStackVar(Cm::IrIntf::GetExCodeVarName(), Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI32(), 1));
    }
    else if (backend == Cm::IrIntf::BackEnd::c)
    {
        exceptionCodeVariable = Cm::IrIntf::CreateStackVar(Cm::IrIntf::GetExCodeVarName(), Ir::Intf::GetFactory()->GetI32());
    }
    emitter->Own(exceptionCodeVariable);
    emitter->Emit(Cm::IrIntf::Alloca(Ir::Intf::GetFactory()->GetI32(), exceptionCodeVariable));
    localVariableIrObjectRepository.SetExceptionCodeVariable(exceptionCodeVariable);
    EmitDummyVar(emitter.get());

    int parameterIndex = 0;
    for (Cm::Sym::ParameterSymbol* parameter : boundFunction.GetFunctionSymbol()->Parameters())
    {
        Ir::Intf::Object* localVariableIrObject = localVariableIrObjectRepository.CreateLocalVariableIrObjectFor(parameter);
        Cm::Sym::TypeSymbol* parameterType = parameter->GetType();
        Ir::Intf::Type* parameterIrType = parameterType->GetIrType();
        if (parameterType->IsPureArrayType())
        {
            parameterIrType = parameterType->GetBaseType()->GetIrType();
            emitter->Emit(Cm::IrIntf::Alloca(parameterIrType, localVariableIrObject, Ir::Intf::GetFactory()->GetI32(), parameterType->GetLastArrayDimension()));
        }
        else
        {
            emitter->Emit(Cm::IrIntf::Alloca(parameterIrType, localVariableIrObject));
        }
        if (currentFunction->GetFunctionSymbol()->IsMemberFunctionSymbol() && !currentFunction->GetFunctionSymbol()->IsStatic() && parameterIndex == 0)
        {
            thisParam = parameter;
        }
        ++parameterIndex;
    }

    for (Cm::Sym::LocalVariableSymbol* localVariable : boundFunction.LocalVariables())
    {
        Ir::Intf::Object* localVariableIrObject = localVariableIrObjectRepository.CreateLocalVariableIrObjectFor(localVariable);
        Cm::Sym::TypeSymbol* localVariableType = localVariable->GetType();
        Ir::Intf::Type* localVariableIrType = localVariableType->GetIrType();
        if (localVariableType->IsPureArrayType())
        {
            localVariableIrType = localVariableType->GetBaseType()->GetIrType();
            localVariableIrType = ReplaceFunctionPtrType(localVariableIrType);
            emitter->Emit(Cm::IrIntf::Alloca(localVariableIrType, localVariableIrObject, Ir::Intf::GetFactory()->GetI32(), localVariableType->GetLastArrayDimension()));
            localVariableIrObject->SetType(Cm::IrIntf::Pointer(localVariableIrType, 1));
        }
        else
        {
            localVariableIrType = ReplaceFunctionPtrType(localVariableIrType);
            emitter->Emit(Cm::IrIntf::Alloca(localVariableIrType, localVariableIrObject));
        }
    }

    Ir::Intf::Object* zero = Ir::Intf::GetFactory()->GetI32()->CreateDefaultValue();
    emitter->Own(zero);
    Cm::IrIntf::Init(*emitter, Ir::Intf::GetFactory()->GetI32(), zero, exceptionCodeVariable);
}

void FunctionEmitter::EndVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
    Ir::Intf::Function* irFunction = emitter->GetIrFunction();
    std::shared_ptr<Cm::Core::GenResult> result = resultStack.Pop();
    if (!irFunction->LastInstructionIsRet())
    {
        Cm::Sym::TypeSymbol* returnType = boundFunction.GetFunctionSymbol()->GetReturnType();
        Ir::Intf::LabelObject* retLabel = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(retLabel);
        emitter->AddNextInstructionLabel(retLabel);
        if (!returnType || returnType->IsVoidTypeSymbol() || boundFunction.GetFunctionSymbol()->ReturnsClassOrInterfaceObjectByValue())
        {
            emitter->Emit(Cm::IrIntf::Ret());
        }
        else
        {
            Ir::Intf::Object* retval = returnType->GetIrType()->CreateDefaultValue();
            emitter->Own(retval);
            emitter->Emit(Cm::IrIntf::Ret(retval));
        }
        result->BackpatchNextTargets(retLabel);
    }
    GenerateLandingPadCode();
    irFunction->Clean();
    Cm::Sym::FunctionSymbol* functionSymbol = boundFunction.GetFunctionSymbol();
    bool weakOdr = functionSymbol->IsReplicated();
    bool inline_ = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::optimize) && functionSymbol->IsInline();
    irFunction->WriteDefinition(codeFormatter, weakOdr, inline_);
    currentFunction = nullptr;

    Cm::Sym::FunctionSymbol* currentFunctionSymbol = boundFunction.GetFunctionSymbol();
    if (currentFunctionSymbol->Parent() && currentFunctionSymbol->Parent()->IsTemplateTypeSymbol())
    {
        Cm::Ast::Node* node = symbolTable->GetNode(currentFunctionSymbol, false);
        if (node)
        {
            if (!node->IsFunctionNode())
            {
                throw std::runtime_error("not function node");
            }
            Cm::Ast::FunctionNode* functionNode = static_cast<Cm::Ast::FunctionNode*>(node);
            functionNode->SetBody(nullptr);
        }
    }
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundLiteral& boundLiteral)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    Ir::Intf::Object* literalValue = boundLiteral.GetValue()->CreateIrObject();
    emitter->Own(literalValue);
    result->SetMainObject(literalValue);
    if (boundLiteral.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        GenJumpingBoolCode(*result);
    }
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundStringLiteral& boundStringLiteral)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    result->SetMainObject(boundStringLiteral.GetType(), typeRepository);
    Ir::Intf::Object* stringConstant = stringRepository.GetStringConstant(boundStringLiteral.Id());
    Ir::Intf::Object* stringObject = stringRepository.GetStringObject(boundStringLiteral.Id());
    SetStringLiteralResult(emitter.get(), result->MainObject(), stringConstant, stringObject);
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundConstant& boundConstant)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    if (boundConstant.IsBoundExceptionTableConstant())
    {
        Ir::Intf::Object* exceptionTableConstant = externalConstantRepository.GetExceptionBaseIdTable();
        Cm::Sym::TypeSymbol* type = boundConstant.GetType();
        result->SetMainObject(type, typeRepository);
        result->AddObject(exceptionTableConstant);
        Cm::IrIntf::Init(*emitter, type->GetIrType(), result->Arg1(), result->MainObject());
        resultStack.Push(result);
        return;
    }
    else if (boundConstant.IsBoundClassHierarchyTableConstant())
    {
        Ir::Intf::Object* classHierarchyTableConstant = externalConstantRepository.GetClassHierarchyTable();
        Cm::Sym::TypeSymbol* type = boundConstant.GetType();
        result->SetMainObject(type, typeRepository);
        result->AddObject(classHierarchyTableConstant);
        Cm::IrIntf::Init(*emitter, type->GetIrType(), result->Arg1(), result->MainObject());
        resultStack.Push(result);
        return;
    }
    Ir::Intf::Object* constantValue = boundConstant.Symbol()->GetValue()->CreateIrObject();
    emitter->Own(constantValue);
    result->SetMainObject(constantValue);
    if (boundConstant.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        GenJumpingBoolCode(*result);
    }
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundEnumConstant& boundEnumConstant) 
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    Ir::Intf::Object* enumConstantValue = boundEnumConstant.Symbol()->GetValue()->CreateIrObject();
    emitter->Own(enumConstantValue);
    result->SetMainObject(enumConstantValue);
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundLocalVariable& boundLocalVariable)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    if (boundLocalVariable.IsBoundExceptionCodeVariable())
    {
        if (boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef) || boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue))
        {
            result->SetMainObject(localVariableIrObjectRepository.GetExceptionCodeVariable());
        }
        else
        {
            Cm::Sym::TypeSymbol* type = boundLocalVariable.GetType();
            result->SetMainObject(type, typeRepository);
            result->AddObject(localVariableIrObjectRepository.GetExceptionCodeVariable());
            Cm::IrIntf::Init(*emitter, type->GetIrType(), result->Arg1(), result->MainObject());
        }
        resultStack.Push(result);
        return;
    }
    Cm::Sym::TypeSymbol* type = boundLocalVariable.Symbol()->GetType();
    bool typeIsReferenceType = type->IsReferenceType() || type->IsRvalueRefType();
    bool byRefOrClassType = boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef) || boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue) || 
        type->IsClassTypeSymbol() || type->IsInterfaceTypeSymbol();
    if ((boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef) || boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue)) && typeIsReferenceType)
    {
        byRefOrClassType = false; // already reference type
    }
    if (byRefOrClassType || boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::constructVariable))
    {
        result->SetMainObject(MakeLocalVarIrObject(type, localVariableIrObjectRepository.GetLocalVariableIrObject(boundLocalVariable.Symbol())));
    }
    else
    {
        Cm::Sym::TypeSymbol* type = boundLocalVariable.Symbol()->GetType();
        result->SetMainObject(type, typeRepository);
        result->AddObject(localVariableIrObjectRepository.GetLocalVariableIrObject(boundLocalVariable.Symbol()));
        Cm::IrIntf::Init(*emitter, type->GetIrType(), result->Arg1(), result->MainObject());
        if (boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
        {
            GenJumpingBoolCode(*result);
        }
    }
    if (boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundLocalVariable.GetType()), *result);
    }
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundParameter& boundParameter)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    if (boundParameter.IsBoundExceptionCodeParameter())
    {
        result->SetMainObject(irFunctionRepository.GetExceptionCodeParam());
        resultStack.Push(result);
        return;
    }
    Cm::Sym::TypeSymbol* type = boundParameter.Symbol()->GetType();
    bool typeIsReferenceType = type->IsReferenceType() || type->IsRvalueRefType();
    bool byRef = boundParameter.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef) || boundParameter.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
    if ((boundParameter.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef) || boundParameter.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue)) && typeIsReferenceType)
    {
        byRef = false; // already reference type
    }
    if (byRef)
    {
        result->SetMainObject(localVariableIrObjectRepository.GetLocalVariableIrObject(boundParameter.Symbol()));
    }
    else
    {
        Cm::Sym::TypeSymbol* type = boundParameter.Symbol()->GetType();
        result->SetMainObject(type, typeRepository);
        result->AddObject(localVariableIrObjectRepository.GetLocalVariableIrObject(boundParameter.Symbol()));
        Cm::IrIntf::Init(*emitter, type->GetIrType(), result->Arg1(), result->MainObject());
        if (boundParameter.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
        {
            GenJumpingBoolCode(*result);
        }
    }
    if (boundParameter.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundParameter.GetType()), *result);
    }
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundMemberVariable& boundMemberVariable)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    if (boundMemberVariable.Symbol()->IsStatic())
    {
        Ir::Intf::Object* irObject = staticMemberVariableRepository.GetStaticMemberVariableIrObject(boundMemberVariable.Symbol());
        Cm::Sym::TypeSymbol* memberVariableType = boundMemberVariable.Symbol()->GetType();
        if (boundMemberVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue) || boundMemberVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef) || 
            memberVariableType->IsClassTypeSymbol() || memberVariableType->IsInterfaceTypeSymbol())
        {
            result->SetMainObject(irObject);
        }
        else
        {
            Cm::Sym::TypeSymbol* type = boundMemberVariable.Symbol()->GetType();
            result->SetMainObject(type, typeRepository);
            result->AddObject(irObject);
            Cm::IrIntf::Init(*emitter, type->GetIrType(), result->Arg1(), result->MainObject());
        }
    }
    else
    {
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
        std::shared_ptr<Cm::Core::GenResult> ptrResult = resultStack.Pop();
        Cm::Sym::TypeSymbol* type = boundMemberVariable.Symbol()->GetType();
        Ir::Intf::MemberVar* memberVariableIrObject = irObjectRepository.MakeMemberVariableIrObject(&boundMemberVariable, ptrResult->MainObject());
        if (!ptrResult->MainObject()->GetType()->IsPointerType())
        {
            memberVariableIrObject->SetDotMember();
        }
        if (boundMemberVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue) || boundMemberVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef))
        {
            result->SetMainObject(memberVariableIrObject->CreateAddr(*emitter, type->GetIrType()));
        }
        else
        {
            result->SetMainObject(type, typeRepository);
            result->AddObject(memberVariableIrObject);
            Cm::IrIntf::Init(*emitter, type->GetIrType(), result->Arg1(), result->MainObject());
        }
        result->Merge(ptrResult);
    }
    if (boundMemberVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundMemberVariable.GetType()), *result);
    }
    if (boundMemberVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        GenJumpingBoolCode(*result);
    }
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundFunctionId& boundFunctionId)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    result->SetMainObject(irFunctionRepository.GetFunctionId(boundFunctionId.FunctionSymbol(), boundFunctionId.GetType()));
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundConversion& boundConversion)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    Ir::Intf::Object* temporary = nullptr;
    Cm::Sym::FunctionSymbol* conversionFun = boundConversion.ConversionFun();
    Ir::Intf::LabelObject* resultLabel = nullptr;
    if (conversionFun->IsConversionFunction())
    {
        result->SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
        boundConversion.Operand()->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> operandResult = resultStack.Pop();
        if (boundConversion.Operand()->GetFlag(Cm::BoundTree::BoundNodeFlags::addrArg))
        {
            result->SetAddrArg();
        }
        resultLabel = operandResult->GetLabel();
        result->Merge(operandResult);
    }
    Cm::BoundTree::BoundExpression* boundTemporary = boundConversion.BoundTemporary();
    if (boundTemporary)
    {
        Cm::Sym::TypeSymbol* temporaryType = boundTemporary->GetType();
        boundTemporary->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> temporaryResult = resultStack.Pop();
        if (!resultLabel)
        {
            resultLabel = temporaryResult->GetLabel();
        }
        temporary = temporaryResult->MainObject();
        result->Merge(temporaryResult);
        if (temporaryType->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* temporaryClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(temporaryType);
            if (temporaryClassType->Destructor())
            {
                currentCompoundDestructionStack.Push(new Cm::BoundTree::BoundDestructionStatement(nullptr, temporary, temporaryClassType->Destructor()));
            }
        }
    }
    else
    {
        result->SetMainObject(boundConversion.GetType(), typeRepository);
    }
    if (boundConversion.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        result->SetGenJumpingBoolCode();
    }
    if (!conversionFun->IsConversionFunction())
    {
        boundConversion.Operand()->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> operandResult = resultStack.Pop();
        if (boundConversion.Operand()->GetFlag(Cm::BoundTree::BoundNodeFlags::addrArg))
        {
            result->SetAddrArg();
        }
        if (boundConversion.Operand()->GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef))
        {
            result->SetArgByRef();
        }
        if (!resultLabel)
        {
            resultLabel = operandResult->GetLabel();
        }
        result->Merge(operandResult);
    }
    if ((boundConversion.Operand()->GetType()->IsClassTypeSymbol() || boundConversion.Operand()->GetType()->IsInterfaceTypeSymbol()) && 
        (boundConversion.GetType()->IsPointerType() || boundConversion.GetType()->IsReferenceType() || boundConversion.GetType()->IsRvalueRefType()))
    {
        result->SetClassTypeToPointerTypeConversion();
        if (boundConversion.Operand()->GetType()->IsInterfaceTypeSymbol())
        {
            result->SetArgByRef();
        }
    }
    GenerateCall(conversionFun, nullptr, *result);
    if (boundConversion.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundConversion.GetType()), *result);
    }
    if (temporary)
    {
        result->SetMainObject(temporary);
    }
    if (resultLabel)
    {
        result->SetLabel(resultLabel);
    }
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundCast& boundCast)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    result->SetMainObject(boundCast.GetType(), typeRepository);
    if (boundCast.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        result->SetGenJumpingBoolCode();
    }
    boundCast.Operand()->Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> operandResult = resultStack.Pop();
    Ir::Intf::LabelObject* resultLabel = operandResult->GetLabel();
    result->Merge(operandResult);
    Cm::Sym::FunctionSymbol* conversionFun = boundCast.ConversionFun();
    GenerateCall(conversionFun, nullptr, *result);
    if (boundCast.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundCast.GetType()), *result);
    }
    if (resultLabel)
    {
        result->SetLabel(resultLabel);
    }
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundSizeOfExpression& boundSizeOfExpr)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    result->SetMainObject(Cm::IrIntf::SizeOf(*emitter, boundSizeOfExpr.Type()->GetIrType()));
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundUnaryOp& boundUnaryOp)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    Cm::Sym::FunctionSymbol* op = boundUnaryOp.GetFunction();
    bool functionReturnsClassOrInterfaceObjectByValue = op->ReturnsClassOrInterfaceObjectByValue() && !op->IsBasicTypeOp();
    if (functionReturnsClassOrInterfaceObjectByValue)
    {
        result->SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
    }
    else
    {
        result->SetMainObject(op->GetReturnType(), typeRepository);
    }
    std::shared_ptr<Cm::Core::GenResult> operandResult = resultStack.Pop();
    if (boundUnaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        result->SetGenJumpingBoolCode();
    }
    if (boundUnaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue) || boundUnaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef))
    {
        result->SetLvalue();
    }
    Ir::Intf::LabelObject* resultLabel = operandResult->GetLabel();
    result->Merge(operandResult);
    Ir::Intf::Object* classOrInterfaceObjectResultValue = nullptr;
    if (functionReturnsClassOrInterfaceObjectByValue)
    {
        Cm::BoundTree::BoundLocalVariable classOrInterfaceObjectResultVar(boundUnaryOp.SyntaxNode(), boundUnaryOp.GetClassOrInterfaceObjectResultVar());
        classOrInterfaceObjectResultVar.SetType(classOrInterfaceObjectResultVar.Symbol()->GetType());
        classOrInterfaceObjectResultVar.Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> argResult = resultStack.Pop();
        if (!resultLabel)
        {
            resultLabel = argResult->GetLabel();
        }
        classOrInterfaceObjectResultValue = argResult->MainObject();
        result->Merge(argResult);
    }
    GenerateCall(op, boundUnaryOp.GetTraceCallInfo(), *result);
    if (boundUnaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundUnaryOp.GetType()), *result);
    }
    if (functionReturnsClassOrInterfaceObjectByValue)
    {
        result->SetMainObject(classOrInterfaceObjectResultValue);
    }
    if (resultLabel)
    {
        result->SetLabel(resultLabel);
    }
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundBinaryOp& boundBinaryOp)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    std::shared_ptr<Cm::Core::GenResult> right = resultStack.Pop();
    std::shared_ptr<Cm::Core::GenResult> left = resultStack.Pop();
    bool functionReturnsClassOrInterfaceObjectByValue = boundBinaryOp.GetFunction()->ReturnsClassOrInterfaceObjectByValue();
    if (functionReturnsClassOrInterfaceObjectByValue)
    {
        result->SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
    }
    else
    {
        result->SetMainObject(boundBinaryOp.GetType(), typeRepository);
    }
    if (boundBinaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        result->SetGenJumpingBoolCode();
    }
    Ir::Intf::LabelObject* resultLabel = left->GetLabel();
    if (!resultLabel)
    {
        resultLabel = right->GetLabel();
    }
    result->Merge(left);
    result->Merge(right);
    Ir::Intf::Object* classOrInterfaceObjectResultValue = nullptr;
    if (functionReturnsClassOrInterfaceObjectByValue)
    {
        Cm::BoundTree::BoundLocalVariable classOrInterfaceObjectResultVar(boundBinaryOp.SyntaxNode(), boundBinaryOp.GetClassOrInterfaceObjectResultVar());
        classOrInterfaceObjectResultVar.SetType(classOrInterfaceObjectResultVar.Symbol()->GetType());
        classOrInterfaceObjectResultVar.Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> argResult = resultStack.Pop();
        if (!resultLabel)
        {
            resultLabel = argResult->GetLabel();
        }
        classOrInterfaceObjectResultValue = argResult->MainObject();
        result->Merge(argResult);
    }
    Cm::Sym::FunctionSymbol* op = boundBinaryOp.GetFunction();
    GenerateCall(op, boundBinaryOp.GetTraceCallInfo(), *result);
    if (boundBinaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundBinaryOp.GetType()), *result);
    }
    if (functionReturnsClassOrInterfaceObjectByValue)
    {
        result->SetMainObject(classOrInterfaceObjectResultValue);
    }
    if (resultLabel)
    {
        result->SetLabel(resultLabel);
    }
    resultStack.Push(result);
}

Cm::Sym::FunctionSymbol* ResolveVirtualCall(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::FunctionSymbol* fun, const std::unordered_set<Cm::Sym::ClassTypeSymbol*>& reachingClasses)
{
    Cm::Sym::FunctionSymbol* devirtualizedFun = nullptr;
    for (Cm::Sym::ClassTypeSymbol* classType : reachingClasses)
    {
        bool found = false;
        while (!found && classType)
        {
            Cm::Sym::TypeSymbol* thisPointerType = nullptr;
            if (fun->IsConst())
            {
                thisPointerType = typeRepository.MakeConstPointerType(classType, Cm::Parsing::Span());
            }
            else
            {
                thisPointerType = typeRepository.MakePointerType(classType, Cm::Parsing::Span());
            }
            if (fun->ThisParameter()->GetType()->FullName() == thisPointerType->FullName())
            {
                if (devirtualizedFun == nullptr)
                {
                    devirtualizedFun = fun;
                }
                else if (devirtualizedFun->FullName() != fun->FullName())
                {
                    return nullptr;
                }
            }
            for (Cm::Sym::FunctionSymbol* inheritedFun : fun->OverrideSet())
            {
                if (inheritedFun->ThisParameter()->GetType()->FullName() == thisPointerType->FullName())
                {
                    if (devirtualizedFun == nullptr)
                    {
                        devirtualizedFun = inheritedFun;
                        break;
                    }
                    else if (devirtualizedFun->FullName() != inheritedFun->FullName())
                    {
                        return nullptr;
                    }
                    else
                    {
                        found = true;
                        break;
                    }
                }
            }
            if (!found)
            {
                classType = classType->BaseClass();
            }
        }
    }
    return devirtualizedFun;
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundFunctionCall& functionCall)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    bool functionReturnsClassOrInterfaceObjectByValue = functionCall.GetFunction()->ReturnsClassOrInterfaceObjectByValue();
    if (functionReturnsClassOrInterfaceObjectByValue)
    {
        result->SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
    }
    else if (!functionCall.GetFunction()->IsBasicTypeOp() && !functionCall.GetFunction()->IsConstructorOrDestructorSymbol())
    {
        result->SetMainObject(functionCall.GetType(), typeRepository);
    }
    if (functionCall.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        result->SetGenJumpingBoolCode();
    }
    Ir::Intf::LabelObject* resultLabel = nullptr;
    for (std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : functionCall.Arguments())
    {
        argument->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> argResult = resultStack.Pop();
        if (!resultLabel)
        {
            resultLabel = argResult->GetLabel();
        }
        result->Merge(argResult);
    }
    Ir::Intf::Object* classOrInterfaceObjectResultValue = nullptr;
    if (functionReturnsClassOrInterfaceObjectByValue)
    {
        Cm::BoundTree::BoundLocalVariable classObjectResultVar(functionCall.SyntaxNode(), functionCall.GetClassOrInterfaceObjectResultVar());
        classObjectResultVar.SetType(classObjectResultVar.Symbol()->GetType());
        classObjectResultVar.Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> argResult = resultStack.Pop();
        if (!resultLabel)
        {
            resultLabel = argResult->GetLabel();
        }
        classOrInterfaceObjectResultValue = argResult->MainObject();
        result->Merge(argResult);
    }
    Cm::Sym::FunctionSymbol* fun = functionCall.GetFunction();
    bool generateVirtualCall = functionCall.GetFlag(Cm::BoundTree::BoundNodeFlags::genVirtualCall);
    if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::fullConfig) && generateVirtualCall && tpGraph)
    {
        uint32_t functionCallSid = functionCall.FunctionCallSid();
        std::unordered_map<uint32_t, Cm::Opt::TpGraphNode*>::const_iterator i = tpGraph->VirtualCallMap().find(functionCallSid);
        if (i != tpGraph->VirtualCallMap().cend())
        {
            Cm::Opt::TpGraphNode* node = i->second;
            Cm::Sym::FunctionSymbol* devirtualizedFun = ResolveVirtualCall(typeRepository, fun, node->ReachingClasses());
            if (devirtualizedFun != nullptr)
            {
                irClassTypeRepository.AddClassType(static_cast<Cm::Sym::ClassTypeSymbol*>(devirtualizedFun->ThisParameter()->GetType()->GetBaseType()));
                Ir::Intf::Type* thisPtrType = devirtualizedFun->ThisParameter()->GetType()->GetIrType();
                Ir::Intf::Object* thisArg = result->Arg1();
                Ir::Intf::Type* thisArgType = thisArg->GetType();
                if (thisPtrType->Name() != thisArgType->Name())
                {
                    Ir::Intf::Object* devirtualizedFunThisArg = Cm::IrIntf::CreateTemporaryRegVar(thisPtrType);
                    emitter->Own(devirtualizedFunThisArg);
                    emitter->Emit(Cm::IrIntf::Bitcast(thisArgType, devirtualizedFunThisArg, thisArg, thisPtrType));
                    result->Objects()[1] = devirtualizedFunThisArg;
                }
                std::string vCallFileName = Cm::Core::GetGlobalSettings()->VirtualCallFileName();
                if (!vCallFileName.empty())
                {
                    if (boost::filesystem::path(vCallFileName).extension().empty())
                    {
                        vCallFileName.append(".txt");
                    }
                    std::ofstream vCallFile(vCallFileName, std::ios_base::app);
                    vCallFile << "virtual function call " << functionCall.FunctionCallSid() << " '" << currentFunction->GetFunctionSymbol()->FullName() << "$" << fun->FullName() << "' devirtualized to '" << devirtualizedFun->FullName() << "' : " << 
                        node->ReachingClassSetStr() << std::endl;
                }
                fun = devirtualizedFun;
                generateVirtualCall = false;
                tpGraph->IncDevirtualizedFunctionCalls();
                if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::debugVCalls) && functionCall.SidLiteral())
                {
                    functionCall.SidLiteral()->Accept(*this);
                    std::shared_ptr<Cm::Core::GenResult> sidLiteralResult = resultStack.Pop();
                    std::shared_ptr<Cm::Core::GenResult> writeResult(new Cm::Core::GenResult(emitter.get(), genFlags));
                    Ir::Intf::Object* mainObject = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI32());
                    emitter->Own(mainObject);
                    writeResult->SetMainObject(mainObject);
                    Ir::Intf::Object* two = Cm::IrIntf::CreateI32Constant(2);
                    emitter->Own(two);
                    writeResult->AddObject(two);
                    writeResult->AddObject(sidLiteralResult->MainObject());
                    Cm::Sym::FunctionSymbol* write = symbolTable->GetOverload("System.Support.Write");
                    if (!write)
                    {
                        throw std::runtime_error("could not get System.Support.Write function from symbol table");
                    }
                    GenerateCall(write, nullptr, *writeResult);
                }
            }
        }
    }
    if (generateVirtualCall)
    {
        result->SetGenVirtualCall();
    }
    GenerateCall(fun, functionCall.GetTraceCallInfo(), *result);
    if (functionCall.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(functionCall.GetType()), *result);
    }
    if (functionReturnsClassOrInterfaceObjectByValue)
    {
        result->SetMainObject(classOrInterfaceObjectResultValue);
    }
    else if (functionCall.GetTemporary())
    {
        if (functionCall.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef))
        {
            functionCall.GetTemporary()->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        }
        functionCall.GetTemporary()->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> temporaryResult = resultStack.Pop();
        result->SetMainObject(temporaryResult->MainObject());
    }
    if (resultLabel)
    {
        result->SetLabel(resultLabel);
    }
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundDelegateCall& boundDelegateCall)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    result->SetMainObject(boundDelegateCall.GetType(), typeRepository);
    boundDelegateCall.Subject()->Accept(*this);
    Ir::Intf::LabelObject* resultLabel = nullptr;
    std::shared_ptr<Cm::Core::GenResult> subjectResult = resultStack.Pop();
    if (!resultLabel)
    {
        resultLabel = subjectResult->GetLabel();
    }
    Ir::Intf::Object* funPtr = subjectResult->MainObject();
    for (const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : boundDelegateCall.Arguments())
    {
        argument->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> argumentResult = resultStack.Pop();
        if (!resultLabel)
        {
            resultLabel = argumentResult->GetLabel();
        }   
        result->Merge(argumentResult);
    }
    if (!boundDelegateCall.DelegateType()->IsNothrow())
    {
        result->AddObject(localVariableIrObjectRepository.GetExceptionCodeVariable());
    }
    emitter->Emit(Cm::IrIntf::IndirectCall(result->MainObject(), funPtr, result->Args()));
    if (!boundDelegateCall.DelegateType()->IsNothrow())
    {
        GenerateTestExceptionResult();
    }
    if (resultLabel)
    {
        result->SetLabel(resultLabel);
    }
    result->Merge(subjectResult);
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundClassDelegateCall& boundClassDelegateCall)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    result->SetMainObject(boundClassDelegateCall.GetType(), typeRepository);
    boundClassDelegateCall.Subject()->Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> subjectResult = resultStack.Pop();
    Cm::Sym::Symbol* objSymbol = boundClassDelegateCall.ClassDelegateType()->GetContainerScope()->Lookup("obj");
    if (!objSymbol->IsMemberVariableSymbol())
    {
        throw std::runtime_error("not member variable symbol");
    }
    Cm::Sym::MemberVariableSymbol* objMemberVarSymbol = static_cast<Cm::Sym::MemberVariableSymbol*>(objSymbol);
    std::unique_ptr<Cm::BoundTree::BoundMemberVariable> objMemberVar(new Cm::BoundTree::BoundMemberVariable(nullptr, objMemberVarSymbol));
    Ir::Intf::Object* obj = irObjectRepository.MakeMemberVariableIrObject(objMemberVar.get(), subjectResult->MainObject());
    Ir::Intf::RegVar* thisPtr = Cm::IrIntf::CreateTemporaryRegVar(obj->GetType()->Clone());
    emitter->Own(thisPtr);
    Cm::IrIntf::Assign(*emitter, obj->GetType(), obj, thisPtr);
    result->AddObject(thisPtr);
    for (const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : boundClassDelegateCall.Arguments())
    {
        argument->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> argumentResult = resultStack.Pop();
        result->Merge(argumentResult);
    }
    if (!boundClassDelegateCall.ClassDelegateType()->IsNothrow())
    {
        result->AddObject(localVariableIrObjectRepository.GetExceptionCodeVariable());
    }
    Cm::Sym::Symbol* dlgSymbol = boundClassDelegateCall.ClassDelegateType()->GetContainerScope()->Lookup("dlg");
    if (!dlgSymbol->IsMemberVariableSymbol())
    {
        throw std::runtime_error("not member variable symbol");
    }
    Cm::Sym::MemberVariableSymbol* dlgMemberVarSymbol = static_cast<Cm::Sym::MemberVariableSymbol*>(dlgSymbol);
    std::unique_ptr<Cm::BoundTree::BoundMemberVariable> dlgMemberVar(new Cm::BoundTree::BoundMemberVariable(nullptr, dlgMemberVarSymbol));
    Ir::Intf::Object* dlg = irObjectRepository.MakeMemberVariableIrObject(dlgMemberVar.get(), subjectResult->MainObject());
    Ir::Intf::RegVar* funPtr = Cm::IrIntf::CreateTemporaryRegVar(dlg->GetType());
    emitter->Own(funPtr);
    Cm::IrIntf::Assign(*emitter, dlg->GetType(), dlg, funPtr);
    emitter->Emit(Cm::IrIntf::IndirectCall(result->MainObject(), funPtr, result->Args()));
    result->Merge(subjectResult);
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundDisjunction& boundDisjunction)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    Ir::Intf::LabelObject* next = nullptr;
    if (boundDisjunction.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        boundDisjunction.Left()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundDisjunction.Left()->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> leftResult = resultStack.Pop();
        if (!result->LabelSet())
        {
            result->SetLabelSet();
            result->SetLabel(leftResult->GetLabel());
        }

        functionDestructionStack.Push(std::move(currentCompoundDestructionStack));
        currentCompoundDestructionStack = CompoundDestructionStack();
        boundDisjunction.Right()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundDisjunction.Right()->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> rightResult = resultStack.Pop();
        leftResult->BackpatchFalseTargets(rightResult->GetLabel());
        if (currentCompoundDestructionStack.IsEmpty())
        {
            result->MergeTargets(result->FalseTargets(), rightResult->FalseTargets());
            result->MergeTargets(result->TrueTargets(), leftResult->TrueTargets());
            result->MergeTargets(result->TrueTargets(), rightResult->TrueTargets());
        }
        else
        {
            bool trueFirst = true;
            std::shared_ptr<Cm::Core::GenResult> trueResult(new Cm::Core::GenResult(emitter.get(), genFlags));
            ExitCompound(*trueResult, currentCompoundDestructionStack, trueFirst);
            rightResult->BackpatchTrueTargets(trueResult->GetLabel());
            Ir::Intf::LabelObject* trueLabel = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(trueLabel);
            emitter->Emit(Cm::IrIntf::Br(trueLabel));
            result->AddTrueTarget(trueLabel);

            bool falseFirst = true;
            std::shared_ptr<Cm::Core::GenResult> falseResult(new Cm::Core::GenResult(emitter.get(), genFlags));
            ExitCompound(*falseResult, currentCompoundDestructionStack, falseFirst);
            rightResult->BackpatchFalseTargets(falseResult->GetLabel());
            Ir::Intf::LabelObject* falseLabel = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(falseLabel);
            emitter->Emit(Cm::IrIntf::Br(falseLabel));
            result->AddFalseTarget(falseLabel);
            result->MergeTargets(result->TrueTargets(), leftResult->TrueTargets());
            result->Merge(trueResult);
            result->Merge(falseResult);
        }
        currentCompoundDestructionStack = functionDestructionStack.Pop();
        result->Merge(leftResult);
        result->Merge(rightResult);
    }
    else
    {
        boundDisjunction.Left()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundDisjunction.Left()->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> leftResult = resultStack.Pop();
        if (!result->LabelSet())
        {
            result->SetLabelSet();
            result->SetLabel(leftResult->GetLabel());
        }

        functionDestructionStack.Push(std::move(currentCompoundDestructionStack));
        currentCompoundDestructionStack = CompoundDestructionStack();
        boundDisjunction.Right()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundDisjunction.Right()->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> rightResult = resultStack.Pop();
        leftResult->BackpatchFalseTargets(rightResult->GetLabel());
        result->SetMainObject(localVariableIrObjectRepository.GetLocalVariableIrObject(boundDisjunction.GetResultVar()));
        if (currentCompoundDestructionStack.IsEmpty())
        {
            Ir::Intf::LabelObject* falseLabel = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(falseLabel);
            emitter->AddNextInstructionLabel(falseLabel);
            Ir::Intf::Object* false_ = Cm::IrIntf::False();
            emitter->Own(false_);
            Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI1(), false_, result->MainObject());
            rightResult->BackpatchFalseTargets(falseLabel);
            next = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(next);
            emitter->Emit(Cm::IrIntf::Br(next));
            Ir::Intf::LabelObject* trueLabel = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(trueLabel);
            emitter->AddNextInstructionLabel(trueLabel);
            Ir::Intf::Object* true_ = Cm::IrIntf::True();
            emitter->Own(true_);
            Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI1(), true_, result->MainObject());
            leftResult->BackpatchTrueTargets(trueLabel);
            rightResult->BackpatchTrueTargets(trueLabel);
        }
        else
        {
            bool falseFirst = true;
            std::shared_ptr<Cm::Core::GenResult> falseResult(new Cm::Core::GenResult(emitter.get(), genFlags));
            ExitCompound(*falseResult, currentCompoundDestructionStack, falseFirst);
            rightResult->BackpatchFalseTargets(falseResult->GetLabel());
            Ir::Intf::Object* false_ = Cm::IrIntf::False();
            emitter->Own(false_);
            Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI1(), false_, result->MainObject());
            next = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(next);
            emitter->Emit(Cm::IrIntf::Br(next));

            bool trueFirst = true;
            std::shared_ptr<Cm::Core::GenResult> trueResult(new Cm::Core::GenResult(emitter.get(), genFlags));
            ExitCompound(*trueResult, currentCompoundDestructionStack, trueFirst);
            rightResult->BackpatchTrueTargets(trueResult->GetLabel());
            Ir::Intf::LabelObject* assignTrueLabel = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(assignTrueLabel);
            emitter->AddNextInstructionLabel(assignTrueLabel);
            Ir::Intf::Object* true_ = Cm::IrIntf::True();
            emitter->Own(true_);
            Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI1(), true_, result->MainObject());
            leftResult->BackpatchTrueTargets(assignTrueLabel);

            result->Merge(falseResult);
            result->Merge(trueResult);
        }
        currentCompoundDestructionStack = functionDestructionStack.Pop();
        result->Merge(leftResult);
        result->Merge(rightResult);
    }
    resultStack.Push(result);
    if (next)
    {
        emitter->AddNextInstructionLabel(next);
    }
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundConjunction& boundConjunction)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    Ir::Intf::LabelObject* next = nullptr;
    if (boundConjunction.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        boundConjunction.Left()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundConjunction.Left()->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> leftResult = resultStack.Pop();
        if (!result->LabelSet())
        {
            result->SetLabelSet();
            result->SetLabel(leftResult->GetLabel());
        }

        functionDestructionStack.Push(std::move(currentCompoundDestructionStack));
        currentCompoundDestructionStack = CompoundDestructionStack();
        boundConjunction.Right()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundConjunction.Right()->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> rightResult = resultStack.Pop();
        leftResult->BackpatchTrueTargets(rightResult->GetLabel());
        if (currentCompoundDestructionStack.IsEmpty())
        {
            result->MergeTargets(result->TrueTargets(), rightResult->TrueTargets());
            result->MergeTargets(result->FalseTargets(), leftResult->FalseTargets());
            result->MergeTargets(result->FalseTargets(), rightResult->FalseTargets());
        }
        else
        {
            bool trueFirst = true;
            std::shared_ptr<Cm::Core::GenResult> trueResult(new Cm::Core::GenResult(emitter.get(), genFlags));
            ExitCompound(*trueResult, currentCompoundDestructionStack, trueFirst);
            rightResult->BackpatchTrueTargets(trueResult->GetLabel());
            Ir::Intf::LabelObject* trueLabel = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(trueLabel);
            emitter->Emit(Cm::IrIntf::Br(trueLabel));
            result->AddTrueTarget(trueLabel);

            bool falseFirst = true;
            std::shared_ptr<Cm::Core::GenResult> falseResult(new Cm::Core::GenResult(emitter.get(), genFlags));
            ExitCompound(*falseResult, currentCompoundDestructionStack, falseFirst);
            rightResult->BackpatchFalseTargets(falseResult->GetLabel());
            Ir::Intf::LabelObject* falseLabel = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(falseLabel);
            emitter->Emit(Cm::IrIntf::Br(falseLabel));
            result->AddFalseTarget(falseLabel);

            result->MergeTargets(result->FalseTargets(), leftResult->FalseTargets());
            result->Merge(trueResult);
            result->Merge(falseResult);
        }
        currentCompoundDestructionStack = functionDestructionStack.Pop();
        result->Merge(leftResult);
        result->Merge(rightResult);
    }
    else
    {
        boundConjunction.Left()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundConjunction.Left()->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> leftResult = resultStack.Pop();
        if (!result->LabelSet())
        {
            result->SetLabelSet();
            result->SetLabel(leftResult->GetLabel());
        }

        functionDestructionStack.Push(std::move(currentCompoundDestructionStack));
        currentCompoundDestructionStack = CompoundDestructionStack();
        boundConjunction.Right()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundConjunction.Right()->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> rightResult = resultStack.Pop();
        leftResult->BackpatchTrueTargets(rightResult->GetLabel());
        result->SetMainObject(localVariableIrObjectRepository.GetLocalVariableIrObject(boundConjunction.GetResultVar()));
        if (currentCompoundDestructionStack.IsEmpty())
        {
            Ir::Intf::LabelObject* trueLabel = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(trueLabel);
            emitter->AddNextInstructionLabel(trueLabel);
            Ir::Intf::Object* true_ = Cm::IrIntf::True();
            emitter->Own(true_);
            Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI1(), true_, result->MainObject());
            rightResult->BackpatchTrueTargets(trueLabel);
            next = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(next);
            emitter->Emit(Cm::IrIntf::Br(next));
            Ir::Intf::LabelObject* falseLabel = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(falseLabel);
            emitter->AddNextInstructionLabel(falseLabel);
            Ir::Intf::Object* false_ = Cm::IrIntf::False();
            emitter->Own(false_);
            Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI1(), false_, result->MainObject());
            leftResult->BackpatchFalseTargets(falseLabel);
            rightResult->BackpatchFalseTargets(falseLabel);
        }
        else
        {
            bool trueFirst = true;
            std::shared_ptr<Cm::Core::GenResult> trueResult(new Cm::Core::GenResult(emitter.get(), genFlags));
            ExitCompound(*trueResult, currentCompoundDestructionStack, trueFirst);
            rightResult->BackpatchTrueTargets(trueResult->GetLabel());
            next = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(next);
            Ir::Intf::Object* true_ = Cm::IrIntf::True();
            emitter->Own(true_);
            Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI1(), true_, result->MainObject());
            emitter->Emit(Cm::IrIntf::Br(next));

            bool falseFirst = true;
            std::shared_ptr<Cm::Core::GenResult> falseResult(new Cm::Core::GenResult(emitter.get(), genFlags));
            ExitCompound(*falseResult, currentCompoundDestructionStack, falseFirst);
            rightResult->BackpatchFalseTargets(falseResult->GetLabel());
            Ir::Intf::LabelObject* assignFalseLabel = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(assignFalseLabel);
            emitter->AddNextInstructionLabel(assignFalseLabel);
            Ir::Intf::Object* false_ = Cm::IrIntf::False();
            emitter->Own(false_);
            Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI1(), false_, result->MainObject());
            leftResult->BackpatchFalseTargets(assignFalseLabel);
            result->Merge(trueResult);
            result->Merge(falseResult);
        }
        currentCompoundDestructionStack = functionDestructionStack.Pop();
        result->Merge(leftResult);
        result->Merge(rightResult);
    }
    resultStack.Push(result);
    if (next)
    {
        emitter->AddNextInstructionLabel(next);
    }
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundPostfixIncDecExpr& boundPostfixIncDecExpr)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    boundPostfixIncDecExpr.Value()->Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> valueResult = resultStack.Pop();
    Ir::Intf::LabelObject* resultLabel = valueResult->GetLabel();
    result->Merge(valueResult);
    if (resultLabel)
    {
        result->SetLabel(resultLabel);
    }
    resultStack.Push(result);
    postfixIncDecStatements.push_back(std::unique_ptr<Cm::BoundTree::BoundStatement>(boundPostfixIncDecExpr.ReleaseStatement()));
}

void FunctionEmitter::BeginVisitStatement(Cm::BoundTree::BoundStatement& statement)
{
    genFlags = Cm::Core::GenFlags::none;
    nextTargetsStack.push(compoundResult->NextTargets());
    compoundResult->NextTargets().clear();
    if (!statement.Label().empty())
    {
        Ir::Intf::LabelObject* label = Cm::IrIntf::CreateLabel(statement.Label());
        emitter->Own(label);
        emitter->SetGotoTargetLabel(label);
    }
}

void FunctionEmitter::BackpatchNextTargets(Ir::Intf::LabelObject* label)
{
    std::vector<Ir::Intf::LabelObject*> nextTargets = nextTargetsStack.top();
    nextTargetsStack.pop();
    if (label)
    {
        Ir::Intf::Backpatch(nextTargets, label);
    }
    else
    {
        compoundResult->MergeTargets(compoundResult->NextTargets(), nextTargets);
    }
}

void FunctionEmitter::EndVisitStatement(Cm::BoundTree::BoundStatement& statement)
{
    std::shared_ptr<Cm::Core::GenResult> statementResult = resultStack.Pop();
    BackpatchNextTargets(statementResult->GetLabel());
    if (statementResult->GetLabel())
    {
        compoundResult->BackpatchNextTargets(statementResult->GetLabel());
    }
    compoundResult->Merge(statementResult);
    if (!compoundResult->LabelSet())
    {
        if (statementResult->GetLabel())
        {
            compoundResult->SetLabel(statementResult->GetLabel());
            compoundResult->SetLabelSet();
        }
    }
}

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundCompoundStatement& boundCompoundStatement)
{
    if (!boundCompoundStatement.Parent())
    {
        PushGenDebugInfo(generateDebugInfo && boundCompoundStatement.SyntaxNode() != nullptr);
    }
    std::shared_ptr<Cm::Core::GenResult> entryResult = nullptr;
    if (generateDebugInfo)
    {
        if (!boundCompoundStatement.SyntaxNode())
        {
            throw std::runtime_error("block body has no syntax node");
        }
        Cm::Ast::CompoundStatementNode* compoundStatementNode = static_cast<Cm::Ast::CompoundStatementNode*>(boundCompoundStatement.SyntaxNode());
        CreateEntryDebugNode(boundCompoundStatement, compoundStatementNode->BeginBraceSpan());
        entryResult = resultStack.Pop();
    }
    compoundResultStack.Push(compoundResult);
    compoundResult = std::shared_ptr<Cm::Core::GenResult>(new Cm::Core::GenResult(emitter.get(), Cm::Core::GenFlags::none));
    if (entryResult)
    {
        compoundResult->Merge(entryResult);
        compoundResult->SetLabel(entryResult->GetLabel());
        compoundResult->SetLabelSet();
    }
    if (boundCompoundStatement.IsEmpty())
    {
        compoundResult->SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
        DoNothing(*compoundResult);
    }
    functionDestructionStack.Push(std::move(currentCompoundDestructionStack));
    currentCompoundDestructionStack = CompoundDestructionStack();
}

void FunctionEmitter::EndVisit(Cm::BoundTree::BoundCompoundStatement& boundCompoundStatement)
{
    ClearCompoundDestructionStack(*compoundResult);
    if (generateDebugInfo)
    {
        if (!boundCompoundStatement.SyntaxNode())
        {
            throw std::runtime_error("block body has no syntax node");
        }
        Cm::Ast::CompoundStatementNode* compoundStatementNode = static_cast<Cm::Ast::CompoundStatementNode*>(boundCompoundStatement.SyntaxNode());
        CreateExitDebugNode(boundCompoundStatement, compoundStatementNode->EndBraceSpan());
        std::shared_ptr<Cm::Core::GenResult> exitResult = resultStack.Pop();
        compoundResult->Merge(exitResult);
    }
    if (!boundCompoundStatement.Parent())
    {
        PopGenDebugInfo();
    }
    currentCompoundDestructionStack = functionDestructionStack.Pop();
    resultStack.Push(compoundResult);
    compoundResult = compoundResultStack.Pop();
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundReceiveStatement& boundReceiveStatement)
{
    Cm::Sym::ParameterSymbol* parameterSymbol = boundReceiveStatement.GetParameterSymbol();
    if (generateDebugInfo)
    {
        Cm::Parsing::Span span = Cm::Parsing::Span(0, 0, 0, 0);
        if (boundReceiveStatement.SyntaxNode())
        {
            span = boundReceiveStatement.SyntaxNode()->GetSpan();
        }
        Cm::Core::CfgNode* node = CreateDebugNode(boundReceiveStatement, span, true);
        if (!node)
        {
            throw std::runtime_error("node is null");
        }
        int32_t defNode = node->Id();
        std::string irName;
        Ir::Intf::Object* irObject = localVariableIrObjectRepository.GetLocalVariableIrObject(parameterSymbol);
        if (irObject)
        {
            irName = irObject->Name();
        }
        else
        {
            throw std::runtime_error("irObject is null");
        }
        std::string typeName = parameterSymbol->GetType()->FullName();
        Cm::Core::CFunctionDebugInfo* functionDebugInfo = GetFunctionDebugInfo();
        if (functionDebugInfo)
        {
            functionDebugInfo->Locals().AddLocal(new Cm::Core::Local(parameterSymbol->Name(), irName, typeName, defNode));
        }
        else
        {
            throw std::runtime_error("functionDebugInfo is null");
        }
    }
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    result->SetMainObject(localVariableIrObjectRepository.GetLocalVariableIrObject(parameterSymbol));
    Ir::Intf::Parameter* irParameter = irFunctionRepository.CreateIrParameter(parameterSymbol);
    emitter->Own(irParameter);
    result->AddObject(irParameter);
    Cm::Sym::FunctionSymbol* ctor = boundReceiveStatement.Constructor();
    GenerateCall(ctor, nullptr, *result);
    if (parameterSymbol->GetType()->IsClassTypeSymbol() && !parameterSymbol->GetType()->IsClassDelegateTypeSymbol())
    {
        Cm::Sym::ClassTypeSymbol* parameterClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(parameterSymbol->GetType());
        if (parameterClassType->Destructor())
        {
            currentCompoundDestructionStack.Push(new Cm::BoundTree::BoundDestructionStatement(boundReceiveStatement.SyntaxNode(), result->MainObject(), parameterClassType->Destructor()));
        }
    }
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundInitClassObjectStatement& boundInitClassObjectStatement)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    boundInitClassObjectStatement.FunctionCall()->Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> callResult = resultStack.Pop();
    result->Merge(callResult);
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundInitMemberVariableStatement& boundInitMemberVariableStatement)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    int n = boundInitMemberVariableStatement.Arguments().Count();
    Ir::Intf::LabelObject* resultLabel = nullptr;
    for (int i = 0; i < n; ++i)
    {
        const std::unique_ptr<Cm::BoundTree::BoundExpression>& arg = boundInitMemberVariableStatement.Arguments()[i];
        arg->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> argResult = resultStack.Pop();
        if (!resultLabel)
        {
            resultLabel = argResult->GetLabel();
        }
        result->Merge(argResult);
    }
    Cm::Sym::FunctionSymbol* ctor = boundInitMemberVariableStatement.Constructor();
    result->SetMemberVar();
    GenerateCall(ctor, nullptr, *result);
    if (boundInitMemberVariableStatement.RegisterDestructor())
    {
        RegisterDestructor(boundInitMemberVariableStatement.GetMemberVariableSymbol());
    }
    if (resultLabel)
    {
        result->SetLabel(resultLabel);
    }
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundFunctionCallStatement& boundFunctionCallStatement) 
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    Cm::Sym::FunctionSymbol* function = boundFunctionCallStatement.Function();
    if (!function->IsBasicTypeOp() && !function->IsConstructorOrDestructorSymbol())
    {
        result->SetMainObject(function->GetReturnType(), typeRepository);
    }
    int n = boundFunctionCallStatement.Arguments().Count();
    Ir::Intf::LabelObject* resultLabel = nullptr;
    for (int i = 0; i < n; ++i)
    {
        const std::unique_ptr<Cm::BoundTree::BoundExpression>& arg = boundFunctionCallStatement.Arguments()[i];
        arg->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> argResult = resultStack.Pop();
        if (!resultLabel)
        {
            resultLabel = argResult->GetLabel();
        }
        result->Merge(argResult);
    }
    GenerateCall(function, nullptr, *result);
    ExecutePostfixIncDecStatements(*result);
    if (resultLabel)
    {
        result->SetLabel(resultLabel);
    }
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundReturnStatement& boundReturnStatement) 
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    std::shared_ptr<Cm::Core::GenResult> entryResult = nullptr;
    Ir::Intf::LabelObject* resultLabel = nullptr;
    if (generateDebugInfo)
    {
        if (!boundReturnStatement.SyntaxNode())
        {
            throw std::runtime_error("no syntax node");
        }
        CreateEntryDebugNode(boundReturnStatement, boundReturnStatement.SyntaxNode()->GetSpan());
        entryResult = resultStack.Pop();
        resultLabel = entryResult->GetLabel();
        compoundResult->Merge(entryResult);
    }
    if (boundReturnStatement.Expression())
    {
        boundReturnStatement.Expression()->Accept(*this);
    }
    if (boundReturnStatement.ReturnsValue())
    {
        bool resultSet = false;
        std::shared_ptr<Cm::Core::GenResult> retValResult = resultStack.Pop();
        Cm::Sym::FunctionSymbol* ctor = boundReturnStatement.Constructor();
        bool returnsClassOrInterfaceObjectByValue = currentFunction->GetFunctionSymbol()->ReturnsClassOrInterfaceObjectByValue();
        if (returnsClassOrInterfaceObjectByValue)
        {
            result->SetMainObject(currentFunction->GetFunctionSymbol()->ClassOrInterfaceObjectResultIrParam());
        }
        else if (boundReturnStatement.Temporary())
        {
            std::shared_ptr<Cm::Core::GenResult> temporaryResult(new Cm::Core::GenResult(emitter.get(), genFlags));
            temporaryResult->SetMainObject(localVariableIrObjectRepository.GetLocalVariableIrObject(boundReturnStatement.Temporary()->Symbol()));
            temporaryResult->Merge(retValResult);
            GenerateCall(ctor, boundReturnStatement.GetTraceCallInfo(), *temporaryResult);
            result->SetMainObject(boundReturnStatement.GetReturnType(), typeRepository);
            result->Merge(temporaryResult);
        }
        else
        {
            result->SetMainObject(boundReturnStatement.GetReturnType(), typeRepository);
        }
        if (!resultLabel)
        {
            resultLabel = retValResult->GetLabel();
        }
        if (ctor->IsDelegateFromFunCtor())
        {
            result->SetMainObject(retValResult->MainObject());
            Cm::Bind::DelegateFromFunCtor* delegateFromFunCtor = static_cast<Cm::Bind::DelegateFromFunCtor*>(ctor);
            result->Objects()[0]->SetType(delegateFromFunCtor->DelegateType()->GetIrType());
            resultSet = true;
        }
        result->Merge(retValResult);
        Ir::Intf::LabelObject* exprNext = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(exprNext);
        emitter->AddNextInstructionLabel(exprNext);
        if (!resultLabel)
        {
            resultLabel = exprNext;
        }
        if (!resultSet)
        {
            GenerateCall(ctor, boundReturnStatement.GetTraceCallInfo(), *result);
        }
        ExecutePostfixIncDecStatements(*result);
        ExitFunction(*result);
        if (generateDebugInfo)
        {
            if (!currentFunction->Body()->SyntaxNode())
            {
                throw std::runtime_error("current function body has no syntax node");
            }
            Cm::Ast::CompoundStatementNode* compoundStatementNode = static_cast<Cm::Ast::CompoundStatementNode*>(currentFunction->Body()->SyntaxNode());
            CreateExitDebugNode(boundReturnStatement, compoundStatementNode->EndBraceSpan());
            std::shared_ptr<Cm::Core::GenResult> exitResult = resultStack.Pop();
            compoundResult->Merge(exitResult);
        }
        if (returnsClassOrInterfaceObjectByValue)
        {
            emitter->Emit(Cm::IrIntf::Ret());
        }
        else
        {
            emitter->Emit(Cm::IrIntf::Ret(result->MainObject()));
        }
        if (resultLabel)
        {
            result->SetLabel(resultLabel);
        }
    }
    else
    {
        ExitFunction(*result);
        if (generateDebugInfo)
        {
            if (!currentFunction->Body()->SyntaxNode())
            {
                throw std::runtime_error("current function body has no syntax node");
            }
            Cm::Ast::CompoundStatementNode* compoundStatementNode = static_cast<Cm::Ast::CompoundStatementNode*>(currentFunction->Body()->SyntaxNode());
            CreateExitDebugNode(boundReturnStatement, compoundStatementNode->EndBraceSpan());
            std::shared_ptr<Cm::Core::GenResult> exitResult = resultStack.Pop();
            compoundResult->Merge(exitResult);
        }
        emitter->Emit(Cm::IrIntf::Ret());
        if (resultLabel)
        {
            result->SetLabel(resultLabel);
        }
    }
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundBeginTryStatement& boundBeginTryStatement)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    catchIdStack.push(currentCatchId);
    currentCatchId = boundBeginTryStatement.FirstCatchId();
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundEndTryStatement& boundEndTryStatement) 
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    currentCatchId = catchIdStack.top();
    catchIdStack.pop();
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundExitBlocksStatement& boundExitBlocksStatement)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    Cm::BoundTree::BoundCompoundStatement* currentCompound = boundExitBlocksStatement.CompoundParent();
    Cm::BoundTree::BoundCompoundStatement* targetCompound = boundExitBlocksStatement.TargetBlock();
    ExitCompounds(currentCompound, targetCompound, *result);
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundBeginThrowStatement& boundBeginThrowStatement)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    result->SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
    DoNothing(*result);
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundEndThrowStatement& boundEndThrowStatement)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    result->SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
    DoNothing(*result);
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundConstructionStatement& boundConstructionStatement)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    if (generateDebugInfo)
    {
        if (!boundConstructionStatement.SyntaxNode())
        {
            throw std::runtime_error("syntax node not set");
        }
        Cm::Core::CfgNode* node = CreateDebugNode(boundConstructionStatement, boundConstructionStatement.SyntaxNode()->GetSpan(), true);
        if (!node)
        {
            throw std::runtime_error("node is null");
        }
        int32_t defNode = node->Id();
        Cm::Sym::LocalVariableSymbol* variable = boundConstructionStatement.LocalVariable();
        std::string irName;
        Ir::Intf::Object* irObject = localVariableIrObjectRepository.GetLocalVariableIrObject(variable);
        if (irObject)
        {
            irName = irObject->Name();
        }
        else
        {
            throw std::runtime_error("irObject is null");
        }
        std::string typeName = variable->GetType()->FullName();
        Cm::Core::CFunctionDebugInfo* functionDebugInfo = GetFunctionDebugInfo();
        if (functionDebugInfo)
        {
            functionDebugInfo->Locals().AddLocal(new Cm::Core::Local(variable->Name(), irName, typeName, defNode));
        }
        else
        {
            throw std::runtime_error("functionDebugInfo is null");
        }
    }
    int n = boundConstructionStatement.Arguments().Count();
    Ir::Intf::LabelObject* resultLabel = nullptr;
    Ir::Intf::Object* object = nullptr;
    for (int i = 0; i < n; ++i)
    {
        const std::unique_ptr<Cm::BoundTree::BoundExpression>& arg = boundConstructionStatement.Arguments()[i];
        arg->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> argResult = resultStack.Pop();
        if (i == 0)
        {
            object = argResult->MainObject();
        }
        if (!resultLabel)
        {
            resultLabel = argResult->GetLabel();
        }
        result->Merge(argResult);
    }
    Cm::Sym::FunctionSymbol* ctor = boundConstructionStatement.Constructor();
    Ir::Intf::LabelObject* exprNext = Cm::IrIntf::CreateNextLocalLabel();
    emitter->Own(exprNext);
    emitter->AddNextInstructionLabel(exprNext);
    GenerateCall(ctor, boundConstructionStatement.GetTraceCallInfo(), *result);
    ExecutePostfixIncDecStatements(*result);
    if (resultLabel)
    {
        result->SetLabel(resultLabel);
    }
    Cm::Sym::TypeSymbol* variableType = boundConstructionStatement.LocalVariable()->GetType();
    if (variableType->IsClassTypeSymbol())
    {
        Cm::Sym::ClassTypeSymbol* variableClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(variableType);
        if (variableClassType->Destructor())
        {
            currentCompoundDestructionStack.Push(new Cm::BoundTree::BoundDestructionStatement(nullptr, object, variableClassType->Destructor()));
        }
    }
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundDestructionStatement& boundDestructionStatement)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    result->SetMainObject(boundDestructionStatement.Object());
    Cm::Sym::FunctionSymbol* dtor = boundDestructionStatement.Destructor();
    GenerateCall(dtor, nullptr, *result);
    resultStack.Push(result);
}

void FunctionEmitter::ClearCompoundDestructionStack(Cm::Core::GenResult& result)
{
    bool first = true;
    while (!currentCompoundDestructionStack.IsEmpty())
    {
        std::unique_ptr<Cm::BoundTree::BoundDestructionStatement> boundDestructionStatement = currentCompoundDestructionStack.Pop();
        boundDestructionStatement->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> destructionResult = resultStack.Pop();
        if (first)
        {
            first = false;
            result.BackpatchNextTargets(destructionResult->GetLabel());
            if (!result.LabelSet())
            {
                result.SetLabel(destructionResult->GetLabel());
                result.SetLabelSet();
            }
        }
    }
}

void FunctionEmitter::ExitCompound(Cm::Core::GenResult& result, const CompoundDestructionStack& compoundDestructionStack, bool& first)
{
    int n = int(compoundDestructionStack.DestructionStatements().size());
    for (int i = n - 1; i >= 0; --i)
    {
        const std::unique_ptr<Cm::BoundTree::BoundDestructionStatement>& boundDestructionStatement = compoundDestructionStack.DestructionStatements()[i];
        boundDestructionStatement->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> destructionResult = resultStack.Pop();
        if (first)
        {
            first = false;
            result.BackpatchNextTargets(destructionResult->GetLabel());
            if (!result.LabelSet())
            {
                result.SetLabelSet();
                result.SetLabel(destructionResult->GetLabel());
            }
        }
    }
}

void FunctionEmitter::ExitCompounds(Cm::BoundTree::BoundCompoundStatement* fromCompound, Cm::BoundTree::BoundCompoundStatement* targetCompound, Cm::Core::GenResult& result)
{
    bool first = true;
    const CompoundDestructionStack* compoundDestructionStack = &currentCompoundDestructionStack;
    Cm::BoundTree::BoundCompoundStatement* compound = fromCompound;
    int n = int(functionDestructionStack.CompoundDestructionStacks().size());
    int i = n - 1;
    while (compound && compound != targetCompound)
    {
        ExitCompound(result, *compoundDestructionStack, first);
        compound = compound->CompoundParent();
        if (i < 0)
        {
            throw std::runtime_error("runaway compound destruction stack");
        }
        compoundDestructionStack = &functionDestructionStack.CompoundDestructionStacks()[i];
        --i;
    }
}

void FunctionEmitter::ExitFunction(Cm::Core::GenResult& result)
{
    bool first = true;
    ExitCompound(result,  currentCompoundDestructionStack, first);
    int n = int(functionDestructionStack.CompoundDestructionStacks().size());
    for (int i = n - 1; i >= 0; --i)
    {
        const CompoundDestructionStack& compoundDestructionStack = functionDestructionStack.CompoundDestructionStacks()[i];
        ExitCompound(result, compoundDestructionStack, first);
    }
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundAssignmentStatement& boundAssignmentStatement)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    if (generateDebugInfo)
    {
        if (!boundAssignmentStatement.SyntaxNode())
        {
            throw std::runtime_error("syntax node not set");
        }
        CreateDebugNode(boundAssignmentStatement, boundAssignmentStatement.SyntaxNode()->GetSpan(), true);
    }
    boundAssignmentStatement.Left()->Accept(*this);
    boundAssignmentStatement.Right()->Accept(*this);
    Cm::Sym::FunctionSymbol* assignment = boundAssignmentStatement.Assignment();
    if (!assignment->IsBasicTypeOp() && !assignment->IsArrayAssignment())
    {
        result->SetMainObject(assignment->GetReturnType(), typeRepository);
    }
    std::shared_ptr<Cm::Core::GenResult> sourceResult = resultStack.Pop();
    std::shared_ptr<Cm::Core::GenResult> targetResult = resultStack.Pop(); 
    Ir::Intf::LabelObject* resultLabel = targetResult->GetLabel();
    if (!resultLabel)
    {
        resultLabel = sourceResult->GetLabel();
    }
    result->Merge(targetResult);
    result->Merge(sourceResult);
    Ir::Intf::LabelObject* exprNext = Cm::IrIntf::CreateNextLocalLabel();
    emitter->Own(exprNext);
    emitter->AddNextInstructionLabel(exprNext);
    GenerateCall(assignment, boundAssignmentStatement.GetTraceCallInfo(), *result);
    ExecutePostfixIncDecStatements(*result);
    if (resultLabel)
    {
        result->SetLabel(resultLabel);
    }
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundSimpleStatement& boundSimpleStatement)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    if (generateDebugInfo)
    {
        if (!boundSimpleStatement.SyntaxNode())
        {
            throw std::runtime_error("statement syntax node not set");
        }
        CreateDebugNode(boundSimpleStatement, boundSimpleStatement.SyntaxNode()->GetSpan(), true);
    }
    if (boundSimpleStatement.HasExpression())
    {
        boundSimpleStatement.GetExpression()->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> expressionResult = resultStack.Pop();
        result->SetLabel(expressionResult->GetLabel());
        result->Merge(expressionResult);
        ExecutePostfixIncDecStatements(*result);
    }
    else
    {
        result->SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
        DoNothing(*result);
    }
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundBreakStatement& boundBreakStatement)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    if (generateDebugInfo)
    {
        if (!boundBreakStatement.SyntaxNode())
        {
            throw std::runtime_error("syntax node not set");
        }
        CreateDebugNode(boundBreakStatement, boundBreakStatement.SyntaxNode()->GetSpan(), false);
    }
    Ir::Intf::LabelObject* breakTargetLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter->Own(breakTargetLabel);
    breakTargetStatement->AddBreakTargetLabel(breakTargetLabel);
    ExitCompounds(boundBreakStatement.CompoundParent(), breakTargetStatement->CompoundParent(), *result);
    if (generateDebugInfo)
    {
        breakTargetStatement->AddToBreakNextSet(boundBreakStatement.GetCfgNode());
    }
    Emitter()->UseCDebugNode(boundBreakStatement.GetCfgNode());
    emitter->Emit(Cm::IrIntf::Br(breakTargetLabel));
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundContinueStatement& boundContinueStatement) 
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    if (generateDebugInfo)
    {
        if (!boundContinueStatement.SyntaxNode())
        {
            throw std::runtime_error("syntax node not set");
        }
        CreateDebugNode(boundContinueStatement, boundContinueStatement.SyntaxNode()->GetSpan(), false);
    }
    Ir::Intf::LabelObject* continueTargetLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter->Own(continueTargetLabel);
    continueTargetStatement->AddContinueTargetLabel(continueTargetLabel);
    ExitCompounds(boundContinueStatement.CompoundParent(), continueTargetStatement->CompoundParent(), *result);
    if (generateDebugInfo)
    {
        continueTargetStatement->AddToContinueNextSet(boundContinueStatement.GetCfgNode());
    }
    Emitter()->UseCDebugNode(boundContinueStatement.GetCfgNode());
    emitter->Emit(Cm::IrIntf::Br(continueTargetLabel));
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundGotoStatement& boundGotoStatement)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    if (generateDebugInfo)
    {
        if (!boundGotoStatement.SyntaxNode())
        {
            throw std::runtime_error("syntax node not set");
        }
        CreateDebugNode(boundGotoStatement, boundGotoStatement.SyntaxNode()->GetSpan(), false);
    }
    if (!boundGotoStatement.IsExceptionHandlingGoto())
    {
        ExitCompounds(boundGotoStatement.CompoundParent(), boundGotoStatement.GetTargetCompoundParent(), *result);
    }
    Ir::Intf::LabelObject* gotoTargetLabel = Cm::IrIntf::CreateLabel(boundGotoStatement.TargetLabel());
    emitter->Own(gotoTargetLabel);
    if (generateDebugInfo)
    {
        if (boundGotoStatement.GetTargetStatement()->GetCfgNode())
        {
            AddDebugNodeTransition(boundGotoStatement, *boundGotoStatement.GetTargetStatement());
        }
        else
        {
            boundGotoStatement.GetTargetStatement()->AddToPrevSet(boundGotoStatement.GetCfgNode());
        }
    }
    emitter->Emit(Cm::IrIntf::Br(gotoTargetLabel));
    resultStack.Push(result);
}

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundConditionalStatement& boundConditionalStatement)
{
    PushSkipContent();
    if (generateDebugInfo)
    {
        if (!boundConditionalStatement.Condition()->SyntaxNode())
        {
            throw std::runtime_error("condition syntax node not set");
        }
        CreateDebugNode(boundConditionalStatement, boundConditionalStatement.Condition()->SyntaxNode()->GetSpan(), true);
    }
    functionDestructionStack.Push(std::move(currentCompoundDestructionStack));
    currentCompoundDestructionStack = CompoundDestructionStack();
}

void FunctionEmitter::EndVisit(Cm::BoundTree::BoundConditionalStatement& boundConditionalStatement)
{
    PopSkipContent();
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    std::shared_ptr<Cm::Core::GenResult> conditionResult = resultStack.Pop();
    Ir::Intf::LabelObject* resultLabel = conditionResult->GetLabel();
    int debugNodeSetHandle = -1;
    if (currentCompoundDestructionStack.IsEmpty())
    {
        currentCompoundDestructionStack = functionDestructionStack.Pop();
        Cm::BoundTree::BoundStatement* thenS = boundConditionalStatement.ThenS();
        BeginVisitStatement(*thenS);
        thenS->Accept(*this);
        if (generateDebugInfo)
        {
            AddDebugNodeTransition(boundConditionalStatement, *thenS);
        }
        std::shared_ptr<Cm::Core::GenResult> thenResult = resultStack.Pop();
        conditionResult->BackpatchTrueTargets(thenResult->GetLabel());
        BackpatchNextTargets(thenResult->GetLabel());
        Ir::Intf::LabelObject* next = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(next);
        result->AddNextTarget(next);
        emitter->Emit(Cm::IrIntf::Br(next));
        result->Merge(thenResult);
        Cm::BoundTree::BoundStatement* elseS = boundConditionalStatement.ElseS();
        if (elseS)
        {
            if (generateDebugInfo)
            {
                debugNodeSetHandle = RetrievePrevDebugNodes();
            }
            BeginVisitStatement(*elseS);
            elseS->Accept(*this);
            if (generateDebugInfo)
            {
                AddDebugNodeTransition(boundConditionalStatement, *elseS);
            }
            std::shared_ptr<Cm::Core::GenResult> elseResult = resultStack.Pop();
            conditionResult->BackpatchFalseTargets(elseResult->GetLabel());
            BackpatchNextTargets(elseResult->GetLabel());
            result->Merge(elseResult);
        }
        else
        {
            result->MergeTargets(result->NextTargets(), conditionResult->FalseTargets());
            AddToPrevDebugNodes(boundConditionalStatement);
        }
    }
    else
    {
        bool trueFirst = true;
        std::shared_ptr<Cm::Core::GenResult> trueResult(new Cm::Core::GenResult(emitter.get(), genFlags));
        ExitCompound(*trueResult, currentCompoundDestructionStack, trueFirst);
        CompoundDestructionStack tempCompoundDestructionStack = std::move(currentCompoundDestructionStack);
        currentCompoundDestructionStack = functionDestructionStack.Pop();
        conditionResult->BackpatchTrueTargets(trueResult->GetLabel());
        Ir::Intf::LabelObject* trueLabel = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(trueLabel);
        emitter->Emit(Cm::IrIntf::Br(trueLabel));
        Cm::BoundTree::BoundStatement* thenS = boundConditionalStatement.ThenS();
        BeginVisitStatement(*thenS);
        thenS->Accept(*this);
        if (generateDebugInfo)
        {
            AddDebugNodeTransition(boundConditionalStatement, *thenS);
        }
        std::shared_ptr<Cm::Core::GenResult> thenResult = resultStack.Pop();
        trueLabel->Set(thenResult->GetLabel());
        BackpatchNextTargets(thenResult->GetLabel());
        Ir::Intf::LabelObject* next = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(next);
        result->AddNextTarget(next);
        emitter->Emit(Cm::IrIntf::Br(next));
        result->Merge(thenResult);

        bool falseFirst = true;
        std::shared_ptr<Cm::Core::GenResult> falseResult(new Cm::Core::GenResult(emitter.get(), genFlags));
        ExitCompound(*falseResult, tempCompoundDestructionStack, falseFirst);
        conditionResult->BackpatchFalseTargets(falseResult->GetLabel());
        Ir::Intf::LabelObject* falseLabel = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(falseLabel);
        emitter->Emit(Cm::IrIntf::Br(falseLabel));
        Cm::BoundTree::BoundStatement* elseS = boundConditionalStatement.ElseS();
        if (elseS)
        {
            if (generateDebugInfo)
            {
                debugNodeSetHandle = RetrievePrevDebugNodes();
            }
            BeginVisitStatement(*elseS);
            elseS->Accept(*this);
            if (generateDebugInfo)
            {
                AddDebugNodeTransition(boundConditionalStatement, *elseS);
            }
            std::shared_ptr<Cm::Core::GenResult> elseResult = resultStack.Pop();
            falseLabel->Set(elseResult->GetLabel());
            BackpatchNextTargets(elseResult->GetLabel());
            result->Merge(elseResult);
        }
        else
        {
            result->AddNextTarget(falseLabel);
            AddToPrevDebugNodes(boundConditionalStatement);
        }
    }
    if (resultLabel)
    {
        result->SetLabel(resultLabel);
    }
    if (debugNodeSetHandle != -1)
    {
        AddToPrevDebugNodes(debugNodeSetHandle);
    }
    result->Merge(conditionResult, true);
    resultStack.Push(result);
}

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundWhileStatement& boundWhileStatement)
{
    PushSkipContent();
    if (generateDebugInfo)
    {
        if (!boundWhileStatement.Condition()->SyntaxNode())
        {
            throw std::runtime_error("condition syntax node not set");
        }
        CreateDebugNode(boundWhileStatement, boundWhileStatement.Condition()->SyntaxNode()->GetSpan(), true);
    }
    functionDestructionStack.Push(std::move(currentCompoundDestructionStack));
    currentCompoundDestructionStack = CompoundDestructionStack();
}

void FunctionEmitter::EndVisit(Cm::BoundTree::BoundWhileStatement& boundWhileStatement)
{
    PopSkipContent();
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    std::shared_ptr<Cm::Core::GenResult> conditionResult = resultStack.Pop();
    std::shared_ptr<Cm::Core::GenResult> statementResult;
    if (currentCompoundDestructionStack.IsEmpty())
    {
        currentCompoundDestructionStack = functionDestructionStack.Pop();
        Cm::BoundTree::BoundStatement* statement = boundWhileStatement.Statement();
        PushBreakTargetStatement(&boundWhileStatement);
        PushContinueTargetStatement(&boundWhileStatement);
        BeginVisitStatement(*statement);
        statement->Accept(*this);
        if (generateDebugInfo)
        {
            PatchPrevDebugNodes(boundWhileStatement);
            AddDebugNodeTransition(boundWhileStatement, *statement);
        }
        statementResult = resultStack.Pop();
        emitter->Emit(Cm::IrIntf::Br(conditionResult->GetLabel()));
        conditionResult->BackpatchTrueTargets(statementResult->GetLabel());
        statementResult->BackpatchNextTargets(conditionResult->GetLabel());
        BackpatchNextTargets(statementResult->GetLabel());
        result->MergeTargets(result->NextTargets(), conditionResult->FalseTargets());
        result->MergeTargets(result->NextTargets(), boundWhileStatement.BreakTargetLabels());
        Ir::Intf::Backpatch(boundWhileStatement.ContinueTargetLabels(), conditionResult->GetLabel());
        PopContinueTargetStatement();
        PopBreakTargetStatement();
    }
    else
    {
        bool trueFirst = true;
        std::shared_ptr<Cm::Core::GenResult> trueResult(new Cm::Core::GenResult(emitter.get(), genFlags));
        ExitCompound(*trueResult, currentCompoundDestructionStack, trueFirst);
        CompoundDestructionStack tempCompoundDestructionStack = std::move(currentCompoundDestructionStack);
        currentCompoundDestructionStack = functionDestructionStack.Pop();
        conditionResult->BackpatchTrueTargets(trueResult->GetLabel());
        Ir::Intf::LabelObject* trueLabel = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(trueLabel);
        emitter->Emit(Cm::IrIntf::Br(trueLabel));
        Cm::BoundTree::BoundStatement* statement = boundWhileStatement.Statement();
        PushBreakTargetStatement(&boundWhileStatement);
        PushContinueTargetStatement(&boundWhileStatement);
        BeginVisitStatement(*statement);
        statement->Accept(*this);
        if (generateDebugInfo)
        {
            PatchPrevDebugNodes(boundWhileStatement);
            AddDebugNodeTransition(boundWhileStatement, *statement);
        }
        statementResult = resultStack.Pop();
        emitter->Emit(Cm::IrIntf::Br(conditionResult->GetLabel()));
        trueLabel->Set(statementResult->GetLabel());
        statementResult->BackpatchNextTargets(conditionResult->GetLabel());
        BackpatchNextTargets(statementResult->GetLabel());

        bool falseFirst = true;
        std::shared_ptr<Cm::Core::GenResult> falseResult(new Cm::Core::GenResult(emitter.get(), genFlags));
        ExitCompound(*falseResult, tempCompoundDestructionStack, falseFirst);
        conditionResult->BackpatchFalseTargets(falseResult->GetLabel());
        Ir::Intf::LabelObject* falseLabel = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(falseLabel);
        emitter->Emit(Cm::IrIntf::Br(falseLabel));
        result->AddNextTarget(falseLabel);
        result->MergeTargets(result->NextTargets(), boundWhileStatement.BreakTargetLabels());
        Ir::Intf::Backpatch(boundWhileStatement.ContinueTargetLabels(), conditionResult->GetLabel());
        PopContinueTargetStatement();
        PopBreakTargetStatement();
    }
    if (generateDebugInfo)
    {
        AddToPrevDebugNodes(boundWhileStatement);
        AddToPrevDebugNodes(boundWhileStatement.BreakNextSet());
        PatchDebugNodes(boundWhileStatement.ContinueNextSet(), boundWhileStatement.GetCfgNode());
    }
    result->SetLabel(conditionResult->GetLabel());
    result->Merge(conditionResult, true);
    result->Merge(statementResult);
    resultStack.Push(result);
}

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundDoStatement& boundDoStatement)
{
    PushSkipContent();
}

void FunctionEmitter::EndVisit(Cm::BoundTree::BoundDoStatement& boundDoStatement)
{
    PopSkipContent();
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    Cm::BoundTree::BoundStatement* statement = boundDoStatement.Statement();
    PushBreakTargetStatement(&boundDoStatement);
    PushContinueTargetStatement(&boundDoStatement);
    BeginVisitStatement(*statement);
    statement->Accept(*this);
    if (generateDebugInfo)
    {
        SetCfgNode(*statement, boundDoStatement);
    }
    std::shared_ptr<Cm::Core::GenResult> statementResult = resultStack.Pop();
    Cm::BoundTree::BoundExpression* condition = boundDoStatement.Condition();
    if (generateDebugInfo)
    {
        if (!condition->SyntaxNode())
        {
            throw std::runtime_error("condition syntax node not set");
        }
        CreateDebugNode(*condition, condition->SyntaxNode()->GetSpan());
    }
    if (generateDebugInfo)
    {
        AddDebugNodeTransition(*condition, boundDoStatement);
    }
    functionDestructionStack.Push(std::move(currentCompoundDestructionStack));
    currentCompoundDestructionStack = CompoundDestructionStack();
    condition->Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> conditionResult = resultStack.Pop();
    statementResult->BackpatchNextTargets(conditionResult->GetLabel());
    if (currentCompoundDestructionStack.IsEmpty())
    {
        currentCompoundDestructionStack = functionDestructionStack.Pop();
        conditionResult->BackpatchTrueTargets(statementResult->GetLabel());
        BackpatchNextTargets(statementResult->GetLabel());
        result->MergeTargets(result->NextTargets(), conditionResult->FalseTargets());
        result->MergeTargets(result->NextTargets(), boundDoStatement.BreakTargetLabels());
        Ir::Intf::Backpatch(boundDoStatement.ContinueTargetLabels(), conditionResult->GetLabel());
    }
    else
    {
        bool trueFirst = true;
        std::shared_ptr<Cm::Core::GenResult> trueResult(new Cm::Core::GenResult(emitter.get(), genFlags));
        ExitCompound(*trueResult, currentCompoundDestructionStack, trueFirst);
        CompoundDestructionStack tempCompoundDestructionStack = std::move(currentCompoundDestructionStack);
        currentCompoundDestructionStack = functionDestructionStack.Pop();
        conditionResult->BackpatchTrueTargets(trueResult->GetLabel());
        Ir::Intf::LabelObject* trueLabel = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(trueLabel);
        emitter->Emit(Cm::IrIntf::Br(statementResult->GetLabel()));

        bool falseFirst = true;
        std::shared_ptr<Cm::Core::GenResult> falseResult(new Cm::Core::GenResult(emitter.get(), genFlags));
        ExitCompound(*falseResult, tempCompoundDestructionStack, falseFirst);
        conditionResult->BackpatchFalseTargets(falseResult->GetLabel());
        Ir::Intf::Backpatch(boundDoStatement.BreakTargetLabels(), falseResult->GetLabel());
        Ir::Intf::Backpatch(boundDoStatement.ContinueTargetLabels(), conditionResult->GetLabel());
    }
    if (generateDebugInfo)
    {
        AddToPrevDebugNodes(*condition);
        AddToPrevDebugNodes(boundDoStatement.BreakNextSet());
        PatchDebugNodes(boundDoStatement.ContinueNextSet(), condition->GetCfgNode());
    }
    PopContinueTargetStatement();
    PopBreakTargetStatement();
    result->SetLabel(statementResult->GetLabel());
    result->Merge(statementResult);
    result->Merge(conditionResult);
    resultStack.Push(result);
}

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundForStatement& boundForStatement)
{
    PushSkipContent();
}

void FunctionEmitter::EndVisit(Cm::BoundTree::BoundForStatement& boundForStatement)
{
    PopSkipContent();
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    PushBreakTargetStatement(&boundForStatement);
    PushContinueTargetStatement(&boundForStatement);
    Cm::Ast::Node* node = nullptr;
    Cm::Ast::ForStatementNode* forStatementNode = nullptr;
    if (GenerateDebugInfo())
    {
        node = boundForStatement.SyntaxNode();
        if (!node)
        {
            throw std::runtime_error("syntax node not set");
        }
        if (!node->IsStatementNode())
        {
            throw std::runtime_error("not statement node");
        }
        Cm::Ast::StatementNode* statementNode = static_cast<Cm::Ast::StatementNode*>(node);
        if (!statementNode->IsForStatementNode())
        {
            throw std::runtime_error("not for statement node");
        }
        forStatementNode = static_cast<Cm::Ast::ForStatementNode*>(statementNode);
    }
    bool debugInfoDisabled = false;
    if (GenerateDebugInfo() && forStatementNode && forStatementNode->IsRangeForStatement())
    {
        PushGenDebugInfo(false);
        debugInfoDisabled = true;
    }
    Cm::BoundTree::BoundStatement* initS = boundForStatement.InitS();
    BeginVisitStatement(*initS);
    initS->Accept(*this);
    if (debugInfoDisabled)
    {
        PopGenDebugInfo();
    }
    if (generateDebugInfo)
    {
        if (!debugInfoDisabled)
        {
            SetCfgNode(*initS, boundForStatement);
        }
        if (!boundForStatement.Condition()->SyntaxNode())
        {
            throw std::runtime_error("condition syntax node not set");
        }
        CreateDebugNode(*boundForStatement.Condition(), boundForStatement.Condition()->SyntaxNode()->GetSpan());
    }
    std::shared_ptr<Cm::Core::GenResult> initResult = resultStack.Pop();
    Ir::Intf::LabelObject* initLabel = initResult->GetLabel();
    Cm::BoundTree::BoundExpression* condition = boundForStatement.Condition();
    condition->Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> conditionResult = resultStack.Pop();
    initResult->BackpatchNextTargets(conditionResult->GetLabel());
    BackpatchNextTargets(initLabel);
    Cm::BoundTree::BoundStatement* action = boundForStatement.Action();
    BeginVisitStatement(*action);
    action->Accept(*this);
    if (generateDebugInfo)
    {
        AddDebugNodeTransition(*boundForStatement.Condition(), *action);
    }
    std::shared_ptr<Cm::Core::GenResult> actionResult = resultStack.Pop();
    conditionResult->BackpatchTrueTargets(actionResult->GetLabel());
    BackpatchNextTargets(actionResult->GetLabel());
    result->MergeTargets(result->NextTargets(), conditionResult->FalseTargets());
    Cm::BoundTree::BoundExpression* increment = boundForStatement.Increment();
    if (generateDebugInfo)
    {
        if (!increment->SyntaxNode())
        {
            throw std::runtime_error("increment syntax node not set");
        }
        CreateDebugNode(*increment, increment->SyntaxNode()->GetSpan());
    }
    increment->Accept(*this);
    if (generateDebugInfo)
    {
        AddDebugNodeTransition(*increment, *condition);
    }
    std::shared_ptr<Cm::Core::GenResult> incrementResult = resultStack.Pop();
    actionResult->BackpatchNextTargets(incrementResult->GetLabel());
    incrementResult->BackpatchNextTargets(conditionResult->GetLabel());
    emitter->Emit(Cm::IrIntf::Br(conditionResult->GetLabel()));
    result->MergeTargets(result->NextTargets(), boundForStatement.BreakTargetLabels());
    Ir::Intf::Backpatch(boundForStatement.ContinueTargetLabels(), incrementResult->GetLabel());
    PopContinueTargetStatement();
    PopBreakTargetStatement();
    if (generateDebugInfo)
    {
        AddToPrevDebugNodes(*condition);
        AddToPrevDebugNodes(boundForStatement.BreakNextSet());
        PatchDebugNodes(boundForStatement.ContinueNextSet(), increment->GetCfgNode());
    }
    result->Merge(initResult);
    result->Merge(conditionResult);
    result->Merge(actionResult);
    result->Merge(incrementResult);
    result->SetLabel(initLabel);
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundSwitchStatement& boundSwitchStatement)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    if (generateDebugInfo)
    {
        if (!boundSwitchStatement.Condition()->SyntaxNode())
        {
            throw std::runtime_error("condition syntax node not set");
        }
        CreateDebugNode(boundSwitchStatement, boundSwitchStatement.Condition()->SyntaxNode()->GetSpan(), false);
    }
    PushBreakTargetStatement(&boundSwitchStatement);
    boundSwitchStatement.Condition()->Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> conditionResult = resultStack.Pop();
    Ir::Intf::LabelObject* resultLabel = conditionResult->GetLabel();
    result->Merge(conditionResult);
    std::vector<Ir::Intf::LabelObject*> caseLabels;
    std::unordered_map<std::string, std::pair<Ir::Intf::LabelObject*, Cm::BoundTree::BoundStatement*>> caseConstantMap;
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
        result->AddNextTarget(defaultDest);
        AddToPrevDebugNodes(boundSwitchStatement);
    }
    currentSwitchCaseConstantMap = switchCaseConstantMapStack.top();
    switchCaseConstantMapStack.pop();
    currentSwitchEmitState = switchEmitStateStack.top();
    switchEmitStateStack.pop();
    Ir::Intf::Instruction* switchInst = Cm::IrIntf::Switch(result->MainObject()->GetType(), result->MainObject(), defaultDest, destinations);
    emitter->Emit(switchInst);
    std::vector<int> prevDebugNodeHandles;
    int index = 0;
    for (std::unique_ptr<Cm::BoundTree::BoundStatement>& caseStatement : boundSwitchStatement.CaseStatements())
    {
        switchEmitStateStack.push(currentSwitchEmitState);
        currentSwitchEmitState = SwitchEmitState::emitStatements;
        switchCaseConstantMapStack.push(currentSwitchCaseConstantMap);
        currentSwitchCaseConstantMap = &caseConstantMap;
        switchCaseLabel = caseLabels[index];
        caseStatement->Accept(*this);
        if (generateDebugInfo)
        {
            AddDebugNodeTransition(boundSwitchStatement, *caseStatement);
            prevDebugNodeHandles.push_back(RetrievePrevDebugNodes());
        }
        currentSwitchCaseConstantMap = switchCaseConstantMapStack.top();
        switchCaseConstantMapStack.pop();
        currentSwitchEmitState = switchEmitStateStack.top();
        switchEmitStateStack.pop();
        std::shared_ptr<Cm::Core::GenResult> caseResult = resultStack.Pop();
        result->Merge(caseResult);
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
        if (generateDebugInfo)
        {
            AddDebugNodeTransition(boundSwitchStatement, *boundSwitchStatement.DefaultStatement());
            prevDebugNodeHandles.push_back(RetrievePrevDebugNodes());
        }
        currentSwitchCaseConstantMap = switchCaseConstantMapStack.top();
        switchCaseConstantMapStack.pop();
        currentSwitchEmitState = switchEmitStateStack.top();
        switchEmitStateStack.pop();
        std::shared_ptr<Cm::Core::GenResult> defaultResult = resultStack.Pop();
        result->Merge(defaultResult);
    }
    result->MergeTargets(result->NextTargets(), boundSwitchStatement.BreakTargetLabels());
    if (resultLabel)
    {
        result->SetLabel(resultLabel);
    }
    PopBreakTargetStatement();
    if (generateDebugInfo)
    {
        for (int handle : prevDebugNodeHandles)
        {
            AddToPrevDebugNodes(handle);
        }
        AddToPrevDebugNodes(boundSwitchStatement.BreakNextSet());
    }
    resultStack.Push(result);
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
                Cm::Parsing::Span span(0, 0, 0, 0);
                if (boundCaseStatement.SyntaxNode())
                {
                    span = boundCaseStatement.SyntaxNode()->GetSpan();
                }
                throw Cm::Core::Exception("duplicate case constant '" + caseConstant->Name() + "'", span);
            }
            currentSwitchCaseConstantMap->insert(std::make_pair(caseConstant->Name(), std::make_pair(label, &boundCaseStatement)));
            switchCaseConstants.push_back(caseConstant);
        }
        switchCaseLabel = label;
    }
    else if (currentSwitchEmitState == SwitchEmitState::emitStatements)
    {
        std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
        if (generateDebugInfo)
        {
            if (!boundCaseStatement.SyntaxNode())
            {
                throw std::runtime_error("condition syntax node not set");
            }
            CreateEntryDebugNode(boundCaseStatement, boundCaseStatement.SyntaxNode()->GetSpan());
            std::shared_ptr<Cm::Core::GenResult> entryResult = ResultStack().Pop();
            result->Merge(entryResult);
            result->SetLabel(entryResult->GetLabel());
            switchCaseLabel->Set(entryResult->GetLabel());
        }
        else
        {
            emitter->AddNextInstructionLabel(switchCaseLabel);
        }
        for (std::unique_ptr<Cm::BoundTree::BoundStatement>& statement : boundCaseStatement.Statements())
        {
            statement->Accept(*this);
            std::shared_ptr<Cm::Core::GenResult> statementResult = resultStack.Pop();
            result->Merge(statementResult);
        }
        resultStack.Push(result);
    }
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundDefaultStatement& boundDefaultStatement)
{
    if (currentSwitchEmitState == SwitchEmitState::createSwitchTargets)
    {
        Ir::Intf::LabelObject* label = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(label);
        switchCaseLabel = label;
        currentSwitchCaseConstantMap->insert(std::make_pair("@default", std::make_pair(label, &boundDefaultStatement)));
    }
    else if (currentSwitchEmitState == SwitchEmitState::emitStatements)
    {
        std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
        if (generateDebugInfo)
        {
            if (!boundDefaultStatement.SyntaxNode())
            {
                throw std::runtime_error("condition syntax node not set");
            }
            CreateEntryDebugNode(boundDefaultStatement, boundDefaultStatement.SyntaxNode()->GetSpan());
            std::shared_ptr<Cm::Core::GenResult> entryResult = ResultStack().Pop();
            result->Merge(entryResult);
            result->SetLabel(entryResult->GetLabel());
            switchCaseLabel->Set(entryResult->GetLabel());
        }
        else
        {
            emitter->AddNextInstructionLabel(switchCaseLabel);
        }
        for (std::unique_ptr<Cm::BoundTree::BoundStatement>& statement : boundDefaultStatement.Statements())
        {
            statement->Accept(*this);
            std::shared_ptr<Cm::Core::GenResult> statementResult = resultStack.Pop();
            result->Merge(statementResult);
        }
        resultStack.Push(result);
    }
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundGotoCaseStatement& boundGotoCaseStatement)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    Cm::Sym::Value* value = boundGotoCaseStatement.Value();
    Ir::Intf::Object* caseConstant = value->CreateIrObject();
    emitter->Own(caseConstant);
    SwitchCaseConstantMapIt i = currentSwitchCaseConstantMap->find(caseConstant->Name());
    if (i != currentSwitchCaseConstantMap->end())
    {
        Ir::Intf::LabelObject* target = i->second.first;
        Cm::BoundTree::BoundStatement* targetS = i->second.second;
        ExitCompounds(boundGotoCaseStatement.CompoundParent(), targetS->CompoundParent(), *result);
        if (generateDebugInfo)
        {
            if (!boundGotoCaseStatement.SyntaxNode())
            {
                throw std::runtime_error("syntax node not set");
            }
            CreateDebugNode(boundGotoCaseStatement, boundGotoCaseStatement.SyntaxNode()->GetSpan(), false);
            if (targetS->GetCfgNode())
            {
                AddDebugNodeTransition(boundGotoCaseStatement, *targetS);
            }
            else
            {
                targetS->AddToPrevSet(boundGotoCaseStatement.GetCfgNode());
            }
        }
        emitter->Emit(Cm::IrIntf::Br(target));
    }
    else
    {
        Cm::Parsing::Span span(0, 0, 0, 0);
        if (boundGotoCaseStatement.SyntaxNode())
        {
            span = boundGotoCaseStatement.SyntaxNode()->GetSpan();
        }
        throw Cm::Core::Exception("goto case statement target not found", span);
    }
    resultStack.Push(result);
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundGotoDefaultStatement& boundGotoDefaultStatement)
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    SwitchCaseConstantMapIt i = currentSwitchCaseConstantMap->find("@default");
    if (i != currentSwitchCaseConstantMap->end())
    {
        Ir::Intf::LabelObject* target = i->second.first;
        Cm::BoundTree::BoundStatement* targetS = i->second.second;
        ExitCompounds(boundGotoDefaultStatement.CompoundParent(), targetS->CompoundParent(), *result);
        if (generateDebugInfo)
        {
            if (!boundGotoDefaultStatement.SyntaxNode())
            {
                throw std::runtime_error("syntax node not set");
            }
            CreateDebugNode(boundGotoDefaultStatement, boundGotoDefaultStatement.SyntaxNode()->GetSpan(), false);
            if (targetS->GetCfgNode())
            {
                AddDebugNodeTransition(boundGotoDefaultStatement, *targetS);
            }
            else
            {
                targetS->AddToPrevSet(boundGotoDefaultStatement.GetCfgNode());
            }
        }
        emitter->Emit(Cm::IrIntf::Br(target));
    }
    else
    {
        Cm::Parsing::Span span(0, 0, 0, 0);
        if (boundGotoDefaultStatement.SyntaxNode())
        {
            span = boundGotoDefaultStatement.SyntaxNode()->GetSpan();
        }
        throw Cm::Core::Exception("goto default statement target not found", span);
    }
    resultStack.Push(result);
}

void FunctionEmitter::PushGenDebugInfo(bool generate)
{
    generateDebugInfoStack.push(generateDebugInfo);
    generateDebugInfo = generate;
}

void FunctionEmitter::PopGenDebugInfo()
{
    generateDebugInfo = generateDebugInfoStack.top();
    generateDebugInfoStack.pop(); // pop added
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundPushGenDebugInfoStatement& boundPushGenDebugInfoStatement) 
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    resultStack.Push(result);
    PushGenDebugInfo(boundPushGenDebugInfoStatement.Generate());
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundPopGenDebugInfoStatement& boundPopGenDebugInfoStatement) 
{
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    resultStack.Push(result);
    PopGenDebugInfo();
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
            std::shared_ptr<Cm::Core::GenResult> statementResult = resultStack.Pop();
            result.Merge(statementResult);
        }
        postfixIncDecStatements.clear();
        executingPostfixIncDecStatements = false;
    }
}

inline void CheckRvalueArrayCall(Cm::Core::Emitter& emitter, Cm::Sym::FunctionSymbol* fun, Cm::Core::GenResult& result)
{
    if (fun->GroupName() != "Rvalue") return;
    if (fun->Parameters().size() != 1) return;
    Cm::Sym::ParameterSymbol* param = fun->Parameters()[0];
    if (!param->GetType()->IsArrayType()) return;
    if (result.Objects().size() < 2) return;
    if (Cm::IrIntf::TypesEqual(result.Arg1()->GetType(), param->GetType()->GetIrType())) return;
    Ir::Intf::Object* temp = Cm::IrIntf::CreateTemporaryRegVar(param->GetType()->GetIrType());
    emitter.Own(temp);
    emitter.Emit(Cm::IrIntf::Bitcast(result.Arg1()->GetType(), temp, result.Arg1(), temp->GetType()));
    result.Objects()[1] = temp;
}

void FunctionEmitter::GenerateCall(Cm::Sym::FunctionSymbol* functionSymbol, Ir::Intf::Function* fun, Cm::BoundTree::TraceCallInfo* traceCallInfo, Cm::Core::GenResult& result, 
    bool constructorOrDestructorCall)
{
    if (traceCallInfo)
    {
        CallEnterFrame(traceCallInfo);
    }
    if (constructorOrDestructorCall)
    {
        Cm::Core::GenResult memberFunctionResult(emitter.get(), result.Flags());
        memberFunctionResult.SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
        for (Ir::Intf::Object* object : result.Objects())
        {
            memberFunctionResult.AddObject(object);
        }
        if (functionSymbol && functionSymbol->CanThrow())
        {
            memberFunctionResult.AddObject(localVariableIrObjectRepository.GetExceptionCodeVariable());
        }
        Ir::Intf::Instruction* callInst = Cm::IrIntf::Call(memberFunctionResult.MainObject(), fun, memberFunctionResult.Args());
        if (generateDebugInfo)
        {
            SetCallDebugInfoInfo(callInst, fun);
        }
        emitter->Emit(callInst);
    }
    else
    {
        if (functionSymbol)
        {
            CheckRvalueArrayCall(*emitter, functionSymbol, result);
        }
        if (functionSymbol && functionSymbol->CanThrow())
        {
            Cm::Core::GenResult functionResult(emitter.get(), result.Flags());
            for (Ir::Intf::Object* object : result.Objects())
            {
                functionResult.AddObject(object);
            }
            functionResult.AddObject(localVariableIrObjectRepository.GetExceptionCodeVariable());
            Ir::Intf::Instruction* callInst = Cm::IrIntf::Call(functionResult.MainObject(), fun, functionResult.Args());
            if (generateDebugInfo)
            {
                SetCallDebugInfoInfo(callInst, fun);
            }
            emitter->Emit(callInst);
        }
        else
        {
            Ir::Intf::Instruction* callInst = Cm::IrIntf::Call(result.MainObject(), fun, result.Args());
            if (endProfiledFunLabel)
            {
                callInst->SetLabel(endProfiledFunLabel);
                endProfiledFunLabel = nullptr;
            }
            if (generateDebugInfo)
            {
                SetCallDebugInfoInfo(callInst, fun);
            }
            emitter->Emit(callInst);
        }
    }
    if (traceCallInfo)
    {
        CallLeaveFrame(traceCallInfo);
    }
    if (functionSymbol && functionSymbol->CanThrow())
    {
        GenerateTestExceptionResult();
    }
    if (fun->IsDoNothingFunction())
    {
        externalFunctions.insert(fun);
    }
}

void FunctionEmitter::GenerateVirtualCall(Cm::Sym::FunctionSymbol* fun, Cm::BoundTree::TraceCallInfo* traceCallInfo, Cm::Core::GenResult& result)
{
    if (traceCallInfo)
    {
        CallEnterFrame(traceCallInfo);
    }
    Cm::Core::GenResult memberFunctionResult(emitter.get(), result.Flags());
    if (fun->IsConstructorOrDestructorSymbol())
    {
        memberFunctionResult.SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
    }
    for (Ir::Intf::Object* object : result.Objects())
    {
        memberFunctionResult.AddObject(object);
    }
    if (fun->CanThrow())
    {
        memberFunctionResult.AddObject(localVariableIrObjectRepository.GetExceptionCodeVariable());
    }
    GenVirtualCall(fun, memberFunctionResult);
    if (traceCallInfo)
    {
        CallLeaveFrame(traceCallInfo);
    }
    if (fun->CanThrow())
    {
        GenerateTestExceptionResult();
    }
}

void FunctionEmitter::GenerateInterfaceCall(Cm::Sym::FunctionSymbol* fun, Cm::BoundTree::TraceCallInfo* traceCallInfo, Cm::Core::GenResult& result)
{
    if (traceCallInfo)
    {
        CallEnterFrame(traceCallInfo);
    }
    Cm::Core::GenResult memberFunctionResult(emitter.get(), result.Flags());
    for (Ir::Intf::Object* object : result.Objects())
    {
        memberFunctionResult.AddObject(object);
    }
    if (fun->CanThrow())
    {
        memberFunctionResult.AddObject(localVariableIrObjectRepository.GetExceptionCodeVariable());
    }
    GenInterfaceCall(fun, memberFunctionResult);
    if (traceCallInfo)
    {
        CallLeaveFrame(traceCallInfo);
    }
    if (fun->CanThrow())
    {
        GenerateTestExceptionResult();
    }
}

void FunctionEmitter::GenerateClassDelegateInitFromFun(Cm::Bind::ClassDelegateFromFunCtor* ctor, Cm::Core::GenResult& result)
{
    Cm::Sym::Symbol* objSymbol = ctor->ClassDelegateType()->GetContainerScope()->Lookup("obj");
    if (!objSymbol->IsMemberVariableSymbol())
    {
        throw std::runtime_error("not member variable symbol");
    }
    Cm::Sym::MemberVariableSymbol* objMemberVarSymbol = static_cast<Cm::Sym::MemberVariableSymbol*>(objSymbol);
    std::unique_ptr<Cm::BoundTree::BoundMemberVariable> objMemberVar(new Cm::BoundTree::BoundMemberVariable(nullptr, objMemberVarSymbol));
    Ir::Intf::Object* obj = irObjectRepository.MakeMemberVariableIrObject(objMemberVar.get(), result.MainObject());
    Cm::IrIntf::Init(*emitter, result.Arg1()->GetType(), result.Arg1(), obj);
    Cm::Sym::Symbol* dlgSymbol = ctor->ClassDelegateType()->GetContainerScope()->Lookup("dlg");
    if (!dlgSymbol->IsMemberVariableSymbol())
    {
        throw std::runtime_error("not member variable symbol");
    }
    Cm::Sym::MemberVariableSymbol* dlgMemberVarSymbol = static_cast<Cm::Sym::MemberVariableSymbol*>(dlgSymbol);
    std::unique_ptr<Cm::BoundTree::BoundMemberVariable> dlgMemberVar(new Cm::BoundTree::BoundMemberVariable(nullptr, dlgMemberVarSymbol));
    Ir::Intf::Object* dlg = irObjectRepository.MakeMemberVariableIrObject(dlgMemberVar.get(), result.MainObject());
    Ir::Intf::Type* funPtrIrType = irFunctionRepository.GetFunPtrIrType(ctor->FunctionSymbol());
    Ir::Intf::RegVar* delegatePtr = Cm::IrIntf::CreateTemporaryRegVar(ctor->DelegateType()->GetIrType());
    emitter->Own(delegatePtr);
    emitter->Emit(Cm::IrIntf::Bitcast(funPtrIrType, delegatePtr, result.Arg2(), ctor->DelegateType()->GetIrType()));
    Cm::IrIntf::Init(*emitter, ctor->DelegateType()->GetIrType(), delegatePtr, dlg);
}

void FunctionEmitter::GenerateClassDelegateAssignmentFromFun(Cm::Bind::ClassDelegateFromFunAssignment* assignment, Cm::Core::GenResult& result)
{
    Cm::Sym::Symbol* objSymbol = assignment->ClassDelegateType()->GetContainerScope()->Lookup("obj");
    if (!objSymbol->IsMemberVariableSymbol())
    {
        throw std::runtime_error("not member variable symbol");
    }
    Cm::Sym::MemberVariableSymbol* objMemberVarSymbol = static_cast<Cm::Sym::MemberVariableSymbol*>(objSymbol);
    std::unique_ptr<Cm::BoundTree::BoundMemberVariable> objMemberVar(new Cm::BoundTree::BoundMemberVariable(nullptr, objMemberVarSymbol));
    Ir::Intf::Object* obj = irObjectRepository.MakeMemberVariableIrObject(objMemberVar.get(), result.MainObject());
    Cm::IrIntf::Assign(*emitter, result.Arg1()->GetType(), result.Arg1(), obj);
    Cm::Sym::Symbol* dlgSymbol = assignment->ClassDelegateType()->GetContainerScope()->Lookup("dlg");
    if (!dlgSymbol->IsMemberVariableSymbol())
    {
        throw std::runtime_error("not member variable symbol");
    }
    Cm::Sym::MemberVariableSymbol* dlgMemberVarSymbol = static_cast<Cm::Sym::MemberVariableSymbol*>(dlgSymbol);
    std::unique_ptr<Cm::BoundTree::BoundMemberVariable> dlgMemberVar(new Cm::BoundTree::BoundMemberVariable(nullptr, dlgMemberVarSymbol));
    Ir::Intf::Object* dlg = irObjectRepository.MakeMemberVariableIrObject(dlgMemberVar.get(), result.MainObject());
    Ir::Intf::Type* funPtrIrType = irFunctionRepository.GetFunPtrIrType(assignment->FunctionSymbol());
    Ir::Intf::RegVar* delegatePtr = Cm::IrIntf::CreateTemporaryRegVar(assignment->DelegateType()->GetIrType());
    emitter->Own(delegatePtr);
    emitter->Emit(Cm::IrIntf::Bitcast(funPtrIrType, delegatePtr, result.Arg2(), assignment->DelegateType()->GetIrType()));
    Cm::IrIntf::Assign(*emitter, assignment->DelegateType()->GetIrType(), delegatePtr, dlg);
}

void FunctionEmitter::GenerateCall(Cm::Sym::FunctionSymbol* fun, Cm::BoundTree::TraceCallInfo* traceCallInfo, Cm::Core::GenResult& result)
{
    Cm::Ast::CompileUnitNode* thisUnit = currentFunction->GetFunctionSymbol()->CompileUnit();
    if (thisUnit)
    {
        Cm::Ast::CompileUnitNode* thatUnit = fun->CompileUnit();
        if (thatUnit)
        {
            Cm::BoundTree::BoundCompileUnit* thisBoundUnit = Cm::Core::GetBoundCompileUnit(thisUnit);
            if (thisBoundUnit)
            {
                Cm::BoundTree::BoundCompileUnit* thatBoundUnit = Cm::Core::GetBoundCompileUnit(thatUnit);
                if (thatBoundUnit && thatBoundUnit != thisBoundUnit)
                {
                    thatBoundUnit->AddDependentUnit(thisBoundUnit);
                }
            }
        }
    }
    if (fun->IsClassDelegateFromFunCtor())
    {
        GenerateClassDelegateInitFromFun(static_cast<Cm::Bind::ClassDelegateFromFunCtor*>(fun), result);
    }
    else if (fun->IsClassDelegateFromFunAssignment())
    {
        GenerateClassDelegateAssignmentFromFun(static_cast<Cm::Bind::ClassDelegateFromFunAssignment*>(fun), result);
    }
    else if (fun->IsBasicTypeOp())
    {
        Cm::Core::BasicTypeOp* op = static_cast<Cm::Core::BasicTypeOp*>(fun);
        op->Generate(*emitter, result);
        if (op->IsPrimitiveArrayTypeDefaultConstructor())
        {
            externalFunctions.insert(irFunctionRepository.GetMemSetFunction());
        }
        else if (op->IsPrimitiveArrayTypeCopyConstructorOrCopyAssignment())
        {
            externalFunctions.insert(irFunctionRepository.GetMemCopyFunction());
        }
    }
    else
    {
        if (result.GenerateVirtualCall())
        {
            Ir::Intf::Function* irFunction = irFunctionRepository.CreateIrFunction(fun);
            externalFunctions.insert(irFunction);
            MapIrFunToFun(irFunction, fun);
            GenerateVirtualCall(fun, traceCallInfo, result);
        }
        else if (fun->Parent()->IsInterfaceTypeSymbol())
        {
            GenerateInterfaceCall(fun, traceCallInfo, result);
        }
        else
        {
            Ir::Intf::Function* irFunction = irFunctionRepository.CreateIrFunction(fun);
            externalFunctions.insert(irFunction);
            MapIrFunToFun(irFunction, fun);
            GenerateCall(fun, irFunction, traceCallInfo, result, fun->IsConstructorOrDestructorSymbol());
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

void FunctionEmitter::CallEnterFrame(Cm::BoundTree::TraceCallInfo* traceCallInfo)
{
    if (currentFunction->GetFunctionSymbol()->FullName() == "System.TracedFun.@constructor(System.TracedFun*, const char*, const char*, int)") return;
    if (currentFunction->GetFunctionSymbol()->FullName() == "System.TracedFun.@destructor(System.TracedFun*)") return;
    Ir::Intf::Function* enterFrameIrFun = irFunctionRepository.CreateIrFunction(enterFrameFun);
	externalFunctions.insert(enterFrameIrFun);
    MapIrFunToFun(enterFrameIrFun, enterFrameFun);
	std::vector<Ir::Intf::Object*> args;
    traceCallInfo->Fun()->Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> funResult = resultStack.Pop();
	args.push_back(funResult->MainObject());
    traceCallInfo->File()->Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> fileResult = resultStack.Pop();
	args.push_back(fileResult->MainObject());
    traceCallInfo->Line()->Accept(*this);
    std::shared_ptr<Cm::Core::GenResult> lineResult = resultStack.Pop();
	args.push_back(lineResult->MainObject());
	Ir::Intf::Instruction* callInst = Cm::IrIntf::Call(nullptr, enterFrameIrFun, args);
	emitter->Emit(callInst);
    if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::trace))
    {
        Ir::Intf::Function* enterTracedCallIrFun = irFunctionRepository.CreateIrFunction(enterTracedCallFun);
        externalFunctions.insert(enterTracedCallIrFun);
        MapIrFunToFun(enterTracedCallIrFun, enterTracedCallFun);
        Ir::Intf::Instruction* enterTracedCallInst = Cm::IrIntf::Call(nullptr, enterTracedCallIrFun, args);
        emitter->Emit(enterTracedCallInst);
    }
}

void FunctionEmitter::CallLeaveFrame(Cm::BoundTree::TraceCallInfo* traceCallInfo)
{
    if (currentFunction->GetFunctionSymbol()->FullName() == "System.TracedFun.@constructor(System.TracedFun*, const char*, const char*, int)") return;
    if (currentFunction->GetFunctionSymbol()->FullName() == "System.TracedFun.@destructor(System.TracedFun*)") return;
    Ir::Intf::Function* leaveFrameIrFun = irFunctionRepository.CreateIrFunction(leaveFrameFun);
	std::vector<Ir::Intf::Object*> args;
	externalFunctions.insert(leaveFrameIrFun);
    MapIrFunToFun(leaveFrameIrFun, leaveFrameFun);
	Ir::Intf::Instruction* callInst = Cm::IrIntf::Call(nullptr, leaveFrameIrFun, args);
	emitter->Emit(callInst);
    if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::trace))
    {
        Ir::Intf::Function* leaveTracedCallIrFun = irFunctionRepository.CreateIrFunction(leaveTracedCallFun);
        externalFunctions.insert(leaveTracedCallIrFun);
        MapIrFunToFun(leaveTracedCallIrFun, leaveTracedCallFun);
        std::vector<Ir::Intf::Object*> traceCallArgs;
        traceCallInfo->Fun()->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> funResult = resultStack.Pop();
        traceCallArgs.push_back(funResult->MainObject());
        traceCallInfo->File()->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> fileResult = resultStack.Pop();
        traceCallArgs.push_back(fileResult->MainObject());
        traceCallInfo->Line()->Accept(*this);
        std::shared_ptr<Cm::Core::GenResult> lineResult = resultStack.Pop();
        traceCallArgs.push_back(lineResult->MainObject());

        Ir::Intf::Instruction* leaveTracedCallInst = Cm::IrIntf::Call(nullptr, leaveTracedCallIrFun, traceCallArgs);
        emitter->Emit(leaveTracedCallInst);
    }
}

void FunctionEmitter::GenerateTestExceptionResult()
{
    if (currentFunction->IsMainFunction()) return;  // in real main() exception code is tested explicitly after call to user$main()
    int landingPadId = currentFunction->GetNextLandingPadId();
    Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
    std::string labelPrefix;
    if (backend == Cm::IrIntf::BackEnd::llvm)
    {
        labelPrefix = "$P";
    }
    else if (backend == Cm::IrIntf::BackEnd::c)
    {
        labelPrefix = "_P_";
    }
    Ir::Intf::LabelObject* landingPadLabel = Cm::IrIntf::CreateLabel(labelPrefix + std::to_string(landingPadId));
    emitter->Own(landingPadLabel);
    Ir::Intf::LabelObject* nextLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter->Own(nextLabel);
    Ir::Intf::Object* exCodeVariable = localVariableIrObjectRepository.GetExceptionCodeVariable();
    Ir::Intf::RegVar* exCodeReg = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI32());
    emitter->Own(exCodeReg);
    Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI32(), exCodeVariable, exCodeReg);
    Ir::Intf::RegVar* resultReg = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI1());
    emitter->Own(resultReg);
    Ir::Intf::Object* zero = Ir::Intf::GetFactory()->GetI32()->CreateDefaultValue();
    emitter->Own(zero);
    emitter->Emit(Cm::IrIntf::ICmp(Ir::Intf::GetFactory()->GetI32(), resultReg, Ir::Intf::IConditionCode::ne, exCodeReg, zero));
    emitter->Emit(Cm::IrIntf::Br(resultReg, landingPadLabel, nextLabel));
    std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
    result->SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
    emitter->AddNextInstructionLabel(nextLabel);
    DoNothing(*result);
    CreateLandingPad(landingPadId);
}

void FunctionEmitter::CreateLandingPad(int landingPadId)
{
    Cm::BoundTree::LandingPad* landingPad = new Cm::BoundTree::LandingPad(landingPadId, currentCatchId);
    int n = int(currentCompoundDestructionStack.DestructionStatements().size());
    for (int i = n - 1; i >= 0; --i)
    {
        Cm::BoundTree::BoundDestructionStatement* destructionStatement = currentCompoundDestructionStack.DestructionStatements()[i].get();
        landingPad->AddDestructionStatement(new Cm::BoundTree::BoundDestructionStatement(destructionStatement->SyntaxNode(), destructionStatement->Object(), destructionStatement->Destructor()));
    }
    if (currentCatchId == -1)
    {
        int n = int(functionDestructionStack.CompoundDestructionStacks().size());
        for (int i = n - 1; i >= 0; --i)
        {
            const CompoundDestructionStack& compoundDestructionStack = functionDestructionStack.CompoundDestructionStacks()[i];
            int m = int(compoundDestructionStack.DestructionStatements().size());
            for (int j = m - 1; j >= 0; --j)
            {
                Cm::BoundTree::BoundDestructionStatement* destructionStatement = compoundDestructionStack.DestructionStatements()[j].get();
                landingPad->AddDestructionStatement(new Cm::BoundTree::BoundDestructionStatement(destructionStatement->SyntaxNode(), destructionStatement->Object(), destructionStatement->Destructor()));
            }
        }
    }
    currentFunction->AddLandingPad(landingPad);
}

void FunctionEmitter::GenerateLandingPadCode()
{
    const std::vector<std::unique_ptr<Cm::BoundTree::LandingPad>>& landingPads = currentFunction->GetLandingPads();
    for (const std::unique_ptr<Cm::BoundTree::LandingPad>& landingPad : landingPads)
    {
        Ir::Intf::LabelObject* landingPadLabel = CreateLandingPadLabel(landingPad->Id());
        emitter->Own(landingPadLabel);
        emitter->SetGotoTargetLabel(landingPadLabel);
        std::shared_ptr<Cm::Core::GenResult> result(new Cm::Core::GenResult(emitter.get(), genFlags));
        result->SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
        DoNothing(*result);
        for (const std::unique_ptr<Cm::BoundTree::BoundDestructionStatement>& destructionStatement : landingPad->DestructionStatements())
        {
            destructionStatement->Accept(*this);
        }
        int jumpToCatchId = landingPad->JumpToCatchId();
        if (jumpToCatchId != -1)    // got handler to jump to...
        {
            std::string continueLabelPrefix;
            Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
            if (backend == Cm::IrIntf::BackEnd::llvm)
            {
                continueLabelPrefix = "$C";
            }
            else if (backend == Cm::IrIntf::BackEnd::c)
            {
                continueLabelPrefix = "_C_";
            }
            Ir::Intf::LabelObject* catchIdLabel = Cm::IrIntf::CreateLabel(continueLabelPrefix + std::to_string(jumpToCatchId));
            emitter->Own(catchIdLabel);
            emitter->Emit(Cm::IrIntf::Br(catchIdLabel));
        }
        else    // no handler to jump to...
        {
            if (currentFunction->GetFunctionSymbol()->CanThrow()) // propagate exception to caller
            {
                Ir::Intf::Object* exCodeParam = irFunctionRepository.GetExceptionCodeParam();
                Ir::Intf::Object* exCodeVariable = localVariableIrObjectRepository.GetExceptionCodeVariable(); 
                Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI32(), exCodeVariable, exCodeParam);
            }
            Cm::Sym::TypeSymbol* returnType = currentFunction->GetFunctionSymbol()->GetReturnType();
            if (!returnType || returnType->IsVoidTypeSymbol() || currentFunction->GetFunctionSymbol()->ReturnsClassOrInterfaceObjectByValue())
            {
                emitter->Emit(Cm::IrIntf::Ret());
            }
            else
            {
                Ir::Intf::Object* retval = returnType->GetIrType()->CreateDefaultValue();
                emitter->Own(retval);
                emitter->Emit(Cm::IrIntf::Ret(retval));
            }
        }
    }
}

void FunctionEmitter::EmitStartProfiledFun(const std::string& functionName)
{
    fid = Cm::Sym::FunctionTable::Instance()->GetFid(functionName);
    Cm::Sym::FunctionSymbol* startProfiledFun = symbolTable->GetOverload("start_profiled_fun");
    Cm::BoundTree::BoundExpressionList startProfiledFunArguments;
    Cm::BoundTree::BoundLiteral* fidLiteral = new Cm::BoundTree::BoundLiteral(nullptr);
    fidLiteral->SetValue(new Cm::Sym::UIntValue(fid));
    Cm::Sym::TypeSymbol* uintType = symbolTable->GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::uintId));
    fidLiteral->SetType(uintType);
    startProfiledFunArguments.Add(fidLiteral);
    Cm::BoundTree::BoundFunctionCallStatement callStartProfileFunStatement(startProfiledFun, std::move(startProfiledFunArguments));
    callStartProfileFunStatement.Accept(*this);
}

void FunctionEmitter::EmitEndProfiledFun(Ir::Intf::LabelObject* label)
{
    endProfiledFunLabel = label;
    Cm::Sym::FunctionSymbol* endProfiledFun = symbolTable->GetOverload("end_profiled_fun");
    Cm::BoundTree::BoundExpressionList endProfiledFunArguments;
    Cm::BoundTree::BoundLiteral* fidLiteral = new Cm::BoundTree::BoundLiteral(nullptr);
    fidLiteral->SetValue(new Cm::Sym::UIntValue(fid));
    Cm::Sym::TypeSymbol* uintType = symbolTable->GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::uintId));
    fidLiteral->SetType(uintType);
    endProfiledFunArguments.Add(fidLiteral);
    Cm::BoundTree::BoundFunctionCallStatement callEndProfileFunStatement(endProfiledFun, std::move(endProfiledFunArguments));
    callEndProfileFunStatement.Accept(*this);
}

} } // namespace Cm::Emit
