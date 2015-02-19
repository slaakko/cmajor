/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Binder.hpp>
#include <Cm.Bind/Class.hpp>
#include <Cm.Bind/Function.hpp>
#include <Cm.Bind/Constant.hpp>
#include <Cm.Bind/Enumeration.hpp>
#include <Cm.Bind/Parameter.hpp>
#include <Cm.Bind/Typedef.hpp>
#include <Cm.Bind/MemberVariable.hpp>
#include <Cm.Bind/LocalVariable.hpp>
#include <Cm.Bind/StatementBinder.hpp>
#include <Cm.Bind/SynthesizedClassFun.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/ClassObjectLayout.hpp>
#include <Cm.BoundTree/BoundClass.hpp>

namespace Cm { namespace Bind {

Binder::Binder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_) : Cm::Ast::Visitor(true, false), boundCompileUnit(boundCompileUnit_), currentContainerScope(nullptr), currentParent(nullptr),
    userMainFunction(nullptr), switchStatement(nullptr)
{
}

void Binder::BeginContainerScope(Cm::Sym::ContainerScope* containerScope)
{
    containerScopeStack.push(currentContainerScope);
    currentContainerScope = containerScope;
}

void Binder::EndContainerScope()
{
    currentContainerScope = containerScopeStack.top();
    containerScopeStack.pop();
}

void Binder::BeginVisit(Cm::Ast::NamespaceNode& namespaceNode)
{
    Cm::Sym::ContainerScope* containerScope = boundCompileUnit.SymbolTable().GetContainerScope(&namespaceNode);
    Cm::Sym::ContainerSymbol* container = containerScope->Container();
    BeginContainerScope(containerScope);
}

void Binder::EndVisit(Cm::Ast::NamespaceNode& namespaceNode)
{
    EndContainerScope();
}

void Binder::BeginVisit(Cm::Ast::ClassNode& classNode)
{
    if (classNode.TemplateParameters().Count() > 0)
    {
        PushSkipContent();
    }
    else
    {
        Cm::Sym::ContainerScope* containerScope = boundCompileUnit.SymbolTable().GetContainerScope(&classNode);
        Cm::Sym::ClassTypeSymbol* classTypeSymbol = containerScope->Class();
        boundCompileUnit.IrClassTypeRepository().AddClassType(classTypeSymbol);
        boundClass.reset(new Cm::BoundTree::BoundClass(classTypeSymbol, &classNode));
        BeginContainerScope(containerScope);
    }
}

void Binder::EndVisit(Cm::Ast::ClassNode& classNode)
{
    if (classNode.TemplateParameters().Count() > 0)
    {
        PopSkipContent();
    }
    else
    {
        EndContainerScope();
        Cm::Sym::ClassTypeSymbol* classTypeSymbol = boundClass->Symbol();
        if (!classTypeSymbol->HasUserDefinedDestructor())
        {
            if (classTypeSymbol->Destructor())
            {
                GenerateDestructorImplementation(classNode.GetSpan(), classTypeSymbol, currentContainerScope, boundCompileUnit);
            }
        }
        if (!classTypeSymbol->HasUserDefinedStaticConstructor())
        {
            if (classTypeSymbol->StaticConstructor())
            {
                GenerateStaticConstructorImplementation(boundClass.get(), currentContainerScope, classNode.GetSpan(), classTypeSymbol, boundCompileUnit);
            }
        }
        boundCompileUnit.AddBoundNode(boundClass.release());
    }
}

void Binder::BeginVisit(Cm::Ast::ConstructorNode& constructorNode)
{
    if ((constructorNode.GetSpecifiers() & (Cm::Ast::Specifiers::default_ | Cm::Ast::Specifiers::suppress)) == Cm::Ast::Specifiers::none)
    {
        Cm::Sym::FunctionSymbol* functionSymbol = boundCompileUnit.SymbolTable().GetFunctionSymbol(&constructorNode);
        BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&constructorNode));
        boundFunction.reset(new Cm::BoundTree::BoundFunction(&constructorNode, functionSymbol));
    }
}

