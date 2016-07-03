/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Parameter.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/OverloadResolution.hpp>
#include <Cm.BoundTree/BoundStatement.hpp>
#include <Cm.Sym/ParameterSymbol.hpp>
#include <Cm.Ast/Parameter.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Bind {

void BindParameter(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Ast::ParameterNode* parameterNode, int parameterIndex)
{
    Cm::Sym::Symbol* symbol = nullptr;
    if (parameterNode->Id())
    {
        symbol = containerScope->Lookup(parameterNode->Id()->Str(), Cm::Sym::SymbolTypeSetId::lookupParameter);
    }
    else
    {
        symbol = containerScope->Lookup("__parameter" + std::to_string(parameterIndex), Cm::Sym::SymbolTypeSetId::lookupParameter);
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
            Cm::Sym::TypeSymbol* parameterType = ResolveType(symbolTable, containerScope, fileScopes, classTemplateRepository, boundCompileUnit, parameterNode->TypeExpr());
            parameterSymbol->SetType(parameterType);
            parameterSymbol->SetBound();
        }
        else
        {
            throw Cm::Core::Exception("symbol '" + symbol->FullName() + "' does not denote a parameter", symbol->GetSpan());
        }
    }
    else
    {
        if (parameterNode->Id())
        {
            throw Cm::Core::Exception("parameter symbol '" + parameterNode->Id()->Str() + "' not found", parameterNode->GetSpan());
        }
        else
        {
            throw Cm::Core::Exception("parameter symbol '__parameter" + std::to_string(parameterIndex) + "' not found", parameterNode->GetSpan());
        }
    }
}

void GenerateReceives(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::BoundTree::BoundFunction* boundFunction)
{
    Cm::Sym::FunctionSymbol* functionSymbol = boundFunction->GetFunctionSymbol();
    if (functionSymbol->IsExternal()) return;
    int index = 0;
    for (Cm::Sym::ParameterSymbol* parameterSymbol : functionSymbol->Parameters())
    {
        Cm::BoundTree::BoundReceiveStatement* boundReceiveStatement = new Cm::BoundTree::BoundReceiveStatement(parameterSymbol);
        Cm::Sym::TypeSymbol* parameterType = parameterSymbol->GetType();
        std::vector<Cm::Core::Argument> resolutionArguments;
        Cm::Core::Argument targetArgument(Cm::Core::ArgumentCategory::lvalue, boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(parameterType, parameterSymbol->GetSpan()));
        resolutionArguments.push_back(targetArgument);
        if (parameterType->IsNonConstReferenceType())
        {
            Cm::Core::Argument sourceArgument = Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, parameterType);
            resolutionArguments.push_back(sourceArgument);
        }
        else if (parameterType->IsRvalueRefType())
        {
            Cm::Core::Argument sourceArgument = Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, parameterType);
            resolutionArguments.push_back(sourceArgument);
        }
        else
        {
            Cm::Core::Argument sourceArgument = Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, boundCompileUnit.SymbolTable().GetTypeRepository().MakeConstReferenceType(parameterType, parameterSymbol->GetSpan()));
            resolutionArguments.push_back(sourceArgument);
        }
        Cm::Sym::FunctionLookupSet functionLookups;
        functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, parameterType->GetContainerScope()->ClassInterfaceOrNsScope()));
        std::vector<Cm::Sym::FunctionSymbol*> conversions;
        Cm::Sym::FunctionSymbol* ctor = ResolveOverload(containerScope, boundCompileUnit, "@constructor", resolutionArguments, functionLookups, parameterSymbol->GetSpan(), conversions);
        boundReceiveStatement->SetConstructor(ctor);
        boundFunction->Body()->InsertStatement(index, boundReceiveStatement);
        ++index;
    }
    boundFunction->SetClassObjectLayoutFunIndex(index);
}

} } // namespace Cm::Bind
