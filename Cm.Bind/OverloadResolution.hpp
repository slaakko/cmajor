/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_OVERLOAD_RESOLUTION_INCLUDED
#define CM_BIND_OVERLOAD_RESOLUTION_INCLUDED
#include <Cm.Core/Argument.hpp>
#include <Cm.Core/ClassConversionTable.hpp>
#include <Cm.Core/PointerOpRepository.hpp>
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/ConversionTable.hpp>

namespace Cm { namespace Bind {

using Cm::Parsing::Span;

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ConversionTable& conversionTable, Cm::Core::ClassConversionTable& classConversionTable, 
    Cm::Core::PointerOpRepository &pointerOpRepository, const std::string& groupName,  const std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, 
    std::vector<Cm::Sym::FunctionSymbol*>& conversions);

Cm::Sym::FunctionSymbol* ResolveOverload(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ConversionTable& conversionTable, Cm::Core::ClassConversionTable& classConversionTable, 
    Cm::Core::PointerOpRepository& pointerOpRepository, const std::string& groupName, const std::vector<Cm::Core::Argument>& arguments, const Cm::Sym::FunctionLookupSet& functionLookups, const Span& span, 
    Cm::Core::ConversionType conversionType, std::vector<Cm::Sym::FunctionSymbol*>& conversions);


} } // namespace Cm::Bind

#endif // CM_BIND_OVERLOAD_RESOLUTION_INCLUDED
