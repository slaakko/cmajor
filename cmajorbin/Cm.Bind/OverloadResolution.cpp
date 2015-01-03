/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/OverloadResolution.hpp>
#include <Cm.Bind/Exception.hpp>
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <unordered_set>
#include <algorithm>

namespace Cm { namespace Bind {

struct ArgumentMatch
{
    ArgumentMatch(Cm::Core::ConversionRank conversionRank_) : conversionRank(conversionRank_), conversionDistance(0) 
    {
    }
    ArgumentMatch(Cm::Core::ConversionRank conversionRank_, const Cm::Sym::DerivationCounts& parameterDerivationCounts_, const Cm::Sym::DerivationCounts& argumentDerivationCounts_) : 
        conversionRank(conversionRank_), conversionDistance(0), parameterDerivationCounts(parameterDerivationCounts_), argumentDerivationCounts(argumentDerivationCounts_) 
    {
    }
    ArgumentMatch(Cm::Core::ConversionRank conversionRank_, int conversionDistance_, const Cm::Sym::DerivationCounts& parameterDerivationCounts_, const Cm::Sym::DerivationCounts& argumentDerivationCounts_):
        conversionRank(conversionRank_), conversionDistance(conversionDistance_), parameterDerivationCounts(parameterDerivationCounts_), argumentDerivationCounts(argumentDerivationCounts_) 
    {
    }
    Cm::Core::ConversionRank conversionRank;
    int conversionDistance;
    Cm::Sym::DerivationCounts parameterDerivationCounts;
    Cm::Sym::DerivationCounts argumentDerivationCounts;
};

bool BetterArgumentMatch(const ArgumentMatch& left, const ArgumentMatch& right)
{
    if (Cm::Core::BetterConversionRank(left.conversionRank, right.conversionRank))
    {
        return true;
    }
    else if (Cm::Core::BetterConversionRank(right.conversionRank, left.conversionRank))
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
    else if (parameterType->IsRvalueRefType() && !argument.BindToRvalueRef())
    {
        return false;
    }
    return true;
}

enum class ClassConversionType
{
    baseToDerived, derivedToBase
};

bool BaseClassDerivedClassRelationShip(Cm::Core::ClassConversionTable& classConversionTable, ClassConversionType classConversionType, 
    Cm::Sym::TypeSymbol* plainParameterType, Cm::Sym::TypeSymbol* plainArgumentType, Cm::Sym::TypeSymbol* parameterType, Cm::Sym::TypeSymbol* argumentType, int& distance, Cm::Sym::FunctionSymbol*& conversion)
{
    int numParameterPointers = plainParameterType->GetPointerCount();
    int numArgumentPointers = plainArgumentType->GetPointerCount();
    bool bothArePointerTypes = numParameterPointers > 0 && numArgumentPointers > 0;
    bool bothAreReferenceTypes = parameterType->IsReferenceType() && argumentType->IsReferenceType();
    if (bothArePointerTypes && numParameterPointers == numArgumentPointers || bothAreReferenceTypes)
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
                        conversion = classConversionTable.MakeBaseClassDerivedClassConversion(parameterType, argumentType, distance);
                        return true;
                    }
                    break;
                }
                case ClassConversionType::baseToDerived:
                {
                    if (parameterClassType->HasBaseClass(argumentClassType, distance))
                    {
                        conversion = classConversionTable.MakeBaseClassDerivedClassConversion(parameterType, argumentType, distance);
                        return true;
                    }
                    break;
                }
            }
        }
    }
    return false;
}

