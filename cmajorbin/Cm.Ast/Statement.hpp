/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_STATEMENT_INCLUDED
#define CM_AST_STATEMENT_INCLUDED
#include <Cm.Ast/Node.hpp>

namespace Cm {  namespace Ast {

class StatementNode;
class Visitor;

class StatementNodeList
{
public:
    typedef std::vector<std::unique_ptr<StatementNode>>::const_iterator const_iterator;
    StatementNodeList();
    const_iterator begin() const { return statementNodes.begin(); }
    const_iterator end() const { return statementNodes.end(); }
    int Count() const { return int(statementNodes.size()); }
    StatementNode* operator[](int index) const { return statementNodes[index].get(); }
    StatementNode* Back() const { return statementNodes.back().get(); }
    void Add(StatementNode* statement) { statementNodes.push_back(std::unique_ptr<StatementNode>(statement)); }
    void SetParent(Node* parent);
    void Read(Reader& reader);
    void Write(Writer& writer);
    void Print(CodeFormatter& formatter);
    void Accept(Visitor& visitor);
private:
    std::vector<std::unique_ptr<StatementNode>> statementNodes;
};

class LabelNode : public Node
{
public:
    LabelNode(const Span& span_);
    LabelNode(const Span& span_, const std::string& label_);
    NodeType GetNodeType() const override { return NodeType::labelNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override { return label + ":";  }
    std::string Label() const { return label; }
private:
    std::string label;
};

class StatementNode : public Node
{
public:
    StatementNode(const Span& span_);
    bool IsStatementNode() const override { return true; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void SetLabelNode(LabelNode* labelNode_);
    void CloneLabelTo(StatementNode* clone, CloneContext& cloneContext) const;
    LabelNode* Label() const { return labelNode.get(); }
    virtual bool IsConditionalStatementNode() const { return false; }
    virtual bool IsSwitchStatementNode() const { return false; }
    virtual bool IsCaseTerminatingNode() const { return false; }
    virtual bool IsWhileStatementNode() const { return false; }
    virtual bool IsDoStatementNode() const { return false; }
    virtual bool IsForStatementNode() const { return false; }
    virtual bool IsFunctionTerminatingNode() const { return false; }
private:
    std::unique_ptr<LabelNode> labelNode;
};

class SimpleStatementNode : public StatementNode
{
public:
    SimpleStatementNode(const Span& span_);
    SimpleStatementNode(const Span& span_, Node* expr_);
    NodeType GetNodeType() const override { return NodeType::simpleStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    bool HasExpr() const { return expr != nullptr; }
private:
    std::unique_ptr<Node> expr;
};

class ReturnStatementNode : public StatementNode 
{
public:
    ReturnStatementNode(const Span& span_);
    ReturnStatementNode(const Span& span_, Node* expr_);
    NodeType GetNodeType() const override { return NodeType::returnStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    bool ReturnsValue() const { return expr != nullptr; }
    bool IsCaseTerminatingNode() const override { return true; }
    bool IsFunctionTerminatingNode() const override { return true; }
private:
    std::unique_ptr<Node> expr;
};

class ConditionalStatementNode : public StatementNode
{
public:
    ConditionalStatementNode(const Span& span_);
    ConditionalStatementNode(const Span& span_, Node* condition_, StatementNode* thenS_, StatementNode* elseS_);
    NodeType GetNodeType() const override { return NodeType::conditionalStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    bool IsConditionalStatementNode() const override { return true; }
    Node* Condition() const { return condition.get(); }
    StatementNode* ThenS() const { return thenS.get(); }
    StatementNode* ElseS() const { return elseS.get(); }
    bool HasElseStatement() const { return elseS != nullptr; }
private:
    std::unique_ptr<Node> condition;
    std::unique_ptr<StatementNode> thenS;
    std::unique_ptr<StatementNode> elseS;
};

class CaseStatementNode;
class DefaultStatementNode;

class SwitchStatementNode : public StatementNode
{
public:
    SwitchStatementNode(const Span& span_);
    SwitchStatementNode(const Span& span_, Node* condition_);
    void AddCase(StatementNode* caseS);
    void SetDefault(StatementNode* defaultS);
    NodeType GetNodeType() const override { return NodeType::switchStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    bool IsSwitchStatementNode() const override { return true; }
    bool IsBreakEnclosingStatementNode() const override { return true; }
    Node* Condition() const { return condition.get(); }
    StatementNodeList& CaseStatements() { return caseStatements; }
    StatementNode* DefaultStatement() const { return defaultStatement.get(); }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<Node> condition;
    StatementNodeList caseStatements;
    std::unique_ptr<StatementNode> defaultStatement;
};

class CaseStatementNode : public StatementNode
{
public:
    CaseStatementNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::caseStatementNode; }
    bool IsCaseStatementNode() const override { return true; }
    void AddExpr(Node* expr);
    NodeList& Expressions() { return expressions; }
    void AddStatement(StatementNode* statement);
    StatementNodeList& Statements() { return statements; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    NodeList expressions;
    StatementNodeList statements;
};

class DefaultStatementNode : public StatementNode
{
public:
    DefaultStatementNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::defaultStatementNode; }
    bool IsDefaultStatementNode() const override { return true; }
    void AddStatement(StatementNode* statement);
    StatementNodeList& Statements() { return statements; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    StatementNodeList statements;
};

class GotoCaseStatementNode : public StatementNode
{
public:
    GotoCaseStatementNode(const Span& span_);
    GotoCaseStatementNode(const Span& span_, Node* targetCaseExpr_);
    NodeType GetNodeType() const override { return NodeType::gotoCaseStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    bool IsCaseTerminatingNode() const override { return true; }
    Node* TargetCaseExpr() const { return targetCaseExpr.get(); }
private:
    std::unique_ptr<Node> targetCaseExpr;
};

class GotoDefaultStatementNode : public StatementNode
{
public:
    GotoDefaultStatementNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::gotoDefaultStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    bool IsCaseTerminatingNode() const override { return true; }
};

class WhileStatementNode : public StatementNode
{
public:
    WhileStatementNode(const Span& span_);
    WhileStatementNode(const Span& span_, Node* condition_, StatementNode* statement_);
    NodeType GetNodeType() const override { return NodeType::whileStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    bool IsWhileStatementNode() const override { return true; }
    bool IsBreakEnclosingStatementNode() const override { return true; }
    bool IsContinueEnclosingStatementNode() const override { return true; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    Node* Condition() const { return condition.get(); }
    StatementNode* Statement() const { return statement.get(); }
private:
    std::unique_ptr<Node> condition;
    std::unique_ptr<StatementNode> statement;
};

class DoStatementNode : public StatementNode
{
public:
    DoStatementNode(const Span& span_);
    DoStatementNode(const Span& span_, StatementNode* statement_, Node* condition_);
    NodeType GetNodeType() const override { return NodeType::doStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    bool IsDoStatementNode() const override { return true; }
    bool IsBreakEnclosingStatementNode() const override { return true; }
    bool IsContinueEnclosingStatementNode() const override { return true; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    StatementNode* Statement() const { return statement.get(); }
    Node* Condition() const { return condition.get(); }
private:
    std::unique_ptr<StatementNode> statement;
    std::unique_ptr<Node> condition;
};

class ForStatementNode : public StatementNode
{
public:
    ForStatementNode(const Span& span_);
    ForStatementNode(const Span& span_, StatementNode* init_, Node* condition_, Node* increment_, StatementNode* action_);
    NodeType GetNodeType() const override { return NodeType::forStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    bool IsForStatementNode() const override { return true; }
    bool IsBreakEnclosingStatementNode() const override { return true; }
    bool IsContinueEnclosingStatementNode() const override { return true; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    bool HasCondition() const { return condition != nullptr; }
    bool HasIncrement() const { return increment != nullptr; }
    Node* Condition() const { return condition.get(); }
    StatementNode* Action() const { return action.get(); }
    void SetAsRangeForStatement() { rangeForStatement = true; }
    bool IsRangeForStatement() const { return rangeForStatement; }
private:
    std::unique_ptr<StatementNode> init;
    std::unique_ptr<Node> condition;
    std::unique_ptr<Node> increment;
    std::unique_ptr<StatementNode> action;
    bool rangeForStatement;
};

class RangeForStatementNode : public StatementNode
{
public:
    RangeForStatementNode(const Span& span_);
    RangeForStatementNode(const Span& span_, Node* varTypeExpr_, IdentifierNode* varId_, Node* container_, StatementNode* action_);
    NodeType GetNodeType() const override { return NodeType::rangeForStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    bool IsBreakEnclosingStatementNode() const override { return true; }
    bool IsContinueEnclosingStatementNode() const override { return true; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    Node* VarTypeExpr() const { return varTypeExpr.get(); }
    IdentifierNode* VarId() const { return varId.get(); }
    Node* Container() const { return container.get(); }
    StatementNode* Action() const { return action.get(); }
    Span VariableSpan() const;
private:
    std::unique_ptr<Node> varTypeExpr;
    std::unique_ptr<IdentifierNode> varId;
    std::unique_ptr<Node> container;
    std::unique_ptr<StatementNode> action;
};

class CompoundStatementNode : public StatementNode
{
public:
    CompoundStatementNode(const Span& span_);
    ~CompoundStatementNode();
    NodeType GetNodeType() const override { return NodeType::compoundStatementNode; }
    void AddStatement(StatementNode* statement);
    StatementNodeList& Statements() { return statements; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    virtual bool IsCompoundStatementNode() const { return true; }
    void Accept(Visitor& visitor) override;
    void SetBeginBraceSpan(const Span& beginBraceSpan_) { beginBraceSpan = beginBraceSpan_; }
    const Span& BeginBraceSpan() const { return beginBraceSpan; }
    void SetEndBraceSpan(const Span& endBraceSpan_) { endBraceSpan = endBraceSpan_; }
    const Span& EndBraceSpan() const { return endBraceSpan; }
private:
    StatementNodeList statements;
    Span beginBraceSpan;
    Span endBraceSpan;
};

class BreakStatementNode : public StatementNode
{
public:
    BreakStatementNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::breakStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    bool IsCaseTerminatingNode() const override { return true; }
};

class ContinueStatementNode : public StatementNode
{
public:
    ContinueStatementNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::continueStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    bool IsCaseTerminatingNode() const override { return true; }
};

class GotoStatementNode : public StatementNode
{
public:
    GotoStatementNode(const Span& span_);
    GotoStatementNode(const Span& span_, LabelNode* target_);
    NodeType GetNodeType() const override { return NodeType::gotoStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    LabelNode* Target() const { return target.get(); }
    bool IsCaseTerminatingNode() const override { return true; }
    bool IsGotoStatementNode() const override { return true; }
    bool IsExceptionHandlingGoto() const { return isExceptionHandlingGoto; }
    void SetExceptionHandlingGoto() { isExceptionHandlingGoto = true; }
private:
    std::unique_ptr<LabelNode> target;
    bool isExceptionHandlingGoto;
};

class TypedefStatementNode : public StatementNode
{
public:
    TypedefStatementNode(const Span& span_);
    TypedefStatementNode(const Span& span_, Node* typeExpr_, IdentifierNode* id_);
    NodeType GetNodeType() const override { return NodeType::typedefStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    bool IsTypedefStatementNode() const override { return true; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    Node* TypeExpr() const { return typeExpr.get(); }
    IdentifierNode* Id() const { return id.get(); }
private:
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
};

class AssignmentStatementNode : public StatementNode
{
public:
    AssignmentStatementNode(const Span& span_);
    AssignmentStatementNode(const Span& span_, Node* targetExpr_, Node* sourceExpr_);
    NodeType GetNodeType() const override { return NodeType::assignmentStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const { return targetExpr->ToString() + " = " + sourceExpr->ToString() + ";"; }
private:
    std::unique_ptr<Node> targetExpr;
    std::unique_ptr<Node> sourceExpr;
};

class ConstructionStatementNode : public StatementNode
{
public:
    ConstructionStatementNode(const Span& span_);
    ConstructionStatementNode(const Span& span_, Node* typeExpr_, IdentifierNode* id_);
    void AddArgument(Node* argument) override;
    NodeType GetNodeType() const override { return NodeType::constructionStatementNode; }
    bool IsConstructionStatementNode() const override { return true; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const;
    Node* TypeExpr() const { return typeExpr.get(); }
    IdentifierNode* Id() const { return id.get(); }
private:
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
    NodeList arguments;
};

class DeleteStatementNode : public StatementNode
{
public:
    DeleteStatementNode(const Span& span_);
    DeleteStatementNode(const Span& span_, Node* pointerExpr_);
    NodeType GetNodeType() const override { return NodeType::deleteStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    Node* PointerExpr() const { return pointerExpr.get(); }
private:
    std::unique_ptr<Node> pointerExpr;
};

class DestroyStatementNode : public StatementNode
{
public:
    DestroyStatementNode(const Span& span_);
    DestroyStatementNode(const Span& span_, Node* pointerExpr_);
    NodeType GetNodeType() const override { return NodeType::destroyStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<Node> pointerExpr;
};

class ThrowStatementNode : public StatementNode
{
public:
    ThrowStatementNode(const Span& span_);
    ThrowStatementNode(const Span& span_, Node* exceptionExpr_);
    NodeType GetNodeType() const override { return NodeType::throwStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    bool IsCaseTerminatingNode() const override { return true; }
    bool IsFunctionTerminatingNode() const override { return true; }
    Node* ExceptionExpr() const { return exceptionExpr.get(); }
private:
    std::unique_ptr<Node> exceptionExpr;
};

class CatchNode;

class TryStatementNode : public StatementNode
{
public:
    TryStatementNode(const Span& span_);
    TryStatementNode(const Span& span_, CompoundStatementNode* tryBlock_);
    NodeType GetNodeType() const override { return NodeType::tryStatementNode; }
    bool IsTryStatementNode() const override { return true; }
    void AddHandler(CatchNode* handler);
    bool IsLastHandler(Cm::Ast::CatchNode* handler);
    CatchNode* GetFirstHandler() const;
    CatchNode* GetNextHandler(CatchNode* handler);
    void SetFirstCatchId(int catchId);
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    CompoundStatementNode* TryBlock() const { return tryBlock.get(); }
    NodeList& Handlers() { return handlers; }
private:
    std::unique_ptr<CompoundStatementNode> tryBlock;
    NodeList handlers;
};

class CatchNode : public Node
{
public:
    CatchNode(const Span& span_);
    CatchNode(const Span& span_, Node* exceptionTypeExpr_, IdentifierNode* exceptionId_, CompoundStatementNode* catchBlock_);
    NodeType GetNodeType() const override { return NodeType::catchNode; }
    bool IsCatchNode() const override { return true; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    Node* ExceptionTypeExpr() const { return exceptionTypeExpr.get(); }
    IdentifierNode* ExceptionId() const { return exceptionId.get(); }
    int CatchId() const { return catchId; }
    void SetCatchId(int catchId_) { catchId = catchId_; }
    CompoundStatementNode* CatchBlock() const { return catchBlock.get(); }
private:
    std::unique_ptr<Node> exceptionTypeExpr;
    std::unique_ptr<IdentifierNode> exceptionId;
    std::unique_ptr<CompoundStatementNode> catchBlock;
    int catchId;
};

class ExitTryStatementNode : public StatementNode
{
public:
    ExitTryStatementNode(const Span& span_);
    ExitTryStatementNode(const Span& span_, TryStatementNode* tryNode_);
    TryStatementNode* TryNode() const { return tryNode; }
    NodeType GetNodeType() const override { return NodeType::exitTryNode; }
    virtual Node* Clone(CloneContext& cloneContext) const;
    void Accept(Visitor& visitor) override;
private:
    TryStatementNode* tryNode;
};

class BeginCatchStatementNode : public StatementNode
{
public:
    BeginCatchStatementNode(const Span& span_);
    NodeType GetNodeType() const override { return NodeType::beginCatchStatementNode; }
    virtual Node* Clone(CloneContext& cloneContext) const;
    void Accept(Visitor& visitor) override;
};

class AssertStatementNode : public StatementNode
{
public:
    AssertStatementNode(const Span& span_);
    AssertStatementNode(const Span& span_, Node* assertExpr_);
    NodeType GetNodeType() const override { return NodeType::assertStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    Node* AssertExpr() { return assertExpr.get(); }
private:
    std::unique_ptr<Node> assertExpr;
};

class CondCompSymbolNode : public Node
{
public:
    CondCompSymbolNode(const Span& span_);
    CondCompSymbolNode(const Span& span_, const std::string& symbol_);
    NodeType GetNodeType() const override { return NodeType::condCompSymbolNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override { return symbol; }
    virtual bool IsCondCompSymbolNode() const { return true; }
    const std::string& Str() const { return symbol; }
private:
    std::string symbol;
};

class CondCompExprNode : public Node
{
public:
    CondCompExprNode(const Span& span_);
    bool IsCondCompExprNode() const override { return true; }
    virtual bool IsCondCompDisjunctionNode() const { return false; }
    virtual bool IsCondCompBinExprNode() const { return false; }
};

class CondCompBinExprNode : public CondCompExprNode
{
public:
    CondCompBinExprNode(const Span& span_);
    CondCompBinExprNode(const Span& span_, CondCompExprNode* left_, CondCompExprNode* right_);
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    CondCompExprNode* Left() const { return left.get(); }
    CondCompExprNode* Right() const { return right.get(); }
    virtual bool IsCondCompBinExprNode() const { return true; }
private:
    std::unique_ptr<CondCompExprNode> left;
    std::unique_ptr<CondCompExprNode> right;
};

class CondCompDisjunctionNode : public CondCompBinExprNode
{
public:
    CondCompDisjunctionNode(const Span& span_);
    CondCompDisjunctionNode(const Span& span_, CondCompExprNode* left_, CondCompExprNode* right_);
    NodeType GetNodeType() const override { return NodeType::condCompDisjunctionNode; }
    virtual bool IsCondCompDisjunctionNode() const { return true; }
    Node* Clone(CloneContext& cloneContext) const override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
};

class CondCompConjunctionNode : public CondCompBinExprNode
{
public:
    CondCompConjunctionNode(const Span& span_);
    CondCompConjunctionNode(const Span& span_, CondCompExprNode* left_, CondCompExprNode* right_);
    NodeType GetNodeType() const override { return NodeType::condCompConjunctionNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
};

class CondCompNotNode : public CondCompExprNode
{
public:
    CondCompNotNode(const Span& span_);
    CondCompNotNode(const Span& span_, CondCompExprNode* subject_);
    NodeType GetNodeType() const override { return NodeType::condCompNotNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<CondCompExprNode> subject;
};

class CondCompPrimaryNode : public CondCompExprNode
{
public:
    CondCompPrimaryNode(const Span& span_);
    CondCompPrimaryNode(const Span& span_, CondCompSymbolNode* symbolNode_);
    NodeType GetNodeType() const override { return NodeType::condCompPrimaryNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    std::string ToString() const override;
    void Accept(Visitor& visitor) override;
    CondCompSymbolNode* Symbol() const { return symbolNode.get(); }
private:
    std::unique_ptr<CondCompSymbolNode> symbolNode;
};

class CondCompilationPartNode : public Node
{
public:
    CondCompilationPartNode(const Span& span_);
    CondCompilationPartNode(const Span& span_, CondCompExprNode* expr_);
    NodeType GetNodeType() const override { return NodeType::condCompilationPartNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void AddStatement(StatementNode* statement);
    bool IsCondCompPartNode() const override { return true; }
    CondCompExprNode* Expr() const { return expr.get(); }
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<CondCompExprNode> expr;
    StatementNodeList statements;
};

class CondCompilationPartNodeList
{
public:
    typedef std::vector<std::unique_ptr<CondCompilationPartNode>>::const_iterator const_iterator;
    CondCompilationPartNodeList();
    const_iterator begin() const { return partNodes.begin(); }
    const_iterator end() const { return partNodes.end(); }
    int Count() const { return int(partNodes.size()); }
    CondCompilationPartNode* operator[](int index) const { return partNodes[index].get(); }
    CondCompilationPartNode* Back() const { return partNodes.back().get(); }
    void Add(CondCompilationPartNode* part) { partNodes.push_back(std::unique_ptr<CondCompilationPartNode>(part)); }
    void Read(Reader& reader);
    void Write(Writer& writer);
    void Accept(Visitor& visitor);
    void SetParent(Node* parent);
private:
    std::vector<std::unique_ptr<CondCompilationPartNode>> partNodes;
};

class CondCompStatementNode : public StatementNode
{
public:
    CondCompStatementNode(const Span& span_);
    CondCompStatementNode(const Span& span_, CondCompExprNode* ifExpr);
    NodeType GetNodeType() const override { return NodeType::condCompStatementNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void AddIfStatement(StatementNode* ifS);
    void AddElifExpr(const Span& span, CondCompExprNode* elifExpr);
    void AddElifStatement(StatementNode* elifS);
    void AddElseStatement(const Span& span, StatementNode* elseS);
    void Accept(Visitor& visitor) override;
    CondCompilationPartNode* IfPart() const { return ifPart.get(); }
    CondCompilationPartNodeList& ElifParts() { return elifParts; }
    CondCompilationPartNode* ElsePart() const { return elsePart.get(); }
private:
    std::unique_ptr<CondCompilationPartNode> ifPart;
    CondCompilationPartNodeList elifParts;
    std::unique_ptr<CondCompilationPartNode> elsePart;
};

} } // namespace Cm::Ast

#endif // CM_AST_STATEMENT_INCLUDED
