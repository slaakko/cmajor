
/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/ExpressionBinder.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/Evaluator.hpp>
#include <Cm.Bind/OverloadResolution.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Bind/Constant.hpp>
#include <Cm.Bind/Class.hpp>
#include <Cm.Bind/LocalVariable.hpp>
#include <Cm.Bind/MemberVariable.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/Parameter.hpp>
#include <Cm.Bind/Function.hpp>
#include <Cm.Bind/Enumeration.hpp>
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

void PrepareFunctionArguments(Cm::Sym::FunctionSymbol* fun, Cm::BoundTree::BoundExpressionList& arguments, bool firstArgByRef, Cm::Core::IrClassTypeRepository& irClassTypeRepository)
{
    if (int(fun->Parameters().size()) != arguments.Count())
    {
        throw std::runtime_error("wrong number of arguments");
    }
    int n = arguments.Count();
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::ParameterSymbol* parameter = fun->Parameters()[i];
        Cm::Sym::TypeSymbol* paramType = parameter->GetType();
        Cm::Sym::TypeSymbol* paramBaseType = paramType->GetBaseType();
        if (paramBaseType->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* paramClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(paramBaseType);
            irClassTypeRepository.AddClassType(paramClassType);
        }
        Cm::BoundTree::BoundExpression* argument = arguments[i].get();
        Cm::Sym::TypeSymbol* argumentBaseType = argument->GetType()->GetBaseType();
        if (argumentBaseType->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* argumentClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(argumentBaseType);
            irClassTypeRepository.AddClassType(argumentClassType);
        }
        if (!fun->IsBasicTypeOp())
        {
            if (paramType->IsNonConstReferenceType())
            {
                argument->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
            }
            else if (paramType->IsConstReferenceType())
            {
                argument->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
            }
            else if (paramType->IsRvalueRefType())
            {
                argument->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
            }
            else if (firstArgByRef && i == 0)
            {
                argument->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
            }
            else if (paramType->IsClassTypeSymbol())
            {
                argument->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
            }
        }
        else if (firstArgByRef && i == 0)
        {
            argument->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        }
        else if (paramType->IsClassTypeSymbol())
        {
            argument->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        }
    }
}

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
    Cm::Core::DerivedTypeOpRepository& derivedTypeOpRepository_, Cm::Core::SynthesizedClassFunRepository& synthesizedClassFunRepository_, Cm::Core::StringRepository& stringRepository_, 
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, Cm::BoundTree::BoundFunction* currentFunction_) : 
    Cm::Ast::Visitor(true, true), symbolTable(symbolTable_), conversionTable(conversionTable_), classConversionTable(classConversionTable_), derivedTypeOpRepository(derivedTypeOpRepository_),
    synthesizedClassFunRepository(synthesizedClassFunRepository_), stringRepository(stringRepository_), irClassTypeRepository(irClassTypeRepository_), containerScope(containerScope_), 
    fileScope(fileScope_), currentFunction(currentFunction_), expressionCount(0)
{
}

Cm::BoundTree::BoundExpressionList ExpressionBinder::GetExpressions()
{
    return boundExpressionStack.GetExpressions();
}

