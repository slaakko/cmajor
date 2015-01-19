/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/StatementBinder.hpp>
#include <Cm.Bind/LocalVariable.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/OverloadResolution.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/Function.hpp>
#include <Cm.Bind/Access.hpp>

namespace Cm { namespace Bind {

StatementBinder::StatementBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ConversionTable& conversionTable_, Cm::Core::ClassConversionTable& classConversionTable_, 
    Cm::Core::DerivedTypeOpRepository& derivedTypeOpRepository_, Cm::Core::SynthesizedClassFunRepository& synthesizedClassFunRepository_, 
    Cm::Core::StringRepository& stringRepository_, Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_,
    Cm::BoundTree::BoundFunction* currentFunction_) :
    ExpressionBinder(symbolTable_, conversionTable_, classConversionTable_, derivedTypeOpRepository_, synthesizedClassFunRepository_, stringRepository_, irClassTypeRepository_, containerScope_, fileScope_, currentFunction_), 
    symbolTable(symbolTable_), containerScope(containerScope_), fileScope(fileScope_), result(nullptr)
{
}

ConstructionStatementBinder::ConstructionStatementBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ConversionTable& conversionTable_, Cm::Core::ClassConversionTable& classConversionTable_, 
    Cm::Core::DerivedTypeOpRepository& derivedTypeOpRepository_, Cm::Core::SynthesizedClassFunRepository& synthesizedClassFunRepository_, 
    Cm::Core::StringRepository& stringRepository_, Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_,
    Cm::BoundTree::BoundFunction* currentFunction_) :
    StatementBinder(symbolTable_, conversionTable_, classConversionTable_, derivedTypeOpRepository_, synthesizedClassFunRepository_, stringRepository_, irClassTypeRepository_, containerScope_, 
    fileScope_, currentFunction_), constructionStatement(nullptr)
{
}

void ConstructionStatementBinder::BeginVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
    constructionStatement = new Cm::BoundTree::BoundConstructionStatement(&constructionStatementNode);
    Cm::Sym::LocalVariableSymbol* localVariable = BindLocalVariable(SymbolTable(), ContainerScope(), FileScope(), &constructionStatementNode);
    constructionStatement->SetLocalVariable(localVariable);
    CurrentFunction()->AddLocalVariable(localVariable);
}

void ConstructionStatementBinder::EndVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
    constructionStatement->SetArguments(GetExpressions());
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Sym::TypeSymbol* localVariableType = constructionStatement->LocalVariable()->GetType();
    Cm::Core::Argument variableArgument(Cm::Core::ArgumentCategory::lvalue, SymbolTable().GetTypeRepository().MakePointerType(localVariableType, constructionStatementNode.GetSpan()));
    resolutionArguments.push_back(variableArgument);
    constructionStatement->GetResolutionArguments(resolutionArguments);
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, constructionStatement->LocalVariable()->GetType()->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Core::ConversionType conversionType = Cm::Core::ConversionType::implicit;
    Cm::Sym::FunctionSymbol* ctor = ResolveOverload(SymbolTable(), ConversionTable(), ClassConversionTable(), DerivedTypeOpRepository(), SynthesizedClassFunRepository(), "@constructor", resolutionArguments, 
        functionLookups, constructionStatementNode.GetSpan(), conversions, conversionType, OverloadResolutionFlags::none);
    PrepareFunctionSymbol(ctor, constructionStatementNode.GetSpan());
    constructionStatement->SetConstructor(ctor);
    constructionStatement->InsertLocalVariableToArguments();
    constructionStatement->Arguments()[0]->SetFlag(Cm::BoundTree::BoundNodeFlags::constructVariable);
    PrepareFunctionArguments(ctor, constructionStatement->Arguments(), true, IrClassTypeRepository());
    constructionStatement->ApplyConversions(conversions);
    SetResult(constructionStatement);
}

AssignmentStatementBinder::AssignmentStatementBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ConversionTable& conversionTable_, Cm::Core::ClassConversionTable& classConversionTable_,
    Cm::Core::DerivedTypeOpRepository& derivedTypeOpRepository_, Cm::Core::SynthesizedClassFunRepository& synthesizedClassFunRepository_, Cm::Core::StringRepository& stringRepository_, 
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, Cm::BoundTree::BoundFunction* currentFunction_) :
    StatementBinder(symbolTable_, conversionTable_, classConversionTable_, derivedTypeOpRepository_, synthesizedClassFunRepository_, stringRepository_, irClassTypeRepository_, containerScope_, fileScope_, 
    currentFunction_)
{
}

