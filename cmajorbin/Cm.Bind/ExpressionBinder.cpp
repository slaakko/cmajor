
/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
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
#include <Cm.Bind/DelegateTypeOpRepository.hpp>
#include <Cm.Bind/ClassDelegateTypeOpRepository.hpp>
#include <Cm.Core/Argument.hpp>
#include <Cm.Core/GlobalSettings.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/FunctionGroupSymbol.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.Ast/Expression.hpp>
#include <Cm.Ast/Literal.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/BasicType.hpp>
#include <Cm.Ast/Clone.hpp>
#include <Cm.Parser/FileRegistry.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Bind {

using Cm::Parsing::Span;

void PrepareArguments(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::BoundTree::BoundFunction* currentFunction, 
    Cm::Sym::TypeSymbol* returnType, const std::vector<Cm::Sym::ParameterSymbol*>& parameters, Cm::BoundTree::BoundExpressionList& arguments, bool firstArgByRef, 
    Cm::Core::IrClassTypeRepository& irClassTypeRepository, bool isBasicTypeOp)
{
    if (int(parameters.size()) != arguments.Count())
    {
        throw std::runtime_error("wrong number of arguments");
    }
    if (!boundCompileUnit.IsPrebindCompileUnit())
    {
        if (returnType && returnType->GetBaseType()->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* returnClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(returnType->GetBaseType());
            AddClassTypeToIrClassTypeRepository(returnClassType, boundCompileUnit, containerScope);
        }
    }
    int n = arguments.Count();
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::ParameterSymbol* parameter = parameters[i];
        Cm::Sym::TypeSymbol* paramType = parameter->GetType();
        Cm::Sym::TypeSymbol* paramBaseType = paramType->GetBaseType();
        if (!boundCompileUnit.IsPrebindCompileUnit())
        {
            if (paramBaseType->IsClassTypeSymbol())
            {
                Cm::Sym::ClassTypeSymbol* paramClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(paramBaseType);
                AddClassTypeToIrClassTypeRepository(paramClassType, boundCompileUnit, containerScope);
            }
        }
        Cm::BoundTree::BoundExpression* argument = arguments[i].get();
        Cm::Sym::TypeSymbol* argumentBaseType = argument->GetType()->GetBaseType();
        if (!boundCompileUnit.IsPrebindCompileUnit())
        {
            if (argumentBaseType->IsClassTypeSymbol())
            {
                Cm::Sym::ClassTypeSymbol* argumentClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(argumentBaseType);
                AddClassTypeToIrClassTypeRepository(argumentClassType, boundCompileUnit, containerScope);
            }
        }
        if (!isBasicTypeOp)
        {
            if (paramType->IsNonConstReferenceType())
            {
				if (!argument->GetType()->IsReferenceType())
				{
					argument->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
				}
            }
            else if (paramType->IsConstReferenceType())
            {
                if (argument->GetType()->IsValueTypeSymbol() &&
                    (argument->IsBoundLiteral() || argument->IsConstant() || argument->IsEnumConstant() || argument->IsBoundUnaryOp() || argument->IsBoundBinaryOp() || 
                    argument->IsBoundFunctionCall() || argument->IsBoundConversion() || argument->IsBoundCast()))
                {
                    if (!currentFunction)
                    {
                        throw std::runtime_error("current function not set in prepare function arguments");
                    }
                    argument = arguments[i].release();
                    std::vector<Cm::Core::Argument> resolutionArguments;
                    resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(argument->GetType(), 
                        argument->SyntaxNode()->GetSpan())));
                    resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, argument->GetType()));
                    Cm::Sym::FunctionLookupSet resolutionLookups;
                    resolutionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, containerScope));
                    resolutionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::fileScopes, nullptr));
                    std::vector<Cm::Sym::FunctionSymbol*> conversions;
                    Cm::Sym::FunctionSymbol* basicTypeCopyCtor = ResolveOverload(containerScope, boundCompileUnit, "@constructor", resolutionArguments, resolutionLookups,
                        argument->SyntaxNode()->GetSpan(), conversions);
                    Cm::Sym::LocalVariableSymbol* temporary = currentFunction->CreateTempLocalVariable(argument->GetType());
                    temporary->SetSid(boundCompileUnit.SymbolTable().GetSid());
                    Cm::BoundTree::BoundExpression* boundTemporary = new Cm::BoundTree::BoundLocalVariable(argument->SyntaxNode(), temporary);
                    boundTemporary->SetType(argument->GetType());
                    Cm::BoundTree::BoundConversion* conversion = new Cm::BoundTree::BoundConversion(argument->SyntaxNode(), argument, basicTypeCopyCtor);
                    boundTemporary->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
                    conversion->SetBoundTemporary(boundTemporary);
                    conversion->SetType(paramType);
                    arguments[i].reset(conversion);
                }
                else
                {
					if (!argument->GetType()->IsReferenceType())
					{
						argument->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
					}
                }
            }
            else if (paramType->IsRvalueRefType())
            {
                argument->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
            }
            else if (firstArgByRef && i == 0)
            {
                argument->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
            }
            else if (paramType->IsClassTypeSymbol() || paramType->IsArrayType() || argument->GetType()->IsArrayType())
            {
                argument->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
            }
            else if (!paramType->IsReferenceType() && !paramType->IsRvalueRefType() && argument->GetType()->IsNonClassReferenceType())
            {
                argument->SetFlag(Cm::BoundTree::BoundNodeFlags::refByValue);
            }
        }
        else if (firstArgByRef && i == 0)
        {
            argument->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        }
        else 
        {
            if (paramType->IsClassTypeSymbol() || paramType->IsArrayType() || argument->GetType()->IsArrayType() || 
                (argument->GetFlag(Cm::BoundTree::BoundNodeFlags::indexArray) && argument->GetFlag(Cm::BoundTree::BoundNodeFlags::argByRef)))
            {
                argument->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
            }
            else
            {
                if (!paramType->IsReferenceType() && !paramType->IsRvalueRefType() && argument->GetType()->IsNonClassReferenceType())
                {
                    argument->SetFlag(Cm::BoundTree::BoundNodeFlags::refByValue);
                }
            }
        }
    }
}

Cm::BoundTree::BoundConversion* CreateBoundConversion(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Ast::Node* node,
    Cm::BoundTree::BoundExpression* operand, Cm::Sym::FunctionSymbol* conversionFun, Cm::BoundTree::BoundFunction* currentFunction)
{
    Cm::BoundTree::BoundConversion* conversion = new Cm::BoundTree::BoundConversion(node, operand, conversionFun);
    Cm::Sym::TypeSymbol* paramType = conversionFun->GetSourceType();
    Cm::BoundTree::BoundExpression* argument = operand;
    if (!conversionFun->IsBasicTypeOp())
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
        else if (paramType->IsClassTypeSymbol())
        {
            argument->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        }
        else if (!paramType->IsReferenceType() && !paramType->IsRvalueRefType() && argument->GetType()->IsNonClassReferenceType())
        {
            argument->SetFlag(Cm::BoundTree::BoundNodeFlags::refByValue);
        }
    }
    else
    {
        if (paramType->IsClassTypeSymbol())
        {
            argument->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        }
        else
        {
            if (!paramType->IsReferenceType() && !paramType->IsRvalueRefType() && argument->GetType()->IsNonClassReferenceType())
            {
                argument->SetFlag(Cm::BoundTree::BoundNodeFlags::refByValue);
            }
        }
    }
    if (conversionFun->GetTargetType()->IsClassTypeSymbol())
    {
        Cm::Sym::LocalVariableSymbol* temporary = currentFunction->CreateTempLocalVariable(conversionFun->GetTargetType());
        temporary->SetSid(boundCompileUnit.SymbolTable().GetSid());
        Cm::BoundTree::BoundExpression* boundTemporary = new Cm::BoundTree::BoundLocalVariable(node, temporary);
        boundTemporary->SetType(conversionFun->GetTargetType());
        boundTemporary->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        conversion->SetBoundTemporary(boundTemporary);
        conversion->SetType(boundTemporary->GetType());
    }
    else if (conversionFun->GetTargetType()->IsConstReferenceType())
    {
        argument = conversion->ReleaseOperand();
        std::vector<Cm::Core::Argument> resolutionArguments;
        Cm::Sym::TypeSymbol* pointerType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(argument->GetType(), argument->SyntaxNode()->GetSpan());
        resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, pointerType));
        resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, argument->GetType()));
        Cm::Sym::FunctionLookupSet resolutionLookups;
        resolutionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, containerScope));
        resolutionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::fileScopes, nullptr));
        std::vector<Cm::Sym::FunctionSymbol*> conversions;
        Cm::Sym::FunctionSymbol* copyCtor = ResolveOverload(containerScope, boundCompileUnit, "@constructor", resolutionArguments, resolutionLookups,
            argument->SyntaxNode()->GetSpan(), conversions);
        Cm::Sym::LocalVariableSymbol* temporary = currentFunction->CreateTempLocalVariable(argument->GetType());
        temporary->SetSid(boundCompileUnit.SymbolTable().GetSid());
        Cm::BoundTree::BoundExpression* boundTemporary = new Cm::BoundTree::BoundLocalVariable(argument->SyntaxNode(), temporary);
        boundTemporary->SetType(argument->GetType());
        Cm::BoundTree::BoundConversion* constRefConversion = new Cm::BoundTree::BoundConversion(argument->SyntaxNode(), argument, copyCtor);
        constRefConversion->SetType(pointerType);
        boundTemporary->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        constRefConversion->SetBoundTemporary(boundTemporary);
        constRefConversion->SetType(pointerType);
        conversion->ResetOperand(constRefConversion);
        constRefConversion->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        conversion->SetType(conversionFun->GetTargetType());
    }
    else
    {
        conversion->SetType(conversionFun->GetTargetType());
    }
    return conversion;
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

ExpressionBinder::ExpressionBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_) :
    Cm::Ast::Visitor(true, true), boundCompileUnit(boundCompileUnit_), containerScope(containerScope_),
    fileScopes(fileScopes_), currentFunction(currentFunction_), expressionCount(0), lookupId(Cm::Sym::SymbolTypeSetId::lookupAllSymbols), unaryMinus(false)
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
    Cm::Sym::TypeSymbol* plainOperandType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePlainType(operand->GetType());
    memFunLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base, plainOperandType->GetBaseType()->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Core::Argument> memFunArguments;
    memFunArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(plainOperandType, node->GetSpan())));
	bool firstArgByRef = false;
    Cm::Sym::FunctionSymbol* fun = ResolveOverload(containerScope, boundCompileUnit, opGroupName, memFunArguments, memFunLookups, node->GetSpan(), conversions, 
        OverloadResolutionFlags::nothrow | OverloadResolutionFlags::bindOnlyMemberFunctions);
	if (fun)
	{
		firstArgByRef = true;
	}
    if (!fun)
    {
        Cm::Sym::FunctionLookupSet freeFunLookups;
        freeFunLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, containerScope));
        freeFunLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, plainOperandType->GetContainerScope()->ClassOrNsScope()));
        freeFunLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::fileScopes, nullptr));
        std::vector<Cm::Core::Argument> freeFunArguments;
        freeFunArguments.push_back(Cm::Core::Argument(operand->GetArgumentCategory(), operand->GetType()));
        fun = ResolveOverload(containerScope, boundCompileUnit, opGroupName, freeFunArguments, freeFunLookups, node->GetSpan(), conversions);
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
        unaryOperand = CreateBoundConversion(containerScope, boundCompileUnit, node, operand, conversionFun, currentFunction);
		firstArgByRef = false;
    }
	Cm::BoundTree::BoundExpressionList arguments;
	arguments.Add(unaryOperand);
	PrepareArguments(containerScope, boundCompileUnit, currentFunction, fun->GetReturnType(), fun->Parameters(), arguments, firstArgByRef, boundCompileUnit.IrClassTypeRepository(), fun->IsBasicTypeOp());
	unaryOperand = arguments[0].release();
	Cm::BoundTree::BoundUnaryOp* op = new Cm::BoundTree::BoundUnaryOp(node, unaryOperand);
    op->SetFunction(fun);
    op->SetType(fun->GetReturnType());
    if (fun->ReturnsClassObjectByValue() && !fun->IsBasicTypeOp())
    {
        Cm::Sym::LocalVariableSymbol* classObjectResultVar = currentFunction->CreateTempLocalVariable(fun->GetReturnType());
        classObjectResultVar->SetSid(boundCompileUnit.SymbolTable().GetSid());
        op->SetClassObjectResultVar(classObjectResultVar);
    }
    if (!fun->IsBasicTypeOp())
    {
        op->SetTraceCallInfo(CreateTraceCallInfo(boundCompileUnit, currentFunction->GetFunctionSymbol(), node->GetSpan()));
    }
    boundExpressionStack.Push(op);
}

