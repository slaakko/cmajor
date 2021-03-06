/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/Symbol.hpp>
#include <Cm.Sym/Factory.hpp>
#include <Cm.Sym/TypeId.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>

namespace Cm { namespace Sym {

Reader::Reader(const std::string& fileName, SymbolTable& symbolTable_) : binaryReader(fileName), astReader(binaryReader), symbolTable(symbolTable_), spanFileIndexOffset(0), markSymbolsBound(false),
    markSymbolsProject(false), markTemplateTypeSymbolsBound(false), fetchCidForVirtualClasses(false)
{
}

void Reader::SetSpanFileIndexOffset(int spanFileIndexOffset_)
{
    spanFileIndexOffset = spanFileIndexOffset_;
}

void Reader::MarkSymbolsBound()
{
    markSymbolsBound = true;
}

void Reader::MarkTemplateTypeSymbolsBound()
{
    markTemplateTypeSymbolsBound = true;
}

void Reader::MarkSymbolsProject()
{
    markSymbolsProject = true;
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
    if (spanFileIndexOffset > 0)
    {
        span.SetFileIndex(span.FileIndex() + spanFileIndexOffset);
    }
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
    value->Read(*this);
    return value;
}

TypeId Reader::ReadTypeId()
{
    TypeId typeId;
    binaryReader.Read(typeId.Rep(), typeIdRepSize);
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
        if (markSymbolsBound)
        {
            symbol->SetBound();
        }
        return symbol;
    }
    else
    {
        Span span = ReadSpan();
        std::string name = binaryReader.ReadString();
        Symbol* symbol = SymbolFactory::Instance().CreateSymbol(symbolType, span, name);
        if (markSymbolsBound)
        {
            symbol->SetBound();
        }
        symbol->Read(*this);
        if (markSymbolsProject)
        {
            symbol->SetProject();
        }
        if (symbol->IsTemplateTypeSymbol() && !symbol->IsReplica() && !markTemplateTypeSymbolsBound)
        {
            symbol->ResetFlag(SymbolFlags::bound);
        }
        symbolTable.AddSymbol(symbol);
        if (symbol->IsClassTypeSymbol())
        {
            ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(symbol);
            if (classTypeSymbol->IsVirtual() || !classTypeSymbol->ImplementedInterfaces().empty())
            {
                initVTableAndITableSet.insert(classTypeSymbol);
                if (fetchCidForVirtualClasses)
                {
                    uint64_t cid = symbolTable.GetVirtualClassCid(classTypeSymbol->FullName());
                    classTypeSymbol->SetCid(cid);
                }
            }
        }
        return symbol;
    }
}

void Reader::EnqueueMakeIrTypeFor(Symbol* symbol)
{
    makeIrTypeSet.insert(symbol);
}

void Reader::MakeIrTypes()
{
    for (Symbol* symbol : makeIrTypeSet)
    {
        symbol->MakeIrType();
    }
}

void Reader::InitVTablesAndITables()
{
    for (ClassTypeSymbol* classType : initVTableAndITableSet)
    {
        classType->InitItbls();
        classType->InitVtbl();
    }
}

bool Reader::AllTypesFetched()
{
    FetchTypeMapIt e = fetchTypeMap.end();
    for (FetchTypeMapIt i = fetchTypeMap.begin(); i != e; ++i)
    {
        FetchTypeList& ftl = i->second;
        if (!ftl.empty())
        {
            return false;
        }
    }
    return true;
}

} } // namespace Cm::Sym