void ExpressionBinder::BindUnaryOp(Cm::Ast::Node* node, const std::string& opGroupName)
{
    Cm::BoundTree::BoundExpression* operand = boundExpressionStack.Pop();
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionLookupSet memFunLookups;
    Cm::Sym::TypeSymbol* plainOperandType = symbolTable.GetTypeRepository().MakePlainType(operand->GetType());
    memFunLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base, plainOperandType->GetBaseType()->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Core::Argument> memFunArguments;
    memFunArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, symbolTable.GetTypeRepository().MakePointerType(plainOperandType, node->GetSpan())));
    Cm::Sym::FunctionSymbol* fun = ResolveOverload(symbolTable, conversionTable, classConversionTable, derivedTypeOpRepository, synthesizedClassFunRepository, opGroupName, memFunArguments, memFunLookups,
        node->GetSpan(), conversions, OverloadResolutionFlags::nothrow | OverloadResolutionFlags::bindOnlyMemberFunctions);
    if (!fun)
    {
        Cm::Sym::FunctionLookupSet freeFunLookups;
        freeFunLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, containerScope->ClassOrNsScope()));
        freeFunLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, operand->GetType()->GetContainerScope()->ClassOrNsScope()));
        std::vector<Cm::Core::Argument> freeFunArguments;
        freeFunArguments.push_back(Cm::Core::Argument(operand->GetArgumentCategory(), operand->GetType()));
        fun = ResolveOverload(symbolTable, conversionTable, classConversionTable, derivedTypeOpRepository, synthesizedClassFunRepository, opGroupName, freeFunArguments, freeFunLookups, node->GetSpan(), 
            conversions);
    }
    PrepareFunctionSymbol(fun, node->GetSpan());
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
    Cm::Sym::FunctionSymbol* fun = ResolveOverload(symbolTable, conversionTable, classConversionTable, derivedTypeOpRepository, synthesizedClassFunRepository, opGroupName, arguments, functionLookups, 
        node->GetSpan(), conversions);
    PrepareFunctionSymbol(fun, node->GetSpan());
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
        throw Cm::Core::Exception("left operand of disjunction is not Boolean expression", disjunctionNode.Left()->GetSpan());
    }
    if (!right->GetType()->IsBoolTypeSymbol())
    {
        throw Cm::Core::Exception("right operand of disjunction is not Boolean expression", disjunctionNode.Right()->GetSpan());
    }
    Cm::BoundTree::BoundDisjunction* disjunction = new Cm::BoundTree::BoundDisjunction(&disjunctionNode, left, right);
    disjunction->SetType(left->GetType());
    Cm::Sym::LocalVariableSymbol* resultVar = currentFunction->CreateTempLocalVariable(disjunction->GetType());
    disjunction->SetResultVar(resultVar);
    boundExpressionStack.Push(disjunction);

}

void ExpressionBinder::EndVisit(Cm::Ast::ConjunctionNode& conjunctionNode)
{
    Cm::BoundTree::BoundExpression* right = boundExpressionStack.Pop();
    Cm::BoundTree::BoundExpression* left = boundExpressionStack.Pop();
    if (!left->GetType()->IsBoolTypeSymbol())
    {
        throw Cm::Core::Exception("left operand of conjunction is not Boolean expression", conjunctionNode.Left()->GetSpan());
    }
    if (!right->GetType()->IsBoolTypeSymbol())
    {
        throw Cm::Core::Exception("right operand of conjunction is not Boolean expression", conjunctionNode.Right()->GetSpan());
    }
    Cm::BoundTree::BoundConjunction* conjunction = new Cm::BoundTree::BoundConjunction(&conjunctionNode, left, right);
    conjunction->SetType(left->GetType());
    Cm::Sym::LocalVariableSymbol* resultVar = currentFunction->CreateTempLocalVariable(conjunction->GetType());
    conjunction->SetResultVar(resultVar);
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
    bool isDerefThis = derefNode.Subject()->IsThisNode();   // '*this' is special
    derefNode.Subject()->Accept(*this);
    BindUnaryOp(&derefNode, "operator*");
    if (isDerefThis)
    {
        Cm::BoundTree::BoundExpression* derefExpr = boundExpressionStack.Pop();
        Cm::Sym::TypeSymbol* type = derefExpr->GetType();
        derefExpr->SetType(symbolTable.GetTypeRepository().MakeReferenceType(type, derefNode.GetSpan()));
        boundExpressionStack.Push(derefExpr);
    }
}

