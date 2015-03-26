/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ConversionTable.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/TypeRepository.hpp>
#include <stdexcept>

namespace Cm { namespace Sym {

ConversionTypes::ConversionTypes(TypeSymbol* sourceType_, TypeSymbol* targetType_) : sourceType(sourceType_), targetType(targetType_), hashCode(sourceType->Id().GetHashCode() ^ targetType->Id().GetHashCode())
{
}

bool operator==(const ConversionTypes& left, const ConversionTypes& right)
{
    return left.SourceType()->Id() == right.SourceType()->Id() && left.TargetType()->Id() == right.TargetType()->Id();
}

ConversionTable::ConversionTable(TypeRepository& typeRepository_) : typeRepository(typeRepository_)
{
}

void ConversionTable::AddConversion(FunctionSymbol* conversion)
{
    if (conversion->Parameters().size() != 2)
    {
        throw std::runtime_error("conversion function parameter count != 2");
    }
    Cm::Sym::ParameterSymbol* targetParam = conversion->Parameters()[0];
    Cm::Sym::TypeSymbol* targetParamType = targetParam->GetType();
    Cm::Sym::TypeSymbol* targetType = typeRepository.MakePlainTypeWithOnePointerRemoved(targetParamType);
    Cm::Sym::ParameterSymbol* sourceParam = conversion->Parameters()[1];
    Cm::Sym::TypeSymbol* sourceParamType = sourceParam->GetType();
    Cm::Sym::TypeSymbol* sourceType = typeRepository.MakePlainType(sourceParamType);
    ConversionTypes key(sourceType, targetType);
    conversionMap[key] = conversion;
}

FunctionSymbol* ConversionTable::GetConversion(TypeSymbol* sourceType, TypeSymbol* targetType) const
{
    ConversionTypes key(sourceType, targetType);
    ConversionMapIt i = conversionMap.find(key);
    if (i != conversionMap.end())
    {
        return i->second;
    }
    return nullptr;
}


} } // namespace Cm::Sym
