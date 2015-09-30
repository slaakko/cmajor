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

void BoundExpression::Write(Cm::Sym::BcuWriter& writer)
{
    BoundNode::Write(writer);
    writer.Write(type);
    if (type)
    {
        type->DoSerialize();
    }
}

void BoundExpression::Read(Cm::Sym::BcuReader& reader)
{
    BoundNode::Read(reader);
    Cm::Sym::Symbol* symbol = reader.ReadSymbol();
    if (symbol)
    {
        if (symbol->IsTypeSymbol())
        {
            type = static_cast<Cm::Sym::TypeSymbol*>(symbol);
        }
    }
}

TraceCallInfo::TraceCallInfo() : fun(), file(), line()
{
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

void TraceCallInfo::Read(Cm::Sym::BcuReader& reader)
{
    Cm::Sym::BcuItem* funItem = reader.ReadItem();
    if (funItem->IsBoundExpression())
    {
        fun.reset(static_cast<BoundExpression*>(funItem));
    }
    Cm::Sym::BcuItem* fileItem = reader.ReadItem();
    if (fileItem->IsBoundExpression())
    {
        file.reset(static_cast<BoundExpression*>(fileItem));
    }
    Cm::Sym::BcuItem* lineItem = reader.ReadItem();
    if (lineItem->IsBoundExpression())
    {
        line.reset(static_cast<BoundExpression*>(lineItem));
    }
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

void BoundExpressionList::Read(Cm::Sym::BcuReader& reader)
{
    int n = reader.GetBinaryReader().ReadInt();
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::BcuItem* item = reader.ReadItem();
        if (item->IsBoundExpression())
        {
            expressions.push_back(std::unique_ptr<BoundExpression>(static_cast<BoundExpression*>(item)));
        }
        else
        {
            throw std::runtime_error("bound expression expected");
        }
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

BoundStringLiteral::BoundStringLiteral() : BoundExpression(nullptr), id(-1)
{
}

BoundStringLiteral::BoundStringLiteral(Cm::Ast::Node* syntaxNode_, int id_) : BoundExpression(syntaxNode_), id(id_)
{
}

void BoundStringLiteral::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.GetBinaryWriter().Write(id);
}

void BoundStringLiteral::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    id = reader.GetBinaryReader().ReadInt();
}

void BoundStringLiteral::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundLiteral::BoundLiteral() : BoundExpression(nullptr)
{
}

BoundLiteral::BoundLiteral(Cm::Ast::Node* syntaxNode_) : BoundExpression(syntaxNode_)
{
}

void BoundLiteral::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.GetSymbolWriter().Write(value.get());
}

void BoundLiteral::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    value.reset(reader.GetSymbolReader().ReadValue());
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

BoundConstant::BoundConstant() : BoundExpression(nullptr), symbol(nullptr)
{
}

BoundConstant::BoundConstant(Cm::Ast::Node* syntaxNode_, Cm::Sym::ConstantSymbol* symbol_) : BoundExpression(syntaxNode_), symbol(symbol_)
{
}

void BoundConstant::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(symbol);
}

void BoundConstant::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s)
    {
        if (s->IsConstantSymbol())
        {
            symbol = static_cast<Cm::Sym::ConstantSymbol*>(s);
        }
        else
        {
            throw std::runtime_error("constant symbol expected");
        }
    }
}

void BoundConstant::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundExceptionTableConstant::BoundExceptionTableConstant() : BoundConstant(nullptr, nullptr)
{
}

BoundExceptionTableConstant::BoundExceptionTableConstant(Cm::Ast::Node* syntaxNode_) : BoundConstant(syntaxNode_, nullptr)
{
}

BoundClassHierarchyTableConstant::BoundClassHierarchyTableConstant() : BoundConstant(nullptr, nullptr)
{
}

BoundClassHierarchyTableConstant::BoundClassHierarchyTableConstant(Cm::Ast::Node* syntaxNode_) : BoundConstant(syntaxNode_, nullptr)
{
}

BoundEnumConstant::BoundEnumConstant() : BoundExpression(nullptr), symbol(nullptr)
{
}

BoundEnumConstant::BoundEnumConstant(Cm::Ast::Node* syntaxNode_, Cm::Sym::EnumConstantSymbol* symbol_) : BoundExpression(syntaxNode_), symbol(symbol_)
{
}

