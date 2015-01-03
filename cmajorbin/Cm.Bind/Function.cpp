/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Function.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Bind/Exception.hpp>
#include <Cm.Bind/Parameter.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>

namespace Cm { namespace Bind {

Cm::Sym::FunctionSymbol* BindFunction(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::FunctionNode* functionNode)
{
    Cm::Sym::FunctionSymbol* functionSymbol = symbolTable.GetFunctionSymbol(functionNode);
    if (!functionSymbol->Bound())
    {
        Cm::Ast::Specifiers specifiers = functionNode->GetSpecifiers();
        bool isClassMember = functionNode->Parent()->IsClassNode();
        SetAccess(functionSymbol, specifiers, isClassMember);
        if ((specifiers & Cm::Ast::Specifiers::external) != Cm::Ast::Specifiers::none)
        {
            functionSymbol->SetExternal();
        }
        if ((specifiers & Cm::Ast::Specifiers::cdecl_) != Cm::Ast::Specifiers::none)
        {
            functionSymbol->SetCDecl();
        }
        if (functionNode->ReturnTypeExpr())
        {
            Cm::Sym::TypeSymbol* returnType = ResolveType(symbolTable, containerScope, fileScope, functionNode->ReturnTypeExpr());
            functionSymbol->SetReturnType(returnType);
        }
    }
    return functionSymbol;
}

void CheckFunctionAccessLevels(Cm::Sym::FunctionSymbol* functionSymbol)
{
    Cm::Sym::TypeSymbol* returnType = functionSymbol->GetReturnType();
    if (returnType)
    {
        if (returnType->Access() < functionSymbol->Access())
        {
            throw Exception("return type of a function must be at least as accessible as the function itself", returnType->GetSpan(), functionSymbol->GetSpan());
        }
    }
    for (Cm::Sym::ParameterSymbol* param : functionSymbol->Parameters())
    {
        Cm::Sym::TypeSymbol* parameterType = param->GetType();
        if (parameterType->Access() < functionSymbol->Access())
        {
            std::string accessStr = Cm::Sym::AccessStr(parameterType->Access());
            throw Exception("parameter type of a function must be at least as accessible as the function itself", parameterType->GetSpan(), functionSymbol->GetSpan());
        }
    }
}

} } // namespace Cm::Bind
