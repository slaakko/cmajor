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
class FunctionGroupSymbol;
class ConceptGroupSymbol;
class TemplateTypeSymbol;

class ContainerSymbol : public Symbol
{
public:
    ContainerSymbol(const Span& span_, const std::string& name_);
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    ContainerScope* GetContainerScope() const override { return const_cast<ContainerScope*>(&containerScope); }
    bool IsContainerSymbol() const override { return true; }
    virtual void AddSymbol(Symbol* symbol);
    FunctionGroupSymbol* MakeFunctionGroupSymbol(const std::string& groupName, const Span& span);
    ConceptGroupSymbol* MakeConceptGroupSymbol(const std::string& groupName, const Span& span);
    std::vector<std::unique_ptr<Symbol>>& Symbols() { return symbols; }
    void Dump(CodeFormatter& formatter) override;
    void CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes) override;
    void CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TemplateTypeSymbol*>& exportedTemplateTypes) override;
    void InitVirtualFunctionTables();
private:
    ContainerScope containerScope;
    std::vector<std::unique_ptr<Symbol>> symbols;
};

} } // namespace Cm::Sym

#endif // CM_SYM_CONTAINER_SYMBOL_INCLUDED