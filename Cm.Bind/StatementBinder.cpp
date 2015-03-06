/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/StatementBinder.hpp>
#include <Cm.Bind/LocalVariable.hpp>
#include <Cm.Bind/OverloadResolution.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/Evaluator.hpp>
#include <Cm.Bind/Function.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Bind/Binder.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Sym/DeclarationVisitor.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Expression.hpp>

namespace Cm { namespace Bind {

StatementBinder::StatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_) :
    ExpressionBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), symbolTable(boundCompileUnit_.SymbolTable()), containerScope(containerScope_), fileScopes(fileScopes_), 
    result(nullptr)
{
}

ConstructionStatementBinder::ConstructionStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_) : 
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), constructionStatement(nullptr)
{
}

void ConstructionStatementBinder::BeginVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
    constructionStatement = new Cm::BoundTree::BoundConstructionStatement(&constructionStatementNode);
    Cm::Sym::LocalVariableSymbol* localVariable = BindLocalVariable(SymbolTable(), ContainerScope(), FileScopes(), BoundCompileUnit().ClassTemplateRepository(), &constructionStatementNode);
    constructionStatement->SetLocalVariable(localVariable);
    CurrentFunction()->AddLocalVariable(localVariable);
}

void ConstructionStatementBinder::EndVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
    if (constructionStatement->LocalVariable()->Used())
    { 
        throw Cm::Core::Exception("local variable '" + constructionStatement->LocalVariable()->Name() + "' is used before it is defined", constructionStatement->LocalVariable()->GetUseSpan(), 
            constructionStatementNode.GetSpan());
    }
    constructionStatement->SetArguments(GetExpressions());
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Sym::TypeSymbol* localVariableType = constructionStatement->LocalVariable()->GetType();
    if (localVariableType->IsAbstract())
    {
        throw Cm::Core::Exception("cannot instantiate an abstract class", constructionStatementNode.GetSpan());
    }
    Cm::Core::Argument variableArgument(Cm::Core::ArgumentCategory::lvalue, SymbolTable().GetTypeRepository().MakePointerType(localVariableType, constructionStatementNode.GetSpan()));
    resolutionArguments.push_back(variableArgument);
    constructionStatement->GetResolutionArguments(localVariableType, resolutionArguments);
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, constructionStatement->LocalVariable()->GetType()->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::ConversionType conversionType = Cm::Sym::ConversionType::implicit;
    Cm::Sym::FunctionSymbol* ctor = ResolveOverload(ContainerScope(), BoundCompileUnit(), "@constructor", resolutionArguments, functionLookups, constructionStatementNode.GetSpan(), conversions, 
        conversionType,  OverloadResolutionFlags::none);
    PrepareFunctionSymbol(ctor, constructionStatementNode.GetSpan());
    constructionStatement->SetConstructor(ctor);
    constructionStatement->InsertLocalVariableToArguments();
    constructionStatement->Arguments()[0]->SetFlag(Cm::BoundTree::BoundNodeFlags::constructVariable);
    constructionStatement->ApplyConversions(conversions, CurrentFunction());
    PrepareFunctionArguments(ctor, ContainerScope(), BoundCompileUnit(), CurrentFunction(), constructionStatement->Arguments(), true, BoundCompileUnit().IrClassTypeRepository());
    if (localVariableType->IsReferenceType())
    {
        constructionStatement->Arguments()[1]->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
    }
    SetResult(constructionStatement);
}

AssignmentStatementBinder::AssignmentStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_) : 
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_)
{
}