void BoundEnumConstant::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(symbol);
}

void BoundEnumConstant::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s->IsEnumConstantSymbol())
    {
        symbol = static_cast<Cm::Sym::EnumConstantSymbol*>(s);
    }
    else
    {
        throw std::runtime_error("enum constant symbol expected");
    }
}

void BoundEnumConstant::Accept(Visitor& visitor) 
{
    visitor.Visit(*this);
}

BoundLocalVariable::BoundLocalVariable() : BoundExpression(nullptr), symbol(nullptr)
{
}

BoundLocalVariable::BoundLocalVariable(Cm::Ast::Node* syntaxNode_, Cm::Sym::LocalVariableSymbol* symbol_) : BoundExpression(syntaxNode_), symbol(symbol_)
{
}

void BoundLocalVariable::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(symbol);
}

void BoundLocalVariable::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s)
    {
        if (s->IsLocalVariableSymbol())
        {
            symbol = static_cast<Cm::Sym::LocalVariableSymbol*>(s);
        }
        else
        {
            throw std::runtime_error("local variable symbol expected");
        }
    }
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

BoundParameter::BoundParameter() : BoundExpression(nullptr), symbol(nullptr)
{
}

BoundParameter::BoundParameter(Cm::Ast::Node* syntaxNode_, Cm::Sym::ParameterSymbol* symbol_) : BoundExpression(syntaxNode_), symbol(symbol_)
{
}

void BoundParameter::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(symbol);
}

void BoundParameter::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s)
    {
        if (s->IsParameterSymbol())
        {
            symbol = static_cast<Cm::Sym::ParameterSymbol*>(s);
        }
        else
        {
            throw std::runtime_error("parameter symbol expected");
        }
    }
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

BoundReturnValue::BoundReturnValue() : BoundExpression(nullptr), symbol(nullptr)
{
}

BoundReturnValue::BoundReturnValue(Cm::Ast::Node* syntaxNode_, Cm::Sym::ReturnValueSymbol* symbol_) : BoundExpression(syntaxNode_), symbol(symbol_)
{
}

void BoundReturnValue::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(symbol);
}

void BoundReturnValue::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s->IsReturnValueSymbol())
    {
        symbol = static_cast<Cm::Sym::ReturnValueSymbol*>(s);
    }
    else
    {
        throw std::runtime_error("return value symbol expected");
    }
}

void BoundReturnValue::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundMemberVariable::BoundMemberVariable() : BoundExpression(nullptr), symbol(nullptr)
{
}

BoundMemberVariable::BoundMemberVariable(Cm::Ast::Node* syntaxNode_, Cm::Sym::MemberVariableSymbol* symbol_) : BoundExpression(syntaxNode_), symbol(symbol_)
{
}

void BoundMemberVariable::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(classObject.get());
    writer.Write(symbol);
}

void BoundMemberVariable::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::BcuItem* item = reader.ReadItem();
    if (item)
    {
        if (item->IsBoundExpression())
        {
            classObject.reset(static_cast<BoundExpression*>(item));
        }
        else
        {
            throw std::runtime_error("bound expression expected");
        }
    }
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s->IsMemberVariableSymbol())
    {
        symbol = static_cast<Cm::Sym::MemberVariableSymbol*>(s);
    }
    else
    {
        throw std::runtime_error("member variable symbol expected");
    }
}

void BoundMemberVariable::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundMemberVariable::SetClassObject(Cm::BoundTree::BoundExpression* classObject_)
{
    classObject.reset(classObject_);
}

BoundFunctionId::BoundFunctionId() : BoundExpression(nullptr), functionSymbol(nullptr)
{
}

BoundFunctionId::BoundFunctionId(Cm::Ast::Node* syntaxNode_, Cm::Sym::FunctionSymbol* functionSymbol_) : BoundExpression(syntaxNode_), functionSymbol(functionSymbol_)
{
}

void BoundFunctionId::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(functionSymbol);
}

void BoundFunctionId::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s->IsFunctionSymbol())
    {
        functionSymbol = static_cast<Cm::Sym::FunctionSymbol*>(s);
    }
    else
    {
        throw std::runtime_error("function symbol expected");
    }
}

void BoundFunctionId::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundTypeExpression::BoundTypeExpression() : BoundExpression(nullptr), typeSymbol(nullptr)
{
}

