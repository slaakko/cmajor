/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ConceptGroupSymbol.hpp>
#include <Cm.Core/Exception.hpp>

namespace Cm { namespace Sym {

ConceptGroupSymbol::ConceptGroupSymbol(const Span& span_, const std::string& name_, ContainerScope* containerScope_) : Symbol(span_, name_), containerScope(containerScope_)
{
}

void ConceptGroupSymbol::Dump(CodeFormatter& formatter)
{
}

void ConceptGroupSymbol::AddConcept(ConceptSymbol* concept)
{
    if (concept->GroupName() != Name())
    {
        throw std::runtime_error("attempt to insert a concept with group name '" + concept->GroupName() + "' to wrong concept group '" + Name() + "'"); 
    }
    int arity = concept->Arity();
    ArityConceptMapIt i = arityConceptMap.find(arity);
    if (i != arityConceptMap.end())
    {
        throw Cm::Core::Exception("concept group '" + Name() + "' already has concept with arity " + std::to_string(arity) + "", GetSpan(), concept->GetSpan());
    }
    arityConceptMap[arity] = concept;
}

ConceptSymbol* ConceptGroupSymbol::GetConcept(int arity) const
{
    ArityConceptMapIt i = arityConceptMap.find(arity);
    if (i != arityConceptMap.end())
    {
        return i->second;
    }
    else
    {
        throw Cm::Core::Exception("concept with arity " + std::to_string(arity) + " not found in concept group '" + Name() + "'", GetSpan());
    }
}

} } // namespace Cm::Sym