void AssignmentStatementBinder::EndVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode)
{
    Cm::BoundTree::BoundExpression* right = Pop();
    Cm::BoundTree::BoundExpression* left = Pop();
    left->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Core::Argument leftArgument(Cm::Core::ArgumentCategory::lvalue, SymbolTable().GetTypeRepository().MakePointerType(left->GetType(), assignmentStatementNode.GetSpan()));
    resolutionArguments.push_back(leftArgument);
    Cm::Core::Argument rightArgument = Cm::Core::Argument(right->GetArgumentCategory(), right->GetType());
/*
    if (right->IsBoundToTemporary()) // todo
    {
        rightArgument.SetBindToRvalueRef(); 
    }
*/
    resolutionArguments.push_back(rightArgument);
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, left->GetType()->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* assignment = ResolveOverload(SymbolTable(), ConversionTable(), ClassConversionTable(), DerivedTypeOpRepository(), SynthesizedClassFunRepository(), "operator=", 
        resolutionArguments, functionLookups, assignmentStatementNode.GetSpan(), conversions);
    PrepareFunctionSymbol(assignment, assignmentStatementNode.GetSpan());
    if (conversions.size() != 2)
    {
        throw std::runtime_error("wrong number of conversions");
    }
    Cm::Sym::FunctionSymbol* leftConversion = conversions[0];
    if (leftConversion)
    {
        left = new Cm::BoundTree::BoundConversion(&assignmentStatementNode, left, leftConversion);
        left->SetType(leftConversion->GetTargetType());
    }
    Cm::Sym::FunctionSymbol* rightConversion = conversions[1];
    if (rightConversion)
    {
        right = new Cm::BoundTree::BoundConversion(&assignmentStatementNode, right, rightConversion);
        right->SetType(rightConversion->GetTargetType());
    }
    Cm::BoundTree::BoundAssignmentStatement* assignmentStatement = new Cm::BoundTree::BoundAssignmentStatement(&assignmentStatementNode, left, right, assignment);
    SetResult(assignmentStatement);
}

SimpleStatementBinder::SimpleStatementBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ConversionTable& conversionTable_, Cm::Core::ClassConversionTable& classConversionTable_,
    Cm::Core::DerivedTypeOpRepository& derivedTypeOpRepository_, Cm::Core::SynthesizedClassFunRepository& synthesizedClassFunRepository_, Cm::Core::StringRepository& stringRepository_, 
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_,
    Cm::BoundTree::BoundFunction* currentFunction_) :
    StatementBinder(symbolTable_, conversionTable_, classConversionTable_, derivedTypeOpRepository_, synthesizedClassFunRepository_, stringRepository_, irClassTypeRepository_, containerScope_, fileScope_, 
    currentFunction_)
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

ReturnStatementBinder::ReturnStatementBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ConversionTable& conversionTable_, Cm::Core::ClassConversionTable& classConversionTable_,
    Cm::Core::DerivedTypeOpRepository& derivedTypeOpRepository_, Cm::Core::SynthesizedClassFunRepository& synthesizedClassFunRepository_, Cm::Core::StringRepository& stringRepository_, 
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, Cm::BoundTree::BoundFunction* currentFunction_) :
    StatementBinder(symbolTable_, conversionTable_, classConversionTable_, derivedTypeOpRepository_, synthesizedClassFunRepository_, stringRepository_, irClassTypeRepository_, containerScope_, fileScope_, 
    currentFunction_)
{
}

void ReturnStatementBinder::EndVisit(Cm::Ast::ReturnStatementNode& returnStatementNode)
{
    Cm::BoundTree::BoundReturnStatement* returnStatement = new Cm::BoundTree::BoundReturnStatement(&returnStatementNode);
    Cm::Ast::FunctionNode* functionNode = returnStatementNode.GetFunction();
    Cm::Ast::Node* returnTypeExpr = functionNode->ReturnTypeExpr();
    if (returnTypeExpr)
    {
        Cm::Sym::TypeSymbol* returnType = ResolveType(SymbolTable(), ContainerScope(), FileScope(), returnTypeExpr);
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
                }
                else
                {
                    Cm::Core::Argument sourceArgument = Cm::Core::Argument(returnValue->GetArgumentCategory(), SymbolTable().GetTypeRepository().MakeConstReferenceType(returnValue->GetType(),
                        returnStatementNode.GetSpan()));
                    resolutionArguments.push_back(sourceArgument);
                }
                // todo: bind to rvalue
                Cm::Sym::FunctionLookupSet functionLookups;
                functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, returnType->GetContainerScope()->ClassOrNsScope()));
                std::vector<Cm::Sym::FunctionSymbol*> conversions;
                Cm::Core::ConversionType conversionType = Cm::Core::ConversionType::implicit;
                if (returnValue->IsCast())
                {
                    conversionType = Cm::Core::ConversionType::explicit_;
                }
                Cm::Sym::FunctionSymbol* ctor = ResolveOverload(SymbolTable(), ConversionTable(), ClassConversionTable(), DerivedTypeOpRepository(), SynthesizedClassFunRepository(), "@constructor", 
                    resolutionArguments, functionLookups, returnStatementNode.GetSpan(), conversions, conversionType, OverloadResolutionFlags::none);
                PrepareFunctionSymbol(ctor, returnStatementNode.GetSpan());
                returnStatement->SetConstructor(ctor);
                if (conversions.size() != 2)
                {
                    throw std::runtime_error("wrong number of conversions");
                }
                Cm::Sym::FunctionSymbol* conversionFun = conversions[1];
                if (conversionFun)
                {
                    returnValue = new Cm::BoundTree::BoundConversion(&returnStatementNode, returnValue, conversionFun);
                    returnValue->SetType(conversionFun->GetTargetType());
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

ConditionalStatementBinder::ConditionalStatementBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ConversionTable& conversionTable_, Cm::Core::ClassConversionTable& classConversionTable_,
    Cm::Core::DerivedTypeOpRepository& derivedTypeOpRepository_, Cm::Core::SynthesizedClassFunRepository& synthesizedClassFunRepository_, Cm::Core::StringRepository& stringRepository_,
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, Cm::BoundTree::BoundFunction* currentFunction_,
    Cm::BoundTree::BoundConditionalStatement* conditionalStatement_) :
    StatementBinder(symbolTable_, conversionTable_, classConversionTable_, derivedTypeOpRepository_, synthesizedClassFunRepository_, stringRepository_, irClassTypeRepository_, containerScope_, fileScope_, 
    currentFunction_), conditionalStatement(conditionalStatement_)
{
    PushSkipContent();
}

