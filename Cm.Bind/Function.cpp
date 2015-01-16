/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Function.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/Parameter.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>

namespace Cm { namespace Bind {

Cm::Sym::FunctionSymbol* BindFunction(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::FunctionNode* functionNode, 
    Cm::Sym::ClassTypeSymbol* currentClass)
{
    Cm::Sym::FunctionSymbol* functionSymbol = symbolTable.GetFunctionSymbol(functionNode);
    if (!functionSymbol->Bound())
    {
        Cm::Ast::Specifiers specifiers = functionNode->GetSpecifiers();
        bool isClassMember = currentClass != nullptr;
        SetAccess(functionSymbol, specifiers, isClassMember);
    }
    return functionSymbol;
}

void CompleteBindFunction(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::FunctionNode* functionNode, 
    Cm::Sym::FunctionSymbol* functionSymbol, Cm::Sym::ClassTypeSymbol* currentClass)
{
    bool staticClass = false;
    if (currentClass && currentClass->IsStatic())
    {
        staticClass = true;
    }
    Cm::Ast::Specifiers specifiers = functionNode->GetSpecifiers();
    if ((specifiers & Cm::Ast::Specifiers::static_) != Cm::Ast::Specifiers::none)
    {
        if (!functionSymbol->IsMemberFunctionSymbol())
        {
            throw Cm::Core::Exception("free functions cannot be static", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsDestructor())
        {
            throw Cm::Core::Exception("destructor cannot be static", functionSymbol->GetSpan());
        }
        if (!functionNode->HasBody())
        {
            throw Cm::Core::Exception("static functions must have body", functionSymbol->GetSpan());
        }
        functionSymbol->SetStatic();
    }
    if ((specifiers & Cm::Ast::Specifiers::explicit_) != Cm::Ast::Specifiers::none)
    {
        if (!functionSymbol->IsConstructor())
        {
            throw Cm::Core::Exception("only constructors can be explicit", functionSymbol->GetSpan());
        }
        if (!functionNode->HasBody())
        {
            throw Cm::Core::Exception("explicit functions must have body", functionSymbol->GetSpan());
        }
        if (staticClass)
        {
            throw Cm::Core::Exception("static class cannot have explicit constructor", functionSymbol->GetSpan());
        }
        functionSymbol->SetExplicit();
    }
    if ((specifiers & Cm::Ast::Specifiers::external) != Cm::Ast::Specifiers::none)
    {
        if (functionSymbol->IsMemberFunctionSymbol())
        {
            throw Cm::Core::Exception("member functions cannot be external", functionSymbol->GetSpan());
        }
        if (functionNode->HasBody())
        {
            throw Cm::Core::Exception("external functions cannot have body", functionSymbol->GetSpan());
        }
        functionSymbol->SetExternal();
    }
    if ((specifiers & Cm::Ast::Specifiers::suppress) != Cm::Ast::Specifiers::none)
    {
        if (!functionSymbol->IsMemberFunctionSymbol())
        {
            throw Cm::Core::Exception("free functions cannot be suppressed", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsDestructor())
        {
            throw Cm::Core::Exception("destructor cannot be suppressed", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsStaticConstructor())
        {
            throw Cm::Core::Exception("static constructor cannot be suppressed", functionSymbol->GetSpan());
        }
        if (functionNode->HasBody())
        {
            throw Cm::Core::Exception("suppressed functions cannot have body", functionSymbol->GetSpan());
        }
        if (staticClass)
        {
            throw Cm::Core::Exception("static class cannot have suppressed members", functionSymbol->GetSpan());
        }
        functionSymbol->SetSuppressed();
    }
    if ((specifiers & Cm::Ast::Specifiers::default_) != Cm::Ast::Specifiers::none)
    {
        if (!functionSymbol->IsMemberFunctionSymbol())
        {
            throw Cm::Core::Exception("free functions cannot be default", functionSymbol->GetSpan());
        }
        if (functionNode->HasBody())
        {
            throw Cm::Core::Exception("default functions cannot have body", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsStaticConstructor())
        {
            throw Cm::Core::Exception("static constructor cannot be default", functionSymbol->GetSpan());
        }
        if (staticClass)
        {
            throw Cm::Core::Exception("static class cannot have default members", functionSymbol->GetSpan());
        }
        functionSymbol->SetDefault();
    }
    if ((specifiers & Cm::Ast::Specifiers::inline_) != Cm::Ast::Specifiers::none)
    {
        if (!functionNode->HasBody())
        {
            throw Cm::Core::Exception("inline functions must have body", functionSymbol->GetSpan());
        }
        functionSymbol->SetInline();
    }
    if ((specifiers & Cm::Ast::Specifiers::cdecl_) != Cm::Ast::Specifiers::none)
    {
        if (functionSymbol->IsMemberFunctionSymbol())
        {
            throw Cm::Core::Exception("member functions cannot be cdecl", functionSymbol->GetSpan());
        }
        functionSymbol->SetCDecl();
    }
    if ((specifiers & Cm::Ast::Specifiers::abstract_) != Cm::Ast::Specifiers::none)
    {
        if (!functionSymbol->IsMemberFunctionSymbol())
        {
            throw Cm::Core::Exception("free functions cannot be abstract", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsConstructor())
        {
            throw Cm::Core::Exception("constructor cannot be abstract", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsStaticConstructor())
        {
            throw Cm::Core::Exception("static constructor cannot be abstract", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsDestructor())
        {
            throw Cm::Core::Exception("destructor cannot be abstract", functionSymbol->GetSpan());
        }
        if (staticClass)
        {
            throw Cm::Core::Exception("static class cannot have abstract members", functionSymbol->GetSpan());
        }
        if (functionNode->HasBody())
        {
            throw Cm::Core::Exception("abstract functions cannot have body", functionSymbol->GetSpan());
        }
        functionSymbol->SetAbstract();
        currentClass->SetVirtual();
    }
    if ((specifiers & Cm::Ast::Specifiers::virtual_) != Cm::Ast::Specifiers::none)
    {
        if (!functionSymbol->IsMemberFunctionSymbol())
        {
            throw Cm::Core::Exception("free functions cannot be virtual", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsConstructor())
        {
            throw Cm::Core::Exception("constructor cannot be virtual", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsStaticConstructor())
        {
            throw Cm::Core::Exception("static constructor cannot be virtual", functionSymbol->GetSpan());
        }
        if (staticClass)
        {
            throw Cm::Core::Exception("static class cannot have virtual members", functionSymbol->GetSpan());
        }
        functionSymbol->SetVirtual();
        currentClass->SetVirtual();
    }
    if ((specifiers & Cm::Ast::Specifiers::override_) != Cm::Ast::Specifiers::none)
    {
        if (!functionSymbol->IsMemberFunctionSymbol())
        {
            throw Cm::Core::Exception("free functions cannot be override", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsConstructor())
        {
            throw Cm::Core::Exception("constructor cannot be override", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsStaticConstructor())
        {
            throw Cm::Core::Exception("static constructor cannot be override", functionSymbol->GetSpan());
        }
        if (staticClass)
        {
            throw Cm::Core::Exception("static class cannot have override members", functionSymbol->GetSpan());
        }
        functionSymbol->SetOverride();
        currentClass->SetVirtual();
    }
    if (functionNode->ReturnTypeExpr())
    {
        Cm::Sym::TypeSymbol* returnType = ResolveType(symbolTable, containerScope, fileScope, functionNode->ReturnTypeExpr());
        functionSymbol->SetReturnType(returnType);
    }
    if (functionSymbol->IsMemberFunctionSymbol())
    {
        if (functionSymbol->IsDefaultConstructor())
        {
            if (functionSymbol->IsSuppressed())
            {
                currentClass->SetHasSuppressedDefaultConstructor();
            }
            else if (functionSymbol->IsDefault())
            {
                currentClass->SetGenerateDefaultConstructor();
            }
            else
            {
                currentClass->SetHasUserDefinedConstructor();
                currentClass->SetHasUserDefinedDefaultConstructor();
            }
        }
        else if (functionSymbol->IsCopyConstructor())
        {
            if (functionSymbol->IsSuppressed())
            {
                currentClass->SetHasSuppressedCopyConstructor();
            }
            else if (functionSymbol->IsDefault())
            {
                currentClass->SetGenerateCopyConstructor();
            }
            else
            {
                currentClass->SetHasUserDefinedConstructor();
                currentClass->SetHasUserDefinedCopyConstructor();
            }
        }
        else if (functionSymbol->IsMoveConstructor())
        {
            if (functionSymbol->IsSuppressed())
            {
                currentClass->SetHasSuppressedMoveConstructor();
            }
            else if (functionSymbol->IsDefault())
            {
                currentClass->SetGenerateMoveConstructor();
            }
            else
            {
                currentClass->SetHasUserDefinedConstructor();
                currentClass->SetHasUserDefinedMoveConstructor();
            }
        }
        else if (functionSymbol->IsStaticConstructor())
        {
            if (functionSymbol->IsSuppressed())
            {
                throw Cm::Core::Exception("static constructor cannot be suppressed", functionSymbol->GetSpan());
            }
            else
            {
                currentClass->SetHasUserDefinedStaticConstructor();
            }
        }
        else if (functionSymbol->IsCopyAssignment())
        {
            if (functionSymbol->IsSuppressed())
            {
                currentClass->SetHasSuppressedCopyAssignment();
            }
            else if (functionSymbol->IsDefault())
            {
                currentClass->SetGenerateCopyAssignment();
            }
            else
            {
                currentClass->SetHasUserDefinedCopyAssignment();
            }
        }
        else if (functionSymbol->IsMoveAssignment())
        {
            if (functionSymbol->IsSuppressed())
            {
                currentClass->SetHasSuppressedMoveAssignment();
            }
            else if (functionSymbol->IsDefault())
            {
                currentClass->SetGenerateMoveAssignment();
            }
            else
            {
                currentClass->SetHasUserDefinedMoveAssignment();
            }
        }
        else if (functionSymbol->IsDestructor())
        {
            if (functionSymbol->IsSuppressed())
            {
                throw Cm::Core::Exception("destructor cannot be suppressed", functionSymbol->GetSpan());
            }
            else if (functionSymbol->IsDefault())
            {
                currentClass->SetGenerateDestructor();
            }
            else
            {
                currentClass->SetHasUserDefinedDestructor();
            }
        }
    }
}

void CheckFunctionAccessLevels(Cm::Sym::FunctionSymbol* functionSymbol)
{
    Cm::Sym::TypeSymbol* returnType = functionSymbol->GetReturnType();
    if (returnType)
    {
        if (returnType->Access() < functionSymbol->Access())
        {
            throw Cm::Core::Exception("return type of a function must be at least as accessible as the function itself", returnType->GetSpan(), functionSymbol->GetSpan());
        }
    }
    int n = int(functionSymbol->Parameters().size());
    int start = 0;
    if (functionSymbol->IsMemberFunctionSymbol())
    {
        start = 1;
    }
    for (int i = start; i < n; ++i)
    {
        Cm::Sym::ParameterSymbol* param = functionSymbol->Parameters()[i];
        Cm::Sym::TypeSymbol* parameterType = param->GetType();
        if (parameterType->Access() < functionSymbol->Access())
        {
            std::string accessStr = Cm::Sym::AccessStr(parameterType->Access());
            throw Cm::Core::Exception("parameter type of a function must be at least as accessible as the function itself", parameterType->GetSpan(), functionSymbol->GetSpan());
        }
    }
}

} } // namespace Cm::Bind