void ExpressionBinder::BindBinaryOp(Cm::Ast::Node* node, const std::string& opGroupName)
{ 
    Cm::BoundTree::BoundExpression* right = boundExpressionStack.Pop();
    Cm::BoundTree::BoundExpression* left = boundExpressionStack.Pop();
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionLookupSet memFunLookups;
    Cm::Sym::TypeSymbol* plainLeftType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePlainType(left->GetType());
    memFunLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base, plainLeftType->GetBaseType()->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Core::Argument> memFunArguments;
    memFunArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(plainLeftType, node->GetSpan())));
    memFunArguments.push_back(Cm::Core::Argument(right->GetArgumentCategory(), right->GetType()));
	bool firstArgByRef = false;
    Cm::Sym::FunctionSymbol* fun = ResolveOverload(containerScope, boundCompileUnit, opGroupName, memFunArguments, memFunLookups, node->GetSpan(), conversions,
        OverloadResolutionFlags::nothrow | OverloadResolutionFlags::bindOnlyMemberFunctions);
	if (fun)
	{
		firstArgByRef = true;
	}
    if (!fun)
    {
        std::vector<Cm::Core::Argument> freeFunArguments;
        freeFunArguments.push_back(Cm::Core::Argument(left->GetArgumentCategory(), left->GetType()));
        freeFunArguments.push_back(Cm::Core::Argument(right->GetArgumentCategory(), right->GetType()));
        Cm::Sym::FunctionLookupSet freeFunLookups;
        freeFunLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, containerScope));
		Cm::Sym::TypeSymbol* plainRightType = BoundCompileUnit().SymbolTable().GetTypeRepository().MakePlainType(right->GetType());
		freeFunLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, plainLeftType->GetContainerScope()->ClassOrNsScope()));
        freeFunLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, plainRightType->GetContainerScope()->ClassOrNsScope()));
        freeFunLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::fileScopes, nullptr));
        fun = ResolveOverload(containerScope, boundCompileUnit, opGroupName, freeFunArguments, freeFunLookups, node->GetSpan(), conversions);
    }
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
        leftOperand = CreateBoundConversion(containerScope, boundCompileUnit, node, left, leftConversionFun, currentFunction);
		firstArgByRef = false;
    }
    Cm::BoundTree::BoundExpression* rightOperand = right;
    if (rightConversionFun)
    {
        rightOperand = CreateBoundConversion(containerScope, boundCompileUnit, node, right, rightConversionFun, currentFunction);
    }
	Cm::BoundTree::BoundExpressionList arguments;
	arguments.Add(leftOperand);
	arguments.Add(rightOperand);
	PrepareArguments(containerScope, boundCompileUnit, currentFunction, fun->GetReturnType(), fun->Parameters(), arguments, firstArgByRef, boundCompileUnit.IrClassTypeRepository(), fun->IsBasicTypeOp());
	leftOperand = arguments[0].release();
	rightOperand = arguments[1].release();
    Cm::BoundTree::BoundBinaryOp* op = new Cm::BoundTree::BoundBinaryOp(node, leftOperand, rightOperand);
    op->SetFunction(fun);
    op->SetType(fun->GetReturnType());
    if (fun->ReturnsClassObjectByValue())
    {
        Cm::Sym::LocalVariableSymbol* classObjectResultVar = currentFunction->CreateTempLocalVariable(fun->GetReturnType());
        classObjectResultVar->SetSid(boundCompileUnit.SymbolTable().GetSid());
        op->SetClassObjectResultVar(classObjectResultVar);
    }
    if (!fun->IsBasicTypeOp())
    {
        op->SetTraceCallInfo(CreateTraceCallInfo(boundCompileUnit, currentFunction->GetFunctionSymbol(), node->GetSpan()));
    }
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
    resultVar->SetSid(boundCompileUnit.SymbolTable().GetSid());
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
    resultVar->SetSid(boundCompileUnit.SymbolTable().GetSid());
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

void ExpressionBinder::BeginVisit(Cm::Ast::UnaryMinusNode& unaryMinusNode)
{
    unaryMinusStack.push(unaryMinus);
    unaryMinus = true;
}

void ExpressionBinder::EndVisit(Cm::Ast::UnaryMinusNode& unaryMinusNode)
{
    unaryMinus = unaryMinusStack.top();
    unaryMinusStack.pop();
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
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupVariableAndParameter;
    addrOfNode.Subject()->Accept(*this);
    lookupId = lookupIdStack.Pop();
    Cm::BoundTree::BoundExpression* operand = boundExpressionStack.Pop();
    operand->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
    boundExpressionStack.Push(operand);
    BindUnaryOp(&addrOfNode, "operator&");
    Cm::BoundTree::BoundExpression* op = boundExpressionStack.Pop();
    op->SetFlag(Cm::BoundTree::BoundNodeFlags::addrArg);
    boundExpressionStack.Push(op);
}

void ExpressionBinder::Visit(Cm::Ast::DerefNode& derefNode)
{
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupVariableAndParameter;
    bool isDerefThis = derefNode.Subject()->IsThisNode();   // '*this' is special
    derefNode.Subject()->Accept(*this);
    lookupId = lookupIdStack.Pop();
    BindUnaryOp(&derefNode, "operator*");
    Cm::BoundTree::BoundExpression* derefExpr = boundExpressionStack.Pop();
    if (isDerefThis)
    {
        Cm::Sym::TypeSymbol* type = derefExpr->GetType();
        derefExpr->SetType(boundCompileUnit.SymbolTable().GetTypeRepository().MakeReferenceType(type, derefNode.GetSpan()));
    }
    if (!derefExpr->IsBoundUnaryOp())
    {
        throw std::runtime_error("not bound unary op");
    }
    Cm::BoundTree::BoundUnaryOp* unaryOp = static_cast<Cm::BoundTree::BoundUnaryOp*>(derefExpr);
    if (!unaryOp->GetFunction()->GetReturnType()->IsConstType())
    {
        unaryOp->SetArgumentCategory(Cm::Core::ArgumentCategory::lvalue);
    }
    boundExpressionStack.Push(derefExpr);
}

void ExpressionBinder::Visit(Cm::Ast::PostfixIncNode& postfixIncNode)
{
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupVariableAndParameter;
    postfixIncNode.Subject()->Accept(*this);
    lookupId = lookupIdStack.Pop();
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupVariableAndParameter;
    Cm::BoundTree::BoundExpression* value = boundExpressionStack.Pop();
    postfixIncNode.Subject()->Accept(*this);
    lookupId = lookupIdStack.Pop();
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
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupVariableAndParameter;
    postfixDecNode.Subject()->Accept(*this);
    lookupId = lookupIdStack.Pop();
    Cm::BoundTree::BoundExpression* value = boundExpressionStack.Pop();
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupVariableAndParameter;
    postfixDecNode.Subject()->Accept(*this);
    lookupId = lookupIdStack.Pop();
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
    Cm::Sym::TypeSymbol* type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
    Cm::Sym::Value* value = new Cm::Sym::BoolValue(booleanLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&booleanLiteralNode);
    literalNode->SetValue(value);
    literalNode->SetType(type);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::SByteLiteralNode& sbyteLiteralNode)
{
    Cm::Sym::TypeSymbol* type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::sbyteId));
    Cm::Sym::Value* value = new Cm::Sym::SByteValue(sbyteLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&sbyteLiteralNode);
    literalNode->SetValue(value);
    literalNode->SetType(type);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::ByteLiteralNode& byteLiteralNode)
{
    Cm::Sym::TypeSymbol* type = nullptr;
    Cm::Sym::Value* value = nullptr;
    if (unaryMinus)
    {
        if (byteLiteralNode.Value() == 128)
        {
            type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::sbyteId));
            value = new Cm::Sym::SByteValue(byteLiteralNode.Value());
        }
        else
        {
            type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::shortId));
            value = new Cm::Sym::ShortValue(byteLiteralNode.Value());
        }
    }
    else
    {
        type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::byteId));
        value = new Cm::Sym::ByteValue(byteLiteralNode.Value());
    }
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&byteLiteralNode);
    literalNode->SetValue(value);
    literalNode->SetType(type);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::ShortLiteralNode& shortLiteralNode)
{
    Cm::Sym::TypeSymbol* type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::shortId));
    Cm::Sym::Value* value = new Cm::Sym::ShortValue(shortLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&shortLiteralNode);
    literalNode->SetValue(value);
    literalNode->SetType(type);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::UShortLiteralNode& ushortLiteralNode)
{
    Cm::Sym::TypeSymbol* type = nullptr;
    Cm::Sym::Value* value = nullptr;
    if (unaryMinus)
    {
        if (ushortLiteralNode.Value() == 32768)
        {
            type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::shortId));
            value = new Cm::Sym::ShortValue(ushortLiteralNode.Value());
        }
        else
        {
            type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId));
            value = new Cm::Sym::IntValue(ushortLiteralNode.Value());
        }
    }
    else
    {
        type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ushortId));
        value = new Cm::Sym::UShortValue(ushortLiteralNode.Value());
    }
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&ushortLiteralNode);
    literalNode->SetValue(value);
    literalNode->SetType(type);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::IntLiteralNode& intLiteralNode)
{
    Cm::Sym::TypeSymbol* type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId));
    Cm::Sym::Value* value = new Cm::Sym::IntValue(intLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&intLiteralNode);
    literalNode->SetValue(value);
    literalNode->SetType(type);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::UIntLiteralNode& uintLiteralNode)
{
    Cm::Sym::TypeSymbol* type = nullptr;
    Cm::Sym::Value* value = nullptr;
    if (unaryMinus)
    {
        if (uintLiteralNode.Value() == 2147483648)
        {
            type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId));
            value = new Cm::Sym::IntValue(uintLiteralNode.Value());
        }
        else
        {
            type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::longId));
            value = new Cm::Sym::LongValue(uintLiteralNode.Value());
        }
    }
    else
    {
        type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::uintId));
        value = new Cm::Sym::UIntValue(uintLiteralNode.Value());
    }
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&uintLiteralNode);
    literalNode->SetValue(value);
    literalNode->SetType(type);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::LongLiteralNode& longLiteralNode)
{
    Cm::Sym::TypeSymbol* type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::longId));
    Cm::Sym::Value* value = new Cm::Sym::LongValue(longLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&longLiteralNode);
    literalNode->SetValue(value);
    literalNode->SetType(type);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::ULongLiteralNode& ulongLiteralNode)
{
    Cm::Sym::TypeSymbol* type = nullptr;
    Cm::Sym::Value* value = nullptr;
    if (unaryMinus)
    {
        if (ulongLiteralNode.Value() == 9223372036854775808)
        {
            type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::longId));
            value = new Cm::Sym::LongValue(ulongLiteralNode.Value());
        }
        else
        {
            type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ulongId));
            value = new Cm::Sym::ULongValue(ulongLiteralNode.Value());
        }
    }
    else
    {
        type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ulongId));
        value = new Cm::Sym::ULongValue(ulongLiteralNode.Value());
    }
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&ulongLiteralNode);
    literalNode->SetValue(value);
    literalNode->SetType(type);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::FloatLiteralNode& floatLiteralNode)
{
    Cm::Sym::TypeSymbol* type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::floatId));
    Cm::Sym::Value* value = new Cm::Sym::FloatValue(floatLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&floatLiteralNode);
    literalNode->SetValue(value);
    literalNode->SetType(type);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::DoubleLiteralNode& doubleLiteralNode)
{
    Cm::Sym::TypeSymbol* type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::doubleId));
    Cm::Sym::Value* value = new Cm::Sym::DoubleValue(doubleLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&doubleLiteralNode);
    literalNode->SetValue(value);
    literalNode->SetType(type);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::CharLiteralNode& charLiteralNode)
{
    Cm::Sym::TypeSymbol* type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::charId));
    Cm::Sym::Value* value = new Cm::Sym::CharValue(charLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&charLiteralNode);
    literalNode->SetValue(value);
    literalNode->SetType(type);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::StringLiteralNode& stringLiteralNode)
{
    Cm::Sym::TypeSymbol* type = boundCompileUnit.SymbolTable().GetTypeRepository().MakeConstCharPtrType(stringLiteralNode.GetSpan());
    int id = boundCompileUnit.StringRepository().Install(stringLiteralNode.Value());
    Cm::BoundTree::BoundStringLiteral* literalNode = new Cm::BoundTree::BoundStringLiteral(&stringLiteralNode, id);
    literalNode->SetType(type);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::NullLiteralNode& nullLiteralNode)
{
    Cm::Sym::TypeSymbol* type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::nullPtrId));
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&nullLiteralNode);
    Cm::Sym::Value* value = new Cm::Sym::NullValue();
    literalNode->SetValue(value);
    literalNode->SetType(type);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::BoolNode& boolNode)
{
    Cm::Sym::TypeSymbol* boolTypeSymbol = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
    Cm::BoundTree::BoundTypeExpression* typeExpression = new Cm::BoundTree::BoundTypeExpression(&boolNode, boolTypeSymbol);
    typeExpression->SetType(boolTypeSymbol);
    boundExpressionStack.Push(typeExpression);
}

void ExpressionBinder::Visit(Cm::Ast::SByteNode& sbyteNode)
{
    Cm::Sym::TypeSymbol* sbyteTypeSymbol = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::sbyteId));
    Cm::BoundTree::BoundTypeExpression* typeExpression = new Cm::BoundTree::BoundTypeExpression(&sbyteNode, sbyteTypeSymbol);
    typeExpression->SetType(sbyteTypeSymbol);
    boundExpressionStack.Push(typeExpression);
}

void ExpressionBinder::Visit(Cm::Ast::ByteNode& byteNode)
{
    Cm::Sym::TypeSymbol* byteTypeSymbol = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::byteId));
    Cm::BoundTree::BoundTypeExpression* typeExpression = new Cm::BoundTree::BoundTypeExpression(&byteNode, byteTypeSymbol);
    typeExpression->SetType(byteTypeSymbol);
    boundExpressionStack.Push(typeExpression);
}

void ExpressionBinder::Visit(Cm::Ast::ShortNode& shortNode)
{
    Cm::Sym::TypeSymbol* shortTypeSymbol = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::shortId));
    Cm::BoundTree::BoundTypeExpression* typeExpression = new Cm::BoundTree::BoundTypeExpression(&shortNode, shortTypeSymbol);
    typeExpression->SetType(shortTypeSymbol);
    boundExpressionStack.Push(typeExpression);
}

