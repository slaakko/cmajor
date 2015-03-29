/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Class.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Bind {

Cm::Sym::ClassTypeSymbol* BindClass(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Ast::ClassNode* classNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(classNode->Id()->Str(), Cm::Sym::ScopeLookup::this_and_base_and_parent, Cm::Sym::SymbolTypeSetId::lookupClassSymbols);
    if (symbol)
    {
        if (symbol->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* classTypeSymbol = static_cast<Cm::Sym::ClassTypeSymbol*>(symbol);
            BindClass(symbolTable, containerScope, fileScopes, classTemplateRepository, classNode, classTypeSymbol);
            return classTypeSymbol;
        }
        else
        {
            throw Cm::Core::Exception("symbol '" + symbol->FullName() + "' does not denote a class", symbol->GetSpan());
        }
    }
    else
    {
        throw Cm::Core::Exception("symbol '" + classNode->Id()->Str() + "' not found");
    }
}

void BindClass(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Ast::ClassNode* classNode, Cm::Sym::ClassTypeSymbol* classTypeSymbol)
{
    if (classTypeSymbol->Bound()) return;
    Cm::Ast::Specifiers specifiers = classNode->GetSpecifiers();
    bool isClassMember = classNode->Parent()->IsClassNode();
    SetAccess(classTypeSymbol, specifiers, isClassMember);
    if ((specifiers & Cm::Ast::Specifiers::static_) != Cm::Ast::Specifiers::none)
    {
        classTypeSymbol->SetStatic();
    }
    if ((specifiers & Cm::Ast::Specifiers::abstract_) != Cm::Ast::Specifiers::none)
    {
        classTypeSymbol->SetAbstract();
    }
    if ((specifiers & Cm::Ast::Specifiers::virtual_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("class cannnot be virtual", classTypeSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::override_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("class cannnot be override", classTypeSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::explicit_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("class cannnot be explicit", classTypeSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::external) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("class cannnot be external", classTypeSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::suppress) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("class cannnot be suppressed", classTypeSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::default_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("class cannnot be default", classTypeSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::inline_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("class cannnot be inline", classTypeSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::cdecl_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("class cannnot be cdecl", classTypeSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::nothrow_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("class cannnot be nothrow", classTypeSymbol->GetSpan());
    }
    if ((specifiers & Cm::Ast::Specifiers::throw_) != Cm::Ast::Specifiers::none)
    {
        throw Cm::Core::Exception("class cannnot be throw", classTypeSymbol->GetSpan());
    }
    if (classNode->TemplateParameters().Count() > 0)
    {
        classTypeSymbol->SetBound();
        return;
    }
    Cm::Ast::Node* baseClassTypeExpr = classNode->BaseClassTypeExpr();
    if (baseClassTypeExpr)
    {
        Cm::Sym::TypeSymbol* baseTypeSymbol = ResolveType(symbolTable, classTypeSymbol->GetContainerScope(), fileScopes, classTemplateRepository, baseClassTypeExpr);
        if (baseTypeSymbol)
        {
            if (baseTypeSymbol->IsClassTypeSymbol())
            {
                Cm::Sym::ClassTypeSymbol* baseClassTypeSymbol = static_cast<Cm::Sym::ClassTypeSymbol*>(baseTypeSymbol);
                Cm::Ast::Node* node = symbolTable.GetNode(baseClassTypeSymbol, false);
                if (node)
                {
                    if (node->IsClassNode())
                    {
                        Cm::Ast::ClassNode* baseClassNode = static_cast<Cm::Ast::ClassNode*>(node);
                        Cm::Sym::ContainerScope* baseClassContainerScope = symbolTable.GetContainerScope(baseClassNode);
                        BindClass(symbolTable, baseClassContainerScope, fileScopes, classTemplateRepository, baseClassNode, baseClassTypeSymbol);
                    }
                    else
                    {
                        throw std::runtime_error("not class node");
                    }
                }
                if (baseClassTypeSymbol->Access() < classTypeSymbol->Access())
                {
                    throw Cm::Core::Exception("base class type must be at least as accessible as the class type itself", baseClassTypeSymbol->GetSpan(), classTypeSymbol->GetSpan());
                }
                classTypeSymbol->SetBaseClass(baseClassTypeSymbol);
                classTypeSymbol->GetContainerScope()->SetBase(baseClassTypeSymbol->GetContainerScope());
            }
            else
            {
                throw Cm::Core::Exception("base class type expression does not denote a class type", baseClassTypeExpr->GetSpan());
            }
        }
        else
        {
            throw Cm::Core::Exception("base class type expression does not denote a type", baseClassTypeExpr->GetSpan());
        }
    }
    if (!classTypeSymbol->GetIrType())
    {
        classTypeSymbol->SetIrType(Cm::IrIntf::CreateClassTypeName(classTypeSymbol->FullName()));
    }
    classTypeSymbol->SetBound();
}

} } // namespace Cm::Bind

