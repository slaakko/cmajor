/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/ClassDelegateTypeOpRepository.hpp>
#include <Cm.Bind/OverloadResolution.hpp>
#include <Cm.Bind/Parameter.hpp>
#include <Cm.Bind/ExpressionBinder.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.Sym/FunctionGroupSymbol.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>

namespace Cm { namespace Bind {

ClassDelegateFromFunCtor::ClassDelegateFromFunCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::ClassDelegateTypeSymbol* classDelegateType_, Cm::Sym::FunctionSymbol* functionSymbol_) :
    Cm::Core::BasicTypeOp(classDelegateType_), classDelegateType(classDelegateType_), delegateType(nullptr), functionSymbol(functionSymbol_)
{
    SetGroupName("@constructor");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Span(), "this"));
    Cm::Sym::TypeSymbol* classDelegatePtrType = typeRepository.MakePointerType(classDelegateType, Cm::Parsing::Span());
    thisParam->SetType(classDelegatePtrType);
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* objParam(new Cm::Sym::ParameterSymbol(Span(), "obj"));
    objParam->SetType(typeRepository.MakeGenericPtrType(Cm::Parsing::Span()));
    AddSymbol(objParam);
    Cm::Sym::ParameterSymbol* dlgParam(new Cm::Sym::ParameterSymbol(Span(), "dlg"));
    Cm::Sym::Symbol* dlgTypeSymbol = classDelegateType->GetContainerScope()->Lookup("dlgType");
    if (!dlgTypeSymbol->IsDelegateTypeSymbol())
    {
        throw std::runtime_error("not delegate type");
    }
    delegateType = static_cast<Cm::Sym::DelegateTypeSymbol*>(dlgTypeSymbol);
    dlgParam->SetType(delegateType);
    AddSymbol(dlgParam);
    ComputeName();
}

void ClassDelegateFromFunCtor::Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result)
{
    // implementation is provided by FunctionEmitter
}

ClassDelegateFromFunAssignment::ClassDelegateFromFunAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::ClassDelegateTypeSymbol* classDelegateType_,
    Cm::Sym::FunctionSymbol* functionSymbol_) :
    Cm::Core::BasicTypeOp(classDelegateType_), classDelegateType(classDelegateType_), delegateType(nullptr), functionSymbol(functionSymbol_)
{
    SetGroupName("operator=");
    Cm::Sym::TypeSymbol* voidType = typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId));
    SetReturnType(voidType);
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Span(), "this"));
    Cm::Sym::TypeSymbol* classDelegatePtrType = typeRepository.MakePointerType(classDelegateType, Cm::Parsing::Span());
    thisParam->SetType(classDelegatePtrType);
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* objParam(new Cm::Sym::ParameterSymbol(Span(), "obj"));
    objParam->SetType(typeRepository.MakeGenericPtrType(Cm::Parsing::Span()));
    AddSymbol(objParam);
    Cm::Sym::ParameterSymbol* dlgParam(new Cm::Sym::ParameterSymbol(Span(), "dlg"));
    Cm::Sym::Symbol* dlgTypeSymbol = classDelegateType->GetContainerScope()->Lookup("dlgType");
    if (!dlgTypeSymbol->IsDelegateTypeSymbol())
    {
        throw std::runtime_error("not delegate type");
    }
    delegateType = static_cast<Cm::Sym::DelegateTypeSymbol*>(dlgTypeSymbol);
    dlgParam->SetType(delegateType);
    AddSymbol(dlgParam);
    ComputeName();
}

void ClassDelegateFromFunAssignment::Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result)
{
    // implementation is provided by FunctionEmitter
}

ClassDelegateEqualOp::ClassDelegateEqualOp() : Cm::Sym::FunctionSymbol(Cm::Parsing::Span(), "*class_delegate_equal*"), classDelegateType(nullptr), ns()
{
}

