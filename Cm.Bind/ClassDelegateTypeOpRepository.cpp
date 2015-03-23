/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/ClassDelegateTypeOpRepository.hpp>
#include <Cm.Bind/OverloadResolution.hpp>
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
    if (!classDelegateFromFunCtor)
    {
        classDelegateFromFunCtor.reset(new ClassDelegateFromFunCtor(typeRepository, classDelegateType, fun));
    }
    return classDelegateFromFunCtor.get();
}

Cm::Sym::FunctionSymbol* ClassDelegateTypeOpCache::GetClassDelegateFromFunAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::ClassDelegateTypeSymbol* classDelegateType, 
    Cm::Sym::FunctionSymbol* fun)
{
    if (!classDelegateFromFunAssignment)
    {
        classDelegateFromFunAssignment.reset(new ClassDelegateFromFunAssignment(typeRepository, classDelegateType, fun));
    }
    return classDelegateFromFunAssignment.get();
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

ClassDelegateTypeOpRepository::ClassDelegateTypeOpRepository(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_) : boundCompileUnit(boundCompileUnit_)
{
    classDelegateTypeOpGroupMap["@constructor"] = &classDelegateConstructorOpGroup;
    classDelegateTypeOpGroupMap["operator="] = &classDelegateAssignmentOpGroup;
}

void ClassDelegateTypeOpRepository::CollectViableFunctions(Cm::Sym::ContainerScope* containerScope, const std::string& groupName,
    int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
    const Cm::Parsing::Span& span, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (int(arguments.size()) != arity)
    {
        throw std::runtime_error("wrong number of arguments");
    }
    if (arity != 3) return;
    ClassDelegateTypeOpGroupMapIt i = classDelegateTypeOpGroupMap.find(groupName);
    if (i != classDelegateTypeOpGroupMap.end())
    {
        ClassDelegateTypeOpGroup* group = i->second;
        group->CollectViableFunctions(boundCompileUnit, containerScope, span, arity, arguments, conversionTable, boundCompileUnit.SymbolTable().GetTypeRepository(), classDelegateTypeOpCacheMap,
            viableFunctions);
    }
}

} } // namespace Cm::Bind
