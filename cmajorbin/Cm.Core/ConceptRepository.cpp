/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/ConceptRepository.hpp>
#include <Cm.Ast/IntrinsicConcept.hpp>
#include <Cm.Sym/SymbolTable.hpp>

namespace Cm { namespace Core {

Cm::Sym::InstantiatedConceptSymbol* ConceptRepository::GetInstantiatedConcept(const Cm::Sym::TypeId& instantiatedConceptId) const
{
    ConceptMapIt i = conceptMap.find(instantiatedConceptId);
    if (i != conceptMap.end())
    {
        return i->second;
    }
    return nullptr;
}

void ConceptRepository::AddInstantiatedConcept(const Cm::Sym::TypeId& instantiatedConceptId, Cm::Sym::InstantiatedConceptSymbol* instantiatedConcept)
{
    conceptMap[instantiatedConceptId] = instantiatedConcept;
    concepts.push_back(std::unique_ptr<Cm::Sym::InstantiatedConceptSymbol>(instantiatedConcept));
}

GlobalConceptData::GlobalConceptData()
{
    intrinsicConcepts.push_back(std::unique_ptr<Cm::Ast::ConceptNode>(new Cm::Ast::SameConceptNode()));
    intrinsicConcepts.push_back(std::unique_ptr<Cm::Ast::ConceptNode>(new Cm::Ast::DerivedConceptNode()));
    intrinsicConcepts.push_back(std::unique_ptr<Cm::Ast::ConceptNode>(new Cm::Ast::ConvertibleConceptNode()));
    intrinsicConcepts.push_back(std::unique_ptr<Cm::Ast::ConceptNode>(new Cm::Ast::ExplicitlyConvertibleConceptNode()));

    memberFunctionGroups.insert("operator<<");
    memberFunctionGroups.insert("operator>>");
    memberFunctionGroups.insert("operator==");
    memberFunctionGroups.insert("operator=");
    memberFunctionGroups.insert("operator<");
    memberFunctionGroups.insert("operator++");
    memberFunctionGroups.insert("operator--");
    memberFunctionGroups.insert("operator->");
    memberFunctionGroups.insert("operator+");
    memberFunctionGroups.insert("operator-");
    memberFunctionGroups.insert("operator*");
    memberFunctionGroups.insert("operator/");
    memberFunctionGroups.insert("operator%");
    memberFunctionGroups.insert("operator&");
    memberFunctionGroups.insert("operator|");
    memberFunctionGroups.insert("operator^");
    memberFunctionGroups.insert("operator!");
    memberFunctionGroups.insert("operator~");
    memberFunctionGroups.insert("operator[]");
    memberFunctionGroups.insert("operator()");
}

void GlobalConceptData::AddIntrinsicConceptsToSymbolTable(Cm::Sym::SymbolTable& symbolTable)
{
    for (const std::unique_ptr<Cm::Ast::ConceptNode>& conceptNode : intrinsicConcepts)
    {
        symbolTable.BeginConceptScope(conceptNode.get());
        symbolTable.EndConceptScope();
    }
}

bool GlobalConceptData::IsMemberFunctionGroup(const std::string& groupName) const
{
    return memberFunctionGroups.find(groupName) != memberFunctionGroups.end();
}

GlobalConceptData* globalConceptData;

void SetGlobalConceptData(GlobalConceptData* globalConceptData_)
{
    globalConceptData = globalConceptData_;
}

GlobalConceptData& GetGlobalConceptData()
{
    return *globalConceptData;
}

bool IsMemberFunctionGroup(const std::string& groupName)
{
    return GetGlobalConceptData().IsMemberFunctionGroup(groupName);
}

} } // namespace Cm::Core