ClassDelegateEqualOp::ClassDelegateEqualOp(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ClassDelegateTypeSymbol* classDelegateType_) : 
    Cm::Sym::FunctionSymbol(classDelegateType_->GetSpan(), "*class_delegate_equal*"), classDelegateType(classDelegateType_), ns(containerScope->Ns()->FullName())
{
    SetGroupName("operator==");
    Cm::Sym::TypeSymbol* boolType = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
    SetReturnType(boolType);
    Cm::Sym::ParameterSymbol* leftParam(new Cm::Sym::ParameterSymbol(Span(), "left"));
    leftParam->SetType(classDelegateType);
    AddSymbol(leftParam);
    Cm::Sym::ParameterSymbol* rightParam(new Cm::Sym::ParameterSymbol(Span(), "right"));
    rightParam->SetType(classDelegateType);
    AddSymbol(rightParam);
    ComputeName();
    SetAccess(Cm::Sym::SymbolAccess::public_);
    SetReplicated();
    SetNothrow();
    SetParent(classDelegateType->Ns());
    std::unique_ptr<Cm::BoundTree::BoundFunction> opEqual(new Cm::BoundTree::BoundFunction(nullptr, this));
    opEqual->SetBody(new Cm::BoundTree::BoundCompoundStatement(nullptr));
    GenerateReceives(containerScope, boundCompileUnit, opEqual.get());
    Cm::Sym::TypeSymbol* classDelegateTypePointer = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(classDelegateType, classDelegateType->GetSpan());
    std::vector<Cm::Core::Argument> boolCopyCtorArgs;
    Cm::Sym::TypeSymbol* boolPtrType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(boolType, classDelegateType->GetSpan());
    boolCopyCtorArgs.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, boolPtrType));
    boolCopyCtorArgs.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, boolType));
    Cm::Sym::FunctionLookupSet boolCopyCtorLookups;
    boolCopyCtorLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, boundCompileUnit.SymbolTable().GlobalScope()));
    std::vector<Cm::Sym::FunctionSymbol*> boolCopyCtorConversions;
    Cm::Sym::FunctionSymbol* boolCopyCtor = Cm::Bind::ResolveOverload(boundCompileUnit.SymbolTable().GlobalScope(), boundCompileUnit, "@constructor", boolCopyCtorArgs, boolCopyCtorLookups,
        classDelegateType->GetSpan(), boolCopyCtorConversions);
    for (Cm::Sym::MemberVariableSymbol* memberVariableSymbol : classDelegateType->MemberVariables())
    {
        Cm::Sym::TypeSymbol* memberVariableType = memberVariableSymbol->GetType();
        std::vector<Cm::Core::Argument> resolutionArguments;
        Cm::BoundTree::BoundParameter* boundLeftParam = new Cm::BoundTree::BoundParameter(nullptr, leftParam);
        boundLeftParam->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        boundLeftParam->SetType(leftParam->GetType());
        Cm::BoundTree::BoundMemberVariable* boundLeftMemberVar = new Cm::BoundTree::BoundMemberVariable(nullptr, memberVariableSymbol);
        boundLeftMemberVar->SetType(memberVariableType);
        boundLeftMemberVar->SetClassObject(boundLeftParam);
        resolutionArguments.push_back(Cm::Core::Argument(boundLeftMemberVar->GetArgumentCategory(), boundLeftMemberVar->GetType()));

        Cm::BoundTree::BoundParameter* boundRightParam = new Cm::BoundTree::BoundParameter(nullptr, rightParam);
        boundRightParam->SetType(rightParam->GetType());
        boundRightParam->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        Cm::BoundTree::BoundMemberVariable* boundRightMemberVar = new Cm::BoundTree::BoundMemberVariable(nullptr, memberVariableSymbol);
        boundRightMemberVar->SetType(memberVariableType);
        boundRightMemberVar->SetClassObject(boundRightParam);
        resolutionArguments.push_back(Cm::Core::Argument(boundRightMemberVar->GetArgumentCategory(), boundRightMemberVar->GetType()));

        Cm::Sym::FunctionLookupSet functionLookups;
        functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, containerScope));
        std::vector<Cm::Sym::FunctionSymbol*> conversions;
        Cm::Sym::FunctionSymbol* memberVarOpEqual = ResolveOverload(containerScope, boundCompileUnit, "operator==", resolutionArguments, functionLookups, classDelegateType->GetSpan(), 
            conversions);
        Cm::BoundTree::BoundExpressionList arguments;
        arguments.Add(boundLeftMemberVar);
        arguments.Add(boundRightMemberVar);
        PrepareArguments(containerScope, boundCompileUnit, opEqual.get(), memberVarOpEqual->GetReturnType(), memberVarOpEqual->Parameters(), arguments, false, 
            boundCompileUnit.IrClassTypeRepository(), memberVarOpEqual->IsBasicTypeOp());
        Cm::BoundTree::BoundExpression* leftArg = arguments[0].release();
        Cm::BoundTree::BoundExpression* rightArg = arguments[1].release();
        Cm::BoundTree::BoundBinaryOp* eq = new Cm::BoundTree::BoundBinaryOp(nullptr, leftArg, rightArg);
        eq->SetType(boolType);
        eq->SetFunction(memberVarOpEqual);
        Cm::BoundTree::BoundUnaryOp* notEq = new Cm::BoundTree::BoundUnaryOp(nullptr, eq);
        notEq->SetType(boolType);
        std::vector<Cm::Core::Argument> notBoolArgs;
        notBoolArgs.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, boolType));
        Cm::Sym::FunctionLookupSet notBoolLookups;
        notBoolLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, boundCompileUnit.SymbolTable().GlobalScope()));
        std::vector<Cm::Sym::FunctionSymbol*> notBoolConversions;
        Cm::Sym::FunctionSymbol* notBool = Cm::Bind::ResolveOverload(boundCompileUnit.SymbolTable().GlobalScope(), boundCompileUnit, "operator!", notBoolArgs, notBoolLookups, 
            classDelegateType->GetSpan(), notBoolConversions);
        notEq->SetFunction(notBool);
        notEq->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        Cm::BoundTree::BoundConditionalStatement* ifNotEqReturnFalse = new Cm::BoundTree::BoundConditionalStatement(nullptr);
        Cm::BoundTree::BoundReturnStatement* returnFalse = new Cm::BoundTree::BoundReturnStatement(nullptr);
        Cm::BoundTree::BoundLiteral* falseLiteral = new Cm::BoundTree::BoundLiteral(nullptr);
        falseLiteral->SetValue(new Cm::Sym::BoolValue(false));
        falseLiteral->SetType(boolType);
        returnFalse->SetExpression(falseLiteral);
        returnFalse->SetReturnType(boolType);
        returnFalse->SetConstructor(boolCopyCtor);
        ifNotEqReturnFalse->SetCondition(notEq);
        ifNotEqReturnFalse->AddStatement(returnFalse);
        opEqual->Body()->AddStatement(ifNotEqReturnFalse);
    }
    Cm::BoundTree::BoundLiteral* trueLiteral = new Cm::BoundTree::BoundLiteral(nullptr);
    trueLiteral->SetValue(new Cm::Sym::BoolValue(true));
    trueLiteral->SetType(boolType);
    Cm::BoundTree::BoundReturnStatement* returnTrue = new Cm::BoundTree::BoundReturnStatement(nullptr);
    returnTrue->SetReturnType(boolType);
    returnTrue->SetExpression(trueLiteral);
    returnTrue->SetConstructor(boolCopyCtor);
    opEqual->Body()->AddStatement(returnTrue);
    boundCompileUnit.AddBoundNode(opEqual.release());
}