void ExpressionBinder::Visit(Cm::Ast::UShortNode& ushortNode)
{
    Cm::Sym::TypeSymbol* ushortTypeSymbol = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ushortId));
    Cm::BoundTree::BoundTypeExpression* typeExpression = new Cm::BoundTree::BoundTypeExpression(&ushortNode, ushortTypeSymbol);
    typeExpression->SetType(ushortTypeSymbol);
    boundExpressionStack.Push(typeExpression);
}

void ExpressionBinder::Visit(Cm::Ast::IntNode& inttNode)
{
    Cm::Sym::TypeSymbol* intTypeSymbol = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId));
    Cm::BoundTree::BoundTypeExpression* typeExpression = new Cm::BoundTree::BoundTypeExpression(&inttNode, intTypeSymbol);
    typeExpression->SetType(intTypeSymbol);
    boundExpressionStack.Push(typeExpression);
}

void ExpressionBinder::Visit(Cm::Ast::UIntNode& uinttNode)
{
    Cm::Sym::TypeSymbol* uintTypeSymbol = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::uintId));
    Cm::BoundTree::BoundTypeExpression* typeExpression = new Cm::BoundTree::BoundTypeExpression(&uinttNode, uintTypeSymbol);
    typeExpression->SetType(uintTypeSymbol);
    boundExpressionStack.Push(typeExpression);
}

void ExpressionBinder::Visit(Cm::Ast::LongNode& longNode)
{
    Cm::Sym::TypeSymbol* longTypeSymbol = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::longId));
    Cm::BoundTree::BoundTypeExpression* typeExpression = new Cm::BoundTree::BoundTypeExpression(&longNode, longTypeSymbol);
    typeExpression->SetType(longTypeSymbol);
    boundExpressionStack.Push(typeExpression);
}

void ExpressionBinder::Visit(Cm::Ast::ULongNode& ulongNode)
{
    Cm::Sym::TypeSymbol* ulongTypeSymbol = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ulongId));
    Cm::BoundTree::BoundTypeExpression* typeExpression = new Cm::BoundTree::BoundTypeExpression(&ulongNode, ulongTypeSymbol);
    typeExpression->SetType(ulongTypeSymbol);
    boundExpressionStack.Push(typeExpression);
}

void ExpressionBinder::Visit(Cm::Ast::FloatNode& floatNode)
{
    Cm::Sym::TypeSymbol* floatTypeSymbol = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::floatId));
    Cm::BoundTree::BoundTypeExpression* typeExpression = new Cm::BoundTree::BoundTypeExpression(&floatNode, floatTypeSymbol);
    typeExpression->SetType(floatTypeSymbol);
    boundExpressionStack.Push(typeExpression);
}

void ExpressionBinder::Visit(Cm::Ast::DoubleNode& doubleNode)
{
    Cm::Sym::TypeSymbol* doubleTypeSymbol = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::doubleId));
    Cm::BoundTree::BoundTypeExpression* typeExpression = new Cm::BoundTree::BoundTypeExpression(&doubleNode, doubleTypeSymbol);
    typeExpression->SetType(doubleTypeSymbol);
    boundExpressionStack.Push(typeExpression);
}

void ExpressionBinder::Visit(Cm::Ast::CharNode& charNode)
{
    Cm::Sym::TypeSymbol* charTypeSymbol = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::charId));
    Cm::BoundTree::BoundTypeExpression* typeExpression = new Cm::BoundTree::BoundTypeExpression(&charNode, charTypeSymbol);
    typeExpression->SetType(charTypeSymbol);
    boundExpressionStack.Push(typeExpression);
}

void ExpressionBinder::Visit(Cm::Ast::VoidNode& voidNode)
{
    Cm::Sym::TypeSymbol* voidTypeSymbol = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId));
    Cm::BoundTree::BoundTypeExpression* typeExpression = new Cm::BoundTree::BoundTypeExpression(&voidNode, voidTypeSymbol);
    typeExpression->SetType(voidTypeSymbol);
    boundExpressionStack.Push(typeExpression);
}

void ExpressionBinder::Visit(Cm::Ast::DerivedTypeExprNode& derivedTypeExprNode)
{
    Cm::Sym::TypeSymbol* baseType = ResolveType(boundCompileUnit.SymbolTable(), containerScope, fileScopes, boundCompileUnit.ClassTemplateRepository(), derivedTypeExprNode.BaseTypeExprNode());
    std::vector<int> arrayDimensions;
    int n = derivedTypeExprNode.NumArrayDimensions();
    if (n > 0)
    {
        for (int i = 0; i < n; ++i)
        {
            Cm::Sym::Value* value = Evaluate(Cm::Sym::ValueType::intValue, false, derivedTypeExprNode.ArrayDimensionNode(i), boundCompileUnit.SymbolTable(), containerScope, boundCompileUnit.GetFileScopes(), boundCompileUnit.ClassTemplateRepository());
            Cm::Sym::IntValue* intValue = static_cast<Cm::Sym::IntValue*>(value);
            int arrayDimension = intValue->Value();
            if (arrayDimension <= 0)
            {
                throw Cm::Core::Exception("array dimension must be positive", derivedTypeExprNode.GetSpan());
            }
            arrayDimensions.push_back(arrayDimension);
        }
    }
    Cm::Sym::TypeSymbol* derivedTypeSymbol = boundCompileUnit.SymbolTable().GetTypeRepository().MakeDerivedType(derivedTypeExprNode.Derivations(), baseType, arrayDimensions, derivedTypeExprNode.GetSpan());
    Cm::BoundTree::BoundTypeExpression* typeExpression = new Cm::BoundTree::BoundTypeExpression(&derivedTypeExprNode, derivedTypeSymbol);
    typeExpression->SetType(derivedTypeSymbol);
    boundExpressionStack.Push(typeExpression);
}

void ExpressionBinder::BeginVisit(Cm::Ast::DotNode& dotNode)
{
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupDotSubjectSymbols;
}

void ExpressionBinder::EndVisit(Cm::Ast::DotNode& dotNode)
{
    lookupId = lookupIdStack.Pop();
    std::unique_ptr<Cm::BoundTree::BoundExpression> expression(boundExpressionStack.Pop());
    if (expression->IsBoundNamespaceExpression() || expression->IsBoundTypeExpression())
    {
        Cm::Sym::ContainerSymbol* containerSymbol = nullptr;
        if (expression->IsBoundNamespaceExpression())
        {
            Cm::BoundTree::BoundNamespaceExpression* namespaceExpression = static_cast<Cm::BoundTree::BoundNamespaceExpression*>(expression.get());
            containerSymbol = namespaceExpression->NamespaceSymbol();
        }
        else
        {
            Cm::BoundTree::BoundTypeExpression* typeExpression = static_cast<Cm::BoundTree::BoundTypeExpression*>(expression.get());
            Cm::Sym::TypeSymbol* typeSymbol = typeExpression->Symbol();
            if (typeSymbol->IsClassTypeSymbol() || typeSymbol->IsEnumTypeSymbol())
            {
                containerSymbol = typeSymbol;
            }
            else
            {
                throw Cm::Core::Exception("expression '" + expression->SyntaxNode()->ToString() + "' must denote a namespace, class type or enumerated type object", dotNode.Subject()->GetSpan());
            }
        }
        Cm::Sym::ContainerScope* containerScope = containerSymbol->GetContainerScope();
        Cm::Sym::Symbol* symbol = containerScope->Lookup(dotNode.MemberId()->Str(), Cm::Sym::ScopeLookup::this_and_base, lookupId);
        if (symbol)
        {
            BindSymbol(&dotNode, symbol);
            if (symbol->IsFunctionGroupSymbol() && (containerSymbol->IsNamespaceSymbol() || containerSymbol->IsClassTypeSymbol()))
            {
                std::unique_ptr<Cm::BoundTree::BoundExpression> expression(boundExpressionStack.Pop());
                if (expression->IsBoundFunctionGroup())
                {
                    Cm::BoundTree::BoundFunctionGroup* boundFunctionGroup = static_cast<Cm::BoundTree::BoundFunctionGroup*>(expression.get());
                    boundFunctionGroup->SetFlag(Cm::BoundTree::BoundNodeFlags::scopeQualified);
                    boundFunctionGroup->SetQualifiedScope(containerSymbol->GetContainerScope());
                    boundExpressionStack.Push(expression.release());
                }
                else
                {
                    throw std::runtime_error("bound function group expected");
                }
            }
        }
        else
        {
            throw Cm::Core::Exception("symbol '" + containerSymbol->FullName() + "' does not have member '" + dotNode.MemberId()->Str() + "'", dotNode.GetSpan());
        }
    }
    else
    {
        Cm::Sym::TypeSymbol* type = boundCompileUnit.SymbolTable().GetTypeRepository().MakePlainType(expression->GetType());
        if (type->IsTemplateTypeSymbol())
        {
            Cm::Sym::TemplateTypeSymbol* templateTypeSymbol = static_cast<Cm::Sym::TemplateTypeSymbol*>(type);
            if (!templateTypeSymbol->Bound())
            {
                boundCompileUnit.ClassTemplateRepository().BindTemplateTypeSymbol(templateTypeSymbol, containerScope, fileScopes);
            }
        }
        if (type->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* classType = static_cast<Cm::Sym::ClassTypeSymbol*>(type);
            Cm::Sym::ContainerScope* containerScope = classType->GetContainerScope();
            Cm::Sym::Symbol* symbol = containerScope->Lookup(dotNode.MemberId()->Str(), Cm::Sym::ScopeLookup::this_and_base, Cm::Sym::SymbolTypeSetId::lookupFunctionGroupAndMemberVariable);
            if (symbol)
            {
                Cm::BoundTree::BoundExpression* classObject = expression.release();
                BindSymbol(&dotNode, symbol);
                std::unique_ptr<Cm::BoundTree::BoundExpression> symbolExpr(boundExpressionStack.Pop());
                if (symbolExpr->IsBoundFunctionGroup())
                {
                    boundExpressionStack.Push(symbolExpr.release());
                    if (!classObject->GetFlag(Cm::BoundTree::BoundNodeFlags::argIsThisOrBase))
                    {
                        Cm::Sym::Symbol* parent = symbol->Parent();
                        if (!parent->IsClassTypeSymbol())
                        {
                            throw std::runtime_error("member variable parent not class type");
                        }
                        Cm::Sym::ClassTypeSymbol* functionGroupOwnerClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(parent);
                        int distance = 0;
                        if (classType->HasBaseClass(functionGroupOwnerClassType, distance))
                        {
                            Cm::Sym::TypeSymbol* functionGroupOwnerClassTypePtr = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(functionGroupOwnerClassType, dotNode.GetSpan());
                            Cm::Sym::TypeSymbol* classTypePtr = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(classType, dotNode.GetSpan());
                            Cm::Sym::FunctionSymbol* derivedBaseConversion = boundCompileUnit.ClassConversionTable().MakeBaseClassDerivedClassConversion(functionGroupOwnerClassTypePtr, classTypePtr, distance,
                                dotNode.GetSpan());
                            classObject->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
                            classObject = CreateBoundConversion(containerScope, boundCompileUnit, &dotNode, classObject, derivedBaseConversion, currentFunction);
                        }
                    }
                    classObject->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
                    classObject->SetFlag(Cm::BoundTree::BoundNodeFlags::classObjectArg);
                    boundExpressionStack.Push(classObject);
                }
                else if (symbolExpr->IsBoundMemberVariable())
                {
                    Cm::BoundTree::BoundMemberVariable* memberVariable = static_cast<Cm::BoundTree::BoundMemberVariable*>(symbolExpr.release());
                    Cm::Sym::Symbol* parent = symbol->Parent();
                    if (!parent->IsClassTypeSymbol())
                    {
                        throw std::runtime_error("member variable parent not class type");
                    }
                    Cm::Sym::ClassTypeSymbol* memVarOwnerClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(parent);
                    int distance = 0;
                    if (classType->HasBaseClass(memVarOwnerClassType, distance))
                    {
                        Cm::Sym::TypeSymbol* memVarOwnerClassTypePtr = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(memVarOwnerClassType, dotNode.GetSpan());
                        Cm::Sym::TypeSymbol* classTypePtr = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(classType, dotNode.GetSpan());
                        Cm::Sym::FunctionSymbol* derivedBaseConversion = boundCompileUnit.ClassConversionTable().MakeBaseClassDerivedClassConversion(memVarOwnerClassTypePtr, classTypePtr, 
                            distance, dotNode.GetSpan());
                        classObject->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
                        classObject = CreateBoundConversion(containerScope, boundCompileUnit, &dotNode, classObject, derivedBaseConversion, currentFunction);
                    }
                    classObject->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
                    memberVariable->SetClassObject(classObject);
                    boundExpressionStack.Push(memberVariable);
                }
                else
                { 
                    throw Cm::Core::Exception("symbol '" + symbolExpr->SyntaxNode()->ToString() + "' does not denote a member variable or a function group", dotNode.GetSpan(), symbolExpr->SyntaxNode()->GetSpan());
                }
            }
            else
            {
                throw Cm::Core::Exception("class '" + classType->FullName() + "' does not have member '" + dotNode.MemberId()->Str() + "'", dotNode.GetSpan());
            }
        }
        else
        {
            throw Cm::Core::Exception("expression '" + expression->SyntaxNode()->ToString() + "' must denote a namespace, class type, or enumerated type object", dotNode.Subject()->GetSpan());
        }
    }
}

