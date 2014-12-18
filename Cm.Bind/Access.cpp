/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Access.hpp>
#include <Cm.Bind/Exception.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>

namespace Cm { namespace Bind {

void SetAccess(Cm::Sym::Symbol* symbol, Cm::Ast::Specifiers specifiers, bool isClassMember)
{
    Cm::Sym::SymbolAccess access = isClassMember ? Cm::Sym::SymbolAccess::private_ : Cm::Sym::SymbolAccess::internal_;
    Cm::Ast::Specifiers accessSpecifiers = specifiers & Cm::Ast::Specifiers::access;
    if (accessSpecifiers == Cm::Ast::Specifiers::public_)
    {
        access = Cm::Sym::SymbolAccess::public_;
    }
    else if (accessSpecifiers == Cm::Ast::Specifiers::protected_)
    {
        if (isClassMember)
        {
            access = Cm::Sym::SymbolAccess::protected_;
        }
        else
        {
            throw Exception("only class members can have protected access", symbol->GetSpan());
        }
    }
    else if (accessSpecifiers == Cm::Ast::Specifiers::internal_)
    {
        access = Cm::Sym::SymbolAccess::internal_;
    }
    else if (accessSpecifiers == Cm::Ast::Specifiers::private_)
    {
        if (isClassMember)
        {
            access = Cm::Sym::SymbolAccess::private_;
        }
        else
        {
            throw Exception("only class members can have private access", symbol->GetSpan());
        }
    }
    else if (accessSpecifiers != Cm::Ast::Specifiers::none)
    {
        throw Exception("invalid combination of access specifiers: " + Cm::Ast::SpecifierStr(accessSpecifiers), symbol->GetSpan());
    }
    symbol->SetAccess(access);
}

void CheckAccess(Cm::Sym::Symbol* fromSymbol, const Span& fromSpan, Cm::Sym::Symbol* toSymbol)
{
    if (!fromSymbol || !toSymbol) return;
    Cm::Sym::FunctionSymbol* toContainingFunction = toSymbol->ContainingFunction();
    if (toContainingFunction)
    {
        Cm::Sym::FunctionSymbol* fromFunction = fromSymbol->Function();
        if (fromFunction == toContainingFunction)
        {
            return;
        }
    }
    Cm::Sym::ClassTypeSymbol* toContainingClass = toSymbol->ContainingClass();
    if (toContainingClass)
    {
        CheckAccess(fromSymbol, fromSpan, toContainingClass);
    }
    switch (toSymbol->DeclaredAccess())
    {
        case Cm::Sym::SymbolAccess::public_:
        {
            return;
        }
        case Cm::Sym::SymbolAccess::protected_:
        {
            Cm::Sym::ClassTypeSymbol* fromContainingClass = fromSymbol->ContainingClass();
            if (fromContainingClass)
            {
                if (toContainingClass->IsSameParentOrAncestorOf(fromContainingClass))
                {
                    return;
                }
                if (fromContainingClass->HasBaseClass(toContainingClass))
                {
                    return;
                }
            }
            break;
        }
        case Cm::Sym::SymbolAccess::internal_:
        {
            return;
        }
        case Cm::Sym::SymbolAccess::private_:
        {
            if (toContainingClass)
            {
                Cm::Sym::ClassTypeSymbol* fromContainingClass = fromSymbol->ContainingClass();
                if (toContainingClass->IsSameParentOrAncestorOf(fromContainingClass))
                {
                    return;
                }
            }
            break;
        }
    }
    Span span = fromSpan.Valid() ? fromSpan : fromSymbol->GetSpan();
    throw Exception(toSymbol->TypeString() + " '" + toSymbol->FullName() + "' is inaccessible due to its protection level", span, toSymbol->GetSpan());
}

} } // namespace Cm::Bind