void ExpressionBinder::Visit(Cm::Ast::PostfixIncNode& postfixIncNode)
{
    postfixIncNode.Subject()->Accept(*this);
    Cm::BoundTree::BoundExpression* value = boundExpressionStack.Pop();
    postfixIncNode.Subject()->Accept(*this);
    Cm::BoundTree::BoundExpression* incOperand = boundExpressionStack.Pop();
    incOperand->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
    boundExpressionStack.Push(incOperand);
    BindUnaryOp(&postfixIncNode, "operator++");
    Cm::BoundTree::BoundExpression* incExpr = boundExpressionStack.Pop();
    Cm::BoundTree::BoundSimpleStatement* increment = new Cm::BoundTree::BoundSimpleStatement(&postfixIncNode);
    increment->SetExpression(incExpr);
    Cm::BoundTree::BoundPostfixIncDecExpr* postfixIncExpr = new Cm::BoundTree::BoundPostfixIncDecExpr(&postfixIncNode, value, increment);
    postfixIncExpr->SetType(value->GetType());
    boundExpressionStack.Push(postfixIncExpr);
}

void ExpressionBinder::Visit(Cm::Ast::PostfixDecNode& postfixDecNode)
{
    postfixDecNode.Subject()->Accept(*this);
    Cm::BoundTree::BoundExpression* value = boundExpressionStack.Pop();
    postfixDecNode.Subject()->Accept(*this);
    Cm::BoundTree::BoundExpression* decOperand = boundExpressionStack.Pop();
    decOperand->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
    boundExpressionStack.Push(decOperand);
    BindUnaryOp(&postfixDecNode, "operator--");
    Cm::BoundTree::BoundExpression* decExpr = boundExpressionStack.Pop();
    Cm::BoundTree::BoundSimpleStatement* decrement = new Cm::BoundTree::BoundSimpleStatement(&postfixDecNode);
    decrement->SetExpression(decExpr);
    Cm::BoundTree::BoundPostfixIncDecExpr* postfixDecExpr = new Cm::BoundTree::BoundPostfixIncDecExpr(&postfixDecNode, value, decrement);
    postfixDecExpr->SetType(value->GetType());
    boundExpressionStack.Push(postfixDecExpr);
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
    int id = stringRepository.Install(stringLiteralNode.Value());
    Cm::BoundTree::BoundStringLiteral* literalNode = new Cm::BoundTree::BoundStringLiteral(&stringLiteralNode, id);
    literalNode->SetType(type);
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
            throw Cm::Core::Exception("symbol '" + containerSymbol->FullName() + "' does not have member '" + dotNode.MemberId()->Str() + "'", dotNode.GetSpan());
        }
    }
    else
    {
        Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().MakePlainType(expression->GetType());
        if (type->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* classType = static_cast<Cm::Sym::ClassTypeSymbol*>(type);
            Cm::Sym::ContainerScope* containerScope = classType->GetContainerScope();
            Cm::Sym::Symbol* symbol = containerScope->Lookup(dotNode.MemberId()->Str());
            if (symbol)
            {
                Cm::BoundTree::BoundExpression* classObject = expression.release();
                BindSymbol(&dotNode, symbol);
                std::unique_ptr<Cm::BoundTree::BoundExpression> symbolExpr(boundExpressionStack.Pop());
                if (symbolExpr->IsBoundFunctionGroup())
                {
                    boundExpressionStack.Push(symbolExpr.release());
                    classObject->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
                    classObject->SetFlag(Cm::BoundTree::BoundNodeFlags::classObjectArg);
                    boundExpressionStack.Push(classObject);
                }
                else if (symbolExpr->IsBoundMemberVariable())
                {
                    Cm::BoundTree::BoundMemberVariable* memberVariable = static_cast<Cm::BoundTree::BoundMemberVariable*>(symbolExpr.release());
                    classObject->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
                    memberVariable->SetClassObject(classObject);
                    boundExpressionStack.Push(memberVariable);
                }
                else
                { 
                    throw Cm::Core::Exception("symbol '" + symbolExpr->SyntaxNode()->Name() + "' does not denote a member variable or a function group", dotNode.GetSpan(), symbolExpr->SyntaxNode()->GetSpan());
                }
            }
            else
            {
                throw Cm::Core::Exception("class '" + classType->FullName() + "' does not have member '" + dotNode.MemberId()->Str() + "'", dotNode.GetSpan());
            }
        }
        else
        {
            throw Cm::Core::Exception("expression '" + expression->SyntaxNode()->Name() + "' must denote a namespace, class type, enumerated type, or a class type object", dotNode.Subject()->GetSpan());
        }
    }
}

