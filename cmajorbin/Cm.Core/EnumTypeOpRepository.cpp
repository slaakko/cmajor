/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/EnumTypeOpRepository.hpp>
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.Sym/TypeRepository.hpp>
#include <Cm.Sym/EnumSymbol.hpp>
#include <stdexcept>

namespace Cm { namespace Core {

Cm::Sym::FunctionSymbol* EnumTypeOpCache::GetDefaultCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType)
{
    if (!defaultCtor)
    {
        defaultCtor.reset(new DefaultCtor(typeRepository, enumType));
    }
    return defaultCtor.get();
}

Cm::Sym::FunctionSymbol* EnumTypeOpCache::GetCopyCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType)
{
    if (!copyCtor)
    {
        copyCtor.reset(new CopyCtor(typeRepository, enumType));
    }
    return copyCtor.get();
}

Cm::Sym::FunctionSymbol* EnumTypeOpCache::GetMoveCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType)
{
    if (!moveCtor)
    {
        moveCtor.reset(new MoveCtor(typeRepository, enumType));
    }
    return moveCtor.get();
}

Cm::Sym::FunctionSymbol* EnumTypeOpCache::GetCopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType)
{
    if (!copyAssignment)
    {
        copyAssignment.reset(new CopyAssignment(typeRepository, enumType));
    }
    return copyAssignment.get();
}

Cm::Sym::FunctionSymbol* EnumTypeOpCache::GetMoveAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType)
{
    if (!moveAssignment)
    {
        moveAssignment.reset(new MoveAssignment(typeRepository, enumType));
    }
    return moveAssignment.get();
}

Cm::Sym::FunctionSymbol* EnumTypeOpCache::GetOpEqual(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType)
{
    if (!opEqual)
    {
        opEqual.reset(new OpEqual(typeRepository, enumType));
    }
    return opEqual.get();
}

Cm::Sym::FunctionSymbol* EnumTypeOpCache::GetOpLess(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType)
{
    if (!opLess)
    {
        opLess.reset(new OpLess(typeRepository, enumType));
    }
    return opLess.get();
}

Cm::Sym::FunctionSymbol* EnumTypeOpCache::GetConversionToUnderlyingType(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType, Cm::Sym::ConversionTable& conversionTable)
{
    if (!conversionToUnderlyingType)
    {
        Cm::Sym::EnumTypeSymbol* enumTypeSymbol = static_cast<Cm::Sym::EnumTypeSymbol*>(enumType);
        conversionToUnderlyingType.reset(new ConvertingCtor(typeRepository, enumTypeSymbol->GetUnderlyingType(), enumType, Cm::Sym::ConversionType::implicit, ConversionInst::bitcast, Cm::Sym::ConversionRank::conversion,
            100));
        conversionTable.AddConversion(conversionToUnderlyingType.get());
    }
    return conversionToUnderlyingType.get();
}

Cm::Sym::FunctionSymbol* EnumTypeOpCache::GetConversionFromUnderlyingType(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType)
{
    if (!conversionFromUnderlyingType)
    {
        Cm::Sym::EnumTypeSymbol* enumTypeSymbol = static_cast<Cm::Sym::EnumTypeSymbol*>(enumType);
        conversionFromUnderlyingType.reset(new ConvertingCtor(typeRepository, enumTypeSymbol, enumTypeSymbol->GetUnderlyingType(), Cm::Sym::ConversionType::explicit_, ConversionInst::bitcast,
            Cm::Sym::ConversionRank::conversion, 100));
    }
    return conversionFromUnderlyingType.get();
}

EnumTypeOpGroup::~EnumTypeOpGroup()
{
}

void EnumConstructorOpGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
    Cm::Sym::TypeRepository& typeRepository, EnumTypeOpCacheMap& enumTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity == 1)
    {
        Cm::Sym::TypeSymbol* type = arguments[0].Type();
        if (type->IsPointerType() && type->GetPointerCount() == 1 && type->GetBaseType()->IsEnumTypeSymbol()) // default constructor
        {
            Cm::Sym::TypeSymbol* enumType = type->GetBaseType();
            EnumTypeOpCache& cache = enumTypeOpCacheMap[enumType];
            viableFunctions.insert(cache.GetDefaultCtor(typeRepository, enumType));
        }
    }
    else if (arity == 2)
    {
        Cm::Sym::TypeSymbol* type = arguments[0].Type();
        if (type->IsPointerType() && type->GetPointerCount() == 1 && type->GetBaseType()->IsEnumTypeSymbol())
        {
            Cm::Sym::TypeSymbol* enumType = type->GetBaseType();
            Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
            if (Cm::Sym::TypesEqual(enumType, rightType)) // enum copy constructor
            {
                EnumTypeOpCache& cache = enumTypeOpCacheMap[enumType];
                viableFunctions.insert(cache.GetCopyCtor(typeRepository, enumType));
            }
            else
            {
                Cm::Sym::TypeSymbol* alternateRightType = typeRepository.MakeConstReferenceType(enumType, span);
                if (Cm::Sym::TypesEqual(alternateRightType, rightType)) // enum copy constructor
                {
                    EnumTypeOpCache& cache = enumTypeOpCacheMap[enumType];
                    viableFunctions.insert(cache.GetCopyCtor(typeRepository, enumType));
                }
                else if (Cm::Sym::TypesEqual(typeRepository.MakeReferenceType(enumType, span), rightType)) // enum copy constructor
                {
                    EnumTypeOpCache& cache = enumTypeOpCacheMap[enumType];
                    viableFunctions.insert(cache.GetCopyCtor(typeRepository, enumType));
                }
                else
                {
                    Cm::Sym::TypeSymbol* rvalueRefRightType = typeRepository.MakeRvalueRefType(enumType, span);
                    if (Cm::Sym::TypesEqual(rvalueRefRightType, rightType)) // enum move constructor
                    {
                        EnumTypeOpCache& cache = enumTypeOpCacheMap[enumType];
                        viableFunctions.insert(cache.GetMoveCtor(typeRepository, enumType));
                    }
                    else
                    {
                        if (rightType->IsIntegerTypeSymbol())
                        {
                            EnumTypeOpCache& cache = enumTypeOpCacheMap[enumType];
                            viableFunctions.insert(cache.GetConversionFromUnderlyingType(typeRepository, enumType));
                        }
                    }
                }
            }
        }
    }
}

void EnumAssignmentOpGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
    Cm::Sym::TypeRepository& typeRepository, EnumTypeOpCacheMap& enumTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 2) return;
    Cm::Sym::TypeSymbol* type = arguments[0].Type();
    if (type->IsPointerType() && type->GetPointerCount() == 1 && type->GetBaseType()->IsEnumTypeSymbol())
    {
        Cm::Sym::TypeSymbol* enumType = type->GetBaseType();
        Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
        if (Cm::Sym::TypesEqual(enumType, rightType)) // enum copy assignment
        {
            EnumTypeOpCache& cache = enumTypeOpCacheMap[enumType];
            viableFunctions.insert(cache.GetCopyAssignment(typeRepository, enumType));
        }
        else
        {
            Cm::Sym::TypeSymbol* alternateRightType = typeRepository.MakeConstReferenceType(enumType, span);
            if (Cm::Sym::TypesEqual(alternateRightType, rightType)) // enum copy assignment
            {
                EnumTypeOpCache& cache = enumTypeOpCacheMap[enumType];
                viableFunctions.insert(cache.GetCopyAssignment(typeRepository, enumType));
            }
            else if (Cm::Sym::TypesEqual(typeRepository.MakeReferenceType(enumType, span), rightType)) // enum copy assignment
            {
                EnumTypeOpCache& cache = enumTypeOpCacheMap[enumType];
                viableFunctions.insert(cache.GetCopyCtor(typeRepository, enumType));
            }
            else
            {
                Cm::Sym::TypeSymbol* rvalueRefRightType = typeRepository.MakeRvalueRefType(enumType, span);
                if (Cm::Sym::TypesEqual(rvalueRefRightType, rightType)) // enum move assignment
                {
                    EnumTypeOpCache& cache = enumTypeOpCacheMap[enumType];
                    viableFunctions.insert(cache.GetMoveAssignment(typeRepository, enumType));
                }
            }
        }
    }
    else if (type->IsPointerType() && type->GetPointerCount() == 1 && type->GetBaseType()->IsIntegerTypeSymbol())
    {
        Cm::Sym::TypeSymbol* rightType = typeRepository.MakePlainType(arguments[1].Type());
        if (rightType->IsEnumTypeSymbol())
        {
            Cm::Sym::TypeSymbol* enumType = rightType;
            EnumTypeOpCache& cache = enumTypeOpCacheMap[enumType];
            viableFunctions.insert(cache.GetConversionFromUnderlyingType(typeRepository, enumType));
        }

    }
}

void EnumEqualOpGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
    Cm::Sym::TypeRepository& typeRepository, EnumTypeOpCacheMap& enumTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 2) return;
    Cm::Sym::TypeSymbol* leftType = arguments[0].Type();
    Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
    if (leftType->IsEnumTypeSymbol() && Cm::Sym::TypesEqual(leftType, rightType))
    {
        EnumTypeOpCache& cache = enumTypeOpCacheMap[leftType];
        viableFunctions.insert(cache.GetOpEqual(typeRepository, leftType));
    }
}

void EnumLessOpGroup::CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
    Cm::Sym::TypeRepository& typeRepository, EnumTypeOpCacheMap& enumTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity != 2) return;
    Cm::Sym::TypeSymbol* leftType = arguments[0].Type();
    Cm::Sym::TypeSymbol* rightType = arguments[1].Type();
    if (leftType->IsEnumTypeSymbol() && Cm::Sym::TypesEqual(leftType, rightType))
    {
        EnumTypeOpCache& cache = enumTypeOpCacheMap[leftType];
        viableFunctions.insert(cache.GetOpLess(typeRepository, leftType));
    }
}

EnumTypeOpRepository::EnumTypeOpRepository(Cm::Sym::TypeRepository& typeRepository_) : typeRepository(typeRepository_)
{
    enumTypeOpGroupMap["@constructor"] = &enumConstructorOpGroup;
    enumTypeOpGroupMap["operator="] = &enumAssignmentOpGroup;
    enumTypeOpGroupMap["operator=="] = &enumEqualOpGroup;
    enumTypeOpGroupMap["operator<"] = &enumLessOpGroup;
}

void EnumTypeOpRepository::CollectViableFunctions(const std::string& groupName, int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
    std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (int(arguments.size()) != arity)
    {
        throw std::runtime_error("wrong number of arguments");
    }
    if (arity < 1 || arity > 2) return;
    EnumTypeOpGroupMapIt i = enumTypeOpGroupMap.find(groupName);
    if (i != enumTypeOpGroupMap.end())
    {
        EnumTypeOpGroup* group = i->second;
        group->CollectViableFunctions(arity, arguments, conversionTable, span, typeRepository, enumTypeOpCacheMap, viableFunctions);
    }
}

void EnumTypeOpRepository::InsertEnumConversionsToConversionTable(Cm::Sym::ConversionTable& conversionTable, Cm::Sym::TypeSymbol* enumType, const Cm::Parsing::Span& span)
{
    EnumTypeOpCache& cache = enumTypeOpCacheMap[enumType];
    Cm::Sym::FunctionSymbol* conversionToUnderlyingType = cache.GetConversionToUnderlyingType(typeRepository, enumType, conversionTable);
}

} } // namespace Cm::Core
