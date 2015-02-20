/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_CONCEPT_INCLUDED
#define CM_BIND_CONCEPT_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.BoundTree/BoundConcept.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/TypeParameterSymbol.hpp>
#include <Cm.Ast/Concept.hpp>

namespace Cm { namespace Bind {

bool CheckConstraint(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::FileScope* functionFileScope, Cm::Ast::WhereConstraintNode* constraint, 
    const std::vector<Cm::Sym::TypeParameterSymbol*>& templateParameters, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments, Cm::Core::ConceptCheckException& exception);

Cm::BoundTree::BoundConstraint* BindConstraint(const std::vector<Cm::Sym::TypeParameterSymbol*>& templateParameters, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments,
    Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::FileScope* functionFileScope, Cm::Ast::WhereConstraintNode* constraint);

} } // namespace Cm::Bind

#endif // CM_BIND_CONCEPT_INCLUDED
