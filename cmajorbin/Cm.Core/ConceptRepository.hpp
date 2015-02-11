/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_CONCEPT_REPOSITORY_INCLUDED
#define CM_CORE_CONCEPT_REPOSITORY_INCLUDED
#include <Cm.Sym/ConceptSymbol.hpp>

namespace Cm { namespace Core {

class ConceptRepository
{
public:
    Cm::Sym::InstantiatedConceptSymbol* GetInstantiatedConcept(const Cm::Sym::TypeId& instantiatedConceptId) const;
    void AddInstantiatedConcept(const Cm::Sym::TypeId& instantiatedConceptId, Cm::Sym::InstantiatedConceptSymbol* instantiatedConcept);
private:
    typedef std::unordered_map<Cm::Sym::TypeId, Cm::Sym::InstantiatedConceptSymbol*, Cm::Sym::TypeIdHash> ConceptMap;
    typedef ConceptMap::const_iterator ConceptMapIt;
    ConceptMap conceptMap;
    std::vector<std::unique_ptr<Cm::Sym::InstantiatedConceptSymbol>> concepts;
};

class GlobalConceptData
{
public:
    GlobalConceptData();
    void AddIntrinsicConceptsToSymbolTable(Cm::Sym::SymbolTable& symbolTable);
    bool IsMemberFunctionGroup(const std::string& groupName) const;
private:
    std::vector<std::unique_ptr<Cm::Ast::ConceptNode>> intrinsicConcepts;
    std::unordered_set<std::string> memberFunctionGroups;
};

void SetGlobalConceptData(GlobalConceptData* globalConceptData_);

GlobalConceptData& GetGlobalConceptData();

bool IsMemberFunctionGroup(const std::string& groupName);

} } // namespace Cm::Core

#endif // CM_CORE_CONCEPT_REPOSITORY_INCLUDED