void AssignmentStatementBinder::EndVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode)
{
    Cm::BoundTree::BoundExpression* right = Pop();
    Cm::BoundTree::BoundExpression* left = Pop();
    left->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Sym::TypeSymbol* leftPlainType = SymbolTable().GetTypeRepository().MakePlainType(left->GetType());
    Cm::Core::Argument leftArgument(Cm::Core::ArgumentCategory::lvalue, SymbolTable().GetTypeRepository().MakePointerType(leftPlainType, assignmentStatementNode.GetSpan()));
    resolutionArguments.push_back(leftArgument);
    if (leftPlainType->IsPointerType() && right->IsBoundNullLiteral())
    {
        right->SetType(leftPlainType);
    }
    Cm::Core::Argument rightArgument = Cm::Core::Argument(right->GetArgumentCategory(), right->GetType());
    if (right->GetFlag(Cm::BoundTree::BoundNodeFlags::argIsTemporary))
    {
        rightArgument.SetBindToRvalueRef();
    }
    resolutionArguments.push_back(rightArgument);
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, leftPlainType->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* assignment = ResolveOverload(ContainerScope(), BoundCompileUnit(), "operator=", resolutionArguments, functionLookups, assignmentStatementNode.GetSpan(), conversions);
    PrepareFunctionSymbol(assignment, assignmentStatementNode.GetSpan());
    if (conversions.size() != 2)
    {
        throw std::runtime_error("wrong number of conversions");
    }
    Cm::Sym::FunctionSymbol* leftConversion = conversions[0];
    if (leftConversion)
    {
        left = Cm::BoundTree::CreateBoundConversion(&assignmentStatementNode, left, leftConversion, CurrentFunction());
    }
    Cm::Sym::FunctionSymbol* rightConversion = conversions[1];
    if (rightConversion)
    {
        right = Cm::BoundTree::CreateBoundConversion(&assignmentStatementNode, right, rightConversion, CurrentFunction());
    }
    if (!assignment->Parameters()[1]->GetType()->IsReferenceType() && right->GetType()->IsReferenceType())
    {
        right->SetFlag(Cm::BoundTree::BoundNodeFlags::refByValue);
    }
    Cm::BoundTree::BoundAssignmentStatement* assignmentStatement = new Cm::BoundTree::BoundAssignmentStatement(&assignmentStatementNode, left, right, assignment);
    SetResult(assignmentStatement);
}

SimpleStatementBinder::SimpleStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_) : 
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_)
{
}

void SimpleStatementBinder::EndVisit(Cm::Ast::SimpleStatementNode& simpleStatementNode)
{
    Cm::BoundTree::BoundSimpleStatement* simpleStatement = new Cm::BoundTree::BoundSimpleStatement(&simpleStatementNode);
    if (simpleStatementNode.HasExpr())
    {
        Cm::BoundTree::BoundExpression* expression = Pop();
        simpleStatement->SetExpression(expression);
    }
    SetResult(simpleStatement);
}

ReturnStatementBinder::ReturnStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_) : 
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_)
{
}

