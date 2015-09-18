/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/BoundExpression.hpp>
#include <Cm.BoundTree/BoundStatement.hpp>
#include <Cm.BoundTree/Visitor.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/NamespaceSymbol.hpp>
#include <Cm.Sym/FunctionGroupSymbol.hpp>
#include <algorithm>
#include <stdexcept>

namespace Cm { namespace BoundTree {

BoundExpression::BoundExpression(Cm::Ast::Node* syntaxNode_) : BoundNode(syntaxNode_), type(nullptr), cfgNode(nullptr)
{
}

void BoundExpression::SetCfgNode(Cm::Core::CfgNode* cfgNode_)
{
    cfgNode = cfgNode_;
}

TraceCallInfo::TraceCallInfo(BoundExpression* fun_, BoundExpression* file_, BoundExpression* line_) : fun(fun_), file(file_), line(line_)
{
}

void TraceCallInfo::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(fun.get());
    writer.Write(file.get());
    writer.Write(line.get());
}

BoundExpressionList::BoundExpressionList()
{
}

void BoundExpressionList::Write(Cm::Sym::BcuWriter& writer)
{
    int n = int(expressions.size());
    writer.GetBinaryWriter().Write(n);
    for (const std::unique_ptr<BoundExpression>& expr : expressions)
    {
        writer.Write(expr.get());
    }
}

void BoundExpressionList::Add(BoundExpression* expression)
{
    expressions.push_back(std::unique_ptr<BoundExpression>(expression));
}

void BoundExpressionList::InsertFront(BoundExpression* expr)
{
    expressions.insert(expressions.begin(), std::unique_ptr<BoundExpression>(expr));
}

BoundExpression* BoundExpressionList::RemoveFirst()
{
    std::unique_ptr<BoundExpression> first = std::move(expressions.front());
    expressions.erase(expressions.begin());
    return first.release();
}

BoundExpression* BoundExpressionList::GetLast()
{
    std::unique_ptr<BoundExpression> last = std::move(expressions.back());
    expressions.pop_back();
    return last.release();
}

void BoundExpressionList::Reverse()
{
    std::reverse(expressions.begin(), expressions.end());
}

void BoundExpressionList::Accept(Visitor& visitor)
{
    for (const std::unique_ptr<BoundExpression>& expression : expressions)
    {
        expression->Accept(visitor);
    }
}

BoundStringLiteral::BoundStringLiteral(Cm::Ast::Node* syntaxNode_, int id_) : BoundExpression(syntaxNode_), id(id_)
{
}

void BoundStringLiteral::Write(Cm::Sym::BcuWriter& writer)
{
    writer.GetBinaryWriter().Write(id);
}

void BoundStringLiteral::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundLiteral::BoundLiteral(Cm::Ast::Node* syntaxNode_) : BoundExpression(syntaxNode_)
{
}

void BoundLiteral::Write(Cm::Sym::BcuWriter& writer)
{
    writer.GetSymbolWriter().Write(value.get());
}

void BoundLiteral::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundLiteral::SetType(Cm::Sym::TypeSymbol* type_) 
{
    BoundExpression::SetType(type_);
    if (value->IsNull())
    {
        Cm::Sym::NullValue* nullValue = static_cast<Cm::Sym::NullValue*>(value.get());
        nullValue->SetType(type_);
    }
}

BoundConstant::BoundConstant(Cm::Ast::Node* syntaxNode_, Cm::Sym::ConstantSymbol* symbol_) : BoundExpression(syntaxNode_), symbol(symbol_)
{
}

void BoundConstant::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(symbol);
}

void BoundConstant::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundExceptionTableConstant::BoundExceptionTableConstant(Cm::Ast::Node* syntaxNode_) : BoundConstant(syntaxNode_, nullptr)
{
}

BoundEnumConstant::BoundEnumConstant(Cm::Ast::Node* syntaxNode_, Cm::Sym::EnumConstantSymbol* symbol_) : BoundExpression(syntaxNode_), symbol(symbol_)
{
}

void BoundEnumConstant::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(symbol);
}

void BoundEnumConstant::Accept(Visitor& visitor) 
{
    visitor.Visit(*this);
}

BoundLocalVariable::BoundLocalVariable(Cm::Ast::Node* syntaxNode_, Cm::Sym::LocalVariableSymbol* symbol_) : BoundExpression(syntaxNode_), symbol(symbol_)
{
}

void BoundLocalVariable::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(symbol);
}

void BoundLocalVariable::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundExceptionCodeVariable::BoundExceptionCodeVariable() : BoundLocalVariable(nullptr, nullptr)
{
}

BoundExceptionCodeParameter::BoundExceptionCodeParameter() : BoundParameter(nullptr, nullptr)
{
}

BoundParameter::BoundParameter(Cm::Ast::Node* syntaxNode_, Cm::Sym::ParameterSymbol* symbol_) : BoundExpression(syntaxNode_), symbol(symbol_)
{
}

void BoundParameter::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(symbol);
}

