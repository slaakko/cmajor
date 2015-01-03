
/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/ExpressionBinder.hpp>
#include <Cm.Bind/Exception.hpp>
#include <Cm.Bind/Evaluator.hpp>
#include <Cm.Bind/OverloadResolution.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Bind/Constant.hpp>
#include <Cm.Bind/Class.hpp>
#include <Cm.Bind/LocalVariable.hpp>
#include <Cm.Bind/MemberVariable.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/Parameter.hpp>
#include <Cm.Core/Argument.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/FunctionGroupSymbol.hpp>
#include <Cm.Ast/Expression.hpp>
#include <Cm.Ast/Literal.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Bind {

using Cm::Parsing::Span;

Cm::BoundTree::BoundExpression* BoundExpressionStack::Pop()
{
    if (expressions.Empty()) throw std::runtime_error("bound expression stack is empty");
    return expressions.GetLast();
}

Cm::BoundTree::BoundExpressionList BoundExpressionStack::Pop(int numExpressions)
{
    Cm::BoundTree::BoundExpressionList expressions;
    for (int i = 0; i < numExpressions; ++i)
    {
        expressions.Add(Pop());
    }
    expressions.Reverse();
    return expressions;
}

ExpressionBinder::ExpressionBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ConversionTable& conversionTable_, Cm::Core::ClassConversionTable& classConversionTable_, 
    Cm::Core::PointerOpRepository& pointerOpRepository_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, Cm::BoundTree::BoundFunction* currentFunction_) :
    Cm::Ast::Visitor(true, true), symbolTable(symbolTable_), conversionTable(conversionTable_), classConversionTable(classConversionTable_), pointerOpRepository(pointerOpRepository_),
    containerScope(containerScope_), fileScope(fileScope_), currentFunction(currentFunction_), expressionCount(0)
{
}

Cm::BoundTree::BoundExpressionList ExpressionBinder::GetExpressions()
{
    return boundExpressionStack.GetExpressions();
}

void ExpressionBinder::BindUnaryOp(Cm::Ast::Node* node, const std::string& opGroupName)
{
    Cm::BoundTree::BoundExpression* operand = boundExpressionStack.Pop();
    std::vector<Cm::Core::Argument> arguments;
    arguments.push_back(Cm::Core::Argument(operand->GetArgumentCategory(), operand->GetType()));
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, containerScope->ClassOrNsScope()));
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, operand->GetType()->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* fun = ResolveOverload(symbolTable, conversionTable, classConversionTable, pointerOpRepository, opGroupName, arguments, functionLookups, node->GetSpan(), conversions);
    CheckAccess(currentFunction->GetFunctionSymbol(), node->GetSpan(), fun);
    if (conversions.size() != 1)
    {
        throw std::runtime_error("wrong number of conversions");
    }
    Cm::Sym::FunctionSymbol* conversionFun = conversions[0];
    Cm::BoundTree::BoundExpression* unaryOperand = operand;
    if (conversionFun)
    {
        unaryOperand = new Cm::BoundTree::BoundConversion(node, operand, conversionFun);
        unaryOperand->SetType(conversionFun->GetTargetType());
    }
    Cm::BoundTree::BoundUnaryOp* op = new Cm::BoundTree::BoundUnaryOp(node, unaryOperand);
    op->SetFunction(fun);
    op->SetType(fun->GetReturnType());
    boundExpressionStack.Push(op);
}

