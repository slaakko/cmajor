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
    bcuConvertingCtor, 
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

class BcuItem
{
public:
    virtual ~BcuItem();
    virtual BcuItemType GetBcuItemType() const { return BcuItemType::bcuNull; }
    virtual void Write(BcuWriter& writer);
};

class SymbolCollection;

class BcuWriter
{
public:
    BcuWriter(const std::string& fileName_, SymbolTable* symbolTable_);
    Writer& GetSymbolWriter() { return writer; }
    BinaryWriter& GetBinaryWriter() { return writer.GetBinaryWriter(); }
    void Write(BcuItem* item);
    void Write(Symbol* symbol);
private:
    Writer writer;
};

class BcuReader
{
public:
    BcuReader(const std::string& fileName_, SymbolTable& symbolTable_);
    Reader& GetSymbolReader() { return reader; }
    BinaryReader& GetBinaryReader() { return reader.GetBinaryReader(); }
    BcuItem* ReadItem();
private:
    Reader reader;
};

} } // namespace Cm::Sym

#endif // CM_SYM_BOUND_COMPILE_UNIT_SERIALIZATION_INCLUDED
