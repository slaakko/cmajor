/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_OVERLOAD_RESOLUTION_INCLUDED
#define CM_BIND_OVERLOAD_RESOLUTION_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.Core/Argument.hpp>
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/ConversionTable.hpp>

namespace Cm { namespace Bind {

using Cm::Parsing::Span;

enum class OverloadResolutionFlags : uint8_t
{
    none = 0,
    nothrow = 1 << 0,
    bindOnlyMemberFunctions = 1 << 1
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

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName,  const std::vector<Cm::Core::Argument>& arguments, 
    const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions);

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName, const std::vector<Cm::Core::Argument>& arguments, 
    const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions, OverloadResolutionFlags flags);

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const std::string& groupName, const std::vector<Cm::Core::Argument>& arguments, 
    const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, std::vector<Cm::Sym::FunctionSymbol*>& conversions, Cm::Sym::ConversionType conversionType, OverloadResolutionFlags flags);


} } // namespace Cm::Bind

#endif // CM_BIND_OVERLOAD_RESOLUTION_INCLUDED