void ClassDelegateEqualOp::Write(Cm::Sym::BcuWriter& writer)
{
    writer.GetBinaryWriter().Write(ns);
    writer.Write(classDelegateType);
}

void ClassDelegateEqualOp::Read(Cm::Sym::BcuReader& reader)
{
    ns = reader.GetBinaryReader().ReadString();
    Cm::Sym::BoundCompileUnit* symbolUnit = reader.GetBoundCompileUnit();
    Cm::BoundTree::BoundCompileUnit* boundCompileUnit = static_cast<Cm::BoundTree::BoundCompileUnit*>(symbolUnit);
    Cm::Sym::SymbolTable& symbolTable = boundCompileUnit->SymbolTable();
    Cm::Sym::Symbol* nsSymbol = symbolTable.GlobalScope()->Lookup(ns);
    Cm::Sym::ContainerScope* containerScope = nsSymbol->GetContainerScope();
    Cm::Sym::Symbol* classDelegateSymbol = reader.ReadSymbol();
    if (classDelegateSymbol->IsClassDelegateTypeSymbol())
    {
        Cm::Sym::ClassDelegateTypeSymbol* classDelegateType = static_cast<Cm::Sym::ClassDelegateTypeSymbol*>(classDelegateSymbol);
        ClassDelegateEqualOp* replacement = new ClassDelegateEqualOp(containerScope, *boundCompileUnit, classDelegateType);
        reader.SetClassDelegateEqualOp(replacement);
    }
    else
    {
        throw std::runtime_error("class delegate type symbol expected");
    }
}

