/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Parameter.hpp>
#include <Cm.Bind/Exception.hpp>
#include <Cm.Bind/TypeResolver.hpp>
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
            bool willBeExported = parameterSymbol->WillBeExported();
            Cm::Sym::TypeSymbol* parameterType = ResolveType(symbolTable, containerScope, fileScope, parameterNode->TypeExpr(), willBeExported);
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

} } // namespace Cm::Bind
