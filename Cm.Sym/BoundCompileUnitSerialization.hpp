/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
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
    bcuConvertingCtor, 
    bcuOpAddPtrInt, bcuOpAddIntPtr, bcuOpSubPtrInt, bcuOpSubPtrPtr, bcuOpDeref, bcuOpIncPtr, bcuOpDecPtr, bcuOpAddrOf, bcuOpArrow,
    bcuPrimitiveArrayTypeOpDefaultConstructor, bcuPrimitiveArrayTypeOpCopyConstructor, bcuPrimitiveArrayTypeOpCopyAssignment, bcuArrayIndexing, 
    bcuDelegateFromFunCtor, bcuDelegateFromFunAssignment, 
    bcuClassDelegateFromFunCtor, bcuClassDelegateFromFunAssignment, bcuClassDelegateEqualOp,
    bcuFunctionSymbol, bcuClassSymbol, bcuOtherSymbol,
    bcuClass, bcuFunction, bcuStringLiteral, bcuLiteral, bcuConstant, bcuExceptionTableConstant, bcuClassHierarchyTableConstant, bcuEnumConstant, bcuLocalVariable, bcuExceptionCodeVariable, bcuParameter, bcuReturnValue,
    bcuExceptionCodeParameter, bcuMemberVariable, bcuFunctionId, bcuTypeExpression, bcuNamespaceExpression, bcuConversion, bcuCast, bcuIsExpr, bcuAsExpr, bcuSizeOfExpression, bcuDynamicTypeNameExpression,
    bcuUnaryOp, bcuBinaryOp, bcuPostfixIncDecExpr, bcuFunctionGroup, bcuFunctionCall, bcuDelegateCall, bcuClassDelegateCall, bcuDisjunction, bcuConjunction,
    bcuCompoundStatement, bcuReceiveStatement, bcuInitClassObjectStatement, bcuInitVPtrStatement, bcuInitMemberVariableStatement, bcuFunctionCallStatement, bcuReturnStatement, bcuBeginTryStatement,
    bcuEndTryStatement, bcuExitBlockStatement, bcuPushGenDebugInfoStatement, bcuPopGenDebugInfoStatement, bcuBeginThrowStatement, bcuEndThrowStatement, bcuBeginCatchStatement, 
    bcuConstructionStatement, bcuDestructionStatement, bcuAssignmentStatement, bcuSimpleStatement, bcuSwitchStatement, bcuCaseStatement, bcuDefaultStatement, bcuBreakStatement, bcuContinueStatement,
    bcuGotoStatement, bcuGotoCaseStatement, bcuGotoDefaultStatement, bcuConditionalStatement, bcuWhileStatement, bcuDoStatement, bcuForStatement, bcuTraceCallInfo,
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
    virtual bool IsBoundLiteral() const { return false; }
    virtual bool IsBoundStringLiteral() const { return false; }
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
    uint32_t GetStatementId() { return nextStatementId++; }
private:
    Writer writer;
    uint32_t nextStatementId;
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

class BcuArrayTypeOpFactory
{
public:
    virtual ~BcuArrayTypeOpFactory();
    virtual FunctionSymbol* CreateArrayTypeOp(BcuItemType itemType, TypeRepository& typeRepository, TypeSymbol* type) const = 0;
};

class DelegateTypeSymbol;

class BcuDelegateTypeOpFactory
{
public:
    virtual ~BcuDelegateTypeOpFactory();
    virtual FunctionSymbol* CreateDelegateOp(BcuItemType itemType, TypeRepository& typeRepository, TypeSymbol* delegatePtrType, DelegateTypeSymbol* delegateType, FunctionSymbol* functionSymbol) const = 0;
};

class ClassDelegateTypeSymbol;

class BcuClassDelegateTypeOpFactory
{
public:
    virtual ~BcuClassDelegateTypeOpFactory();
    virtual FunctionSymbol* CreateClassDelegateOp(BcuItemType itemType, TypeRepository& typeRepository, ClassDelegateTypeSymbol* classDelegateTypeSymbol, FunctionSymbol* functionSymbol) const = 0;
    virtual FunctionSymbol* CreateClassDelegateOpEqual(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::ClassDelegateTypeSymbol* classDelegateTypeSymbol) const = 0;
};

class FunctionSymbol;
class ClassTypeSymbol;

class BoundCompileUnit
{
public:
    virtual ~BoundCompileUnit();
    virtual void Own(FunctionSymbol* fun) = 0;
};

class CompoundTargetStatement
{
public:
    virtual ~CompoundTargetStatement();
    virtual void SetCompoundTargetStatement(void* targetStatement) = 0;
};

class LabelTargetStatement
{
public:
    virtual ~LabelTargetStatement();
    virtual void SetLabeledStatement(void* labeledStatement) = 0;
};

class BcuReader
{
public:
    BcuReader(const std::string& fileName_, SymbolTable& symbolTable_, BcuBasicTypeOpSymbolFactory& basicTypeOpSymbolFactory_, BcuItemFactory& itemFactory_, BcuArrayTypeOpFactory& arrayTypeOpFactory_,
        BcuDelegateTypeOpFactory& delegateTypeOpFactory_, BcuClassDelegateTypeOpFactory& classDelegateTypeOpFactory_);
    void SetBoundCompileUnit(BoundCompileUnit* boundCompileUnit_);
    BoundCompileUnit* GetBoundCompileUnit() const { return boundCompileUnit; }
    Reader& GetSymbolReader() { return reader; }
    BinaryReader& GetBinaryReader() { return reader.GetBinaryReader(); }
    BcuItem* ReadItem();
    Symbol* ReadSymbol();
    ClassTypeSymbol* ReadClassTypeSymbol();
    void FetchCompoundTargetStatement(CompoundTargetStatement* statement, uint32_t statementId);
    void SetCompoundTargetStatement(uint32_t statementId, void* targetStatement);
    void FetchLabeledStatement(LabelTargetStatement* statement, uint32_t statementId);
    void SetLabeledStatement(uint32_t statementId, void* labeledStatement);
    void SetClassDelegateEqualOp(FunctionSymbol* classDelegateEqualOp_) { classDelegateEqualOp = classDelegateEqualOp_; }
private:
    Reader reader;
    BoundCompileUnit* boundCompileUnit;
    BcuBasicTypeOpSymbolFactory& basicTypeOpSymbolFactory;
    BcuItemFactory& itemFactory;
    BcuArrayTypeOpFactory& arrayTypeOpFactory;
    BcuDelegateTypeOpFactory& delegateTypeOpFactory;
    BcuClassDelegateTypeOpFactory& classDelegateTypeOpFactory;
    FunctionSymbol* classDelegateEqualOp;
    std::unordered_map<uint32_t, std::forward_list<CompoundTargetStatement*>> compoundTargetStatementMap;
    std::unordered_map<uint32_t, void*> boundStatementMap;
    std::unordered_map<uint32_t, std::forward_list<LabelTargetStatement*>> labelTargetStatementMap;
    std::unordered_map<uint32_t, void*> labelStatementMap;
};

} } // namespace Cm::Sym

#endif // CM_SYM_BOUND_COMPILE_UNIT_SERIALIZATION_INCLUDED