void ExpressionBinder::BindBinaryOp(Cm::Ast::Node* node, const std::string& opGroupName)
{ 
    Cm::BoundTree::BoundExpression* right = boundExpressionStack.Pop();
    Cm::BoundTree::BoundExpression* left = boundExpressionStack.Pop();
    std::vector<Cm::Core::Argument> arguments;
    arguments.push_back(Cm::Core::Argument(left->GetArgumentCategory(), left->GetType()));
    arguments.push_back(Cm::Core::Argument(right->GetArgumentCategory(), right->GetType()));
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, containerScope->ClassOrNsScope()));
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, left->GetType()->GetContainerScope()->ClassOrNsScope()));
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, right->GetType()->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* fun = ResolveOverload(symbolTable, conversionTable, classConversionTable, pointerOpRepository, opGroupName, arguments, functionLookups, node->GetSpan(), conversions);
    CheckAccess(currentFunction->GetFunctionSymbol(), node->GetSpan(), fun);
    if (conversions.size() != 2)
    {
        throw std::runtime_error("wrong number of conversions");
    }
    Cm::Sym::FunctionSymbol* leftConversionFun = conversions[0];
    Cm::Sym::FunctionSymbol* rightConversionFun = conversions[1];
    Cm::BoundTree::BoundExpression* leftOperand = left;
    if (leftConversionFun)
    {
        leftOperand = new Cm::BoundTree::BoundConversion(node, left, leftConversionFun);
        leftOperand->SetType(leftConversionFun->GetTargetType());
    }
    Cm::BoundTree::BoundExpression* rightOperand = right;
    if (rightConversionFun)
    {
        rightOperand = new Cm::BoundTree::BoundConversion(node, right, rightConversionFun);
        rightOperand->SetType(rightConversionFun->GetTargetType());
    }
    Cm::BoundTree::BoundBinaryOp* op = new Cm::BoundTree::BoundBinaryOp(node, leftOperand, rightOperand);
    op->SetFunction(fun);
    op->SetType(fun->GetReturnType());
    boundExpressionStack.Push(op);
}

void ExpressionBinder::EndVisit(Cm::Ast::DisjunctionNode& disjunctionNode)
{
    Cm::BoundTree::BoundExpression* right = boundExpressionStack.Pop();
    Cm::BoundTree::BoundExpression* left = boundExpressionStack.Pop();
    if (!left->GetType()->IsBoolTypeSymbol())
    {
        throw Exception("left operand of disjunction is not Boolean expression", disjunctionNode.Left()->GetSpan());
    }
    if (!right->GetType()->IsBoolTypeSymbol())
    {
        throw Exception("right operand of disjunction is not Boolean expression", disjunctionNode.Right()->GetSpan());
    }
    Cm::BoundTree::BoundDisjunction* disjunction = new Cm::BoundTree::BoundDisjunction(&disjunctionNode, left, right);
    disjunction->SetType(left->GetType());
    boundExpressionStack.Push(disjunction);
}

void ExpressionBinder::EndVisit(Cm::Ast::ConjunctionNode& conjunctionNode)
{
    Cm::BoundTree::BoundExpression* right = boundExpressionStack.Pop();
    Cm::BoundTree::BoundExpression* left = boundExpressionStack.Pop();
    if (!left->GetType()->IsBoolTypeSymbol())
    {
        throw Exception("left operand of conjunction is not Boolean expression", conjunctionNode.Left()->GetSpan());
    }
    if (!right->GetType()->IsBoolTypeSymbol())
    {
        throw Exception("right operand of conjunction is not Boolean expression", conjunctionNode.Right()->GetSpan());
    }
    Cm::BoundTree::BoundConjunction* conjunction = new Cm::BoundTree::BoundConjunction(&conjunctionNode, left, right);
    conjunction->SetType(left->GetType());
    boundExpressionStack.Push(conjunction);
}

void ExpressionBinder::EndVisit(Cm::Ast::BitOrNode& bitOrNode)
{
    BindBinaryOp(&bitOrNode, "operator|");
}

void ExpressionBinder::EndVisit(Cm::Ast::BitXorNode& bitXorNode)
{
    BindBinaryOp(&bitXorNode, "operator^");
}

void ExpressionBinder::EndVisit(Cm::Ast::BitAndNode& bitAndNode)
{
    BindBinaryOp(&bitAndNode, "operator&");
}

void ExpressionBinder::EndVisit(Cm::Ast::EqualNode& equalNode)
{
    BindBinaryOp(&equalNode, "operator==");
}

void ExpressionBinder::EndVisit(Cm::Ast::NotEqualNode& notEqualNode)
{
//  a != b <=> !(a == b);
    BindBinaryOp(&notEqualNode, "operator==");
    BindUnaryOp(&notEqualNode, "operator!");
}

void ExpressionBinder::EndVisit(Cm::Ast::LessNode& lessNode)
{
    BindBinaryOp(&lessNode, "operator<");
}

void ExpressionBinder::EndVisit(Cm::Ast::GreaterNode& greaterNode)
{
//  a > b <=> b < a
    Cm::BoundTree::BoundExpression* right = boundExpressionStack.Pop();
    Cm::BoundTree::BoundExpression* left = boundExpressionStack.Pop();
    boundExpressionStack.Push(right);
    boundExpressionStack.Push(left);
    BindBinaryOp(&greaterNode, "operator<");
}