void ExpressionBinder::BindArrow(Cm::Ast::Node* node, const std::string& memberId)
{
    BindUnaryOp(node, "operator->");
    std::unique_ptr<Cm::BoundTree::BoundExpression> boundUnaryOpExpr(boundExpressionStack.Pop());
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().MakePlainType(boundUnaryOpExpr->GetType());
    if (type->IsPointerToClassType() || type->IsClassTypeSymbol())
    {
        if (type->IsPointerType())
        {
            type = type->GetBaseType();
        }
        type = symbolTable.GetTypeRepository().MakePlainType(type);
        if (type->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* classTypeSymbol = static_cast<Cm::Sym::ClassTypeSymbol*>(type);
            if (boundUnaryOpExpr->IsBoundUnaryOp())
            {
                Cm::BoundTree::BoundUnaryOp* boundUnaryOp = static_cast<Cm::BoundTree::BoundUnaryOp*>(boundUnaryOpExpr.get());
                Cm::Sym::FunctionSymbol* function = boundUnaryOp->GetFunction();
                if (function->IsMemberFunctionSymbol())
                {
                    boundExpressionStack.Push(boundUnaryOpExpr.release());
                    BindArrow(node, memberId);
                }
                else
                {
                    Cm::BoundTree::BoundExpression* classObject = boundUnaryOp->ReleaseOperand();
                    Cm::Sym::ContainerScope* containerScope = classTypeSymbol->GetContainerScope();
                    Cm::Sym::Symbol* symbol = containerScope->Lookup(memberId);
                    if (symbol)
                    {
                        BindSymbol(node, symbol);
                        std::unique_ptr<Cm::BoundTree::BoundExpression> symbolExpr(boundExpressionStack.Pop());
                        if (symbolExpr->IsBoundFunctionGroup())
                        {
                            boundExpressionStack.Push(symbolExpr.release());
                            classObject->SetFlag(Cm::BoundTree::BoundNodeFlags::classObjectArg);
                            boundExpressionStack.Push(classObject);
                        }
                        else if (symbolExpr->IsBoundMemberVariable())
                        {
                            Cm::BoundTree::BoundMemberVariable* memberVariable = static_cast<Cm::BoundTree::BoundMemberVariable*>(symbolExpr.release());
                            memberVariable->SetClassObject(classObject);
                            boundExpressionStack.Push(memberVariable);
                        }
                        else
                        {
                            throw Cm::Core::Exception("symbol '" + symbolExpr->SyntaxNode()->Name() + "' does not denote a member variable or a function group", node->GetSpan(), symbolExpr->SyntaxNode()->GetSpan());
                        }
                    }
                    else
                    {
                        throw Cm::Core::Exception("class '" + classTypeSymbol->FullName() + "' does not have member '" + memberId + "'", node->GetSpan());
                    }
                }
            }
            else
            {
                throw Cm::Core::Exception("operator->() not bound to unary operator", node->GetSpan());
            }
        }
        else
        {
            throw Cm::Core::Exception("operator->() must eventually return class type or pointer to class type", node->GetSpan());
        }
    }
    else
    {
        throw Cm::Core::Exception("operator->() must eventually return class type or pointer to class type", node->GetSpan());
    }
}

void ExpressionBinder::Visit(Cm::Ast::ArrowNode& arrowNode)
{
    arrowNode.Subject()->Accept(*this);
    BindArrow(&arrowNode, arrowNode.MemberId()->Str());
}