void ReturnStatementBinder::EndVisit(Cm::Ast::ReturnStatementNode& returnStatementNode)
{
    Cm::BoundTree::BoundReturnStatement* returnStatement = new Cm::BoundTree::BoundReturnStatement(&returnStatementNode);
    Cm::Ast::FunctionNode* functionNode = returnStatementNode.GetFunction();
    Cm::Ast::Node* returnTypeExpr = functionNode->ReturnTypeExpr();
    if (returnTypeExpr)
    {
        Cm::Sym::TypeSymbol* returnType = ResolveType(SymbolTable(), ContainerScope(), FileScopes(), BoundCompileUnit().ClassTemplateRepository(), returnTypeExpr);
        if (!returnType->IsVoidTypeSymbol())
        {
            if (returnStatementNode.ReturnsValue())
            {
                returnStatement->SetReturnType(returnType);
                Cm::BoundTree::BoundExpression* returnValue = Pop();
                std::vector<Cm::Core::Argument> resolutionArguments;
                Cm::Core::Argument targetArgument(Cm::Core::ArgumentCategory::lvalue, SymbolTable().GetTypeRepository().MakePointerType(returnType, returnStatementNode.GetSpan()));
                resolutionArguments.push_back(targetArgument);
                if (returnType->IsNonConstReferenceType() && (returnValue->GetType()->IsNonConstReferenceType() || !returnValue->GetType()->IsReferenceType()))
                {
                    Cm::Core::Argument sourceArgument = Cm::Core::Argument(returnValue->GetArgumentCategory(), SymbolTable().GetTypeRepository().MakeReferenceType(returnValue->GetType(),
                        returnStatementNode.GetSpan()));
                    resolutionArguments.push_back(sourceArgument);
                    if (!returnValue->GetType()->IsReferenceType())
                    {
                        returnValue->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
                    }
                }
                else
                {
                    Cm::Core::Argument sourceArgument = Cm::Core::Argument(returnValue->GetArgumentCategory(), SymbolTable().GetTypeRepository().MakeConstReferenceType(returnValue->GetType(),
                        returnStatementNode.GetSpan()));
                    resolutionArguments.push_back(sourceArgument);
                    if (returnType->IsReferenceType())
                    {
                        if (!returnValue->GetType()->IsReferenceType())
                        {
                            returnValue->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
                        }
                    }
                }
                if (returnValue->GetFlag(Cm::BoundTree::BoundNodeFlags::argIsTemporary) || returnValue->IsBoundLocalVariable())
                {
                    Cm::Core::Argument& sourceArgument = resolutionArguments[1];
                    sourceArgument.SetBindToRvalueRef();
                }
                Cm::Sym::FunctionLookupSet functionLookups;
                functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, returnType->GetContainerScope()->ClassOrNsScope()));
                std::vector<Cm::Sym::FunctionSymbol*> conversions;
                Cm::Sym::ConversionType conversionType = Cm::Sym::ConversionType::implicit;
                if (returnValue->IsCast())
                {
                    conversionType = Cm::Sym::ConversionType::explicit_;
                }
                Cm::Sym::FunctionSymbol* ctor = ResolveOverload(ContainerScope(), BoundCompileUnit(), "@constructor", resolutionArguments, functionLookups, returnStatementNode.GetSpan(), conversions, 
                    conversionType, OverloadResolutionFlags::none);
                PrepareFunctionSymbol(ctor, returnStatementNode.GetSpan());
                returnStatement->SetConstructor(ctor);
                if (conversions.size() != 2)
                {
                    throw std::runtime_error("wrong number of conversions");
                }
                Cm::Sym::FunctionSymbol* conversionFun = conversions[1];
                if (conversionFun)
                {
                    returnValue = Cm::BoundTree::CreateBoundConversion(&returnStatementNode, returnValue, conversionFun, CurrentFunction());
                }
                if (returnValue->GetType()->IsClassTypeSymbol() && returnType->IsReferenceType())
                {
                    returnValue->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
                }
                returnStatement->SetExpression(returnValue);
            }
            else
            {
                throw Cm::Core::Exception("must return a value", returnStatementNode.GetSpan(), functionNode->GetSpan());
            }
        }
        else if (returnStatementNode.ReturnsValue())
        {
            throw Cm::Core::Exception("void function cannot return a value", returnStatementNode.GetSpan(), functionNode->GetSpan());
        }
    }
    else if (returnStatementNode.ReturnsValue())
    {
        throw Cm::Core::Exception("cannot return a value", returnStatementNode.GetSpan(), functionNode->GetSpan());
    }
    SetResult(returnStatement);
}

ConditionalStatementBinder::ConditionalStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundConditionalStatement* conditionalStatement_) :
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), conditionalStatement(conditionalStatement_)
{
    PushSkipContent();
}

void ConditionalStatementBinder::EndVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode)
{
    PopSkipContent();
    Cm::BoundTree::BoundExpression* condition = Pop();
    Cm::Sym::TypeSymbol* condType = SymbolTable().GetTypeRepository().MakePlainType(condition->GetType());
    if (!condType->IsBoolTypeSymbol())
    {
        throw Cm::Core::Exception("if statement condition must be Boolean expression (now of type '" + condType->FullName() + "')", conditionalStatementNode.Condition()->GetSpan());
    }
    condition->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
    conditionalStatement->SetCondition(condition);
}

