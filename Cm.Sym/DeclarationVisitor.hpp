/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_DECLARATION_VISITOR_INCLUDED
#define CM_SYM_DECLARATION_VISITOR_INCLUDED
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Sym {

class DeclarationVisitor : public Cm::Ast::Visitor
{
public:
    DeclarationVisitor(SymbolTable& symbolTable_);
    void BeginVisit(Cm::Ast::NamespaceNode& namespaceNode) override;
    void EndVisit(Cm::Ast::NamespaceNode& namespaceNode) override;
    void BeginVisit(Cm::Ast::ClassNode& classNode) override;
    void EndVisit(Cm::Ast::ClassNode& classNode) override;
    void BeginVisit(Cm::Ast::EnumTypeNode& enumTypeNode) override;
    void EndVisit(Cm::Ast::EnumTypeNode& enumTypeNode) override;
    void Visit(Cm::Ast::EnumConstantNode& enumConstantNode) override;
private:
    SymbolTable& symbolTable;
};

} } // namespace Cm::Sym

#endif // CM_SYM_DECLARATION_VISITOR_INCLUDED
