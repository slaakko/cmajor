/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Parameter.hpp>
#include <Cm.Bind/Exception.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/OverloadResolution.hpp>
#include <Cm.BoundTree/BoundStatement.hpp>
#include <Cm.Sym/ParameterSymbol.hpp>
#include <Cm.Ast/Parameter.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Bind {

void BindParameter(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::ParameterNode* parameterNode, int parameterIndex)
{
    Cm::Sym::Symbol* symbol = nullptr;
    if (parameterNode->Id())
    {
        symbol = containerScope->Lookup(parameterNode->Id()->Str());
    }
    else
    {
        symbol = containerScope->Lookup("__parameter" + std::to_string(parameterIndex));
    }
    if (symbol)
    {
        if (symbol->IsParameterSymbol())
        {
            Cm::Sym::ParameterSymbol* parameterSymbol = static_cast<Cm::Sym::ParameterSymbol*>(symbol);
            if (parameterSymbol->Bound())
            {
                return;
            }
            Cm::Sym::TypeSymbol* parameterType = ResolveType(symbolTable, containerScope, fileScope, parameterNode->TypeExpr());
            parameterSymbol->SetType(parameterType);
            parameterSymbol->SetBound();
        }
        else
        {
            throw Exception("symbol '" + symbol->FullName() + "' does not denote a parameter", symbol->GetSpan());
        }
    }
    else
    {
        throw Exception("symbol '" + parameterNode->Id()->Str() + "' not found");
    }
}

void GenerateReceives(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ConversionTable& conversionTable, Cm::Core::ClassConversionTable& classConversionTable, Cm::Core::DerivedTypeOpRepository& derivedTypeOpRepository, 
    Cm::BoundTree::BoundFunction* boundFunction)
{
    Cm::Sym::FunctionSymbol* functionSymbol = boundFunction->GetFunctionSymbol();
    if (functionSymbol->IsExternal()) return;
    int index = 0;
    for (Cm::Sym::ParameterSymbol* parameterSymbol : functionSymbol->Parameters())
    {
        Cm::BoundTree::BoundReceiveStatement* boundReceiveStatement = new Cm::BoundTree::BoundReceiveStatement(parameterSymbol);
        Cm::Sym::TypeSymbol* parameterType = parameterSymbol->GetType();
        std::vector<Cm::Core::Argument> resolutionArguments;
        Cm::Core::Argument targetArgument(Cm::Core::ArgumentCategory::lvalue, symbolTable.GetTypeRepository().MakePointerType(parameterType, parameterSymbol->GetSpan()));
        resolutionArguments.push_back(targetArgument);
        Cm::Core::Argument sourceArgument = Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, symbolTable.GetTypeRepository().MakeConstReferenceType(parameterType, parameterSymbol->GetSpan()));
        resolutionArguments.push_back(sourceArgument);
        Cm::Sym::FunctionLookupSet functionLookups;
        functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, parameterType->GetContainerScope()->ClassOrNsScope()));
        std::vector<Cm::Sym::FunctionSymbol*> conversions;
        Cm::Sym::FunctionSymbol* ctor = ResolveOverload(symbolTable, conversionTable, classConversionTable, derivedTypeOpRepository, "@constructor", resolutionArguments, functionLookups, 
            parameterSymbol->GetSpan(), conversions);
        boundReceiveStatement->SetConstructor(ctor);
        boundFunction->Body()->InsertStatement(index, boundReceiveStatement);
        ++index;
    }
}

} } // namespace Cm::Bind
