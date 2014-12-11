/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_TYPEDEF_SYMBOL_INCLUDED
#define CM_SYM_TYPEDEF_SYMBOL_INCLUDED
#include <Cm.Sym/Symbol.hpp>

namespace Cm { namespace Sym {

class TypeSymbol;

class TypedefSymbol : public Symbol
{
public:
    TypedefSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::typedefSymbol; }
    bool IsTypedefSymbol() const override { return true; }
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    void SetType(TypeSymbol* type_) { type = type_;  }
    TypeSymbol* GetType() const { return type; }
    bool Evaluating() const { return evaluating; }
    void SetEvaluating() { evaluating = true; }
    void ResetEvaluating() { evaluating = false; }
private:
    TypeSymbol* type;
    bool evaluating;
};

} } // namespace Cm::Sym

#endif // CM_SYM_TYPEDEF_SYMBOL_INCLUDED
