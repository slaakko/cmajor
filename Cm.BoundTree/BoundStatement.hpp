/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BOUND_TREE_BOUND_STATEMENT_INCLUDED
#define CM_BOUND_TREE_BOUND_STATEMENT_INCLUDED
#include <Cm.BoundTree/BoundExpression.hpp>
#include <Cm.Core/Argument.hpp>
#include <Cm.Core/CDebugInfo.hpp>
#include <Cm.Sym/LocalVariableSymbol.hpp>
#include <Ir.Intf/Label.hpp>

namespace Cm { namespace BoundTree {

class BoundCompoundStatement;
class BoundFunction;

class BoundStatement : public BoundNode
{
public:
    BoundStatement(Cm::Ast::Node* syntaxNode_);
    bool IsBoundStatement() const override { return true; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    virtual bool IsConditionStatement() const { return false; }
    virtual bool IsBoundConditionalStatement() const { return false; }
    virtual bool IsBoundWhileStatement() const { return false; }
    virtual bool IsBoundDoStatement() const { return false; }
    virtual bool IsBoundForStatement() const { return false; }
    virtual bool IsBoundSwitchStatement() const { return false; }
    virtual bool IsBoundCaseStatement() const { return false; }
    virtual bool IsBoundDefaultStatement() const { return false; }
    virtual void AddBreakTargetLabel(Ir::Intf::LabelObject* breakTargetLabel) {}
    virtual void AddContinueTargetLabel(Ir::Intf::LabelObject* continueTargetLabel) {}
    virtual void AddToBreakNextSet(Cm::Core::CfgNode* node) {}
    virtual void AddToContinueNextSet(Cm::Core::CfgNode* node) {}
    BoundStatement* Parent() const { return parent; }
    void SetParent(BoundStatement* parent_) { parent = parent_; }
    BoundCompoundStatement* CompoundParent() const;
    const std::string& Label() const { return label; }
    void SetCfgNode(Cm::Core::CfgNode* cfgNode_);
    Cm::Core::CfgNode* GetCfgNode() const { return cfgNode; }
    void AddToPrevSet(Cm::Core::CfgNode* node);
    void PatchPrevSet();
private:
    BoundStatement* parent;
    std::string label;
    Cm::Core::CfgNode* cfgNode;
    std::unordered_set<Cm::Core::CfgNode*> prevSet;
};

class BoundStatementList : Cm::Sym::BcuItem
{
public:
    BoundStatementList();
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    typedef std::vector<std::unique_ptr<BoundStatement>>::iterator iterator;
    iterator begin() { return statements.begin(); }
    iterator end() { return statements.end(); }
    void AddStatement(BoundStatement* statement);
    void InsertStatement(int index, BoundStatement* statement);
    void Accept(Visitor& visitor);
    bool IsEmpty() const { return statements.empty(); }
private:
    std::vector<std::unique_ptr<BoundStatement>> statements;
};

class BoundParentStatement : public BoundStatement
{
public:
    BoundParentStatement(Cm::Ast::Node* syntaxNode_);
    virtual void AddStatement(BoundStatement* statement) = 0;
};

const uint32_t noCompoundId = -1;

class BoundCompoundStatement : public BoundParentStatement
{
public:
    BoundCompoundStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuCompoundStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void AddStatement(BoundStatement* statement) override;
    void InsertStatement(int index, BoundStatement* statement);
    bool IsBoundCompoundStatement() const override { return true; }
    void Accept(Visitor& visitor) override;
    bool IsEmpty() const { return statementList.IsEmpty(); }
    uint32_t GetCompoundId() const { return compoundId; }
    void SetCompoundId(uint32_t compoundId_) { compoundId = compoundId_; }
private:
    BoundStatementList statementList;
    uint32_t compoundId;
};

class BoundReceiveStatement : public BoundStatement
{
public:
    BoundReceiveStatement(Cm::Sym::ParameterSymbol* parameterSymbol_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuReceiveStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    Cm::Sym::ParameterSymbol* GetParameterSymbol() const { return parameterSymbol; }
    void SetConstructor(Cm::Sym::FunctionSymbol* ctor_) { ctor = ctor_; }
    Cm::Sym::FunctionSymbol* Constructor() const { return ctor; }
    void Accept(Visitor& visitor) override;
private:
    Cm::Sym::ParameterSymbol* parameterSymbol;
    Cm::Sym::FunctionSymbol* ctor;
};

class BoundInitClassObjectStatement : public BoundStatement
{
public:
    BoundInitClassObjectStatement(BoundFunctionCall* functionCall_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuInitClassObjectStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    BoundFunctionCall* FunctionCall() const { return functionCall.get(); }
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<BoundFunctionCall> functionCall;
};

class BoundInitVPtrStatement : public BoundStatement
{
public:
    BoundInitVPtrStatement(Cm::Sym::ClassTypeSymbol* classType_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuInitVPtrStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    Cm::Sym::ClassTypeSymbol* ClassType() const { return classType; }
    void Accept(Visitor& visitor) override;
private:
    Cm::Sym::ClassTypeSymbol* classType;
};

class BoundInitMemberVariableStatement : public BoundStatement
{
public:
    BoundInitMemberVariableStatement(Cm::Sym::FunctionSymbol* ctor_, BoundExpressionList&& arguments_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuInitMemberVariableStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    Cm::Sym::FunctionSymbol* Constructor() const { return ctor; }
    BoundExpressionList& Arguments() { return arguments; }
    void Accept(Visitor& visitor) override;
    void SetMemberVariableSymbol(Cm::Sym::MemberVariableSymbol* memberVarSymbol_);
    Cm::Sym::MemberVariableSymbol* GetMemberVariableSymbol() const { return memberVarSymbol; }
    void SetRegisterDestructor() { registerDestructor = true; }
    bool RegisterDestructor() const { return registerDestructor; }
private:
    Cm::Sym::MemberVariableSymbol* memberVarSymbol;
    Cm::Sym::FunctionSymbol* ctor;
    BoundExpressionList arguments;
    bool registerDestructor;
};

class BoundFunctionCallStatement : public BoundStatement
{
public:
    BoundFunctionCallStatement(Cm::Sym::FunctionSymbol* function_, BoundExpressionList&& arguments_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuFunctionCallStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    Cm::Sym::FunctionSymbol* Function() const { return function; }
    BoundExpressionList& Arguments() { return arguments; }
    void Accept(Visitor& visitor) override;
    void SetTraceCallInfo(TraceCallInfo* traceCallInfo_);
    TraceCallInfo* GetTraceCallInfo() const { return traceCallInfo.get(); }
private:
    Cm::Sym::FunctionSymbol* function;
    BoundExpressionList arguments;
    std::unique_ptr<TraceCallInfo> traceCallInfo;
};

class BoundReturnStatement : public BoundStatement
{
public:
    BoundReturnStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuReturnStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void SetExpression(BoundExpression* expression_);
    BoundExpression* Expression() const { return expression.get(); }
    void SetConstructor(Cm::Sym::FunctionSymbol* ctor_) { ctor = ctor_; }
    Cm::Sym::FunctionSymbol* Constructor() const { return ctor; }
    void Accept(Visitor& visitor) override;
    bool ReturnsValue() const { return expression != nullptr; }
    Cm::Sym::TypeSymbol* GetReturnType() const { return returnType; }
    void SetReturnType(Cm::Sym::TypeSymbol* returnType_) { returnType = returnType_; }
    void SetTraceCallInfo(TraceCallInfo* traceCallInfo_);
    TraceCallInfo* GetTraceCallInfo() const { return traceCallInfo.get(); }
    void SetBoundTemporary(Cm::BoundTree::BoundLocalVariable* boundTemporary_);
    Cm::BoundTree::BoundLocalVariable* Temporary() const { return boundTemporary.get(); }
    void SetBoundReturnValue(BoundReturnValue* returnValue);
    BoundReturnValue* ReturnValue() const { return boundReturnValue.get(); }
private:
    std::unique_ptr<BoundExpression> expression;
    Cm::Sym::FunctionSymbol* ctor;
    Cm::Sym::TypeSymbol* returnType;
    std::unique_ptr<TraceCallInfo> traceCallInfo;
    std::unique_ptr<Cm::BoundTree::BoundLocalVariable> boundTemporary;
    std::unique_ptr<Cm::BoundTree::BoundReturnValue> boundReturnValue;
};

class BoundBeginTryStatement : public BoundStatement
{
public:
    BoundBeginTryStatement(Cm::Ast::Node* syntaxNode_, int firstCatchId_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuBeginTryStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void Accept(Visitor& visitor) override;
    int FirstCatchId() const { return firstCatchId; }
private:
    int firstCatchId;
};

class BoundEndTryStatement : public BoundStatement
{
public:
    BoundEndTryStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuEndTryStatement; }
    void Accept(Visitor& visitor) override;
};

class BoundExitBlocksStatement : public BoundStatement
{
public:
    BoundExitBlocksStatement(Cm::Ast::Node* syntaxNode_, BoundCompoundStatement* targetBlock_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuExitBlockStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void Accept(Visitor& visitor) override;
    BoundCompoundStatement* TargetBlock() const { return targetBlock; }
private:
    BoundCompoundStatement* targetBlock;
};

class BoundPushGenDebugInfoStatement : public BoundStatement
{
public:
    BoundPushGenDebugInfoStatement(Cm::Ast::Node* syntaxNode_, bool generate_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuPushGenDebugInfoStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    bool Generate() const { return generate; }
    void Accept(Visitor& visitor) override;
private:
    bool generate;
};

class BoundPopGenDebugInfoStatement : public BoundStatement
{
public:
    BoundPopGenDebugInfoStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuPopGenDebugInfoStatement; }
    void Accept(Visitor& visitor) override;
};

class BoundBeginThrowStatement : public BoundStatement
{
public:
    BoundBeginThrowStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuBeginThrowStatement; }
    void Accept(Visitor& visitor) override;
};

class BoundEndThrowStatement : public BoundStatement
{
public:
    BoundEndThrowStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuEndThrowStatement; }
    void Accept(Visitor& visitor) override;
};

class BoundBeginCatchStatement : public BoundStatement
{
public:
    BoundBeginCatchStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuBeginCatchStatement; }
    void Accept(Visitor& visitor) override;
};

class BoundConstructionStatement : public BoundStatement
{
public:
    BoundConstructionStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuConstructionStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void SetLocalVariable(Cm::Sym::LocalVariableSymbol* localVariable_) { localVariable = localVariable_;  }
    Cm::Sym::LocalVariableSymbol* LocalVariable() const { return localVariable; }
    void SetArguments(BoundExpressionList&& arguments_);
    void GetResolutionArguments(Cm::Sym::TypeSymbol* localVariableType, std::vector<Cm::Core::Argument>& resolutionArguments);
    void SetConstructor(Cm::Sym::FunctionSymbol* ctor_) { ctor = ctor_; }
    Cm::Sym::FunctionSymbol* Constructor() const { return ctor; }
    void InsertLocalVariableToArguments();
    void Accept(Visitor& visitor) override;
    BoundExpressionList& Arguments() { return arguments; }
    void SetTraceCallInfo(TraceCallInfo* traceCallInfo_);
    TraceCallInfo* GetTraceCallInfo() const { return traceCallInfo.get(); }
private:
    Cm::Sym::LocalVariableSymbol* localVariable;
    BoundExpressionList arguments;
    Cm::Sym::FunctionSymbol* ctor;
    std::unique_ptr<TraceCallInfo> traceCallInfo;
};

class BoundDestructionStatement : public BoundStatement
{
public:
    BoundDestructionStatement(Cm::Ast::Node* syntaxNode_, Ir::Intf::Object* object_, Cm::Sym::FunctionSymbol* destructor_);
    void Accept(Visitor& visitor) override;
    Ir::Intf::Object* Object() const { return object; }
    Cm::Sym::FunctionSymbol* Destructor() const { return destructor; }
private:
    Ir::Intf::Object* object;
    Cm::Sym::FunctionSymbol* destructor;
};

class BoundAssignmentStatement : public BoundStatement
{
public:
    BoundAssignmentStatement(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_, Cm::Sym::FunctionSymbol* assignment_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuAssignmentStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void Accept(Visitor& visitor) override;
    Cm::BoundTree::BoundExpression* Left() const { return left.get(); }
    Cm::BoundTree::BoundExpression* Right() const { return right.get(); }
    Cm::Sym::FunctionSymbol* Assignment() const { return assignment; }
    void SetTraceCallInfo(TraceCallInfo* traceCallInfo_);
    TraceCallInfo* GetTraceCallInfo() const { return traceCallInfo.get(); }
    std::unique_ptr<BoundExpression>& RightArgument() { return right; }
private:
    std::unique_ptr<BoundExpression> left;
    std::unique_ptr<BoundExpression> right;
    Cm::Sym::FunctionSymbol* assignment;
    std::unique_ptr<TraceCallInfo> traceCallInfo;
};

class BoundSimpleStatement : public BoundStatement
{
public:
    BoundSimpleStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuSimpleStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void SetExpression(BoundExpression* expression_);
    BoundExpression* GetExpression() const { return expression.get(); }
    void Accept(Visitor& visitor) override;
    bool HasExpression() const { return expression != nullptr; }
private:
    std::unique_ptr<BoundExpression> expression;
};

class BoundSwitchStatement : public BoundParentStatement
{
public:
    BoundSwitchStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuSwitchStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void SetCondition(BoundExpression* condition_);
    BoundExpression* Condition() const { return condition.get(); }
    bool IsConditionStatement() const override { return true; }
    bool IsBoundSwitchStatement() const override { return true; }
    void AddStatement(BoundStatement* statement_) override;
    BoundStatementList& CaseStatements() { return caseStatements; }
    BoundStatement* DefaultStatement() { return defaultStatement.get(); }
    void Accept(Visitor& visitor) override;
    void AddBreakTargetLabel(Ir::Intf::LabelObject* breakTargetLabel) override;
    std::vector<Ir::Intf::LabelObject*>& BreakTargetLabels() { return breakTargetLabels; }
    void AddToBreakNextSet(Cm::Core::CfgNode* node) override;
    const std::unordered_set<Cm::Core::CfgNode*>& BreakNextSet() const { return breakNextSet; }
private:
    std::unique_ptr<BoundExpression> condition;
    BoundStatementList caseStatements;
    std::unique_ptr<BoundStatement> defaultStatement;
    std::vector<Ir::Intf::LabelObject*> breakTargetLabels;
    std::unordered_set<Cm::Core::CfgNode*> breakNextSet;
};

class BoundCaseStatement : public BoundParentStatement
{
public:
    BoundCaseStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuCaseStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    bool IsBoundCaseStatement() const override { return true; }
    void AddStatement(BoundStatement* statement_) override;
    void Accept(Visitor& visitor) override;
    void AddValue(Cm::Sym::Value* value);
    BoundStatementList& Statements() { return statements; }
    const std::vector<std::unique_ptr<Cm::Sym::Value>>& Values() const { return values; }
private:
    BoundStatementList statements;
    std::vector<std::unique_ptr<Cm::Sym::Value>> values;
};

class BoundDefaultStatement : public BoundParentStatement
{
public:
    BoundDefaultStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuDefaultStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    bool IsBoundDefaultStatement() const override { return true; }
    void AddStatement(BoundStatement* statement_) override;
    BoundStatementList& Statements() { return statements; }
    void Accept(Visitor& visitor) override;
private:
    BoundStatementList statements;
};

class BoundBreakStatement : public BoundStatement
{
public:
    BoundBreakStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuBreakStatement; }
    void Accept(Visitor& visitor) override;
};

class BoundContinueStatement : public BoundStatement
{
public:
    BoundContinueStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuContinueStatement; }
    void Accept(Visitor& visitor) override;
};

class BoundGotoStatement : public BoundStatement
{
public:
    BoundGotoStatement(Cm::Ast::Node* syntaxNode_, const std::string& targetLabel_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuGotoStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void Accept(Visitor& visitor) override;
    const std::string& TargetLabel() const { return targetLabel; }
    void SetTargetStatement(Cm::BoundTree::BoundStatement* targetStatement_) { targetStatement = targetStatement_; }
    Cm::BoundTree::BoundStatement* GetTargetStatement() const { return targetStatement; }
    void SetTargetCompoundParent(Cm::BoundTree::BoundCompoundStatement* targetCompoundParent_) { targetCompoundParent = targetCompoundParent_; }
    Cm::BoundTree::BoundCompoundStatement* GetTargetCompoundParent() const { return targetCompoundParent; }
    void SetExceptionHandlingGoto() { isExceptionHandlingGoto = true; }
    bool IsExceptionHandlingGoto() const { return isExceptionHandlingGoto; }
private:
    std::string targetLabel;
    Cm::BoundTree::BoundStatement* targetStatement;
    Cm::BoundTree::BoundCompoundStatement* targetCompoundParent;
    bool isExceptionHandlingGoto;
};

class BoundGotoCaseStatement : public BoundStatement
{
public:
    BoundGotoCaseStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuGotoCaseStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void SetValue(Cm::Sym::Value* value_);
    void Accept(Visitor& visitor) override;
    Cm::Sym::Value* Value() const { return value.get(); }
private:
    std::unique_ptr<Cm::Sym::Value> value;
};

class BoundGotoDefaultStatement : public BoundStatement
{
public:
    BoundGotoDefaultStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuGotoDefaultStatement; }
    void Accept(Visitor& visitor) override;
};

class BoundConditionalStatement : public BoundParentStatement
{
public:
    BoundConditionalStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuConditionalStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    bool IsConditionStatement() const override { return true; }
    void SetCondition(BoundExpression* condition_);
    BoundExpression* Condition() const { return condition.get(); }
    void AddStatement(BoundStatement* statement) override;
    void Accept(Visitor& visitor) override;
    bool IsBoundConditionalStatement() const override { return true; }
    BoundStatement* ThenS() const { return thenS.get(); }
    BoundStatement* ElseS() const { return elseS.get(); }
private:
    std::unique_ptr<BoundExpression> condition;
    std::unique_ptr<BoundStatement> thenS;
    std::unique_ptr<BoundStatement> elseS;
};

class BoundWhileStatement : public BoundParentStatement
{
public:
    BoundWhileStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuWhileStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    bool IsConditionStatement() const override { return true; }
    void SetCondition(BoundExpression* condition_);
    BoundExpression* Condition() const { return condition.get(); }
    void AddStatement(BoundStatement* statement_) override;
    bool IsBoundWhileStatement() const override { return true; }
    void Accept(Visitor& visitor) override;
    BoundStatement* Statement() const { return statement.get(); }
    void AddBreakTargetLabel(Ir::Intf::LabelObject* breakTargetLabel) override;
    void AddContinueTargetLabel(Ir::Intf::LabelObject* continueTargetLabel) override;
    std::vector<Ir::Intf::LabelObject*>& BreakTargetLabels() { return breakTargetLabels; }
    std::vector<Ir::Intf::LabelObject*>& ContinueTargetLabels() { return continueTargetLabels; }
    void AddToBreakNextSet(Cm::Core::CfgNode* node) override;
    const std::unordered_set<Cm::Core::CfgNode*>& BreakNextSet() const { return breakNextSet; }
    void AddToContinueNextSet(Cm::Core::CfgNode* node) override;
    const std::unordered_set<Cm::Core::CfgNode*>& ContinueNextSet() const { return continueNextSet; }
private:
    std::unique_ptr<BoundExpression> condition;
    std::unique_ptr<BoundStatement> statement;
    std::vector<Ir::Intf::LabelObject*> breakTargetLabels;
    std::vector<Ir::Intf::LabelObject*> continueTargetLabels;
    std::unordered_set<Cm::Core::CfgNode*> breakNextSet;
    std::unordered_set<Cm::Core::CfgNode*> continueNextSet;
};

class BoundDoStatement : public BoundParentStatement
{
public:
    BoundDoStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuDoStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    bool IsConditionStatement() const override { return true; }
    void SetCondition(BoundExpression* condition_);
    void AddStatement(BoundStatement* statement_) override;
    bool IsBoundDoStatement() const override { return true; }
    void Accept(Visitor& visitor) override;
    BoundStatement* Statement() const { return statement.get(); }
    BoundExpression* Condition() const { return condition.get(); }
    void AddBreakTargetLabel(Ir::Intf::LabelObject* breakTargetLabel) override;
    void AddContinueTargetLabel(Ir::Intf::LabelObject* continueTargetLabel) override;
    std::vector<Ir::Intf::LabelObject*>& BreakTargetLabels() { return breakTargetLabels; }
    std::vector<Ir::Intf::LabelObject*>& ContinueTargetLabels() { return continueTargetLabels; }
    void AddToBreakNextSet(Cm::Core::CfgNode* node) override;
    const std::unordered_set<Cm::Core::CfgNode*>& BreakNextSet() const { return breakNextSet; }
    void AddToContinueNextSet(Cm::Core::CfgNode* node) override;
    const std::unordered_set<Cm::Core::CfgNode*>& ContinueNextSet() const { return continueNextSet; }
private:
    std::unique_ptr<BoundStatement> statement;
    std::unique_ptr<BoundExpression> condition;
    std::vector<Ir::Intf::LabelObject*> breakTargetLabels;
    std::vector<Ir::Intf::LabelObject*> continueTargetLabels;
    std::unordered_set<Cm::Core::CfgNode*> breakNextSet;
    std::unordered_set<Cm::Core::CfgNode*> continueNextSet;
};

class BoundForStatement : public BoundParentStatement
{
public:
    BoundForStatement(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuForStatement; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void SetCondition(BoundExpression* condition_);
    void SetIncrement(BoundExpression* increment_);
    void AddStatement(BoundStatement* statement_) override;
    bool IsConditionStatement() const override { return true; }
    bool IsBoundForStatement() const override { return true; }
    void Accept(Visitor& visitor) override;
    BoundStatement* InitS() const { return initS.get(); }
    BoundExpression* Condition() const { return condition.get(); }
    BoundExpression* Increment() const { return increment.get(); }
    BoundStatement* Action() const { return action.get(); }
    void AddBreakTargetLabel(Ir::Intf::LabelObject* breakTargetLabel) override;
    void AddContinueTargetLabel(Ir::Intf::LabelObject* continueTargetLabel) override;
    std::vector<Ir::Intf::LabelObject*>& BreakTargetLabels() { return breakTargetLabels; }
    std::vector<Ir::Intf::LabelObject*>& ContinueTargetLabels() { return continueTargetLabels; }
    void AddToBreakNextSet(Cm::Core::CfgNode* node) override;
    const std::unordered_set<Cm::Core::CfgNode*>& BreakNextSet() const { return breakNextSet; }
    void AddToContinueNextSet(Cm::Core::CfgNode* node) override;
    const std::unordered_set<Cm::Core::CfgNode*>& ContinueNextSet() const { return continueNextSet; }
private:
    std::unique_ptr<BoundStatement> initS;
    std::unique_ptr<BoundExpression> condition;
    std::unique_ptr<BoundExpression> increment;
    std::unique_ptr<BoundStatement> action;
    std::vector<Ir::Intf::LabelObject*> breakTargetLabels;
    std::vector<Ir::Intf::LabelObject*> continueTargetLabels;
    std::unordered_set<Cm::Core::CfgNode*> breakNextSet;
    std::unordered_set<Cm::Core::CfgNode*> continueNextSet;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_BOUND_STATEMENT_INCLUDED
