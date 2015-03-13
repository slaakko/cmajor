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
#include <Llvm.Ir/Type.hpp>

namespace Cm { namespace Emit {

LocalVariableIrObjectRepository::LocalVariableIrObjectRepository() : exceptionCodeVariable(nullptr)
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
            if (type->GetBaseType()->IsVoidTypeSymbol())
            {
                localVariableObject = Cm::IrIntf::CreateRefVar(assemblyName, Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), type->GetPointerCount() + 2));
            }
            else
            {
                localVariableObject = Cm::IrIntf::CreateRefVar(assemblyName, Cm::IrIntf::Pointer(type->GetBaseType()->GetIrType(), type->GetPointerCount() + 2));
            }
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

IrObjectRepository::IrObjectRepository() 
{
}

Ir::Intf::Object* IrObjectRepository::MakeMemberVariableIrObject(Cm::BoundTree::BoundMemberVariable* boundMemberVariable, Ir::Intf::Object* ptr)
{
    Ir::Intf::MemberVar* memberVar = Cm::IrIntf::CreateMemberVar(boundMemberVariable->Symbol()->Name(), ptr, boundMemberVariable->Symbol()->LayoutIndex(), boundMemberVariable->Symbol()->GetType()->GetIrType());
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

FunctionEmitter::FunctionEmitter(Cm::Core::Emitter* emitter_, Cm::Util::CodeFormatter& codeFormatter_, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_,
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::StringRepository& stringRepository_, Cm::BoundTree::BoundClass* currentClass_, 
    std::unordered_set<Ir::Intf::Function*>& externalFunctions_, Cm::Core::StaticMemberVariableRepository& staticMemberVariableRepository_, 
    Cm::Core::ExternalConstantRepository& externalConstantRepository_, Cm::Ast::CompileUnitNode* currentCompileUnit_, Cm::Sym::FunctionSymbol* enterFrameFun_, 
	Cm::Sym::FunctionSymbol* leaveFrameFun_) :
    Cm::BoundTree::Visitor(true), emitter(emitter_), codeFormatter(codeFormatter_), genFlags(Cm::Core::GenFlags::none), typeRepository(typeRepository_), 
    irFunctionRepository(irFunctionRepository_), irClassTypeRepository(irClassTypeRepository_), stringRepository(stringRepository_), compoundResult(), currentCompileUnit(currentCompileUnit_),
    currentClass(currentClass_), currentFunction(nullptr), thisParam(nullptr), externalFunctions(externalFunctions_), staticMemberVariableRepository(staticMemberVariableRepository_), 
    externalConstantRepository(externalConstantRepository_),
    executingPostfixIncDecStatements(false), continueTargetStatement(nullptr), breakTargetStatement(nullptr), currentSwitchEmitState(SwitchEmitState::none), currentSwitchCaseConstantMap(nullptr), 
    switchCaseLabel(nullptr), firstStatementInCompound(false), currentCatchId(-1), enterFrameFun(enterFrameFun_), leaveFrameFun(leaveFrameFun_)
{
}

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
    currentFunction = &boundFunction;
    Cm::IrIntf::ResetLocalLabelCounter();
    Ir::Intf::Function* irFunction = irFunctionRepository.CreateIrFunction(currentFunction->GetFunctionSymbol());
    emitter->SetIrFunction(irFunction);

    irFunction->SetComment(boundFunction.GetFunctionSymbol()->FullName());
    Ir::Intf::Object* exceptionCodeVariable = Cm::IrIntf::CreateStackVar(Cm::IrIntf::GetExCodeVarName(), Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI32(), 1));
    emitter->Own(exceptionCodeVariable);
    emitter->Emit(Cm::IrIntf::Alloca(Ir::Intf::GetFactory()->GetI32(), exceptionCodeVariable));
    localVariableIrObjectRepository.SetExceptionCodeVariable(exceptionCodeVariable);

    int parameterIndex = 0;
    for (Cm::Sym::ParameterSymbol* parameter : boundFunction.GetFunctionSymbol()->Parameters())
    {
        Ir::Intf::Object* localVariableIrObject = localVariableIrObjectRepository.CreateLocalVariableIrObjectFor(parameter);
        emitter->Emit(Cm::IrIntf::Alloca(parameter->GetType()->GetIrType(), localVariableIrObject));
        if (currentFunction->GetFunctionSymbol()->IsMemberFunctionSymbol() && !currentFunction->GetFunctionSymbol()->IsStatic() && parameterIndex == 0)
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

    Ir::Intf::Object* zero = Ir::Intf::GetFactory()->GetI32()->CreateDefaultValue();
    emitter->Own(zero);
    Cm::IrIntf::Init(*emitter, Ir::Intf::GetFactory()->GetI32(), zero, exceptionCodeVariable);
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
        if (!returnType || returnType->IsVoidTypeSymbol() || boundFunction.GetFunctionSymbol()->ReturnsClassObjectByValue())
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
    GenerateLandingPadCode();
    irFunction->Clean();
    Cm::Sym::FunctionSymbol* functionSymbol = boundFunction.GetFunctionSymbol();
    bool weakOdr = functionSymbol->IsReplicated();
    bool inline_ = Cm::Core::GetGlobalFlag(Cm::Core::GlobalFlags::optimize) && functionSymbol->IsInline();
    irFunction->WriteDefinition(codeFormatter, weakOdr, inline_);
    currentFunction = nullptr;
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundLiteral& boundLiteral)
{
    Cm::Core::GenResult result(emitter, genFlags);
    Ir::Intf::Object* literalValue = boundLiteral.GetValue()->CreateIrObject();
    emitter->Own(literalValue);
    result.SetMainObject(literalValue);
    if (boundLiteral.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        GenJumpingBoolCode(result);
    }
    resultStack.Push(std::move(result));
}

bool setCharcterClassCalled = false;