BoundTypeExpression::BoundTypeExpression(Cm::Ast::Node* syntaxNode_, Cm::Sym::TypeSymbol* typeSymbol_) : BoundExpression(syntaxNode_), typeSymbol(typeSymbol_)
{
}

void BoundTypeExpression::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(typeSymbol);
}

void BoundTypeExpression::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s->IsTypeSymbol())
    {
        typeSymbol = static_cast<Cm::Sym::TypeSymbol*>(s);
    }
    else
    {
        throw std::runtime_error("type symbol expected");
    }
}

void BoundTypeExpression::Accept(Visitor& visitor)
{
    throw std::runtime_error("member function not applicable");
}

BoundNamespaceExpression::BoundNamespaceExpression() : BoundExpression(nullptr), namespaceSymbol(nullptr)
{
}

BoundNamespaceExpression::BoundNamespaceExpression(Cm::Ast::Node* syntaxNode_, Cm::Sym::NamespaceSymbol* namespaceSymbol_) : BoundExpression(syntaxNode_), namespaceSymbol(namespaceSymbol_)
{
}

void BoundNamespaceExpression::Accept(Visitor& visitor)
{
    throw std::runtime_error("member function not applicable");
}

void BoundNamespaceExpression::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(namespaceSymbol);
}

void BoundNamespaceExpression::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s->IsNamespaceSymbol())
    {
        namespaceSymbol = static_cast<Cm::Sym::NamespaceSymbol*>(s);
    }
    else
    {
        throw std::runtime_error("namespace symbol expected");
    }
}

BoundConversion::BoundConversion() : BoundExpression(nullptr), operand(nullptr), conversionFun(nullptr)
{
}

BoundConversion::BoundConversion(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_, Cm::Sym::FunctionSymbol* conversionFun_) : BoundExpression(syntaxNode_), operand(operand_), conversionFun(conversionFun_)
{
}

void BoundConversion::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(operand.get());
    writer.Write(conversionFun);
    writer.Write(boundTemporary.get());
}

void BoundConversion::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::BcuItem* operandItem = reader.ReadItem();
    if (operandItem->IsBoundExpression())
    {
        operand.reset(static_cast<BoundExpression*>(operandItem));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s->IsFunctionSymbol())
    {
        conversionFun = static_cast<Cm::Sym::FunctionSymbol*>(s);
    }
    else
    {
        throw std::runtime_error("function symbol expected");
    }
    Cm::Sym::BcuItem* temporaryItem = reader.ReadItem();
    if (temporaryItem)
    {
        if (temporaryItem->IsBoundExpression())
        {
            boundTemporary.reset(static_cast<BoundExpression*>(temporaryItem));
        }
        else
        {
            throw std::runtime_error("bound expression expected");
        }
    }
}

void BoundConversion::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundConversion::SetBoundTemporary(Cm::BoundTree::BoundExpression* boundTemporary_)
{
    boundTemporary.reset(boundTemporary_);
}

BoundCast::BoundCast() : BoundExpression(nullptr), operand(nullptr), conversionFun(nullptr), sourceType(nullptr)
{
}

BoundCast::BoundCast(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_, Cm::Sym::FunctionSymbol* conversionFun_) : 
    BoundExpression(syntaxNode_), operand(operand_), conversionFun(conversionFun_), sourceType(nullptr)
{
}

void BoundCast::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(operand.get());
    writer.Write(conversionFun);
    writer.Write(sourceType);
}

void BoundCast::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::BcuItem* operandItem = reader.ReadItem();
    if (operandItem->IsBoundExpression())
    {
        operand.reset(static_cast<BoundExpression*>(operandItem));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s->IsFunctionSymbol())
    {
        conversionFun = static_cast<Cm::Sym::FunctionSymbol*>(s);
    }
    else
    {
        throw std::runtime_error("function symbol expected");
    }
    Cm::Sym::Symbol* t = reader.ReadSymbol();
    if (t->IsTypeSymbol())
    {
        sourceType = static_cast<Cm::Sym::TypeSymbol*>(t);
    }
    else
    {
        throw std::runtime_error("type symbol expected");
    }
}

void BoundCast::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundIsExpression::BoundIsExpression() : BoundExpression(nullptr), leftClassType(nullptr), rightClassType(nullptr)
{
}