void ConditionalStatementBinder::EndVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode)
{
    PopSkipContent();
    Cm::BoundTree::BoundExpression* condition = Pop();
    condition->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
    conditionalStatement->SetCondition(condition);
}

WhileStatementBinder::WhileStatementBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ConversionTable& conversionTable_, Cm::Core::ClassConversionTable& classConversionTable_,
    Cm::Core::DerivedTypeOpRepository& derivedTypeOpRepository_, Cm::Core::SynthesizedClassFunRepository& synthesizedClassFunRepository_, Cm::Core::StringRepository& stringRepository_, 
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, Cm::BoundTree::BoundFunction* currentFunction_, 
    Cm::BoundTree::BoundWhileStatement* whileStatement_) :
    StatementBinder(symbolTable_, conversionTable_, classConversionTable_, derivedTypeOpRepository_, synthesizedClassFunRepository_, stringRepository_, irClassTypeRepository_, containerScope_, fileScope_, 
    currentFunction_), whileStatement(whileStatement_)
{
    PushSkipContent();
}

void WhileStatementBinder::EndVisit(Cm::Ast::WhileStatementNode& whileStatementNode) 
{
    PopSkipContent();
    Cm::BoundTree::BoundExpression* condition = Pop();
    condition->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
    whileStatement->SetCondition(condition);
}

DoStatementBinder::DoStatementBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ConversionTable& conversionTable_, Cm::Core::ClassConversionTable& classConversionTable_,
    Cm::Core::DerivedTypeOpRepository& derivedTypeOpRepository_, Cm::Core::SynthesizedClassFunRepository& synthesizedClassFunRepository_, Cm::Core::StringRepository& stringRepository_, 
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, Cm::BoundTree::BoundFunction* currentFunction_, 
    Cm::BoundTree::BoundDoStatement* doStatement_) :
    StatementBinder(symbolTable_, conversionTable_, classConversionTable_, derivedTypeOpRepository_, synthesizedClassFunRepository_, stringRepository_, irClassTypeRepository_, containerScope_, fileScope_, 
    currentFunction_), doStatement(doStatement_)
{
    PushSkipContent();
}

void DoStatementBinder::EndVisit(Cm::Ast::DoStatementNode& doStatementNode)
{
    PopSkipContent();
    Cm::BoundTree::BoundExpression* condition = Pop();
    condition->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
    doStatement->SetCondition(condition);
}

ForStatementBinder::ForStatementBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ConversionTable& conversionTable_, Cm::Core::ClassConversionTable& classConversionTable_,
    Cm::Core::DerivedTypeOpRepository& derivedTypeOpRepository_, Cm::Core::SynthesizedClassFunRepository& synthesizedClassFunRepository_, Cm::Core::StringRepository& stringRepository_, 
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, Cm::BoundTree::BoundFunction* currentFunction_, 
    Cm::BoundTree::BoundForStatement* forStatement_) :
    StatementBinder(symbolTable_, conversionTable_, classConversionTable_, derivedTypeOpRepository_, synthesizedClassFunRepository_, stringRepository_, irClassTypeRepository_, containerScope_, fileScope_, 
    currentFunction_), forStatement(forStatement_)
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
    condition->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
    forStatement->SetCondition(condition);
}

} } // namespace Cm::Bind