void ExpressionBinder::EndVisit(Cm::Ast::LessOrEqualNode& lessOrEqualNode)
{
//  a <= b <=> !(b < a)
    Cm::BoundTree::BoundExpression* right = boundExpressionStack.Pop();
    Cm::BoundTree::BoundExpression* left = boundExpressionStack.Pop();
    boundExpressionStack.Push(right);
    boundExpressionStack.Push(left);
    BindBinaryOp(&lessOrEqualNode, "operator<");
    BindUnaryOp(&lessOrEqualNode, "operator!");
}

void ExpressionBinder::EndVisit(Cm::Ast::GreaterOrEqualNode& greaterOrEqualNode)
{
//  a >= b <=> !(a < b)
    BindBinaryOp(&greaterOrEqualNode, "operator<");
    BindUnaryOp(&greaterOrEqualNode, "operator!");
}

void ExpressionBinder::EndVisit(Cm::Ast::ShiftLeftNode& shiftLeftNode)
{
    BindBinaryOp(&shiftLeftNode, "operator<<");
}

void ExpressionBinder::EndVisit(Cm::Ast::ShiftRightNode& shiftRightNode)
{
    BindBinaryOp(&shiftRightNode, "operator>>");
}

void ExpressionBinder::EndVisit(Cm::Ast::AddNode& addNode)
{
    BindBinaryOp(&addNode, "operator+");
}

void ExpressionBinder::EndVisit(Cm::Ast::SubNode& subNode)
{
    BindBinaryOp(&subNode, "operator-");
}

void ExpressionBinder::EndVisit(Cm::Ast::MulNode& mulNode)
{
    BindBinaryOp(&mulNode, "operator*");
}

void ExpressionBinder::EndVisit(Cm::Ast::DivNode& divNode)
{
    BindBinaryOp(&divNode, "operator/");
}

void ExpressionBinder::EndVisit(Cm::Ast::RemNode& remNode)
{
    BindBinaryOp(&remNode, "operator%");
}

void ExpressionBinder::EndVisit(Cm::Ast::PrefixIncNode& prefixIncNode)
{
    Cm::BoundTree::BoundExpression* operand = boundExpressionStack.Pop();
    operand->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
    boundExpressionStack.Push(operand);
    BindUnaryOp(&prefixIncNode, "operator++");
}

void ExpressionBinder::EndVisit(Cm::Ast::PrefixDecNode& prefixDecNode)
{
    Cm::BoundTree::BoundExpression* operand = boundExpressionStack.Pop();
    operand->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
    boundExpressionStack.Push(operand);
    BindUnaryOp(&prefixDecNode, "operator--");
}

void ExpressionBinder::EndVisit(Cm::Ast::UnaryPlusNode& unaryPlusNode) 
{
    BindUnaryOp(&unaryPlusNode, "operator+");
}

void ExpressionBinder::EndVisit(Cm::Ast::UnaryMinusNode& unaryMinusNode)
{
    BindUnaryOp(&unaryMinusNode, "operator-");
}

void ExpressionBinder::EndVisit(Cm::Ast::NotNode& notNode)
{
    BindUnaryOp(&notNode, "operator!");
}

void ExpressionBinder::EndVisit(Cm::Ast::ComplementNode& complementNode) 
{
    BindUnaryOp(&complementNode, "operator~");
}

void ExpressionBinder::Visit(Cm::Ast::AddrOfNode& addrOfNode)
{
    addrOfNode.Subject()->Accept(*this);
    Cm::BoundTree::BoundExpression* operand = boundExpressionStack.Pop();
    operand->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
    boundExpressionStack.Push(operand);
    BindUnaryOp(&addrOfNode, "operator&");
}

void ExpressionBinder::Visit(Cm::Ast::DerefNode& derefNode)
{
    derefNode.Subject()->Accept(*this);
    BindUnaryOp(&derefNode, "operator*");
}