void ExpressionBinder::BindArrow(Cm::Ast::Node* node, const std::string& memberId)
{
    BindUnaryOp(node, "operator->");
    std::unique_ptr<Cm::BoundTree::BoundExpression> boundUnaryOpExpr(boundExpressionStack.Pop());
    Cm::Sym::TypeSymbol* type = boundCompileUnit.SymbolTable().GetTypeRepository().MakePlainType(boundUnaryOpExpr->GetType());
    if (type->IsPointerToClassType() || type->IsClassTypeSymbol())
    {
        if (type->IsPointerType())
        {
            type = type->GetBaseType();
        }
        type = boundCompileUnit.SymbolTable().GetTypeRepository().MakePlainType(type);
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
                    Cm::Sym::Symbol* symbol = containerScope->Lookup(memberId, Cm::Sym::ScopeLookup::this_and_base, Cm::Sym::SymbolTypeSetId::lookupFunctionGroupAndMemberVariable);
                    if (symbol)
                    {
                        BindSymbol(node, symbol);
                        std::unique_ptr<Cm::BoundTree::BoundExpression> symbolExpr(boundExpressionStack.Pop());
                        if (symbolExpr->IsBoundFunctionGroup())
                        {
                            boundExpressionStack.Push(symbolExpr.release());
                            if (!classObject->GetFlag(Cm::BoundTree::BoundNodeFlags::argIsThisOrBase))
                            {
                                Cm::Sym::Symbol* parent = symbol->Parent();
                                if (!parent->IsClassTypeSymbol())
                                {
                                    throw std::runtime_error("member variable parent not class type");
                                }
                                Cm::Sym::ClassTypeSymbol* functionGroupOwnerClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(parent);
                                int distance = 0;
                                if (classTypeSymbol->HasBaseClass(functionGroupOwnerClassType, distance))
                                {
                                    Cm::Sym::TypeSymbol* functionGroupOwnerClassTypePtr = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(functionGroupOwnerClassType, node->GetSpan());
                                    Cm::Sym::TypeSymbol* classTypePtr = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(classTypeSymbol, node->GetSpan());
                                    Cm::Sym::FunctionSymbol* derivedBaseConversion = boundCompileUnit.ClassConversionTable().MakeBaseClassDerivedClassConversion(functionGroupOwnerClassTypePtr, classTypePtr, distance,
                                        node->GetSpan());
                                    classObject = CreateBoundConversion(containerScope, boundCompileUnit, node, classObject, derivedBaseConversion, currentFunction);
                                }
                            }
                            classObject->SetFlag(Cm::BoundTree::BoundNodeFlags::classObjectArg);
                            boundExpressionStack.Push(classObject);
                        }
                        else if (symbolExpr->IsBoundMemberVariable())
                        {
                            Cm::BoundTree::BoundMemberVariable* memberVariable = static_cast<Cm::BoundTree::BoundMemberVariable*>(symbolExpr.release());
                            Cm::Sym::Symbol* parent = symbol->Parent();
                            if (!parent->IsClassTypeSymbol())
                            {
                                throw std::runtime_error("member variable parent not class type");
                            }
                            Cm::Sym::ClassTypeSymbol* memVarOwnerClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(parent);
                            int distance = 0;
                            if (classTypeSymbol->HasBaseClass(memVarOwnerClassType, distance))
                            {
                                Cm::Sym::TypeSymbol* memVarOwnerClassTypePtr = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(memVarOwnerClassType, node->GetSpan());
                                Cm::Sym::TypeSymbol* classTypePtr = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(classTypeSymbol, node->GetSpan());
                                Cm::Sym::FunctionSymbol* derivedBaseConversion = boundCompileUnit.ClassConversionTable().MakeBaseClassDerivedClassConversion(memVarOwnerClassTypePtr, 
                                    classTypePtr, distance, node->GetSpan());
                                classObject->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
                                classObject = CreateBoundConversion(containerScope, boundCompileUnit, node, classObject, derivedBaseConversion, currentFunction);
                            }
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
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupVariableAndParameter;
    arrowNode.Subject()->Accept(*this);
    lookupId = lookupIdStack.Pop();
    BindArrow(&arrowNode, arrowNode.MemberId()->Str());
}

void ExpressionBinder::BeginVisit(Cm::Ast::InvokeNode& invokeNode)
{
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupInvokeSubject;
    invokeNode.Subject()->Accept(*this);
    lookupId = lookupIdStack.Pop();
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
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupVariableAndParameter;
    indexNode.Subject()->Accept(*this);
    lookupId = lookupIdStack.Pop();
    std::unique_ptr<Cm::BoundTree::BoundExpression> subject(boundExpressionStack.Pop());
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupArgumentSymbol;
    indexNode.Index()->Accept(*this);
    lookupId = lookupIdStack.Pop();
    std::unique_ptr<Cm::BoundTree::BoundExpression> index(boundExpressionStack.Pop());
    Cm::Sym::TypeSymbol* subjectType = subject->GetType();
    Cm::Sym::TypeSymbol* plainSubjectType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePlainType(subjectType);
    if (plainSubjectType->IsArrayType())
    {
        BindIndexArray(&indexNode, subject.release(), index.release());
    }
    else if (plainSubjectType->IsPointerType())
    {
        BindIndexPointer(&indexNode, subject.release(), index.release());
    }
    else if (plainSubjectType->IsClassTypeSymbol())
    {
        BindIndexClass(&indexNode, subject.release(), index.release());
    }
    else
    {
        throw Cm::Core::Exception("subscript operator can be applied only to pointer, array or class type subject", indexNode.GetSpan());
    }
}

void ExpressionBinder::BindIndexPointer(Cm::Ast::Node* indexNode, Cm::BoundTree::BoundExpression* subject, Cm::BoundTree::BoundExpression* index)
{
    boundExpressionStack.Push(subject);
    boundExpressionStack.Push(index);
    BindBinaryOp(indexNode, "operator+");
    BindUnaryOp(indexNode, "operator*");
}

void ExpressionBinder::BindIndexArray(Cm::Ast::Node* indexNode, Cm::BoundTree::BoundExpression* subject, Cm::BoundTree::BoundExpression* index)
{
    boundExpressionStack.Push(subject);
    boundExpressionStack.Push(index);
    BindBinaryOp(indexNode, "operator[]");
    Cm::BoundTree::BoundExpression* indexArray = Pop();
    indexArray->SetFlag(Cm::BoundTree::BoundNodeFlags::indexArray);
    boundExpressionStack.Push(indexArray);
}

void ExpressionBinder::BindIndexClass(Cm::Ast::Node* indexNode, Cm::BoundTree::BoundExpression* subject, Cm::BoundTree::BoundExpression* index)
{
    Cm::Sym::FunctionGroupSymbol subscriptFunctionGroup(indexNode->GetSpan(), "operator[]", boundCompileUnit.SymbolTable().GetContainerScope(indexNode));
    Cm::BoundTree::BoundFunctionGroup* boundSubscriptFunctionGroup = new Cm::BoundTree::BoundFunctionGroup(indexNode, &subscriptFunctionGroup);
    boundExpressionStack.Push(boundSubscriptFunctionGroup);
    subject->SetFlag(Cm::BoundTree::BoundNodeFlags::classObjectArg);
    subject->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
    boundExpressionStack.Push(subject);
    boundExpressionStack.Push(index);
    expressionCountStack.push(expressionCount);
    expressionCountStack.push(0);
    BindInvoke(indexNode, 2);
    expressionCount = expressionCountStack.top();
    expressionCountStack.pop();
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
    Cm::Sym::FunctionSymbol* fun = nullptr;
    Cm::Sym::TypeSymbol* type = nullptr;
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    bool firstArgByRef = false;
    bool constructTemporary = false;
    bool returnClassObjectByValue = false;
    Cm::Sym::LocalVariableSymbol* temporary = nullptr;
    if (subject->IsBoundFunctionGroup())
    {
        bool generateVirtualCall1 = false;
        bool generateVirtualCall2 = false;
        Cm::Sym::FunctionSymbol* fun1 = nullptr;
        Cm::Sym::FunctionSymbol* fun2 = nullptr;
        Cm::Sym::TypeSymbol* type1 = nullptr;
        Cm::Sym::TypeSymbol* type2 = nullptr;
        std::vector<Cm::Sym::FunctionSymbol*> conversions1;
        std::vector<Cm::Sym::FunctionSymbol*> conversions2;
        bool firstArgByRef1 = false;
        bool firstArgByRef2 = false;
        bool returnClassObjectByValue1 = false;
        bool returnClassObjectByValue2 = false;
        std::unique_ptr<Cm::Core::Exception> exception1;
        std::unique_ptr<Cm::Core::Exception> exception2;
        FunctionMatch bestMatch1;
        FunctionMatch bestMatch2;
        std::vector<Cm::Core::Argument> resolutionArguments1;
        std::vector<Cm::Core::Argument> resolutionArguments2;
        Cm::BoundTree::BoundFunctionGroup* functionGroup = static_cast<Cm::BoundTree::BoundFunctionGroup*>(subject.get());
        functionGroupSymbol = functionGroup->GetFunctionGroupSymbol();
        functionGroupName = functionGroupSymbol->Name();
        std::unique_ptr<Cm::BoundTree::BoundExpression> firstArg;
        Cm::Sym::ContainerScope* qualifiedScope = nullptr;
        bool scopeQualified = subject->GetFlag(Cm::BoundTree::BoundNodeFlags::scopeQualified);
        if (scopeQualified)
        {
            Cm::BoundTree::BoundFunctionGroup* boundFunctionGroup = static_cast<Cm::BoundTree::BoundFunctionGroup*>(subject.get());
            qualifiedScope = boundFunctionGroup->QualifiedScope();
        }
        if (!scopeQualified && currentFunction->GetFunctionSymbol()->IsMemberFunctionSymbol() && !currentFunction->GetFunctionSymbol()->IsStatic())
        {
            fun1 = BindInvokeMemFun(node, conversions1, arguments, firstArgByRef1, generateVirtualCall1, functionGroupName, numArgs, bestMatch1, resolutionArguments1, exception1);
            if (fun1)
            {
                type1 = fun1->GetReturnType();
                if (type1->IsClassTypeSymbol())
                {
                    returnClassObjectByValue1 = true;
                }
                firstArg.reset(arguments.RemoveFirst());
            }
        }
        fun2 = BindInvokeFun(node, conversions2, arguments, firstArgByRef2, generateVirtualCall2, functionGroupSymbol, functionGroup->BoundTemplateArguments(), bestMatch2, resolutionArguments2, exception2, qualifiedScope);
        if (fun2)
        {
            type2 = fun2->GetReturnType();
            if (type2->IsClassTypeSymbol())
            {
                returnClassObjectByValue2 = true;
            }
        }
        if (fun1 && fun2)
        {
            if (fun1 == fun2)
            {
                fun2 = nullptr;
            }
            else
            {
                BetterFunctionMatch betterMatch;
                if (betterMatch(bestMatch1, bestMatch2))
                {
                    fun2 = nullptr;
                }
                else if (betterMatch(bestMatch2, bestMatch1))
                {
                    fun1 = nullptr;
                    --numArgs;
                }
            }
        }
        if (fun1 && !fun2)
        {
            fun = fun1;
            arguments.InsertFront(firstArg.release());
            conversions = conversions1;
            firstArgByRef = firstArgByRef1;
            generateVirtualCall = generateVirtualCall1;
            type = type1;
            returnClassObjectByValue = returnClassObjectByValue1;
        }
        else if (fun2 && !fun1)
        {
            fun = fun2;
            conversions = conversions2;
            firstArgByRef = firstArgByRef2;
            generateVirtualCall = generateVirtualCall2;
            type = type2;
            returnClassObjectByValue = returnClassObjectByValue2;
        }
        else if (fun1 && fun2)
        {
            if (exception2)
            {
                Cm::Core::Exception copyOfEx2 = *exception2;
                throw copyOfEx2;
            }
            else if (exception1)
            {
                Cm::Core::Exception copyOfEx1 = *exception1;
                throw copyOfEx1;
            }
            else
            {
                std::string overloadName = MakeOverloadName(functionGroupName, resolutionArguments2);
                std::string matchedFunctionNames;
                matchedFunctionNames.append(bestMatch1.function->FullName());
                matchedFunctionNames.append(" or ").append(bestMatch2.function->FullName());
                throw Cm::Core::Exception("overload resolution for overload name '" + overloadName + "' failed: call is ambiguous:\n" + matchedFunctionNames, node->GetSpan(), 
                    bestMatch1.function->GetSpan(), bestMatch2.function->GetSpan());
            }
        }
        else // !fun1 && !fun2
        {
            if (exception2)
            {
                Cm::Core::Exception copyOfEx2 = *exception2;
                throw copyOfEx2;
            }
            else if (exception1)
            {
                Cm::Core::Exception copyOfEx1 = *exception1;
                throw copyOfEx1;
            }
            else
            {
                std::string overloadName = MakeOverloadName(functionGroupName, resolutionArguments2);
                throw Cm::Core::Exception("overload resolution failed: '" + overloadName + "' not found. No viable functions taking " + std::to_string(resolutionArguments2.size()) +
                    " arguments found in function group '" + functionGroupName + "'.", node->GetSpan());
            }
        }
    }
    else if (subject->IsBoundTypeExpression())
    {
        Cm::BoundTree::BoundTypeExpression* boundTypeExpression = static_cast<Cm::BoundTree::BoundTypeExpression*>(subject.get());
        fun = BindInvokeConstructTemporary(node, conversions, arguments, boundTypeExpression->Symbol(), temporary, numArgs);
        ++numArgs;
        constructTemporary = true;
        type = boundTypeExpression->Symbol();
    }
    else 
    {
        Cm::Sym::TypeSymbol* subjectType = subject->GetType();
        Cm::Sym::TypeSymbol* plainSubjectType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePlainType(subjectType);
        if (plainSubjectType->IsDelegateTypeSymbol())
        {
            Cm::Sym::DelegateTypeSymbol* delegateType = static_cast<Cm::Sym::DelegateTypeSymbol*>(plainSubjectType);
            BindInvokeDelegate(node, delegateType, subject.release(), arguments);
            return;
        }
        else if (plainSubjectType->IsClassDelegateTypeSymbol())
        {
            Cm::Sym::ClassDelegateTypeSymbol* classDelegateType = static_cast<Cm::Sym::ClassDelegateTypeSymbol*>(plainSubjectType);
            BindInvokeClassDelegate(node, classDelegateType, subject.release(), arguments);
            return;
        }
        else if (plainSubjectType->IsClassTypeSymbol())
        {
            firstArgByRef = true;
            fun = BindInvokeOpApply(node, conversions, arguments, plainSubjectType, subject.get());
            arguments.InsertFront(subject.release());
            ++numArgs;
            type = fun->GetReturnType();
        }
        else
        {
            throw std::runtime_error("BindInvoke todo");
        }
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
            Cm::BoundTree::BoundExpression* arg = arguments[i].release();
            std::unique_ptr<Cm::BoundTree::BoundExpression>& argument = arguments[i];
            argument.reset(CreateBoundConversion(containerScope, boundCompileUnit, arg->SyntaxNode(), arg, conversionFun, currentFunction));
        }
    }
    if (constructTemporary)
    {
        Cm::BoundTree::BoundFunctionCall* functionCall = new Cm::BoundTree::BoundFunctionCall(node, std::move(arguments));
        functionCall->SetFunction(fun);
        functionCall->SetType(type);
        Cm::BoundTree::BoundLocalVariable* boundTemporary = new Cm::BoundTree::BoundLocalVariable(node, temporary);
        boundTemporary->SetFlag(Cm::BoundTree::BoundNodeFlags::argIsTemporary);
        boundTemporary->SetType(type);
        functionCall->SetTemporary(boundTemporary);
        functionCall->SetFlag(Cm::BoundTree::BoundNodeFlags::argIsTemporary);
        if (!fun->IsBasicTypeOp())
        {
            functionCall->SetTraceCallInfo(CreateTraceCallInfo(boundCompileUnit, currentFunction->GetFunctionSymbol(), node->GetSpan()));
        }
        boundExpressionStack.Push(functionCall);
        return;
    }
    else
    {
        PrepareArguments(containerScope, boundCompileUnit, currentFunction, fun->GetReturnType(), fun->Parameters(), arguments, firstArgByRef && fun->IsMemberFunctionSymbol() && !fun->IsStatic(), 
            boundCompileUnit.IrClassTypeRepository(), fun->IsBasicTypeOp());
    }
    Cm::BoundTree::BoundFunctionCall* functionCall = new Cm::BoundTree::BoundFunctionCall(node, std::move(arguments));
    uint32_t functionCallSid = boundCompileUnit.SymbolTable().GetSid();
    functionCall->SetFunctionCallSid(functionCallSid);
    functionCall->SetFunction(fun);
    functionCall->SetType(type);
    if (fun->ReturnsClassObjectByValue())
    {
        Cm::Sym::LocalVariableSymbol* classObjectResultVar = currentFunction->CreateTempLocalVariable(type);
        classObjectResultVar->SetSid(boundCompileUnit.SymbolTable().GetSid());
        functionCall->SetClassObjectResultVar(classObjectResultVar);
    }
    if (generateVirtualCall)
    {
        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::debugVCalls))
        {
            int id = boundCompileUnit.StringRepository().Install(std::to_string(functionCall->FunctionCallSid()) + "\n");
            Cm::Sym::TypeSymbol* type = boundCompileUnit.SymbolTable().GetTypeRepository().MakeConstCharPtrType(node->GetSpan());
            Cm::BoundTree::BoundStringLiteral* literalNode = new Cm::BoundTree::BoundStringLiteral(node, id);
            literalNode->SetType(type);
            functionCall->SetSidLiteral(literalNode);
        }
        functionCall->SetFlag(Cm::BoundTree::BoundNodeFlags::genVirtualCall);
    }
    if (returnClassObjectByValue)
    {
        functionCall->SetFlag(Cm::BoundTree::BoundNodeFlags::argIsTemporary);
    }
    if (!fun->IsBasicTypeOp())
    {
        functionCall->SetTraceCallInfo(CreateTraceCallInfo(boundCompileUnit, currentFunction->GetFunctionSymbol(), node->GetSpan()));
    }
    boundExpressionStack.Push(functionCall);
}