bool FindConversions(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ConversionTable& conversionTable, Cm::Core::ClassConversionTable& classConversionTable, Cm::Core::PointerOpRepository& pointerOpRepository,
    const std::vector<Cm::Sym::ParameterSymbol*>& parameters, const std::vector<Cm::Core::Argument>& arguments, Cm::Core::ConversionType conversionType, const Cm::Parsing::Span& span, 
    FunctionMatch& functionMatch)
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
        if (Cm::Sym::TypesEqual(parameterType, argumentType))
        {
            functionMatch.argumentMatches.push_back(ArgumentMatch(Cm::Core::ConversionRank::exactMatch));
            functionMatch.conversions.push_back(nullptr);
            continue;
        }
        if (!CheckArgVsParam(argument, parameterType)) return false;
        Cm::Sym::TypeSymbol* plainParameterType = symbolTable.GetTypeRepository().MakePlainType(parameterType);
        Cm::Sym::TypeSymbol* plainArgumentType = symbolTable.GetTypeRepository().MakePlainType(argumentType);
        if (Cm::Sym::TypesEqual(plainParameterType, plainArgumentType))
        {
            functionMatch.argumentMatches.push_back(ArgumentMatch(Cm::Core::ConversionRank::exactMatch, parameterType->GetDerivationCounts(), argumentType->GetDerivationCounts()));
            functionMatch.conversions.push_back(nullptr);
            continue;
        }
        int distance = 0;
        Cm::Sym::FunctionSymbol* conversion = nullptr;
        if (BaseClassDerivedClassRelationShip(classConversionTable, ClassConversionType::derivedToBase, plainParameterType, plainArgumentType, parameterType, argumentType, distance, conversion))
        {
            functionMatch.argumentMatches.push_back(ArgumentMatch(Cm::Core::ConversionRank::conversion, distance, parameterType->GetDerivationCounts(), argumentType->GetDerivationCounts()));
            functionMatch.conversions.push_back(conversion);
            ++functionMatch.numConversions;
            continue;
        }
        if (conversionType == Cm::Core::ConversionType::explicit_)
        {
            int distance = 0;
            Cm::Sym::FunctionSymbol* conversion = nullptr;
            if (BaseClassDerivedClassRelationShip(classConversionTable, ClassConversionType::baseToDerived, plainParameterType, plainArgumentType, parameterType, argumentType, distance, conversion))
            {
                functionMatch.argumentMatches.push_back(ArgumentMatch(Cm::Core::ConversionRank::conversion, distance, parameterType->GetDerivationCounts(), argumentType->GetDerivationCounts()));
                functionMatch.conversions.push_back(conversion);
                ++functionMatch.numConversions;
                continue;
            }
        }
        if (plainArgumentType->IsPointerType())
        {
            pointerOpRepository.InsertPointerConversionsToConversionTable(conversionTable, plainArgumentType, span);
        }
        if (plainParameterType->IsPointerType())
        {
            pointerOpRepository.InsertPointerConversionsToConversionTable(conversionTable, plainParameterType, span);
        }
        conversion = conversionTable.GetConversion(plainArgumentType, plainParameterType);
        if (conversion)
        {
            if (conversion->IsConvertingConstructor())
            {
                Cm::Core::ConvertingCtor* convertingCtor = static_cast<Cm::Core::ConvertingCtor*>(conversion);
                functionMatch.argumentMatches.push_back(ArgumentMatch(convertingCtor->GetConversionRank(), convertingCtor->GetConversionDistance(),
                    parameterType->GetDerivationCounts(), argumentType->GetDerivationCounts()));
                functionMatch.conversions.push_back(conversion);
                ++functionMatch.numConversions;
                continue;
            }
            else
            {
                // todo
                continue;
            }
        }
        return false;
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
    }
    overloadName.append(1, ')');
    return overloadName;
}

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ConversionTable& conversionTable, Cm::Core::ClassConversionTable& classConversionTable, 
    Cm::Core::PointerOpRepository& pointerOpRepository, const std::string& groupName, const std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, 
    std::vector<Cm::Sym::FunctionSymbol*>& conversions)
{
    return ResolveOverload(symbolTable, conversionTable, classConversionTable, pointerOpRepository, groupName, arguments, functionLookups, span, Cm::Core::ConversionType::implicit, conversions);
}

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ConversionTable& conversionTable, Cm::Core::ClassConversionTable& classConversionTable, 
    Cm::Core::PointerOpRepository& pointerOpRepository, const std::string& groupName, const std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, 
    Cm::Core::ConversionType conversionType, std::vector<Cm::Sym::FunctionSymbol*>& conversions)
{
    int arity = int(arguments.size());
    std::unordered_set<Cm::Sym::FunctionSymbol*> viableFunctions;
    pointerOpRepository.CollectViableFunctions(groupName, arity, arguments, conversionTable, span, viableFunctions);
    for (const Cm::Sym::FunctionLookup& functionLookup : functionLookups)
    {
        Cm::Sym::ScopeLookup lookup = functionLookup.Lookup();
        Cm::Sym::ContainerScope* scope = functionLookup.Scope();
        if (scope)
        {
            scope->CollectViableFunctions(lookup, groupName, arity, viableFunctions);
        }
    }
    if (viableFunctions.empty())
    {
        throw Exception("overload resolution failed: no overloads taking " + std::to_string(arity) + " arguments found in function group '" + groupName + "'", span);
    }
    std::vector<FunctionMatch> functionMatches;
    Cm::Core::ConvertingCtor* convertingCtor = nullptr;
    bool mustCast = false;
    for (Cm::Sym::FunctionSymbol* viableFunction : viableFunctions)
    {
        if (viableFunction->IsConvertingConstructor())
        {
            convertingCtor = static_cast<Cm::Core::ConvertingCtor*>(viableFunction);
            if (convertingCtor->GetConversionType() == Cm::Core::ConversionType::explicit_ && conversionType == Cm::Core::ConversionType::implicit)
            {
                FunctionMatch functionMatch(viableFunction);
                bool candidateFound = FindConversions(symbolTable, conversionTable, classConversionTable, pointerOpRepository, viableFunction->Parameters(), arguments, Cm::Core::ConversionType::explicit_, span, functionMatch);
                if (candidateFound)
                {
                    mustCast = true;
                }
                continue;
            }
        }
        FunctionMatch functionMatch(viableFunction);
        bool candidateFound = FindConversions(symbolTable, conversionTable, classConversionTable, pointerOpRepository, viableFunction->Parameters(), arguments, conversionType, span, functionMatch);
        if (candidateFound)
        {
            functionMatches.push_back(functionMatch);
        }
    }
    if (functionMatches.empty())
    {
        if (mustCast)
        {
            throw Exception("overload resolution failed: cannot convert '" + arguments[1].Type()->FullName() + "' to '" + convertingCtor->GetTargetType()->FullName() + "' without a cast", span);
        }
        else
        {
            std::string overloadName = MakeOverloadName(groupName, arguments);
            throw Exception("overload resolution failed: '" + overloadName + "' not found, or there are no acceptable conversions for all argument types. " +
                std::to_string(viableFunctions.size()) + " viable functions examined.", span);
        }
    }
    else
    {
        if (functionMatches.size() > 1)
        {
            BetterFunctionMatch betterFunctionMatch;
            std::sort(functionMatches.begin(), functionMatches.end(), betterFunctionMatch);
            if (betterFunctionMatch(functionMatches[0], functionMatches[1]))
            {
                const FunctionMatch& bestMatch = functionMatches[0];
                conversions = bestMatch.conversions;
                return bestMatch.function;
            }
            else
            {
                std::string overloadName = MakeOverloadName(groupName, arguments);
                std::string matchedFunctionNames;
                bool first = true;
                for (const FunctionMatch& match : functionMatches)
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
                throw Exception("overload resolution for overload name '" + overloadName + "' failed: call is ambiguous:\n" + matchedFunctionNames, span); 
            }
        }
        else // single best
        {
            const FunctionMatch& bestMatch = functionMatches[0];
            conversions = bestMatch.conversions;
            return bestMatch.function;
        }
    }
}

} } // namespace Cm::Bind
