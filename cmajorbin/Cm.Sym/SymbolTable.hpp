/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_SYMBOL_TABLE_INCLUDED
#define CM_SYM_SYMBOL_TABLE_INCLUDED
#include <Cm.Sym/NamespaceSymbol.hpp>
#include <Cm.Ast/Namespace.hpp>
#include <Cm.Ast/Class.hpp>
#include <Cm.Ast/Enumeration.hpp>
#include <Cm.Ast/Enumeration.hpp>
#include <stack>

namespace Cm { namespace Sym {

class SymbolTable
{
public:
    SymbolTable();
    void BeginNamespaceScope(Cm::Ast::NamespaceNode* namespaceNode);
    void EndNamespaceScope();
    void BeginClassScope(Cm::Ast::ClassNode* classNode);
    void EndClassScope();
    void BeginEnumScope(Cm::Ast::EnumTypeNode* enumTypeNode);
    void EndEnumScope();
    void AddEnumConstant(Cm::Ast::EnumConstantNode* enumConstantNode);
private:
    NamespaceSymbol globalNs;
    ContainerSymbol* container;
    std::stack<ContainerSymbol*> containerStack;
    void BeginContainer(ContainerSymbol* container_);
    void EndContainer();
};

} } // namespace Cm::Sym

#endif // CM_SYM_SYMBOL_TABLE_INCLUDED