WhileStatementBinder::WhileStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundWhileStatement* whileStatement_) :
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_,  currentFunction_), whileStatement(whileStatement_)
{
    PushSkipContent();
}

void WhileStatementBinder::EndVisit(Cm::Ast::WhileStatementNode& whileStatementNode) 
{
    PopSkipContent();
    Cm::BoundTree::BoundExpression* condition = Pop();
    Cm::Sym::TypeSymbol* condType = SymbolTable().GetTypeRepository().MakePlainType(condition->GetType());
    if (!condType->IsBoolTypeSymbol())
    {
        throw Cm::Core::Exception("while statement condition must be Boolean expression (now of type '" + condType->FullName() + "')", whileStatementNode.Condition()->GetSpan());
    }
    condition->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
    whileStatement->SetCondition(condition);
}

DoStatementBinder::DoStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundDoStatement* doStatement_) :
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), doStatement(doStatement_)
{
    PushSkipContent();
}

void DoStatementBinder::EndVisit(Cm::Ast::DoStatementNode& doStatementNode)
{
    PopSkipContent();
    Cm::BoundTree::BoundExpression* condition = Pop();
    Cm::Sym::TypeSymbol* condType = SymbolTable().GetTypeRepository().MakePlainType(condition->GetType());
    if (!condType->IsBoolTypeSymbol())
    {
        throw Cm::Core::Exception("do statement condition must be Boolean expression (now of type '" + condType->FullName() + "')", doStatementNode.Condition()->GetSpan());
    }
    condition->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
    doStatement->SetCondition(condition);
}

ForStatementBinder::ForStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundForStatement* forStatement_) :
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), forStatement(forStatement_)
{
    PushSkipContent();
}

void ForStatementBinder::EndVisit(Cm::Ast::ForStatementNode& forStatementNode)
{
    PopSkipContent();
    if (!forStatementNode.HasIncrement())
    {
        GenerateTrueExpression(&forStatementNode);
    }
    Cm::BoundTree::BoundExpression* increment = Pop();
    forStatement->SetIncrement(increment);
    if (!forStatementNode.HasCondition())
    {
        GenerateTrueExpression(&forStatementNode);
    }
    Cm::BoundTree::BoundExpression* condition = Pop();
    Cm::Sym::TypeSymbol* condType = SymbolTable().GetTypeRepository().MakePlainType(condition->GetType());
    if (!condType->IsBoolTypeSymbol())
    {
        throw Cm::Core::Exception("for statement condition must be Boolean expression (now of type '" + condType->FullName() + "')", forStatementNode.Condition()->GetSpan());
    }
    condition->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
    forStatement->SetCondition(condition);
}

