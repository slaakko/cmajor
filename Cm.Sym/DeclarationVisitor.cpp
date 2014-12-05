/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/DeclarationVisitor.hpp>
#include <Cm.Ast/Namespace.hpp>

namespace Cm { namespace Sym {

DeclarationVisitor::DeclarationVisitor(SymbolTable& symbolTable_) : Cm::Ast::Visitor(false, false), symbolTable(symbolTable_)
{
}

void DeclarationVisitor::BeginVisit(Cm::Ast::NamespaceNode& namespaceNode)
{
    symbolTable.BeginNamespaceScope(&namespaceNode);
}

void DeclarationVisitor::EndVisit(Cm::Ast::NamespaceNode& namespaceNode)
{
    symbolTable.EndNamespaceScope();
}

void DeclarationVisitor::BeginVisit(Cm::Ast::ClassNode& classNode)
{
    symbolTable.BeginClassScope(&classNode);
}

void DeclarationVisitor::EndVisit(Cm::Ast::ClassNode& classNode)
{
    symbolTable.EndClassScope();
}

void DeclarationVisitor::BeginVisit(Cm::Ast::EnumTypeNode& enumTypeNode)
{
    symbolTable.BeginEnumScope(&enumTypeNode);
}

void DeclarationVisitor::EndVisit(Cm::Ast::EnumTypeNode& enumTypeNode)
{
    symbolTable.EndEnumScope();
}

void DeclarationVisitor::Visit(Cm::Ast::EnumConstantNode& enumConstantNode)
{
    symbolTable.AddEnumConstant(&enumConstantNode);
}

} } // namespace Cm::Sym