Cm::Core::ArgumentCategory BoundParameter::GetArgumentCategory() const
{
    if (GetType()->IsNonConstReferenceType()) return Cm::Core::ArgumentCategory::lvalue;
    return Cm::Core::ArgumentCategory::rvalue;
}

void BoundParameter::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundReturnValue::BoundReturnValue(Cm::Ast::Node* syntaxNode_, Cm::Sym::ReturnValueSymbol* symbol_) : BoundExpression(syntaxNode_), symbol(symbol_)
{
}

void BoundReturnValue::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(symbol);
}

void BoundReturnValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundMemberVariable::BoundMemberVariable(Cm::Ast::Node* syntaxNode_, Cm::Sym::MemberVariableSymbol* symbol_) : BoundExpression(syntaxNode_), symbol(symbol_)
{
}

void BoundMemberVariable::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(classObject.get());
    writer.Write(symbol);
}

void BoundMemberVariable::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundMemberVariable::SetClassObject(Cm::BoundTree::BoundExpression* classObject_)
{
    classObject.reset(classObject_);
}

BoundFunctionId::BoundFunctionId(Cm::Ast::Node* syntaxNode_, Cm::Sym::FunctionSymbol* functionSymbol_) : BoundExpression(syntaxNode_), functionSymbol(functionSymbol_)
{
}

void BoundFunctionId::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(functionSymbol);
}

void BoundFunctionId::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundTypeExpression::BoundTypeExpression(Cm::Ast::Node* syntaxNode_, Cm::Sym::TypeSymbol* typeSymbol_) : BoundExpression(syntaxNode_), typeSymbol(typeSymbol_)
{
}

void BoundTypeExpression::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(typeSymbol);
}

void BoundTypeExpression::Accept(Visitor& visitor)
{
    throw std::runtime_error("member function not applicable");
}

void BoundNamespaceExpression::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(namespaceSymbol);
}

BoundConversion::BoundConversion(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_, Cm::Sym::FunctionSymbol* conversionFun_) : BoundExpression(syntaxNode_), operand(operand_), conversionFun(conversionFun_)
{
}

void BoundConversion::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(operand.get());
    writer.Write(conversionFun);
    writer.Write(boundTemporary.get());
}

void BoundConversion::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundConversion::SetBoundTemporary(Cm::BoundTree::BoundExpression* boundTemporary_)
{
    boundTemporary.reset(boundTemporary_);
}

BoundCast::BoundCast(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_, Cm::Sym::FunctionSymbol* conversionFun_) : 
    BoundExpression(syntaxNode_), operand(operand_), conversionFun(conversionFun_), sourceType(nullptr)
{
}

void BoundCast::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(operand.get());
    writer.Write(conversionFun);
    writer.Write(sourceType);
}

void BoundCast::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundSizeOfExpression::BoundSizeOfExpression(Cm::Ast::Node* syntaxNode_, Cm::Sym::TypeSymbol* type_) : BoundExpression(syntaxNode_), type(type_)
{
}

void BoundSizeOfExpression::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(type);
}

void BoundSizeOfExpression::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundDynamicTypeNameExpression::BoundDynamicTypeNameExpression(Cm::Ast::Node* syntaxNode_, BoundExpression* subject_, Cm::Sym::ClassTypeSymbol* classType_) : 
    BoundExpression(syntaxNode_), subject(subject_), classType(classType_)
{
}

void BoundDynamicTypeNameExpression::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(subject.get());
    writer.Write(classType);
}

void BoundDynamicTypeNameExpression::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundNamespaceExpression::BoundNamespaceExpression(Cm::Ast::Node* syntaxNode_, Cm::Sym::NamespaceSymbol* namespaceSymbol_) : BoundExpression(syntaxNode_), namespaceSymbol(namespaceSymbol_)
{
}

void BoundNamespaceExpression::Accept(Visitor& visitor)
{
    throw std::runtime_error("member function not applicable");
}

BoundUnaryOp::BoundUnaryOp(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_) : 
    BoundExpression(syntaxNode_), operand(operand_), fun(nullptr), classObjectResultVar(nullptr), argumentCategory(Cm::Core::ArgumentCategory::rvalue)
{
}

void BoundUnaryOp::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(operand.get());
    writer.Write(fun);
    writer.Write(classObjectResultVar);
    writer.Write(traceCallInfo.get());
    writer.GetBinaryWriter().Write(uint8_t(argumentCategory));
}

void BoundUnaryOp::Accept(Visitor& visitor)
{
    operand->Accept(visitor);
    visitor.Visit(*this);
}

void BoundUnaryOp::SetTraceCallInfo(TraceCallInfo* traceCallInfo_)
{
    traceCallInfo.reset(traceCallInfo_);
}

BoundBinaryOp::BoundBinaryOp(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_) : BoundExpression(syntaxNode_), left(left_), right(right_), fun(nullptr), 
    classObjectResultVar(nullptr)
{
}