Cm::Sym::FunctionSymbol* ResolveClassDelegateOverload(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, 
    Cm::Sym::ClassDelegateTypeSymbol* classDelegateType, Cm::Sym::TypeSymbol* classObjectType, const std::string& groupName, const Cm::Parsing::Span& span)
{
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Sym::TypeSymbol* classObjectPtrType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(classObjectType, span);
    resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, classObjectPtrType));
    for (Cm::Sym::ParameterSymbol* parameter : classDelegateType->Parameters())
    {
        Cm::Sym::TypeSymbol* parameterType = parameter->GetType();
        Cm::Core::ArgumentCategory category = Cm::Core::ArgumentCategory::rvalue;
        if (parameterType->IsReferenceType())
        {
            category = Cm::Core::ArgumentCategory::lvalue;
        }
        resolutionArguments.push_back(Cm::Core::Argument(category, parameterType));
    }
    Cm::Sym::FunctionLookupSet resolutionLookups;
    resolutionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base, classObjectType->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* overload = ResolveOverload(containerScope, boundCompileUnit, groupName, resolutionArguments, resolutionLookups, span, conversions);
    if (classDelegateType->IsNothrow() && overload->CanThrow())
    {
        throw Cm::Core::Exception("conflicting nothrow specification of class delegate and function types", span, overload->GetSpan());
    }
    if (!classDelegateType->IsNothrow() && !overload->CanThrow())
    {
        throw Cm::Core::Exception("conflicting nothrow specification of class delegate and function types", span, overload->GetSpan());
    }
    return overload;
}

Cm::Sym::FunctionSymbol* ClassDelegateTypeOpCache::GetClassDelegateFromFunCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::ClassDelegateTypeSymbol* classDelegateType, 
    Cm::Sym::FunctionSymbol* fun)
{
    std::unique_ptr<Cm::Sym::FunctionSymbol>& classDelegateFromFunCtor = classDelegateFromFunCtorMap[fun];
    if (!classDelegateFromFunCtor)
    {
        classDelegateFromFunCtor.reset(new ClassDelegateFromFunCtor(typeRepository, classDelegateType, fun));
    }
    return classDelegateFromFunCtor.get();
}

Cm::Sym::FunctionSymbol* ClassDelegateTypeOpCache::GetClassDelegateFromFunAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::ClassDelegateTypeSymbol* classDelegateType, 
    Cm::Sym::FunctionSymbol* fun)
{
    std::unique_ptr<Cm::Sym::FunctionSymbol>& classDelegateFromFunAssignment = classDelegateFromFunAssignmentMap[fun];
    if (!classDelegateFromFunAssignment)
    {
        classDelegateFromFunAssignment.reset(new ClassDelegateFromFunAssignment(typeRepository, classDelegateType, fun));
    }
    return classDelegateFromFunAssignment.get();
}

Cm::Sym::FunctionSymbol* ClassDelegateTypeOpCache::GetClassDelegateEqualOp(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, 
    Cm::Sym::ClassDelegateTypeSymbol* classDelegateType)
{
    if (!classDelegateEqualOp)
    {
        classDelegateEqualOp.reset(new ClassDelegateEqualOp(containerScope, boundCompileUnit, classDelegateType));
    }
    return classDelegateEqualOp.get();
}

ClassDelegateTypeOpGroup::~ClassDelegateTypeOpGroup()
{
}

