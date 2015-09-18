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

namespace Cm { namespace Sym {

BcuItem::~BcuItem()
{
}

void BcuItem::Write(BcuWriter& writer)
{
}

BcuWriter::BcuWriter(const std::string& fileName_, SymbolTable* symbolTable_) : writer(fileName_, symbolTable_)
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
                return;
            }
        }
        else if (symbol->IsClassTypeSymbol())
        {
            ClassTypeSymbol* classSymbol = static_cast<ClassTypeSymbol*>(symbol);
            writer.GetBinaryWriter().Write(classSymbol->Cid());
            classSymbol->DoSerialize();
            return;
        }
        else if (symbol->IsBasicTypeSymbol())
        {
            BasicTypeSymbol* basicTypeSymbol = static_cast<BasicTypeSymbol*>(symbol);
            uint8_t itemType = uint8_t(BcuItemType::bcuBasicType);
            writer.GetBinaryWriter().Write(itemType);
            uint8_t shortTypeId = basicTypeSymbol->Id().Rep().Tag().data[0];
            writer.GetBinaryWriter().Write(shortTypeId);
            return;
        }
        writer.GetBinaryWriter().Write(symbol->Sid());
        symbol->DoSerialize();
    }
    else
    {
        uint8_t itemType = uint8_t(BcuItemType::bcuNull);
        writer.GetBinaryWriter().Write(itemType);
    }
}

BcuReader::BcuReader(const std::string& fileName_, SymbolTable& symbolTable_) : reader(fileName_, symbolTable_)
{
}

BcuItem* BcuReader::ReadItem()
{
    uint8_t itemType = reader.GetBinaryReader().ReadByte();
    if ((itemType & 0x80) != 0x00)
    {
        return nullptr;
    }
    else
    {
        // todo
    }
    return nullptr;
}

} } // namespace Cm::Sym
