/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/InitSymbolTable.hpp>
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>

namespace Cm { namespace Core {

void MakeBoolOps(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::BasicTypeSymbol* boolType)
{
    symbolTable.AddPredefinedSymbolToGlobalScope(new DefaultCtor(symbolTable.GetTypeRepository(), boolType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new CopyCtor(symbolTable.GetTypeRepository(), boolType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new CopyAssignment(symbolTable.GetTypeRepository(), boolType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new MoveCtor(symbolTable.GetTypeRepository(), boolType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new MoveAssignment(symbolTable.GetTypeRepository(), boolType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpEqual(symbolTable.GetTypeRepository(), boolType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpLess(symbolTable.GetTypeRepository(), boolType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpNot(symbolTable.GetTypeRepository(), boolType));
}

void MakeIntegerOps(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::BasicTypeSymbol* integerType)
{
    symbolTable.AddPredefinedSymbolToGlobalScope(new DefaultCtor(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new CopyCtor(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new CopyAssignment(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new MoveCtor(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new MoveAssignment(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpEqual(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpLess(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpAdd(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpSub(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpMul(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpDiv(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpRem(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpShl(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpShr(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpBitAnd(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpBitOr(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpBitXor(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpUnaryPlus(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpUnaryMinus(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpComplement(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpIncrement(symbolTable.GetTypeRepository(), integerType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpDecrement(symbolTable.GetTypeRepository(), integerType));
}

void MakeFloatingPointOps(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::BasicTypeSymbol* floatingPointType)
{
    symbolTable.AddPredefinedSymbolToGlobalScope(new DefaultCtor(symbolTable.GetTypeRepository(), floatingPointType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new CopyCtor(symbolTable.GetTypeRepository(), floatingPointType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new CopyAssignment(symbolTable.GetTypeRepository(), floatingPointType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new MoveCtor(symbolTable.GetTypeRepository(), floatingPointType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new MoveAssignment(symbolTable.GetTypeRepository(), floatingPointType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpEqual(symbolTable.GetTypeRepository(), floatingPointType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpLess(symbolTable.GetTypeRepository(), floatingPointType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpAdd(symbolTable.GetTypeRepository(), floatingPointType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpSub(symbolTable.GetTypeRepository(), floatingPointType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpMul(symbolTable.GetTypeRepository(), floatingPointType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpDiv(symbolTable.GetTypeRepository(), floatingPointType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpUnaryPlus(symbolTable.GetTypeRepository(), floatingPointType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpUnaryMinus(symbolTable.GetTypeRepository(), floatingPointType));
}

void MakeCharOps(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::BasicTypeSymbol* charType)
{
    symbolTable.AddPredefinedSymbolToGlobalScope(new DefaultCtor(symbolTable.GetTypeRepository(), charType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new CopyCtor(symbolTable.GetTypeRepository(), charType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new CopyAssignment(symbolTable.GetTypeRepository(), charType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new MoveCtor(symbolTable.GetTypeRepository(), charType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new MoveAssignment(symbolTable.GetTypeRepository(), charType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpEqual(symbolTable.GetTypeRepository(), charType));
    symbolTable.AddPredefinedSymbolToGlobalScope(new OpLess(symbolTable.GetTypeRepository(), charType));
}

void MakeStandardConversions(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::SByteTypeSymbol* sbyteType, Cm::Sym::ByteTypeSymbol* byteType, 
    Cm::Sym::ShortTypeSymbol* shortType, Cm::Sym::UShortTypeSymbol* ushortType, Cm::Sym::IntTypeSymbol* intType, Cm::Sym::UIntTypeSymbol* uintType, 
    Cm::Sym::LongTypeSymbol* longType, Cm::Sym::ULongTypeSymbol* ulongType, Cm::Sym::FloatTypeSymbol* floatType, Cm::Sym::DoubleTypeSymbol* doubleType, 
    Cm::Sym::CharTypeSymbol* charType, Cm::Sym::BoolTypeSymbol* boolType)
{
    std::vector<ConvertingCtor*> conversions;
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), sbyteType, byteType, ConversionType::explicit_, ConversionInst::bitcast, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), sbyteType, shortType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), sbyteType, ushortType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), sbyteType, intType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), sbyteType, uintType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), sbyteType, longType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), sbyteType, ulongType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), sbyteType, floatType, ConversionType::explicit_, ConversionInst::fptosi, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), sbyteType, doubleType, ConversionType::explicit_, ConversionInst::fptosi, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), sbyteType, charType, ConversionType::explicit_, ConversionInst::bitcast, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), sbyteType, boolType, ConversionType::explicit_, ConversionInst::zext, ConversionRank::conversion, 100));

    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), byteType, sbyteType, ConversionType::explicit_, ConversionInst::bitcast, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), byteType, shortType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), byteType, ushortType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), byteType, intType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), byteType, uintType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), byteType, longType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), byteType, ulongType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), byteType, floatType, ConversionType::explicit_, ConversionInst::fptoui, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), byteType, doubleType, ConversionType::explicit_, ConversionInst::fptoui, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), byteType, charType, ConversionType::explicit_, ConversionInst::bitcast, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), byteType, boolType, ConversionType::explicit_, ConversionInst::zext, ConversionRank::conversion, 100));

    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), shortType, sbyteType, ConversionType::implicit, ConversionInst::sext, ConversionRank::promotion, 1));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), shortType, byteType, ConversionType::implicit, ConversionInst::zext, ConversionRank::promotion, 2));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), shortType, ushortType, ConversionType::explicit_, ConversionInst::bitcast, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), shortType, intType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), shortType, uintType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), shortType, longType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), shortType, ulongType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), shortType, floatType, ConversionType::explicit_, ConversionInst::fptosi, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), shortType, doubleType, ConversionType::explicit_, ConversionInst::fptosi, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), shortType, charType, ConversionType::explicit_, ConversionInst::zext, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), shortType, boolType, ConversionType::explicit_, ConversionInst::zext, ConversionRank::conversion, 100));

    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ushortType, sbyteType, ConversionType::explicit_, ConversionInst::sext, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ushortType, byteType, ConversionType::implicit, ConversionInst::zext, ConversionRank::promotion, 1));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ushortType, shortType, ConversionType::explicit_, ConversionInst::bitcast, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ushortType, intType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ushortType, uintType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ushortType, longType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ushortType, ulongType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ushortType, floatType, ConversionType::explicit_, ConversionInst::fptoui, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ushortType, doubleType, ConversionType::explicit_, ConversionInst::fptoui, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ushortType, charType, ConversionType::explicit_, ConversionInst::zext, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ushortType, boolType, ConversionType::explicit_, ConversionInst::zext, ConversionRank::conversion, 100));

    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), intType, sbyteType, ConversionType::implicit, ConversionInst::sext, ConversionRank::promotion, 3));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), intType, byteType, ConversionType::implicit, ConversionInst::zext, ConversionRank::promotion, 4));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), intType, shortType, ConversionType::implicit, ConversionInst::sext, ConversionRank::promotion, 1));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), intType, ushortType, ConversionType::implicit, ConversionInst::zext, ConversionRank::promotion, 2));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), intType, uintType, ConversionType::explicit_, ConversionInst::bitcast, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), intType, longType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), intType, ulongType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), intType, floatType, ConversionType::explicit_, ConversionInst::fptosi, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), intType, doubleType, ConversionType::explicit_, ConversionInst::fptosi, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), intType, charType, ConversionType::explicit_, ConversionInst::zext, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), intType, boolType, ConversionType::explicit_, ConversionInst::zext, ConversionRank::conversion, 100));

    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), uintType, sbyteType, ConversionType::explicit_, ConversionInst::sext, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), uintType, byteType, ConversionType::implicit, ConversionInst::zext, ConversionRank::promotion, 2));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), uintType, shortType, ConversionType::explicit_, ConversionInst::sext, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), uintType, ushortType, ConversionType::implicit, ConversionInst::zext, ConversionRank::promotion, 1));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), uintType, intType, ConversionType::explicit_, ConversionInst::bitcast, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), uintType, longType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), uintType, ulongType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), uintType, floatType, ConversionType::explicit_, ConversionInst::fptoui, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), uintType, doubleType, ConversionType::explicit_, ConversionInst::fptoui, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), uintType, charType, ConversionType::explicit_, ConversionInst::zext, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), uintType, boolType, ConversionType::explicit_, ConversionInst::zext, ConversionRank::conversion, 100));

    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), longType, sbyteType, ConversionType::implicit, ConversionInst::sext, ConversionRank::promotion, 5));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), longType, byteType, ConversionType::implicit, ConversionInst::zext, ConversionRank::promotion, 6));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), longType, shortType, ConversionType::implicit, ConversionInst::sext, ConversionRank::promotion, 3));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), longType, ushortType, ConversionType::implicit, ConversionInst::zext, ConversionRank::promotion, 4));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), longType, intType, ConversionType::implicit, ConversionInst::sext, ConversionRank::promotion, 1));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), longType, uintType, ConversionType::implicit, ConversionInst::zext, ConversionRank::promotion, 2));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), longType, ulongType, ConversionType::explicit_, ConversionInst::bitcast, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), longType, floatType, ConversionType::explicit_, ConversionInst::fptosi, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), longType, doubleType, ConversionType::explicit_, ConversionInst::fptosi, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), longType, charType, ConversionType::explicit_, ConversionInst::zext, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), longType, boolType, ConversionType::explicit_, ConversionInst::zext, ConversionRank::conversion, 100));

    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ulongType, sbyteType, ConversionType::explicit_, ConversionInst::sext, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ulongType, byteType, ConversionType::implicit, ConversionInst::zext, ConversionRank::promotion, 3));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ulongType, shortType, ConversionType::explicit_, ConversionInst::sext, ConversionRank::conversion, 100 ));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ulongType, ushortType, ConversionType::implicit, ConversionInst::zext, ConversionRank::promotion, 2));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ulongType, intType, ConversionType::explicit_, ConversionInst::sext, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ulongType, uintType, ConversionType::implicit, ConversionInst::zext, ConversionRank::promotion, 1));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ulongType, longType, ConversionType::explicit_, ConversionInst::bitcast, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ulongType, floatType, ConversionType::explicit_, ConversionInst::fptoui, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ulongType, doubleType, ConversionType::explicit_, ConversionInst::fptoui, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ulongType, charType, ConversionType::explicit_, ConversionInst::zext, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), ulongType, boolType, ConversionType::explicit_, ConversionInst::zext, ConversionRank::conversion, 100));

    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), floatType, sbyteType, ConversionType::implicit, ConversionInst::sitofp, ConversionRank::conversion, 5));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), floatType, byteType, ConversionType::implicit, ConversionInst::uitofp, ConversionRank::conversion, 6));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), floatType, shortType, ConversionType::implicit, ConversionInst::sitofp, ConversionRank::conversion, 3));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), floatType, ushortType, ConversionType::implicit, ConversionInst::uitofp, ConversionRank::conversion, 4));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), floatType, intType, ConversionType::implicit, ConversionInst::sitofp, ConversionRank::conversion, 1));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), floatType, uintType, ConversionType::implicit, ConversionInst::uitofp, ConversionRank::conversion, 2));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), floatType, longType, ConversionType::explicit_, ConversionInst::sitofp, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), floatType, ulongType, ConversionType::explicit_, ConversionInst::uitofp, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), floatType, doubleType, ConversionType::explicit_, ConversionInst::fptrunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), floatType, charType, ConversionType::explicit_, ConversionInst::uitofp, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), floatType, boolType, ConversionType::explicit_, ConversionInst::uitofp, ConversionRank::conversion, 100));

    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), doubleType, sbyteType, ConversionType::implicit, ConversionInst::sitofp, ConversionRank::conversion, 8));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), doubleType, byteType, ConversionType::implicit, ConversionInst::uitofp, ConversionRank::conversion, 9));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), doubleType, shortType, ConversionType::implicit, ConversionInst::sitofp, ConversionRank::conversion, 6));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), doubleType, ushortType, ConversionType::implicit, ConversionInst::uitofp, ConversionRank::conversion, 7));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), doubleType, intType, ConversionType::implicit, ConversionInst::sitofp, ConversionRank::conversion, 4));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), doubleType, uintType, ConversionType::implicit, ConversionInst::uitofp, ConversionRank::conversion, 5));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), doubleType, longType, ConversionType::implicit, ConversionInst::sitofp, ConversionRank::conversion, 2));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), doubleType, ulongType, ConversionType::implicit, ConversionInst::uitofp, ConversionRank::conversion, 3));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), doubleType, floatType, ConversionType::implicit, ConversionInst::fpext, ConversionRank::promotion, 1));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), doubleType, charType, ConversionType::explicit_, ConversionInst::uitofp, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), doubleType, boolType, ConversionType::explicit_, ConversionInst::uitofp, ConversionRank::conversion, 100));

    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), charType, sbyteType, ConversionType::explicit_, ConversionInst::bitcast, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), charType, byteType, ConversionType::explicit_, ConversionInst::bitcast, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), charType, shortType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), charType, ushortType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), charType, intType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), charType, uintType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), charType, longType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), charType, ulongType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), charType, floatType, ConversionType::explicit_, ConversionInst::fptoui, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), charType, doubleType, ConversionType::explicit_, ConversionInst::fptoui, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), charType, boolType, ConversionType::explicit_, ConversionInst::zext, ConversionRank::conversion, 100));

    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), boolType, sbyteType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), boolType, byteType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), boolType, shortType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), boolType, ushortType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), boolType, intType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), boolType, uintType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), boolType, longType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), boolType, ulongType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), boolType, floatType, ConversionType::explicit_, ConversionInst::fptoui, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), boolType, doubleType, ConversionType::explicit_, ConversionInst::fptoui, ConversionRank::conversion, 100));
    conversions.push_back(new ConvertingCtor(symbolTable.GetTypeRepository(), boolType, charType, ConversionType::explicit_, ConversionInst::trunc, ConversionRank::conversion, 100));

    for (ConvertingCtor* conversion : conversions)
    {
        symbolTable.AddPredefinedSymbolToGlobalScope(conversion);
        if (conversion->GetConversionType() == ConversionType::implicit)
        {
            symbolTable.GetStandardConversionTable().AddConversion(conversion);
        }
    }
}