void FunctionEmitter::Visit(Cm::BoundTree::BoundStringLiteral& boundStringLiteral)
{
    Cm::Core::GenResult result(emitter, genFlags);
    result.SetMainObject(boundStringLiteral.GetType(), typeRepository);
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
    Cm::Core::GenResult result(emitter, genFlags);
    if (boundConstant.IsBoundExceptionTableConstant())
    {
        Ir::Intf::Object* exceptionTableConstant = externalConstantRepository.GetExceptionBaseIdTable();
        Cm::Sym::TypeSymbol* type = boundConstant.GetType();
        result.SetMainObject(type, typeRepository);
        result.AddObject(exceptionTableConstant);
        Cm::IrIntf::Init(*emitter, type->GetIrType(), result.Arg1(), result.MainObject());
        resultStack.Push(std::move(result));
        return;
    }
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
    Cm::Core::GenResult result(emitter, genFlags);
    Ir::Intf::Object* enumConstantValue = boundEnumConstant.Symbol()->GetValue()->CreateIrObject();
    emitter->Own(enumConstantValue);
    result.SetMainObject(enumConstantValue);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundLocalVariable& boundLocalVariable)
{
    Cm::Core::GenResult result(emitter, genFlags);
    if (boundLocalVariable.IsBoundExceptionCodeVariable())
    {
        if (boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef) || boundLocalVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue))
        {
            result.SetMainObject(localVariableIrObjectRepository.GetExceptionCodeVariable());
        }
        else
        {
            Cm::Sym::TypeSymbol* type = boundLocalVariable.GetType();
            result.SetMainObject(type, typeRepository);
            result.AddObject(localVariableIrObjectRepository.GetExceptionCodeVariable());
            Cm::IrIntf::Init(*emitter, type->GetIrType(), result.Arg1(), result.MainObject());
        }
        resultStack.Push(std::move(result));
        return;
    }
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
        result.SetMainObject(type, typeRepository);
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
    Cm::Core::GenResult result(emitter, genFlags);
    if (boundParameter.IsBoundExceptionCodeParameter())
    {
        result.SetMainObject(irFunctionRepository.GetExceptionCodeParam());
        resultStack.Push(std::move(result));
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
        result.SetMainObject(localVariableIrObjectRepository.GetLocalVariableIrObject(boundParameter.Symbol()));
    }
    else
    {
        Cm::Sym::TypeSymbol* type = boundParameter.Symbol()->GetType();
        result.SetMainObject(type, typeRepository);
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
    Cm::Core::GenResult result(emitter, genFlags);
    if (boundMemberVariable.Symbol()->IsStatic())
    {
        Ir::Intf::Object* irObject = staticMemberVariableRepository.GetStaticMemberVariableIrObject(boundMemberVariable.Symbol());
        Cm::Sym::TypeSymbol* memberVariableType = boundMemberVariable.Symbol()->GetType();
        if (boundMemberVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue) || boundMemberVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef) || memberVariableType->IsClassTypeSymbol())
        {
            result.SetMainObject(irObject);
        }
        else
        {
            Cm::Sym::TypeSymbol* type = boundMemberVariable.Symbol()->GetType();
            result.SetMainObject(type, typeRepository);
            result.AddObject(irObject);
            Cm::IrIntf::Init(*emitter, type->GetIrType(), result.Arg1(), result.MainObject());
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
        Cm::Core::GenResult ptrResult = resultStack.Pop();
        Cm::Sym::TypeSymbol* type = boundMemberVariable.Symbol()->GetType();
        Ir::Intf::Object* memberVariableIrObject = irObjectRepository.MakeMemberVariableIrObject(&boundMemberVariable, ptrResult.MainObject());
        if (boundMemberVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue) || boundMemberVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef))
        {
            result.SetMainObject(memberVariableIrObject->CreateAddr(*emitter, type->GetIrType()));
        }
        else
        {
            result.SetMainObject(type, typeRepository);
            result.AddObject(memberVariableIrObject);
            Cm::IrIntf::Init(*emitter, type->GetIrType(), result.Arg1(), result.MainObject());
        }
        result.Merge(ptrResult);
    }
    if (boundMemberVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundMemberVariable.GetType()), result);
    }
    if (boundMemberVariable.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        GenJumpingBoolCode(result);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundConversion& boundConversion)
{
    Cm::Core::GenResult result(emitter, genFlags);
    Ir::Intf::Object* temporary = nullptr;
    Cm::BoundTree::BoundExpression* boundTemporary = boundConversion.BoundTemporary();
    if (boundTemporary)
    {
        Cm::Sym::TypeSymbol* temporaryType = boundTemporary->GetType();
        boundTemporary->Accept(*this);
        Cm::Core::GenResult temporaryResult = resultStack.Pop();
        temporary = temporaryResult.MainObject();
        result.Merge(temporaryResult);
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
        result.SetMainObject(boundConversion.GetType(), typeRepository);
    }
    if (boundConversion.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        result.SetGenJumpingBoolCode();
    }
    boundConversion.Operand()->Accept(*this);
    Cm::Core::GenResult operandResult = resultStack.Pop();
    if (boundConversion.Operand()->GetFlag(Cm::BoundTree::BoundNodeFlags::addrArg))
    {
        result.SetAddrArg();
    }
    Ir::Intf::LabelObject* resultLabel = operandResult.GetLabel();
    result.Merge(operandResult);
    Cm::Sym::FunctionSymbol* conversionFun = boundConversion.ConversionFun();
    if (boundConversion.Operand()->GetType()->IsClassTypeSymbol() && (boundConversion.GetType()->IsPointerType() || boundConversion.GetType()->IsReferenceType() || boundConversion.GetType()->IsRvalueRefType()))
    {
        result.SetClassTypeToPointerTypeConversion();
    }
    GenerateCall(conversionFun, nullptr, result);
    if (boundConversion.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundConversion.GetType()), result);
    }
    if (temporary)
    {
        result.SetMainObject(temporary);
    }
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundCast& boundCast)
{
    Cm::Core::GenResult result(emitter, genFlags);
    result.SetMainObject(boundCast.GetType(), typeRepository);
    if (boundCast.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        result.SetGenJumpingBoolCode();
    }
    Cm::Core::GenResult operandResult = resultStack.Pop();
    Ir::Intf::LabelObject* resultLabel = operandResult.GetLabel();
    result.Merge(operandResult);
    Cm::Sym::FunctionSymbol* conversionFun = boundCast.ConversionFun();
    GenerateCall(conversionFun, nullptr, result);
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

void FunctionEmitter::Visit(Cm::BoundTree::BoundSizeOfExpression& boundSizeOfExpr)
{
    Cm::Core::GenResult result(emitter, genFlags);
    result.SetMainObject(Cm::IrIntf::SizeOf(*emitter, boundSizeOfExpr.Type()->GetIrType()));
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundDynamicTypeNameExpression& boundDynamiceTypeNameExpression)
{
    Cm::Core::GenResult result(emitter, genFlags);
    boundDynamiceTypeNameExpression.Subject()->Accept(*this);
    Cm::Core::GenResult subjectResult = resultStack.Pop();
    Ir::Intf::LabelObject* resultLabel = subjectResult.GetLabel();
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    Cm::Sym::ClassTypeSymbol* classType = boundDynamiceTypeNameExpression.ClassType();
    Ir::Intf::Type* classTypePtrType = Cm::IrIntf::Pointer(classType->GetIrType(), 1);
    emitter->Own(classTypePtrType);
    Ir::Intf::Object* objectPtr = subjectResult.MainObject();
    Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 1);
    emitter->Own(i8Ptr);
    Ir::Intf::Type* i8PtrPtr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 2);
    emitter->Own(i8PtrPtr);
    Ir::Intf::Object* vtblPtrContainerPtr = objectPtr;
    int vptrIndex = classType->VPtrIndex();
    if (vptrIndex == -1)
    {
        Cm::Sym::ClassTypeSymbol* vptrContainerClass = classType->VPtrContainerClass();
        vptrIndex = vptrContainerClass->VPtrIndex();
        Ir::Intf::Type* containerPtrType = Cm::IrIntf::Pointer(vptrContainerClass->GetIrType(), 1);
        emitter->Own(containerPtrType);
        Ir::Intf::RegVar* containerPtr = Cm::IrIntf::CreateTemporaryRegVar(containerPtrType);
        emitter->Own(containerPtr);
        emitter->Emit(Cm::IrIntf::Bitcast(classTypePtrType, containerPtr, objectPtr, containerPtrType));
        vtblPtrContainerPtr = containerPtr;
    }
    Ir::Intf::MemberVar* vptr = Cm::IrIntf::CreateMemberVar(Cm::IrIntf::GetVPtrVarName(), vtblPtrContainerPtr, vptrIndex, i8PtrPtr);
    emitter->Own(vptr);
    Ir::Intf::RegVar* loadedVptr = Cm::IrIntf::CreateTemporaryRegVar(i8PtrPtr);
    emitter->Own(loadedVptr);
    Cm::IrIntf::Assign(*emitter, i8PtrPtr, vptr, loadedVptr);
    Ir::Intf::RegVar* typenameI8PtrPtr = Cm::IrIntf::CreateTemporaryRegVar(i8PtrPtr);
    emitter->Own(typenameI8PtrPtr);
    Ir::Intf::Object* zero = Ir::Intf::GetFactory()->GetI16()->CreateDefaultValue();
    emitter->Own(zero);
    emitter->Emit(Cm::IrIntf::GetElementPtr(i8PtrPtr, typenameI8PtrPtr, loadedVptr, zero));
    Ir::Intf::RegVar* loadedTypenameI8Ptr = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
    emitter->Own(loadedTypenameI8Ptr);
    Cm::IrIntf::Assign(*emitter, i8Ptr, typenameI8PtrPtr, loadedTypenameI8Ptr);
    result.SetMainObject(loadedTypenameI8Ptr);
    result.Merge(subjectResult);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundUnaryOp& boundUnaryOp)
{
    Cm::Core::GenResult result(emitter, genFlags);
    Cm::Sym::FunctionSymbol* op = boundUnaryOp.GetFunction();
    bool functionReturnsClassObjectByValue = op->ReturnsClassObjectByValue() && !op->IsBasicTypeOp();
    if (functionReturnsClassObjectByValue)
    {
        result.SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
    }
    else
    {
        result.SetMainObject(op->GetReturnType(), typeRepository);
    }
    Cm::Core::GenResult operandResult = resultStack.Pop();
    if (boundUnaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        result.SetGenJumpingBoolCode();
    }
    if (boundUnaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue) || boundUnaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef))
    {
        result.SetLvalue();
    }
    Ir::Intf::LabelObject* resultLabel = operandResult.GetLabel();
    result.Merge(operandResult);
    Ir::Intf::Object* classObjectResultValue = nullptr;
    if (functionReturnsClassObjectByValue)
    {
        Cm::BoundTree::BoundLocalVariable classObjectResultVar(boundUnaryOp.SyntaxNode(), boundUnaryOp.GetClassObjectResultVar());
        classObjectResultVar.SetType(classObjectResultVar.Symbol()->GetType());
        classObjectResultVar.Accept(*this);
        Cm::Core::GenResult argResult = resultStack.Pop();
        if (!resultLabel)
        {
            resultLabel = argResult.GetLabel();
        }
        classObjectResultValue = argResult.MainObject();
        result.Merge(argResult);
    }
    GenerateCall(op, boundUnaryOp.GetTraceCallInfo(), result);
    if (boundUnaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundUnaryOp.GetType()), result);
    }
    if (functionReturnsClassObjectByValue)
    {
        result.SetMainObject(classObjectResultValue);
    }
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundBinaryOp& boundBinaryOp)
{
    Cm::Core::GenResult result(emitter, genFlags);
    Cm::Core::GenResult right = resultStack.Pop();
    Cm::Core::GenResult left = resultStack.Pop();
    bool functionReturnsClassObjectByValue = boundBinaryOp.GetFunction()->ReturnsClassObjectByValue();
    if (functionReturnsClassObjectByValue)
    {
        result.SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
    }
    else
    {
        result.SetMainObject(boundBinaryOp.GetType(), typeRepository);
    }
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
    Ir::Intf::Object* classObjectResultValue = nullptr;
    if (functionReturnsClassObjectByValue)
    {
        Cm::BoundTree::BoundLocalVariable classObjectResultVar(boundBinaryOp.SyntaxNode(), boundBinaryOp.GetClassObjectResultVar());
        classObjectResultVar.SetType(classObjectResultVar.Symbol()->GetType());
        classObjectResultVar.Accept(*this);
        Cm::Core::GenResult argResult = resultStack.Pop();
        if (!resultLabel)
        {
            resultLabel = argResult.GetLabel();
        }
        classObjectResultValue = argResult.MainObject();
        result.Merge(argResult);
    }
    Cm::Sym::FunctionSymbol* op = boundBinaryOp.GetFunction();
    GenerateCall(op, boundBinaryOp.GetTraceCallInfo(), result);
    if (boundBinaryOp.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(boundBinaryOp.GetType()), result);
    }
    if (functionReturnsClassObjectByValue)
    {
        result.SetMainObject(classObjectResultValue);
    }
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundFunctionCall& functionCall)
{
    Cm::Core::GenResult result(emitter, genFlags);
    bool functionReturnsClassObjectByValue = functionCall.GetFunction()->ReturnsClassObjectByValue();
    if (functionReturnsClassObjectByValue)
    {
        result.SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
    }
    else if (!functionCall.GetFunction()->IsBasicTypeOp() && !functionCall.GetFunction()->IsConstructorOrDestructorSymbol())
    {
        result.SetMainObject(functionCall.GetType(), typeRepository);
    }
    if (functionCall.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        result.SetGenJumpingBoolCode();
    }
    Ir::Intf::LabelObject* resultLabel = nullptr;
    for (std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : functionCall.Arguments())
    {
        argument->Accept(*this);
        Cm::Core::GenResult argResult = resultStack.Pop();
        if (!resultLabel)
        {
            resultLabel = argResult.GetLabel();
        }
        result.Merge(argResult);
    }
    Ir::Intf::Object* classObjectResultValue = nullptr;
    if (functionReturnsClassObjectByValue)
    {
        Cm::BoundTree::BoundLocalVariable classObjectResultVar(functionCall.SyntaxNode(), functionCall.GetClassObjectResultVar());
        classObjectResultVar.SetType(classObjectResultVar.Symbol()->GetType());
        classObjectResultVar.Accept(*this);
        Cm::Core::GenResult argResult = resultStack.Pop();
        if (!resultLabel)
        {
            resultLabel = argResult.GetLabel();
        }
        classObjectResultValue = argResult.MainObject();
        result.Merge(argResult);
    }
    Cm::Sym::FunctionSymbol* fun = functionCall.GetFunction();
    if (functionCall.GetFlag(Cm::BoundTree::BoundNodeFlags::genVirtualCall))
    {
        result.SetGenVirtualCall();
    }
    GenerateCall(fun, functionCall.GetTraceCallInfo(), result);
    if (functionCall.GetFlag(Cm::BoundTree::BoundNodeFlags::refByValue))
    {
        MakePlainValueResult(typeRepository.MakePlainType(functionCall.GetType()), result);
    }
    if (functionReturnsClassObjectByValue)
    {
        result.SetMainObject(classObjectResultValue);
    }
    else if (functionCall.GetTemporary())
    {
        if (functionCall.GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef))
        {
            functionCall.GetTemporary()->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        }
        functionCall.GetTemporary()->Accept(*this);
        Cm::Core::GenResult temporaryResult = resultStack.Pop();
        result.SetMainObject(temporaryResult.MainObject());
    }
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundDisjunction& boundDisjunction)
{
    Cm::Core::GenResult result(emitter, genFlags);
    if (boundDisjunction.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        boundDisjunction.Left()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundDisjunction.Left()->Accept(*this);
        Cm::Core::GenResult leftResult = resultStack.Pop();
        if (!result.LabelSet())
        {
            result.SetLabelSet();
            result.SetLabel(leftResult.GetLabel());
        }

        functionDestructionStack.Push(std::move(currentCompoundDestructionStack));
        currentCompoundDestructionStack = CompoundDestructionStack();
        boundDisjunction.Right()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundDisjunction.Right()->Accept(*this);
        Cm::Core::GenResult rightResult = resultStack.Pop();
        leftResult.BackpatchFalseTargets(rightResult.GetLabel());
        if (currentCompoundDestructionStack.IsEmpty())
        {
            result.MergeTargets(result.FalseTargets(), rightResult.FalseTargets());
            result.MergeTargets(result.TrueTargets(), leftResult.TrueTargets());
            result.MergeTargets(result.TrueTargets(), rightResult.TrueTargets());
        }
        else
        {
            bool trueFirst = true;
            Cm::Core::GenResult trueResult(emitter, genFlags);
            ExitCompound(trueResult, currentCompoundDestructionStack, trueFirst);
            rightResult.BackpatchTrueTargets(trueResult.GetLabel());
            Ir::Intf::LabelObject* trueLabel = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(trueLabel);
            emitter->Emit(Cm::IrIntf::Br(trueLabel));
            result.AddTrueTarget(trueLabel);

            bool falseFirst = true;
            Cm::Core::GenResult falseResult(emitter, genFlags);
            ExitCompound(falseResult, currentCompoundDestructionStack, falseFirst);
            rightResult.BackpatchFalseTargets(falseResult.GetLabel());
            Ir::Intf::LabelObject* falseLabel = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(falseLabel);
            emitter->Emit(Cm::IrIntf::Br(falseLabel));
            result.AddFalseTarget(falseLabel);
            result.MergeTargets(result.TrueTargets(), leftResult.TrueTargets());
            result.Merge(trueResult);
            result.Merge(falseResult);
        }
        currentCompoundDestructionStack = functionDestructionStack.Pop();
        result.Merge(leftResult);
        result.Merge(rightResult);
    }
    else
    {
        boundDisjunction.Left()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundDisjunction.Left()->Accept(*this);
        Cm::Core::GenResult leftResult = resultStack.Pop();
        if (!result.LabelSet())
        {
            result.SetLabelSet();
            result.SetLabel(leftResult.GetLabel());
        }

        functionDestructionStack.Push(std::move(currentCompoundDestructionStack));
        currentCompoundDestructionStack = CompoundDestructionStack();
        boundDisjunction.Right()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundDisjunction.Right()->Accept(*this);
        Cm::Core::GenResult rightResult = resultStack.Pop();
        leftResult.BackpatchFalseTargets(rightResult.GetLabel());
        result.SetMainObject(localVariableIrObjectRepository.GetLocalVariableIrObject(boundDisjunction.GetResultVar()));
        if (currentCompoundDestructionStack.IsEmpty())
        {
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
        }
        else
        {
            bool falseFirst = true;
            Cm::Core::GenResult falseResult(emitter, genFlags);
            ExitCompound(falseResult, currentCompoundDestructionStack, falseFirst);
            rightResult.BackpatchFalseTargets(falseResult.GetLabel());
            Ir::Intf::Object* false_ = Cm::IrIntf::False();
            emitter->Own(false_);
            Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI1(), false_, result.MainObject());
            Ir::Intf::LabelObject* next = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(next);
            emitter->Emit(Cm::IrIntf::Br(next));
            result.AddArgNextTarget(next);

            bool trueFirst = true;
            Cm::Core::GenResult trueResult(emitter, genFlags);
            ExitCompound(trueResult, currentCompoundDestructionStack, trueFirst);
            rightResult.BackpatchTrueTargets(trueResult.GetLabel());
            Ir::Intf::LabelObject* assignTrueLabel = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(assignTrueLabel);
            emitter->AddNextInstructionLabel(assignTrueLabel);
            Ir::Intf::Object* true_ = Cm::IrIntf::True();
            emitter->Own(true_);
            Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI1(), true_, result.MainObject());
            leftResult.BackpatchTrueTargets(assignTrueLabel);

            result.Merge(falseResult);
            result.Merge(trueResult);
        }
        currentCompoundDestructionStack = functionDestructionStack.Pop();
        result.Merge(leftResult);
        result.Merge(rightResult);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundConjunction& boundConjunction)
{
    Cm::Core::GenResult result(emitter, genFlags);
    if (boundConjunction.GetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode))
    {
        boundConjunction.Left()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundConjunction.Left()->Accept(*this);
        Cm::Core::GenResult leftResult = resultStack.Pop();
        if (!result.LabelSet())
        {
            result.SetLabelSet();
            result.SetLabel(leftResult.GetLabel());
        }

        functionDestructionStack.Push(std::move(currentCompoundDestructionStack));
        currentCompoundDestructionStack = CompoundDestructionStack();
        boundConjunction.Right()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundConjunction.Right()->Accept(*this);
        Cm::Core::GenResult rightResult = resultStack.Pop();
        leftResult.BackpatchTrueTargets(rightResult.GetLabel());
        if (currentCompoundDestructionStack.IsEmpty())
        {
            result.MergeTargets(result.TrueTargets(), rightResult.TrueTargets());
            result.MergeTargets(result.FalseTargets(), leftResult.FalseTargets());
            result.MergeTargets(result.FalseTargets(), rightResult.FalseTargets());
        }
        else
        {
            bool trueFirst = true;
            Cm::Core::GenResult trueResult(emitter, genFlags);
            ExitCompound(trueResult, currentCompoundDestructionStack, trueFirst);
            rightResult.BackpatchTrueTargets(trueResult.GetLabel());
            Ir::Intf::LabelObject* trueLabel = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(trueLabel);
            emitter->Emit(Cm::IrIntf::Br(trueLabel));
            result.AddTrueTarget(trueLabel);

            bool falseFirst = true;
            Cm::Core::GenResult falseResult(emitter, genFlags);
            ExitCompound(falseResult, currentCompoundDestructionStack, falseFirst);
            rightResult.BackpatchFalseTargets(falseResult.GetLabel());
            Ir::Intf::LabelObject* falseLabel = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(falseLabel);
            emitter->Emit(Cm::IrIntf::Br(falseLabel));
            result.AddFalseTarget(falseLabel);

            result.MergeTargets(result.FalseTargets(), leftResult.FalseTargets());
            result.Merge(trueResult);
            result.Merge(falseResult);
        }
        currentCompoundDestructionStack = functionDestructionStack.Pop();
        result.Merge(leftResult);
        result.Merge(rightResult);
    }
    else
    {
        boundConjunction.Left()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundConjunction.Left()->Accept(*this);
        Cm::Core::GenResult leftResult = resultStack.Pop();
        if (!result.LabelSet())
        {
            result.SetLabelSet();
            result.SetLabel(leftResult.GetLabel());
        }

        functionDestructionStack.Push(std::move(currentCompoundDestructionStack));
        currentCompoundDestructionStack = CompoundDestructionStack();
        boundConjunction.Right()->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        boundConjunction.Right()->Accept(*this);
        Cm::Core::GenResult rightResult = resultStack.Pop();
        leftResult.BackpatchTrueTargets(rightResult.GetLabel());
        result.SetMainObject(localVariableIrObjectRepository.GetLocalVariableIrObject(boundConjunction.GetResultVar()));
        if (currentCompoundDestructionStack.IsEmpty())
        {
            Ir::Intf::LabelObject* trueLabel = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(trueLabel);
            emitter->AddNextInstructionLabel(trueLabel);
            Ir::Intf::Object* true_ = Cm::IrIntf::True();
            emitter->Own(true_);
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
        }
        else
        {
            bool trueFirst = true;
            Cm::Core::GenResult trueResult(emitter, genFlags);
            ExitCompound(trueResult, currentCompoundDestructionStack, trueFirst);
            rightResult.BackpatchTrueTargets(trueResult.GetLabel());
            Ir::Intf::LabelObject* next = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(next);
            Ir::Intf::Object* true_ = Cm::IrIntf::True();
            emitter->Own(true_);
            Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI1(), true_, result.MainObject());
            emitter->Emit(Cm::IrIntf::Br(next));
            result.AddArgNextTarget(next);

            bool falseFirst = true;
            Cm::Core::GenResult falseResult(emitter, genFlags);
            ExitCompound(falseResult, currentCompoundDestructionStack, falseFirst);
            rightResult.BackpatchFalseTargets(falseResult.GetLabel());
            Ir::Intf::LabelObject* assignFalseLabel = Cm::IrIntf::CreateNextLocalLabel();
            emitter->Own(assignFalseLabel);
            emitter->AddNextInstructionLabel(assignFalseLabel);
            Ir::Intf::Object* false_ = Cm::IrIntf::False();
            emitter->Own(false_);
            Cm::IrIntf::Assign(*emitter, Ir::Intf::GetFactory()->GetI1(), false_, result.MainObject());
            leftResult.BackpatchFalseTargets(assignFalseLabel);
            result.Merge(trueResult);
            result.Merge(falseResult);
        }
        currentCompoundDestructionStack = functionDestructionStack.Pop();
        result.Merge(leftResult);
        result.Merge(rightResult);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundPostfixIncDecExpr& boundPostfixIncDecExpr)
{
    Cm::Core::GenResult result(emitter, genFlags);
    boundPostfixIncDecExpr.Value()->Accept(*this);
    Cm::Core::GenResult valueResult = resultStack.Pop();
    Ir::Intf::LabelObject* resultLabel = valueResult.GetLabel();
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
    if (!statement.Label().empty())
    {
        Ir::Intf::LabelObject* label = Cm::IrIntf::CreateLabel(statement.Label());
        emitter->Own(label);
        emitter->SetGotoTargetLabel(label);
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
    if (!compoundResult.LabelSet())
    {
        if (statementResult.GetLabel())
        {
            compoundResult.SetLabel(statementResult.GetLabel());
            compoundResult.SetLabelSet();
        }
    }
}

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundCompoundStatement& boundCompoundStatement)
{
    compoundResultStack.Push(std::move(compoundResult));
    compoundResult = Cm::Core::GenResult(emitter, Cm::Core::GenFlags::none);
    if (boundCompoundStatement.IsEmpty())
    {
        compoundResult.SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
        GenerateCall(nullptr, irFunctionRepository.GetDoNothingFunction(), nullptr, compoundResult, false);
    }
    functionDestructionStack.Push(std::move(currentCompoundDestructionStack));
    currentCompoundDestructionStack = CompoundDestructionStack();
}

