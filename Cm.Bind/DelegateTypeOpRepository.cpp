/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/DelegateTypeOpRepository.hpp>
#include <Cm.Bind/OverloadResolution.hpp>
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.Sym/FunctionGroupSymbol.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Bind {

DelegateFromFunCtor::DelegateFromFunCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegatePtrType_, Cm::Sym::DelegateTypeSymbol* delegateType_,
    Cm::Sym::FunctionSymbol* functionSymbol_) :
    Cm::Core::BasicTypeOp(delegateType_), delegateType(delegateType_), functionSymbol(functionSymbol_)
{
    SetGroupName("@constructor");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Span(), "this"));
    typeRepository.GetSymbolTable().SetSidAndAddSymbol(thisParam);
    thisParam->SetType(delegatePtrType_);
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* funParam(new Cm::Sym::ParameterSymbol(Span(), "fun"));
    typeRepository.GetSymbolTable().SetSidAndAddSymbol(funParam);
    funParam->SetType(delegatePtrType_);
    AddSymbol(funParam);
    ComputeName();
}

void DelegateFromFunCtor::Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result)
{
    Cm::IrIntf::Init(emitter, GetIrType(), result.Arg1(), result.MainObject());
}

DelegateFromFunAssignment::DelegateFromFunAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegatePtrType_, Cm::Sym::DelegateTypeSymbol* delegateType_, 
    Cm::Sym::FunctionSymbol* functionSymbol_) :
    Cm::Core::BasicTypeOp(delegateType_), delegateType(delegateType_), functionSymbol(functionSymbol_)
{
    SetGroupName("operator=");
    Cm::Sym::TypeSymbol* voidType = typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId));
    SetReturnType(voidType);
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Span(), "this"));
    typeRepository.GetSymbolTable().SetSidAndAddSymbol(thisParam);
    thisParam->SetType(delegatePtrType_);
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* funParam(new Cm::Sym::ParameterSymbol(Span(), "fun"));
    typeRepository.GetSymbolTable().SetSidAndAddSymbol(funParam);
    funParam->SetType(delegatePtrType_);
    AddSymbol(funParam);
    ComputeName();
}

void DelegateFromFunAssignment::Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result)
{
    Cm::IrIntf::Assign(emitter, GetIrType(), result.Arg1(), result.MainObject());
}

Cm::Sym::FunctionSymbol* ResolveDelegateOverload(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::DelegateTypeSymbol* delegateType, 
    const std::string& groupName, const Cm::Parsing::Span& span)
{
    std::vector<Cm::Core::Argument> resolutionArguments;
    for (Cm::Sym::ParameterSymbol* parameter : delegateType->Parameters())
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
    resolutionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, containerScope));
    resolutionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::fileScopes, nullptr));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* overload = ResolveOverload(containerScope, boundCompileUnit, groupName, resolutionArguments, resolutionLookups, span, conversions);
    if (delegateType->IsNothrow() && overload->CanThrow())
    {
        throw Cm::Core::Exception("conflicting nothrow specification of delegate and function types", span, overload->GetSpan());
    }
    if (!delegateType->IsNothrow() && !overload->CanThrow())
    {
        throw Cm::Core::Exception("conflicting nothrow specification of delegate and function types", span, overload->GetSpan());
    }
    return overload;
}

Cm::Sym::FunctionSymbol* DelegateTypeOpCache::GetDefaultCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegateType)
{
    if (!defaultCtor)
    {
        defaultCtor.reset(new Cm::Core::DefaultCtor(typeRepository, delegateType));
    }
    return defaultCtor.get();
}

Cm::Sym::FunctionSymbol* DelegateTypeOpCache::GetCopyCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegateType)
{
    if (!copyCtor)
    {
        copyCtor.reset(new Cm::Core::CopyCtor(typeRepository, delegateType));
    }
    return copyCtor.get();
}

Cm::Sym::FunctionSymbol* DelegateTypeOpCache::GetMoveCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegateType)
{
    if (!moveCtor)
    {
        moveCtor.reset(new Cm::Core::MoveCtor(typeRepository, delegateType));
    }
    return moveCtor.get();
}

Cm::Sym::FunctionSymbol* DelegateTypeOpCache::GetCopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegateType)
{
    if (!copyAssignment)
    {
        copyAssignment.reset(new Cm::Core::CopyAssignment(typeRepository, delegateType));
    }
    return copyAssignment.get();
}

Cm::Sym::FunctionSymbol* DelegateTypeOpCache::GetMoveAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegateType)
{
    if (!moveAssignment)
    {
        moveAssignment.reset(new Cm::Core::MoveAssignment(typeRepository, delegateType));
    }
    return moveAssignment.get();
}

Cm::Sym::FunctionSymbol* DelegateTypeOpCache::GetOpEqual(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegateType)
{
    if (!opEqual)
    {
        opEqual.reset(new Cm::Core::OpEqual(typeRepository, delegateType));
    }
    return opEqual.get();
}

Cm::Sym::FunctionSymbol* DelegateTypeOpCache::GetOpLess(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegateType)
{
    if (!opLess)
    {
        opLess.reset(new Cm::Core::OpLess(typeRepository, delegateType));
    }
    return opLess.get();
}

