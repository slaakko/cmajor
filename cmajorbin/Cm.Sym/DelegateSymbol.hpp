/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_DELEGATE_SYMBOL_INCLUDED
#define CM_SYM_DELEGATE_SYMBOL_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Ast/Delegate.hpp>

namespace Cm { namespace Sym {

class DelegateSymbol : public TypeSymbol
{
public:
    DelegateSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::delegateSymbol; }
    bool IsDelegateTypeSymbol() const override { return true; }

};

class ClassDelegateSymbol : public TypeSymbol
{
public:
    ClassDelegateSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::classDelegateSymbol; }
    bool IsClassDelegateTypeSymbol() const override { return true; }
};

} } // namespace Cm::Sym

#endif // CM_SYM_DELEGATE_SYMBOL_INCLUDED