Cm::Sym::FunctionSymbol* ExpressionBinder::BindInvokeConstructTemporary(Cm::Ast::Node* node, std::vector<Cm::Sym::FunctionSymbol*>& conversions, Cm::BoundTree::BoundExpressionList& arguments,
    Cm::Sym::TypeSymbol* typeSymbol, Cm::Sym::LocalVariableSymbol*& temporary, int& numArgs)
{
    Cm::Sym::FunctionLookupSet ctorLookups;
    ctorLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base, typeSymbol->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Core::Argument> ctorResolutionArguments;
    std::vector<Cm::Sym::TypeSymbol*> boundTemplateArguments;
    temporary = currentFunction->CreateTempLocalVariable(typeSymbol);
    temporary->SetSid(boundCompileUnit.SymbolTable().GetSid());
    ctorResolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(typeSymbol, node->GetSpan())));
    if (typeSymbol->IsClassDelegateTypeSymbol())
    {
        if (arguments.Count() == 1)
        {
            Cm::Sym::ParameterSymbol* thisParam = CurrentFunction()->GetFunctionSymbol()->Parameters()[0];
            Cm::BoundTree::BoundParameter* thisParamArg = new Cm::BoundTree::BoundParameter(node, thisParam);
            thisParamArg->SetType(thisParam->GetType());
            arguments.InsertFront(thisParamArg);
            ++numArgs;
        }
        else if (arguments.Count() == 2)
        {
            arguments.Reverse();
        }
    }
    for (const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : arguments)
    {
        ctorResolutionArguments.push_back(Cm::Core::Argument(argument->GetArgumentCategory(), argument->GetType()));
    }
    Cm::Sym::FunctionSymbol* fun = ResolveOverload(containerScope, boundCompileUnit, "@constructor", ctorResolutionArguments, ctorLookups, node->GetSpan(), conversions);
    Cm::BoundTree::BoundLocalVariable* boundTemporary = new Cm::BoundTree::BoundLocalVariable(node, temporary);
    boundTemporary->SetType(typeSymbol);
    arguments.InsertFront(boundTemporary);
    if (fun->IsDelegateFromFunCtor())
    {
        DelegateFromFunCtor* delegateFromFunCtor = static_cast<DelegateFromFunCtor*>(fun);
        Cm::BoundTree::BoundFunctionId* boundFunctionId = new Cm::BoundTree::BoundFunctionId(node, delegateFromFunCtor->FunctionSymbol());
        boundFunctionId->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        boundFunctionId->SetType(BoundCompileUnit().SymbolTable().GetTypeRepository().MakePointerType(delegateFromFunCtor->DelegateType(), node->GetSpan()));
        arguments[1].reset(boundFunctionId);
    }
    else if (fun->IsClassDelegateFromFunCtor())
    {
        ClassDelegateFromFunCtor* classDelegateFromFunCtor = static_cast<ClassDelegateFromFunCtor*>(fun);
        Cm::BoundTree::BoundFunctionId* boundFunctionId = new Cm::BoundTree::BoundFunctionId(node, classDelegateFromFunCtor->FunctionSymbol());
        boundFunctionId->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        boundFunctionId->SetType(BoundCompileUnit().SymbolTable().GetTypeRepository().MakePointerType(classDelegateFromFunCtor->DelegateType(), node->GetSpan()));
        arguments[2].reset(boundFunctionId);
    }
    PrepareArguments(containerScope, boundCompileUnit, currentFunction, fun->GetReturnType(), fun->Parameters(), arguments, true, boundCompileUnit.IrClassTypeRepository(), fun->IsBasicTypeOp());
    return fun;
}

Cm::Sym::FunctionSymbol* ExpressionBinder::BindInvokeMemFun(Cm::Ast::Node* node, std::vector<Cm::Sym::FunctionSymbol*>& conversions, Cm::BoundTree::BoundExpressionList& arguments, bool& firstArgByRef, 
    bool& generateVirtualCall, const std::string& functionGroupName, int& numArgs, FunctionMatch& bestMatch, std::vector<Cm::Core::Argument>& resolutionArguments, std::unique_ptr<Cm::Core::Exception>& exception)
{
    firstArgByRef = false;
    generateVirtualCall = false;
    Cm::Sym::FunctionLookupSet memberFunLookups;
    Cm::Sym::ParameterSymbol* thisParam = currentFunction->GetFunctionSymbol()->Parameters()[0];
    Cm::Sym::TypeSymbol* thisParamType = thisParam->GetType();
    resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, thisParamType));
    memberFunLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base, thisParamType->GetBaseType()->GetContainerScope()->ClassOrNsScope()));
    bool first = true;
    for (const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : arguments)
    {
        if (argument->GetFlag(Cm::BoundTree::BoundNodeFlags::classObjectArg) && argument->GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue))
        {
            resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue,
                boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(argument->GetType()->GetBaseType(), node->GetSpan())));
            if (first)
            {
                firstArgByRef = true;
            }
        }
        else
        {
            Cm::Core::Argument resolutionArgument(argument->GetArgumentCategory(), argument->GetType());
            if (argument->GetFlag(Cm::BoundTree::BoundNodeFlags::argIsTemporary))
            {
                resolutionArgument.SetBindToRvalueRef();
            }
            resolutionArguments.push_back(resolutionArgument);
        }
        if (first)
        {
            first = false;
        }
    }
    Cm::Sym::FunctionSymbol* fun = ResolveOverload(containerScope, boundCompileUnit, functionGroupName, resolutionArguments, memberFunLookups, node->GetSpan(), conversions,
        OverloadResolutionFlags::nothrow, bestMatch, exception);
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
    bool& firstArgByRef, bool& generateVirtualCall, Cm::Sym::FunctionGroupSymbol* functionGroupSymbol, const std::vector<Cm::Sym::TypeSymbol*>& boundTemplateArguments, FunctionMatch& bestMatch, 
    std::vector<Cm::Core::Argument>& resolutionArguments, std::unique_ptr<Cm::Core::Exception>& exception, Cm::Sym::ContainerScope* qualifiedScope)
{
    Cm::Sym::FunctionLookupSet functionLookups;
    bool first = true;
    bool firstArgIsPointerOrReference = false;
    bool firstArgIsThisOrBase = false;
    firstArgByRef = false;
    generateVirtualCall = false;
    for (const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : arguments)
    {
        Cm::Sym::TypeSymbol* argumentType = argument->GetType();
        Cm::Sym::TypeSymbol* plainArgumentType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePlainType(argumentType);
        if (!qualifiedScope)
        {
            functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, plainArgumentType->GetContainerScope()->ClassOrNsScope()));
        }
        if (argument->GetFlag(Cm::BoundTree::BoundNodeFlags::classObjectArg) && argument->GetFlag(Cm::BoundTree::BoundNodeFlags::lvalue))
        {
            if (argument->GetType()->IsConstType())
            {
                resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue,
                    boundCompileUnit.SymbolTable().GetTypeRepository().MakeConstPointerType(argument->GetType()->GetBaseType(), node->GetSpan())));
            }
            else
            {
                resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue,
                    boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(argument->GetType()->GetBaseType(), node->GetSpan())));
            }
            if (first)
            {
                if (argumentType->IsReferenceType())
                {
                    argument->ResetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
                }
                else
                {
                    firstArgByRef = true;
                }
            }
        }
        else
        {
            Cm::Core::Argument resolutionArgument(argument->GetArgumentCategory(), argument->GetType());
            if (argument->GetFlag(Cm::BoundTree::BoundNodeFlags::argIsTemporary))
            {
                resolutionArgument.SetBindToRvalueRef();
            }
            resolutionArguments.push_back(resolutionArgument);
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
    if (qualifiedScope)
    {
        functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base, qualifiedScope));
    }
    else
    {
        functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, functionGroupSymbol->GetContainerScope()));
        functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::fileScopes, nullptr));
    }
    Cm::Sym::FunctionSymbol* fun = ResolveOverload(containerScope, boundCompileUnit, functionGroupSymbol->Name(), resolutionArguments, functionLookups, node->GetSpan(), conversions, 
        Cm::Sym::ConversionType::implicit, boundTemplateArguments, OverloadResolutionFlags::nothrow, bestMatch, exception);
    if (fun)
    {
        if (fun->IsVirtualAbstractOrOverride() && firstArgIsPointerOrReference && !firstArgIsThisOrBase)
        {
            generateVirtualCall = true;
        }
    }
    return fun;
}