void FunctionEmitter::EndVisit(Cm::BoundTree::BoundCompoundStatement& boundCompoundStatement)
{
    ClearCompoundDestructionStack(compoundResult);
    currentCompoundDestructionStack = functionDestructionStack.Pop();
    resultStack.Push(std::move(compoundResult));
    compoundResult = std::move(compoundResultStack.Pop());
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundReceiveStatement& boundReceiveStatement)
{
    Cm::Sym::ParameterSymbol* parameterSymbol = boundReceiveStatement.GetParameterSymbol();
    Cm::Core::GenResult result(emitter, genFlags);
    result.SetMainObject(localVariableIrObjectRepository.GetLocalVariableIrObject(parameterSymbol));
    Ir::Intf::Parameter* irParameter = Cm::Core::CreateIrParameter(parameterSymbol);
    emitter->Own(irParameter);
    result.AddObject(irParameter);
    Cm::Sym::FunctionSymbol* ctor = boundReceiveStatement.Constructor();
    GenerateCall(ctor, nullptr, result);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundInitClassObjectStatement& boundInitClassObjectStatement)
{
    Cm::Core::GenResult result(emitter, genFlags);
    boundInitClassObjectStatement.FunctionCall()->Accept(*this);
    Cm::Core::GenResult callResult = resultStack.Pop();
    result.Merge(callResult);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundInitVPtrStatement& boundInitVPtrStatement)
{
    Cm::Core::GenResult result(emitter, genFlags);
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

void FunctionEmitter::RegisterDestructor(Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol)
{
    Ir::Intf::Object* irObject = staticMemberVariableRepository.GetStaticMemberVariableIrObject(staticMemberVariableSymbol);
    Ir::Intf::Object* destructionNode = staticMemberVariableRepository.GetDestructionNode(staticMemberVariableSymbol);
    Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 1);
    emitter->Own(i8Ptr);
    Ir::Intf::Type* i8PtrPtr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 2);
    emitter->Own(i8PtrPtr);
    Ir::Intf::RegVar* objectFieldPtr = Cm::IrIntf::CreateTemporaryRegVar(i8PtrPtr);
    emitter->Own(objectFieldPtr);
    Ir::Intf::Object* zero = Cm::IrIntf::CreateI32Constant(0);
    emitter->Own(zero);
    Ir::Intf::Object* one = Cm::IrIntf::CreateI32Constant(1);
    emitter->Own(one);
    emitter->Emit(Cm::IrIntf::GetElementPtr(destructionNode->GetType(), objectFieldPtr, destructionNode, zero, one));
    Ir::Intf::Object* irObjectAsI8Ptr = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
    emitter->Own(irObjectAsI8Ptr);
    emitter->Emit(Cm::IrIntf::Bitcast(irObject->GetType(), irObjectAsI8Ptr, irObject, i8Ptr));
    emitter->Emit(Cm::IrIntf::Store(i8Ptr, irObjectAsI8Ptr, objectFieldPtr));

    Cm::Sym::TypeSymbol* type = staticMemberVariableSymbol->GetType();
    if (type->IsClassTypeSymbol())
    {
        Cm::Sym::ClassTypeSymbol* classType = static_cast<Cm::Sym::ClassTypeSymbol*>(type);
        if (classType->Destructor())
        {
            Cm::Sym::FunctionSymbol* destructor = classType->Destructor();
            Ir::Intf::Function* destructorIrFun = irFunctionRepository.CreateIrFunction(destructor);
            Ir::Intf::Type* destructorPtrType = irFunctionRepository.GetFunPtrIrType(destructor);
            std::vector<Ir::Intf::Type*> dtorParamTypes1(1, i8Ptr->Clone());
            Ir::Intf::Type* destructorFieldType = Cm::IrIntf::Pointer(Cm::IrIntf::CreateFunctionType(Cm::IrIntf::Void(), dtorParamTypes1), 1);
            emitter->Own(destructorFieldType);
            std::vector<Ir::Intf::Type*> dtorParamTypes2(1, i8Ptr->Clone());
            Ir::Intf::Type* destructorFieldPtrType = Cm::IrIntf::Pointer(Cm::IrIntf::CreateFunctionType(Cm::IrIntf::Void(), dtorParamTypes2), 2);
            emitter->Own(destructorFieldPtrType);
            Ir::Intf::RegVar* destructorFieldPtr = Cm::IrIntf::CreateTemporaryRegVar(destructorFieldPtrType);
            emitter->Own(destructorFieldPtr);
            Ir::Intf::Object* two = Cm::IrIntf::CreateI32Constant(2);
            emitter->Own(two);
            emitter->Emit(Cm::IrIntf::GetElementPtr(destructionNode->GetType(), destructorFieldPtr, destructionNode, zero, two));
            Ir::Intf::Global* dtor(Cm::IrIntf::CreateGlobal(destructorIrFun->Name(), destructorPtrType));
            emitter->Own(dtor);
            Ir::Intf::RegVar* dtorPtr = Cm::IrIntf::CreateTemporaryRegVar(destructorFieldType);
            emitter->Own(dtorPtr);
            emitter->Emit(Cm::IrIntf::Bitcast(destructorPtrType, dtorPtr, dtor, destructorFieldType));
            emitter->Emit(Cm::IrIntf::Store(destructorFieldType, dtorPtr, destructorFieldPtr));

            std::vector<Ir::Intf::Parameter*> registerFunParams;
            Ir::Intf::Parameter* param = Cm::IrIntf::CreateParameter("node", destructionNode->GetType()->Clone());
            emitter->Own(param);
            registerFunParams.push_back(param);
            Ir::Intf::Function* registerFun = Cm::IrIntf::CreateFunction(Cm::IrIntf::GetRegisterDestructorFunctionName(), Ir::Intf::GetFactory()->GetVoid(), registerFunParams);
            emitter->Own(registerFun);
            std::vector<Ir::Intf::Object*> registerFunArgs;
            registerFunArgs.push_back(destructionNode);
            Ir::Intf::RegVar* result = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetVoid());
            emitter->Own(result);
            emitter->Emit(Cm::IrIntf::Call(result, registerFun, registerFunArgs));
        }
    }
    
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundInitMemberVariableStatement& boundInitMemberVariableStatement)
{
    Cm::Core::GenResult result(emitter, genFlags);
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
    GenerateCall(ctor, nullptr, result);
    if (boundInitMemberVariableStatement.RegisterDestructor())
    {
        RegisterDestructor(boundInitMemberVariableStatement.GetMemberVariableSymbol());
    }
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundFunctionCallStatement& boundFunctionCallStatement) 
{
    Cm::Core::GenResult result(emitter, genFlags);
    Cm::Sym::FunctionSymbol* function = boundFunctionCallStatement.Function();
    if (!function->IsBasicTypeOp() && !function->IsConstructorOrDestructorSymbol())
    {
        result.SetMainObject(function->GetReturnType(), typeRepository);
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
    GenerateCall(function, nullptr, result);
    ExecutePostfixIncDecStatements(result);
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundReturnStatement& boundReturnStatement) 
{
    Cm::Core::GenResult result(emitter, genFlags);
    if (boundReturnStatement.ReturnsValue())
    {
        Cm::Sym::FunctionSymbol* ctor = boundReturnStatement.Constructor();
        bool returnsClassObjectByValue = currentFunction->GetFunctionSymbol()->ReturnsClassObjectByValue();
        if (returnsClassObjectByValue)
        {
            result.SetMainObject(currentFunction->GetFunctionSymbol()->ClassObjectResultIrParam());
        }
        else
        {
            result.SetMainObject(boundReturnStatement.GetReturnType(), typeRepository);
        }
        Cm::Core::GenResult retValResult = resultStack.Pop();
        Ir::Intf::LabelObject* resultLabel = retValResult.GetLabel();
        result.Merge(retValResult);
        Ir::Intf::LabelObject* exprNext = Cm::IrIntf::CreateNextLocalLabel();
        emitter->Own(exprNext);
        emitter->AddNextInstructionLabel(exprNext);
        if (!resultLabel)
        {
            resultLabel = exprNext;
        }
        GenerateCall(ctor, boundReturnStatement.GetTraceCallInfo(), result);
        result.BackpatchArgNextTargets(exprNext);
        ExecutePostfixIncDecStatements(result);
        ExitFunction(result);
        if (returnsClassObjectByValue)
        {
            emitter->Emit(Cm::IrIntf::Ret());
        }
        else
        {
            emitter->Emit(Cm::IrIntf::Ret(result.MainObject()));
        }
        if (resultLabel)
        {
            result.SetLabel(resultLabel);
        }
    }
    else
    {
        ExitFunction(result);
        emitter->Emit(Cm::IrIntf::Ret());
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundBeginTryStatement& boundBeginTryStatement)
{
    Cm::Core::GenResult result(emitter, genFlags);
    catchIdStack.push(currentCatchId);
    currentCatchId = boundBeginTryStatement.FirstCatchId();
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundEndTryStatement& boundEndTryStatement) 
{
    Cm::Core::GenResult result(emitter, genFlags);
    currentCatchId = catchIdStack.top();
    catchIdStack.pop();
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundExitBlocksStatement& boundExitBlocksStatement)
{
    Cm::Core::GenResult result(emitter, genFlags);
    Cm::BoundTree::BoundCompoundStatement* currentCompound = boundExitBlocksStatement.CompoundParent();
    Cm::BoundTree::BoundCompoundStatement* targetCompound = boundExitBlocksStatement.TargetBlock();
    ExitCompounds(currentCompound, targetCompound, result);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundConstructionStatement& boundConstructionStatement)
{
    Cm::Core::GenResult result(emitter, genFlags);
    int n = boundConstructionStatement.Arguments().Count();
    Ir::Intf::LabelObject* resultLabel = nullptr;
    Ir::Intf::Object* object = nullptr;
    for (int i = 0; i < n; ++i)
    {
        const std::unique_ptr<Cm::BoundTree::BoundExpression>& arg = boundConstructionStatement.Arguments()[i];
        arg->Accept(*this);
        Cm::Core::GenResult argResult = resultStack.Pop();
        if (i == 0)
        {
            object = argResult.MainObject();
        }
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
    GenerateCall(ctor, boundConstructionStatement.GetTraceCallInfo(), result);
    result.BackpatchArgNextTargets(exprNext);
    ExecutePostfixIncDecStatements(result);
    if (resultLabel)
    {
        result.SetLabel(resultLabel);
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
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundDestructionStatement& boundDestructionStatement)
{
    Cm::Core::GenResult result(emitter, genFlags);
    result.SetMainObject(boundDestructionStatement.Object());
    Cm::Sym::FunctionSymbol* dtor = boundDestructionStatement.Destructor();
    GenerateCall(dtor, nullptr, result);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::ClearCompoundDestructionStack(Cm::Core::GenResult& result)
{
    bool first = true;
    while (!currentCompoundDestructionStack.IsEmpty())
    {
        std::unique_ptr<Cm::BoundTree::BoundDestructionStatement> boundDestructionStatement = currentCompoundDestructionStack.Pop();
        boundDestructionStatement->Accept(*this);
        Cm::Core::GenResult destructionResult = resultStack.Pop();
        if (first)
        {
            first = false;
            result.BackpatchNextTargets(destructionResult.GetLabel());
            if (!result.LabelSet())
            {
                result.SetLabel(destructionResult.GetLabel());
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
        Cm::Core::GenResult destructionResult = resultStack.Pop();
        if (first)
        {
            first = false;
            result.BackpatchNextTargets(destructionResult.GetLabel());
            if (!result.LabelSet())
            {
                result.SetLabelSet();
                result.SetLabel(destructionResult.GetLabel());
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
    Cm::Core::GenResult result(emitter, genFlags);
    Cm::Sym::FunctionSymbol* assignment = boundAssignmentStatement.Assignment();
    if (!assignment->IsBasicTypeOp())
    {
        result.SetMainObject(assignment->GetReturnType(), typeRepository);
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
    GenerateCall(assignment, boundAssignmentStatement.GetTraceCallInfo(), result);
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
    Cm::Core::GenResult result(emitter, genFlags);
    if (boundSimpleStatement.HasExpression())
    {
        Cm::Core::GenResult expressionResult = resultStack.Pop();
        result.SetLabel(expressionResult.GetLabel());
        result.Merge(expressionResult);
        ExecutePostfixIncDecStatements(result);
    }
    else
    {
        result.SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
        GenerateCall(nullptr, irFunctionRepository.GetDoNothingFunction(), nullptr, result, false);
    }
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundBreakStatement& boundBreakStatement)
{
    Cm::Core::GenResult result(emitter, genFlags);
    Ir::Intf::LabelObject* breakTargetLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter->Own(breakTargetLabel);
    breakTargetStatement->AddBreakTargetLabel(breakTargetLabel);
    ExitCompounds(boundBreakStatement.CompoundParent(), breakTargetStatement->CompoundParent(), result);
    emitter->Emit(Cm::IrIntf::Br(breakTargetLabel));
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundContinueStatement& boundContinueStatement) 
{
    Cm::Core::GenResult result(emitter, genFlags);
    Ir::Intf::LabelObject* continueTargetLabel = Cm::IrIntf::CreateNextLocalLabel();
    emitter->Own(continueTargetLabel);
    continueTargetStatement->AddContinueTargetLabel(continueTargetLabel);
    ExitCompounds(boundContinueStatement.CompoundParent(), continueTargetStatement->CompoundParent(), result);
    emitter->Emit(Cm::IrIntf::Br(continueTargetLabel));
    resultStack.Push(std::move(result));
}

void FunctionEmitter::Visit(Cm::BoundTree::BoundGotoStatement& boundGotoStatement)
{
    Cm::Core::GenResult result(emitter, genFlags);
    if (!boundGotoStatement.IsExceptionHandlingGoto())
    {
        ExitCompounds(boundGotoStatement.CompoundParent(), boundGotoStatement.GetTargetCompoundParent(), result);
    }
    Ir::Intf::LabelObject* gotoTargetLabel = Cm::IrIntf::CreateLabel(boundGotoStatement.TargetLabel());
    emitter->Own(gotoTargetLabel);
    emitter->Emit(Cm::IrIntf::Br(gotoTargetLabel));
    resultStack.Push(std::move(result));
}

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundConditionalStatement& boundConditionalStatement)
{
    PushSkipContent();
}

void FunctionEmitter::EndVisit(Cm::BoundTree::BoundConditionalStatement& boundConditionalStatement)
{
    PopSkipContent();
    Cm::Core::GenResult result(emitter, genFlags);
    Cm::Core::GenResult conditionResult = resultStack.Pop();
    Ir::Intf::LabelObject* resultLabel = conditionResult.GetLabel();
    Cm::BoundTree::BoundStatement* thenS = boundConditionalStatement.ThenS();
    BeginVisitStatement(*thenS);
    thenS->Accept(*this);
    Cm::Core::GenResult thenResult = resultStack.Pop();
    conditionResult.BackpatchTrueTargets(thenResult.GetLabel());
    Ir::Intf::LabelObject* next = Cm::IrIntf::CreateNextLocalLabel();
    emitter->Own(next);
    result.AddNextTarget(next);
    emitter->Emit(Cm::IrIntf::Br(next));
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
    result.Merge(conditionResult, true);
    resultStack.Push(std::move(result));
}

void FunctionEmitter::BeginVisit(Cm::BoundTree::BoundWhileStatement& boundWhileStatement)
{
    PushSkipContent();
}

void FunctionEmitter::EndVisit(Cm::BoundTree::BoundWhileStatement& boundWhileStatement)
{
    PopSkipContent();
    Cm::Core::GenResult result(emitter, genFlags);
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
    result.Merge(conditionResult, true);
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
    Cm::Core::GenResult result(emitter, genFlags);
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
    Cm::Core::GenResult result(emitter, genFlags);
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
    Cm::Core::GenResult result(emitter, genFlags);
    PushBreakTargetStatement(&boundSwitchStatement);
    boundSwitchStatement.Condition()->Accept(*this);
    Cm::Core::GenResult conditionResult = resultStack.Pop();
    Ir::Intf::LabelObject* resultLabel = conditionResult.GetLabel();
    result.Merge(conditionResult);
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
            currentSwitchCaseConstantMap->insert(std::make_pair(caseConstant->Name(), std::make_pair(label, &boundCaseStatement)));
            switchCaseConstants.push_back(caseConstant);
        }
        switchCaseLabel = label;
    }
    else if (currentSwitchEmitState == SwitchEmitState::emitStatements)
    {
        Cm::Core::GenResult result(emitter, genFlags);
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
        currentSwitchCaseConstantMap->insert(std::make_pair("@default", std::make_pair(label, &boundDefaultStatement)));
    }
    else if (currentSwitchEmitState == SwitchEmitState::emitStatements)
    {
        Cm::Core::GenResult result(emitter, genFlags);
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
    Cm::Core::GenResult result(emitter, genFlags);
    Cm::Sym::Value* value = boundGotoCaseStatement.Value();
    Ir::Intf::Object* caseConstant = value->CreateIrObject();
    emitter->Own(caseConstant);
    SwitchCaseConstantMapIt i = currentSwitchCaseConstantMap->find(caseConstant->Name());
    if (i != currentSwitchCaseConstantMap->end())
    {
        Ir::Intf::LabelObject* target = i->second.first;
        Cm::BoundTree::BoundStatement* targetS = i->second.second;
        ExitCompounds(boundGotoCaseStatement.CompoundParent(), targetS->CompoundParent(), result);
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
    Cm::Core::GenResult result(emitter, genFlags);
    SwitchCaseConstantMapIt i = currentSwitchCaseConstantMap->find("@default");
    if (i != currentSwitchCaseConstantMap->end())
    {
        Ir::Intf::LabelObject* target = i->second.first;
        Cm::BoundTree::BoundStatement* targetS = i->second.second;
        ExitCompounds(boundGotoDefaultStatement.CompoundParent(), targetS->CompoundParent(), result);
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

void FunctionEmitter::GenerateCall(Cm::Sym::FunctionSymbol* functionSymbol, Ir::Intf::Function* fun, Cm::BoundTree::TraceCallInfo* traceCallInfo, Cm::Core::GenResult& result, 
    bool constructorOrDestructorCall)
{
    if (traceCallInfo)
    {
        CallEnterFrame(traceCallInfo);
    }
    if (constructorOrDestructorCall)
    {
        Cm::Core::GenResult memberFunctionResult(emitter, result.Flags());
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
        emitter->Emit(callInst);
    }
    else
    {
        if (functionSymbol && functionSymbol->CanThrow())
        {
            Cm::Core::GenResult functionResult(emitter, result.Flags());
            for (Ir::Intf::Object* object : result.Objects())
            {
                functionResult.AddObject(object);
            }
            functionResult.AddObject(localVariableIrObjectRepository.GetExceptionCodeVariable());
            Ir::Intf::Instruction* callInst = Cm::IrIntf::Call(functionResult.MainObject(), fun, functionResult.Args());
            emitter->Emit(callInst);
        }
        else
        {
            Ir::Intf::Instruction* callInst = Cm::IrIntf::Call(result.MainObject(), fun, result.Args());
            emitter->Emit(callInst);
        }
    }
    if (traceCallInfo)
    {
        CallLeaveFrame();
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
    Cm::Core::GenResult memberFunctionResult(emitter, result.Flags());
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
    if (traceCallInfo)
    {
        CallLeaveFrame();
    }
    if (fun->CanThrow())
    {
        GenerateTestExceptionResult();
    }
}

void FunctionEmitter::GenerateCall(Cm::Sym::FunctionSymbol* fun, Cm::BoundTree::TraceCallInfo* traceCallInfo, Cm::Core::GenResult& result)
{
    if (fun->IsBasicTypeOp())
    {
        Cm::Core::BasicTypeOp* op = static_cast<Cm::Core::BasicTypeOp*>(fun);
        op->Generate(*emitter, result);
    }
    else
    {
		if (fun->FullName().find("SetCharacterClass") != std::string::npos)
		{
			setCharcterClassCalled = true;
		}
        if (result.GenerateVirtualCall())
        {
            GenerateVirtualCall(fun, traceCallInfo, result);
        }
        else
        {
            Ir::Intf::Function* irFunction = irFunctionRepository.CreateIrFunction(fun);
            if (fun->CompileUnit() != currentCompileUnit && !fun->IsReplicated())
            {
                externalFunctions.insert(irFunction);
            }
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
	return;	// todo
	Ir::Intf::Function* enterFrameIrFun = irFunctionRepository.CreateIrFunction(enterFrameFun);
	externalFunctions.insert(enterFrameIrFun);
	std::vector<Ir::Intf::Object*> args;
    traceCallInfo->Fun()->Accept(*this);
    Cm::Core::GenResult funResult = resultStack.Pop();
	args.push_back(funResult.MainObject());
    traceCallInfo->File()->Accept(*this);
    Cm::Core::GenResult fileResult = resultStack.Pop();
	args.push_back(fileResult.MainObject());
    traceCallInfo->Line()->Accept(*this);
    Cm::Core::GenResult lineResult = resultStack.Pop();
	args.push_back(lineResult.MainObject());
	Ir::Intf::Instruction* callInst = Cm::IrIntf::Call(nullptr, enterFrameIrFun, args);
	emitter->Emit(callInst);
}

void FunctionEmitter::CallLeaveFrame()
{
	return;	// todo
	Ir::Intf::Function* leaveFrameIrFun = irFunctionRepository.CreateIrFunction(leaveFrameFun);
	std::vector<Ir::Intf::Object*> args;
	externalFunctions.insert(leaveFrameIrFun);
	Ir::Intf::Instruction* callInst = Cm::IrIntf::Call(nullptr, leaveFrameIrFun, args);
	emitter->Emit(callInst);
}

void FunctionEmitter::GenerateTestExceptionResult()
{
    if (currentFunction->IsMainFunction()) return;  // in real main() exception code is tested explicitly after call to user$main()
    int landingPadId = currentFunction->GetNextLandingPadId();
    Ir::Intf::LabelObject* landingPadLabel = Cm::IrIntf::CreateLabel("$P" + std::to_string(landingPadId));
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
    Cm::Core::GenResult result(emitter, genFlags);
    result.SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
    emitter->AddNextInstructionLabel(nextLabel);
    GenerateCall(nullptr, irFunctionRepository.GetDoNothingFunction(), nullptr, result, false);
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
        Ir::Intf::LabelObject* landingPadLabel = Cm::IrIntf::CreateLabel("$P" + std::to_string(landingPad->Id()));
        emitter->Own(landingPadLabel);
        emitter->SetGotoTargetLabel(landingPadLabel);
        Cm::Core::GenResult result(emitter, genFlags);
        result.SetMainObject(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), typeRepository);
        GenerateCall(nullptr, irFunctionRepository.GetDoNothingFunction(), nullptr, result, false);

        for (const std::unique_ptr<Cm::BoundTree::BoundDestructionStatement>& destructionStatement : landingPad->DestructionStatements())
        {
            destructionStatement->Accept(*this);
        }
        int jumpToCatchId = landingPad->JumpToCatchId();
        if (jumpToCatchId != -1)    // got handler to jump to...
        {
            Ir::Intf::LabelObject* catchIdLabel = Cm::IrIntf::CreateLabel("$C" + std::to_string(jumpToCatchId));
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
            if (!returnType || returnType->IsVoidTypeSymbol() || currentFunction->GetFunctionSymbol()->ReturnsClassObjectByValue())
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

} } // namespace Cm::Emit
