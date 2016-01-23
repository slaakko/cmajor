/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/ClassConversionTable.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/TypeRepository.hpp>
#include <Cm.Core/BasicTypeOp.hpp>

namespace Cm { namespace Core {

BaseDerivedPair::BaseDerivedPair(Cm::Sym::TypeSymbol* baseClassDerivedType_, Cm::Sym::TypeSymbol* derivedClassDerivedType_) : 
    baseClassDerivedType(baseClassDerivedType_), derivedClassDerivedType(derivedClassDerivedType_), hashCode(baseClassDerivedType->Id().GetHashCode() ^ derivedClassDerivedType->Id().GetHashCode())
{
}

bool operator==(const BaseDerivedPair& left, const BaseDerivedPair& right)
{
    return left.BaseClassDerivedType()->Id() == right.BaseClassDerivedType()->Id() && left.DerivedClassDerivedType()->Id() == right.DerivedClassDerivedType()->Id();
}

ClassConversionTable::ClassConversionTable(Cm::Sym::TypeRepository& typeRepository_) : typeRepository(typeRepository_)
{
}

Cm::Sym::FunctionSymbol* ClassConversionTable::MakeBaseClassDerivedClassConversion(Cm::Sym::TypeSymbol* baseClassDerivedType, Cm::Sym::TypeSymbol* derivedClassDerivedType, int distance, 
    const Cm::Parsing::Span& span)
{
    if (derivedClassDerivedType->IsClassTypeSymbol())
    {
        if (baseClassDerivedType->IsReferenceType())
        {
            derivedClassDerivedType = typeRepository.MakeReferenceType(derivedClassDerivedType, span);
        }
        else
        {
            derivedClassDerivedType = typeRepository.MakePointerType(derivedClassDerivedType, span);
        }
    }
    BaseDerivedPair key(baseClassDerivedType, derivedClassDerivedType);
    ClassConversionMapIt i = classConversionMap.find(key);
    if (i != classConversionMap.end())
    {
        return i->second;
    }
    Cm::Sym::FunctionSymbol* conversion = new ConvertingCtor(typeRepository, baseClassDerivedType, derivedClassDerivedType, Cm::Sym::ConversionType::implicit, ConversionInst::bitcast, Cm::Sym::ConversionRank::conversion, distance);
    conversionFunctions.push_back(std::unique_ptr<Cm::Sym::FunctionSymbol>(conversion));
    return conversion;
}

} } // namespace Cm::Core