RangeForStatementBinder::RangeForStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_,
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_, Cm::Ast::RangeForStatementNode& rangeForStatementNode, Binder& binder_):
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), binder(binder_)
{
    rangeForStatementNode.Container()->Accept(*this);
    std::unique_ptr<Cm::BoundTree::BoundExpression> container(Pop());
    Cm::Sym::TypeSymbol* containerType = container->GetType();
    Cm::Sym::TypeSymbol* plainContainerType = SymbolTable().GetTypeRepository().MakePlainType(containerType);
    Cm::Ast::IdentifierNode* beginNode(new Cm::Ast::IdentifierNode(rangeForStatementNode.GetSpan(), "Begin"));
    Cm::Ast::IdentifierNode* endNode(new Cm::Ast::IdentifierNode(rangeForStatementNode.GetSpan(), "End"));
    Cm::Ast::CloneContext cloneContext;
    Cm::Ast::DotNode* containerBeginNode(new Cm::Ast::DotNode(rangeForStatementNode.GetSpan(), rangeForStatementNode.Container()->Clone(cloneContext), beginNode));
    Cm::Ast::InvokeNode* invokeContainerBeginNode(new Cm::Ast::InvokeNode(rangeForStatementNode.GetSpan(), containerBeginNode));
    Cm::Ast::DotNode* containerEndNode(new Cm::Ast::DotNode(rangeForStatementNode.GetSpan(), rangeForStatementNode.Container()->Clone(cloneContext), endNode));
    Cm::Ast::InvokeNode* invokeContainerEndNode(new Cm::Ast::InvokeNode(rangeForStatementNode.GetSpan(), containerEndNode));
    Cm::Ast::IdentifierNode* iteratorId(new Cm::Ast::IdentifierNode(rangeForStatementNode.GetSpan(), CurrentFunction()->GetNextTempVariableName()));
    Cm::Ast::IdentifierNode* iteratorTypeId = nullptr; 
    if (containerType->IsConstType())
    {
        iteratorTypeId = new Cm::Ast::IdentifierNode(rangeForStatementNode.GetSpan(), "ConstIterator");
    }
    else
    {
        iteratorTypeId = new Cm::Ast::IdentifierNode(rangeForStatementNode.GetSpan(), "Iterator");
    }
    Cm::Ast::IdentifierNode* containerTypeId(new Cm::Ast::IdentifierNode(rangeForStatementNode.GetSpan(), plainContainerType->FullName()));
    Cm::Ast::DotNode* containerIterator(new Cm::Ast::DotNode(rangeForStatementNode.GetSpan(), containerTypeId, iteratorTypeId));
    Cm::Ast::ConstructionStatementNode* initNode(new Cm::Ast::ConstructionStatementNode(rangeForStatementNode.GetSpan(), containerIterator, iteratorId));
    initNode->AddArgument(invokeContainerBeginNode);
    Cm::Ast::NotEqualNode* iteratorNotEqualToEnd(new Cm::Ast::NotEqualNode(rangeForStatementNode.GetSpan(), iteratorId->Clone(cloneContext), invokeContainerEndNode));
    Cm::Ast::PrefixIncNode* incIterator(new Cm::Ast::PrefixIncNode(rangeForStatementNode.GetSpan(), iteratorId->Clone(cloneContext)));
    Cm::Ast::CompoundStatementNode* action(new Cm::Ast::CompoundStatementNode(rangeForStatementNode.GetSpan()));
    Cm::Ast::DerefNode* derefIterator(new Cm::Ast::DerefNode(rangeForStatementNode.GetSpan(), iteratorId->Clone(cloneContext)));
    Cm::Ast::ConstructionStatementNode* initVariable(new Cm::Ast::ConstructionStatementNode(rangeForStatementNode.GetSpan(), rangeForStatementNode.VarTypeExpr()->Clone(cloneContext), 
        static_cast<Cm::Ast::IdentifierNode*>(rangeForStatementNode.VarId()->Clone(cloneContext))));
    initVariable->AddArgument(derefIterator);
    action->AddStatement(initVariable);
    Cm::Sym::DeclarationVisitor declarationVisitor(SymbolTable());
    action->AddStatement(static_cast<Cm::Ast::StatementNode*>(rangeForStatementNode.Action()->Clone(cloneContext)));
    Cm::Ast::ForStatementNode forStatementNode(rangeForStatementNode.GetSpan(), initNode, iteratorNotEqualToEnd, incIterator, action);
    forStatementNode.Accept(declarationVisitor);
    Cm::Sym::ContainerScope* containerScope = SymbolTable().GetContainerScope(&forStatementNode);
    containerScope->SetParent(ContainerScope());
    binder.BeginContainerScope(containerScope);
    forStatementNode.Accept(binder);
    binder.EndContainerScope();
}

SwitchStatementBinder::SwitchStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundSwitchStatement* switchStatement_): 
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), switchStatement(switchStatement_)
{
}

