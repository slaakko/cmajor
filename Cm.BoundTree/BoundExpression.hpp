/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BOUND_TREE_BOUND_EXPRESSION_INCLUDED
#define CM_BOUND_TREE_BOUND_EXPRESSION_INCLUDED
#include <Cm.BoundTree/BoundNode.hpp>
#include <Cm.Core/Argument.hpp>
#include <Cm.Core/CDebugInfo.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/ConstantSymbol.hpp>
#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Sym/LocalVariableSymbol.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Sym/ReturnValueSymbol.hpp>
#include <Cm.Sym/Value.hpp>
#include <Cm.Sym/ParameterSymbol.hpp>
#include <Cm.Sym/DelegateSymbol.hpp>

namespace Cm { namespace BoundTree {

class BoundStatement;

class BoundExpression : public BoundNode
{
public:
    BoundExpression(Cm::Ast::Node* syntaxNode_);
    bool IsBoundExpressionNode() const override { return true; }
    bool IsBoundExpression() const override { return true; }
    virtual bool IsBoundNamespaceExpression() const { return false; }
    virtual bool IsBoundFunctionGroup() const { return false; }
    virtual bool IsBoundMemberVariable() const { return false; }
    virtual bool IsBoundUnaryOp() const { return false; }
    virtual bool IsBoundBinaryOp() const { return false; }
    virtual bool IsBoundPostfixIncDecExpr() const { return false; }
    virtual bool IsBoundTypeExpression() const { return false; }
	virtual bool IsBoundParameter() const { return false; }
    virtual bool IsBoundExceptionCodeVariable() const { return false; }
    virtual bool IsBoundExceptionCodeParameter() const { return false; }
    virtual bool IsBoundConversion() const { return false; }
    virtual bool IsLiteral() const { return false; }
    virtual bool IsBoundNullLiteral() const { return false; }
    virtual bool IsConstant() const { return false; }
    virtual bool IsBoundExceptionTableConstant() const { return false; }
    virtual bool IsEnumConstant() const { return false; }
    virtual bool IsBoundCast() const { return false; }
    virtual void SetType(Cm::Sym::TypeSymbol* type_) { type = type_;  }
    Cm::Sym::TypeSymbol* GetType() const { return type; }
    virtual Cm::Core::ArgumentCategory GetArgumentCategory() const { return Cm::Core::ArgumentCategory::rvalue; }
    void SetCfgNode(Cm::Core::CfgNode* cfgNode_);
    Cm::Core::CfgNode* GetCfgNode() const { return cfgNode; }
private:
    Cm::Sym::TypeSymbol* type;
    Cm::Core::CfgNode* cfgNode;
};

class TraceCallInfo : public Cm::Sym::BcuItem
{
public:
    TraceCallInfo(BoundExpression* fun_, BoundExpression* file_, BoundExpression* line_);
    bool IsTraceCall() const override { return true; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    BoundExpression* Fun() const { return fun.get(); }
    BoundExpression* File() const { return file.get(); }
    BoundExpression* Line() const { return line.get(); }
private:
    std::unique_ptr<BoundExpression> fun;
    std::unique_ptr<BoundExpression> file;
    std::unique_ptr<BoundExpression> line;
};

class BoundExpressionList : public Cm::Sym::BcuItem
{
public:
    BoundExpressionList();
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    typedef std::vector<std::unique_ptr<BoundExpression>>::iterator iterator;
    iterator begin() { return expressions.begin(); }
    iterator end() { return expressions.end(); }
    BoundExpressionList(BoundExpressionList&& that) = default;
    BoundExpressionList& operator=(BoundExpressionList&& that) = default;
    void Add(BoundExpression* expression);
    bool Empty() const { return expressions.empty(); }
    int Count() const { return int(expressions.size()); }
    std::unique_ptr<BoundExpression>& operator[](int index) { return expressions[index]; }
    void InsertFront(BoundExpression* expr);
    BoundExpression* RemoveFirst();
    BoundExpression* GetLast();
    void Reverse();
    void Accept(Visitor& visitor);
private:
    std::vector<std::unique_ptr<BoundExpression>> expressions;
};

class BoundStringLiteral : public BoundExpression
{
public:
    BoundStringLiteral(Cm::Ast::Node* syntaxNode_, int id_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuStringLiteral; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    int Id() const { return id; }
    void Accept(Visitor& visitor) override;
private:
    int id;
};

class BoundLiteral : public BoundExpression
{
public:
    BoundLiteral(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuLiteral; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void SetValue(Cm::Sym::Value* value_) { value.reset(value_); }
    Cm::Sym::Value* GetValue() const { return value.get(); }
    void Accept(Visitor& visitor) override;
    bool IsLiteral() const override { return true; }
    bool IsBoundNullLiteral() const override { return value->IsNull(); }
    void SetType(Cm::Sym::TypeSymbol* type_) override;
private:
    std::unique_ptr<Cm::Sym::Value> value;
};

class BoundConstant : public BoundExpression
{
public:
    BoundConstant(Cm::Ast::Node* syntaxNode_, Cm::Sym::ConstantSymbol* symbol_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuConstant; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    Cm::Sym::ConstantSymbol* Symbol() const { return symbol; }
    void Accept(Visitor& visitor) override;
    bool IsConstant() const override { return true; }
private:
    Cm::Sym::ConstantSymbol* symbol;
};

class BoundExceptionTableConstant : public BoundConstant
{
public:
    BoundExceptionTableConstant(Cm::Ast::Node* syntaxNode_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuExceptionTableConstant; }
    bool IsBoundExceptionTableConstant() const override { return true; }
};

class BoundEnumConstant : public BoundExpression
{
public:
    BoundEnumConstant(Cm::Ast::Node* syntaxNode_, Cm::Sym::EnumConstantSymbol* symbol_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuEnumConstant; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    Cm::Sym::EnumConstantSymbol* Symbol() const { return symbol; }
    void Accept(Visitor& visitor) override;
    bool IsEnumConstant() const override { return true; }
private:
    Cm::Sym::EnumConstantSymbol* symbol;
};

class BoundLocalVariable : public BoundExpression
{
public:
    BoundLocalVariable(Cm::Ast::Node* syntaxNode_, Cm::Sym::LocalVariableSymbol* symbol_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuLocalVariable; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    Cm::Sym::LocalVariableSymbol* Symbol() const { return symbol; }
    Cm::Core::ArgumentCategory GetArgumentCategory() const override { return Cm::Core::ArgumentCategory::lvalue; }
    void Accept(Visitor& visitor) override;
    bool IsBoundLocalVariable() const override { return true; }
private:
    Cm::Sym::LocalVariableSymbol* symbol;
};

class BoundExceptionCodeVariable : public BoundLocalVariable
{
public:
    BoundExceptionCodeVariable();
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuExceptionCodeVariable; }
    bool IsBoundExceptionCodeVariable() const override { return true; }
};

class BoundParameter: public BoundExpression
{
public:
    BoundParameter(Cm::Ast::Node* syntaxNode_, Cm::Sym::ParameterSymbol* symbol_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuParameter; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    Cm::Sym::ParameterSymbol* Symbol() const { return symbol; }
    Cm::Core::ArgumentCategory GetArgumentCategory() const override;
    void Accept(Visitor& visitor) override;
	bool IsBoundParameter() const override { return true; }
private:
    Cm::Sym::ParameterSymbol* symbol;
};

class BoundReturnValue : public BoundExpression
{
public:
    BoundReturnValue(Cm::Ast::Node* syntaxNode_, Cm::Sym::ReturnValueSymbol* symbol_);
    bool IsBoundReturnValue() const override { return true; }
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuReturnValue; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    Cm::Sym::ReturnValueSymbol* Symbol() const { return symbol; }
    void Accept(Visitor& visitor) override;
private:
    Cm::Sym::ReturnValueSymbol* symbol;
};

class BoundExceptionCodeParameter : public BoundParameter
{
public:
    BoundExceptionCodeParameter();
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuExceptionCodeParameter; }
    bool IsBoundExceptionCodeParameter() const override { return true; }
};

class BoundMemberVariable : public BoundExpression
{
public:
    BoundMemberVariable(Cm::Ast::Node* syntaxNode_, Cm::Sym::MemberVariableSymbol* symbol_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuMemberVariable; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    bool IsBoundMemberVariable() const override { return true; }
    Cm::Sym::MemberVariableSymbol* Symbol() const { return symbol; }
    Cm::Core::ArgumentCategory GetArgumentCategory() const override { return Cm::Core::ArgumentCategory::lvalue; }
    void Accept(Visitor& visitor) override;
    void SetClassObject(Cm::BoundTree::BoundExpression* classObject_);
    Cm::BoundTree::BoundExpression* GetClassObject() const { return classObject.get(); }
private:
    std::unique_ptr<Cm::BoundTree::BoundExpression> classObject;
    Cm::Sym::MemberVariableSymbol* symbol;
};

class BoundFunctionId : public BoundExpression
{
public:
    BoundFunctionId(Cm::Ast::Node* syntaxNode_, Cm::Sym::FunctionSymbol* functionSymbol_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuFunctionId; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    Cm::Sym::FunctionSymbol* FunctionSymbol() const { return functionSymbol; }
    void Accept(Visitor& visitor) override;
private:
    Cm::Sym::FunctionSymbol* functionSymbol;
};

class BoundTypeExpression : public BoundExpression
{
public:
    BoundTypeExpression(Cm::Ast::Node* syntaxNode_, Cm::Sym::TypeSymbol* typeSymbol_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuTypeExpression; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    bool IsBoundTypeExpression() const override { return true; }
    Cm::Sym::TypeSymbol* Symbol() const { return typeSymbol; }
    void Accept(Visitor& visitor) override;
private:
    Cm::Sym::TypeSymbol* typeSymbol;
};

class BoundNamespaceExpression : public BoundExpression
{
public:
    BoundNamespaceExpression(Cm::Ast::Node* syntaxNode_, Cm::Sym::NamespaceSymbol* namespaceSymbol_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuNamespaceExpression; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    Cm::Sym::NamespaceSymbol* NamespaceSymbol() const { return namespaceSymbol; }
    bool IsBoundNamespaceExpression() const override { return true; }
    void Accept(Visitor& visitor) override;
private:
    Cm::Sym::NamespaceSymbol* namespaceSymbol;
};

class BoundConversion : public BoundExpression
{
public:
    BoundConversion(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_, Cm::Sym::FunctionSymbol* conversionFun_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuConversion; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    bool IsBoundConversion() const override { return true; }
    BoundExpression* Operand() const { return operand.get(); }
    BoundExpression* ReleaseOperand() { return operand.release(); }
    void ResetOperand(BoundExpression* operand_) { operand.reset(operand_); }
    Cm::Sym::FunctionSymbol* ConversionFun() const { return conversionFun; }
    void Accept(Visitor& visitor) override;
    void SetBoundTemporary(Cm::BoundTree::BoundExpression* boundTemporary_);
    Cm::BoundTree::BoundExpression* BoundTemporary() const { return boundTemporary.get(); }
private:
    std::unique_ptr<BoundExpression> operand;
    Cm::Sym::FunctionSymbol* conversionFun;
    std::unique_ptr<Cm::BoundTree::BoundExpression> boundTemporary;
};

class BoundCast : public BoundExpression
{
public:
    BoundCast(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_, Cm::Sym::FunctionSymbol* conversionFun_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuCast; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    Cm::Sym::FunctionSymbol* ConversionFun() const { return conversionFun; }
    void Accept(Visitor& visitor) override;
    bool IsBoundCast() const override { return true; }
    BoundExpression* Operand() const { return operand.get(); }
    void SetSourceType(Cm::Sym::TypeSymbol* sourceType_) { sourceType = sourceType_; }
    Cm::Sym::TypeSymbol* GetSourceType() const { return sourceType; }
private:
    std::unique_ptr<BoundExpression> operand;
    Cm::Sym::FunctionSymbol* conversionFun;
    Cm::Sym::TypeSymbol* sourceType;
};

class BoundSizeOfExpression : public BoundExpression
{
public:
    BoundSizeOfExpression(Cm::Ast::Node* syntaxNode_, Cm::Sym::TypeSymbol* type_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuSizeOfExpression; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void Accept(Visitor& visitor) override;
    Cm::Sym::TypeSymbol* Type() const { return type; }
private:
    Cm::Sym::TypeSymbol* type;
};

class BoundDynamicTypeNameExpression : public BoundExpression
{
public:
    BoundDynamicTypeNameExpression(Cm::Ast::Node* syntaxNode_, BoundExpression* subject_, Cm::Sym::ClassTypeSymbol* classType_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuDynamicTypeNameExpression; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void Accept(Visitor& visitor) override;
    BoundExpression* Subject() const { return subject.get(); }
    Cm::Sym::ClassTypeSymbol* ClassType() const { return classType; }
private:
    std::unique_ptr<BoundExpression> subject;
    Cm::Sym::ClassTypeSymbol* classType;
};

class BoundUnaryOp : public BoundExpression
{
public:
    BoundUnaryOp(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuUnaryOp; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    bool IsBoundUnaryOp() const override { return true; }
    void SetFunction(Cm::Sym::FunctionSymbol* fun_) { fun = fun_; }
    Cm::Sym::FunctionSymbol* GetFunction() const { return fun; }
    BoundExpression* Operand() const { return operand.get(); }
    BoundExpression* ReleaseOperand() { return operand.release(); }
    void Accept(Visitor& visitor) override;
    void SetClassObjectResultVar(Cm::Sym::LocalVariableSymbol* classObjectResultVar_) { classObjectResultVar = classObjectResultVar_; }
    Cm::Sym::LocalVariableSymbol* GetClassObjectResultVar() const { return classObjectResultVar; }
    void SetTraceCallInfo(TraceCallInfo* traceCallInfo_);
    TraceCallInfo* GetTraceCallInfo() const { return traceCallInfo.get(); }
    Cm::Core::ArgumentCategory GetArgumentCategory() const override { return argumentCategory; }
    void SetArgumentCategory(Cm::Core::ArgumentCategory argumentCategory_) { argumentCategory = argumentCategory_; }
private:
    std::unique_ptr<BoundExpression> operand;
    Cm::Sym::FunctionSymbol* fun;
    Cm::Sym::LocalVariableSymbol* classObjectResultVar;
    std::unique_ptr<TraceCallInfo> traceCallInfo;
    Cm::Core::ArgumentCategory argumentCategory;
};

class BoundBinaryOp : public BoundExpression
{
public:
    BoundBinaryOp(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuBinaryOp; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    bool IsBoundBinaryOp() const override { return true; }
    BoundExpression* Left() const { return left.get(); }
    BoundExpression* Right() const { return right.get(); }
    void SetFunction(Cm::Sym::FunctionSymbol* fun_) { fun = fun_; }
    Cm::Sym::FunctionSymbol* GetFunction() const { return fun; }
    void Accept(Visitor& visitor) override;
    void SetClassObjectResultVar(Cm::Sym::LocalVariableSymbol* classObjectResultVar_) { classObjectResultVar = classObjectResultVar_; }
    Cm::Sym::LocalVariableSymbol* GetClassObjectResultVar() const { return classObjectResultVar; }
    void SetTraceCallInfo(TraceCallInfo* traceCallInfo_);
    TraceCallInfo* GetTraceCallInfo() const { return traceCallInfo.get(); }
private:
    std::unique_ptr<BoundExpression> left;
    std::unique_ptr<BoundExpression> right;
    Cm::Sym::FunctionSymbol* fun;
    Cm::Sym::LocalVariableSymbol* classObjectResultVar;
    std::unique_ptr<TraceCallInfo> traceCallInfo;
};

class BoundPostfixIncDecExpr : public BoundExpression
{
public:
    BoundPostfixIncDecExpr(Cm::Ast::Node* syntaxNode_, BoundExpression* value_, BoundStatement* statement_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuPostfixIncDecExpr; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    BoundExpression* Value() const { return value.get(); }
    BoundStatement* Statement() const { return statement.get(); }
    BoundStatement* ReleaseStatement() { return statement.release(); }
    void Accept(Visitor& visitor) override;
    bool IsBoundPostfixIncDecExpr() const override { return true; }
private:
    std::unique_ptr<BoundExpression> value;
    std::unique_ptr<BoundStatement> statement;
};

class BoundFunctionGroup : public BoundExpression
{
public:
    BoundFunctionGroup(Cm::Ast::Node* syntaxNode_, Cm::Sym::FunctionGroupSymbol* functionGroupSymbol_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuFunctionGroup; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    Cm::Sym::FunctionGroupSymbol* GetFunctionGroupSymbol() const { return functionGroupSymbol; }
    void Accept(Visitor& visitor) override;
    bool IsBoundFunctionGroup() const override { return true; }
    void SetBoundTemplateArguments(const std::vector<Cm::Sym::TypeSymbol*>& boundTemplateArguments_);
    const std::vector<Cm::Sym::TypeSymbol*>& BoundTemplateArguments() const { return boundTemplateArguments; }
    void SetType(Cm::Sym::TypeSymbol* type_) override;
private:
    Cm::Sym::FunctionGroupSymbol* functionGroupSymbol;
    std::vector<Cm::Sym::TypeSymbol*> boundTemplateArguments;
    std::unique_ptr<Cm::Sym::TypeSymbol> ownedTypeSymbol;
};

class BoundFunctionCall : public BoundExpression
{
public:
    BoundFunctionCall(Cm::Ast::Node* syntaxNode_, BoundExpressionList&& arguments_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuFunctionCall; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    bool IsBoundFunctionCall() const override { return true; }
    void SetFunction(Cm::Sym::FunctionSymbol* fun_) { fun = fun_; }
    Cm::Sym::FunctionSymbol* GetFunction() const { return fun; }
    void Accept(Visitor& visitor) override;
    BoundExpressionList& Arguments() { return arguments; }
    void SetClassObjectResultVar(Cm::Sym::LocalVariableSymbol* classObjectResultVar_) { classObjectResultVar = classObjectResultVar_; }
    Cm::Sym::LocalVariableSymbol* GetClassObjectResultVar() const { return classObjectResultVar; }
    void SetTemporary(BoundLocalVariable* temporary_) { temporary.reset(temporary_); }
    BoundLocalVariable* GetTemporary() const { return temporary.get(); }
    void SetTraceCallInfo(TraceCallInfo* traceCallInfo_);
    TraceCallInfo* GetTraceCallInfo() const { return traceCallInfo.get(); }
private:
    BoundExpressionList arguments;
    Cm::Sym::FunctionSymbol* fun;
    Cm::Sym::LocalVariableSymbol* classObjectResultVar;
    std::unique_ptr<BoundLocalVariable> temporary;
    std::unique_ptr<TraceCallInfo> traceCallInfo;
};

class BoundDelegateCall : public BoundExpression
{
public:
    BoundDelegateCall(Cm::Sym::DelegateTypeSymbol* delegateType_, BoundExpression* subject_, Cm::Ast::Node* syntaxNode_, BoundExpressionList&& arguments_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuDelegateCall; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void Accept(Visitor& visitor) override;
    Cm::Sym::DelegateTypeSymbol* DelegateType() const { return delegateType; }
    BoundExpression* Subject() const { return subject.get(); }
    BoundExpressionList& Arguments() { return arguments; }
private:
    Cm::Sym::DelegateTypeSymbol* delegateType;
    std::unique_ptr<BoundExpression> subject;
    BoundExpressionList arguments;
};

class BoundClassDelegateCall : public BoundExpression
{
public:
    BoundClassDelegateCall(Cm::Sym::ClassDelegateTypeSymbol* classDelegateType_, BoundExpression* subject_, Cm::Ast::Node* syntaxNode_, BoundExpressionList&& arguments_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuClassDelegateCall; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void Accept(Visitor& visitor) override;
    Cm::Sym::ClassDelegateTypeSymbol* ClassDelegateType() const { return classDelegateType; }
    BoundExpression* Subject() const { return subject.get(); }
    BoundExpressionList& Arguments() { return arguments; }
private:
    Cm::Sym::ClassDelegateTypeSymbol* classDelegateType;
    std::unique_ptr<BoundExpression> subject;
    BoundExpressionList arguments;
};

class BoundBooleanBinaryExpression : public BoundExpression
{
public:
    BoundBooleanBinaryExpression(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_);
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    BoundExpression* Left() const { return left.get(); }
    BoundExpression* Right() const { return right.get(); }
private:
    std::unique_ptr<BoundExpression> left;
    std::unique_ptr<BoundExpression> right;
};

class BoundDisjunction : public BoundBooleanBinaryExpression
{
public:    
    BoundDisjunction(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuDisjunction; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void Accept(Visitor& visitor) override;
    void SetResultVar(Cm::Sym::LocalVariableSymbol* resultVar_) { resultVar = resultVar_; }
    Cm::Sym::LocalVariableSymbol* GetResultVar() const { return resultVar; }
private:
    Cm::Sym::LocalVariableSymbol* resultVar;
};

class BoundConjunction : public BoundBooleanBinaryExpression
{
public:
    BoundConjunction(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_);
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuConjunction; }
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void Accept(Visitor& visitor) override;
    void SetResultVar(Cm::Sym::LocalVariableSymbol* resultVar_) { resultVar = resultVar_; }
    Cm::Sym::LocalVariableSymbol* GetResultVar() const { return resultVar; }
private:
    Cm::Sym::LocalVariableSymbol* resultVar;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_BOUND_EXPRESSION_INCLUDED