Cm::Sym::FunctionSymbol* DelegateTypeOpCache::GetDelegateFromFunCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegatePtrType, Cm::Sym::DelegateTypeSymbol* delegateType, 
    Cm::Sym::FunctionSymbol* fun)
{
    std::unique_ptr<Cm::Sym::FunctionSymbol>& delegateFromFunCtor = delegateFromFunCtorMap[fun];
    if (!delegateFromFunCtor)
    {
        delegateFromFunCtor.reset(new DelegateFromFunCtor(typeRepository, delegatePtrType, delegateType, fun));
    }
    return delegateFromFunCtor.get();
}

Cm::Sym::FunctionSymbol* DelegateTypeOpCache::GetDelegateFromFunAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegatePtrType, 
    Cm::Sym::DelegateTypeSymbol* delegateType, Cm::Sym::FunctionSymbol* fun)
{
    std::unique_ptr<Cm::Sym::FunctionSymbol>& delegateFromFunAssignment = delegateFromFunAssignmentMap[fun];
    if (!delegateFromFunAssignment)
    {
        delegateFromFunAssignment.reset(new DelegateFromFunAssignment(typeRepository, delegatePtrType, delegateType, fun));
    }
    return delegateFromFunAssignment.get();
}

DelegateTypeOpGroup::~DelegateTypeOpGroup()
{
}

void DelegateConstructorOpGroup::CollectViableFunctions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span,
    int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
    Cm::Sym::TypeRepository& typeRepository, DelegateTypeOpCacheMap& delegateTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity == 1)
    {
        Cm::Sym::TypeSymbol* type = arguments[0].Type();
        if (type->IsPointerType() && type->GetPointerCount() == 1 && type->GetBaseType()->IsDelegateTypeSymbol()) // delegate default constructor
        {
            Cm::Sym::TypeSymbol* delegateType = type->GetBaseType();
            DelegateTypeOpCache& cache = delegateTypeOpCacheMap[delegateType];
            viableFunctions.insert(cache.GetDefaultCtor(typeRepository, delegateType));
        }
    }
    else if (arity == 2)
    {
        Cm::Sym::TypeSymbol* type = arguments[0].Type();
        if (type->IsPointerType() && type->GetPointerCount() == 1 && type->GetBaseType()->IsDelegateTypeSymbol())
        {
            Cm::Sym::DelegateTypeSymbol* delegateType = static_cast<Cm::Sym::DelegateTypeSymbol*>(type->GetBaseType());
            Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
            if (Cm::Sym::TypesEqual(delegateType, rightType)) // delegate copy constructor
            {
                DelegateTypeOpCache& cache = delegateTypeOpCacheMap[delegateType];
                viableFunctions.insert(cache.GetCopyCtor(typeRepository, delegateType));
            }
            else
            {
                Cm::Sym::TypeSymbol* alternateRightType = typeRepository.MakeConstReferenceType(delegateType, span);
                if (Cm::Sym::TypesEqual(alternateRightType, rightType)) // delegate copy constructor
                {
                    DelegateTypeOpCache& cache = delegateTypeOpCacheMap[delegateType];
                    viableFunctions.insert(cache.GetCopyCtor(typeRepository, delegateType));
                }
                else
                {
                    Cm::Sym::TypeSymbol* rvalueRefRightType = typeRepository.MakeRvalueRefType(delegateType, span);
                    if (Cm::Sym::TypesEqual(rvalueRefRightType, rightType)) // delegate move constructor
                    {
                        DelegateTypeOpCache& cache = delegateTypeOpCacheMap[delegateType];
                        viableFunctions.insert(cache.GetMoveCtor(typeRepository, delegateType));
                    }
                    else if (rightType->IsFunctionGroupTypeSymbol())    // delegate from function constructor
                    {
                        Cm::Sym::FunctionGroupTypeSymbol* functionGroupTypeSymbol = static_cast<Cm::Sym::FunctionGroupTypeSymbol*>(rightType);
                        Cm::Sym::FunctionGroupSymbol* functionGroupSymbol = functionGroupTypeSymbol->GetFunctionGroupSymbol();
                        Cm::Sym::FunctionSymbol* functionSymbol = ResolveDelegateOverload(containerScope, boundCompileUnit, delegateType, functionGroupSymbol->Name(), span);
                        DelegateTypeOpCache& cache = delegateTypeOpCacheMap[delegateType];
                        Cm::Sym::TypeSymbol* delegatePtrType = typeRepository.MakePointerType(delegateType, span);
                        viableFunctions.insert(cache.GetDelegateFromFunCtor(typeRepository, delegatePtrType, delegateType, functionSymbol));
                        arguments[1] = Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, delegatePtrType);
                    }
                }
            }
        }
    }
}

