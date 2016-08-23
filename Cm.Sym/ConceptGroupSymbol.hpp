/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_CONCEPT_GROUP_SYMBOL_INCLUDED
#define CM_SYM_CONCEPT_GROUP_SYMBOL_INCLUDED
#include <Cm.Sym/ConceptSymbol.hpp>

namespace Cm { namespace Sym {

class ConceptGroupSymbol : public Symbol
{
public:
    ConceptGroupSymbol(const Span& span_, const std::string& name_, ContainerScope* containerScope_);
    SymbolType GetSymbolType() const override { return SymbolType::conceptGroupSymbol; }
    bool IsExportSymbol() const override { return false; }
    bool IsConceptGroupSymbol() const override { return true; }
    std::string TypeString() const override { return "concept group"; };
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    ContainerScope* GetContainerScope() const override { return const_cast<ContainerScope*>(containerScope); }
    void AddConcept(ConceptSymbol* concept);
    ConceptSymbol* GetConcept(int arity) const;
    void Dump(CodeFormatter& formatter) override;
    void CollectSymbolsForCC(std::unordered_set<Symbol*>& ccSymbols);
private:
    ContainerScope* containerScope;
    typedef std::unordered_map<int, ConceptSymbol*> ArityConceptMap;
    typedef ArityConceptMap::const_iterator ArityConceptMapIt;
    ArityConceptMap arityConceptMap;
};

} } // namespace Cm::Sym

#endif // CM_SYM_CONCEPT_GROUP_SYMBOL_INCLUDED