void MakeBasicTypes(Cm::Sym::SymbolTable& symbolTable)
{
    std::vector<Cm::Sym::BasicTypeSymbol*> integerTypes;
    Cm::Sym::BoolTypeSymbol* boolTypeSymbol = new Cm::Sym::BoolTypeSymbol();
    symbolTable.AddPredefinedSymbolToGlobalScope(boolTypeSymbol);
    Cm::Sym::CharTypeSymbol* charTypeSymbol = new Cm::Sym::CharTypeSymbol();
    symbolTable.AddPredefinedSymbolToGlobalScope(charTypeSymbol);
    Cm::Sym::VoidTypeSymbol* voidTypeSymbol = new Cm::Sym::VoidTypeSymbol();
    symbolTable.AddPredefinedSymbolToGlobalScope(voidTypeSymbol);
    Cm::Sym::SByteTypeSymbol* sbyteTypeSymbol = new Cm::Sym::SByteTypeSymbol();
    symbolTable.AddPredefinedSymbolToGlobalScope(sbyteTypeSymbol);
    integerTypes.push_back(sbyteTypeSymbol);
    Cm::Sym::ByteTypeSymbol* byteTypeSymbol = new Cm::Sym::ByteTypeSymbol();
    symbolTable.AddPredefinedSymbolToGlobalScope(byteTypeSymbol);
    integerTypes.push_back(byteTypeSymbol);
    Cm::Sym::ShortTypeSymbol* shortTypeSymbol = new Cm::Sym::ShortTypeSymbol();
    symbolTable.AddPredefinedSymbolToGlobalScope(shortTypeSymbol);
    integerTypes.push_back(shortTypeSymbol);
    Cm::Sym::UShortTypeSymbol* ushortTypeSymbol = new Cm::Sym::UShortTypeSymbol();
    symbolTable.AddPredefinedSymbolToGlobalScope(ushortTypeSymbol);
    integerTypes.push_back(ushortTypeSymbol);
    Cm::Sym::IntTypeSymbol* intTypeSymbol = new Cm::Sym::IntTypeSymbol();
    symbolTable.AddPredefinedSymbolToGlobalScope(intTypeSymbol);
    integerTypes.push_back(intTypeSymbol);
    Cm::Sym::UIntTypeSymbol* uintTypeSymbol = new Cm::Sym::UIntTypeSymbol();
    symbolTable.AddPredefinedSymbolToGlobalScope(uintTypeSymbol);
    integerTypes.push_back(uintTypeSymbol);
    Cm::Sym::LongTypeSymbol* longTypeSymbol = new Cm::Sym::LongTypeSymbol();
    symbolTable.AddPredefinedSymbolToGlobalScope(longTypeSymbol);
    integerTypes.push_back(longTypeSymbol);
    Cm::Sym::ULongTypeSymbol* ulongTypeSymbol = new Cm::Sym::ULongTypeSymbol();
    symbolTable.AddPredefinedSymbolToGlobalScope(ulongTypeSymbol);
    integerTypes.push_back(ulongTypeSymbol);
    Cm::Sym::FloatTypeSymbol* floatTypeSymbol = new Cm::Sym::FloatTypeSymbol();
    symbolTable.AddPredefinedSymbolToGlobalScope(floatTypeSymbol);
    Cm::Sym::DoubleTypeSymbol* doubleTypeSymbol = new Cm::Sym::DoubleTypeSymbol();
    symbolTable.AddPredefinedSymbolToGlobalScope(doubleTypeSymbol);

    MakeBoolOps(symbolTable, boolTypeSymbol);
    for (Cm::Sym::BasicTypeSymbol* integerType : integerTypes)
    {
        MakeIntegerOps(symbolTable, integerType);
    }
    MakeFloatingPointOps(symbolTable, floatTypeSymbol);
    MakeFloatingPointOps(symbolTable, doubleTypeSymbol);
    MakeCharOps(symbolTable, charTypeSymbol);

    MakeStandardConversions(symbolTable, sbyteTypeSymbol, byteTypeSymbol, shortTypeSymbol, ushortTypeSymbol, intTypeSymbol, uintTypeSymbol, longTypeSymbol, ulongTypeSymbol, 
        floatTypeSymbol, doubleTypeSymbol, charTypeSymbol, boolTypeSymbol);
}

void InitSymbolTable(Cm::Sym::SymbolTable& symbolTable)
{
    MakeBasicTypes(symbolTable);
}

} } // namespace Cm::Core