BoundIsExpression::BoundIsExpression(Cm::Ast::Node* syntaxNode_, BoundExpression* expr_, Cm::Sym::ClassTypeSymbol* leftClassType_, Cm::Sym::ClassTypeSymbol* rightClassType_) : 
    BoundExpression(syntaxNode_), expr(expr_), leftClassType(leftClassType_), rightClassType(rightClassType_)
{
}

void BoundIsExpression::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(expr.get());
    writer.Write(leftClassType);
    writer.Write(rightClassType);
}

void BoundIsExpression::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::BcuItem* exprItem = reader.ReadItem();
    if (exprItem->IsBoundExpression())
    {
        expr.reset(static_cast<BoundExpression*>(exprItem));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
    leftClassType = reader.ReadClassTypeSymbol();
    rightClassType = reader.ReadClassTypeSymbol();
}

void BoundIsExpression::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundAsExpression::BoundAsExpression() : BoundExpression(nullptr), leftClassType(nullptr), rightClassType(nullptr)
{
}

BoundAsExpression::BoundAsExpression(Cm::Ast::Node* syntaxNode_, BoundExpression* expr_, Cm::Sym::ClassTypeSymbol* leftClassType_, Cm::Sym::ClassTypeSymbol* rightClassType_) :
    BoundExpression(syntaxNode_), expr(expr_), leftClassType(leftClassType_), rightClassType(rightClassType_)
{
}

void BoundAsExpression::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(expr.get());
    writer.Write(leftClassType);
    writer.Write(rightClassType);
    writer.Write(boundTemporary.get());
}

void BoundAsExpression::Read(Cm::Sym::BcuReader& reader)
{
    Cm::Sym::BcuItem* exprItem = reader.ReadItem();
    if (exprItem->IsBoundExpression())
    {
        expr.reset(static_cast<BoundExpression*>(exprItem));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
    leftClassType = reader.ReadClassTypeSymbol();
    rightClassType = reader.ReadClassTypeSymbol();
    Cm::Sym::BcuItem* boundTemporaryItem = reader.ReadItem();
    if (boundTemporaryItem->IsBoundExpression())
    {
        boundTemporary.reset(static_cast<BoundExpression*>(boundTemporaryItem));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
}

void BoundAsExpression::SetBoundTemporary(BoundExpression* boundTemporary_)
{
    boundTemporary.reset(boundTemporary_);
}

void BoundAsExpression::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundSizeOfExpression::BoundSizeOfExpression() : BoundExpression(nullptr), type(nullptr)
{
}

BoundSizeOfExpression::BoundSizeOfExpression(Cm::Ast::Node* syntaxNode_, Cm::Sym::TypeSymbol* type_) : BoundExpression(syntaxNode_), type(type_)
{
}

void BoundSizeOfExpression::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(type);
}

void BoundSizeOfExpression::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s->IsTypeSymbol())
    {
        type = static_cast<Cm::Sym::TypeSymbol*>(s);
    }
    else
    {
        throw std::runtime_error("type symbol expected");
    }
}

void BoundSizeOfExpression::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundDynamicTypeNameExpression::BoundDynamicTypeNameExpression() : BoundExpression(nullptr), subject(nullptr), classType(nullptr)
{
}

BoundDynamicTypeNameExpression::BoundDynamicTypeNameExpression(Cm::Ast::Node* syntaxNode_, BoundExpression* subject_, Cm::Sym::ClassTypeSymbol* classType_) : 
    BoundExpression(syntaxNode_), subject(subject_), classType(classType_)
{
}

void BoundDynamicTypeNameExpression::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(subject.get());
    writer.Write(classType);
}

void BoundDynamicTypeNameExpression::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::BcuItem* item = reader.ReadItem();
    if (item->IsBoundExpression())
    {
        subject.reset(static_cast<BoundExpression*>(item));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
    classType = reader.ReadClassTypeSymbol();
}

void BoundDynamicTypeNameExpression::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundUnaryOp::BoundUnaryOp() : BoundExpression(nullptr), operand(nullptr), fun(nullptr), classObjectResultVar(nullptr), argumentCategory(Cm::Core::ArgumentCategory::rvalue)
{
}

BoundUnaryOp::BoundUnaryOp(Cm::Ast::Node* syntaxNode_, BoundExpression* operand_) : 
    BoundExpression(syntaxNode_), operand(operand_), fun(nullptr), classObjectResultVar(nullptr), argumentCategory(Cm::Core::ArgumentCategory::rvalue)
{
}