void DelegateAssignmentOpGroup::CollectViableFunctions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span,
    int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
    Cm::Sym::TypeRepository& typeRepository, DelegateTypeOpCacheMap& delegateTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 2) return;
    Cm::Sym::TypeSymbol* type = arguments[0].Type();
    if (type->IsPointerType() && type->GetPointerCount() == 1 && type->GetBaseType()->IsDelegateTypeSymbol())
    {
        Cm::Sym::DelegateTypeSymbol* delegateType = static_cast<Cm::Sym::DelegateTypeSymbol*>(type->GetBaseType());
        Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
        if (Cm::Sym::TypesEqual(delegateType, rightType)) // delegate copy assignment
        {
            DelegateTypeOpCache& cache = delegateTypeOpCacheMap[delegateType];
            viableFunctions.insert(cache.GetCopyAssignment(typeRepository, delegateType));
        }
        else
        {
            Cm::Sym::TypeSymbol* alternateRightType = typeRepository.MakeConstReferenceType(delegateType, span);
            if (Cm::Sym::TypesEqual(alternateRightType, rightType)) // delegate copy assignment
            {
                DelegateTypeOpCache& cache = delegateTypeOpCacheMap[delegateType];
                viableFunctions.insert(cache.GetCopyAssignment(typeRepository, delegateType));
            }
            else
            {
                Cm::Sym::TypeSymbol* rvalueRefRightType = typeRepository.MakeRvalueRefType(delegateType, span);
                if (Cm::Sym::TypesEqual(rvalueRefRightType, rightType)) // delegate move assignment
                {
                    DelegateTypeOpCache& cache = delegateTypeOpCacheMap[delegateType];
                    viableFunctions.insert(cache.GetMoveAssignment(typeRepository, delegateType));
                }
                else if (rightType->IsFunctionGroupTypeSymbol())    // delegate from function assignment
                {
                    Cm::Sym::FunctionGroupTypeSymbol* functionGroupTypeSymbol = static_cast<Cm::Sym::FunctionGroupTypeSymbol*>(rightType);
                    Cm::Sym::FunctionGroupSymbol* functionGroupSymbol = functionGroupTypeSymbol->GetFunctionGroupSymbol();
                    Cm::Sym::FunctionSymbol* functionSymbol = ResolveDelegateOverload(containerScope, boundCompileUnit, delegateType, functionGroupSymbol->Name(), span);
                    DelegateTypeOpCache& cache = delegateTypeOpCacheMap[delegateType];
                    Cm::Sym::TypeSymbol* delegatePtrType = typeRepository.MakePointerType(delegateType, span);
                    viableFunctions.insert(cache.GetDelegateFromFunAssignment(typeRepository, delegatePtrType, delegateType, functionSymbol));
                    arguments[1] = Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, delegatePtrType);
                }
            }
        }
    }
}

void DelegateEqualOpGroup::CollectViableFunctions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span,
    int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
    Cm::Sym::TypeRepository& typeRepository, DelegateTypeOpCacheMap& delegateTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 2) return;
    Cm::Sym::TypeSymbol* leftType = arguments[0].Type();
    Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
    if (leftType->IsDelegateTypeSymbol() && Cm::Sym::TypesEqual(leftType, rightType))
    {
        DelegateTypeOpCache& cache = delegateTypeOpCacheMap[leftType];
        viableFunctions.insert(cache.GetOpEqual(typeRepository, leftType));
    }
}

void DelegateLessOpGroup::CollectViableFunctions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span,
    int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
    Cm::Sym::TypeRepository& typeRepository, DelegateTypeOpCacheMap& delegateTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 2) return;
    Cm::Sym::TypeSymbol* leftType = arguments[0].Type();
    Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
    if (leftType->IsDelegateTypeSymbol() && Cm::Sym::TypesEqual(leftType, rightType))
    {
        DelegateTypeOpCache& cache = delegateTypeOpCacheMap[leftType];
        viableFunctions.insert(cache.GetOpLess(typeRepository, leftType));
    }
}

DelegateTypeOpRepository::DelegateTypeOpRepository(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_) : boundCompileUnit(boundCompileUnit_)
{
    delegateTypeOpGroupMap["@constructor"] = &delegateConstructorOpGroup;
    delegateTypeOpGroupMap["operator="] = &delegateAssignmentOpGroup;
    delegateTypeOpGroupMap["operator=="] = &delegateEqualOpGroup;
    delegateTypeOpGroupMap["operator<"] = &delegateLessOpGroup;
}

void DelegateTypeOpRepository::CollectViableFunctions(Cm::Sym::ContainerScope* containerScope, const std::string& groupName, 
    int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
    const Cm::Parsing::Span& span, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (int(arguments.size()) != arity)
    {
        throw std::runtime_error("wrong number of arguments");
    }
    if (arity < 1 || arity > 2) return;
    DelegateTypeOpGroupMapIt i = delegateTypeOpGroupMap.find(groupName);
    if (i != delegateTypeOpGroupMap.end())
    {
        DelegateTypeOpGroup* group = i->second;
        group->CollectViableFunctions(boundCompileUnit, containerScope, span, arity, arguments, conversionTable, boundCompileUnit.SymbolTable().GetTypeRepository(), delegateTypeOpCacheMap, 
            viableFunctions);
    }
}

} } // namespace Cm::Bind
