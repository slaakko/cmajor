/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Template.hpp>
#include <Cm.Bind/Prebinder.hpp>
#include <Cm.Bind/Binder.hpp>
#include <Cm.Parser/FileRegistry.hpp>
#include <Cm.Sym/TemplateParameterSymbol.hpp>
#include <Cm.Sym/DeclarationVisitor.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Util/TextUtils.hpp>

namespace Cm { namespace Bind {

Cm::Ast::NamespaceNode* CreateNamespaces(const Cm::Parsing::Span& span, const std::string& nsFullName, Cm::Ast::NamespaceNode*& currentNs)
{
    Cm::Ast::NamespaceNode* globalNs = new Cm::Ast::NamespaceNode(span);
    currentNs = globalNs;
    if (!nsFullName.empty())
    {
        std::vector<std::string> nsComponents = Cm::Util::Split(nsFullName, '.');
        for (const std::string& nsComponent : nsComponents)
        {
            Cm::Ast::NamespaceNode* namespaceNode(new Cm::Ast::NamespaceNode(span, new Cm::Ast::IdentifierNode(span, nsComponent)));
            currentNs->AddMember(namespaceNode);
            currentNs = namespaceNode;
        }
    }
    return globalNs;
}

Cm::Ast::CompoundStatementNode* ReadFunctionTemplateBody(Cm::Sym::FunctionSymbol* functionTemplate)
{
    const std::string& cmlFilePath = functionTemplate->CmlFilePath();
    Cm::Ser::BinaryReader binaryReader(cmlFilePath);
    binaryReader.SetPos(functionTemplate->BodyPos());
    Cm::Ast::Reader astReader(binaryReader);
    Cm::Ast::CompoundStatementNode* body = astReader.ReadCompoundStatementNode();
    return body;
}

Cm::Ast::FunctionNode* CreateFunctionInstanceNode(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::FunctionSymbol* functionTemplate, std::unique_ptr<Cm::Ast::CompoundStatementNode>& ownedBody)
{
    Cm::Ast::FunctionNode* functionInstanceNode = nullptr;
    Cm::Ast::CompoundStatementNode* body = nullptr;
    Cm::Ast::FunctionNode* functionTemplateNode = static_cast<Cm::Ast::FunctionNode*>(boundCompileUnit.SymbolTable().GetNode(functionTemplate, false));
    if (functionTemplateNode)
    {
        Cm::Ast::Node* returnTypeExpr = functionTemplateNode->ReturnTypeExpr();
        functionInstanceNode = new Cm::Ast::FunctionNode(functionTemplate->GetSpan(), functionTemplateNode->GetSpecifiers(), returnTypeExpr ? returnTypeExpr->Clone() : nullptr,
            static_cast<Cm::Ast::FunctionGroupIdNode*>(functionTemplateNode->GroupId()->Clone()));
        body = functionTemplateNode->Body();
        for (const std::unique_ptr<Cm::Ast::ParameterNode>& parameterNode : functionTemplateNode->Parameters())
        {
            functionInstanceNode->AddParameter(static_cast<Cm::Ast::ParameterNode*>(parameterNode->Clone()));
        }
    }
    else
    {
        Cm::Ast::Node* returnTypeExpr = functionTemplate->ReturnTypeExprNode();
        Cm::Ast::FunctionGroupIdNode* groupId = functionTemplate->GroupId();
        functionInstanceNode = new Cm::Ast::FunctionNode(functionTemplate->GetSpan(), functionTemplate->GetSpecifiers(), returnTypeExpr ? returnTypeExpr->Clone() : nullptr,
            static_cast<Cm::Ast::FunctionGroupIdNode*>(groupId->Clone()));
        ownedBody.reset(ReadFunctionTemplateBody(functionTemplate));
        for (const Cm::Sym::ParameterSymbol* parameter : functionTemplate->Parameters())
        {
            functionInstanceNode->AddParameter(static_cast<Cm::Ast::ParameterNode*>(parameter->ParameterNode()->Clone()));
        }
        body = ownedBody.get();
    }
    functionInstanceNode->SetBody(static_cast<Cm::Ast::CompoundStatementNode*>(body->Clone()));
    return functionInstanceNode;
}

void BindTemplateParameters(Cm::Sym::FunctionSymbol* functionTemplate, Cm::Sym::FunctionSymbol* functionTemplateInstance, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments)
{
    int n = int(functionTemplate->TemplateParameters().size());
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::TemplateParameterSymbol* templateParameter = functionTemplate->TemplateParameters()[i];
        Cm::Sym::BoundTemplateParameterSymbol* boundTemplateParam = new Cm::Sym::BoundTemplateParameterSymbol(templateParameter->GetSpan(), templateParameter->Name());
        boundTemplateParam->SetType(templateArguments[i]);
        functionTemplateInstance->AddSymbol(boundTemplateParam);
    }
}

Cm::Sym::FunctionSymbol* Instantiate(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::FunctionSymbol* functionTemplate, 
    const std::vector<Cm::Sym::TypeSymbol*>& templateArguments)
{
    Cm::Ast::NamespaceNode* currentNs = nullptr;
    std::unique_ptr<Cm::Ast::NamespaceNode> globalNs(CreateNamespaces(functionTemplate->GetSpan(), functionTemplate->Ns()->FullName(), currentNs));
    std::unique_ptr<Cm::Ast::CompoundStatementNode> ownedBody;
    Cm::Ast::FunctionNode* functionInstanceNode = CreateFunctionInstanceNode(boundCompileUnit, functionTemplate, ownedBody);
    currentNs->AddMember(functionInstanceNode);
    Cm::Sym::DeclarationVisitor declarationVisitor(boundCompileUnit.SymbolTable());
    globalNs->Accept(declarationVisitor);
    Cm::Sym::FunctionSymbol* functionTemplateInstance = boundCompileUnit.SymbolTable().GetFunctionSymbol(functionInstanceNode);
    functionTemplateInstance->SetReplicated();
    functionTemplateInstance->SetFunctionTemplateSpecialization();
    BindTemplateParameters(functionTemplate, functionTemplateInstance, templateArguments);
    Prebinder prebinder(boundCompileUnit.SymbolTable());
    globalNs->Accept(prebinder);
    functionTemplateInstance->SetTemplateArguments(templateArguments);
    functionTemplateInstance->ComputeName();
    Binder binder(boundCompileUnit);
    globalNs->Accept(binder);
    return functionTemplateInstance;
}

} } // namespace Cm::Bind