void BoundUnaryOp::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(operand.get());
    writer.Write(fun);
    writer.Write(classObjectResultVar);
    writer.Write(traceCallInfo.get());
    writer.GetBinaryWriter().Write(uint8_t(argumentCategory));
}

void BoundUnaryOp::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::BcuItem* operandItem = reader.ReadItem();
    if (operandItem->IsBoundExpression())
    {
        operand.reset(static_cast<BoundExpression*>(operandItem));
    }
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s->IsFunctionSymbol())
    {
        fun = static_cast<Cm::Sym::FunctionSymbol*>(s);
    }
    Cm::Sym::Symbol* t = reader.ReadSymbol();
    if (t)
    {
        if (t->IsLocalVariableSymbol())
        {
            classObjectResultVar = static_cast<Cm::Sym::LocalVariableSymbol*>(t);
        }
        else
        {
            throw std::runtime_error("local variable symbol expected");
        }
    }
    Cm::Sym::BcuItem* traceCallItem = reader.ReadItem();
    if (traceCallItem)
    {
        if (traceCallItem->IsTraceCall())
        {
            traceCallInfo.reset(static_cast<TraceCallInfo*>(traceCallItem));
        }
        else
        {
            throw std::runtime_error("trace call info expected");
        }
    }
    argumentCategory = Cm::Core::ArgumentCategory(reader.GetBinaryReader().ReadByte());
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

BoundBinaryOp::BoundBinaryOp() : BoundExpression(nullptr), left(nullptr), right(nullptr), fun(nullptr), classObjectResultVar(nullptr)
{
}

BoundBinaryOp::BoundBinaryOp(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_) : BoundExpression(syntaxNode_), left(left_), right(right_), fun(nullptr), 
    classObjectResultVar(nullptr)
{
}

void BoundBinaryOp::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(left.get());
    writer.Write(right.get());
    writer.Write(fun);
    writer.Write(classObjectResultVar);
    writer.Write(traceCallInfo.get());
}

void BoundBinaryOp::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::BcuItem* leftItem = reader.ReadItem();
    if (leftItem->IsBoundExpression())
    {
        left.reset(static_cast<BoundExpression*>(leftItem));
    }
    Cm::Sym::BcuItem* rightItem = reader.ReadItem();
    if (rightItem->IsBoundExpression())
    {
        right.reset(static_cast<BoundExpression*>(rightItem));
    }
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s->IsFunctionSymbol())
    {
        fun = static_cast<Cm::Sym::FunctionSymbol*>(s);
    }
    else
    {
        throw std::runtime_error("function symbol expected");
    }
    Cm::Sym::Symbol* t = reader.ReadSymbol();
    if (t)
    {
        if (t->IsLocalVariableSymbol())
        {
            classObjectResultVar = static_cast<Cm::Sym::LocalVariableSymbol*>(t);
        }
        else
        {
            throw std::runtime_error("local variable symbol expected");
        }
    }
    Cm::Sym::BcuItem* traceCallItem = reader.ReadItem();
    if (traceCallItem)
    {
        if (traceCallItem->IsTraceCall())
        {
            traceCallInfo.reset(static_cast<TraceCallInfo*>(traceCallItem));
        }
        else
        {
            throw std::runtime_error("trace call info expected");
        }
    }
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

BoundPostfixIncDecExpr::BoundPostfixIncDecExpr() : BoundExpression(nullptr), value(nullptr), statement(nullptr)
{
}

BoundPostfixIncDecExpr::BoundPostfixIncDecExpr(Cm::Ast::Node* syntaxNode_, BoundExpression* value_, BoundStatement* statement_) : BoundExpression(syntaxNode_), value(value_), statement(statement_)
{
}

void BoundPostfixIncDecExpr::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(value.get());
    writer.Write(statement.get());
}

void BoundPostfixIncDecExpr::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::BcuItem* valueItem = reader.ReadItem();
    if (valueItem->IsBoundExpression())
    {
        value.reset(static_cast<BoundExpression*>(valueItem));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
    Cm::Sym::BcuItem* statementItem = reader.ReadItem();
    if (statementItem->IsBoundStatement())
    {
        statement.reset(static_cast<BoundStatement*>(statementItem));
    }
    else
    {
        throw std::runtime_error("bound statement expected");
    }
}

