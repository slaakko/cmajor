/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Enumeration.hpp>
#include <Cm.Bind/Exception.hpp>
#include <Cm.Bind/Evaluator.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/LocalVariableSymbol.hpp>
#include <Cm.Ast/Enumeration.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Bind {

void BindEnumType(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::EnumTypeNode* enumTypeNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(enumTypeNode->Id()->Str());
    if (symbol)
    {
        if (symbol->IsEnumTypeSymbol())
        {
            Cm::Sym::EnumTypeSymbol* enumTypeSymbol = static_cast<Cm::Sym::EnumTypeSymbol*>(symbol);
            if (enumTypeSymbol->Bound())
            {
                return;
            }
            Cm::Ast::Node* underlyingTypeNode = enumTypeNode->GetUnderlyingType();
            if (underlyingTypeNode)
            {
                Cm::Sym::ContainerScope* scope = symbolTable.GetContainerScope(underlyingTypeNode);
                Cm::Sym::TypeSymbol* underlyingType = ResolveType(symbolTable, scope, fileScope, underlyingTypeNode);
                if (underlyingType->IsBasicTypeSymbol())
                {
                    enumTypeSymbol->SetUnderlyingType(underlyingType);
                }
                else
                {
                    throw Exception("underlying type for an enumerated type must be basic type", enumTypeSymbol->GetSpan());
                }
            }
            else
            {
                enumTypeSymbol->SetUnderlyingType(symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId)));
            }
            enumTypeSymbol->SetBound();
            enumTypeSymbol->SetExportSymbol();
        }
        else
        {
            throw Exception("symbol '" + symbol->FullName() + "' does not denote an enumerated type", symbol->GetSpan());
        }
    }
    else
    {
        throw Exception("symbol '" + enumTypeNode->Id()->Str() + "' not found");
    }
}

void BindEnumConstant(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::EnumConstantNode* enumConstantNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(enumConstantNode->Id()->Str());
    if (symbol)
    {
        if (symbol->IsEnumConstantSymbol())
        {
            Cm::Sym::EnumConstantSymbol* enumConstantSymbol = static_cast<Cm::Sym::EnumConstantSymbol*>(symbol);
            if (enumConstantSymbol->Evaluating())
            {
                throw Exception("cyclic enumeration constant definitions detected", enumConstantSymbol->GetSpan());
            }
            if (enumConstantSymbol->Bound())
            {
                return; 
            }
            if (!containerScope->Container()->IsEnumTypeSymbol())
            {
                throw std::runtime_error("enumeration constant not contained by enumeration type");
            }
            Cm::Sym::EnumTypeSymbol* enumType = static_cast<Cm::Sym::EnumTypeSymbol*>(containerScope->Container());
            Cm::Sym::TypeSymbol* underlyingType = enumType->GetUnderlyingType();
            Cm::Sym::SymbolType symbolType = underlyingType->GetSymbolType();
            Cm::Sym::ValueType valueType = GetValueTypeFor(symbolType);
            enumConstantSymbol->SetEvaluating();
            Cm::Sym::Value* value = Evaluate(valueType, false, enumConstantNode->Value(), symbolTable, containerScope, fileScope);
            enumConstantSymbol->ResetEvaluating();
            enumConstantSymbol->SetValue(value);
            enumConstantSymbol->SetBound();
            enumConstantSymbol->SetExportSymbol();
        }
        else
        {
            throw Exception("symbol '" + symbol->FullName() + "' does not denote an enumeration constant", symbol->GetSpan());
        }
    }
    else
    {
        throw Exception("symbol '" + enumConstantNode->Id()->Str() + "' not found");
    }
}

} } // namespace Cm::Bind