void SwitchStatementBinder::BeginVisit(Cm::Ast::SwitchStatementNode& switchStatementNode)
{
    PushSkipContent();
    Cm::Ast::Node* conditionNode = switchStatementNode.Condition();
    conditionNode->Accept(*this);
    Cm::BoundTree::BoundExpression* condition = Pop();
    Cm::Sym::TypeSymbol* condType = SymbolTable().GetTypeRepository().MakePlainType(condition->GetType());
    if (condType->IsIntegerTypeSymbol() || condType->IsCharTypeSymbol() || condType->IsBoolTypeSymbol() || condType->IsEnumTypeSymbol())
    {
        switchStatement->SetCondition(condition);
    }
    else
    {
        throw Cm::Core::Exception("switch statement condition must be of integer, character, enumerated or Boolean type", conditionNode->GetSpan());
    }
}

void SwitchStatementBinder::EndVisit(Cm::Ast::SwitchStatementNode& switchStatementNode)
{
    PopSkipContent();
}

CaseStatementBinder::CaseStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundCaseStatement* caseStatement_, Cm::BoundTree::BoundSwitchStatement* switchStatement_) : 
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), caseStatement(caseStatement_), switchStatement(switchStatement_)
{
    PushSkipContent();
}

bool TerminatesCase(Cm::Ast::StatementNode* statementNode)
{
    if (statementNode->IsConditionalStatementNode())
    {
        Cm::Ast::ConditionalStatementNode* conditionalStatement = static_cast<Cm::Ast::ConditionalStatementNode*>(statementNode);
        if (conditionalStatement->HasElseStatement())
        {
            if (TerminatesCase(conditionalStatement->ThenS()) && TerminatesCase(conditionalStatement->ElseS()))
            {
                return true;
            }
        }
    }
    else if (statementNode->IsCompoundStatementNode())
    {
        Cm::Ast::CompoundStatementNode* compoundStatement = static_cast<Cm::Ast::CompoundStatementNode*>(statementNode);
        for (const std::unique_ptr<Cm::Ast::StatementNode>& statement : compoundStatement->Statements())
        {
            if (TerminatesCase(statement.get())) return true;
        }
    }
    else
    {
        return statementNode->IsCaseTerminatingNode();
    }
    return false;
}

void CaseStatementBinder::EndVisit(Cm::Ast::CaseStatementNode& caseStatementNode)
{
    PopSkipContent();
    for (const std::unique_ptr<Cm::Ast::Node>& expr : caseStatementNode.Expressions())
    {
        Cm::Sym::TypeSymbol* condType = switchStatement->Condition()->GetType();
        if (condType->IsEnumTypeSymbol())
        {
            Cm::Sym::EnumTypeSymbol* enumTypeSymbol = static_cast<Cm::Sym::EnumTypeSymbol*>(condType);
            condType = enumTypeSymbol->GetUnderlyingType();
        }
        Cm::Sym::SymbolType symbolType = condType->GetSymbolType();
        Cm::Sym::ValueType valueType = Cm::Sym::GetValueTypeFor(symbolType);
        Cm::Sym::Value* value = Evaluate(valueType, false, expr.get(), SymbolTable(), ContainerScope(), FileScopes(), BoundCompileUnit().ClassTemplateRepository());
        caseStatement->AddValue(value);
    }
    for (const std::unique_ptr<Cm::Ast::StatementNode>& statement : caseStatementNode.Statements())
    {
        if (TerminatesCase(statement.get())) return;
    }
    throw Cm::Core::Exception("case must end in break, continue, return, throw, goto, goto case or goto default statement", caseStatementNode.GetSpan());
}

DefaultStatementBinder::DefaultStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundDefaultStatement* defaultStatement_) : StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_),
    defaultStatement(defaultStatement_)
{
}

void DefaultStatementBinder::EndVisit(Cm::Ast::DefaultStatementNode& defaultStatementNode)
{
    for (const std::unique_ptr<Cm::Ast::StatementNode>& statement : defaultStatementNode.Statements())
    {
        if (TerminatesCase(statement.get())) return;
    }
    throw Cm::Core::Exception("default case must end in break, continue, return, throw goto, goto case or goto default statement", defaultStatementNode.GetSpan());
}