void BoundPostfixIncDecExpr::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundFunctionGroup::BoundFunctionGroup() : BoundExpression(nullptr), functionGroupSymbol(nullptr)
{
}

BoundFunctionGroup::BoundFunctionGroup(Cm::Ast::Node* syntaxNode_, Cm::Sym::FunctionGroupSymbol* functionGroupSymbol_) : BoundExpression(syntaxNode_), functionGroupSymbol(functionGroupSymbol_)
{
}

void BoundFunctionGroup::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(functionGroupSymbol);
    int n = int(boundTemplateArguments.size());
    writer.GetBinaryWriter().Write(n);
    for (Cm::Sym::TypeSymbol* templateArg : boundTemplateArguments)
    {
        writer.Write(templateArg);
    }
    writer.Write(ownedTypeSymbol.get());
}

void BoundFunctionGroup::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s->IsFunctionGroupSymbol())
    {
        functionGroupSymbol = static_cast<Cm::Sym::FunctionGroupSymbol*>(s);
    }
    else
    {
        throw std::runtime_error("function group symbol expected");
    }
    int n = reader.GetBinaryReader().ReadInt();
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::Symbol* b = reader.ReadSymbol();
        if (b->IsTypeSymbol())
        {
            boundTemplateArguments.push_back(static_cast<Cm::Sym::TypeSymbol*>(b));
        }
        else
        {
            throw std::runtime_error("type symbol expected");
        }
    }
//  todo:
    Cm::Sym::Symbol* u = reader.ReadSymbol(); 
    if (u->IsTypeSymbol())
    {
        SetType(static_cast<Cm::Sym::TypeSymbol*>(u));
    }
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

BoundFunctionCall::BoundFunctionCall() : BoundExpression(nullptr), arguments(), fun(nullptr), classObjectResultVar(nullptr)
{
}

BoundFunctionCall::BoundFunctionCall(Cm::Ast::Node* syntaxNode_, BoundExpressionList&& arguments_) : BoundExpression(syntaxNode_), arguments(std::move(arguments_)), fun(nullptr), 
    classObjectResultVar(nullptr)
{
}

void BoundFunctionCall::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    arguments.Write(writer);
    writer.Write(fun);
    writer.Write(classObjectResultVar);
    writer.Write(temporary.get());
    writer.Write(traceCallInfo.get());
}

void BoundFunctionCall::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    arguments.Read(reader);
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s->IsFunctionSymbol())
    {
        fun = static_cast<Cm::Sym::FunctionSymbol*>(s);
    }
    else
    {
        throw std::runtime_error("function symbol expected");
    }
    Cm::Sym::Symbol* t = reader.ReadSymbol();
    if (t)
    {
        if (t->IsLocalVariableSymbol())
        {
            classObjectResultVar = static_cast<Cm::Sym::LocalVariableSymbol*>(t);
        }
        else
        {
            throw std::runtime_error("local variable symbol expected");
        }
    }
    Cm::Sym::BcuItem* item = reader.ReadItem();
    if (item)
    {
        if (item->IsBoundLocalVariable())
        {
            temporary.reset(static_cast<BoundLocalVariable*>(item));
        }
        else
        {
            throw std::runtime_error("bound local variable expected");
        }
    }
    Cm::Sym::BcuItem* traceCallItem = reader.ReadItem();
    if (traceCallItem)
    {
        if (traceCallItem->IsTraceCall())
        {
            traceCallInfo.reset(static_cast<TraceCallInfo*>(traceCallItem));
        }
        else
        {
            throw std::runtime_error("trace call info expected");
        }
    }
}

void BoundFunctionCall::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundFunctionCall::SetTraceCallInfo(TraceCallInfo* traceCallInfo_)
{
    traceCallInfo.reset(traceCallInfo_);
}

BoundDelegateCall::BoundDelegateCall() : BoundExpression(nullptr), delegateType(nullptr), subject(nullptr), arguments()
{
}

BoundDelegateCall::BoundDelegateCall(Cm::Sym::DelegateTypeSymbol* delegateType_, BoundExpression* subject_, Cm::Ast::Node* syntaxNode_, BoundExpressionList&& arguments_) :
    BoundExpression(syntaxNode_), delegateType(delegateType_), subject(subject_), arguments(std::move(arguments_))
{
}

void BoundDelegateCall::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(delegateType);
    writer.Write(subject.get());
    arguments.Write(writer);
}