void ExpressionBinder::Visit(Cm::Ast::BooleanLiteralNode& booleanLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
    Cm::Sym::Value* value = new Cm::Sym::BoolValue(booleanLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&booleanLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::SByteLiteralNode& sbyteLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::sbyteId));
    Cm::Sym::Value* value = new Cm::Sym::SByteValue(sbyteLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&sbyteLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::ByteLiteralNode& byteLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::byteId));
    Cm::Sym::Value* value = new Cm::Sym::ByteValue(byteLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&byteLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::ShortLiteralNode& shortLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::shortId));
    Cm::Sym::Value* value = new Cm::Sym::ShortValue(shortLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&shortLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::UShortLiteralNode& ushortLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ushortId));
    Cm::Sym::Value* value = new Cm::Sym::UShortValue(ushortLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&ushortLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::IntLiteralNode& intLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId));
    Cm::Sym::Value* value = new Cm::Sym::IntValue(intLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&intLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::UIntLiteralNode& uintLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::uintId));
    Cm::Sym::Value* value = new Cm::Sym::UIntValue(uintLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&uintLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::LongLiteralNode& longLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::longId));
    Cm::Sym::Value* value = new Cm::Sym::LongValue(longLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&longLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::ULongLiteralNode& ulongLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ulongId));
    Cm::Sym::Value* value = new Cm::Sym::ULongValue(ulongLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&ulongLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::FloatLiteralNode& floatLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::floatId));
    Cm::Sym::Value* value = new Cm::Sym::FloatValue(floatLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&floatLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::DoubleLiteralNode& doubleLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::doubleId));
    Cm::Sym::Value* value = new Cm::Sym::DoubleValue(doubleLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&doubleLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::CharLiteralNode& charLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::charId));
    Cm::Sym::Value* value = new Cm::Sym::CharValue(charLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&charLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::StringLiteralNode& stringLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().MakeConstCharPtrType(stringLiteralNode.GetSpan());
    Cm::Sym::Value* value = new Cm::Sym::StringValue(stringLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&stringLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::NullLiteralNode& nullLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::nullPtrId));
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&nullLiteralNode);
    literalNode->SetType(type);
    Cm::Sym::Value* value = new Cm::Sym::NullValue();
    literalNode->SetValue(value);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::EndVisit(Cm::Ast::DotNode& dotNode)
{
    std::unique_ptr<Cm::BoundTree::BoundExpression> expression(boundExpressionStack.Pop());
    if (expression->IsContainerExpression())
    {
        Cm::BoundTree::BoundContainerExpression* containerExpression = static_cast<Cm::BoundTree::BoundContainerExpression*>(expression.get());
        Cm::Sym::ContainerSymbol* containerSymbol = containerExpression->ContainerSymbol();
        Cm::Sym::ContainerScope* containerScope = containerSymbol->GetContainerScope();
        Cm::Sym::Symbol* symbol = containerScope->Lookup(dotNode.MemberId()->Str());
        if (symbol)
        {
            BindSymbol(&dotNode, symbol);
        }
        else
        {
            throw Exception("symbol '" + containerSymbol->FullName() + "' does not have member '" + dotNode.MemberId()->Str() + "'", dotNode.GetSpan());
        }
    }
    else
    {
        throw Exception("expression '" + expression->SyntaxNode()->FullName() + "' must denote a namespace, class type or enumerated type", dotNode.Subject()->GetSpan());
    }
}

void ExpressionBinder::Visit(Cm::Ast::ArrowNode& arrowNode)
{
    arrowNode.Subject()->Accept(*this);
    BindUnaryOp(&arrowNode, "operator->");
    // todo
}

void ExpressionBinder::BeginVisit(Cm::Ast::InvokeNode& invokeNode)
{
    invokeNode.Subject()->Accept(*this);
    expressionCountStack.push(expressionCount);
    expressionCount = boundExpressionStack.ItemCount();
}

