/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
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
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/TypeParameterSymbol.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <unordered_set>
#include <algorithm>

namespace Cm { namespace Bind {

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

bool BetterFunctionMatch::operator()(const FunctionMatch& left, const FunctionMatch& right)
{
    int leftBetterArgumentMatches = 0;
    int rightBetterArgumentMatches = 0;
    int n = std::max(int(left.argumentMatches.size()), int(right.argumentMatches.size()));
    for (int i = 0; i < n; ++i)
    {
        ArgumentMatch leftMatch;
        if (i < int(left.argumentMatches.size()))
        {
            leftMatch = left.argumentMatches[i];
        }
        ArgumentMatch rightMatch;
        if (i < int(right.argumentMatches.size()))
        {
            rightMatch = right.argumentMatches[i];
        }
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
    else if (left.function->IsArrayConstructor() && !right.function->IsArrayConstructor())
    {
        return true;
    }
    else if (right.function->IsArrayConstructor() && !left.function->IsArrayConstructor())
    {
        return false;
    }
    else if (left.function->IsArrayAssignment() && !right.function->IsArrayAssignment())
    {
        return true;
    }
    else if (right.function->IsArrayAssignment() && !left.function->IsArrayAssignment())
    {
        return false;
    }
    else if (left.constraint && !right.constraint)
    {
        return true;
    }
    else if (right.constraint && !left.constraint)
    {
        return false;
    }
    else
    {
        if (left.constraint && right.constraint)
        {
            bool leftSubsumeRight = left.boundConstraint->Subsume(right.boundConstraint.get());
            bool rightSubsumeLeft = right.boundConstraint->Subsume(left.boundConstraint.get());
            if (leftSubsumeRight && !rightSubsumeLeft)
            {
                return true;
            }
            else if (rightSubsumeLeft && !leftSubsumeRight)
            {
                return false;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
}

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
        if (argument.BindToRvalueRef() && !parameterType->IsRvalueRefType() && !parameterType->IsValueTypeSymbol())
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
    if (plainArgumentType->IsArrayType() && plainParameterType->IsPointerType())
    {
        if (Cm::Sym::TypesEqual(plainParameterType->GetBaseType(), plainArgumentType->GetBaseType()))
        {
            argumentMatch = ArgumentMatch(Cm::Sym::ConversionRank::conversion, 1, parameterType->GetDerivationCounts(), argumentType->GetDerivationCounts());
            return true;
        }
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
            conversionClassType2 = nullptr;
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
    if (!otherConversion)
    {
        otherConversion = boundCompileUnit.ConversionTable().GetConversion(plainArgumentType, parameterType);
    }
    if (otherConversion)
    {
        if (otherConversion->IsConvertingConstructor() || otherConversion->IsConversionFunction())
        {
            argumentMatch = ArgumentMatch(otherConversion->GetConversionRank(), otherConversion->GetConversionDistance(),
                parameterType->GetDerivationCounts(), argumentType->GetDerivationCounts());
            conversion = otherConversion;
            ++numConversions;
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

bool Bind(const Cm::Ast::DerivationList& parameterDerivations, const Cm::Ast::DerivationList& argumentDerivations)
{
    if (Cm::Sym::HasRvalueRefDerivation(parameterDerivations))
    {
        return true;
    }
    else if (Cm::Sym::HasReferenceDerivation(parameterDerivations))
    {
        if (!Cm::Sym::HasConstDerivation(parameterDerivations) && Cm::Sym::HasConstReferenceDerivation(argumentDerivations))
        {
            return false;   // const reference cannot bind to non-const lvalue reference
        }
        return true;
    }
    return true;
}

bool Bind(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::TypeSymbol* parameterType, Cm::Sym::TypeSymbol* argumentType, std::vector<Cm::Sym::TypeSymbol*>& templateArguments, 
    Cm::Sym::TypeSymbol*& boundType)
{
    if (parameterType->IsTypeParameterSymbol())
    {
        Cm::Sym::TypeParameterSymbol* typeParameterSymbol = static_cast<Cm::Sym::TypeParameterSymbol*>(parameterType);
        int index = typeParameterSymbol->Index();
        Cm::Sym::TypeSymbol*& templateArgumentType = templateArguments[index];
        if (!templateArgumentType)  // unbound template argument
        {
            templateArgumentType = argumentType;
            boundType = argumentType;
            return true;
        }
        else if (Cm::Sym::TypesEqual(templateArgumentType, argumentType)) // bound template argument
        {
            boundType = argumentType;
            return true;
        }
    }
    else
    {
        Cm::Sym::TypeSymbol* parameterBaseType = parameterType->GetBaseType();
        if (parameterBaseType->IsTemplateTypeSymbol())
        {
            Cm::Ast::DerivationList argumentTypeDerivations = Cm::Sym::RemoveDerivations(argumentType->Derivations(), parameterType->Derivations());
            Cm::Sym::TypeSymbol* plainArgumentType = argumentType->GetBaseType();
            if (argumentTypeDerivations.NumDerivations() > 0)
            {
                plainArgumentType = boundCompileUnit.SymbolTable().GetTypeRepository().MakeDerivedType(argumentTypeDerivations, argumentType->GetBaseType(), std::vector<int>(), Cm::Parsing::Span());
            }
            if (plainArgumentType->IsTemplateTypeSymbol())
            {
                Cm::Sym::TemplateTypeSymbol* argumentTemplateTypeSymbol = static_cast<Cm::Sym::TemplateTypeSymbol*>(plainArgumentType);
                Cm::Sym::TemplateTypeSymbol* parameterTemplateTypeSymbol = static_cast<Cm::Sym::TemplateTypeSymbol*>(parameterBaseType);
                if (Cm::Sym::TypesEqual(argumentTemplateTypeSymbol->GetSubjectType(), parameterTemplateTypeSymbol->GetSubjectType()))
                {
                    int n = int(parameterTemplateTypeSymbol->TypeArguments().size());
                    int m = int(argumentTemplateTypeSymbol->TypeArguments().size());
                    if (m == n)
                    {
                        for (int i = 0; i < n; ++i)
                        {
                            Cm::Sym::TypeSymbol* argumentTypeArgumentSymbol = argumentTemplateTypeSymbol->TypeArguments()[i];
                            Cm::Sym::TypeSymbol* parameterTypeArgumentSymbol = parameterTemplateTypeSymbol->TypeArguments()[i];
                            if (parameterTypeArgumentSymbol->IsTypeParameterSymbol())
                            {
                                Cm::Sym::TypeParameterSymbol* typeParameterSymbol = static_cast<Cm::Sym::TypeParameterSymbol*>(parameterTypeArgumentSymbol);
                                int index = typeParameterSymbol->Index();
                                Cm::Sym::TypeSymbol*& templateArgumentType = templateArguments[index];
                                if (!templateArgumentType)  // unbound template argument
                                {
                                    templateArgumentType = argumentTypeArgumentSymbol;
                                }
                                else // bound template argument
                                {
                                    if (!Cm::Sym::TypesEqual(argumentTypeArgumentSymbol, templateArgumentType))
                                    {
                                        return false;
                                    }
                                }
                            }
                        }
                        return true;
                    }
                }
            }
        }
        else if (parameterType->IsDerivedTypeSymbol())
        {
            Cm::Ast::DerivationList argumentTypeDerivations = Cm::Sym::RemoveDerivations(argumentType->Derivations(), parameterType->Derivations());
            Cm::Sym::TypeSymbol* plainArgumentType = argumentType->GetBaseType();
            if (argumentTypeDerivations.NumDerivations() > 0)
            { 
                std::vector<int> arrayDimensions;
                if (argumentType->IsDerivedTypeSymbol())
                {
                    Cm::Sym::DerivedTypeSymbol* derivedArgumentType = static_cast<Cm::Sym::DerivedTypeSymbol*>(argumentType);
                    arrayDimensions = derivedArgumentType->GetArrayDimensions();
                }
                plainArgumentType = boundCompileUnit.SymbolTable().GetTypeRepository().MakeDerivedType(argumentTypeDerivations, argumentType->GetBaseType(), arrayDimensions, Cm::Parsing::Span());
            }
            if (Bind(boundCompileUnit, parameterType->GetBaseType(), plainArgumentType, templateArguments, boundType))
            {
                return true;
            }
        }
    }
    return false;
}

bool Bind(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::TypeSymbol* parameterType, Cm::Sym::TypeSymbol* argumentType, std::vector<Cm::Sym::TypeSymbol*>& templateArguments)
{
    Cm::Sym::TypeSymbol* boundType = nullptr;
    return Bind(boundCompileUnit, parameterType, argumentType, templateArguments, boundType);
}

bool DeduceTypeParameter(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const Cm::Parsing::Span& span, Cm::Sym::TypeSymbol* parameterType, const Cm::Core::Argument& argument,
    std::vector<Cm::Sym::TypeSymbol*>& templateArguments, std::unordered_set<Cm::Sym::ClassTypeSymbol*>& conversionClassTypes, 
    Cm::Sym::FunctionSymbol*& conversion, Cm::Bind::ArgumentMatch& argumentMatch, int& numConversions)
{
    conversion = nullptr;
    if (Cm::Sym::TypesEqual(parameterType, argument.Type()))
    {
        return true;
    }
    bool bindResult = Bind(boundCompileUnit, parameterType, argument.Type(), templateArguments);
    if (bindResult)
    {
        return true;
    }
    else if (FindConversion(boundCompileUnit, span, Cm::Sym::ConversionType::implicit, parameterType, argument, conversionClassTypes, conversion, argumentMatch, numConversions))
    {
        return true;
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
        Cm::Sym::TypeSymbol* parameterType = ResolveType(boundCompileUnit.SymbolTable(), &deductionScope, boundCompileUnit.GetFileScopes(), boundCompileUnit.ClassTemplateRepository(), boundCompileUnit, parameterTypeExpr,
            TypeResolverFlags::dontThrow);
        if (!parameterType)
        {
            return false;
        }
        const Cm::Core::Argument& argument = arguments[i];
        Cm::Sym::FunctionSymbol* conversion = nullptr;
        Cm::Bind::ArgumentMatch argumentMatch;
        bool bound = DeduceTypeParameter(boundCompileUnit, span, parameterType, argument, functionMatch.templateArguments, conversionClassTypes, conversion, argumentMatch, functionMatch.numConversions);
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
    std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions)
{
    return ResolveOverload(containerScope, boundCompileUnit, groupName, arguments, functionLookups, span, conversions, Cm::Sym::ConversionType::implicit, OverloadResolutionFlags::none);
}

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName,
    std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions,
    OverloadResolutionFlags flags, FunctionMatch& bestMatch, std::unique_ptr<Cm::Core::Exception>& exception)
{
    std::vector<Cm::Sym::TypeSymbol*> boundTemplateArguments;
    return ResolveOverload(containerScope, boundCompileUnit, groupName, arguments, functionLookups, span, conversions, Cm::Sym::ConversionType::implicit, boundTemplateArguments, flags, bestMatch, exception);
}

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName, 
    std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions, 
    OverloadResolutionFlags flags)
{
    return ResolveOverload(containerScope, boundCompileUnit, groupName, arguments, functionLookups, span, conversions, Cm::Sym::ConversionType::implicit, flags);
}

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName, 
    std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions, 
    Cm::Sym::ConversionType conversionType, OverloadResolutionFlags flags)
{
    std::vector<Cm::Sym::TypeSymbol*> boundTemplateArguments;
    return ResolveOverload(containerScope, boundCompileUnit, groupName, arguments, functionLookups, span, conversions, conversionType, boundTemplateArguments, flags);
}

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName,
    std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions,
    Cm::Sym::ConversionType conversionType, const std::vector<Cm::Sym::TypeSymbol*>& boundTemplateArguments, OverloadResolutionFlags flags)
{
    std::unique_ptr<Cm::Core::Exception> exception;
    FunctionMatch bestMatch;
    return ResolveOverload(containerScope, boundCompileUnit, groupName, arguments, functionLookups, span, conversions, conversionType, boundTemplateArguments, flags, bestMatch, exception);
}

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName,
    std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions,
    Cm::Sym::ConversionType conversionType, const std::vector<Cm::Sym::TypeSymbol*>& boundTemplateArguments, OverloadResolutionFlags flags, FunctionMatch& bestMatch,
    std::unique_ptr<Cm::Core::Exception>& exception)
{
    for (Cm::Core::Argument& argument : arguments)
    {
        if (argument.Type()->GetBaseType()->IsTemplateTypeSymbol())
        {
            Cm::Sym::TemplateTypeSymbol* templateTypeSymbol = static_cast<Cm::Sym::TemplateTypeSymbol*>(argument.Type()->GetBaseType());
            if (!templateTypeSymbol->Bound())
            {
                boundCompileUnit.ClassTemplateRepository().BindTemplateTypeSymbol(templateTypeSymbol, containerScope, boundCompileUnit.GetFileScopes());
            }
        }
    }
    std::unordered_set<Cm::Sym::ClassTypeSymbol*> conversionClassTypes;
    conversions.clear();
    int arity = int(arguments.size());
    std::unordered_set<Cm::Sym::FunctionSymbol*> viableFunctions;
    if ((flags & OverloadResolutionFlags::bindOnlyMemberFunctions) == OverloadResolutionFlags::none)
    {
        boundCompileUnit.DerivedTypeOpRepository().CollectViableFunctions(groupName, arity, arguments, boundCompileUnit.ConversionTable(), span, viableFunctions);
        boundCompileUnit.InterfaceTypeOpRepository().CollectViableFunctions(containerScope, groupName, arity, arguments, span, viableFunctions);
        boundCompileUnit.ArrayTypeOpRepository().CollectViableFunctions(groupName, arity, arguments, containerScope, span, viableFunctions);
        boundCompileUnit.EnumTypeOpRepository().CollectViableFunctions(groupName, arity, arguments, boundCompileUnit.ConversionTable(), span, viableFunctions);
        boundCompileUnit.DelegateTypeOpRepository().CollectViableFunctions(containerScope, groupName, arity, arguments, boundCompileUnit.ConversionTable(), span, viableFunctions);
    }
    std::unique_ptr<Cm::Core::Exception> innerException = nullptr;
    boundCompileUnit.ClassTemplateRepository().CollectViableFunctions(groupName, arity, arguments, span, containerScope, viableFunctions);
    boundCompileUnit.SynthesizedClassFunRepository().CollectViableFunctions(groupName, arity, arguments, span, containerScope, viableFunctions, innerException);
    boundCompileUnit.ClassDelegateTypeOpRepository().CollectViableFunctions(containerScope, groupName, arity, arguments, boundCompileUnit.ConversionTable(), span, viableFunctions);
    if (viableFunctions.empty())
    {
        bool fileScopesLookedUp = false;
        std::unordered_set<Cm::Sym::ContainerScope*> processedContainerScopes;
        for (const Cm::Sym::FunctionLookup& functionLookup : functionLookups)
        {
            Cm::Sym::ScopeLookup lookup = functionLookup.Lookup();
            Cm::Sym::ContainerScope* scope = functionLookup.Scope();
            if (scope)
            {
                scope->CollectViableFunctions(lookup, groupName, arity, viableFunctions);
                processedContainerScopes.insert(scope);
            }
            if ((lookup & Cm::Sym::ScopeLookup::fileScopes) != Cm::Sym::ScopeLookup::none && !fileScopesLookedUp)
            {
                fileScopesLookedUp = true;
                for (const std::unique_ptr<Cm::Sym::FileScope>& fileScope : boundCompileUnit.GetFileScopes())
                {
                    fileScope->CollectViableFunctions(groupName, arity, viableFunctions, processedContainerScopes);
                }
            }
        }
    }
    if (arguments.size() == 2 && arguments[1].Type()->GetBaseType()->IsClassTypeSymbol())
    {
        Cm::Sym::ClassTypeSymbol* cls = static_cast<Cm::Sym::ClassTypeSymbol*>(arguments[1].Type()->GetBaseType());
        for (Cm::Sym::FunctionSymbol* conversion : cls->Conversions())
        {
            if (conversion->IsConversionFunction())
            {
                Cm::Sym::TypeSymbol* targetType = conversion->GetTargetType();
                Cm::Sym::TypeSymbol* ptrTargetType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(targetType, span);
                Cm::Sym::TypeSymbol* constRefTargetType = boundCompileUnit.SymbolTable().GetTypeRepository().MakeConstReferenceType(targetType, span);
                std::vector<Cm::Core::Argument> args;
                args.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, ptrTargetType));
                args.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, constRefTargetType));
                if ((flags & OverloadResolutionFlags::bindOnlyMemberFunctions) == OverloadResolutionFlags::none)
                {
                    boundCompileUnit.DerivedTypeOpRepository().CollectViableFunctions(groupName, arity, args, boundCompileUnit.ConversionTable(), span, viableFunctions);
                    boundCompileUnit.EnumTypeOpRepository().CollectViableFunctions(groupName, arity, args, boundCompileUnit.ConversionTable(), span, viableFunctions);
                    boundCompileUnit.DelegateTypeOpRepository().CollectViableFunctions(containerScope, groupName, arity, args, boundCompileUnit.ConversionTable(), span, viableFunctions);
                }
                boundCompileUnit.ClassTemplateRepository().CollectViableFunctions(groupName, arity, args, span, containerScope, viableFunctions);
                boundCompileUnit.SynthesizedClassFunRepository().CollectViableFunctions(groupName, arity, args, span, containerScope, viableFunctions, innerException);
                boundCompileUnit.ClassDelegateTypeOpRepository().CollectViableFunctions(containerScope, groupName, arity, args, boundCompileUnit.ConversionTable(), span, viableFunctions);
            }
        }
    }
    if (viableFunctions.empty())
    {
        std::string overloadName = MakeOverloadName(groupName, arguments);
        if (GetFlag(OverloadResolutionFlags::nothrow, flags))
        {
            if (innerException)
            {
                exception.reset(innerException.release());
            }
            else
            {
                exception.reset(new Cm::Core::Exception("overload resolution failed: '" + overloadName + "' not found. No viable functions taking " + std::to_string(arity) + " arguments found in function group '" + groupName + "'.", span));
            }
            return nullptr;
        }
        else if (innerException)
        {
            Cm::Core::Exception copyOfEx = *innerException;
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
    std::vector<Cm::Core::ConceptCheckException> conceptCheckExceptions;
    for (Cm::Sym::FunctionSymbol* viableFunction : viableFunctions)
    {
        if (viableFunction->IsConvertingConstructor() || viableFunction->IsConversionFunction())
        {
            if (viableFunction->GetConversionType() == Cm::Sym::ConversionType::explicit_ && conversionType == Cm::Sym::ConversionType::implicit)
            {
                FunctionMatch functionMatch(viableFunction, containerScope, &boundCompileUnit);
                bool candidateFound = FindConversions(boundCompileUnit, viableFunction->Parameters(), arguments, Cm::Sym::ConversionType::explicit_, span, functionMatch, conversionClassTypes);
                if (candidateFound)
                {
                    mustCast = true;
                    convertingCtor = viableFunction;
                }
                continue;
            }
        }
        FunctionMatch functionMatch(viableFunction, containerScope, &boundCompileUnit);
        if (viableFunction->IsFunctionTemplate())
        {
            bool candidateFound = DeduceTypeParameters(containerScope, boundCompileUnit, viableFunction->TypeParameters(), boundTemplateArguments, viableFunction->Parameters(), arguments, span,
                conversionClassTypes, functionMatch);
            Cm::Ast::WhereConstraintNode* constraintNode = nullptr;
            if (candidateFound)
            {
                Cm::Ast::Node* node = boundCompileUnit.SymbolTable().GetNode(viableFunction, false);
                if (node)
                {
                    if (node->IsFunctionNode())
                    {
                        Cm::Ast::FunctionNode* functionNode = static_cast<Cm::Ast::FunctionNode*>(node);
                        constraintNode = functionNode->Constraint();
                        if (constraintNode)
                        {
                            if (!viableFunction->HasConstraint())
                            {
                                Cm::Ast::CloneContext cloneContext;
                                viableFunction->SetConstraint(static_cast<Cm::Ast::WhereConstraintNode*>(constraintNode->Clone(cloneContext)));
                            }
                        }
                    }
                    else
                    {
                        throw std::runtime_error("not function node");
                    }
                }
                else if (viableFunction->HasConstraint())
                {
                    constraintNode = viableFunction->GetConstraint();
                }
            }
            if (candidateFound && constraintNode)
            {
                Cm::Core::ConceptCheckException exception;
                std::unique_ptr<Cm::BoundTree::BoundConstraint> boundConstraint;
                candidateFound = CheckConstraint(containerScope, boundCompileUnit, viableFunction->GetFileScope(containerScope), constraintNode, viableFunction->TypeParameters(), 
                    functionMatch.templateArguments, exception, boundConstraint);
                if (candidateFound)
                {
                    functionMatch.constraint = constraintNode;
                    functionMatch.boundConstraint.reset(boundConstraint.release());
                }
                else
                {
                    conceptCheckExceptions.push_back(exception);
                }
            }
            if (candidateFound)
            {
                functionMatches.push_back(std::move(functionMatch));
            }
        }
        else if (boundTemplateArguments.empty())
        {
            bool candidateFound = FindConversions(boundCompileUnit, viableFunction->Parameters(), arguments, conversionType, span, functionMatch, conversionClassTypes);
            if (candidateFound)
            {
                functionMatches.push_back(std::move(functionMatch));
            }
        }
    }
    if (functionMatches.empty())
    {
        if (mustCast)
        {
            if (GetFlag(OverloadResolutionFlags::nothrow, flags))
            {
                exception.reset(new Cm::Core::Exception("overload resolution failed: cannot convert '" + arguments[1].Type()->FullName() + "' to '" + convertingCtor->GetTargetType()->FullName() + "' without a cast", span));
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
            std::string errorMessage = "overload resolution failed: '" + overloadName + "' not found, or there are no acceptable conversions for all argument types. " +
                std::to_string(viableFunctions.size()) + " viable functions examined";
            Cm::Parsing::Span conceptCheckSpan;
            if (!conceptCheckExceptions.empty())
            {
                Cm::Core::ConceptCheckException& firstEx = conceptCheckExceptions.front();
                conceptCheckSpan = firstEx.Defined();
                errorMessage.append(":\n").append(firstEx.Message());
            }
            else
            {
                errorMessage.append(".");
            }
            if (GetFlag(OverloadResolutionFlags::nothrow, flags))
            {
                exception.reset(new Cm::Core::Exception(errorMessage, span, conceptCheckSpan));
                return nullptr;
            }
            else
            {
                throw Cm::Core::Exception(errorMessage, span, conceptCheckSpan);
            }
        }
    }
    else
    {
        Cm::Sym::FunctionSymbol* function = nullptr;
        FunctionMatch bm(function, containerScope, &boundCompileUnit);
        bestMatch = std::move(bm);
        if (functionMatches.size() > 1)
        {
            BetterFunctionMatch betterFunctionMatch;
            std::sort(functionMatches.begin(), functionMatches.end(), betterFunctionMatch);
            if (betterFunctionMatch(functionMatches[0], functionMatches[1]))
            {
                bestMatch = std::move(functionMatches[0]);
                conversions = bestMatch.conversions;
                function = bestMatch.function;
            }
            else
            {
                std::string overloadName = MakeOverloadName(groupName, arguments);
                std::string matchedFunctionNames;
                bool first = true;
                FunctionMatch equalMatch = std::move(functionMatches[0]);
                std::vector<FunctionMatch> equalMatches;
                equalMatches.push_back(std::move(equalMatch));
                int n = int(functionMatches.size());
                for (int i = 1; i < n; ++i)
                {
                    FunctionMatch match = std::move(functionMatches[i]);
                    if (!betterFunctionMatch(equalMatches[0], match))
                    {
                        equalMatches.push_back(std::move(match));
                    }
                }
                std::vector<Cm::Parsing::Span> references;
                for (const FunctionMatch& match : equalMatches)
                {
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        matchedFunctionNames.append(", or ");
                    }
                    matchedFunctionNames.append(match.function->FullName());
                    if (match.function->HasConstraint())
                    {
                        matchedFunctionNames.append(1, ' ').append(match.function->GetConstraint()->ToString());
                    }
                    references.push_back(match.function->GetSpan());
                }
                if (GetFlag(OverloadResolutionFlags::nothrow, flags))
                {
                    exception.reset(new Cm::Core::Exception("overload resolution for overload name '" + overloadName + "' failed: call is ambiguous:\n" + matchedFunctionNames, span, references));
                    return nullptr;
                }
                else
                {
                    throw Cm::Core::Exception("overload resolution for overload name '" + overloadName + "' failed: call is ambiguous:\n" + matchedFunctionNames, span, references);
                }
            }
        }
        else // single best
        {
            bestMatch = std::move(functionMatches[0]);
            conversions = bestMatch.conversions;
            function = bestMatch.function;
        }
        if (function->IsSuppressed())
        {
            if (GetFlag(OverloadResolutionFlags::nothrow, flags))
            {
                exception.reset(new Cm::Core::Exception("cannot call suppressed member function", span, function->GetSpan()));
                return nullptr;
            }
            else
            {
                throw Cm::Core::Exception("cannot call suppressed member function", span, function->GetSpan());
            }
        }
        for (Cm::Sym::FunctionSymbol* conversion : conversions)
        {
            if (conversion && conversion->IsMemberOfTemplateType())
            {
                if ((flags & OverloadResolutionFlags::dontInstantiate) == OverloadResolutionFlags::none)
                {
                    boundCompileUnit.ClassTemplateRepository().Instantiate(containerScope, conversion);
                }
            }
        }
        if (function->IsFunctionTemplate())
        {
            if ((flags & OverloadResolutionFlags::dontInstantiate) != OverloadResolutionFlags::none)
            {
                return function;
            }
            function = Instantiate(boundCompileUnit.FunctionTemplateRepository(), containerScope, boundCompileUnit, function, bestMatch.templateArguments);
        }
        else if (function->IsMemberOfTemplateType())
        {
            if ((flags & OverloadResolutionFlags::dontInstantiate) != OverloadResolutionFlags::none)
            {
                return function;
            }
            if (!function->IsAbstract())
            {
                boundCompileUnit.ClassTemplateRepository().Instantiate(containerScope, function);
            }
        }
        else if (function->IsInline() && Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::optimize))
        {
            boundCompileUnit.InlineFunctionRepository().Instantiate(containerScope, function);
        }
        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::debug_heap))
        {
            if (function->FullName() == "System.Support.MemAlloc(ulong)")
            {
                function = boundCompileUnit.SymbolTable().GetOverload("System.Support.DebugHeapMemAlloc");
                if (!function)
                {
                    throw std::runtime_error("System.Support.DebugHeapMemAlloc not found");
                }
            }
            else if (function->FullName() == "System.Support.MemFree(void*)")
            {
                function = boundCompileUnit.SymbolTable().GetOverload("System.Support.DebugHeapMemFree");
                if (!function)
                {
                    throw std::runtime_error("System.Support.DebugHeapMemFree not found");
                }
            }
        }
        return function;
    }
}

} } // namespace Cm::Bind
