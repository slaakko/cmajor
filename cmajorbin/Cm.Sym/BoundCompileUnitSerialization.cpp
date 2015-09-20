/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/BoundCompileUnitSerialization.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/SymbolTable.hpp>

namespace Cm { namespace Sym {

BcuItem::~BcuItem()
{
}

void BcuItem::Write(BcuWriter& writer)
{
}

void BcuItem::Read(BcuReader& reader)
{
}

BcuWriter::BcuWriter(const std::string& fileName_, SymbolTable* symbolTable_) : writer(fileName_, symbolTable_), nextCompoundId(0)
{
}

void BcuWriter::Write(BcuItem* item)
{
    if (item)
    {
        uint8_t itemType = uint8_t(item->GetBcuItemType());
        writer.GetBinaryWriter().Write(itemType);
        item->Write(*this);
    }
    else // null item
    {
        uint8_t itemType = uint8_t(BcuItemType::bcuNull);
        writer.GetBinaryWriter().Write(itemType);
    }
}

void BcuWriter::Write(Symbol* symbol)
{
    if (symbol)
    {
        if (symbol->IsFunctionSymbol())
        {
            FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(symbol);
            if (functionSymbol->IsBasicTypeOp())
            {
                uint8_t itemType = uint8_t(functionSymbol->GetBcuItemType());
                writer.GetBinaryWriter().Write(itemType);
                static_cast<BcuItem*>(functionSymbol)->Write(*this);
            }
            else
            {
                uint8_t itemType = uint8_t(BcuItemType::bcuFunctionSymbol);
                writer.GetBinaryWriter().Write(itemType);
                writer.GetBinaryWriter().Write(symbol->Sid());
                symbol->DoSerialize();
            }
        }
        else if (symbol->IsClassTypeSymbol())
        {
            uint8_t itemType = uint8_t(BcuItemType::bcuClassSymbol);
            writer.GetBinaryWriter().Write(itemType);
            ClassTypeSymbol* classSymbol = static_cast<ClassTypeSymbol*>(symbol);
            writer.GetBinaryWriter().Write(classSymbol->Cid());
            classSymbol->DoSerialize();
        }
        else if (symbol->IsBasicTypeSymbol())
        {
            BasicTypeSymbol* basicTypeSymbol = static_cast<BasicTypeSymbol*>(symbol);
            uint8_t itemType = uint8_t(BcuItemType::bcuBasicType);
            writer.GetBinaryWriter().Write(itemType);
            uint8_t shortTypeId = basicTypeSymbol->Id().Rep().Tag().data[0];
            writer.GetBinaryWriter().Write(shortTypeId);
        }
        else
        {
            uint8_t itemType = uint8_t(BcuItemType::bcuOtherSymbol);
            writer.GetBinaryWriter().Write(itemType);
            writer.GetBinaryWriter().Write(symbol->Sid());
            symbol->DoSerialize();
        }
    }
    else
    {
        uint8_t itemType = uint8_t(BcuItemType::bcuNull);
        writer.GetBinaryWriter().Write(itemType);
    }
}

BcuBasicTypeOpSymbolFactory::~BcuBasicTypeOpSymbolFactory()
{
}

BcuItemFactory::~BcuItemFactory()
{
}

BoundCompileUnit::~BoundCompileUnit()
{
}

BcuReader::BcuReader(const std::string& fileName_, SymbolTable& symbolTable_, BcuBasicTypeOpSymbolFactory& basicTypeOpSymbolFactory_, BcuItemFactory& itemFactory_) :
    reader(fileName_, symbolTable_), boundCompileUnit(nullptr), basicTypeOpSymbolFactory(basicTypeOpSymbolFactory_), itemFactory(itemFactory_)
{
}

void BcuReader::SetBoundCompileUnit(BoundCompileUnit* boundCompileUnit_)
{
    boundCompileUnit = boundCompileUnit_;
}

BcuItem* BcuReader::ReadItem()
{
    BcuItemType itemType = BcuItemType(reader.GetBinaryReader().ReadByte());
    if (itemType == BcuItemType::bcuNull)
    {
        return nullptr;
    }
    else
    {
        BcuItem* item = itemFactory.CreateItem(itemType);
        item->Read(*this);
        return item;
    }
}

Symbol* BcuReader::ReadSymbol()
{
    BcuItemType itemType = BcuItemType(reader.GetBinaryReader().ReadByte());
    switch (itemType)
    {
        case BcuItemType::bcuBasicType:
        {
            ShortBasicTypeId shortBasicTypeId = static_cast<ShortBasicTypeId>(reader.GetBinaryReader().ReadByte());
            TypeId typeId(GetBasicTypeId(shortBasicTypeId));
            Symbol* typeSymbol = reader.GetSymbolTable().GetTypeRepository().GetType(typeId);
            return typeSymbol;
        }
        case BcuItemType::bcuFunctionSymbol:
        {
            uint32_t sid = reader.GetBinaryReader().ReadUInt();
            Symbol* functionSymbol = reader.GetSymbolTable().GetSymbol(sid);
            return functionSymbol;
        }
        case BcuItemType::bcuClassSymbol:
        {
            uint32_t cid = reader.GetBinaryReader().ReadUInt();
            Symbol* classSymbol = reader.GetSymbolTable().GetClass(cid);
            return classSymbol;
        }
        case BcuItemType::bcuDefaultCtor: case BcuItemType::bcuCopyCtor: case BcuItemType::bcuCopyAssignment: case BcuItemType::bcuMoveCtor: case BcuItemType::bcuMoveAssignment: 
        case BcuItemType::bcuOpEqual: case BcuItemType::bcuOpLess: case BcuItemType::bcuOpAdd: case BcuItemType::bcuOpSub: case BcuItemType::bcuOpMul: case BcuItemType::bcuOpDiv:
        case BcuItemType::bcuOpRem: case BcuItemType::bcuOpShl: case BcuItemType::bcuOpShr: case BcuItemType::bcuOpBitAnd: case BcuItemType::bcuOpBitOr: case BcuItemType::bcuOpBitXor: 
        case BcuItemType::bcuOpNot: case BcuItemType::bcuOpUnaryPlus: case BcuItemType::bcuOpUnaryMinus: case BcuItemType::bcuOpComplement: case BcuItemType::bcuOpIncrement: 
        case BcuItemType::bcuOpDecrement: 
        {
            uint32_t sid = reader.GetBinaryReader().ReadUInt();
            Symbol* typeSymbol = reader.GetSymbolTable().GetSymbol(sid);
            if (typeSymbol->IsTypeSymbol())
            {
                TypeSymbol* type = static_cast<TypeSymbol*>(typeSymbol);
                Symbol* symbol = basicTypeOpSymbolFactory.CreateBasicTypeOpSymbol(itemType, reader.GetSymbolTable().GetTypeRepository(), type);
                if (symbol->IsFunctionSymbol())
                {
                    FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(symbol);
                    if (boundCompileUnit)
                    {
                        boundCompileUnit->Own(functionSymbol);
                    }
                }
                return symbol;
            }
            else
            {
                throw std::runtime_error("type symbol expected");
            }
        }
        case BcuItemType::bcuConvertingCtor:
        {
            uint32_t targetSid = reader.GetBinaryReader().ReadUInt();
            Symbol* targetTypeSymbol = reader.GetSymbolTable().GetSymbol(targetSid);
            if (targetTypeSymbol->IsTypeSymbol())
            {
                TypeSymbol* targetType = static_cast<TypeSymbol*>(targetTypeSymbol);
                uint32_t sourceSid = reader.GetBinaryReader().ReadUInt();
                Symbol* sourceTypeSymbol = reader.GetSymbolTable().GetSymbol(sourceSid);
                if (sourceTypeSymbol->IsTypeSymbol())
                {
                    TypeSymbol* sourceType = static_cast<TypeSymbol*>(sourceTypeSymbol);
                    Symbol* symbol = basicTypeOpSymbolFactory.CreateConvertingCtor(reader.GetSymbolTable().GetTypeRepository(), targetType, sourceType);
                    static_cast<BcuItem*>(symbol)->Read(*this);
                    if (symbol->IsFunctionSymbol())
                    {
                        FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(symbol);
                        if (boundCompileUnit)
                        {
                            boundCompileUnit->Own(functionSymbol);
                        }
                    }
                    return symbol;
                }
                else
                {
                    throw std::runtime_error("type symbol expected");
                }
            }
            else
            {
                throw std::runtime_error("type symbol expected");
            }
        }
        case BcuItemType::bcuOtherSymbol:
        {
            uint32_t sid = reader.GetBinaryReader().ReadUInt();
            Symbol* symbol = reader.GetSymbolTable().GetSymbol(sid);
            return symbol;
        }
        default:
        {
            throw std::runtime_error("bcu symbol expected");
        }
    }
}

ClassTypeSymbol* BcuReader::ReadClassTypeSymbol()
{
    Symbol* symbol = ReadSymbol();
    if (symbol->IsClassTypeSymbol())
    {
        return static_cast<ClassTypeSymbol*>(symbol);
    }
    else
    {
        throw std::runtime_error("class type symbol expected");
    }
}

} } // namespace Cm::Sym