void ExpressionBinder::BeginVisit(Cm::Ast::InvokeNode& invokeNode)
{
    invokeNode.Subject()->Accept(*this);
    std::unique_ptr<Cm::BoundTree::BoundExpression> subject(boundExpressionStack.Pop());
    expressionCountStack.push(expressionCount);
    expressionCount = boundExpressionStack.ItemCount();
    if (!subject->GetFlag(Cm::BoundTree::BoundNodeFlags::classObjectArg))
    {
        ++expressionCount;
    }
    boundExpressionStack.Push(subject.release());
}

void ExpressionBinder::EndVisit(Cm::Ast::InvokeNode& invokeNode)
{
    int numArgs = boundExpressionStack.ItemCount() - expressionCount;
    BindInvoke(&invokeNode, numArgs);
}

void ExpressionBinder::Visit(Cm::Ast::IndexNode& indexNode)
{
    indexNode.Subject()->Accept(*this);
    std::unique_ptr<Cm::BoundTree::BoundExpression> subject(boundExpressionStack.Pop());
    indexNode.Index()->Accept(*this);
    std::unique_ptr<Cm::BoundTree::BoundExpression> index(boundExpressionStack.Pop());
    Cm::Sym::TypeSymbol* subjectType = subject->GetType();
    Cm::Sym::TypeSymbol* plainSubjectType = symbolTable.GetTypeRepository().MakePlainType(subjectType);
    if (plainSubjectType->IsPointerType())
    {
        BindIndexPointer(&indexNode, subject.release(), index.release());
    }
    else if (plainSubjectType->IsClassTypeSymbol())
    {
        BindIndexClass(&indexNode, subject.release(), index.release());
    }
    else
    {
        throw Cm::Core::Exception("subscript operator can be applied only to pointer or class type subject", indexNode.GetSpan());
    }
}

void ExpressionBinder::BindIndexPointer(Cm::Ast::Node* indexNode, Cm::BoundTree::BoundExpression* subject, Cm::BoundTree::BoundExpression* index)
{
    boundExpressionStack.Push(subject);
    boundExpressionStack.Push(index);
    BindBinaryOp(indexNode, "operator+");
    BindUnaryOp(indexNode, "operator*");
}

void ExpressionBinder::BindIndexClass(Cm::Ast::Node* indexNode, Cm::BoundTree::BoundExpression* subject, Cm::BoundTree::BoundExpression* index)
{
    Cm::Sym::FunctionGroupSymbol subscriptFunctionGroup(indexNode->GetSpan(), "operator[]", symbolTable.GetContainerScope(indexNode));
    Cm::BoundTree::BoundFunctionGroup* boundSubscriptFunctionGroup = new Cm::BoundTree::BoundFunctionGroup(indexNode, &subscriptFunctionGroup);
    boundExpressionStack.Push(boundSubscriptFunctionGroup);
    subject->SetFlag(Cm::BoundTree::BoundNodeFlags::classObjectArg | Cm::BoundTree::BoundNodeFlags::lvalue);
    boundExpressionStack.Push(subject);
    boundExpressionStack.Push(index);
    expressionCountStack.push(0);
    BindInvoke(indexNode, 2);
}

