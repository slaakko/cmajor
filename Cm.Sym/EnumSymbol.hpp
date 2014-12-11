/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_ENUM_SYMBOL_INCLUDED
#define CM_SYM_ENUM_SYMBOL_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/Value.hpp>
#include <Cm.Ast/Enumeration.hpp>

namespace Cm { namespace Sym {

class EnumTypeSymbol : public TypeSymbol
{
public:
    EnumTypeSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::enumTypeSymbol; }
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    bool IsEnumTypeSymbol() const override { return true; }
    TypeSymbol* GetUnderlyingType() const { return underlyingType; }
    void SetUnderlyingType(TypeSymbol* underlyingType_);
private:
    TypeSymbol* underlyingType;
};

class EnumConstantSymbol : public Symbol
{
public:
    EnumConstantSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::enumConstantSymbol; }
    bool IsEnumConstantSymbol() const override { return true; }
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    void SetValue(Value* value_);
    Value* GetValue() const { return value.get(); }
    bool Evaluating() const { return evaluating; }
    void SetEvaluating() { evaluating = true; }
    void ResetEvaluating() { evaluating = false; }
private:
    std::unique_ptr<Value> value;
    bool evaluating;
};

} } // namespace Cm::Sym

#endif // CM_SYM_ENUM_SYMBOL_INCLUDED
