/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_TEMPLATE_PARAMETER_SYMBOL_INCLUDED
#define CM_SYM_TEMPLATE_PARAMETER_SYMBOL_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Ast/Template.hpp>

namespace Cm { namespace Sym {
    
class TemplateParameterSymbol : public TypeSymbol
{
public:
    TemplateParameterSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::templateParameterSymbol; }
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    bool IsTemplateParameterSymbol() const override { return true; }
    std::string TypeString() const override { return "template parameter"; };
    std::string GetMangleId() const { return Name(); }
    bool IsExportSymbol() const override { return true; }
    int Index() const { return index; }
    void SetIndex(int index_) { index = index_; }
private:
    int index;
};

class BoundTemplateParameterSymbol : public Symbol
{
public:
    BoundTemplateParameterSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::boundTemplateParameterSymbol; }
    bool IsBoundTemplateParameterSymbol() const override { return true; }
    std::string TypeString() const override { return "bound template parameter"; };
    bool IsExportSymbol() const override { return false; }
    TypeSymbol* GetType() const { return type; }
    void SetType(TypeSymbol* type_) { type = type_; }
private:
    TypeSymbol* type;
};

} } // namespace Cm::Sym

#endif // CM_SYM_TEMPLATE_PARAMETER_SYMBOL_INCLUDED