void ClassDelegateConstructorOpGroup::CollectViableFunctions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span,
    int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
    Cm::Sym::TypeRepository& typeRepository, ClassDelegateTypeOpCacheMap& delegateTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 3) return;
    Cm::Sym::TypeSymbol* type = arguments[0].Type();
    if (type->IsPointerType() && type->GetPointerCount() == 1 && type->GetBaseType()->IsClassDelegateTypeSymbol())
    {
        Cm::Sym::ClassDelegateTypeSymbol* classDelegateType = static_cast<Cm::Sym::ClassDelegateTypeSymbol*>(type->GetBaseType());
        Cm::Sym::TypeSymbol* classObjectType = arguments[1].Type();
        if (classObjectType->IsClassTypeSymbol())
        {
            Cm::Sym::TypeSymbol* rightType = arguments[2].Type();
            if (rightType->IsFunctionGroupTypeSymbol())    // class delegate from function constructor
            {
                Cm::Sym::FunctionGroupTypeSymbol* functionGroupTypeSymbol = static_cast<Cm::Sym::FunctionGroupTypeSymbol*>(rightType);
                Cm::Sym::FunctionGroupSymbol* functionGroupSymbol = functionGroupTypeSymbol->GetFunctionGroupSymbol();
                Cm::Sym::FunctionSymbol* functionSymbol = ResolveClassDelegateOverload(containerScope, boundCompileUnit, classDelegateType, classObjectType, functionGroupSymbol->Name(), span);
                ClassDelegateTypeOpCache& cache = delegateTypeOpCacheMap[classDelegateType];
                Cm::Sym::FunctionSymbol* viableFun = cache.GetClassDelegateFromFunCtor(typeRepository, classDelegateType, functionSymbol);
                viableFunctions.insert(viableFun);
                arguments[1] = Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(classObjectType, span));
                arguments[2] = Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, static_cast<ClassDelegateFromFunCtor*>(viableFun)->DelegateType());
            }
        }
        else if (classObjectType->IsPointerToClassType())
        {
            Cm::Sym::TypeSymbol* rightType = arguments[2].Type();
            if (rightType->IsFunctionGroupTypeSymbol())    // class delegate from function constructor
            {
                Cm::Sym::FunctionGroupTypeSymbol* functionGroupTypeSymbol = static_cast<Cm::Sym::FunctionGroupTypeSymbol*>(rightType);
                Cm::Sym::FunctionGroupSymbol* functionGroupSymbol = functionGroupTypeSymbol->GetFunctionGroupSymbol();
                Cm::Sym::FunctionSymbol* functionSymbol = ResolveClassDelegateOverload(containerScope, boundCompileUnit, classDelegateType, classObjectType->GetBaseType(), 
                    functionGroupSymbol->Name(), span);
                ClassDelegateTypeOpCache& cache = delegateTypeOpCacheMap[classDelegateType];
                Cm::Sym::FunctionSymbol* viableFun = cache.GetClassDelegateFromFunCtor(typeRepository, classDelegateType, functionSymbol);
                viableFunctions.insert(viableFun);
                arguments[2] = Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, static_cast<ClassDelegateFromFunCtor*>(viableFun)->DelegateType());
            }
        }
    }
}

void ClassDelegateAssignmentOpGroup::CollectViableFunctions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span,
    int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
    Cm::Sym::TypeRepository& typeRepository, ClassDelegateTypeOpCacheMap& delegateTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 3) return;
    Cm::Sym::TypeSymbol* type = arguments[0].Type();
    if (type->IsPointerType() && type->GetPointerCount() == 1 && type->GetBaseType()->IsClassDelegateTypeSymbol())
    {
        Cm::Sym::ClassDelegateTypeSymbol* classDelegateType = static_cast<Cm::Sym::ClassDelegateTypeSymbol*>(type->GetBaseType());
        Cm::Sym::TypeSymbol* classObjectType = arguments[1].Type();
        if (classObjectType->IsClassTypeSymbol())
        {
            Cm::Sym::TypeSymbol* rightType = arguments[2].Type();
            if (rightType->IsFunctionGroupTypeSymbol())    // class delegate from function assignment
            {
                Cm::Sym::FunctionGroupTypeSymbol* functionGroupTypeSymbol = static_cast<Cm::Sym::FunctionGroupTypeSymbol*>(rightType);
                Cm::Sym::FunctionGroupSymbol* functionGroupSymbol = functionGroupTypeSymbol->GetFunctionGroupSymbol();
                Cm::Sym::FunctionSymbol* functionSymbol = ResolveClassDelegateOverload(containerScope, boundCompileUnit, classDelegateType, classObjectType, functionGroupSymbol->Name(), span);
                ClassDelegateTypeOpCache& cache = delegateTypeOpCacheMap[classDelegateType];
                Cm::Sym::FunctionSymbol* viableFun = cache.GetClassDelegateFromFunAssignment(typeRepository, classDelegateType, functionSymbol);
                viableFunctions.insert(viableFun);
                arguments[1] = Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(classObjectType, span));
                arguments[2] = Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, static_cast<ClassDelegateFromFunCtor*>(viableFun)->DelegateType());
            }
        }
        else if (classObjectType->IsPointerToClassType())
        {
            Cm::Sym::TypeSymbol* rightType = arguments[2].Type();
            if (rightType->IsFunctionGroupTypeSymbol())    // class delegate from function assignment
            {
                Cm::Sym::FunctionGroupTypeSymbol* functionGroupTypeSymbol = static_cast<Cm::Sym::FunctionGroupTypeSymbol*>(rightType);
                Cm::Sym::FunctionGroupSymbol* functionGroupSymbol = functionGroupTypeSymbol->GetFunctionGroupSymbol();
                Cm::Sym::FunctionSymbol* functionSymbol = ResolveClassDelegateOverload(containerScope, boundCompileUnit, classDelegateType, classObjectType->GetBaseType(),
                    functionGroupSymbol->Name(), span);
                ClassDelegateTypeOpCache& cache = delegateTypeOpCacheMap[classDelegateType];
                Cm::Sym::FunctionSymbol* viableFun = cache.GetClassDelegateFromFunAssignment(typeRepository, classDelegateType, functionSymbol);
                viableFunctions.insert(viableFun);
                arguments[2] = Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, static_cast<ClassDelegateFromFunCtor*>(viableFun)->DelegateType());
            }
        }
    }
}