void Binder::EndVisit(Cm::Ast::ConstructorNode& constructorNode)
{
    if ((constructorNode.GetSpecifiers() & Cm::Ast::Specifiers::default_) != Cm::Ast::Specifiers::none)
    {
        Cm::Sym::FunctionSymbol* functionSymbol = boundCompileUnit.SymbolTable().GetFunctionSymbol(&constructorNode);
        bool unique = !boundClass->Symbol()->IsTemplateTypeSymbol();
        GenerateSynthesizedFunctionImplementation(functionSymbol, constructorNode.GetSpan(), boundClass->Symbol(), currentContainerScope, boundCompileUnit, unique);
    }
    else if ((constructorNode.GetSpecifiers() & Cm::Ast::Specifiers::suppress) == Cm::Ast::Specifiers::none)
    {
        CheckFunctionAccessLevels(boundFunction->GetFunctionSymbol());
        if (boundFunction->Body())
        {
            GenerateReceives(currentContainerScope, boundCompileUnit, boundFunction.get());
            bool callToThisInitializerGenerated = false;
            GenerateClassInitStatement(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), boundClass->Symbol(), &constructorNode,
                callToThisInitializerGenerated);
            if (boundClass->Symbol()->IsVirtual() && !callToThisInitializerGenerated)
            {
                GenerateInitVPtrStatement(boundClass->Symbol(), boundFunction.get());
            }
            if (!callToThisInitializerGenerated)
            {
                GenerateMemberVariableInitStatements(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), boundClass->Symbol(), &constructorNode);
            }
            boundClass->AddBoundNode(boundFunction.release());
        }
        EndContainerScope();
    }
}

void Binder::BeginVisit(Cm::Ast::DestructorNode& destructorNode)
{
    if ((destructorNode.GetSpecifiers() & Cm::Ast::Specifiers::default_) == Cm::Ast::Specifiers::none)
    {
        Cm::Sym::FunctionSymbol* functionSymbol = boundCompileUnit.SymbolTable().GetFunctionSymbol(&destructorNode);
        BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&destructorNode));
        boundFunction.reset(new Cm::BoundTree::BoundFunction(&destructorNode, functionSymbol));
    }
}

void Binder::EndVisit(Cm::Ast::DestructorNode& destructorNode)
{
    if ((destructorNode.GetSpecifiers() & Cm::Ast::Specifiers::default_) == Cm::Ast::Specifiers::none)
    {
        CheckFunctionAccessLevels(boundFunction->GetFunctionSymbol());
        if (boundFunction->Body())
        {
            GenerateReceives(currentContainerScope, boundCompileUnit, boundFunction.get());
            if (boundClass->Symbol()->IsVirtual())
            {
                GenerateInitVPtrStatement(boundClass->Symbol(), boundFunction.get());
            }
            GenerateMemberVariableDestructionStatements(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), boundClass->Symbol(), &destructorNode);
            GenerateBaseClassDestructionStatement(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), boundClass->Symbol(), &destructorNode);
            boundClass->AddBoundNode(boundFunction.release());
        }
        EndContainerScope();
    }
}

void Binder::BeginVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode)
{
    if ((memberFunctionNode.GetSpecifiers() & (Cm::Ast::Specifiers::default_ | Cm::Ast::Specifiers::suppress)) == Cm::Ast::Specifiers::none)
    {
        Cm::Sym::FunctionSymbol* functionSymbol = boundCompileUnit.SymbolTable().GetFunctionSymbol(&memberFunctionNode);
        BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&memberFunctionNode));
        boundFunction.reset(new Cm::BoundTree::BoundFunction(&memberFunctionNode, functionSymbol));
    }
}

