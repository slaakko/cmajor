/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_CONVERSION_TABLE_INCLUDED
#define CM_SYM_CONVERSION_TABLE_INCLUDED
#include <Cm.Sym\TypeSymbol.hpp>

namespace Cm { namespace Sym {

class ConversionTypes
{
public:
    ConversionTypes(TypeSymbol* sourceType_, TypeSymbol* targetType_);
    TypeSymbol* SourceType() const { return sourceType; }
    TypeSymbol* TargetType() const { return targetType; }
    size_t GetHashCode() const { return hashCode; }
private:
    TypeSymbol* sourceType;
    TypeSymbol* targetType;
    size_t hashCode;
};

bool operator==(const ConversionTypes& left, const ConversionTypes& right);

struct ConversionTypeHash
{
    size_t operator()(const ConversionTypes& conversionTypes) const { return conversionTypes.GetHashCode(); }
};

class ConversionTable
{
public:
    ConversionTable(TypeRepository& typeRepository_);
    void AddConversion(FunctionSymbol* conversion);
    FunctionSymbol* GetConversion(TypeSymbol* sourceType, TypeSymbol* targetType) const;
private:
    TypeRepository& typeRepository;
    typedef std::unordered_map<ConversionTypes, FunctionSymbol*, ConversionTypeHash> ConversionMap;
    typedef ConversionMap::const_iterator ConversionMapIt;
    ConversionMap conversionMap;
};

} } // namespace Cm::Sym

#endif // CM_SYM_CONVERSION_TABLE_INCLUDED
