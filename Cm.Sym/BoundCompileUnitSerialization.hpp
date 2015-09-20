/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_BOUND_COMPILE_UNIT_SERIALIZATION_INCLUDED
#define CM_SYM_BOUND_COMPILE_UNIT_SERIALIZATION_INCLUDED
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/Writer.hpp>

namespace Cm { namespace Sym {

enum class BcuItemType : uint8_t
{
    bcuNull, bcuBasicType, bcuDefaultCtor, bcuCopyCtor, bcuCopyAssignment, bcuMoveCtor, bcuMoveAssignment, bcuOpEqual, bcuOpLess, bcuOpAdd, bcuOpSub, bcuOpMul, bcuOpDiv, bcuOpRem, bcuOpShl, bcuOpShr,
    bcuOpBitAnd, bcuOpBitOr, bcuOpBitXor, bcuOpNot, bcuOpUnaryPlus, bcuOpUnaryMinus, bcuOpComplement, bcuOpIncrement, bcuOpDecrement, 
    bcuConvertingCtor, bcuFunctionSymbol, bcuClassSymbol, bcuOtherSymbol,
    bcuClass, bcuFunction, bcuStringLiteral, bcuLiteral, bcuConstant, bcuExceptionTableConstant, bcuEnumConstant, bcuLocalVariable, bcuExceptionCodeVariable, bcuParameter, bcuReturnValue, 
    bcuExceptionCodeParameter, bcuMemberVariable, bcuFunctionId, bcuTypeExpression, bcuNamespaceExpression, bcuConversion, bcuCast, bcuSizeOfExpression, bcuDynamicTypeNameExpression, 
    bcuUnaryOp, bcuBinaryOp, bcuPostfixIncDecExpr, bcuFunctionGroup, bcuFunctionCall, bcuDelegateCall, bcuClassDelegateCall, bcuDisjunction, bcuConjunction,
    bcuCompoundStatement, bcuReceiveStatement, bcuInitClassObjectStatement, bcuInitVPtrStatement, bcuInitMemberVariableStatement, bcuFunctionCallStatement, bcuReturnStatement, bcuBeginTryStatement,
    bcuEndTryStatement, bcuExitBlockStatement, bcuPushGenDebugInfoStatement, bcuPopGenDebugInfoStatement, bcuBeginThrowStatement, bcuEndThrowStatement, bcuBeginCatchStatement, 
    bcuConstructionStatement, bcuDestructionStatement, bcuAssignmentStatement, bcuSimpleStatement, bcuSwitchStatement, bcuCaseStatement, bcuDefaultStatement, bcuBreakStatement, bcuContinueStatement,
    bcuGotoStatement, bcuGotoCaseStatement, bcuGotoDefaultStatement, bcuConditionalStatement, bcuWhileStatement, bcuDoStatement, bcuForStatement, 
    bcuMax
};

class BcuWriter;
class BcuReader;

class BcuItem
{
public:
    virtual ~BcuItem();
    virtual BcuItemType GetBcuItemType() const { return BcuItemType::bcuNull; }
    virtual void Write(BcuWriter& writer);
    virtual void Read(BcuReader& reader);
    virtual bool IsBoundNode() const { return false; }
    virtual bool IsBoundExpression() const { return false; }
    virtual bool IsTraceCall() const { return false; }
    virtual bool IsBoundStatement() const { return false; }
    virtual bool IsBoundCompoundStatement() const { return false; }
    virtual bool IsBoundLocalVariable() const { return false; }
    virtual bool IsBoundFunctionCall() const { return false; }
    virtual bool IsBoundReturnValue() const { return false; }
};

class BcuWriter
{
public:
    BcuWriter(const std::string& fileName_, SymbolTable* symbolTable_);
    Writer& GetSymbolWriter() { return writer; }
    BinaryWriter& GetBinaryWriter() { return writer.GetBinaryWriter(); }
    void Write(BcuItem* item);
    void Write(Symbol* symbol);
    uint32_t GetCompoundId() { return nextCompoundId++; }
private:
    Writer writer;
    uint32_t nextCompoundId;
};

class TypeRepository;

class BcuBasicTypeOpSymbolFactory
{
public:
    virtual ~BcuBasicTypeOpSymbolFactory();
    virtual Symbol* CreateBasicTypeOpSymbol(BcuItemType itemType, TypeRepository& typeRepository, TypeSymbol* type) const = 0;
    virtual Symbol* CreateConvertingCtor(TypeRepository& typeRepository, TypeSymbol* targetType, TypeSymbol* sourceType) const = 0;
};

class BcuItemFactory
{
public:
    virtual ~BcuItemFactory();
    virtual BcuItem* CreateItem(BcuItemType itemType) const = 0;
};

class FunctionSymbol;
class ClassTypeSymbol;

class BoundCompileUnit
{
public:
    virtual ~BoundCompileUnit();
    virtual void Own(FunctionSymbol* fun) = 0;
};

class BcuReader
{
public:
    BcuReader(const std::string& fileName_, SymbolTable& symbolTable_, BcuBasicTypeOpSymbolFactory& basicTypeOpSymbolFactory_, BcuItemFactory& itemFactory_);
    void SetBoundCompileUnit(BoundCompileUnit* boundCompileUnit_);
    Reader& GetSymbolReader() { return reader; }
    BinaryReader& GetBinaryReader() { return reader.GetBinaryReader(); }
    BcuItem* ReadItem();
    Symbol* ReadSymbol();
    ClassTypeSymbol* ReadClassTypeSymbol();
private:
    Reader reader;
    BoundCompileUnit* boundCompileUnit;
    BcuBasicTypeOpSymbolFactory& basicTypeOpSymbolFactory;
    BcuItemFactory& itemFactory;
};

} } // namespace Cm::Sym

#endif // CM_SYM_BOUND_COMPILE_UNIT_SERIALIZATION_INCLUDED
