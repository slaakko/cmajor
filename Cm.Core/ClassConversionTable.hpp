/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_CLASS_CONVERSION_TABLE_INCLUDED
#define CM_CORE_CLASS_CONVERSION_TABLE_INCLUDED
#include <Cm.Sym/FunctionSymbol.hpp>

namespace Cm { namespace Core {

class BaseDerivedPair
{
public:
    BaseDerivedPair(Cm::Sym::TypeSymbol* baseClassDerivedType_, Cm::Sym::TypeSymbol* derivedClassDerivedType_);
    Cm::Sym::TypeSymbol* BaseClassDerivedType() const { return baseClassDerivedType; }
    Cm::Sym::TypeSymbol* DerivedClassDerivedType() const { return derivedClassDerivedType; }
    size_t GetHashCode() const { return hashCode; }
private:
    Cm::Sym::TypeSymbol* baseClassDerivedType;
    Cm::Sym::TypeSymbol* derivedClassDerivedType;
    size_t hashCode;
};

bool operator==(const BaseDerivedPair& left, const BaseDerivedPair& right);

struct BaseDerivedPairHash
{
    size_t operator()(const BaseDerivedPair& baseDerivedPair) const { return baseDerivedPair.GetHashCode(); }
};

class ClassConversionTable
{
public:
    ClassConversionTable(Cm::Sym::TypeRepository& typeRepository_);
    Cm::Sym::FunctionSymbol* MakeBaseClassDerivedClassConversion(Cm::Sym::TypeSymbol* baseClassDerivedType, Cm::Sym::TypeSymbol* derivedClassDerivedType, int distance, const Cm::Parsing::Span& span);
private:
    Cm::Sym::TypeRepository& typeRepository;
    typedef std::unordered_map<BaseDerivedPair, Cm::Sym::FunctionSymbol*, BaseDerivedPairHash> ClassConversionMap;
    typedef ClassConversionMap::const_iterator ClassConversionMapIt;
    ClassConversionMap classConversionMap;
    std::vector<std::unique_ptr<Cm::Sym::FunctionSymbol>> conversionFunctions;
};

} } // namespace Cm::Core

#endif // CM_CORE_CLASS_CONVERSION_TABLE_INCLUDED