Cm::Sym::FunctionSymbol* ExpressionBinder::BindInvokeOpApply(Cm::Ast::Node* node, std::vector<Cm::Sym::FunctionSymbol*>& conversions, Cm::BoundTree::BoundExpressionList& arguments,
    Cm::Sym::TypeSymbol* plainSubjectType, Cm::BoundTree::BoundExpression* subject)
{
    Cm::Sym::FunctionLookupSet functionLookups;
    std::vector<Cm::Core::Argument> resolutionArguments;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base, plainSubjectType->GetContainerScope()->ClassOrNsScope()));
    Cm::Sym::TypeSymbol* subjectPtrType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(plainSubjectType, node->GetSpan());
    resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, subjectPtrType));
    for (const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : arguments)
    {
        Cm::Core::Argument resolutionArgument(argument->GetArgumentCategory(), argument->GetType());
        if (argument->GetFlag(Cm::BoundTree::BoundNodeFlags::argIsTemporary))
        {
            resolutionArgument.SetBindToRvalueRef();
        }
        resolutionArguments.push_back(resolutionArgument);
    }
    Cm::Sym::FunctionSymbol* fun = ResolveOverload(containerScope, boundCompileUnit, "operator()", resolutionArguments, functionLookups, node->GetSpan(), conversions);
    return fun;
}

void ExpressionBinder::BindInvokeDelegate(Cm::Ast::Node* node, Cm::Sym::DelegateTypeSymbol* delegateType, Cm::BoundTree::BoundExpression* subject, Cm::BoundTree::BoundExpressionList& arguments)
{
    int numParams = int(delegateType->Parameters().size());
    if (arguments.Count() != numParams)
    {
        throw Cm::Core::Exception("wrong number of arguments to delegate call (got " + std::to_string(arguments.Count()) + ", need " + std::to_string(numParams) + ")", node->GetSpan());
    }
    std::vector<Cm::Core::Argument> resolutionArguments;
    for (const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : arguments)
    {
        Cm::Core::Argument resolutionArgument(argument->GetArgumentCategory(), argument->GetType());
        if (argument->GetFlag(Cm::BoundTree::BoundNodeFlags::argIsTemporary))
        {
            resolutionArgument.SetBindToRvalueRef();
        }
        resolutionArguments.push_back(resolutionArgument);
    }
    std::unordered_set<Cm::Sym::ClassTypeSymbol*> conversionClassTypes;
    Cm::Bind::FunctionMatch functionMatch(nullptr, containerScope, &boundCompileUnit);
    if (FindConversions(boundCompileUnit, delegateType->Parameters(), resolutionArguments, Cm::Sym::ConversionType::implicit, node->GetSpan(), functionMatch, conversionClassTypes))
    {
        int n = int(functionMatch.conversions.size());
        if (n != arguments.Count())
        {
            throw std::runtime_error("wrong number of arguments");
        }
        for (int i = 0; i < n; ++i)
        {
            Cm::Sym::FunctionSymbol* conversion = functionMatch.conversions[i];
            if (conversion)
            {
                Cm::BoundTree::BoundExpression* arg = arguments[i].release();
                arguments[i].reset(CreateBoundConversion(containerScope, boundCompileUnit, node, arg, conversion, currentFunction));
            }
        }
        PrepareArguments(containerScope, boundCompileUnit, currentFunction, delegateType->GetReturnType(), delegateType->Parameters(), arguments, false, boundCompileUnit.IrClassTypeRepository(), false);
        Cm::BoundTree::BoundDelegateCall* delegateCall = new Cm::BoundTree::BoundDelegateCall(delegateType, subject, node, std::move(arguments));
        delegateCall->SetType(delegateType->GetReturnType());
        boundExpressionStack.Push(delegateCall);
    }
    else
    {
        std::string errorMessage = "delegate resolution failed: there are no acceptable conversions for all argument types.";
        throw Cm::Core::Exception(errorMessage, node->GetSpan());
    }
}

void ExpressionBinder::BindInvokeClassDelegate(Cm::Ast::Node* node, Cm::Sym::ClassDelegateTypeSymbol* classDelegateType, Cm::BoundTree::BoundExpression* subject, 
    Cm::BoundTree::BoundExpressionList& arguments)
{
    subject->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
    int numParams = int(classDelegateType->Parameters().size());
    if (arguments.Count() != numParams)
    {
        throw Cm::Core::Exception("wrong number of arguments to class delegate call (got " + std::to_string(arguments.Count()) + ", need " + std::to_string(numParams) + ")", node->GetSpan());
    }
    std::vector<Cm::Core::Argument> resolutionArguments;
    for (const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : arguments)
    {
        Cm::Core::Argument resolutionArgument(argument->GetArgumentCategory(), argument->GetType());
        if (argument->GetFlag(Cm::BoundTree::BoundNodeFlags::argIsTemporary))
        {
            resolutionArgument.SetBindToRvalueRef();
        }
        resolutionArguments.push_back(resolutionArgument);
    }
    std::unordered_set<Cm::Sym::ClassTypeSymbol*> conversionClassTypes;
    Cm::Bind::FunctionMatch functionMatch(nullptr, containerScope, &boundCompileUnit);
    if (FindConversions(boundCompileUnit, classDelegateType->Parameters(), resolutionArguments, Cm::Sym::ConversionType::implicit, node->GetSpan(), functionMatch, conversionClassTypes))
    {
        int n = int(functionMatch.conversions.size());
        if (n != arguments.Count())
        {
            throw std::runtime_error("wrong number of arguments");
        }
        for (int i = 0; i < n; ++i)
        {
            Cm::Sym::FunctionSymbol* conversion = functionMatch.conversions[i];
            if (conversion)
            {
                arguments[i].reset(CreateBoundConversion(containerScope, boundCompileUnit, node, arguments[i].release(), conversion, currentFunction));
            }
        }
        PrepareArguments(containerScope, boundCompileUnit, currentFunction, classDelegateType->GetReturnType(), classDelegateType->Parameters(), arguments, false, boundCompileUnit.IrClassTypeRepository(), false);
        Cm::BoundTree::BoundClassDelegateCall* classDelegateCall = new Cm::BoundTree::BoundClassDelegateCall(classDelegateType, subject, node, std::move(arguments));
        classDelegateCall->SetType(classDelegateType->GetReturnType());
        boundExpressionStack.Push(classDelegateCall);
    }
    else
    {
        std::string errorMessage = "class delegate resolution failed: there are no acceptable conversions for all argument types.";
        throw Cm::Core::Exception(errorMessage, node->GetSpan());
    }
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
        case Cm::Sym::SymbolType::delegateSymbol:
        {
            Cm::Sym::DelegateTypeSymbol* delegateTypeSymbol = static_cast<Cm::Sym::DelegateTypeSymbol*>(symbol);
            BindDelegateTypeSymbol(node, delegateTypeSymbol);
            break;
        }
        case Cm::Sym::SymbolType::classDelegateSymbol:
        {
            Cm::Sym::ClassDelegateTypeSymbol* classDelegateSymbol = static_cast<Cm::Sym::ClassDelegateTypeSymbol*>(symbol);
            BindClassDelegateTypeSymbol(node, classDelegateSymbol);
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
        case Cm::Sym::SymbolType::typedefSymbol:
        {
            Cm::Sym::TypedefSymbol* typedefSymbol = static_cast<Cm::Sym::TypedefSymbol*>(symbol);
            BindTypedefSymbol(node, typedefSymbol);
            break;
        }
        case Cm::Sym::SymbolType::boundTypeParameterSymbol:
        {
            Cm::Sym::BoundTypeParameterSymbol* boundTypeParameterSymbol = static_cast<Cm::Sym::BoundTypeParameterSymbol*>(symbol);
            BindBoundTypeParameterSymbol(node, boundTypeParameterSymbol);
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
        Cm::Ast::Node* node = boundCompileUnit.SymbolTable().GetNode(constantSymbol);
        if (node->IsConstantNode())
        {
            Cm::Ast::ConstantNode* constantNode = static_cast<Cm::Ast::ConstantNode*>(node);
            Cm::Sym::ContainerScope* constantScope = boundCompileUnit.SymbolTable().GetContainerScope(constantNode);
            BindConstant(boundCompileUnit.SymbolTable(), constantScope, fileScopes, boundCompileUnit.ClassTemplateRepository(), constantNode, constantSymbol);
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
        Cm::Ast::Node* node = boundCompileUnit.SymbolTable().GetNode(localVariableSymbol);
        if (node->IsConstructionStatementNode())
        {
            Cm::Ast::ConstructionStatementNode* constructionStatementNode = static_cast<Cm::Ast::ConstructionStatementNode*>(node);
            Cm::Sym::ContainerScope* localVariableScope = boundCompileUnit.SymbolTable().GetContainerScope(constructionStatementNode);
            //BindLocalVariable(boundCompileUnit.SymbolTable(), containerScope, fileScopes, boundCompileUnit.ClassTemplateRepository(), constructionStatementNode, localVariableSymbol);
            BindLocalVariable(boundCompileUnit.SymbolTable(), localVariableScope, fileScopes, boundCompileUnit.ClassTemplateRepository(), constructionStatementNode, localVariableSymbol);
        }
        else
        {
            throw std::runtime_error("not construction statement node");
        }
    }
    Cm::BoundTree::BoundLocalVariable* boundLocalVariable = new Cm::BoundTree::BoundLocalVariable(idNode, localVariableSymbol);
    boundLocalVariable->SetType(localVariableSymbol->GetType());
    boundExpressionStack.Push(boundLocalVariable);
    if (!localVariableSymbol->Used())
    {
        localVariableSymbol->SetUsed();
        localVariableSymbol->SetUseSpan(idNode->GetSpan());
    }
}

void ExpressionBinder::BindMemberVariableSymbol(Cm::Ast::Node* idNode, Cm::Sym::MemberVariableSymbol* memberVariableSymbol)
{
    if (!memberVariableSymbol->Bound())
    {
        Cm::Ast::Node* node = boundCompileUnit.SymbolTable().GetNode(memberVariableSymbol);
        if (node->IsMemberVariableNode())
        {
            Cm::Ast::MemberVariableNode* memberVariableNode = static_cast<Cm::Ast::MemberVariableNode*>(node);
            Cm::Sym::ContainerScope* memberVariableScope = boundCompileUnit.SymbolTable().GetContainerScope(memberVariableNode);
            //BindMemberVariable(boundCompileUnit.SymbolTable(), containerScope, fileScopes, boundCompileUnit.ClassTemplateRepository(), memberVariableNode, memberVariableSymbol);
            BindMemberVariable(boundCompileUnit.SymbolTable(), memberVariableScope, fileScopes, boundCompileUnit.ClassTemplateRepository(), memberVariableNode, memberVariableSymbol);
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
        Cm::Ast::Node* node = boundCompileUnit.SymbolTable().GetNode(classTypeSymbol);
        if (node->IsClassNode())
        {
            Cm::Ast::ClassNode* classNode = static_cast<Cm::Ast::ClassNode*>(node);
            Cm::Sym::ContainerScope* classScope = boundCompileUnit.SymbolTable().GetContainerScope(classNode);
            BindClass(boundCompileUnit.SymbolTable(), classScope, fileScopes, boundCompileUnit.ClassTemplateRepository(), classNode, classTypeSymbol);
        }
        else
        {
            throw std::runtime_error("not class node");
        }
    }
    Cm::BoundTree::BoundTypeExpression* boundType = new Cm::BoundTree::BoundTypeExpression(idNode, classTypeSymbol);
    boundType->SetType(classTypeSymbol);
    boundExpressionStack.Push(boundType);
}

void ExpressionBinder::BindDelegateTypeSymbol(Cm::Ast::Node* idNode, Cm::Sym::DelegateTypeSymbol* delegateTypeSymbol)
{
    Cm::BoundTree::BoundTypeExpression* boundType = new Cm::BoundTree::BoundTypeExpression(idNode, delegateTypeSymbol);
    boundType->SetType(delegateTypeSymbol);
    boundExpressionStack.Push(boundType);
}

void ExpressionBinder::BindClassDelegateTypeSymbol(Cm::Ast::Node* idNode, Cm::Sym::ClassDelegateTypeSymbol* classDelegateTypeSymbol)
{
    Cm::BoundTree::BoundTypeExpression* boundType = new Cm::BoundTree::BoundTypeExpression(idNode, classDelegateTypeSymbol);
    boundType->SetType(classDelegateTypeSymbol);
    boundExpressionStack.Push(boundType);
}

void ExpressionBinder::BindNamespaceSymbol(Cm::Ast::Node* idNode, Cm::Sym::NamespaceSymbol* namespaceSymbol)
{
    Cm::BoundTree::BoundNamespaceExpression* boundNamespace = new Cm::BoundTree::BoundNamespaceExpression(idNode, namespaceSymbol);
    boundExpressionStack.Push(boundNamespace);
}

void ExpressionBinder::BindEnumTypeSymbol(Cm::Ast::Node* idNode, Cm::Sym::EnumTypeSymbol* enumTypeSymbol)
{
    Cm::BoundTree::BoundTypeExpression* boundType = new Cm::BoundTree::BoundTypeExpression(idNode, enumTypeSymbol);
    boundType->SetType(enumTypeSymbol);
    boundExpressionStack.Push(boundType);
}

void ExpressionBinder::BindEnumConstantSymbol(Cm::Ast::Node* idNode, Cm::Sym::EnumConstantSymbol* enumConstantSymbol)
{
    Cm::Sym::TypeSymbol* enumType = nullptr;
    Cm::Sym::Symbol* enumConstantParent = enumConstantSymbol->Parent();
    if (enumConstantParent)
    {
        if (enumConstantParent->IsEnumTypeSymbol())
        {
            enumType = static_cast<Cm::Sym::EnumTypeSymbol*>(enumConstantParent);
        }
    }
    if (!enumType)
    {
        throw Cm::Core::Exception("enum type for enum constant not found", idNode->GetSpan());
    }
    Cm::BoundTree::BoundEnumConstant* boundEnumConstant = new Cm::BoundTree::BoundEnumConstant(idNode, enumConstantSymbol);
    boundEnumConstant->SetType(enumType);
    boundExpressionStack.Push(boundEnumConstant);
}

void ExpressionBinder::BindTypedefSymbol(Cm::Ast::Node* idNode, Cm::Sym::TypedefSymbol* typedefSymbol)
{
    CheckAccess(currentFunction->GetFunctionSymbol(), idNode->GetSpan(), typedefSymbol);
    Cm::BoundTree::BoundTypeExpression* boundTypeExpr = new Cm::BoundTree::BoundTypeExpression(idNode, typedefSymbol->GetType());
    boundTypeExpr->SetType(typedefSymbol->GetType());
    boundExpressionStack.Push(boundTypeExpr);
}

void ExpressionBinder::BindBoundTypeParameterSymbol(Cm::Ast::Node* idNode, Cm::Sym::BoundTypeParameterSymbol* boundTypeParameterSymbol)
{
    Cm::BoundTree::BoundTypeExpression* boundTypeExpr = new Cm::BoundTree::BoundTypeExpression(idNode, boundTypeParameterSymbol->GetType());
    boundTypeExpr->SetType(boundTypeParameterSymbol->GetType());
    boundExpressionStack.Push(boundTypeExpr);
}

void ExpressionBinder::BindFunctionGroup(Cm::Ast::Node* idNode, Cm::Sym::FunctionGroupSymbol* functionGroupSymbol)
{
    Cm::BoundTree::BoundFunctionGroup* boundFunctionGroup = new Cm::BoundTree::BoundFunctionGroup(idNode, functionGroupSymbol);
    boundFunctionGroup->SetType(new Cm::Sym::FunctionGroupTypeSymbol(functionGroupSymbol));
    boundExpressionStack.Push(boundFunctionGroup);
}

void ExpressionBinder::Visit(Cm::Ast::SizeOfNode& sizeOfNode) 
{
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupSizeOfSubject;
    sizeOfNode.Subject()->Accept(*this);
    lookupId = lookupIdStack.Pop();
    std::unique_ptr<Cm::BoundTree::BoundExpression> subject(boundExpressionStack.Pop());
    Cm::BoundTree::BoundSizeOfExpression* boundSizeOfExpr = new Cm::BoundTree::BoundSizeOfExpression(&sizeOfNode, subject->GetType());
    boundSizeOfExpr->SetType(boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ulongId)));
    boundExpressionStack.Push(boundSizeOfExpr);
}

void ExpressionBinder::BindCast(Cm::Ast::Node* node, Cm::Ast::Node* targetTypeExpr, Cm::Ast::Node* sourceExpr, const Cm::Parsing::Span& span)
{
    Cm::Sym::TypeSymbol* toType = ResolveType(boundCompileUnit.SymbolTable(), containerScope, fileScopes, boundCompileUnit.ClassTemplateRepository(), targetTypeExpr);
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupCastSource;
    sourceExpr->Accept(*this);
    lookupId = lookupIdStack.Pop();
    Cm::BoundTree::BoundExpression* operand = boundExpressionStack.Pop();
    BindCast(node, toType, operand);
}

void ExpressionBinder::BindCast(Cm::Ast::Node* node, Cm::Sym::TypeSymbol* targetType, Cm::BoundTree::BoundExpression* sourceExpr)
{
    std::vector<Cm::Core::Argument> resolutionArguments;
    resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(targetType, node->GetSpan())));
    resolutionArguments.push_back(Cm::Core::Argument(sourceExpr->GetArgumentCategory(), sourceExpr->GetType()));
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, targetType->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* convertingCtor = ResolveOverload(containerScope, boundCompileUnit, "@constructor", resolutionArguments, functionLookups, node->GetSpan(), conversions, Cm::Sym::ConversionType::explicit_,
        OverloadResolutionFlags::none);
    if (conversions.size() != 2)
    {
        throw std::runtime_error("wrong number of conversions");
    }
    Cm::Sym::FunctionSymbol* conversion = conversions[1];
    if (conversion)
    {
        sourceExpr = CreateBoundConversion(containerScope, boundCompileUnit, node, sourceExpr, conversion, CurrentFunction());
    }
    Cm::BoundTree::BoundCast* cast = new Cm::BoundTree::BoundCast(node, sourceExpr, convertingCtor);
    cast->SetType(targetType);
    cast->SetSourceType(sourceExpr->GetType());
    boundExpressionStack.Push(cast);
}

