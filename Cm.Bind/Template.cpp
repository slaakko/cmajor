/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Template.hpp>
#include <Cm.Bind/Prebinder.hpp>
#include <Cm.Bind/Binder.hpp>
#include <Cm.Sym/TemplateParameterSymbol.hpp>
#include <Cm.Sym/DeclarationVisitor.hpp>

namespace Cm { namespace Bind {

Cm::Sym::FunctionSymbol* Instantiate(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::FunctionSymbol* function, 
    const std::vector<Cm::Sym::TypeSymbol*>& templateArguments)
{
    Cm::Ast::Node* body = function->Body();
    if (!body)
    {
        throw std::runtime_error("function template subject has no body");
    }
    Cm::Ast::FunctionNode* functionNode = static_cast<Cm::Ast::FunctionNode*>(boundCompileUnit.SymbolTable().GetNode(function));
    std::unique_ptr<Cm::Ast::FunctionNode> functionInstanceNode(new Cm::Ast::FunctionNode(function->GetSpan(), functionNode->GetSpecifiers(), functionNode->ReturnTypeExpr()->Clone(), 
        static_cast<Cm::Ast::FunctionGroupIdNode*>(functionNode->GroupId()->Clone())));
    functionInstanceNode->SetBody(static_cast<Cm::Ast::CompoundStatementNode*>(body->Clone()));
    for (const std::unique_ptr<Cm::Ast::ParameterNode>& parameterNode : functionNode->Parameters())
    {
        functionInstanceNode->AddParameter(static_cast<Cm::Ast::ParameterNode*>(parameterNode->Clone()));
    }
    Cm::Sym::DeclarationVisitor declarationVisitor(boundCompileUnit.SymbolTable());
    functionInstanceNode->Accept(declarationVisitor);
    Cm::Sym::FunctionSymbol* functionTemplateInstance = boundCompileUnit.SymbolTable().GetFunctionSymbol(functionInstanceNode.get());
    functionTemplateInstance->SetReplicated();
    functionTemplateInstance->SetFunctionTemplateSpecialization();
    int n = int(function->TemplateParameters().size());
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::TemplateParameterSymbol* templateParameter = function->TemplateParameters()[i];
        Cm::Sym::BoundTemplateParameterSymbol* boundTemplateParam = new Cm::Sym::BoundTemplateParameterSymbol(templateParameter->GetSpan(), templateParameter->Name());
        boundTemplateParam->SetType(templateArguments[i]);
        functionTemplateInstance->AddSymbol(boundTemplateParam);
    }
    Prebinder prebinder(boundCompileUnit.SymbolTable());
    functionInstanceNode->Accept(prebinder);
    Binder binder(boundCompileUnit);
    functionInstanceNode->Accept(binder);
    functionTemplateInstance->ComputeName();
    return functionTemplateInstance;
}

} } // namespace Cm::Bind