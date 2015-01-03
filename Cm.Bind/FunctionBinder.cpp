/*========================================================================
Copyright (c) 2012-2015 Seppo Laakko
http://sourceforge.net/projects/cmajor/

Distributed under the GNU General Public License, version 3 (GPLv3).
(See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/FunctionBinder.hpp>
#include <Cm.Bind/Class.hpp>
#include <Cm.Bind/Function.hpp>
#include <Cm.Bind/Constant.hpp>
#include <Cm.Bind/Enumeration.hpp>
#include <Cm.Bind/Parameter.hpp>
#include <Cm.Bind/Typedef.hpp>
#include <Cm.Bind/MemberVariable.hpp>
#include <Cm.Bind/LocalVariable.hpp>
#include <Cm.Bind/StatementBinder.hpp>
#include <Cm.BoundTree/BoundClass.hpp>

namespace Cm { namespace Bind {

FunctionBinder::FunctionBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_) : Cm::Ast::Visitor(true, false), boundCompileUnit(boundCompileUnit_), currentContainerScope(nullptr), currentParent(nullptr)
{
}

void FunctionBinder::BeginContainerScope(Cm::Sym::ContainerScope* containerScope)
{
    containerScopeStack.push(currentContainerScope);
    currentContainerScope = containerScope;
}

void FunctionBinder::EndContainerScope()
{
    currentContainerScope = containerScopeStack.top();
    containerScopeStack.pop();
}

void FunctionBinder::BeginVisit(Cm::Ast::NamespaceNode& namespaceNode)
{
    Cm::Sym::ContainerScope* containerScope = boundCompileUnit.SymbolTable().GetContainerScope(&namespaceNode);
    Cm::Sym::ContainerSymbol* container = containerScope->Container();
    BeginContainerScope(containerScope);
}

void FunctionBinder::EndVisit(Cm::Ast::NamespaceNode& namespaceNode)
{
    EndContainerScope();
}

void FunctionBinder::BeginVisit(Cm::Ast::ClassNode& classNode)
{
    if (classNode.TemplateParameters().Count() > 0)
    {
        PushSkipContent();
    }
    else
    {
        Cm::Sym::ContainerScope* containerScope = boundCompileUnit.SymbolTable().GetContainerScope(&classNode);
        BeginContainerScope(containerScope);
    }
}

void FunctionBinder::EndVisit(Cm::Ast::ClassNode& classNode)
{
    if (classNode.TemplateParameters().Count() > 0)
    {
        PopSkipContent();
    }
    else
    {
        EndContainerScope();

    }
}

void FunctionBinder::BeginVisit(Cm::Ast::ConstructorNode& constructorNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&constructorNode));
}

void FunctionBinder::EndVisit(Cm::Ast::ConstructorNode& constructorNode)
{
    EndContainerScope();
}

void FunctionBinder::BeginVisit(Cm::Ast::DestructorNode& destructorNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&destructorNode));
}

void FunctionBinder::EndVisit(Cm::Ast::DestructorNode& destructorNode)
{
    EndContainerScope();
}

void FunctionBinder::BeginVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&memberFunctionNode));
}

void FunctionBinder::EndVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode)
{
    EndContainerScope();
}

void FunctionBinder::BeginVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&conversionFunctionNode));
}

void FunctionBinder::EndVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode)
{
    EndContainerScope();
}

void FunctionBinder::BeginVisit(Cm::Ast::FunctionNode& functionNode)
{
    if (functionNode.TemplateParameters().Count() > 0)
    {
        PushSkipContent();
    }
    else
    {
        Cm::Sym::FunctionSymbol* functionSymbol = boundCompileUnit.SymbolTable().GetFunctionSymbol(&functionNode);
        BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&functionNode));
        boundFunction.reset(new Cm::BoundTree::BoundFunction(&functionNode, functionSymbol));
    }
}

void FunctionBinder::EndVisit(Cm::Ast::FunctionNode& functionNode)
{
    if (functionNode.TemplateParameters().Count() > 0)
    {
        PopSkipContent();
    }
    else
    {
        EndContainerScope();
    }
    CheckFunctionAccessLevels(boundFunction->GetFunctionSymbol());
    GenerateReceives(boundCompileUnit.SymbolTable(), boundCompileUnit.ConversionTable(), boundCompileUnit.ClassConversionTable(), boundCompileUnit.PointerOpRepository(), boundFunction.get());
    boundCompileUnit.AddBoundNode(boundFunction.release());
}

void FunctionBinder::BeginVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&compoundStatementNode));
    parentStack.push(currentParent.release());
    currentParent.reset(new Cm::BoundTree::BoundCompoundStatement(&compoundStatementNode));
}

void FunctionBinder::EndVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode)
{
    Cm::BoundTree::BoundParentStatement* parent = parentStack.top();
    parentStack.pop();
    if (parent)
    {
        parent->AddStatement(currentParent.release());
        currentParent.reset(parent);
    }
    else
    {
        if (currentParent->IsBoundCompoundStatement())
        {
            Cm::BoundTree::BoundCompoundStatement* compound = static_cast<Cm::BoundTree::BoundCompoundStatement*>(currentParent.release());
            boundFunction->SetBody(compound);
        }
        else
        {
            throw std::runtime_error("current parent is not compound statement");
        }
    }
    EndContainerScope();
}

void FunctionBinder::BeginVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&rangeForStatementNode));
}

void FunctionBinder::EndVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode)
{
    EndContainerScope();
}

void FunctionBinder::BeginVisit(Cm::Ast::ForStatementNode& forStatementNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&forStatementNode));
}

void FunctionBinder::EndVisit(Cm::Ast::ForStatementNode& forStatementNode)
{
    EndContainerScope();
}

void FunctionBinder::BeginVisit(Cm::Ast::ReturnStatementNode& returnStatementNode)
{
    ReturnStatementBinder binder(boundCompileUnit.SymbolTable(), boundCompileUnit.ConversionTable(), boundCompileUnit.ClassConversionTable(), boundCompileUnit.PointerOpRepository(),
        currentContainerScope, &boundCompileUnit.GetFileScope(), boundFunction.get());
    returnStatementNode.Accept(binder);
    currentParent->AddStatement(binder.Result());
}

void FunctionBinder::EndVisit(Cm::Ast::ReturnStatementNode& returnStatementNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode)
{
    parentStack.push(currentParent.release());
    currentParent.reset(new Cm::BoundTree::BoundConditionalStatement(&conditionalStatementNode));
}

void FunctionBinder::EndVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode)
{
    Cm::BoundTree::BoundParentStatement* cp = currentParent.release();
    if (cp->IsBoundConditionalStatement())
    {
        Cm::BoundTree::BoundConditionalStatement* conditionalStatement = static_cast<Cm::BoundTree::BoundConditionalStatement*>(cp);
        ConditionalStatementBinder binder(boundCompileUnit.SymbolTable(), boundCompileUnit.ConversionTable(), boundCompileUnit.ClassConversionTable(), boundCompileUnit.PointerOpRepository(),
            currentContainerScope, &boundCompileUnit.GetFileScope(), boundFunction.get(), conditionalStatement);
        conditionalStatementNode.Accept(binder);
        Cm::BoundTree::BoundParentStatement* parent = parentStack.top();
        parentStack.pop();
        if (parent)
        {
            parent->AddStatement(conditionalStatement);
            currentParent.reset(parent);
        }
        else
        {
            throw std::runtime_error("no parent");
        }
    }
    else
    {
        throw std::runtime_error("not a conditional statement");
    }
}

void FunctionBinder::BeginVisit(Cm::Ast::SwitchStatementNode& switchStatementNode)
{
}

void FunctionBinder::EndVisit(Cm::Ast::SwitchStatementNode& switchStatementNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::CaseStatementNode& caseStatementNode)
{
}

void FunctionBinder::EndVisit(Cm::Ast::CaseStatementNode& caseStatementNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::DefaultStatementNode& defaultStatementNode)
{
}

void FunctionBinder::EndVisit(Cm::Ast::DefaultStatementNode& defaultStatementNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::GotoCaseStatementNode& gotoCaseStatementNode)
{
}

void FunctionBinder::EndVisit(Cm::Ast::GotoCaseStatementNode& gotoCaseStatementNode)
{
}

void FunctionBinder::Visit(Cm::Ast::GotoDefaultStatementNode& gotoDefaultStatementNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::WhileStatementNode& whileStatementNode)
{
    parentStack.push(currentParent.release());
    currentParent.reset(new Cm::BoundTree::BoundWhileStatement(&whileStatementNode));
}

void FunctionBinder::EndVisit(Cm::Ast::WhileStatementNode& whileStatementNode)
{
    Cm::BoundTree::BoundParentStatement* cp = currentParent.release();
    if (cp->IsBoundWhileStatement())
    {
        Cm::BoundTree::BoundWhileStatement* whileStatement = static_cast<Cm::BoundTree::BoundWhileStatement*>(cp);
        WhileStatementBinder binder(boundCompileUnit.SymbolTable(), boundCompileUnit.ConversionTable(), boundCompileUnit.ClassConversionTable(), boundCompileUnit.PointerOpRepository(),
            currentContainerScope, &boundCompileUnit.GetFileScope(), boundFunction.get(), whileStatement);
        whileStatementNode.Accept(binder);
        Cm::BoundTree::BoundParentStatement* parent = parentStack.top();
        parentStack.pop();
        if (parent)
        {
            parent->AddStatement(whileStatement);
            currentParent.reset(parent);
        }
        else
        {
            throw std::runtime_error("no parent");
        }
    }
    else
    {
        throw std::runtime_error("not a while statement");
    }
}

void FunctionBinder::BeginVisit(Cm::Ast::DoStatementNode& doStatementNode)
{
}

void FunctionBinder::EndVisit(Cm::Ast::DoStatementNode& doStatementNode)
{
}

void FunctionBinder::Visit(Cm::Ast::BreakStatementNode& breakStatementNode)
{
}

void FunctionBinder::Visit(Cm::Ast::ContinueStatementNode& continueStatementNode)
{
}

void FunctionBinder::Visit(Cm::Ast::GotoStatementNode& gotoStatementNode)
{
}

void FunctionBinder::Visit(Cm::Ast::TypedefStatementNode& typedefStatementNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::SimpleStatementNode& simpleStatementNode)
{
    SimpleStatementBinder binder(boundCompileUnit.SymbolTable(), boundCompileUnit.ConversionTable(), boundCompileUnit.ClassConversionTable(), boundCompileUnit.PointerOpRepository(),
        currentContainerScope, &boundCompileUnit.GetFileScope(), boundFunction.get());
    simpleStatementNode.Accept(binder);
    currentParent->AddStatement(binder.Result());
}

void FunctionBinder::EndVisit(Cm::Ast::SimpleStatementNode& simpleStatementNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode)
{
    AssignmentStatementBinder binder(boundCompileUnit.SymbolTable(), boundCompileUnit.ConversionTable(), boundCompileUnit.ClassConversionTable(), boundCompileUnit.PointerOpRepository(),
        currentContainerScope, &boundCompileUnit.GetFileScope(), boundFunction.get());
    assignmentStatementNode.Accept(binder);
    currentParent->AddStatement(binder.Result());
}

void FunctionBinder::EndVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
    ConstructionStatementBinder binder(boundCompileUnit.SymbolTable(), boundCompileUnit.ConversionTable(), boundCompileUnit.ClassConversionTable(), boundCompileUnit.PointerOpRepository(),
        currentContainerScope, &boundCompileUnit.GetFileScope(), boundFunction.get());
    constructionStatementNode.Accept(binder);
    currentParent->AddStatement(binder.Result());
}

void FunctionBinder::EndVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::DeleteStatementNode& deleteStatementNode)
{
}

void FunctionBinder::EndVisit(Cm::Ast::DeleteStatementNode& deleteStatementNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::DestroyStatementNode& destroyStatementNode)
{
}

void FunctionBinder::EndVisit(Cm::Ast::DestroyStatementNode& destroyStatementNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::ThrowStatementNode& throwStatementNode)
{
}

void FunctionBinder::EndVisit(Cm::Ast::ThrowStatementNode& throwStatementNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::TryStatementNode& tryStatementNode)
{
}

void FunctionBinder::EndVisit(Cm::Ast::TryStatementNode& tryStatementNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::CatchNode& catchNode)
{

}

void FunctionBinder::EndVisit(Cm::Ast::CatchNode& catchNode)
{

}

void FunctionBinder::BeginVisit(Cm::Ast::AssertStatementNode& assertStatementNode)
{
}

void FunctionBinder::EndVisit(Cm::Ast::AssertStatementNode& assertStatementNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::CondCompDisjunctionNode& condCompDisjunctionNode)
{
}

void FunctionBinder::EndVisit(Cm::Ast::CondCompDisjunctionNode& condCompDisjunctionNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::CondCompConjunctionNode& condCompDisjunctionNode)
{
}

void FunctionBinder::EndVisit(Cm::Ast::CondCompConjunctionNode& condCompDisjunctionNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::CondCompNotNode& condCompNotNode)
{
}

void FunctionBinder::EndVisit(Cm::Ast::CondCompNotNode& condCompNotNode)
{
}

void FunctionBinder::Visit(Cm::Ast::CondCompPrimaryNode& condCompPrimaryNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::CondCompilationPartNode& condCompilationPartNode)
{
}

void FunctionBinder::EndVisit(Cm::Ast::CondCompilationPartNode& condCompilationPartNode)
{
}

void FunctionBinder::BeginVisit(Cm::Ast::CondCompStatementNode& condCompStatementNode)
{
}

void FunctionBinder::EndVisit(Cm::Ast::CondCompStatementNode& condCompStatementNode)
{
}

} } // namespace Cm::Bind