void ExpressionBinder::Visit(Cm::Ast::CastNode& castNode)
{
    Cm::Ast::Node* targetTypeExpr = castNode.TargetTypeExpr();
    Cm::Ast::Node* sourceExpr = castNode.SourceExpr();
    BindCast(&castNode, targetTypeExpr, sourceExpr, castNode.GetSpan());
}

void ExpressionBinder::Visit(Cm::Ast::IsNode& isNode)
{
    Cm::Ast::Node* expr = isNode.Expr();
    expr->Accept(*this);
    std::unique_ptr<Cm::BoundTree::BoundExpression> boundExpr(Pop());
    if (boundExpr->GetType()->IsPointerToClassType())
    {
        Cm::Sym::TypeSymbol* exprBaseType = boundExpr->GetType()->GetBaseType();
        if (exprBaseType->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* exprClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(exprBaseType);
            if (exprClassType->IsVirtual())
            {
                Cm::Ast::Node* typeExpr = isNode.TypeExpr();
                Cm::Sym::TypeSymbol* type = ResolveType(boundCompileUnit.SymbolTable(), containerScope, boundCompileUnit.GetFileScopes(), boundCompileUnit.ClassTemplateRepository(), typeExpr);
                if (type->IsPointerToClassType())
                {
                    Cm::Sym::TypeSymbol* baseType = type->GetBaseType();
                    if (baseType->IsClassTypeSymbol())
                    {
                        Cm::Sym::ClassTypeSymbol* rightClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(baseType);
                        if (rightClassType->IsVirtual())
                        {
                            Cm::BoundTree::BoundIsExpression* boundIsExpression = new Cm::BoundTree::BoundIsExpression(&isNode, boundExpr.release(), exprClassType, rightClassType);
                            Cm::Sym::TypeSymbol* boolType = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
                            boundIsExpression->SetType(boolType);
                            boundExpressionStack.Push(boundIsExpression);
                        }
                        else
                        {
                            throw Cm::Core::Exception("right operand of is-expression must be pointer to virtual class type", typeExpr->GetSpan());
                        }
                    }
                    else
                    {
                        throw Cm::Core::Exception("right operand of is-expression must be pointer to virtual class type", typeExpr->GetSpan());
                    }
                }
                else
                {
                    throw Cm::Core::Exception("right operand of is-expression must be pointer to virtual class type", typeExpr->GetSpan());
                }
            }
            else
            {
                throw Cm::Core::Exception("type of left operand of is-expression must be pointer to virtual class type", expr->GetSpan());
            }
        }
        else
        {
            throw Cm::Core::Exception("type of left operand of is-expression must be pointer to virtual class type", expr->GetSpan());
        }
    }
    else
    {
        throw Cm::Core::Exception("type of left operand of is-expression must be pointer to virtual class type", expr->GetSpan());
    }
}

void ExpressionBinder::Visit(Cm::Ast::AsNode& asNode)
{
    Cm::Ast::Node* expr = asNode.Expr();
    expr->Accept(*this);
    std::unique_ptr<Cm::BoundTree::BoundExpression> boundExpr(Pop());
    if (boundExpr->GetType()->IsPointerToClassType())
    {
        Cm::Sym::TypeSymbol* exprBaseType = boundExpr->GetType()->GetBaseType();
        if (exprBaseType->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* exprClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(exprBaseType);
            if (exprClassType->IsVirtual())
            {
                Cm::Ast::Node* typeExpr = asNode.TypeExpr();
                Cm::Sym::TypeSymbol* type = ResolveType(boundCompileUnit.SymbolTable(), containerScope, boundCompileUnit.GetFileScopes(), boundCompileUnit.ClassTemplateRepository(), typeExpr);
                if (type->IsPointerToClassType())
                {
                    Cm::Sym::TypeSymbol* baseType = type->GetBaseType();
                    if (baseType->IsClassTypeSymbol())
                    {
                        Cm::Sym::ClassTypeSymbol* rightClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(baseType);
                        if (rightClassType->IsVirtual())
                        {
                            Cm::BoundTree::BoundAsExpression* boundAsExpression = new Cm::BoundTree::BoundAsExpression(&asNode, boundExpr.release(), exprClassType, rightClassType);
                            boundAsExpression->SetType(type);
                            Cm::Sym::LocalVariableSymbol* temporary = currentFunction->CreateTempLocalVariable(type);
                            temporary->SetSid(boundCompileUnit.SymbolTable().GetSid());
                            Cm::BoundTree::BoundExpression* boundTemporary = new Cm::BoundTree::BoundLocalVariable(&asNode, temporary);
                            boundTemporary->SetType(type);
                            boundTemporary->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
                            boundAsExpression->SetBoundTemporary(boundTemporary);
                            boundExpressionStack.Push(boundAsExpression);
                        }
                        else
                        {
                            throw Cm::Core::Exception("right operand of as-expression must be pointer to virtual class type", typeExpr->GetSpan());
                        }
                    }
                    else
                    {
                        throw Cm::Core::Exception("right operand of as-expression must be pointer to virtual class type", typeExpr->GetSpan());
                    }
                }
                else
                {
                    throw Cm::Core::Exception("right operand of as-expression must be pointer to virtual class type", typeExpr->GetSpan());
                }
            }
            else
            {
                throw Cm::Core::Exception("type of left operand of as-expression must be pointer to virtual class type", expr->GetSpan());
            }
        }
        else
        {
            throw Cm::Core::Exception("type of left operand of as-expression must be pointer to virtual class type", expr->GetSpan());
        }
    }
    else
    {
        throw Cm::Core::Exception("type of left operand of as-expression must be pointer to virtual class type", expr->GetSpan());
    }
}

void ExpressionBinder::BindConstruct(Cm::Ast::Node* node, Cm::Ast::Node* typeExpr, Cm::Ast::NodeList& argumentNodes, Cm::BoundTree::BoundExpression* allocationArg)
{
    Cm::Sym::TypeSymbol* type = ResolveType(boundCompileUnit.SymbolTable(), containerScope, boundCompileUnit.GetFileScopes(), boundCompileUnit.ClassTemplateRepository(), typeExpr);
    if (type->IsReferenceType() || type->IsRvalueRefType())
    {
        throw Cm::Core::Exception("cannot construct a reference", node->GetSpan());
    }
    if (type->IsAbstract())
    {
        throw Cm::Core::Exception("cannot instantiate an abstract class", type->GetSpan(), node->GetSpan());
    }
    Cm::Sym::TypeSymbol* returnType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(type, node->GetSpan());
    int n = argumentNodes.Count();
    if (n == 0 && !allocationArg)
    {
        throw Cm::Core::Exception("must supply at least one argument to construct expression", node->GetSpan());
    }
    for (int i = 0; i < n; ++i)
    {
        Cm::Ast::Node* argument = argumentNodes[i];
        lookupIdStack.Push(lookupId);
        lookupId = Cm::Sym::SymbolTypeSetId::lookupArgumentSymbol;
        argument->Accept(*this);
        lookupId = lookupIdStack.Pop();
    }
    Cm::BoundTree::BoundExpressionList arguments = boundExpressionStack.Pop(n);
    if (allocationArg)
    {
        arguments.InsertFront(allocationArg);
    }
    Cm::Sym::TypeSymbol* pointerType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePlainType(arguments[0]->GetType());
    if (!pointerType->IsPointerType())
    {
        throw Cm::Core::Exception("first argument of a construct expression must be of a pointer type", node->GetSpan());
    }
    if (pointerType->IsVoidPtrType())
    {
        Cm::Ast::DerivationList pointerDerivation;
        pointerDerivation.Add(Cm::Ast::Derivation::pointer);
        Cm::Ast::CloneContext cloneContext;
        Cm::Ast::DerivedTypeExprNode pointerTypeNode(node->GetSpan(), pointerDerivation, typeExpr->Clone(cloneContext));
        BindCast(node, &pointerTypeNode, argumentNodes[0], node->GetSpan());
        Cm::BoundTree::BoundExpression* cast = boundExpressionStack.Pop();
        arguments[0].reset(cast);
    }
    else if (!Cm::Sym::TypesEqual(returnType, pointerType))
    {
        throw Cm::Core::Exception("type of the first argument conflicts with the return type of the construct expression", node->GetSpan());
    }
    std::vector<Cm::Core::Argument> resolutionArguments;
    resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, returnType));
    if (allocationArg)
    {
        ++n;
    }
    for (int i = 1; i < n; ++i)
    {
        Cm::Core::Argument resolutionArgument(arguments[i]->GetArgumentCategory(), arguments[i]->GetType());
        if (arguments[i]->GetFlag(Cm::BoundTree::BoundNodeFlags::argIsTemporary))
        {
            resolutionArgument.SetBindToRvalueRef();
        }
        resolutionArguments.push_back(resolutionArgument);
    }
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, type->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* ctor = ResolveOverload(containerScope, boundCompileUnit, "@constructor", resolutionArguments, functionLookups, node->GetSpan(), conversions);
    PrepareFunctionSymbol(ctor, node->GetSpan());
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::FunctionSymbol* conversionFun = conversions[i];
        if (conversionFun)
        {
            Cm::BoundTree::BoundExpression* arg = arguments[i].release();
            arguments[i].reset(CreateBoundConversion(containerScope, boundCompileUnit, node, arg, conversionFun, currentFunction));
        }
    }
    PrepareArguments(containerScope, boundCompileUnit, currentFunction, nullptr, ctor->Parameters(), arguments, false, boundCompileUnit.IrClassTypeRepository(), ctor->IsBasicTypeOp());
    Cm::BoundTree::BoundFunctionCall* functionCall = new Cm::BoundTree::BoundFunctionCall(node, std::move(arguments));
    functionCall->SetFunction(ctor);
    functionCall->SetType(returnType);
    boundExpressionStack.Push(functionCall);
    if (node->IsNewNode())
    {
        functionCall->SetFlag(Cm::BoundTree::BoundNodeFlags::newCall);
    }
}

