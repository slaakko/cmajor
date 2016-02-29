/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Type.hpp>
#include <Cm.Bind/Class.hpp>
#include <Cm.Bind/Interface.hpp>
#include <Cm.Bind/Delegate.hpp>
#include <Cm.Bind/Enumeration.hpp>

namespace Cm { namespace Bind {

void BindType(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Sym::TypeSymbol* type)
{
    if (type->Bound()) return;
    if (type->IsDerivedTypeSymbol()) return;
    if (type->IsTemplateTypeSymbol()) return;
    Cm::Ast::Node* node = symbolTable.GetNode(type);
    if (type->IsClassTypeSymbol())
    {
        Cm::Ast::ClassNode* classNode = nullptr;
        if (node->IsClassNode())
        {
            classNode = static_cast<Cm::Ast::ClassNode*>(node);
        }
        else
        {
            throw std::runtime_error("not class node");
        }
        BindClass(symbolTable, containerScope, fileScopes, classTemplateRepository, classNode, static_cast<Cm::Sym::ClassTypeSymbol*>(type));
    }
    else if (type->IsInterfaceTypeSymbol())
    {
        Cm::Ast::InterfaceNode* interfaceNode = nullptr;
        if (node->IsInterfaceNode())
        {
            interfaceNode = static_cast<Cm::Ast::InterfaceNode*>(node);
        }
        else
        {
            throw std::runtime_error("interface node expected");
        }
        BindInterface(symbolTable, containerScope, fileScopes, interfaceNode);
    }
    else if (type->IsEnumTypeSymbol())
    {
        Cm::Ast::EnumTypeNode* enumTypeNode = nullptr;
        if (node->IsEnumTypeNode())
        {
            enumTypeNode = static_cast<Cm::Ast::EnumTypeNode*>(node);
        }
        else
        {
            throw std::runtime_error("not enum type node");
        }
        BindEnumType(symbolTable, containerScope, fileScopes, classTemplateRepository, enumTypeNode, static_cast<Cm::Sym::EnumTypeSymbol*>(type));
    }
    else if (type->IsDelegateTypeSymbol())
    {
        Cm::Ast::DelegateNode* delegateTypeNode = nullptr;
        if (node->IsDelegateTypeNode())
        {
            delegateTypeNode = static_cast<Cm::Ast::DelegateNode*>(node);
        }
        else
        {
            throw std::runtime_error("not delegate type node");
        }
        BindDelegate(symbolTable, containerScope, fileScopes, classTemplateRepository, delegateTypeNode, static_cast<Cm::Sym::DelegateTypeSymbol*>(type));
    }
    else if (type->IsClassDelegateTypeSymbol())
    {
        Cm::Ast::ClassDelegateNode* classDelegateTypeNode = nullptr;
        if (node->IsClassDelegateTypeNode())
        {
            classDelegateTypeNode = static_cast<Cm::Ast::ClassDelegateNode*>(node);
        }
        else
        {
            throw std::runtime_error("not delegate type node");
        }
        BindClassDelegate(symbolTable, containerScope, fileScopes, classDelegateTypeNode, static_cast<Cm::Sym::ClassDelegateTypeSymbol*>(type));
    }
}

} } // namespace Cm::Bind