void ExpressionBinder::BindInvoke(Cm::Ast::Node* node, int numArgs)
{
    bool generateVirtualCall = false;
    expressionCount = expressionCountStack.top();
    expressionCountStack.pop();
    Cm::BoundTree::BoundExpressionList arguments = boundExpressionStack.Pop(numArgs);
    std::unique_ptr<Cm::BoundTree::BoundExpression> subject(boundExpressionStack.Pop());
    std::string functionGroupName;
    Cm::Sym::FunctionGroupSymbol* functionGroupSymbol = nullptr;
    if (subject->IsBoundFunctionGroup())
    {
        Cm::BoundTree::BoundFunctionGroup* functionGroup = static_cast<Cm::BoundTree::BoundFunctionGroup*>(subject.get());
        functionGroupSymbol = functionGroup->GetFunctionGroupSymbol();
        functionGroupName = functionGroupSymbol->Name();
    }
    Cm::Sym::FunctionSymbol* fun = nullptr;
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    bool firstArgByRef = false;
    if (currentFunction->GetFunctionSymbol()->IsMemberFunctionSymbol())
    {
        fun = BindInvokeMemFun(node, conversions, arguments, firstArgByRef, generateVirtualCall, functionGroupName, numArgs);
    }
    if (!fun)
    {
        fun = BindInvokeFun(node, conversions, arguments, firstArgByRef, generateVirtualCall, functionGroupSymbol);
    }
    PrepareFunctionSymbol(fun, node->GetSpan());
    if (conversions.size() != numArgs)
    {
        throw std::runtime_error("wrong number of conversions");
    }
    for (int i = 0; i < numArgs; ++i)
    {
        Cm::Sym::FunctionSymbol* conversionFun = conversions[i];
        if (conversionFun)
        {
            Cm::BoundTree::BoundConversion* conversion = new Cm::BoundTree::BoundConversion(node, arguments[i].release(), conversionFun);
            conversion->SetType(conversionFun->GetTargetType());
            arguments[i].reset(conversion);
        }
    }
    PrepareFunctionArguments(fun, arguments, firstArgByRef && fun->IsMemberFunctionSymbol(), irClassTypeRepository);
    Cm::BoundTree::BoundFunctionCall* functionCall = new Cm::BoundTree::BoundFunctionCall(node, std::move(arguments));
    functionCall->SetFunction(fun);
    functionCall->SetType(fun->GetReturnType());
    if (generateVirtualCall)
    {
        functionCall->SetFlag(Cm::BoundTree::BoundNodeFlags::genVirtualCall);
    }
    boundExpressionStack.Push(functionCall);
}

Cm::Sym::FunctionSymbol* ExpressionBinder::BindInvokeMemFun(Cm::Ast::Node* node, std::vector<Cm::Sym::FunctionSymbol*>& conversions, Cm::BoundTree::BoundExpressionList& arguments, bool& firstArgByRef, 
    bool& generateVirtualCall, const std::string& functionGroupName, int& numArgs)
{
    Cm::Sym::FunctionLookupSet memberFunLookups;
    std::vector<Cm::Core::Argument> memberFunResolutionArguments;
    Cm::Sym::ParameterSymbol* thisParam = currentFunction->GetFunctionSymbol()->Parameters()[0];
    Cm::Sym::TypeSymbol* thisParamType = thisParam->GetType();
    memberFunResolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, thisParamType));
    memberFunLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base, thisParamType->GetBaseType()->GetContainerScope()->ClassOrNsScope()));
    bool first = true;
    for (const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : arguments)
    {
        if (argument->GetFlag(Cm::BoundTree::BoundNodeFlags::classObjectArg) && argument->GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue))
        {
            memberFunResolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, symbolTable.GetTypeRepository().MakePointerType(argument->GetType()->GetBaseType(), 
                node->GetSpan())));
            if (first)
            {
                firstArgByRef = true;
            }
        }
        else
        {
            memberFunResolutionArguments.push_back(Cm::Core::Argument(argument->GetArgumentCategory(), argument->GetType()));
        }
        if (first)
        {
            first = false;
        }
    }
    Cm::Sym::FunctionSymbol* fun = ResolveOverload(symbolTable, conversionTable, classConversionTable, derivedTypeOpRepository, synthesizedClassFunRepository, functionGroupName, 
        memberFunResolutionArguments, memberFunLookups, node->GetSpan(), conversions, OverloadResolutionFlags::nothrow);
    if (fun)
    {
        Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
        boundThisParam->SetType(thisParam->GetType());
        arguments.InsertFront(boundThisParam);
        ++numArgs;
        if (fun->IsVirtualAbstractOrOverride())
        {
            generateVirtualCall = true;
        }
    }
    return fun;
}

