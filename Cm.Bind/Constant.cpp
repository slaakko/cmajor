/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Constant.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/Evaluator.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/Enumeration.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Sym/ConstantSymbol.hpp>
#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Bind {

void BindConstant(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, Cm::Ast::ConstantNode* constantNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(constantNode->Id()->Str());
    if (symbol)
    {
        if (symbol->IsConstantSymbol())
        {
            Cm::Sym::ConstantSymbol* constantSymbol = static_cast<Cm::Sym::ConstantSymbol*>(symbol);
            BindConstant(symbolTable, containerScope, fileScopes, constantNode, constantSymbol);
        }
        else
        {
            throw Cm::Core::Exception("symbol '" + symbol->FullName() + "' does not denote a constant", symbol->GetSpan());
        }
    }
    else
    {
        throw Cm::Core::Exception("symbol '" + constantNode->Id()->Str() + "' not found");
    }
}

void BindConstant(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, Cm::Ast::ConstantNode* constantNode, 
    Cm::Sym::ConstantSymbol* constantSymbol)
{
    if (constantSymbol->Evaluating())
    {
        throw Cm::Core::Exception("cyclic constant definitions detected", constantSymbol->GetSpan());
    }
    if (constantSymbol->Bound())
    {
        return;
    }
    Cm::Ast::Specifiers specifiers = constantNode->GetSpecifiers();
    bool isClassMember = constantNode->Parent()->IsClassNode();
    SetAccess(constantSymbol, specifiers, isClassMember);
    if ((specifiers & Cm::Ast::Specifiers::abstract_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("constant cannot be abstract", constantSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::virtual_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("constant cannot be virtual", constantSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::override_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("constant cannot be override", constantSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::static_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("constant cannot be static", constantSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::explicit_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("constant cannot be explicit", constantSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::external) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("constant cannot be external", constantSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::suppress) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("constant cannot be suppressed", constantSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::default_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("constant cannot be default", constantSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::inline_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("constant cannot be inline", constantSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::cdecl_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("constant cannot be cdecl", constantSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::nothrow_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("constant cannot be nothrow", constantSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::throw_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("constant cannot be throw", constantSymbol->GetSpan());
    }
    if (constantSymbol->Parent()->IsClassTypeSymbol())
    {
        Cm::Sym::ClassTypeSymbol* parentClass = static_cast<Cm::Sym::ClassTypeSymbol*>(constantSymbol->Parent());
        if (parentClass->IsClassTemplate())
        {
            constantSymbol->SetBound();
            return;
        }
    }
    Cm::Sym::TypeSymbol* type = ResolveType(symbolTable, containerScope, fileScopes, constantNode->TypeExpr());
    if (type->IsBoolTypeSymbol() || type->IsCharTypeSymbol() || type->IsEnumTypeSymbol() || type->IsIntegerTypeSymbol() || type->IsFloatingPointTypeSymbol())
    {
        if (type->IsEnumTypeSymbol())
        {
            Cm::Sym::EnumTypeSymbol* enumTypeSymbol = static_cast<Cm::Sym::EnumTypeSymbol*>(type);
            if (!enumTypeSymbol->Bound())
            {
                Cm::Ast::Node* node = symbolTable.GetNode(enumTypeSymbol);
                if (node->IsEnumTypeNode())
                {
                    Cm::Ast::EnumTypeNode* enumTypeNode = static_cast<Cm::Ast::EnumTypeNode*>(node);
                    Cm::Sym::ContainerScope* scope = symbolTable.GetContainerScope(enumTypeNode);
                    BindEnumType(symbolTable, scope, fileScopes, enumTypeNode);
                }
            }
            type = enumTypeSymbol->GetUnderlyingType();
        }
        if (type->Access() < constantSymbol->Access())
        {
            throw Cm::Core::Exception("type of a constant must be at least as accessible as the constant itself", type->GetSpan(), constantSymbol->GetSpan());
        }
        constantSymbol->SetType(type);
        Cm::Sym::SymbolType symbolType = type->GetSymbolType();
        Cm::Sym::ValueType valueType = GetValueTypeFor(symbolType);
        constantSymbol->SetEvaluating();
        Cm::Sym::Value* value = Evaluate(valueType, false, constantNode->Value(), symbolTable, containerScope, fileScopes);
        constantSymbol->ResetEvaluating();
        constantSymbol->SetValue(value);
        constantSymbol->SetBound();
    }
    else
    {
        throw Cm::Core::Exception("constant must be of Boolean, character, enumerated, integer or floating pointor type", constantSymbol->GetSpan());
    }
}

} } // namespace Cm::Bind
