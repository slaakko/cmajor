/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_CONSTANT_SYMBOL_INCLUDED
#define CM_SYM_CONSTANT_SYMBOL_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/Value.hpp>
#include <Cm.Ast/Constant.hpp>

namespace Cm { namespace Sym {

class ConstantSymbol : public Symbol
{
public:
    ConstantSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::constantSymbol; }
    void SetType(TypeSymbol* type_);
    TypeSymbol* GetType() const { return type.get(); }
    virtual bool IsConstantSymbol() const { return true; }
    void SetValue(Value* value_);
    Value* GetValue() const { return value.get(); }
    bool Evaluating() const { return evaluating; }
    void SetEvaluating() { evaluating = true; }
    void ResetEvaluating() { evaluating = false; }
private:
    std::unique_ptr<TypeSymbol> type;
    std::unique_ptr<Value> value;
    bool evaluating;
};

} } // namespace Cm::Sym

#endif // CM_SYM_CONSTANT_SYMBOL_INCLUDED