Cm::Sym::FunctionSymbol* ExpressionBinder::BindInvokeFun(Cm::Ast::Node* node, std::vector<Cm::Sym::FunctionSymbol*>& conversions, Cm::BoundTree::BoundExpressionList& arguments,
    bool& firstArgByRef, bool& generateVirtualCall, Cm::Sym::FunctionGroupSymbol* functionGroupSymbol)
{
    Cm::Sym::FunctionLookupSet functionLookups;
    std::vector<Cm::Core::Argument> resolutionArguments;
    bool first = true;
    bool firstArgIsPointerOrReference = false;
    bool firstArgIsThisOrBase = false;
    for (const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : arguments)
    {
        functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, argument->GetType()->GetContainerScope()->ClassOrNsScope()));
        if (argument->GetFlag(Cm::BoundTree::BoundNodeFlags::classObjectArg) && argument->GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue))
        {
            resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, symbolTable.GetTypeRepository().MakePointerType(argument->GetType()->GetBaseType(), node->GetSpan())));
            if (first)
            {
                firstArgByRef = true;
            }
        }
        else
        {
            resolutionArguments.push_back(Cm::Core::Argument(argument->GetArgumentCategory(), argument->GetType()));
        }
        if (first)
        {
            first = false;
            if (argument->GetType()->IsReferenceType() || argument->GetType()->IsPointerType() || argument->GetType()->IsRvalueRefType())
            {
                firstArgIsPointerOrReference = true;
            }
            if (argument->GetFlag(Cm::BoundTree::BoundNodeFlags::argIsThisOrBase))
            {
                firstArgIsThisOrBase = true;
            }
        }
    }
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, functionGroupSymbol->GetContainerScope()->ClassOrNsScope()));
    Cm::Sym::FunctionSymbol* fun = ResolveOverload(symbolTable, conversionTable, classConversionTable, derivedTypeOpRepository, synthesizedClassFunRepository, 
        functionGroupSymbol->Name(), resolutionArguments, functionLookups, node->GetSpan(), conversions);
    if (fun->IsVirtualAbstractOrOverride() && firstArgIsPointerOrReference && !firstArgIsThisOrBase)
    {
        generateVirtualCall = true;
    }
    return fun;
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
        case Cm::Sym::SymbolType::enumConstantSymbol:
        {
            Cm::Sym::EnumConstantSymbol* enumConstantSymbol = static_cast<Cm::Sym::EnumConstantSymbol*>(symbol);
            BindEnumConstantSymbol(node, enumConstantSymbol);
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
            throw Cm::Core::Exception("could not bind '" + symbol->FullName() + "'", symbol->GetSpan()); // todo
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

void ExpressionBinder::BindEnumConstantSymbol(Cm::Ast::Node* idNode, Cm::Sym::EnumConstantSymbol* enumConstantSymbol)
{
    Cm::Ast::Node* node = symbolTable.GetNode(enumConstantSymbol);
    Cm::Sym::TypeSymbol* enumType = nullptr;
    if (node->IsEnumConstantNode())
    {
        Cm::Ast::EnumConstantNode* enumConstantNode = static_cast<Cm::Ast::EnumConstantNode*>(node);
        Cm::Sym::ContainerScope* enumConstantScope = symbolTable.GetContainerScope(enumConstantNode);
        enumType = static_cast<Cm::Sym::EnumTypeSymbol*>(containerScope->Container());
        BindEnumConstant(symbolTable, enumConstantScope, fileScope, enumConstantNode);
    }
    else 
    {
        throw std::runtime_error("not enum constant node");
    }
    Cm::BoundTree::BoundEnumConstant* boundEnumConstant = new Cm::BoundTree::BoundEnumConstant(idNode, enumConstantSymbol);
    boundEnumConstant->SetType(enumType);
    boundExpressionStack.Push(boundEnumConstant);
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
    Cm::Sym::FunctionSymbol* convertingCtor = ResolveOverload(SymbolTable(), ConversionTable(), ClassConversionTable(), DerivedTypeOpRepository(), SynthesizedClassFunRepository(), "@constructor", 
        resolutionArguments, functionLookups, castNode.GetSpan(), conversions, Cm::Core::ConversionType::explicit_, OverloadResolutionFlags::none);
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
        throw Cm::Core::Exception("symbol '" + identifierNode.Str() + "' not found");
    }
}