void ClassDelegateEqualOpGroup::CollectViableFunctions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span,
    int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
    Cm::Sym::TypeRepository& typeRepository, ClassDelegateTypeOpCacheMap& delegateTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 2) return;
    Cm::Sym::TypeSymbol* type = arguments[0].Type();
    if (type->IsClassDelegateTypeSymbol())
    {
        Cm::Sym::ClassDelegateTypeSymbol* classDelegateType = static_cast<Cm::Sym::ClassDelegateTypeSymbol*>(type);
        Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
        if (rightType->IsClassDelegateTypeSymbol() && Cm::Sym::TypesEqual(classDelegateType, rightType))
        {
            ClassDelegateTypeOpCache& cache = delegateTypeOpCacheMap[classDelegateType];
            Cm::Sym::FunctionSymbol* viableFun = cache.GetClassDelegateEqualOp(containerScope, boundCompileUnit, classDelegateType);
            viableFunctions.insert(viableFun);
        }
    }
}

ClassDelegateTypeOpRepository::ClassDelegateTypeOpRepository(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_) : boundCompileUnit(boundCompileUnit_)
{
    classDelegateTypeOpGroupMap["@constructor"] = &classDelegateConstructorOpGroup;
    classDelegateTypeOpGroupMap["operator="] = &classDelegateAssignmentOpGroup;
    classDelegateTypeOpGroupMap["operator=="] = &classDelegateEqualOpGroup;
}

void ClassDelegateTypeOpRepository::CollectViableFunctions(Cm::Sym::ContainerScope* containerScope, const std::string& groupName,
    int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
    const Cm::Parsing::Span& span, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (int(arguments.size()) != arity)
    {
        throw std::runtime_error("wrong number of arguments");
    }
    if (arity < 2 || arity > 3) return;
    ClassDelegateTypeOpGroupMapIt i = classDelegateTypeOpGroupMap.find(groupName);
    if (i != classDelegateTypeOpGroupMap.end())
    {
        ClassDelegateTypeOpGroup* group = i->second;
        group->CollectViableFunctions(boundCompileUnit, containerScope, span, arity, arguments, conversionTable, boundCompileUnit.SymbolTable().GetTypeRepository(), classDelegateTypeOpCacheMap,
            viableFunctions);
    }
}

Cm::Sym::FunctionSymbol* ClassDelegateTypeOpFactory::CreateClassDelegateOp(Cm::Sym::BcuItemType itemType, Cm::Sym::TypeRepository& typeRepository, Cm::Sym::ClassDelegateTypeSymbol* classDelegateTypeSymbol,
    Cm::Sym::FunctionSymbol* functionSymbol) const
{
    switch (itemType)
    {
        case Cm::Sym::BcuItemType::bcuClassDelegateFromFunCtor: return new ClassDelegateFromFunCtor(typeRepository, classDelegateTypeSymbol, functionSymbol);
        case Cm::Sym::BcuItemType::bcuClassDelegateFromFunAssignment: return new ClassDelegateFromFunAssignment(typeRepository, classDelegateTypeSymbol, functionSymbol);
    }
    throw std::runtime_error("unknown item type " + std::to_string(uint8_t(itemType)));
}

Cm::Sym::FunctionSymbol* ClassDelegateTypeOpFactory::CreateClassDelegateOpEqual() const
{
    return new ClassDelegateEqualOp();
}

} } // namespace Cm::Bind
