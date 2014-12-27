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

class DelegateTypeSymbol : public TypeSymbol
{
public:
    DelegateTypeSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::delegateSymbol; }
    std::string TypeString() const override { return "delegate"; };
    bool IsDelegateTypeSymbol() const override { return true; }
    std::string GetMangleId() const override;
};

class ClassDelegateTypeSymbol : public TypeSymbol
{
public:
    ClassDelegateTypeSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::classDelegateSymbol; }
    std::string TypeString() const override { return "class delegate"; };
    std::string GetMangleId() const override;
    bool IsClassDelegateTypeSymbol() const override { return true; }
};

} } // namespace Cm::Sym

#endif // CM_SYM_DELEGATE_SYMBOL_INCLUDED