BreakStatementBinder::BreakStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_) : StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_)
{
}

void BreakStatementBinder::Visit(Cm::Ast::BreakStatementNode& breakStatementNode)
{
    Cm::Ast::Node* parent = breakStatementNode.Parent();
    while (parent && !parent->IsBreakEnclosingStatementNode())
    {
        parent = parent->Parent();
    }
    if (!parent)
    {
        throw Cm::Core::Exception("break statement must be enclosed in while, do, for or switch statement", breakStatementNode.GetSpan());
    }
    Cm::BoundTree::BoundBreakStatement* boundBreakStatement = new Cm::BoundTree::BoundBreakStatement(&breakStatementNode);
    SetResult(boundBreakStatement);
}

ContinueStatementBinder::ContinueStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_) : 
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_)
{
}

void ContinueStatementBinder::Visit(Cm::Ast::ContinueStatementNode& continueStatementNode)
{
    Cm::Ast::Node* parent = continueStatementNode.Parent();
    while (parent && !parent->IsContinueEnclosingStatementNode())
    {
        parent = parent->Parent();
    }
    if (!parent)
    {
        throw Cm::Core::Exception("continue statement must be enclosed in while, do, or for statement", continueStatementNode.GetSpan());
    }
    Cm::BoundTree::BoundContinueStatement* boundContinueStatement = new Cm::BoundTree::BoundContinueStatement(&continueStatementNode);
    SetResult(boundContinueStatement);
}

GotoCaseStatementBinder::GotoCaseStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundSwitchStatement* switchStatement_) :
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), switchStatement(switchStatement_)
{
}

void GotoCaseStatementBinder::EndVisit(Cm::Ast::GotoCaseStatementNode& gotoCaseStatementNode)
{
    Cm::Ast::Node* parent = gotoCaseStatementNode.Parent();
    while (parent && !parent->IsCaseStatementNode() && !parent->IsDefaultStatementNode())
    {
        parent = parent->Parent();
    }
    if (!parent)
    {
        throw Cm::Core::Exception("goto case statement must be enclosed in case or default statement", gotoCaseStatementNode.GetSpan());
    }
    Cm::Sym::TypeSymbol* condType = switchStatement->Condition()->GetType();
    if (condType->IsEnumTypeSymbol())
    {
        Cm::Sym::EnumTypeSymbol* enumTypeSymbol = static_cast<Cm::Sym::EnumTypeSymbol*>(condType);
        condType = enumTypeSymbol->GetUnderlyingType();
    }
    Cm::Sym::SymbolType symbolType = condType->GetSymbolType();
    Cm::Sym::ValueType valueType = Cm::Sym::GetValueTypeFor(symbolType);
    Cm::Sym::Value* value = Evaluate(valueType, false, gotoCaseStatementNode.TargetCaseExpr(), SymbolTable(), ContainerScope(), FileScopes(), BoundCompileUnit().ClassTemplateRepository());
    Cm::BoundTree::BoundGotoCaseStatement* boundGotoCasetatement = new Cm::BoundTree::BoundGotoCaseStatement(&gotoCaseStatementNode);
    boundGotoCasetatement->SetValue(value);
    SetResult(boundGotoCasetatement);
}

GotoDefaultStatementBinder::GotoDefaultStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_) : 
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_)
{
}

void GotoDefaultStatementBinder::Visit(Cm::Ast::GotoDefaultStatementNode& gotoDefaultStatementNode)
{
    Cm::Ast::Node* parent = gotoDefaultStatementNode.Parent();
    while (parent && !parent->IsCaseStatementNode())
    {
        parent = parent->Parent();
    }
    if (!parent)
    {
        throw Cm::Core::Exception("goto default statement must be enclosed in case statement", gotoDefaultStatementNode.GetSpan());
    }
    Cm::BoundTree::BoundGotoDefaultStatement* boundGotoDefaultStatement = new Cm::BoundTree::BoundGotoDefaultStatement(&gotoDefaultStatementNode);
    SetResult(boundGotoDefaultStatement);
}

