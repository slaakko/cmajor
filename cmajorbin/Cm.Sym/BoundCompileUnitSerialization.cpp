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
#include <Cm.Sym/DelegateSymbol.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <iostream>

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

BcuWriter::BcuWriter(const std::string& fileName_, SymbolTable* symbolTable_) : writer(fileName_, symbolTable_), nextStatementId(0)
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
            else if (functionSymbol->IsClassDelegateEqualOp())
            {
                uint8_t itemType = uint8_t(functionSymbol->GetBcuItemType());
                writer.GetBinaryWriter().Write(itemType);
                static_cast<BcuItem*>(functionSymbol)->Write(*this);
            }
            else
            {
                uint8_t itemType = uint8_t(BcuItemType::bcuFunctionSymbol);
                writer.GetBinaryWriter().Write(itemType);
                if (symbol->Sid() == noSid)
                {
                    std::cout << symbol->FullName() << " has no sid" << std::endl;
                }
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
            if (symbol->Sid() == noSid)
            {
                int x = 0;
            }
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

BcuArrayTypeOpFactory::~BcuArrayTypeOpFactory()
{
}

BcuDelegateTypeOpFactory::~BcuDelegateTypeOpFactory()
{
}

BcuClassDelegateTypeOpFactory::~BcuClassDelegateTypeOpFactory()
{
}

BoundCompileUnit::~BoundCompileUnit()
{
}

CompoundTargetStatement::~CompoundTargetStatement()
{
}

LabelTargetStatement::~LabelTargetStatement()
{
}

BcuReader::BcuReader(const std::string& fileName_, SymbolTable& symbolTable_, BcuBasicTypeOpSymbolFactory& basicTypeOpSymbolFactory_, BcuItemFactory& itemFactory_, BcuArrayTypeOpFactory& arrayTypeOpFactory_,
    BcuDelegateTypeOpFactory& delegateTypeOpFactory_, BcuClassDelegateTypeOpFactory& classDelegateTypeOpFactory_) :
    reader(fileName_, symbolTable_), boundCompileUnit(nullptr), basicTypeOpSymbolFactory(basicTypeOpSymbolFactory_), itemFactory(itemFactory_), arrayTypeOpFactory(arrayTypeOpFactory_), 
    delegateTypeOpFactory(delegateTypeOpFactory_), classDelegateTypeOpFactory(classDelegateTypeOpFactory_), classDelegateEqualOp(nullptr)
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
            if (!functionSymbol)
            {
                throw std::runtime_error("could not get function symbol for sid " + std::to_string(sid) + " from symbol table");
            }
            return functionSymbol;
        }
        case BcuItemType::bcuClassSymbol:
        {
            uint64_t cid = reader.GetBinaryReader().ReadULong();
            Symbol* classSymbol = reader.GetSymbolTable().GetClass(cid);
            if (!classSymbol)
            {
                throw std::runtime_error("could not get class symbol for cid " + std::to_string(cid) + " from symbol table");
            }
            return classSymbol;
        }
        case BcuItemType::bcuDefaultCtor: case BcuItemType::bcuCopyCtor: case BcuItemType::bcuCopyAssignment: case BcuItemType::bcuMoveCtor: case BcuItemType::bcuMoveAssignment: 
        case BcuItemType::bcuOpEqual: case BcuItemType::bcuOpLess: case BcuItemType::bcuOpAdd: case BcuItemType::bcuOpSub: case BcuItemType::bcuOpMul: case BcuItemType::bcuOpDiv:
        case BcuItemType::bcuOpRem: case BcuItemType::bcuOpShl: case BcuItemType::bcuOpShr: case BcuItemType::bcuOpBitAnd: case BcuItemType::bcuOpBitOr: case BcuItemType::bcuOpBitXor: 
        case BcuItemType::bcuOpNot: case BcuItemType::bcuOpUnaryPlus: case BcuItemType::bcuOpUnaryMinus: case BcuItemType::bcuOpComplement: case BcuItemType::bcuOpIncrement: 
        case BcuItemType::bcuOpDecrement: case BcuItemType::bcuOpAddPtrInt: case BcuItemType::bcuOpAddIntPtr: case BcuItemType::bcuOpSubPtrInt: case BcuItemType::bcuOpSubPtrPtr:
        case BcuItemType::bcuOpDeref: case BcuItemType::bcuOpIncPtr: case BcuItemType::bcuOpDecPtr: case BcuItemType::bcuOpAddrOf: case BcuItemType::bcuOpArrow:
        {
            Symbol* typeSymbol = ReadSymbol();
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
            Symbol* targetTypeSymbol = ReadSymbol();
            if (targetTypeSymbol->IsTypeSymbol())
            {
                TypeSymbol* targetType = static_cast<TypeSymbol*>(targetTypeSymbol);
                Symbol* sourceTypeSymbol = ReadSymbol();
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
        case BcuItemType::bcuPrimitiveArrayTypeOpDefaultConstructor: case BcuItemType::bcuPrimitiveArrayTypeOpCopyConstructor: case BcuItemType::bcuPrimitiveArrayTypeOpCopyAssignment: case BcuItemType::bcuArrayIndexing:
        {
            Symbol* typeSymbol = ReadSymbol();
            if (typeSymbol->IsTypeSymbol())
            {
                TypeSymbol* type = static_cast<TypeSymbol*>(typeSymbol);
                FunctionSymbol* functionSymbol = arrayTypeOpFactory.CreateArrayTypeOp(itemType, reader.GetSymbolTable().GetTypeRepository(), type);
                if (boundCompileUnit)
                {
                    boundCompileUnit->Own(functionSymbol);
                }
                return functionSymbol;
            }
            else
            {
                throw std::runtime_error("type symbol expected");
            }
        }
        case BcuItemType::bcuDelegateFromFunCtor: case BcuItemType::bcuDelegateFromFunAssignment:
        {
            Symbol* typeSymbol = ReadSymbol();
            if (typeSymbol->IsTypeSymbol())
            {
                TypeSymbol* delegatePtrType = static_cast<TypeSymbol*>(typeSymbol);
                Symbol* delegateTypeSymbol = ReadSymbol();
                if (delegateTypeSymbol->IsDelegateTypeSymbol())
                {
                    DelegateTypeSymbol* delegateType = static_cast<DelegateTypeSymbol*>(delegateTypeSymbol);
                    Symbol* functionSymbol = ReadSymbol();
                    if (functionSymbol->IsFunctionSymbol())
                    {
                        FunctionSymbol* fun = static_cast<FunctionSymbol*>(functionSymbol);
                        boundCompileUnit->Own(fun);
                        FunctionSymbol* delegateOp = delegateTypeOpFactory.CreateDelegateOp(itemType, reader.GetSymbolTable().GetTypeRepository(), delegatePtrType, delegateType, fun);
                        boundCompileUnit->Own(delegateOp);
                        return delegateOp;
                    }
                    else
                    {
                        throw std::runtime_error("function symbol expected");
                    }
                }
                else
                {
                    throw std::runtime_error("delegate type symbol expected");
                }
            }
            else
            {
                throw std::runtime_error("type symbol expected");
            }
        }
        case BcuItemType::bcuClassDelegateFromFunCtor: case BcuItemType::bcuClassDelegateFromFunAssignment:
        {
            Symbol* typeSymbol = ReadSymbol();
            if (typeSymbol->IsClassDelegateTypeSymbol())
            {
                ClassDelegateTypeSymbol* classDelegateType = static_cast<ClassDelegateTypeSymbol*>(typeSymbol);
                Symbol* functionSymbol = ReadSymbol();
                if (!functionSymbol)
                {
                    throw std::runtime_error("got no function symbol");
                }
                if (functionSymbol->IsFunctionSymbol())
                {
                    FunctionSymbol* fun = static_cast<FunctionSymbol*>(functionSymbol);
                    boundCompileUnit->Own(fun);
                    FunctionSymbol* classDelegateOp = classDelegateTypeOpFactory.CreateClassDelegateOp(itemType, reader.GetSymbolTable().GetTypeRepository(), classDelegateType, fun);
                    boundCompileUnit->Own(classDelegateOp);
                    return classDelegateOp;
                }
                else
                {
                    throw std::runtime_error("function symbol expected");
                }
            }
            else
            {
                throw std::runtime_error("class delegate type symbol expected");
            }
        }
        case BcuItemType::bcuClassDelegateEqualOp:
        {
            Symbol* typeSymbol = ReadSymbol();
            if (typeSymbol->IsClassDelegateTypeSymbol())
            {
                ClassDelegateTypeSymbol* classDelegateType = static_cast<ClassDelegateTypeSymbol*>(typeSymbol);
                FunctionSymbol* classDelegateEqualOp = classDelegateTypeOpFactory.CreateClassDelegateOpEqual(reader.GetSymbolTable().GetTypeRepository(), classDelegateType);
                boundCompileUnit->Own(classDelegateEqualOp);
                return classDelegateEqualOp;
            }
            else
            {
                throw std::runtime_error("class delegate type symbol expected");
            }
        }
        case BcuItemType::bcuOtherSymbol:
        {
            uint32_t sid = reader.GetBinaryReader().ReadUInt();
            if (sid == noSid)
            {
                int x = 0;
            }
            Symbol* symbol = reader.GetSymbolTable().GetSymbol(sid);
            if (!symbol)
            {
                throw std::runtime_error("could not get symbol for sid " + std::to_string(sid) + " from symbol table");
            }
            return symbol;
        }
        case BcuItemType::bcuNull:
        {
            return nullptr;
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
    if (symbol)
    {
        if (symbol->IsClassTypeSymbol())
        {
            return static_cast<ClassTypeSymbol*>(symbol);
        }
        else
        {
            throw std::runtime_error("class type symbol expected");
        }
    }
    else
    {
        throw std::runtime_error("null class type symbol read");
    }
}

void BcuReader::FetchCompoundTargetStatement(CompoundTargetStatement* statement, uint32_t statementId)
{
    std::unordered_map<uint32_t, void*>::const_iterator i = boundStatementMap.find(statementId);
    if (i != boundStatementMap.end())
    {
        void* targetStatement = i->second;
        statement->SetCompoundTargetStatement(targetStatement);
    }
    else
    {
        compoundTargetStatementMap[statementId].push_front(statement);
    }
}

void BcuReader::SetCompoundTargetStatement(uint32_t statementId, void* targetStatement)
{
    boundStatementMap[statementId] = targetStatement;
    std::unordered_map<uint32_t, std::forward_list<CompoundTargetStatement*>>::iterator i = compoundTargetStatementMap.find(statementId);
    if (i != compoundTargetStatementMap.end())
    {
        std::forward_list<CompoundTargetStatement*>& targets = i->second;
        for (CompoundTargetStatement* target : targets)
        {
            target->SetCompoundTargetStatement(targetStatement);
        }
    }
}

void BcuReader::FetchLabeledStatement(LabelTargetStatement* statement, uint32_t statementId)
{
    std::unordered_map<uint32_t, void*>::const_iterator i = labelStatementMap.find(statementId);
    if (i != labelStatementMap.end())
    {
        void* labeledStatement = i->second;
        statement->SetLabeledStatement(labeledStatement);
    }
    else
    {
        labelTargetStatementMap[statementId].push_front(statement);
    }
}

void BcuReader::SetLabeledStatement(uint32_t statementId, void* labeledStatement)
{
    labelStatementMap[statementId] = labeledStatement;
    std::unordered_map<uint32_t, std::forward_list<LabelTargetStatement*>>::iterator i = labelTargetStatementMap.find(statementId);
    if (i != labelTargetStatementMap.end())
    {
        std::forward_list<LabelTargetStatement*>& targets = i->second;
        for (LabelTargetStatement* target : targets)
        {
            target->SetLabeledStatement(labeledStatement);
        }
    }
}

} } // namespace Cm::Sym
