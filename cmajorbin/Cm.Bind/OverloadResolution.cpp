/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/OverloadResolution.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/Template.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.Core/ClassConversionTable.hpp>
#include <Cm.Core/DerivedTypeOpRepository.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/TypeParameterSymbol.hpp>
#include <unordered_set>
#include <algorithm>

namespace Cm { namespace Bind {

struct ArgumentMatch
{
    ArgumentMatch(): conversionRank(Cm::Sym::ConversionRank::exactMatch), conversionDistance(0)
    {
    }
    ArgumentMatch(Cm::Sym::ConversionRank conversionRank_) : conversionRank(conversionRank_), conversionDistance(0) 
    {
    }
    ArgumentMatch(Cm::Sym::ConversionRank conversionRank_, const Cm::Sym::DerivationCounts& parameterDerivationCounts_, const Cm::Sym::DerivationCounts& argumentDerivationCounts_) : 
        conversionRank(conversionRank_), conversionDistance(0), parameterDerivationCounts(parameterDerivationCounts_), argumentDerivationCounts(argumentDerivationCounts_) 
    {
    }
    ArgumentMatch(Cm::Sym::ConversionRank conversionRank_, int conversionDistance_, const Cm::Sym::DerivationCounts& parameterDerivationCounts_, const Cm::Sym::DerivationCounts& argumentDerivationCounts_):
        conversionRank(conversionRank_), conversionDistance(conversionDistance_), parameterDerivationCounts(parameterDerivationCounts_), argumentDerivationCounts(argumentDerivationCounts_) 
    {
    }
    Cm::Sym::ConversionRank conversionRank;
    int conversionDistance;
    Cm::Sym::DerivationCounts parameterDerivationCounts;
    Cm::Sym::DerivationCounts argumentDerivationCounts;
};

bool BetterArgumentMatch(const ArgumentMatch& left, const ArgumentMatch& right)
{
    if (Cm::Sym::BetterConversionRank(left.conversionRank, right.conversionRank))
    {
        return true;
    }
    else if (Cm::Sym::BetterConversionRank(right.conversionRank, left.conversionRank))
    {
        return false;
    }
    else
    {
        if (left.conversionDistance < right.conversionDistance)
        {
            return true;
        }
        else if (right.conversionDistance > right.conversionDistance)
        {
            return false;
        }
        else if (left.parameterDerivationCounts < right.parameterDerivationCounts)
        {
            return true;
        }
        else if (right.parameterDerivationCounts < left.parameterDerivationCounts)
        {
            return false;
        }
        else if (left.argumentDerivationCounts < right.argumentDerivationCounts)
        {
            return true;
        }
        else 
        {
            return false;
        }
    }
}

struct FunctionMatch
{
    FunctionMatch(Cm::Sym::FunctionSymbol* function_) : function(function_), numConversions(0) {}
    Cm::Sym::FunctionSymbol* function;
    std::vector<ArgumentMatch> argumentMatches;
    int numConversions;
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    std::vector<Cm::Sym::TypeSymbol*> templateArguments;
};

struct BetterFunctionMatch
{
    bool operator()(const FunctionMatch& left, const FunctionMatch& right) const
    {
        int leftBetterArgumentMatches = 0;
        int rightBetterArgumentMatches = 0;
        int n = int(left.argumentMatches.size());
        for (int i = 0; i < n; ++i)
        {
            const ArgumentMatch& leftMatch = left.argumentMatches[i];
            const ArgumentMatch& rightMatch = right.argumentMatches[i];
            if (BetterArgumentMatch(leftMatch, rightMatch))
            {
                ++leftBetterArgumentMatches;
            }
            else if (BetterArgumentMatch(rightMatch, leftMatch))
            {
                ++rightBetterArgumentMatches;
            }
        }
        if (leftBetterArgumentMatches > rightBetterArgumentMatches)
        {
            return true;
        }
        else if (rightBetterArgumentMatches > leftBetterArgumentMatches)
        {
            return false;
        }
        else if (left.numConversions < right.numConversions)
        {
            return true;
        }
        else if (right.numConversions < left.numConversions)
        { 
            return false;
        }
        else if (!left.function->IsFunctionTemplate() && right.function->IsFunctionTemplate())
        {
            return true;
        }
        else if (!right.function->IsFunctionTemplate() && left.function->IsFunctionTemplate())
        {
            return false;
        }
        else if (!left.function->IsFunctionTemplateSpecialization() && right.function->IsFunctionTemplateSpecialization())
        {
            return true;
        }
        else if (!right.function->IsFunctionTemplateSpecialization() && left.function->IsFunctionTemplateSpecialization())
        {
            return  false;
        }
        else
        {
            return false;
        }
    }
};

bool CheckArgVsParam(const Cm::Core::Argument& argument, Cm::Sym::TypeSymbol* parameterType)
{
    if (parameterType->IsNonConstReferenceType())
    {
        if (argument.Category() != Cm::Core::ArgumentCategory::lvalue)
        {
            return false;
        }
        if (argument.Type()->IsConstType())
        {
            return false;
        }
    }
    else if (parameterType->IsRvalueRefType() && !argument.BindToRvalueRef() && !argument.Type()->IsRvalueRefType())
    {
        return false;
    }
    return true;
}

enum class ClassConversionType
{
    baseToDerived, derivedToBase
};

bool BaseClassDerivedClassRelationShip(Cm::Core::ClassConversionTable& classConversionTable, ClassConversionType classConversionType, Cm::Sym::TypeSymbol* plainParameterType, 
    Cm::Sym::TypeSymbol* plainArgumentType, Cm::Sym::TypeSymbol* parameterType, Cm::Sym::TypeSymbol* argumentType, int& distance, Cm::Sym::FunctionSymbol*& conversion, const Cm::Parsing::Span& span)
{
    int numParameterPointers = plainParameterType->GetPointerCount();
    int numArgumentPointers = plainArgumentType->GetPointerCount();
    bool bothArePointerTypes = numParameterPointers > 0 && numArgumentPointers > 0;
    bool parameterTypeIsReferenceType = parameterType->IsReferenceType();
    if (bothArePointerTypes && numParameterPointers == numArgumentPointers || parameterTypeIsReferenceType)
    {
        Cm::Sym::TypeSymbol* parameterBaseType = plainParameterType->GetBaseType();
        Cm::Sym::TypeSymbol* argumentBaseType = plainArgumentType->GetBaseType();
        if (parameterBaseType->IsClassTypeSymbol() && argumentBaseType->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* parameterClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(parameterBaseType);
            Cm::Sym::ClassTypeSymbol* argumentClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(argumentBaseType);
            distance = 0;
            switch (classConversionType)
            {
                case ClassConversionType::derivedToBase:
                {
                    if (argumentClassType->HasBaseClass(parameterClassType, distance))
                    {
                        conversion = classConversionTable.MakeBaseClassDerivedClassConversion(parameterType, argumentType, distance, span);
                        return true;
                    }
                    break;
                }
                case ClassConversionType::baseToDerived:
                {
                    if (parameterClassType->HasBaseClass(argumentClassType, distance))
                    {
                        conversion = classConversionTable.MakeBaseClassDerivedClassConversion(parameterType, argumentType, distance, span);
                        return true;
                    }
                    break;
                }
            }
        }
    }
    return false;
}

bool FindConversion(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const Cm::Parsing::Span& span, Cm::Sym::ConversionType conversionType, 
    Cm::Sym::TypeSymbol* parameterType, const Cm::Core::Argument& argument, std::unordered_set<Cm::Sym::ClassTypeSymbol*>& conversionClassTypes, 
    Cm::Sym::FunctionSymbol*& conversion, Cm::Bind::ArgumentMatch& argumentMatch, int& numConversions)
{
    conversion = nullptr;
    Cm::Sym::TypeSymbol* argumentType = argument.Type();
    if (Cm::Sym::TypesEqual(parameterType, argumentType))
    {
        if (argument.BindToRvalueRef() && !parameterType->IsRvalueRefType())
        {
            argumentMatch = ArgumentMatch(Cm::Sym::ConversionRank::conversion, 1, parameterType->GetDerivationCounts(), argumentType->GetDerivationCounts());
        }
        else
        {
            argumentMatch = ArgumentMatch(Cm::Sym::ConversionRank::exactMatch);
        }
        return true;
    }
    if (!CheckArgVsParam(argument, parameterType)) return false;
    Cm::Sym::TypeSymbol* plainParameterType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePlainType(parameterType);
    Cm::Sym::TypeSymbol* plainArgumentType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePlainType(argumentType);
    if (Cm::Sym::TypesEqual(plainParameterType, plainArgumentType))
    {
        argumentMatch = ArgumentMatch(Cm::Sym::ConversionRank::exactMatch, parameterType->GetDerivationCounts(), argumentType->GetDerivationCounts());
        return true;
    }
    int distance = 0;
    Cm::Sym::FunctionSymbol* derivedBaseConversion = nullptr;
    if (BaseClassDerivedClassRelationShip(boundCompileUnit.ClassConversionTable(), ClassConversionType::derivedToBase, plainParameterType, plainArgumentType, parameterType, argumentType,
        distance, derivedBaseConversion, span))
    {
        argumentMatch = ArgumentMatch(Cm::Sym::ConversionRank::conversion, distance, parameterType->GetDerivationCounts(), argumentType->GetDerivationCounts());
        conversion = derivedBaseConversion;
        ++numConversions;
        return true;
    }
    if (conversionType == Cm::Sym::ConversionType::explicit_)
    {
        int distance = 0;
        Cm::Sym::FunctionSymbol* baseDerivedConversion = nullptr;
        if (BaseClassDerivedClassRelationShip(boundCompileUnit.ClassConversionTable(), ClassConversionType::baseToDerived, plainParameterType, plainArgumentType, parameterType, argumentType,
            distance, baseDerivedConversion, span))
        {
            argumentMatch = ArgumentMatch(Cm::Sym::ConversionRank::conversion, distance, parameterType->GetDerivationCounts(), argumentType->GetDerivationCounts());
            conversion  = baseDerivedConversion;
            ++numConversions;
            return true;
        }
    }
    if (plainArgumentType->IsPointerType())
    {
        boundCompileUnit.DerivedTypeOpRepository().InsertPointerConversionsToConversionTable(boundCompileUnit.ConversionTable(), plainArgumentType, span);
    }
    if (plainParameterType->IsPointerType())
    {
        boundCompileUnit.DerivedTypeOpRepository().InsertPointerConversionsToConversionTable(boundCompileUnit.ConversionTable(), plainParameterType, span);
    }
    if (plainArgumentType->IsEnumTypeSymbol())
    {
        boundCompileUnit.EnumTypeOpRepository().InsertEnumConversionsToConversionTable(boundCompileUnit.ConversionTable(), plainArgumentType, span);
    }
    if (plainParameterType->IsEnumTypeSymbol())
    {
        boundCompileUnit.EnumTypeOpRepository().InsertEnumConversionsToConversionTable(boundCompileUnit.ConversionTable(), plainParameterType, span);
    }
    Cm::Sym::ClassTypeSymbol* conversionClassType1 = nullptr;
    Cm::Sym::ClassTypeSymbol* conversionClassType2 = nullptr;
    if (plainArgumentType->IsClassTypeSymbol())
    {
        conversionClassType1 = static_cast<Cm::Sym::ClassTypeSymbol*>(plainArgumentType);
        if (conversionClassTypes.find(conversionClassType1) != conversionClassTypes.end())
        {
            conversionClassType1 = nullptr;
        }
    }
    if (plainParameterType->IsClassTypeSymbol())
    {
        conversionClassType2 = static_cast<Cm::Sym::ClassTypeSymbol*>(plainParameterType);
        if (conversionClassTypes.find(conversionClassType2) != conversionClassTypes.end())
        {
            conversionClassType1 = nullptr;
        }
    }
    if (conversionClassType1)
    {
        conversionClassTypes.insert(conversionClassType1);
        for (Cm::Sym::FunctionSymbol* conversionFun : conversionClassType1->Conversions())
        {
            boundCompileUnit.ConversionTable().AddConversion(conversionFun);
        }
    }
    if (conversionClassType2 && conversionClassType2 != conversionClassType1)
    {
        conversionClassTypes.insert(conversionClassType2);
        for (Cm::Sym::FunctionSymbol* conversionFun : conversionClassType2->Conversions())
        {
            boundCompileUnit.ConversionTable().AddConversion(conversionFun);
        }
    }
    Cm::Sym::FunctionSymbol* otherConversion = boundCompileUnit.ConversionTable().GetConversion(plainArgumentType, plainParameterType);
    if (otherConversion)
    {
        if (otherConversion->IsConvertingConstructor())
        {
            argumentMatch = ArgumentMatch(otherConversion->GetConversionRank(), otherConversion->GetConversionDistance(),
                parameterType->GetDerivationCounts(), argumentType->GetDerivationCounts());
            conversion = otherConversion;
            ++numConversions;
            return true;
        }
        else
        {
            // todo: conversion function
            return true;
        }
    }
    return  false;
}

bool FindConversions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::vector<Cm::Sym::ParameterSymbol*>& parameters, const std::vector<Cm::Core::Argument>& arguments, 
    Cm::Sym::ConversionType conversionType, const Cm::Parsing::Span& span, FunctionMatch& functionMatch, std::unordered_set<Cm::Sym::ClassTypeSymbol*>& conversionClassTypes)
{
    int arity = int(parameters.size());
    if (arity != int(arguments.size()))
    {
        throw std::runtime_error("wrong arity");
    }
    for (int i = 0; i < arity; ++i)
    {
        Cm::Sym::ParameterSymbol* parameter = parameters[i];
        Cm::Sym::TypeSymbol* parameterType = parameter->GetType();
        const Cm::Core::Argument& argument = arguments[i];
        Cm::Sym::TypeSymbol* argumentType = argument.Type();
        Cm::Sym::FunctionSymbol* conversion = nullptr;
        Cm::Bind::ArgumentMatch argumentMatch;
        if (FindConversion(boundCompileUnit, span, conversionType, parameterType, argument, conversionClassTypes, conversion, argumentMatch, functionMatch.numConversions))
        {
            functionMatch.argumentMatches.push_back(argumentMatch);
            functionMatch.conversions.push_back(conversion);
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool DeduceTemplateParameter(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const Cm::Parsing::Span& span, Cm::Sym::TypeSymbol* parameterType, const Cm::Core::Argument& argument, 
    std::vector<Cm::Sym::TypeSymbol*>& templateArguments, std::unordered_set<Cm::Sym::ClassTypeSymbol*>& conversionClassTypes, 
    Cm::Sym::FunctionSymbol*& conversion, Cm::Bind::ArgumentMatch& argumentMatch, int& numConversions)
{
    conversion = nullptr;
    if (Cm::Sym::TypesEqual(parameterType, argument.Type()))
    {
        return true;
    }
    Cm::Sym::TypeSymbol* parameterBaseType = parameterType->GetBaseType();
    if (parameterBaseType->IsTypeParameterSymbol())
    {
        Cm::Sym::TypeParameterSymbol* templateParameterSymbol = static_cast<Cm::Sym::TypeParameterSymbol*>(parameterBaseType);
        int index = templateParameterSymbol->Index();
        Cm::Sym::TypeSymbol*& templateArgumentType = templateArguments[index];
        if (!templateArgumentType)  // unbound template argument
        {
            templateArgumentType = argument.Type();
            return true;
        }
        else    // bound template argument
        {
            if (Cm::Sym::TypesEqual(argument.Type(), templateArgumentType))
            {
                return true;
            }
            else
            {
                if (FindConversion(boundCompileUnit, span, Cm::Sym::ConversionType::implicit, templateArgumentType, argument, conversionClassTypes, conversion, argumentMatch, numConversions))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool DeduceTypeParameters(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::vector<Cm::Sym::TypeParameterSymbol*>& templateParameters,
    const std::vector<Cm::Sym::TypeSymbol*>& boundTemplateArguments, const std::vector<Cm::Sym::ParameterSymbol*>& parameters, const std::vector<Cm::Core::Argument>& arguments,
    const Cm::Parsing::Span& span, std::unordered_set<Cm::Sym::ClassTypeSymbol*>& conversionClassTypes, FunctionMatch& functionMatch)
{
    int m = int(parameters.size());
    if (m != int(arguments.size())) return false;
    int n = int(templateParameters.size());
    functionMatch.templateArguments.resize(n);
    int b = int(boundTemplateArguments.size());
    if (b > n)
    {
        return false; 
    }
    if (b > 0)
    {
        for (int i = 0; i < b; ++i)
        {
            functionMatch.templateArguments[i] = boundTemplateArguments[i];
        }
    }
    Cm::Sym::ContainerScope deductionScope;
    for (Cm::Sym::TypeParameterSymbol* templateParameter : templateParameters)
    {
        deductionScope.Install(templateParameter);
    }
    deductionScope.SetParent(containerScope);
    for (int i = 0; i < m; ++i)
    {
        Cm::Sym::ParameterSymbol* parameterSymbol = parameters[i];
        Cm::Ast::ParameterNode* parameterNode = static_cast<Cm::Ast::ParameterNode*>(boundCompileUnit.SymbolTable().GetNode(parameterSymbol));
        Cm::Ast::Node* parameterTypeExpr = parameterNode->TypeExpr();
        Cm::Sym::TypeSymbol* parameterType = ResolveType(boundCompileUnit.SymbolTable(), &deductionScope, boundCompileUnit.GetFileScope(), parameterTypeExpr);
        const Cm::Core::Argument& argument = arguments[i];
        Cm::Sym::FunctionSymbol* conversion = nullptr;
        Cm::Bind::ArgumentMatch argumentMatch;
        bool bound = DeduceTemplateParameter(boundCompileUnit, span, parameterType, argument, functionMatch.templateArguments, conversionClassTypes, conversion, argumentMatch, functionMatch.numConversions);
        if (bound)
        {
            functionMatch.conversions.push_back(conversion);
            functionMatch.argumentMatches.push_back(argumentMatch);
        }
        else
        {
            return false;
        }
    }
    for (Cm::Sym::TypeSymbol* templateArgumentType : functionMatch.templateArguments)
    {
        if (!templateArgumentType)  // unbound template parameter
        {
            return false;
        }
    }
    return true;
}

std::string MakeOverloadName(const std::string& groupName, const std::vector<Cm::Core::Argument>& arguments)
{
    std::string overloadName(groupName);
    overloadName.append(1, '(');
    bool first = true;
    for (const Cm::Core::Argument& argument : arguments)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            overloadName.append(", ");
        }
        overloadName.append(argument.Type()->FullName());
        if (argument.BindToRvalueRef())
        {
            overloadName.append("&&");
        }
    }
    overloadName.append(1, ')');
    return overloadName;
}

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName, 
    const std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions)
{
    return ResolveOverload(containerScope, boundCompileUnit, groupName, arguments, functionLookups, span, conversions, Cm::Sym::ConversionType::implicit, OverloadResolutionFlags::none);
}

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName, 
    const std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions, 
    OverloadResolutionFlags flags)
{
    return ResolveOverload(containerScope, boundCompileUnit, groupName, arguments, functionLookups, span, conversions, Cm::Sym::ConversionType::implicit, flags);
}

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName, 
    const std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions, 
    Cm::Sym::ConversionType conversionType, OverloadResolutionFlags flags)
{
    std::vector<Cm::Sym::TypeSymbol*> boundTemplateArguments;
    return ResolveOverload(containerScope, boundCompileUnit, groupName, arguments, functionLookups, span, conversions, conversionType, boundTemplateArguments, flags);
}

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName,
    const std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions,
    Cm::Sym::ConversionType conversionType, const std::vector<Cm::Sym::TypeSymbol*>& boundTemplateArguments, OverloadResolutionFlags flags)
{
    std::unordered_set<Cm::Sym::ClassTypeSymbol*> conversionClassTypes;
    conversions.clear();
    int arity = int(arguments.size());
    std::unordered_set<Cm::Sym::FunctionSymbol*> viableFunctions;
    if ((flags & OverloadResolutionFlags::bindOnlyMemberFunctions) == OverloadResolutionFlags::none)
    {
        boundCompileUnit.DerivedTypeOpRepository().CollectViableFunctions(groupName, arity, arguments, boundCompileUnit.ConversionTable(), span, viableFunctions);
        boundCompileUnit.EnumTypeOpRepository().CollectViableFunctions(groupName, arity, arguments, boundCompileUnit.ConversionTable(), span, viableFunctions);
    }
    std::unique_ptr<Cm::Core::Exception> ownedException = nullptr;
    Cm::Core::Exception* exception = nullptr;
    boundCompileUnit.SynthesizedClassFunRepository().CollectViableFunctions(groupName, arity, arguments, span, containerScope, viableFunctions, exception);
    if (exception)
    {
        ownedException.reset(exception);
    }
    if (viableFunctions.empty())
    {
        for (const Cm::Sym::FunctionLookup& functionLookup : functionLookups)
        {
            Cm::Sym::ScopeLookup lookup = functionLookup.Lookup();
            Cm::Sym::ContainerScope* scope = functionLookup.Scope();
            if (scope)
            {
                scope->CollectViableFunctions(lookup, groupName, arity, viableFunctions);
            }
        }
    }
    if (viableFunctions.empty())
    {
        std::string overloadName = MakeOverloadName(groupName, arguments);
        if (GetFlag(OverloadResolutionFlags::nothrow, flags))
        {
            return nullptr;
        }
        else if (exception)
        {
            Cm::Core::Exception copyOfEx = *exception;
            throw copyOfEx;
        }
        else
        {
            throw Cm::Core::Exception("overload resolution failed: '" + overloadName + "' not found. No viable functions taking " + std::to_string(arity) + " arguments found in function group '" + groupName + "'.", span);
        }
    }
    std::vector<FunctionMatch> functionMatches;
    Cm::Sym::FunctionSymbol* convertingCtor = nullptr;
    bool mustCast = false;
    for (Cm::Sym::FunctionSymbol* viableFunction : viableFunctions)
    {
        if (viableFunction->IsConvertingConstructor())
        {
            if (viableFunction->GetConversionType() == Cm::Sym::ConversionType::explicit_ && conversionType == Cm::Sym::ConversionType::implicit)
            {
                FunctionMatch functionMatch(viableFunction);
                bool candidateFound = FindConversions(boundCompileUnit, viableFunction->Parameters(), arguments, Cm::Sym::ConversionType::explicit_, span, functionMatch, conversionClassTypes);
                if (candidateFound)
                {
                    mustCast = true;
                    convertingCtor = viableFunction;
                }
                continue;
            }
        }
        FunctionMatch functionMatch(viableFunction);
        if (viableFunction->IsFunctionTemplate())
        {
            bool candidateFound = DeduceTypeParameters(containerScope, boundCompileUnit, viableFunction->TypeParameters(), boundTemplateArguments, viableFunction->Parameters(), arguments, span,
                conversionClassTypes, functionMatch);
            if (candidateFound)
            {
                functionMatches.push_back(functionMatch);
            }
        }
        else if (boundTemplateArguments.empty())
        {
            bool candidateFound = FindConversions(boundCompileUnit, viableFunction->Parameters(), arguments, conversionType, span, functionMatch, conversionClassTypes);
            if (candidateFound)
            {
                functionMatches.push_back(functionMatch);
            }
        }
    }
    if (functionMatches.empty())
    {
        if (mustCast)
        {
            if (GetFlag(OverloadResolutionFlags::nothrow, flags))
            {
                return nullptr;
            }
            else
            {
                throw Cm::Core::Exception("overload resolution failed: cannot convert '" + arguments[1].Type()->FullName() + "' to '" + convertingCtor->GetTargetType()->FullName() + "' without a cast", span);
            }
        }
        else
        {
            std::string overloadName = MakeOverloadName(groupName, arguments);
            if (GetFlag(OverloadResolutionFlags::nothrow, flags))
            {
                return nullptr;
            }
            else
            {
                throw Cm::Core::Exception("overload resolution failed: '" + overloadName + "' not found, or there are no acceptable conversions for all argument types. " +
                    std::to_string(viableFunctions.size()) + " viable functions examined.", span);
            }
        }
    }
    else
    {
        Cm::Sym::FunctionSymbol* function = nullptr;
        FunctionMatch bestMatch(function);
        if (functionMatches.size() > 1)
        {
            BetterFunctionMatch betterFunctionMatch;
            std::sort(functionMatches.begin(), functionMatches.end(), betterFunctionMatch);
            if (betterFunctionMatch(functionMatches[0], functionMatches[1]))
            {
                bestMatch = functionMatches[0];
                conversions = bestMatch.conversions;
                function = bestMatch.function;
            }
            else
            {
                std::string overloadName = MakeOverloadName(groupName, arguments);
                std::string matchedFunctionNames;
                bool first = true;
                FunctionMatch equalMatch = functionMatches[0];
                std::vector<FunctionMatch> equalMatches;
                equalMatches.push_back(equalMatch);
                int n = int(functionMatches.size());
                for (int i = 1; i < n; ++i)
                {
                    const FunctionMatch& match = functionMatches[i];
                    if (!betterFunctionMatch(equalMatch, match))
                    {
                        equalMatches.push_back(match);
                    }
                }
                for (const FunctionMatch& match : equalMatches)
                {
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        matchedFunctionNames.append(" or ");
                    }
                    matchedFunctionNames.append(match.function->FullName());
                }
                if (GetFlag(OverloadResolutionFlags::nothrow, flags))
                {
                    return nullptr;
                }
                else
                {
                    throw Cm::Core::Exception("overload resolution for overload name '" + overloadName + "' failed: call is ambiguous:\n" + matchedFunctionNames, span);
                }
            }
        }
        else // single best
        {
            bestMatch = functionMatches[0];
            conversions = bestMatch.conversions;
            function = bestMatch.function;
        }
        if (function->IsSuppressed())
        {
            if (GetFlag(OverloadResolutionFlags::nothrow, flags))
            {
                return nullptr;
            }
            else
            {
                throw Cm::Core::Exception("cannot call suppressed member function", span, function->GetSpan());
            }
        }
        if (function->IsFunctionTemplate())
        {
            Cm::Core::FunctionTemplateKey key(function, bestMatch.templateArguments);
            Cm::Sym::FunctionSymbol* functionTemplateInstance = boundCompileUnit.FunctionTemplateRepository().GetFunctionTemplateInstance(key);
            if (!functionTemplateInstance)
            {
                functionTemplateInstance = Instantiate(containerScope, boundCompileUnit, function, bestMatch.templateArguments);
                boundCompileUnit.FunctionTemplateRepository().AddFunctionTemplateInstance(key, functionTemplateInstance);
            }
            function = functionTemplateInstance;
        }
        return function;
    }
}

} } // namespace Cm::Bind