void Binder::EndVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode)
{
    if ((memberFunctionNode.GetSpecifiers() & Cm::Ast::Specifiers::default_) != Cm::Ast::Specifiers::none)
    {
        Cm::Sym::FunctionSymbol* functionSymbol = boundCompileUnit.SymbolTable().GetFunctionSymbol(&memberFunctionNode);
        bool unique = !boundClass->Symbol()->IsTemplateTypeSymbol();
        GenerateSynthesizedFunctionImplementation(functionSymbol, memberFunctionNode.GetSpan(), boundClass->Symbol(), currentContainerScope, boundCompileUnit, unique);
    }
    else if ((memberFunctionNode.GetSpecifiers() & Cm::Ast::Specifiers::suppress) == Cm::Ast::Specifiers::none)
    {
        if (!boundFunction->GetFunctionSymbol()->IsAbstract() && boundFunction->Body())
        {
            CheckFunctionReturnPaths(boundCompileUnit.SymbolTable(), currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction->GetFunctionSymbol(), &memberFunctionNode);
            CheckFunctionAccessLevels(boundFunction->GetFunctionSymbol());
            GenerateReceives(currentContainerScope, boundCompileUnit, boundFunction.get());
            if (boundFunction->GetFunctionSymbol()->IsStatic() && boundClass->Symbol()->StaticConstructor())
            {
                GenerateStaticConstructorCall(boundCompileUnit, boundFunction.get(), boundClass->Symbol(), &memberFunctionNode);
            }
            boundClass->AddBoundNode(boundFunction.release());
        }
        EndContainerScope(); 
    }
}

void Binder::BeginVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode)
{
    Cm::Sym::FunctionSymbol* functionSymbol = boundCompileUnit.SymbolTable().GetFunctionSymbol(&conversionFunctionNode);
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&conversionFunctionNode));
    boundFunction.reset(new Cm::BoundTree::BoundFunction(&conversionFunctionNode, functionSymbol));
}

void Binder::EndVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode)
{
    CheckFunctionReturnPaths(boundCompileUnit.SymbolTable(), currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction->GetFunctionSymbol(), &conversionFunctionNode);
    CheckFunctionAccessLevels(boundFunction->GetFunctionSymbol());
    if (boundFunction->Body())
    {
        GenerateReceives(currentContainerScope, boundCompileUnit, boundFunction.get());
        boundClass->AddBoundNode(boundFunction.release());
    }
    EndContainerScope();
}

void Binder::BeginVisit(Cm::Ast::StaticConstructorNode& staticConstructorNode)
{
    Cm::Sym::FunctionSymbol* functionSymbol = boundCompileUnit.SymbolTable().GetFunctionSymbol(&staticConstructorNode);
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&staticConstructorNode));
    boundFunction.reset(new Cm::BoundTree::BoundFunction(&staticConstructorNode, functionSymbol));
}

void Binder::EndVisit(Cm::Ast::StaticConstructorNode& staticConstructorNode)
{
    if (boundFunction->Body())
    {
        GenerateStaticInitStatement(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), boundClass->Symbol(), &staticConstructorNode);
        boundClass->AddBoundNode(boundFunction.release());
    }
    EndContainerScope();
}

void Binder::BeginVisit(Cm::Ast::FunctionNode& functionNode)
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
        if (functionSymbol->GroupName() == "main")
        {
            if (userMainFunction)
            {
                throw Cm::Core::Exception("already has main() function", functionNode.GetSpan(), userMainFunction->GetSpan());
            }
            else
            {
                userMainFunction = functionSymbol;
            }
        }
    }
}

void Binder::EndVisit(Cm::Ast::FunctionNode& functionNode)
{
    if (functionNode.TemplateParameters().Count() > 0)
    {
        PopSkipContent();
    }
    else
    {
        CheckFunctionReturnPaths(boundCompileUnit.SymbolTable(), currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction->GetFunctionSymbol(), &functionNode);
        CheckFunctionAccessLevels(boundFunction->GetFunctionSymbol());
        GenerateReceives(currentContainerScope, boundCompileUnit, boundFunction.get());
        boundCompileUnit.AddBoundNode(boundFunction.release());
        EndContainerScope();
    }
}

