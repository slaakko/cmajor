/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_CONTAINER_SYMBOL_INCLUDED
#define CM_SYM_CONTAINER_SYMBOL_INCLUDED
#include <Cm.Sym/Scope.hpp>

namespace Cm { namespace Sym {

class FunctionSymbol;
class TemplateTypeSymbol;

class ContainerSymbol : public Symbol
{
public:
    ContainerSymbol(const Span& span_, const std::string& name_);
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    ContainerScope* GetContainerScope() override { return &containerScope; }
    bool IsContainerSymbol() const override { return true; }
    void AddSymbol(Symbol* symbol);
    void AddFunctionSymbol(FunctionSymbol* functionSymbol);
    void AddTemplateTypeSymbol(TemplateTypeSymbol* templateTypeSymbol);
    std::vector<std::unique_ptr<Symbol>>& Symbols() { return symbols; }
    void Dump(CodeFormatter& formatter) override;
private:
    ContainerScope containerScope;
    std::vector<std::unique_ptr<Symbol>> symbols;
};

} } // namespace Cm::Sym

#endif // CM_SYM_CONTAINER_SYMBOL_INCLUDED