void ExpressionBinder::EndVisit(Cm::Ast::InvokeNode& invokeNode) 
{
    int numArgs = boundExpressionStack.ItemCount() - expressionCount;
    expressionCount = expressionCountStack.top();
    expressionCountStack.pop();
    Cm::BoundTree::BoundExpressionList arguments = boundExpressionStack.Pop(numArgs);
    std::unique_ptr<Cm::BoundTree::BoundExpression> subject(boundExpressionStack.Pop());
    Cm::Sym::FunctionLookupSet functionLookups;
    std::string functionGroupName;
    Cm::Sym::FunctionGroupSymbol* functionGroupSymbol = nullptr;
    if (subject->IsBoundFunctionGroup())
    {
        Cm::BoundTree::BoundFunctionGroup* functionGroup = static_cast<Cm::BoundTree::BoundFunctionGroup*>(subject.get());
        functionGroupSymbol = functionGroup->GetFunctionGroupSymbol();
        functionGroupName = functionGroupSymbol->Name();
    }
    std::vector<Cm::Core::Argument> resolutionArguments;
    for (const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : arguments)
    {
        resolutionArguments.push_back(Cm::Core::Argument(argument->GetArgumentCategory(), argument->GetType()));
        functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, argument->GetType()->GetContainerScope()->ClassOrNsScope()));
    }
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, functionGroupSymbol->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* fun = ResolveOverload(symbolTable, conversionTable, classConversionTable, pointerOpRepository, functionGroupName, resolutionArguments, functionLookups, invokeNode.GetSpan(), conversions);
    CheckAccess(currentFunction->GetFunctionSymbol(), invokeNode.GetSpan(), fun);
    if (conversions.size() != numArgs)
    {
        throw std::runtime_error("wrong number of conversions");
    }
    for (int i = 0; i < numArgs; ++i)
    {
        Cm::Sym::FunctionSymbol* conversionFun = conversions[i];
        if (conversionFun)
        {
            Cm::BoundTree::BoundConversion* conversion = new Cm::BoundTree::BoundConversion(&invokeNode, arguments[i].release(), conversionFun);
            conversion->SetType(conversionFun->GetTargetType());
            arguments[i].reset(conversion);
        }
    }
    Cm::BoundTree::BoundFunctionCall* functionCall = new Cm::BoundTree::BoundFunctionCall(&invokeNode, std::move(arguments));
    functionCall->SetFunction(fun);
    functionCall->SetType(fun->GetReturnType());
    boundExpressionStack.Push(functionCall);
}

void ExpressionBinder::BindSymbol(Cm::Ast::Node* node, Cm::Sym::Symbol* symbol)
{
    Cm::Sym::SymbolType symbolType = symbol->GetSymbolType();
    switch (symbolType)
    {
        case Cm::Sym::SymbolType::constantSymbol:
        {
            Cm::Sym::ConstantSymbol* constantSymbol = static_cast<Cm::Sym::ConstantSymbol*>(symbol);
            BindConstantSymbol(node, constantSymbol);
            break;
        }
        case Cm::Sym::SymbolType::localVariableSymbol:
        {
            Cm::Sym::LocalVariableSymbol* localVariableSymbol = static_cast<Cm::Sym::LocalVariableSymbol*>(symbol);
            BindLocalVariableSymbol(node, localVariableSymbol);
            break;
        }
        case Cm::Sym::SymbolType::memberVariableSymbol:
        {
            Cm::Sym::MemberVariableSymbol* memberVariableSymbol = static_cast<Cm::Sym::MemberVariableSymbol*>(symbol);
            BindMemberVariableSymbol(node, memberVariableSymbol);
            break;
        }
        case Cm::Sym::SymbolType::parameterSymbol:
        {
            Cm::Sym::ParameterSymbol* parameterSymbol = static_cast<Cm::Sym::ParameterSymbol*>(symbol);
            BindParameterSymbol(node, parameterSymbol);
            break;
        }
        case Cm::Sym::SymbolType::classSymbol:
        {
            Cm::Sym::ClassTypeSymbol* classTypeSymbol = static_cast<Cm::Sym::ClassTypeSymbol*>(symbol);
            BindClassTypeSymbol(node, classTypeSymbol);
            break;
        }
        case Cm::Sym::SymbolType::namespaceSymbol:
        {
            Cm::Sym::NamespaceSymbol* namespaceSymbol = static_cast<Cm::Sym::NamespaceSymbol*>(symbol);
            BindNamespaceSymbol(node, namespaceSymbol);
            break;
        }
        case Cm::Sym::SymbolType::enumTypeSymbol:
        {
            Cm::Sym::EnumTypeSymbol* enumTypeSymbol = static_cast<Cm::Sym::EnumTypeSymbol*>(symbol);
            BindEnumTypeSymbol(node, enumTypeSymbol);
            break;
        }
        case Cm::Sym::SymbolType::functionGroupSymbol:
        {
            Cm::Sym::FunctionGroupSymbol* functionGroupSymbol = static_cast<Cm::Sym::FunctionGroupSymbol*>(symbol);
            BindFunctionGroup(node, functionGroupSymbol);
            break;
        }
        default:
        {
            throw Exception("could not bind '" + symbol->FullName() + "'", symbol->GetSpan()); // todo
            break;
        }
    }
}