void BoundDelegateCall::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::Symbol* d = reader.ReadSymbol();
    if (d->IsDelegateTypeSymbol())
    {
        delegateType = static_cast<Cm::Sym::DelegateTypeSymbol*>(d);
    }
    else
    {
        throw std::runtime_error("delegate type symbol expected");
    }
    Cm::Sym::BcuItem* item = reader.ReadItem();
    if (item->IsBoundExpression())
    {
        subject.reset(static_cast<BoundExpression*>(item));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
    arguments.Read(reader);
}

void BoundDelegateCall::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundClassDelegateCall::BoundClassDelegateCall() : BoundExpression(nullptr), classDelegateType(nullptr), subject(nullptr), arguments()
{
}

BoundClassDelegateCall::BoundClassDelegateCall(Cm::Sym::ClassDelegateTypeSymbol* classDelegateType_, BoundExpression* subject_, Cm::Ast::Node* syntaxNode_, BoundExpressionList&& arguments_) :
    BoundExpression(syntaxNode_), classDelegateType(classDelegateType_), subject(subject_), arguments(std::move(arguments_))
{
}

void BoundClassDelegateCall::Write(Cm::Sym::BcuWriter& writer)
{
    BoundExpression::Write(writer);
    writer.Write(classDelegateType);
    writer.Write(subject.get());
    arguments.Write(writer);
}

void BoundClassDelegateCall::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::Symbol* d = reader.ReadSymbol();
    if (d->IsClassDelegateTypeSymbol())
    {
        classDelegateType = static_cast<Cm::Sym::ClassDelegateTypeSymbol*>(d);
    }
    else
    {
        throw std::runtime_error("class delegate type symbol expected");
    }
    Cm::Sym::BcuItem* item = reader.ReadItem();
    if (item->IsBoundExpression())
    {
        subject.reset(static_cast<BoundExpression*>(item));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
    arguments.Read(reader);
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
    BoundExpression::Write(writer);
    writer.Write(left.get());
    writer.Write(right.get());
}

void BoundBooleanBinaryExpression::Read(Cm::Sym::BcuReader& reader)
{
    BoundExpression::Read(reader);
    Cm::Sym::BcuItem* leftItem = reader.ReadItem();
    if (leftItem->IsBoundExpression())
    {
        left.reset(static_cast<BoundExpression*>(leftItem));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
    Cm::Sym::BcuItem* rightItem = reader.ReadItem();
    if (rightItem->IsBoundExpression())
    {
        right.reset(static_cast<BoundExpression*>(rightItem));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
}

BoundDisjunction::BoundDisjunction() : BoundBooleanBinaryExpression(nullptr, nullptr, nullptr), resultVar(nullptr)
{
}

BoundDisjunction::BoundDisjunction(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_) : BoundBooleanBinaryExpression(syntaxNode_, left_, right_), resultVar(nullptr)
{
}

void BoundDisjunction::Write(Cm::Sym::BcuWriter& writer)
{
    BoundBooleanBinaryExpression::Write(writer);
    writer.Write(resultVar);
}

void BoundDisjunction::Read(Cm::Sym::BcuReader& reader)
{
    BoundBooleanBinaryExpression::Read(reader);
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s->IsLocalVariableSymbol())
    {
        resultVar = static_cast<Cm::Sym::LocalVariableSymbol*>(s);
    }
    else
    {
        throw std::runtime_error("local variable symbol expected");
    }
}

void BoundDisjunction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundConjunction::BoundConjunction() : BoundBooleanBinaryExpression(nullptr, nullptr, nullptr), resultVar(nullptr)
{
}

BoundConjunction::BoundConjunction(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_) : BoundBooleanBinaryExpression(syntaxNode_, left_, right_), resultVar(nullptr)
{
}

void BoundConjunction::Write(Cm::Sym::BcuWriter& writer)
{
    BoundBooleanBinaryExpression::Write(writer);
    writer.Write(resultVar);
}

void BoundConjunction::Read(Cm::Sym::BcuReader& reader)
{
    BoundBooleanBinaryExpression::Read(reader);
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s->IsLocalVariableSymbol())
    {
        resultVar = static_cast<Cm::Sym::LocalVariableSymbol*>(s);
    }
    else
    {
        throw std::runtime_error("local variable symbol expected");
    }
}

void BoundConjunction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace Cm::BoundTree