void Binder::BeginVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&compoundStatementNode));
    parentStack.push(currentParent.release());
    currentParent.reset(new Cm::BoundTree::BoundCompoundStatement(&compoundStatementNode));
}

void Binder::EndVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode)
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

void Binder::BeginVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&rangeForStatementNode));
    // todo
}

void Binder::EndVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode)
{
    EndContainerScope();
}

void Binder::BeginVisit(Cm::Ast::ForStatementNode& forStatementNode)
{
    parentStack.push(currentParent.release());
    currentParent.reset(new Cm::BoundTree::BoundForStatement(&forStatementNode));
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&forStatementNode));
}

void Binder::EndVisit(Cm::Ast::ForStatementNode& forStatementNode)
{
    Cm::BoundTree::BoundParentStatement* cp = currentParent.release();
    if (cp->IsBoundForStatement())
    {
        Cm::BoundTree::BoundForStatement* forStatement = static_cast<Cm::BoundTree::BoundForStatement*>(cp);
        ForStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), forStatement);
        forStatementNode.Accept(binder);
        Cm::BoundTree::BoundParentStatement* parent = parentStack.top();
        parentStack.pop();
        if (parent)
        {
            parent->AddStatement(forStatement);
            currentParent.reset(parent);
        }
        else
        {
            throw std::runtime_error("no parent");
        }
    }
    else
    {
        throw std::runtime_error("not a for statement");
    }
    EndContainerScope();
}

void Binder::BeginVisit(Cm::Ast::ReturnStatementNode& returnStatementNode)
{
    ReturnStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get());
    returnStatementNode.Accept(binder);
    currentParent->AddStatement(binder.Result());
}

void Binder::BeginVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode)
{
    parentStack.push(currentParent.release());
    currentParent.reset(new Cm::BoundTree::BoundConditionalStatement(&conditionalStatementNode));
}

void Binder::EndVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode)
{
    Cm::BoundTree::BoundParentStatement* cp = currentParent.release();
    if (cp->IsBoundConditionalStatement())
    {
        Cm::BoundTree::BoundConditionalStatement* conditionalStatement = static_cast<Cm::BoundTree::BoundConditionalStatement*>(cp);
        ConditionalStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), conditionalStatement);
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

void Binder::BeginVisit(Cm::Ast::SwitchStatementNode& switchStatementNode)
{
    parentStack.push(currentParent.release());
    switchStatement = new Cm::BoundTree::BoundSwitchStatement(&switchStatementNode);
    SwitchStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), switchStatement);
    switchStatementNode.Accept(binder);
    currentParent.reset(switchStatement);
}

void Binder::EndVisit(Cm::Ast::SwitchStatementNode& switchStatementNode)
{
    Cm::BoundTree::BoundParentStatement* cp = currentParent.release();
    if (cp->IsBoundSwitchStatement())
    {
        Cm::BoundTree::BoundSwitchStatement* switchStatement = static_cast<Cm::BoundTree::BoundSwitchStatement*>(cp);
        Cm::BoundTree::BoundParentStatement* parent = parentStack.top();
        parentStack.pop();
        if (parent)
        {
            parent->AddStatement(switchStatement);
            currentParent.reset(parent);
        }
        else
        {
            throw std::runtime_error("no parent");
        }
    }
    else
    {
        throw std::runtime_error("not a switch statement");
    }
    switchStatement = nullptr;
}

void Binder::BeginVisit(Cm::Ast::CaseStatementNode& caseStatementNode)
{
    parentStack.push(currentParent.release());
    currentParent.reset(new Cm::BoundTree::BoundCaseStatement(&caseStatementNode));
    PushSkipContent();
}

