/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Class.hpp>
#include <Cm.Bind/Exception.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Bind {

Cm::Sym::ClassTypeSymbol* BindClass(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::ClassNode* classNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(classNode->Id()->Str());
    if (symbol)
    {
        if (symbol->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* classTypeSymbol = static_cast<Cm::Sym::ClassTypeSymbol*>(symbol);
            BindClass(symbolTable, containerScope, fileScope, classNode, classTypeSymbol);
            return classTypeSymbol;
        }
        else
        {
            throw Exception("symbol '" + symbol->FullName() + "' does not denote a class", symbol->GetSpan());
        }
    }
    else
    {
        throw Exception("symbol '" + classNode->Id()->Str() + "' not found");
    }
}

void BindClass(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::ClassNode* classNode, Cm::Sym::ClassTypeSymbol* classTypeSymbol)
{
    if (classTypeSymbol->Bound()) return;
    Cm::Ast::Specifiers specifiers = classNode->GetSpecifiers();
    bool isClassMember = classNode->Parent()->IsClassNode();
    SetAccess(classTypeSymbol, specifiers, isClassMember);
    Cm::Ast::Node* baseClassTypeExpr = classNode->BaseClassTypeExpr();
    if (baseClassTypeExpr)
    {
        Cm::Sym::TypeSymbol* baseTypeSymbol = ResolveType(symbolTable, containerScope, fileScope, baseClassTypeExpr);
        if (baseTypeSymbol)
        {
            if (baseTypeSymbol->IsClassTypeSymbol())
            {
                Cm::Sym::ClassTypeSymbol* baseClassTypeSymbol = static_cast<Cm::Sym::ClassTypeSymbol*>(baseTypeSymbol);
                if (baseClassTypeSymbol->Access() < classTypeSymbol->Access())
                {
                    throw Exception("base class type must be at least as accessible as the class type itself", baseClassTypeSymbol->GetSpan(), classTypeSymbol->GetSpan());
                }
                classTypeSymbol->SetBaseClass(baseClassTypeSymbol);
                classTypeSymbol->GetContainerScope()->SetBase(baseClassTypeSymbol->GetContainerScope());
            }
            else
            {
                throw Exception("base class type expression does not denote a class type", baseClassTypeExpr->GetSpan());
            }
        }
        else
        {
            throw Exception("base class type expression does not denote a type", baseClassTypeExpr->GetSpan());
        }
    }
    classTypeSymbol->SetIrType(Cm::IrIntf::CreateClassTypeName(classTypeSymbol->FullName()));
    classTypeSymbol->SetBound();
}

} } // namespace Cm::Bind