void ExpressionBinder::BindConstantSymbol(Cm::Ast::Node* idNode, Cm::Sym::ConstantSymbol* constantSymbol)
{
    if (!constantSymbol->Bound())
    {
        Cm::Ast::Node* node = symbolTable.GetNode(constantSymbol);
        if (node->IsConstantNode())
        {
            Cm::Ast::ConstantNode* constantNode = static_cast<Cm::Ast::ConstantNode*>(node);
            Cm::Sym::ContainerScope* constantScope = symbolTable.GetContainerScope(constantNode);
            BindConstant(symbolTable, constantScope, fileScope, constantNode, constantSymbol);
        }
        else
        {
            throw std::runtime_error("not constant node");
        }
    }
    CheckAccess(currentFunction->GetFunctionSymbol(), idNode->GetSpan(), constantSymbol);
    Cm::BoundTree::BoundConstant* boundConstant = new Cm::BoundTree::BoundConstant(idNode, constantSymbol);
    boundConstant->SetType(constantSymbol->GetType());
    boundExpressionStack.Push(boundConstant);
}

void ExpressionBinder::BindLocalVariableSymbol(Cm::Ast::Node* idNode, Cm::Sym::LocalVariableSymbol* localVariableSymbol)
{
    if (!localVariableSymbol->Bound())
    {
        Cm::Ast::Node* node = symbolTable.GetNode(localVariableSymbol);
        if (node->IsConstructionStatementNode())
        {
            Cm::Ast::ConstructionStatementNode* constructionStatementNode = static_cast<Cm::Ast::ConstructionStatementNode*>(node);
            Cm::Sym::ContainerScope* localVariableScope = symbolTable.GetContainerScope(constructionStatementNode);
            BindLocalVariable(symbolTable, containerScope, fileScope, constructionStatementNode, localVariableSymbol);
        }
        else
        {
            throw std::runtime_error("not construction statement node");
        }
    }
    CheckAccess(currentFunction->GetFunctionSymbol(), idNode->GetSpan(), localVariableSymbol);
    Cm::BoundTree::BoundLocalVariable* boundLocalVariable = new Cm::BoundTree::BoundLocalVariable(idNode, localVariableSymbol);
    boundLocalVariable->SetType(localVariableSymbol->GetType());
    boundExpressionStack.Push(boundLocalVariable);
}

void ExpressionBinder::BindMemberVariableSymbol(Cm::Ast::Node* idNode, Cm::Sym::MemberVariableSymbol* memberVariableSymbol)
{
    if (!memberVariableSymbol->Bound())
    {
        Cm::Ast::Node* node = symbolTable.GetNode(memberVariableSymbol);
        if (node->IsMemberVariableNode())
        {
            Cm::Ast::MemberVariableNode* memberVariableNode = static_cast<Cm::Ast::MemberVariableNode*>(node);
            Cm::Sym::ContainerScope* memberVariableScope = symbolTable.GetContainerScope(memberVariableNode);
            BindMemberVariable(symbolTable, containerScope, fileScope, memberVariableNode, memberVariableSymbol);
        }
        else
        {
            throw std::runtime_error("not member variable node");
        }
    }
    CheckAccess(currentFunction->GetFunctionSymbol(), idNode->GetSpan(), memberVariableSymbol);
    Cm::BoundTree::BoundMemberVariable* boundMemberVariable = new Cm::BoundTree::BoundMemberVariable(idNode, memberVariableSymbol);
    boundMemberVariable->SetType(memberVariableSymbol->GetType());
    boundExpressionStack.Push(boundMemberVariable);
}

void ExpressionBinder::BindParameterSymbol(Cm::Ast::Node* idNode, Cm::Sym::ParameterSymbol* parameterSymbol)
{
    CheckAccess(currentFunction->GetFunctionSymbol(), idNode->GetSpan(), parameterSymbol);
    Cm::BoundTree::BoundParameter* boundParameter = new Cm::BoundTree::BoundParameter(idNode, parameterSymbol);
    boundParameter->SetType(parameterSymbol->GetType());
    boundExpressionStack.Push(boundParameter);
}

