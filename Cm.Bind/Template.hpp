/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_TEMPLATE_INCLUDED
#define CM_BIND_TEMPLATE_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>

namespace Cm { namespace Bind {

Cm::Ast::NamespaceNode* CreateNamespaces(const Cm::Parsing::Span& span, const std::string& nsFullName, const Cm::Ast::NodeList& usingNodes, Cm::Ast::NamespaceNode*& currentNs);

Cm::Sym::FunctionSymbol* Instantiate(Cm::Core::FunctionTemplateRepository& functionTemplateRepository, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, 
    Cm::Sym::FunctionSymbol* function, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments);

} } // namespace Cm::Bind

#endif // CM_BIND_TEMPLATE_INCLUDED