void BoundBinaryOp::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(left.get());
    writer.Write(right.get());
    writer.Write(fun);
    writer.Write(classObjectResultVar);
    writer.Write(traceCallInfo.get());
}

void BoundBinaryOp::Accept(Visitor& visitor)
{
    left->Accept(visitor);
    right->Accept(visitor);
    visitor.Visit(*this);
}

void BoundBinaryOp::SetTraceCallInfo(TraceCallInfo* traceCallInfo_)
{
    traceCallInfo.reset(traceCallInfo_);
}

BoundPostfixIncDecExpr::BoundPostfixIncDecExpr(Cm::Ast::Node* syntaxNode_, BoundExpression* value_, BoundStatement* statement_) : BoundExpression(syntaxNode_), value(value_), statement(statement_)
{
}

void BoundPostfixIncDecExpr::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(value.get());
    writer.Write(statement.get());
}

void BoundPostfixIncDecExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundFunctionGroup::BoundFunctionGroup(Cm::Ast::Node* syntaxNode_, Cm::Sym::FunctionGroupSymbol* functionGroupSymbol_) : BoundExpression(syntaxNode_), functionGroupSymbol(functionGroupSymbol_)
{
}

void BoundFunctionGroup::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(functionGroupSymbol);
    int n = int(boundTemplateArguments.size());
    writer.GetBinaryWriter().Write(n);
    for (Cm::Sym::TypeSymbol* templateArg : boundTemplateArguments)
    {
        writer.Write(templateArg);
    }
    writer.Write(ownedTypeSymbol.get());
}

void BoundFunctionGroup::Accept(Visitor& visitor)
{
    throw std::runtime_error("member function not applicable");
}

void BoundFunctionGroup::SetBoundTemplateArguments(const std::vector<Cm::Sym::TypeSymbol*>& boundTemplateArguments_)
{
    boundTemplateArguments = boundTemplateArguments_;
}

void BoundFunctionGroup::SetType(Cm::Sym::TypeSymbol* type_)
{
    BoundExpression::SetType(type_);
    ownedTypeSymbol.reset(type_);
}

BoundFunctionCall::BoundFunctionCall(Cm::Ast::Node* syntaxNode_, BoundExpressionList&& arguments_) : BoundExpression(syntaxNode_), arguments(std::move(arguments_)), fun(nullptr), 
    classObjectResultVar(nullptr)
{
}

void BoundFunctionCall::Write(Cm::Sym::BcuWriter& writer)
{
    arguments.Write(writer);
    writer.Write(fun);
    writer.Write(classObjectResultVar);
    writer.Write(temporary.get());
    writer.Write(traceCallInfo.get());
}

void BoundFunctionCall::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundFunctionCall::SetTraceCallInfo(TraceCallInfo* traceCallInfo_)
{
    traceCallInfo.reset(traceCallInfo_);
}

BoundDelegateCall::BoundDelegateCall(Cm::Sym::DelegateTypeSymbol* delegateType_, BoundExpression* subject_, Cm::Ast::Node* syntaxNode_, BoundExpressionList&& arguments_) :
    BoundExpression(syntaxNode_), delegateType(delegateType_), subject(subject_), arguments(std::move(arguments_))
{
}

void BoundDelegateCall::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(delegateType);
    writer.Write(subject.get());
    arguments.Write(writer);
}

void BoundDelegateCall::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundClassDelegateCall::BoundClassDelegateCall(Cm::Sym::ClassDelegateTypeSymbol* classDelegateType_, BoundExpression* subject_, Cm::Ast::Node* syntaxNode_, BoundExpressionList&& arguments_) :
    BoundExpression(syntaxNode_), classDelegateType(classDelegateType_), subject(subject_), arguments(std::move(arguments_))
{
}

void BoundClassDelegateCall::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(classDelegateType);
    writer.Write(subject.get());
    arguments.Write(writer);
}

void BoundClassDelegateCall::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundBooleanBinaryExpression::BoundBooleanBinaryExpression(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_) : BoundExpression(syntaxNode_), left(left_), right(right_)
{
}

void BoundBooleanBinaryExpression::Write(Cm::Sym::BcuWriter& writer)
{
    writer.Write(left.get());
    writer.Write(right.get());
}

BoundDisjunction::BoundDisjunction(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_) : BoundBooleanBinaryExpression(syntaxNode_, left_, right_), resultVar(nullptr)
{
}

void BoundDisjunction::Write(Cm::Sym::BcuWriter& writer)
{
    BoundBooleanBinaryExpression::Write(writer);
    writer.Write(resultVar);
}

void BoundDisjunction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundConjunction::BoundConjunction(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_) : BoundBooleanBinaryExpression(syntaxNode_, left_, right_), resultVar(nullptr)
{
}

void BoundConjunction::Write(Cm::Sym::BcuWriter& writer)
{
    BoundBooleanBinaryExpression::Write(writer);
    writer.Write(resultVar);
}

void BoundConjunction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace Cm::BoundTree
