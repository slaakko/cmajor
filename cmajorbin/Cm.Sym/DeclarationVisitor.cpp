/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/DeclarationVisitor.hpp>
#include <Cm.Ast/Namespace.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

DeclarationVisitor::DeclarationVisitor(SymbolTable& symbolTable_) : Cm::Ast::Visitor(true, false), symbolTable(symbolTable_)
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

void DeclarationVisitor::BeginVisit(Cm::Ast::ConstructorNode& constructorNode)
{
    symbolTable.BeginFunctionScope(&constructorNode);
    parameterIndex = 0;
}

void DeclarationVisitor::EndVisit(Cm::Ast::ConstructorNode& constructornNode)
{
    symbolTable.EndFunctionScope();
}

void DeclarationVisitor::BeginVisit(Cm::Ast::DestructorNode& destructorNode)
{
    symbolTable.BeginFunctionScope(&destructorNode);
    parameterIndex = 0;
}

void DeclarationVisitor::EndVisit(Cm::Ast::DestructorNode& destructorNode)
{
    symbolTable.EndFunctionScope();
}

void DeclarationVisitor::BeginVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode)
{
    symbolTable.BeginFunctionScope(&memberFunctionNode);
    parameterIndex = 0;
}

void DeclarationVisitor::EndVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode) 
{
    symbolTable.EndFunctionScope();
}

void DeclarationVisitor::BeginVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode)
{
    symbolTable.BeginFunctionScope(&conversionFunctionNode);
    parameterIndex = 0;
}

void DeclarationVisitor::EndVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode)
{
    symbolTable.EndFunctionScope();
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

void DeclarationVisitor::Visit(Cm::Ast::TypedefNode& typedefNode)
{
    symbolTable.AddTypedef(&typedefNode);
}

void DeclarationVisitor::BeginVisit(Cm::Ast::FunctionNode& functionNode)
{
    symbolTable.BeginFunctionScope(&functionNode);
    parameterIndex = 0;
}

void DeclarationVisitor::EndVisit(Cm::Ast::FunctionNode& functionNode)
{
    symbolTable.EndFunctionScope();
}

void DeclarationVisitor::BeginVisit(Cm::Ast::DelegateNode& delegateNode)
{
    symbolTable.BeginDelegateScope(&delegateNode);
    parameterIndex = 0;
}

void DeclarationVisitor::EndVisit(Cm::Ast::DelegateNode& delegateNode)
{
    symbolTable.EndDelegateScope();
}

void DeclarationVisitor::BeginVisit(Cm::Ast::ClassDelegateNode& classDelegateNode)
{
    symbolTable.BeginClassDelegateScope(&classDelegateNode);
    parameterIndex = 0;
}

void DeclarationVisitor::EndVisit(Cm::Ast::ClassDelegateNode& classDelegateNode)
{
    symbolTable.EndClassDelegateScope();
}

void DeclarationVisitor::Visit(Cm::Ast::ConstantNode& constantNode)
{
    symbolTable.AddConstant(&constantNode);
}

void DeclarationVisitor::Visit(Cm::Ast::TemplateParameterNode& templateParameterNode)
{
    symbolTable.AddTemplateParameter(&templateParameterNode);
}

void DeclarationVisitor::Visit(Cm::Ast::ParameterNode& parameterNode)
{
    std::string parameterName;
    if (parameterNode.Id())
    {
        parameterName = parameterNode.Id()->Str();
    }
    else
    {
        parameterName = "__parameter" + std::to_string(parameterIndex);
    }
    symbolTable.AddParameter(&parameterNode, parameterName);
    ++parameterIndex;
}

void DeclarationVisitor::Visit(Cm::Ast::MemberVariableNode& memberVariableNode) 
{
    symbolTable.AddMemberVariable(&memberVariableNode);
}

void DeclarationVisitor::BeginVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode)
{
    symbolTable.BeginDeclarationScope(&compoundStatementNode);
}

void DeclarationVisitor::EndVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode)
{
    symbolTable.EndDeclarationcope();
}

void DeclarationVisitor::BeginVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode)
{
    symbolTable.BeginDeclarationScope(&rangeForStatementNode);
}

void DeclarationVisitor::EndVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode)
{
    symbolTable.EndDeclarationcope();
}

void DeclarationVisitor::BeginVisit(Cm::Ast::ForStatementNode& forStatementNode)
{
    symbolTable.BeginDeclarationScope(&forStatementNode);
}

void DeclarationVisitor::EndVisit(Cm::Ast::ForStatementNode& forStatementNode)
{
    symbolTable.EndDeclarationcope();
}

void DeclarationVisitor::BeginVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
    symbolTable.AddLocalVariable(&constructionStatementNode);
}

} } // namespace Cm::Sym
