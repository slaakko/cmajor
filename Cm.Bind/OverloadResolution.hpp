/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_OVERLOAD_RESOLUTION_INCLUDED
#define CM_BIND_OVERLOAD_RESOLUTION_INCLUDED
#include <Cm.Bind/Concept.hpp>
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.Core/Argument.hpp>
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/ConversionTable.hpp>

namespace Cm { namespace Bind {

using Cm::Parsing::Span;

std::string MakeOverloadName(const std::string& groupName, const std::vector<Cm::Core::Argument>& arguments);

struct ArgumentMatch
{
    ArgumentMatch() : conversionRank(Cm::Sym::ConversionRank::exactMatch), conversionDistance(0)
    {
    }
    ArgumentMatch(Cm::Sym::ConversionRank conversionRank_) : conversionRank(conversionRank_), conversionDistance(0)
    {
    }
    ArgumentMatch(Cm::Sym::ConversionRank conversionRank_, const Cm::Sym::DerivationCounts& parameterDerivationCounts_, const Cm::Sym::DerivationCounts& argumentDerivationCounts_) :
        conversionRank(conversionRank_), conversionDistance(0), parameterDerivationCounts(parameterDerivationCounts_), argumentDerivationCounts(argumentDerivationCounts_)
    {
    }
    ArgumentMatch(Cm::Sym::ConversionRank conversionRank_, int conversionDistance_, const Cm::Sym::DerivationCounts& parameterDerivationCounts_, const Cm::Sym::DerivationCounts& argumentDerivationCounts_) :
        conversionRank(conversionRank_), conversionDistance(conversionDistance_), parameterDerivationCounts(parameterDerivationCounts_), argumentDerivationCounts(argumentDerivationCounts_)
    {
    }
    Cm::Sym::ConversionRank conversionRank;
    int conversionDistance;
    Cm::Sym::DerivationCounts parameterDerivationCounts;
    Cm::Sym::DerivationCounts argumentDerivationCounts;
};

struct FunctionMatch
{
    FunctionMatch() : function(nullptr), numConversions(0), containerScope(nullptr), compileUnit(nullptr), constraint(nullptr), boundConstraint(nullptr) {}
    FunctionMatch(Cm::Sym::FunctionSymbol* function_, Cm::Sym::ContainerScope* containerScope_, Cm::BoundTree::BoundCompileUnit* compileUnit_) :
        function(function_), numConversions(0), containerScope(containerScope_), compileUnit(compileUnit_), constraint(nullptr), boundConstraint(nullptr) {}
    Cm::Sym::FunctionSymbol* function;
    std::vector<ArgumentMatch> argumentMatches;
    int numConversions;
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    std::vector<Cm::Sym::TypeSymbol*> templateArguments;
    Cm::Ast::WhereConstraintNode* constraint;
    Cm::Sym::ContainerScope* containerScope;
    Cm::BoundTree::BoundCompileUnit* compileUnit;
    mutable std::unique_ptr<Cm::BoundTree::BoundConstraint> boundConstraint;
};

struct BetterFunctionMatch
{
    bool operator()(const FunctionMatch& left, const FunctionMatch& right);
};

bool FindConversions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::vector<Cm::Sym::ParameterSymbol*>& parameters, const std::vector<Cm::Core::Argument>& arguments,
    Cm::Sym::ConversionType conversionType, const Cm::Parsing::Span& span, FunctionMatch& functionMatch, std::unordered_set<Cm::Sym::ClassTypeSymbol*>& conversionClassTypes);

enum class OverloadResolutionFlags : uint8_t
{
    none = 0,
    nothrow = 1 << 0,
    bindOnlyMemberFunctions = 1 << 1,
    dontInstantiate = 1 << 2
};

inline OverloadResolutionFlags operator&(OverloadResolutionFlags left, OverloadResolutionFlags right)
{
    return OverloadResolutionFlags(uint8_t(left) & uint8_t(right));
}

inline OverloadResolutionFlags operator|(OverloadResolutionFlags left, OverloadResolutionFlags right)
{
    return OverloadResolutionFlags(uint8_t(left) | uint8_t(right));
}

inline bool GetFlag(OverloadResolutionFlags flag, OverloadResolutionFlags flags)
{
    return (flags & flag) != OverloadResolutionFlags::none;
}

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName,  
    std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions);

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName, 
    std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions, 
    OverloadResolutionFlags flags, FunctionMatch& bestMatch, std::unique_ptr<Cm::Core::Exception>& exception);

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName,
    std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions,
    OverloadResolutionFlags flags);

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName, 
    std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions, 
    Cm::Sym::ConversionType conversionType, OverloadResolutionFlags flags);

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName,
    std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions,
    Cm::Sym::ConversionType conversionType, const std::vector<Cm::Sym::TypeSymbol*>& boundTemplateArguments, OverloadResolutionFlags flags);

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName,
    std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions,
    Cm::Sym::ConversionType conversionType, const std::vector<Cm::Sym::TypeSymbol*>& boundTemplateArguments, OverloadResolutionFlags flags, FunctionMatch& bestMatch, 
    std::unique_ptr<Cm::Core::Exception>& exception);

} } // namespace Cm::Bind

#endif // CM_BIND_OVERLOAD_RESOLUTION_INCLUDED