DestroyStatementBinder::DestroyStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_,
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_) : StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_)
{
}

void DestroyStatementBinder::EndVisit(Cm::Ast::DestroyStatementNode& destroyStatementNode)
{
    std::unique_ptr<Cm::BoundTree::BoundExpression> ptr(Pop());
    Cm::Sym::TypeSymbol* type = ptr->GetType();
    if (!type->IsPointerType())
    {
        throw Cm::Core::Exception("destroy statement needs pointer type operand", destroyStatementNode.GetSpan());
    }
    if (type->GetPointerCount() == 1)
    {
        Cm::Sym::TypeSymbol* baseType = type->GetBaseType();
        if (baseType->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* classType = static_cast<Cm::Sym::ClassTypeSymbol*>(baseType);
            Cm::Sym::FunctionSymbol* destructor = classType->Destructor();
            if (destructor)
            {
                Cm::BoundTree::BoundExpressionList arguments;
                arguments.Add(ptr.release());
                Cm::BoundTree::BoundFunctionCallStatement* destructionStatement = new Cm::BoundTree::BoundFunctionCallStatement(destructor, std::move(arguments));
                SetResult(destructionStatement);
                return;
            }
        }
    }
    SetResult(nullptr);
}

DeleteStatementBinder::DeleteStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_,
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_) : StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_)
{
}

void DeleteStatementBinder::EndVisit(Cm::Ast::DeleteStatementNode& deleteStatementNode)
{
    bool resultSet = false;
    std::unique_ptr<Cm::BoundTree::BoundExpression> ptr(Pop());
    Cm::Sym::TypeSymbol* type = ptr->GetType();
    if (!type->IsPointerType())
    {
        throw Cm::Core::Exception("delete statement needs pointer type operand", deleteStatementNode.GetSpan());
    }
    if (type->GetPointerCount() == 1)
    {
        Cm::Sym::TypeSymbol* baseType = type->GetBaseType();
        if (baseType->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* classType = static_cast<Cm::Sym::ClassTypeSymbol*>(baseType);
            Cm::Sym::FunctionSymbol* destructor = classType->Destructor();
            if (destructor)
            {
                Cm::BoundTree::BoundExpressionList arguments;
                arguments.Add(ptr.release());
                Cm::BoundTree::BoundFunctionCallStatement* destructionStatement = new Cm::BoundTree::BoundFunctionCallStatement(destructor, std::move(arguments));
                SetResult(destructionStatement);
                resultSet = true;
            }
        }
    }
    if (!resultSet)
    {
        SetResult(nullptr);
    }
    deleteStatementNode.PointerExpr()->Accept(*this);
    std::unique_ptr<Cm::BoundTree::BoundExpression> mem(Pop());
    std::vector<Cm::Core::Argument> resolutionArguments;
    resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, mem->GetType()));
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_parent, ContainerScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* memFreeFun = ResolveOverload(ContainerScope(), BoundCompileUnit(), "System.Support.MemFree", resolutionArguments, functionLookups, deleteStatementNode.GetSpan(), conversions);
    if (conversions.size() != 1)
    {
        throw std::runtime_error("wrong number of conversions");
    }
    Cm::BoundTree::BoundExpressionList arguments;
    if (conversions[0])
    {
        arguments.Add(CreateBoundConversion(&deleteStatementNode, mem.release(), conversions[0], CurrentFunction()));
    }
    else
    {
        arguments.Add(mem.release());
    }
    freeStatement = new Cm::BoundTree::BoundFunctionCallStatement(memFreeFun, std::move(arguments));
}

} } // namespace Cm::Bind
