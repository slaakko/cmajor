/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_PARAMETER_SYMBOL_INCLUDED
#define CM_SYM_PARAMETER_SYMBOL_INCLUDED
#include <Cm.Sym/Symbol.hpp>
#include <Cm.Ast/Parameter.hpp>

namespace Cm { namespace Sym {

class TypeSymbol;

class ParameterSymbol : public Symbol
{
public:
    ParameterSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::parameterSymbol; }
    std::string TypeString() const override { return "parameter"; };
    bool IsExportSymbol() const override { return true; }
    bool IsParameterSymbol() const override { return true; }
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    TypeSymbol* GetType() const;
    void SetType(TypeSymbol* type_) { type = type_; }
    void SetType(TypeSymbol* type_, int index) override;
private:
    TypeSymbol* type;
};

} } // namespace Cm::Sym

#endif // CM_SYM_PARAMETER_SYMBOL_INCLUDED