void Binder::EndVisit(Cm::Ast::CaseStatementNode& caseStatementNode)
{
    PopSkipContent();
    Cm::BoundTree::BoundParentStatement* cp = currentParent.release();
    if (cp->IsBoundCaseStatement())
    {
        Cm::BoundTree::BoundCaseStatement* caseStatement = static_cast<Cm::BoundTree::BoundCaseStatement*>(cp);
        CaseStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), caseStatement, switchStatement);
        caseStatementNode.Accept(binder);
        Cm::BoundTree::BoundParentStatement* parent = parentStack.top();
        parentStack.pop();
        if (parent)
        {
            parent->AddStatement(caseStatement);
            currentParent.reset(parent);
        }
        else
        {
            throw std::runtime_error("no parent");
        }
    }
    else
    {
        throw std::runtime_error("not a case statement");
    }
}

void Binder::BeginVisit(Cm::Ast::DefaultStatementNode& defaultStatementNode)
{
    parentStack.push(currentParent.release());
    currentParent.reset(new Cm::BoundTree::BoundDefaultStatement(&defaultStatementNode));
}

void Binder::EndVisit(Cm::Ast::DefaultStatementNode& defaultStatementNode)
{
    Cm::BoundTree::BoundParentStatement* cp = currentParent.release();
    if (cp->IsBoundDefaultStatement())
    {
        Cm::BoundTree::BoundDefaultStatement* defaultStatement = static_cast<Cm::BoundTree::BoundDefaultStatement*>(cp);
        DefaultStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), defaultStatement);
        defaultStatementNode.Accept(binder);
        Cm::BoundTree::BoundParentStatement* parent = parentStack.top();
        parentStack.pop();
        if (parent)
        {
            parent->AddStatement(defaultStatement);
            currentParent.reset(parent);
        }
        else
        {
            throw std::runtime_error("no parent");
        }
    }
    else
    {
        throw std::runtime_error("not a default statement");
    }
}

void Binder::BeginVisit(Cm::Ast::GotoCaseStatementNode& gotoCaseStatementNode)
{
}

void Binder::EndVisit(Cm::Ast::GotoCaseStatementNode& gotoCaseStatementNode)
{
    GotoCaseStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), switchStatement);
    gotoCaseStatementNode.Accept(binder);
    currentParent->AddStatement(binder.Result());
}

void Binder::Visit(Cm::Ast::GotoDefaultStatementNode& gotoDefaultStatementNode)
{
    GotoDefaultStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get());
    gotoDefaultStatementNode.Accept(binder);
    currentParent->AddStatement(binder.Result());
}

void Binder::BeginVisit(Cm::Ast::WhileStatementNode& whileStatementNode)
{
    parentStack.push(currentParent.release());
    currentParent.reset(new Cm::BoundTree::BoundWhileStatement(&whileStatementNode));
}

void Binder::EndVisit(Cm::Ast::WhileStatementNode& whileStatementNode)
{
    Cm::BoundTree::BoundParentStatement* cp = currentParent.release();
    if (cp->IsBoundWhileStatement())
    {
        Cm::BoundTree::BoundWhileStatement* whileStatement = static_cast<Cm::BoundTree::BoundWhileStatement*>(cp);
        WhileStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), whileStatement);
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

void Binder::BeginVisit(Cm::Ast::DoStatementNode& doStatementNode)
{
    parentStack.push(currentParent.release());
    currentParent.reset(new Cm::BoundTree::BoundDoStatement(&doStatementNode));
}

void Binder::EndVisit(Cm::Ast::DoStatementNode& doStatementNode)
{
    Cm::BoundTree::BoundParentStatement* cp = currentParent.release();
    if (cp->IsBoundDoStatement())
    {
        Cm::BoundTree::BoundDoStatement* doStatement = static_cast<Cm::BoundTree::BoundDoStatement*>(cp);
        DoStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), doStatement);
        doStatementNode.Accept(binder);
        Cm::BoundTree::BoundParentStatement* parent = parentStack.top();
        parentStack.pop();
        if (parent)
        {
            parent->AddStatement(doStatement);
            currentParent.reset(parent);
        }
        else
        {
            throw std::runtime_error("no parent");
        }
    }
    else
    {
        throw std::runtime_error("not a do statement");
    }
}