void ExpressionBinder::BindConstruct(Cm::Ast::Node* node, Cm::Ast::Node* typeExpr, Cm::Ast::NodeList& argumentNodes)
{
    BindConstruct(node, typeExpr, argumentNodes, nullptr);
}

void ExpressionBinder::Visit(Cm::Ast::ConstructNode& constructNode)
{
    Cm::Ast::Node* typeExpr = constructNode.TypeExpr();
    BindConstruct(&constructNode, typeExpr, constructNode.Arguments());
}

void ExpressionBinder::Visit(Cm::Ast::NewNode& newNode)
{
    Cm::Sym::FunctionSymbol* memAlloc = nullptr;
    if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::debug_heap))
    {
        memAlloc = boundCompileUnit.SymbolTable().GetOverload("System.Support.DebugHeapMemAlloc");
    }
    else
    {
        memAlloc = boundCompileUnit.SymbolTable().GetOverload("System.Support.MemAlloc");
    }
    Cm::BoundTree::BoundExpressionList memAllocArguments;
    Cm::Sym::TypeSymbol* type = ResolveType(boundCompileUnit.SymbolTable(), containerScope, boundCompileUnit.GetFileScopes(), boundCompileUnit.ClassTemplateRepository(), newNode.TypeExpr());
    Cm::BoundTree::BoundSizeOfExpression* boundSizeOfExpr = new Cm::BoundTree::BoundSizeOfExpression(&newNode, type);
    boundSizeOfExpr->SetType(boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ulongId)));
    memAllocArguments.Add(boundSizeOfExpr);
    Cm::BoundTree::BoundFunctionCall* memAllocCall = new Cm::BoundTree::BoundFunctionCall(&newNode, std::move(memAllocArguments));
    memAllocCall->SetFunction(memAlloc);
    memAllocCall->SetType(boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)), newNode.GetSpan()));
    BindCast(&newNode, boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(type, newNode.GetSpan()), memAllocCall);
    Cm::BoundTree::BoundExpression* castedMemAllocCall = boundExpressionStack.Pop();
    BindConstruct(&newNode, newNode.TypeExpr(), newNode.Arguments(), castedMemAllocCall);
}

void ExpressionBinder::Visit(Cm::Ast::TemplateIdNode& templateIdNode)
{
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupTypeAndFunctionGroupSymbols;
    templateIdNode.Subject()->Accept(*this);
    lookupId = lookupIdStack.Pop();
    std::unique_ptr<Cm::BoundTree::BoundExpression> subject(boundExpressionStack.Pop());
    if (subject->IsBoundFunctionGroup())
    {
        std::vector<Cm::Sym::TypeSymbol*> boundTemplateArguments;
        Cm::BoundTree::BoundFunctionGroup* boundFunctionGroup = static_cast<Cm::BoundTree::BoundFunctionGroup*>(subject.get());
        for (const std::unique_ptr<Cm::Ast::Node>& templateArgument : templateIdNode.TemplateArguments())
        {
            Cm::Sym::TypeSymbol* templateArgumentType = ResolveType(boundCompileUnit.SymbolTable(), containerScope, boundCompileUnit.GetFileScopes(), boundCompileUnit.ClassTemplateRepository(), 
                templateArgument.get());
            boundTemplateArguments.push_back(templateArgumentType);
        }
        boundFunctionGroup->SetBoundTemplateArguments(boundTemplateArguments);
        boundFunctionGroup->GetFunctionGroupSymbol()->SetBoundTemplateArguments(boundTemplateArguments);
        boundExpressionStack.Push(subject.release());
    }
    else if (subject->IsBoundTypeExpression())
    {
        Cm::BoundTree::BoundTypeExpression* boundTypeExpression = static_cast<Cm::BoundTree::BoundTypeExpression*>(subject.get());
        Cm::Sym::TypeSymbol* subjectType = boundTypeExpression->Symbol();
        std::vector<Cm::Sym::TypeSymbol*> typeArguments;
        for (const std::unique_ptr<Cm::Ast::Node>& templateArgument : templateIdNode.TemplateArguments())
        {
            Cm::Sym::TypeSymbol* templateArgumentType = ResolveType(boundCompileUnit.SymbolTable(), containerScope, boundCompileUnit.GetFileScopes(), boundCompileUnit.ClassTemplateRepository(), 
                templateArgument.get());
            typeArguments.push_back(templateArgumentType);
        }
        Cm::Sym::TypeSymbol* templateTypeSymbol = boundCompileUnit.SymbolTable().GetTypeRepository().MakeTemplateType(subjectType, typeArguments, templateIdNode.GetSpan());
        boundExpressionStack.Push(new Cm::BoundTree::BoundTypeExpression(&templateIdNode, templateTypeSymbol));
    }
    else
    {
        throw Cm::Core::Exception("function or class template symbol expected", templateIdNode.GetSpan());
    }
}

void ExpressionBinder::Visit(Cm::Ast::IdentifierNode& identifierNode)
{
    if (identifierNode.Str() == Cm::IrIntf::GetExCodeVarName())
    {
        Cm::BoundTree::BoundExceptionCodeVariable* exceptionCodeVariable = new Cm::BoundTree::BoundExceptionCodeVariable();
        exceptionCodeVariable->SetType(boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId)));
        boundExpressionStack.Push(exceptionCodeVariable);
        return;
    }
    else if (identifierNode.Str() == Cm::IrIntf::GetExceptionCodeParamName())
    {
        Cm::BoundTree::BoundExceptionCodeParameter* exceptionCodeParam = new Cm::BoundTree::BoundExceptionCodeParameter();
        exceptionCodeParam->SetType(boundCompileUnit.SymbolTable().GetTypeRepository().MakeReferenceType(
            boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId)), identifierNode.GetSpan()));
        boundExpressionStack.Push(exceptionCodeParam);
        return;
    }
    else if (identifierNode.Str() == Cm::IrIntf::GetExceptionBaseIdTableName())
    {
        Cm::BoundTree::BoundExceptionTableConstant* exceptionTableConstant = new Cm::BoundTree::BoundExceptionTableConstant(&identifierNode);
        exceptionTableConstant->SetType(boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(
            boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId)), identifierNode.GetSpan()));
        boundExpressionStack.Push(exceptionTableConstant);
        return;
    }
    Cm::Sym::Symbol* symbol = containerScope->Lookup(identifierNode.Str(), Cm::Sym::ScopeLookup::this_and_base_and_parent, lookupId);
    if (!symbol)
    {
        for (const std::unique_ptr<Cm::Sym::FileScope>& fileScope : fileScopes)
        {
            symbol = fileScope->Lookup(identifierNode.Str(), lookupId);
            if (symbol) break;
        }
    }
    if (symbol)
    {
        BindSymbol(&identifierNode, symbol);
    }
    else
    {
        throw Cm::Core::Exception("symbol '" + identifierNode.Str() + "' not found", identifierNode.GetSpan());
    }
}

void ExpressionBinder::Visit(Cm::Ast::ThisNode& thisNode)
{
    if (currentFunction->GetFunctionSymbol()->IsMemberFunctionSymbol() && !currentFunction->GetFunctionSymbol()->IsStatic())
    {
        Cm::Sym::ParameterSymbol* thisParam = currentFunction->GetFunctionSymbol()->Parameters()[0];
        Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(&thisNode, thisParam);
        boundThisParam->SetType(thisParam->GetType());
        boundThisParam->SetFlag(Cm::BoundTree::BoundNodeFlags::argIsThisOrBase);
        boundExpressionStack.Push(boundThisParam);
    }
    else
    {
        throw Cm::Core::Exception("'this' can be used only in non-static member function context", thisNode.GetSpan());
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
            Cm::Sym::TypeSymbol* baseClassPtrType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(baseClassType, baseNode.GetSpan());
            Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(&baseNode, thisParam);
            boundThisParam->SetType(thisParam->GetType());
            Cm::Sym::FunctionSymbol* conversionFun = boundCompileUnit.ClassConversionTable().MakeBaseClassDerivedClassConversion(baseClassPtrType, thisParam->GetType(), 1, baseNode.GetSpan());
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

void ExpressionBinder::Visit(Cm::Ast::TypeNameNode& typeNameNode)
{
    Cm::Sym::TypeSymbol* constCharPtrType = boundCompileUnit.SymbolTable().GetTypeRepository().MakeConstCharPtrType(typeNameNode.GetSpan());
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupTypenameSubject;
    typeNameNode.Subject()->Accept(*this);
    lookupId = lookupIdStack.Pop();
    std::unique_ptr<Cm::BoundTree::BoundExpression> subject(Pop());
    Cm::Sym::TypeSymbol* subjectType = subject->GetType();
    Cm::Sym::TypeSymbol* plainType = BoundCompileUnit().SymbolTable().GetTypeRepository().MakePlainType(subjectType);
    if (plainType->IsClassTypeSymbol())
    {
        Cm::Sym::ClassTypeSymbol* classType = static_cast<Cm::Sym::ClassTypeSymbol*>(plainType);
        if (classType->IsVirtual())
        {
            Cm::BoundTree::BoundDynamicTypeNameExpression* boundDynamicTypeNameExpression = new Cm::BoundTree::BoundDynamicTypeNameExpression(&typeNameNode, subject.release(), classType);
            boundDynamicTypeNameExpression->SetType(constCharPtrType);
            boundExpressionStack.Push(boundDynamicTypeNameExpression);
            return;
        }
    }
    int id = boundCompileUnit.StringRepository().Install(subjectType->FullName());
    Cm::BoundTree::BoundStringLiteral* literalNode = new Cm::BoundTree::BoundStringLiteral(&typeNameNode, id);
    literalNode->SetType(constCharPtrType);
    boundExpressionStack.Push(literalNode);
}

void ExpressionBinder::GenerateTrueExpression(Cm::Ast::Node* node)
{
    Cm::Sym::TypeSymbol* type = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
    Cm::Sym::Value* value1 = new Cm::Sym::BoolValue(true);
    Cm::BoundTree::BoundLiteral* literalNode1 = new Cm::BoundTree::BoundLiteral(node);
    literalNode1->SetValue(value1);
    literalNode1->SetType(type);
    boundExpressionStack.Push(literalNode1);
    Cm::Sym::Value* value2 = new Cm::Sym::BoolValue(true);
    Cm::BoundTree::BoundLiteral* literalNode2 = new Cm::BoundTree::BoundLiteral(node);
    literalNode2->SetValue(value2);
    literalNode2->SetType(type);
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
                AddClassTypeToIrClassTypeRepository(classTypeSymbol, boundCompileUnit, containerScope);
            }
        }
    }
	if (CurrentFunction()->GetFunctionSymbol()->IsNothrow() && fun->CanThrow())
	{
		if (!CurrentFunction()->GetCurrentTry())
		{
			throw Cm::Core::Exception("a nothrow function can call a function that can throw only from a try block", span, fun->GetSpan());
		}
	}
}

Cm::BoundTree::TraceCallInfo* CreateTraceCallInfo(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::FunctionSymbol* fun, const Cm::Parsing::Span& span)
{
    if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::no_call_stacks)) return nullptr;
    if (Cm::Core::GetGlobalSettings()->Config() == "release" && !fun->CanThrow()) return nullptr;
    if (fun->FullName() == "main()" && Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::unit_test)) return nullptr;
    std::string funFullName = fun->FullName();
    Cm::Sym::TypeSymbol* constCharPtrType = boundCompileUnit.SymbolTable().GetTypeRepository().MakeConstCharPtrType(span);
    int funId = boundCompileUnit.StringRepository().Install(funFullName);
    Cm::BoundTree::BoundStringLiteral* funLiteral = new Cm::BoundTree::BoundStringLiteral(nullptr, funId);
    funLiteral->SetType(constCharPtrType);
    std::string filePath = Cm::Parser::FileRegistry::Instance()->GetParsedFileName(span.FileIndex());
    int fileId = boundCompileUnit.StringRepository().Install(filePath);
    Cm::BoundTree::BoundStringLiteral* fileLiteral = new Cm::BoundTree::BoundStringLiteral(nullptr, fileId);
    fileLiteral->SetType(constCharPtrType);
    Cm::Sym::TypeSymbol* intType = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId));
    Cm::Sym::Value* value = new Cm::Sym::IntValue(span.LineNumber());
    Cm::BoundTree::BoundLiteral* lineLiteral = new Cm::BoundTree::BoundLiteral(nullptr);
    lineLiteral->SetValue(value);
    lineLiteral->SetType(intType);
    Cm::BoundTree::TraceCallInfo* traceCallInfo = new Cm::BoundTree::TraceCallInfo(funLiteral, fileLiteral, lineLiteral);
    return traceCallInfo;
}

} } // namespace Cm::Bind
