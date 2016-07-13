/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Template.hpp>
#include <Cm.Bind/Prebinder.hpp>
#include <Cm.Bind/Binder.hpp>
#include <Cm.Parser/FileRegistry.hpp>
#include <Cm.Sym/TypeParameterSymbol.hpp>
#include <Cm.Sym/DeclarationVisitor.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Clone.hpp>
#include <Cm.Util/TextUtils.hpp>

namespace Cm { namespace Bind {

Cm::Ast::NamespaceNode* CreateNamespaces(const Cm::Parsing::Span& span, const std::string& nsFullName, const Cm::Ast::NodeList& usingNodes, Cm::Ast::NamespaceNode*& currentNs)
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
    Cm::Ast::CloneContext cloneContext;
    for (const std::unique_ptr<Cm::Ast::Node>& usingNode : usingNodes)
    {
        currentNs->AddMember(usingNode->Clone(cloneContext));
    }
    return globalNs;
}

void BindTypeParameters(Cm::Sym::FunctionSymbol* functionTemplate, Cm::Sym::FunctionSymbol* functionTemplateInstance, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments)
{
    int n = int(functionTemplate->TypeParameters().size());
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::TypeParameterSymbol* templateParameter = functionTemplate->TypeParameters()[i];
        Cm::Sym::BoundTypeParameterSymbol* boundTemplateParam = new Cm::Sym::BoundTypeParameterSymbol(templateParameter->GetSpan(), templateParameter->Name());
        boundTemplateParam->SetType(templateArguments[i]);
        functionTemplateInstance->AddSymbol(boundTemplateParam);
    }
}

Cm::Sym::FunctionSymbol* Instantiate(Cm::Core::FunctionTemplateRepository& functionTemplateRepository, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit,
    Cm::Sym::FunctionSymbol* functionTemplate, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments)
{
    Cm::Core::FunctionTemplateKey key(functionTemplate, templateArguments);
    Cm::Sym::FunctionSymbol* functionTemplateInstance = functionTemplateRepository.GetFunctionTemplateInstance(key);
    if (functionTemplateInstance)
    {
        return functionTemplateInstance;
    }
    Cm::Ast::Node* node = boundCompileUnit.SymbolTable().GetNode(functionTemplate, false);
    if (!node)
    {
        functionTemplate->ReadFunctionNode(boundCompileUnit.SymbolTable(), functionTemplate->GetSpan().FileIndex());
        node = boundCompileUnit.SymbolTable().GetNode(functionTemplate);
    }
    if (!node->IsFunctionNode())
    {
        throw std::runtime_error("node is not function node");
    }
    Cm::Ast::FunctionNode* functionNode = static_cast<Cm::Ast::FunctionNode*>(node);
    Cm::Ast::NamespaceNode* currentNs = nullptr;
    std::unique_ptr<Cm::Ast::NamespaceNode> globalNs(CreateNamespaces(functionTemplate->GetSpan(), functionTemplate->Ns()->FullName(), functionTemplate->GetUsingNodes(), currentNs));
    Cm::Ast::CloneContext cloneContext;
    cloneContext.SetInstantiateFunctionNode();
    Cm::Ast::FunctionNode* functionInstanceNode = static_cast<Cm::Ast::FunctionNode*>(functionNode->Clone(cloneContext));
    currentNs->AddMember(functionInstanceNode);
    Cm::Sym::DeclarationVisitor declarationVisitor(boundCompileUnit.SymbolTable());
    declarationVisitor.MarkFunctionSymbolAsTemplateSpecialization();
    globalNs->Accept(declarationVisitor);
    functionTemplateInstance = boundCompileUnit.SymbolTable().GetFunctionSymbol(functionInstanceNode);
    if (functionTemplate->IsConstExpr())
    {
        functionTemplateInstance->SetConstExpr();
    }
    functionTemplateInstance->SetFunctionTemplate(functionTemplate);
    functionTemplateRepository.AddFunctionTemplateInstance(key, functionTemplateInstance);
    functionTemplateInstance->SetReplicated();
    functionTemplateInstance->SetFunctionTemplateSpecialization();
    BindTypeParameters(functionTemplate, functionTemplateInstance, templateArguments);
    Prebinder prebinder(boundCompileUnit.SymbolTable(), boundCompileUnit.ClassTemplateRepository(), boundCompileUnit);
    prebinder.BeginCompileUnit();
    globalNs->Accept(prebinder);
    functionTemplateInstance->SetTypeArguments(templateArguments);
    functionTemplateInstance->ComputeName();
    std::string constraintStr;
    if (functionNode->Constraint())
    {
        constraintStr = functionNode->Constraint()->ToString();
    }
    if (!constraintStr.empty())
    {
        functionTemplateInstance->SetName(functionTemplateInstance->Name() + " " + constraintStr);
    }
    prebinder.EndCompileUnit();
    if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::generate_docs))
    {
        if (functionNode->Constraint())
        {
            functionTemplateInstance->SetConstraintDocId(functionNode->Constraint()->DocId());
        }
    }
    else
    {
        Cm::Sym::FileScope* fileScope = prebinder.ReleaseFileScope();
        boundCompileUnit.AddFileScope(fileScope);
        Binder binder(boundCompileUnit);
        globalNs->Accept(binder);
        boundCompileUnit.RemoveLastFileScope();
    }
    functionTemplateInstance->SetGlobalNs(globalNs.release());
    return functionTemplateInstance;
}

} } // namespace Cm::Bind