void ExpressionBinder::Visit(Cm::Ast::ThisNode& thisNode)
{
    if (currentFunction->GetFunctionSymbol()->IsMemberFunctionSymbol())
    {
        Cm::Sym::ParameterSymbol* thisParam = currentFunction->GetFunctionSymbol()->Parameters()[0];
        Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(&thisNode, thisParam);
        boundThisParam->SetType(thisParam->GetType());
        boundExpressionStack.Push(boundThisParam);
        boundThisParam->SetFlag(Cm::BoundTree::BoundNodeFlags::argIsThisOrBase);
    }
    else
    {
        throw Cm::Core::Exception("'this' can be used only in member function context", thisNode.GetSpan());
    }
}

void ExpressionBinder::Visit(Cm::Ast::BaseNode& baseNode)
{
    if (currentFunction->GetFunctionSymbol()->IsMemberFunctionSymbol())
    {
        Cm::Sym::ParameterSymbol* thisParam = currentFunction->GetFunctionSymbol()->Parameters()[0];
        Cm::Sym::ClassTypeSymbol* classType = static_cast<Cm::Sym::ClassTypeSymbol*>(thisParam->GetType()->GetBaseType());
        if (classType->BaseClass())
        {
            Cm::Sym::ClassTypeSymbol* baseClassType = classType->BaseClass();
            Cm::Sym::TypeSymbol* baseClassPtrType = SymbolTable().GetTypeRepository().MakePointerType(baseClassType, baseNode.GetSpan());
            Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(&baseNode, thisParam);
            boundThisParam->SetType(thisParam->GetType());
            Cm::Sym::FunctionSymbol* conversionFun = ClassConversionTable().MakeBaseClassDerivedClassConversion(baseClassPtrType, thisParam->GetType(), 1, baseNode.GetSpan());
            Cm::BoundTree::BoundConversion* thisAsBase = new Cm::BoundTree::BoundConversion(&baseNode, boundThisParam, conversionFun);
            thisAsBase->SetType(baseClassPtrType);
            thisAsBase->SetFlag(Cm::BoundTree::BoundNodeFlags::argIsThisOrBase);
            boundExpressionStack.Push(thisAsBase);
        }
        else
        {
            throw Cm::Core::Exception("class '" + classType->FullName() + "' does not have a base class", baseNode.GetSpan(), classType->GetSpan());
        }
    }
    else
    {
        throw Cm::Core::Exception("'base' can be used only in member function context", baseNode.GetSpan());
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

void ExpressionBinder::PrepareFunctionSymbol(Cm::Sym::FunctionSymbol* fun, const Cm::Parsing::Span& span)
{
    if (!fun->IsBasicTypeOp())
    {
        CheckAccess(currentFunction->GetFunctionSymbol(), span, fun);
        for (Cm::Sym::ParameterSymbol* param : fun->Parameters())
        {
            Cm::Sym::TypeSymbol* paramBaseType = param->GetType()->GetBaseType();
            if (paramBaseType->IsClassTypeSymbol())
            {
                Cm::Sym::ClassTypeSymbol* classTypeSymbol = static_cast<Cm::Sym::ClassTypeSymbol*>(paramBaseType);
                irClassTypeRepository.AddClassType(classTypeSymbol);
            }
        }
    }
}

} } // namespace Cm::Bind
