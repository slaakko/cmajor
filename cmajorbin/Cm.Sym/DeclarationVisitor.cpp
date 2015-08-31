/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/DeclarationVisitor.hpp>
#include <Cm.Sym/ParameterSymbol.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Ast/Namespace.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

DeclarationVisitor::DeclarationVisitor(SymbolTable& symbolTable_) : 
    Cm::Ast::Visitor(true, false), symbolTable(symbolTable_), parameterIndex(0), templateClassNode(nullptr), templateType(nullptr), markFunctionSymbolAsTemplateSpecialization(false)
{
}

void DeclarationVisitor::SetTemplateType(Cm::Ast::ClassNode* templateClassNode_, Cm::Sym::TemplateTypeSymbol* templateType_)
{
    templateClassNode = templateClassNode_;
    templateType = templateType_;
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
    if (&classNode == templateClassNode)
    {
        symbolTable.BeginTemplateTypeScope(templateClassNode, templateType);
    }
    else
    {
        symbolTable.BeginClassScope(&classNode);
    }
}

void DeclarationVisitor::EndVisit(Cm::Ast::ClassNode& classNode)
{
    if (&classNode == templateClassNode)
    {
        symbolTable.EndTemplateTypeScope();
    }
    else
    {
        symbolTable.EndClassScope();
    }
}

void DeclarationVisitor::BeginVisit(Cm::Ast::ConstructorNode& constructorNode)
{
    symbolTable.BeginFunctionScope(&constructorNode, FunctionSymbolFlags::constructorOrDestructorSymbol | FunctionSymbolFlags::memberFunctionSymbol);
    parameterIndex = 0;
    ParameterSymbol* thisParam = new ParameterSymbol(constructorNode.GetSpan(), "this");
    TypeSymbol* thisParamType = symbolTable.GetTypeRepository().MakePointerType(symbolTable.CurrentClass(), constructorNode.GetSpan());
    thisParam->SetType(thisParamType);
    thisParam->SetBound();
    symbolTable.AddParameter(thisParam);
}

void DeclarationVisitor::EndVisit(Cm::Ast::ConstructorNode& constructornNode)
{
    symbolTable.EndFunctionScope();
}

void DeclarationVisitor::BeginVisit(Cm::Ast::DestructorNode& destructorNode)
{
    symbolTable.BeginFunctionScope(&destructorNode, FunctionSymbolFlags::constructorOrDestructorSymbol | FunctionSymbolFlags::memberFunctionSymbol);
    parameterIndex = 0;
    ParameterSymbol* thisParam = new ParameterSymbol(destructorNode.GetSpan(), "this");
    TypeSymbol* thisParamType = symbolTable.GetTypeRepository().MakePointerType(symbolTable.CurrentClass(), destructorNode.GetSpan());
    thisParam->SetType(thisParamType);
    thisParam->SetBound();
    symbolTable.AddParameter(thisParam);
}

void DeclarationVisitor::EndVisit(Cm::Ast::DestructorNode& destructorNode)
{
    symbolTable.EndFunctionScope();
}

void DeclarationVisitor::BeginVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode)
{
    symbolTable.BeginFunctionScope(&memberFunctionNode, FunctionSymbolFlags::memberFunctionSymbol);
    parameterIndex = 0;
    if ((memberFunctionNode.GetSpecifiers() & Cm::Ast::Specifiers::static_) == Cm::Ast::Specifiers::none)
    {
        ParameterSymbol* thisParam = new ParameterSymbol(memberFunctionNode.GetSpan(), "this");
        if (memberFunctionNode.IsConst())
        {
            TypeSymbol* thisParamType = symbolTable.GetTypeRepository().MakeConstPointerType(symbolTable.CurrentClass(), memberFunctionNode.GetSpan());
            thisParam->SetType(thisParamType);
        }
        else
        {
            TypeSymbol* thisParamType = symbolTable.GetTypeRepository().MakePointerType(symbolTable.CurrentClass(), memberFunctionNode.GetSpan());
            thisParam->SetType(thisParamType);
        }
        thisParam->SetBound();
        symbolTable.AddParameter(thisParam);
    }
}

void DeclarationVisitor::EndVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode) 
{
    symbolTable.EndFunctionScope();
}

void DeclarationVisitor::BeginVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode)
{
    symbolTable.BeginFunctionScope(&conversionFunctionNode, FunctionSymbolFlags::memberFunctionSymbol);
    parameterIndex = 0;
    ParameterSymbol* thisParam = new ParameterSymbol(conversionFunctionNode.GetSpan(), "this");
    if (conversionFunctionNode.IsConst())
    {
        TypeSymbol* thisParamType = symbolTable.GetTypeRepository().MakeConstPointerType(symbolTable.CurrentClass(), conversionFunctionNode.GetSpan());
        thisParam->SetType(thisParamType);
    }
    else
    {
        TypeSymbol* thisParamType = symbolTable.GetTypeRepository().MakePointerType(symbolTable.CurrentClass(), conversionFunctionNode.GetSpan());
        thisParam->SetType(thisParamType);
    }
    thisParam->SetBound();
    symbolTable.AddParameter(thisParam);
}

void DeclarationVisitor::EndVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode)
{
    symbolTable.EndFunctionScope();
}

void DeclarationVisitor::BeginVisit(Cm::Ast::StaticConstructorNode& staticConstructorNode)
{
    symbolTable.BeginFunctionScope(&staticConstructorNode, FunctionSymbolFlags::constructorOrDestructorSymbol | FunctionSymbolFlags::memberFunctionSymbol);
}

void DeclarationVisitor::EndVisit(Cm::Ast::StaticConstructorNode& staticConstructorNode)
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
    FunctionSymbolFlags flags = FunctionSymbolFlags::none;
    if (markFunctionSymbolAsTemplateSpecialization)
    {
        flags = flags | FunctionSymbolFlags::templateSpecialization;
    }
    symbolTable.BeginFunctionScope(&functionNode, flags);
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

void DeclarationVisitor::Visit(Cm::Ast::TypedefStatementNode& typedefStatementNode)
{
    symbolTable.AddTypedef(&typedefStatementNode);
}

void DeclarationVisitor::Visit(Cm::Ast::ConceptNode& conceptNode)
{
    symbolTable.BeginConceptScope(&conceptNode);
    symbolTable.EndConceptScope();
}

} } // namespace Cm::Sym
