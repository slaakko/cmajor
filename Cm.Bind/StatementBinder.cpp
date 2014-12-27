/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/StatementBinder.hpp>
#include <Cm.Bind/LocalVariable.hpp>
#include <Cm.Bind/Exception.hpp>
#include <Cm.Bind/OverloadResolution.hpp>

namespace Cm { namespace Bind {

StatementBinder::StatementBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ConversionTable& conversionTable_, Cm::Core::ClassConversionTable& classConversionTable_, 
    Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, Cm::BoundTree::BoundFunction* currentFunction_) :
    ExpressionBinder(symbolTable_, conversionTable_, classConversionTable_, containerScope_, fileScope_, currentFunction_), 
    symbolTable(symbolTable_), containerScope(containerScope_), fileScope(fileScope_), result(nullptr)
{
}

ConstructionStatementBinder::ConstructionStatementBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ConversionTable& conversionTable_, Cm::Core::ClassConversionTable& classConversionTable_, 
    Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, Cm::BoundTree::BoundFunction* currentFunction_) :
    StatementBinder(symbolTable_, conversionTable_, classConversionTable_, containerScope_, fileScope_, currentFunction_), constructionStatement(nullptr)
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
    Cm::Core::Argument variableArgument(Cm::Core::ArgumentCategory::lvalue, SymbolTable().GetTypeRepository().MakePointerType(localVariableType, constructionStatementNode.GetSpan(), false));
    resolutionArguments.push_back(variableArgument);
    constructionStatement->GetResolutionArguments(resolutionArguments);
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, constructionStatement->LocalVariable()->GetType()->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Core::ConversionType conversionType = Cm::Core::ConversionType::implicit;
    // todo explicit!
    Cm::Sym::FunctionSymbol* ctor = ResolveOverload(SymbolTable(), ConversionTable(), ClassConversionTable(), "@constructor", resolutionArguments, functionLookups, constructionStatementNode.GetSpan(), conversionType, conversions);
    constructionStatement->SetConstructor(ctor);
    constructionStatement->InsertLocalVariableToArguments();
    constructionStatement->Arguments()[0]->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
    constructionStatement->ApplyConversions(conversions);
    SetResult(constructionStatement);
}

AssignmentStatementBinder::AssignmentStatementBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ConversionTable& conversionTable_, Cm::Core::ClassConversionTable& classConversionTable_,
    Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, Cm::BoundTree::BoundFunction* currentFunction_) :
    StatementBinder(symbolTable_, conversionTable_, classConversionTable_, containerScope_, fileScope_, currentFunction_)
{
}

void AssignmentStatementBinder::BeginVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode)
{
}

void AssignmentStatementBinder::EndVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode)
{
    Cm::BoundTree::BoundExpression* right = Pop();
    Cm::BoundTree::BoundExpression* left = Pop();
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Core::Argument leftArgument(Cm::Core::ArgumentCategory::lvalue, SymbolTable().GetTypeRepository().MakePointerType(left->GetType(), assignmentStatementNode.GetSpan(), false));
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
    Cm::Sym::FunctionSymbol* assignment = ResolveOverload(SymbolTable(), ConversionTable(), ClassConversionTable(), "operator=", resolutionArguments, functionLookups, assignmentStatementNode.GetSpan(), conversions);
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

} } // namespace Cm::Bind