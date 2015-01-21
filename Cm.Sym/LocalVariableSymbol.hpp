/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_LOCAL_VARIABLE_SYMBOL_INCLUDED
#define CM_SYM_LOCAL_VARIABLE_SYMBOL_INCLUDED
#include <Cm.Sym/Symbol.hpp>
#include <Cm.Ast/Statement.hpp>

namespace Cm { namespace Sym {

class TypeSymbol;

class LocalVariableSymbol : public Symbol
{
public:
    LocalVariableSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::localVariableSymbol; }
    bool IsLocalVariableSymbol() const override { return true; }
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::private_; }
    std::string TypeString() const override { return "local variable"; };
    bool IsExportSymbol() const { return false; }
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    TypeSymbol* GetType() const;
    void SetType(TypeSymbol* type_) { type = type_; }
    void SetType(TypeSymbol* type_, int index) override;
    bool Used() const { return used; }
    void SetUsed() { used = true; }
    void SetUseSpan(const Cm::Parsing::Span& useSpan_);
    const Cm::Parsing::Span& GetUseSpan() const { return useSpan; }
private:
    TypeSymbol* type;
    bool used;
    Cm::Parsing::Span useSpan;
};

} } // namespace Cm::Sym

#endif // CM_SYM_LOCAL_VARIABLE_SYMBOL_INCLUDED