void Binder::Visit(Cm::Ast::BreakStatementNode& breakStatementNode)
{
    BreakStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get());
    breakStatementNode.Accept(binder);
    currentParent->AddStatement(binder.Result());
}

void Binder::Visit(Cm::Ast::ContinueStatementNode& continueStatementNode)
{
    ContinueStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get());
    continueStatementNode.Accept(binder);
    currentParent->AddStatement(binder.Result());
}

void Binder::Visit(Cm::Ast::GotoStatementNode& gotoStatementNode)
{
}

void Binder::Visit(Cm::Ast::TypedefStatementNode& typedefStatementNode)
{
}

void Binder::BeginVisit(Cm::Ast::SimpleStatementNode& simpleStatementNode)
{
    SimpleStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get());
    simpleStatementNode.Accept(binder);
    currentParent->AddStatement(binder.Result());
}

void Binder::EndVisit(Cm::Ast::SimpleStatementNode& simpleStatementNode)
{
}

void Binder::BeginVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode)
{
    AssignmentStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get());
    assignmentStatementNode.Accept(binder);
    currentParent->AddStatement(binder.Result());
}

void Binder::BeginVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
    ConstructionStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get());
    constructionStatementNode.Accept(binder);
    currentParent->AddStatement(binder.Result());
}

void Binder::BeginVisit(Cm::Ast::DeleteStatementNode& deleteStatementNode)
{
    DeleteStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get());
    deleteStatementNode.Accept(binder);
    if (binder.Result())
    {
        currentParent->AddStatement(binder.Result());
    }
    Cm::BoundTree::BoundStatement* freeStatement = binder.GetFreeStatement();
    currentParent->AddStatement(freeStatement);
}

void Binder::BeginVisit(Cm::Ast::DestroyStatementNode& destroyStatementNode)
{
    DestroyStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get());
    destroyStatementNode.Accept(binder);
    if (binder.Result())
    {
        currentParent->AddStatement(binder.Result());
    }
}

void Binder::BeginVisit(Cm::Ast::ThrowStatementNode& throwStatementNode)
{
}

void Binder::EndVisit(Cm::Ast::ThrowStatementNode& throwStatementNode)
{
}

void Binder::BeginVisit(Cm::Ast::TryStatementNode& tryStatementNode)
{
}

void Binder::EndVisit(Cm::Ast::TryStatementNode& tryStatementNode)
{
}

void Binder::BeginVisit(Cm::Ast::CatchNode& catchNode)
{

}

void Binder::EndVisit(Cm::Ast::CatchNode& catchNode)
{

}

void Binder::BeginVisit(Cm::Ast::AssertStatementNode& assertStatementNode)
{
}

void Binder::EndVisit(Cm::Ast::AssertStatementNode& assertStatementNode)
{
}

void Binder::BeginVisit(Cm::Ast::CondCompDisjunctionNode& condCompDisjunctionNode)
{
}

void Binder::EndVisit(Cm::Ast::CondCompDisjunctionNode& condCompDisjunctionNode)
{
}

void Binder::BeginVisit(Cm::Ast::CondCompConjunctionNode& condCompDisjunctionNode)
{
}

void Binder::EndVisit(Cm::Ast::CondCompConjunctionNode& condCompDisjunctionNode)
{
}

void Binder::BeginVisit(Cm::Ast::CondCompNotNode& condCompNotNode)
{
}

void Binder::EndVisit(Cm::Ast::CondCompNotNode& condCompNotNode)
{
}

void Binder::Visit(Cm::Ast::CondCompPrimaryNode& condCompPrimaryNode)
{
}

void Binder::BeginVisit(Cm::Ast::CondCompilationPartNode& condCompilationPartNode)
{
}

void Binder::EndVisit(Cm::Ast::CondCompilationPartNode& condCompilationPartNode)
{
}

void Binder::BeginVisit(Cm::Ast::CondCompStatementNode& condCompStatementNode)
{
}

void Binder::EndVisit(Cm::Ast::CondCompStatementNode& condCompStatementNode)
{
}

} } // namespace Cm::Bind
