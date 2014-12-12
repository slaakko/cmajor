/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/Symbol.hpp>
#include <Cm.Sym/Factory.hpp>
#include <Cm.Sym/TypeId.hpp>
#include <Cm.Sym/SymbolTable.hpp>

namespace Cm { namespace Sym {

Reader::Reader(const std::string& fileName, SymbolTable& symbolTable_) : binaryReader(fileName), symbolTable(symbolTable_)
{
}

SymbolType Reader::ReadSymbolType()
{
    uint8_t st = binaryReader.ReadByte();
    return SymbolType(st);
}

Span Reader::ReadSpan()
{
    Span span;
    binaryReader.Read(&span, sizeof(span));
    return span;
}

Cm::Ast::Derivation Reader::ReadDerivation()
{
    uint8_t d = binaryReader.ReadByte();
    return *reinterpret_cast<Cm::Ast::Derivation*>(&d);
}

Cm::Ast::DerivationList Reader::ReadDerivationList()
{
    Cm::Ast::DerivationList derivationList;
    uint8_t numDerivations = binaryReader.ReadByte();
    for (uint8_t i = 0; i < numDerivations; ++i)
    {
        derivationList.Add(ReadDerivation());
    }
    return derivationList;
}

ValueType Reader::ReadValueType()
{
    ValueType valueType = ValueType(binaryReader.ReadByte());
    return valueType;
}

Value* Reader::ReadValue()
{
    ValueType valueType = ReadValueType();
    Value* value = ValueFactory::Instance().CreateValue(valueType);
    value->Read(binaryReader);
    return value;
}

TypeId Reader::ReadTypeId()
{
    TypeId typeId;
    binaryReader.Read(&typeId.Rep(), sizeof(Cm::Util::Uuid));
    return typeId;
}

void Reader::FetchTypeFor(Symbol* symbol, int index)
{
    TypeId typeId = ReadTypeId();
    TypeSymbol* type = symbolTable.GetTypeRepository().GetTypeNothrow(typeId);
    if (type)
    {
        symbol->SetType(type, index);
    }
    else
    {
        fetchTypeMap[typeId].push_front(std::make_pair(symbol, index));
    }
}

void Reader::BackpatchType(TypeSymbol* type)
{
    symbolTable.GetTypeRepository().AddType(type);
    FetchTypeMapIt i = fetchTypeMap.find(type->Id());
    if (i != fetchTypeMap.end())
    {
        FetchTypeList& fetchTypeList = i->second;
        while (!fetchTypeList.empty())
        {
            std::pair<Symbol*, int> p = fetchTypeList.front();
            fetchTypeList.pop_front();
            Symbol* symbol = p.first;
            int index = p.second;
            symbol->SetType(type, index);
        }
        fetchTypeList.clear();
    }
}

Symbol* Reader::ReadSymbol()
{
    SymbolType symbolType = ReadSymbolType();
    if (IsBasicSymbolType(symbolType))
    {
        Symbol* symbol = SymbolFactory::Instance().CreateBasicTypeSymbol(symbolType);
        return symbol;
    }
    else
    {
        Span span = ReadSpan();
        std::string name = binaryReader.ReadString();
        Symbol* symbol = SymbolFactory::Instance().CreateSymbol(symbolType, span, name);
        symbol->Read(*this);
        return symbol;
    }
}

} } // namespace Cm::Sym
