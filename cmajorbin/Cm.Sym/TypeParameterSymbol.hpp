/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_TYPE_PARAMETER_SYMBOL_INCLUDED
#define CM_SYM_TYPE_PARAMETER_SYMBOL_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Ast/Template.hpp>

namespace Cm { namespace Sym {
    
class TypeParameterSymbol : public TypeSymbol
{
public:
    TypeParameterSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::typeParameterSymbol; }
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    bool IsTypeParameterSymbol() const override { return true; }
    std::string TypeString() const override { return "type parameter"; };
    std::string GetMangleId() const { return Name(); }
    bool IsExportSymbol() const override { return true; }
    int Index() const { return index; }
    void SetIndex(int index_) { index = index_; }
    std::string FullName() const override {return Name(); }
    void Dump(CodeFormatter& formatter) override;
private:
    int index;
};

class BoundTypeParameterSymbol : public Symbol
{
public:
    BoundTypeParameterSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::boundTypeParameterSymbol; }
    bool IsBoundTypeParameterSymbol() const override { return true; }
    std::string TypeString() const override { return "bound type parameter"; };
    bool IsExportSymbol() const override { return false; }
    TypeSymbol* GetType() const { return type; }
    void SetType(TypeSymbol* type_) { type = type_; }
    void Dump(CodeFormatter& formatter) override;
private:
    TypeSymbol* type;
};

} } // namespace Cm::Sym

#endif // CM_SYM_TYPE_PARAMETER_SYMBOL_INCLUDED