void ExpressionBinder::BindClassTypeSymbol(Cm::Ast::Node* idNode, Cm::Sym::ClassTypeSymbol* classTypeSymbol)
{
    if (!classTypeSymbol->Bound())
    {
        Cm::Ast::Node* node = symbolTable.GetNode(classTypeSymbol);
        if (node->IsClassNode())
        {
            Cm::Ast::ClassNode* classNode = static_cast<Cm::Ast::ClassNode*>(node);
            Cm::Sym::ContainerScope* classScope = symbolTable.GetContainerScope(classNode);
            BindClass(symbolTable, classScope, fileScope, classNode, classTypeSymbol);
        }
        else
        {
            throw std::runtime_error("not class node");
        }
    }
    Cm::BoundTree::BoundContainerExpression* boundContainer = new Cm::BoundTree::BoundContainerExpression(idNode, classTypeSymbol);
    boundExpressionStack.Push(boundContainer);
}

void ExpressionBinder::BindNamespaceSymbol(Cm::Ast::Node* idNode, Cm::Sym::NamespaceSymbol* namespaceSymbol)
{
    Cm::BoundTree::BoundContainerExpression* boundContainer = new Cm::BoundTree::BoundContainerExpression(idNode, namespaceSymbol);
    boundExpressionStack.Push(boundContainer);
}

void ExpressionBinder::BindEnumTypeSymbol(Cm::Ast::Node* idNode, Cm::Sym::EnumTypeSymbol* enumTypeSymbol)
{
    Cm::BoundTree::BoundContainerExpression* boundContainer = new Cm::BoundTree::BoundContainerExpression(idNode, enumTypeSymbol);
    boundExpressionStack.Push(boundContainer);
}

void ExpressionBinder::BindFunctionGroup(Cm::Ast::Node* idNode, Cm::Sym::FunctionGroupSymbol* functionGroupSymbol)
{
    Cm::BoundTree::BoundFunctionGroup* boundFunctionGroup = new Cm::BoundTree::BoundFunctionGroup(idNode, functionGroupSymbol);
    boundExpressionStack.Push(boundFunctionGroup);
}

void ExpressionBinder::Visit(Cm::Ast::CastNode& castNode)
{
    Cm::Ast::Node* targetTypeExpr = castNode.TargetTypeExpr();
    Cm::Sym::TypeSymbol* toType = ResolveType(symbolTable, containerScope, fileScope, targetTypeExpr);
    castNode.SourceExpr()->Accept(*this);
    Cm::BoundTree::BoundExpression* operand = boundExpressionStack.Pop();
    std::vector<Cm::Core::Argument> resolutionArguments;
    resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, SymbolTable().GetTypeRepository().MakePointerType(toType, castNode.GetSpan())));
    resolutionArguments.push_back(Cm::Core::Argument(operand->GetArgumentCategory(), operand->GetType()));
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, toType->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* convertingCtor = ResolveOverload(SymbolTable(), ConversionTable(), ClassConversionTable(), PointerOpRepository(), "@constructor", resolutionArguments, functionLookups,
        castNode.GetSpan(), Cm::Core::ConversionType::explicit_, conversions);
    Cm::BoundTree::BoundCast* cast = new Cm::BoundTree::BoundCast(&castNode, operand, convertingCtor);
    cast->SetType(toType);
    boundExpressionStack.Push(cast);
}

void ExpressionBinder::Visit(Cm::Ast::IdentifierNode& identifierNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(identifierNode.Str(), Cm::Sym::ScopeLookup::this_and_base_and_parent);
    if (!symbol)
    {
        symbol = fileScope->Lookup(identifierNode.Str());
    }
    if (symbol)
    {
        BindSymbol(&identifierNode, symbol);
    }
    else
    {
        throw Exception("symbol '" + identifierNode.Str() + "' not found");
    }
}

void ExpressionBinder::GenerateTrueExpression(Cm::Ast::Node* node)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
    Cm::Sym::Value* value1 = new Cm::Sym::BoolValue(true);
    Cm::BoundTree::BoundLiteral* literalNode1 = new Cm::BoundTree::BoundLiteral(node);
    literalNode1->SetType(type);
    literalNode1->SetValue(value1);
    boundExpressionStack.Push(literalNode1);
    Cm::Sym::Value* value2 = new Cm::Sym::BoolValue(true);
    Cm::BoundTree::BoundLiteral* literalNode2 = new Cm::BoundTree::BoundLiteral(node);
    literalNode2->SetType(type);
    literalNode2->SetValue(value2);
    boundExpressionStack.Push(literalNode2);
    BindBinaryOp(node, "operator==");
}

} } // namespace Cm::Bind
