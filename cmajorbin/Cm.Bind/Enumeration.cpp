/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Enumeration.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/Evaluator.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/LocalVariableSymbol.hpp>
#include <Cm.Ast/Enumeration.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Bind {

void BindEnumType(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Ast::EnumTypeNode* enumTypeNode)
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
            Cm::Ast::Specifiers specifiers = enumTypeNode->GetSpecifiers();
            bool isClassMember = enumTypeNode->Parent()->IsClassNode();
            SetAccess(enumTypeSymbol, specifiers, isClassMember);
            if ((specifiers & Cm::Ast::Specifiers::abstract_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("enumerated type cannot be abstract", enumTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::virtual_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("enumerated type cannot be virtual", enumTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::override_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("enumerated type cannot be override", enumTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::static_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("enumerated type cannot be static", enumTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::explicit_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("enumerated type cannot be explicit", enumTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::external) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("enumerated type cannot be external", enumTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::suppress) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("enumerated type cannot be suppressed", enumTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::default_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("enumerated type cannot be default", enumTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::inline_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("enumerated type cannot be inline", enumTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::cdecl_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("enumerated type cannot be cdecl", enumTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::nothrow_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("enumerated type cannot be nothrow", enumTypeSymbol->GetSpan());
            }
            if ((specifiers & Cm::Ast::Specifiers::throw_) != Cm::Ast::Specifiers::none)
            {
                throw Cm::Core::Exception("enumerated type cannot be throw", enumTypeSymbol->GetSpan());
            }
            Cm::Ast::Node* underlyingTypeNode = enumTypeNode->GetUnderlyingType();
            if (underlyingTypeNode)
            {
                Cm::Sym::ContainerScope* scope = symbolTable.GetContainerScope(underlyingTypeNode);
                Cm::Sym::TypeSymbol* underlyingType = ResolveType(symbolTable, scope, fileScopes, classTemplateRepository, underlyingTypeNode);
                if (underlyingType->IsIntegerTypeSymbol())
                {
                    enumTypeSymbol->SetUnderlyingType(underlyingType);
                }
                else
                {
                    throw Cm::Core::Exception("underlying type for an enumerated type must be integer type", enumTypeSymbol->GetSpan());
                }
            }
            else
            {
                enumTypeSymbol->SetUnderlyingType(symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId)));
            }
            if (!enumTypeSymbol->GetIrType())
            {
                enumTypeSymbol->SetIrType(enumTypeSymbol->GetUnderlyingType()->GetIrType()->Clone());
                enumTypeSymbol->SetDefaultIrValue(enumTypeSymbol->GetUnderlyingType()->GetIrType()->CreateDefaultValue());
            }
            enumTypeSymbol->SetBound();
        }
        else
        {
            throw Cm::Core::Exception("symbol '" + symbol->FullName() + "' does not denote an enumerated type", symbol->GetSpan());
        }
    }
    else
    {
        throw Cm::Core::Exception("symbol '" + enumTypeNode->Id()->Str() + "' not found");
    }
}

void BindEnumConstant(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Ast::EnumConstantNode* enumConstantNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(enumConstantNode->Id()->Str());
    if (symbol)
    {
        if (symbol->IsEnumConstantSymbol())
        {
            Cm::Sym::EnumConstantSymbol* enumConstantSymbol = static_cast<Cm::Sym::EnumConstantSymbol*>(symbol);
            if (enumConstantSymbol->Evaluating())
            {
                throw Cm::Core::Exception("cyclic enumeration constant definitions detected", enumConstantSymbol->GetSpan());
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
            Cm::Sym::Value* value = Evaluate(valueType, false, enumConstantNode->Value(), symbolTable, containerScope, fileScopes, classTemplateRepository);
            enumConstantSymbol->ResetEvaluating();
            enumConstantSymbol->SetValue(value);
            enumConstantSymbol->SetBound();
        }
        else
        {
            throw Cm::Core::Exception("symbol '" + symbol->FullName() + "' does not denote an enumeration constant", symbol->GetSpan());
        }
    }
    else
    {
        throw Cm::Core::Exception("symbol '" + enumConstantNode->Id()->Str() + "' not found");
    }
}

} } // namespace Cm::Bind
