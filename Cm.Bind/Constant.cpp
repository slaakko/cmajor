/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Constant.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/Evaluator.hpp>
#include <Cm.Bind/Exception.hpp>
#include <Cm.Bind/Enumeration.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Sym/ConstantSymbol.hpp>
#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Bind {

void BindConstant(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::ConstantNode* constantNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(constantNode->Id()->Str());
    if (symbol)
    {
        if (symbol->IsConstantSymbol())
        {
            Cm::Sym::ConstantSymbol* constantSymbol = static_cast<Cm::Sym::ConstantSymbol*>(symbol);
            BindConstant(symbolTable, containerScope, fileScope, constantNode, constantSymbol);
        }
        else
        {
            throw Exception("symbol '" + symbol->FullName() + "' does not denote a constant", symbol->GetSpan());
        }
    }
    else
    {
        throw Exception("symbol '" + constantNode->Id()->Str() + "' not found");
    }
}

void BindConstant(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::ConstantNode* constantNode, Cm::Sym::ConstantSymbol* constantSymbol)
{
    if (constantSymbol->Evaluating())
    {
        throw Exception("cyclic constant definitions detected", constantSymbol->GetSpan());
    }
    if (constantSymbol->Bound())
    {
        return;
    }
    Cm::Ast::Specifiers specifiers = constantNode->GetSpecifiers();
    bool isClassMember = constantNode->Parent()->IsClassNode();
    SetAccess(constantSymbol, specifiers, isClassMember);
    Cm::Sym::TypeSymbol* type = ResolveType(symbolTable, containerScope, fileScope, constantNode->TypeExpr());
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
                    BindEnumType(symbolTable, scope, fileScope, enumTypeNode);
                }
            }
            type = enumTypeSymbol->GetUnderlyingType();
        }
        if (type->Access() < constantSymbol->Access())
        {
            throw Exception("type of a constant must be at least as accessible as the constant itself", type->GetSpan(), constantSymbol->GetSpan());
        }
        constantSymbol->SetType(type);
        Cm::Sym::SymbolType symbolType = type->GetSymbolType();
        Cm::Sym::ValueType valueType = GetValueTypeFor(symbolType);
        constantSymbol->SetEvaluating();
        Cm::Sym::Value* value = Evaluate(valueType, false, constantNode->Value(), symbolTable, containerScope, fileScope);
        constantSymbol->ResetEvaluating();
        constantSymbol->SetValue(value);
        constantSymbol->SetBound();
    }
    else
    {
        throw Exception("constant must be of Boolean, character, enumerated, integer or floating pointor type", constantSymbol->GetSpan());
    }
}

} } // namespace Cm::Bind
