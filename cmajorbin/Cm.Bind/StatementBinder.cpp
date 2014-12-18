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
    Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, Cm::Sym::FunctionSymbol* currentFunction_) :
    ExpressionBinder(symbolTable_, conversionTable_, classConversionTable_, containerScope_, fileScope_, currentFunction_), 
    symbolTable(symbolTable_), containerScope(containerScope_), fileScope(fileScope_), result(nullptr)
{
}

ConstructionStatementBinder::ConstructionStatementBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ConversionTable& conversionTable_, Cm::Core::ClassConversionTable& classConversionTable_, 
    Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, Cm::Sym::FunctionSymbol* currentFunction_) :
    StatementBinder(symbolTable_, conversionTable_, classConversionTable_, containerScope_, fileScope_, currentFunction_), constructionStatement(nullptr)
{
}

void ConstructionStatementBinder::BeginVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
    constructionStatement = new Cm::BoundTree::BoundConstructionStatement(&constructionStatementNode);
    constructionStatement->SetLocalVariable(BindLocalVariable(SymbolTable(), ContainerScope(), FileScope(), &constructionStatementNode));
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
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base, constructionStatement->LocalVariable()->GetType()->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* ctor = ResolveOverload(SymbolTable(), ConversionTable(), ClassConversionTable(), "@constructor", resolutionArguments, functionLookups, constructionStatementNode.GetSpan(), conversions);
    constructionStatement->SetConstructor(ctor);
    constructionStatement->InsertLocalVariableToArguments();
    constructionStatement->ApplyConversions(conversions);
    SetResult(constructionStatement);
}

} } // namespace Cm::Bind