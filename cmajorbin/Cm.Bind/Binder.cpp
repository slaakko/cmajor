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
#include <Cm.Bind/ArrayTypeOpRepository.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Core/GlobalSettings.hpp>
#include <Cm.Bind/ClassObjectLayout.hpp>
#include <Cm.BoundTree/BoundClass.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/Conditional.hpp>
#include <Cm.Sym/Warning.hpp>
#include <iostream>

namespace Cm { namespace Bind {

Binder::Binder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_) : Cm::Ast::Visitor(true, false), boundCompileUnit(boundCompileUnit_), currentContainerScope(nullptr), currentParent(nullptr),
    switchStatement(nullptr), isRvalueArrayFun(false)
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

void Binder::AddBoundStatement(Cm::BoundTree::BoundStatement* boundStatement)
{
    if (!currentParent)
    {
        throw std::runtime_error("binder has no current parent");
    }
    currentParent->AddStatement(boundStatement);
}

Cm::BoundTree::BoundCompoundStatement* Binder::GetCurrentCompound()
{
    Cm::BoundTree::BoundParentStatement* parent = currentParent.get();
    if (parent->IsBoundCompoundStatement())
    {
        return static_cast<Cm::BoundTree::BoundCompoundStatement*>(parent);
    }
    else
    {
        return parent->CompoundParent();
    }
}

void Binder::SetCurrentFunction(Cm::BoundTree::BoundFunction* function)
{
    boundFunction.reset(function);
}

Cm::BoundTree::BoundFunction* Binder::ReleaseCurrentFunction()
{
    return boundFunction.release();
}

void Binder::SetCurrentParent(Cm::BoundTree::BoundParentStatement* parent)
{
    currentParent.reset(parent);
}

Cm::BoundTree::BoundCompoundStatement* Binder::ReleaseCurrentCompound()
{
    Cm::BoundTree::BoundParentStatement* parent = currentParent.release();
    if (!parent->IsBoundCompoundStatement())
    {
        throw std::runtime_error("not compound");
    }
    return static_cast<Cm::BoundTree::BoundCompoundStatement*>(parent);
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
        if (!boundCompileUnit.IsPrebindCompileUnit())
        {
            AddClassTypeToIrClassTypeRepository(classTypeSymbol, boundCompileUnit, currentContainerScope);
        }
        boundClassStack.push(std::move(boundClass));
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
            for (Cm::Sym::MemberVariableSymbol* memberVar : classTypeSymbol->MemberVariables())
            {
                if (!memberVar->GetType()->Bound() && memberVar->GetType()->IsTemplateTypeSymbol())
                {
                    Cm::Sym::TemplateTypeSymbol* templateTypeSymbol = static_cast<Cm::Sym::TemplateTypeSymbol*>(memberVar->GetType());
                    boundCompileUnit.ClassTemplateRepository().BindTemplateTypeSymbol(templateTypeSymbol, currentContainerScope, boundCompileUnit.GetFileScopes());
                }
            }
            if (classTypeSymbol->DoGenerateDestructor())
            {
                Cm::Sym::FunctionSymbol* destructor = GenerateDestructorSymbol(boundCompileUnit.SymbolTable(), classNode.GetSpan(), classTypeSymbol, boundCompileUnit.SyntaxUnit());
                destructor->SetPublic();
                classTypeSymbol->AddSymbol(destructor);
            }
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
        boundClass = std::move(boundClassStack.top());
        boundClassStack.pop();
    }
}

void Binder::BeginClass(Cm::Sym::ClassTypeSymbol* classTypeSymbol)
{
    if (!boundCompileUnit.IsPrebindCompileUnit())
    {
        AddClassTypeToIrClassTypeRepository(classTypeSymbol, boundCompileUnit, currentContainerScope);
    }
    boundClassStack.push(std::move(boundClass));
    boundClass.reset(new Cm::BoundTree::BoundClass(classTypeSymbol, nullptr));
}

void Binder::EndClass()
{
    boundCompileUnit.AddBoundNode(boundClass.release());
    boundClass = std::move(boundClassStack.top());
    boundClassStack.pop();
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
        if (!functionSymbol->IsVirtualAbstractOrOverride() && !functionSymbol->IsNew())
        {
            Cm::Sym::ClassTypeSymbol* ownerClass = boundClass->Symbol();
            if (ownerClass && ownerClass->BaseClass())
            {
                Cm::Sym::ClassTypeSymbol* baseClass = ownerClass->BaseClass();
                for (Cm::Sym::FunctionSymbol* f : baseClass->Vtbl())
                {
                    if (f && f->IsVirtualAbstractOrOverride())
                    {
                        if (Cm::Sym::Overrides(functionSymbol, f))
                        {
                            std::string warningMessage = "function '" + functionSymbol->FullName() + "' hides base class virtual function '" + f->FullName() + "'. " +
                                "To get rid of this warning declare the function either 'override' or 'new'.";
                            std::cout << "warning: " << warningMessage << std::endl;
                            Cm::Sym::Warning warning(Cm::Sym::CompileWarningCollection::Instance().GetCurrentProjectName(), warningMessage);
                            warning.SetDefined(functionSymbol->GetSpan());
                            warning.SetReferenced(f->GetSpan());
                            Cm::Sym::CompileWarningCollection::Instance().AddWarning(warning);
                        }
                    }
                }
            }
        }
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
            CheckFunctionReturnPaths(boundCompileUnit.SymbolTable(), currentContainerScope, boundCompileUnit.GetFileScopes(), boundCompileUnit.ClassTemplateRepository(), 
                boundFunction->GetFunctionSymbol(), &memberFunctionNode);
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
    CheckFunctionReturnPaths(boundCompileUnit.SymbolTable(), currentContainerScope, boundCompileUnit.GetFileScopes(), boundCompileUnit.ClassTemplateRepository(), boundFunction->GetFunctionSymbol(), 
        &conversionFunctionNode);
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
    }
}

void Binder::EndVisit(Cm::Ast::FunctionNode& functionNode)
{
    if (functionNode.TemplateParameters().Count() > 0)
    {
        PopSkipContent();
    }
    else if (isRvalueArrayFun)
    {
        PopSkipContent();
    }
    else
    {
        CheckFunctionReturnPaths(boundCompileUnit.SymbolTable(), currentContainerScope, boundCompileUnit.GetFileScopes(), boundCompileUnit.ClassTemplateRepository(), boundFunction->GetFunctionSymbol(), 
            &functionNode);
        CheckFunctionAccessLevels(boundFunction->GetFunctionSymbol());
        GenerateReceives(currentContainerScope, boundCompileUnit, boundFunction.get());
        boundCompileUnit.AddBoundNode(boundFunction.release());
        EndContainerScope();
    }
}

void Binder::BeginVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode)
{
    BeginContainerScope(boundCompileUnit.SymbolTable().GetContainerScope(&compoundStatementNode));
    bool functionBlock = currentParent == nullptr;
    parentStack.push(currentParent.release());
    currentParent.reset(new Cm::BoundTree::BoundCompoundStatement(&compoundStatementNode));
    if (functionBlock)
    {
        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::trace))
        {
            Cm::BoundTree::BoundStatement* tracedFunConstructionStatement = CreateTracedFunConstructionStatement(boundCompileUnit, currentContainerScope, boundFunction.get(),
                compoundStatementNode.GetSpan());
            if (tracedFunConstructionStatement)
            {
                int classObjectLayoutFunIndex = boundFunction->GetClassObjectLayoutFunIndex();
                Cm::BoundTree::BoundCompoundStatement* functionCompound = static_cast<Cm::BoundTree::BoundCompoundStatement*>(currentParent.get());
                functionCompound->InsertStatement(classObjectLayoutFunIndex, tracedFunConstructionStatement);
                ++classObjectLayoutFunIndex;
                boundFunction->SetClassObjectLayoutFunIndex(classObjectLayoutFunIndex);
            }
        }
    }
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
    RangeForStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), rangeForStatementNode, *this);
    rangeForStatementNode.Accept(binder);
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
}

void Binder::EndVisit(Cm::Ast::CaseStatementNode& caseStatementNode)
{
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
    boundFunction->SetHasGotos();
    boundCompileUnit.SetHasGotos();
    Cm::BoundTree::BoundGotoStatement* boundGotoStatement = new Cm::BoundTree::BoundGotoStatement(&gotoStatementNode, gotoStatementNode.Target()->Label());
    if (gotoStatementNode.IsExceptionHandlingGoto())
    {
        boundGotoStatement->SetExceptionHandlingGoto();
    }
    currentParent->AddStatement(boundGotoStatement);
}

void Binder::BeginVisit(Cm::Ast::SimpleStatementNode& simpleStatementNode)
{
    SimpleStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get());
    simpleStatementNode.Accept(binder);
    currentParent->AddStatement(binder.Result());
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
    ThrowStatementBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), *this);
    throwStatementNode.Accept(binder);
}

void Binder::Visit(Cm::Ast::TryStatementNode& tryStatementNode)
{
    TryBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), *this);
    tryStatementNode.Accept(binder);
}

void Binder::Visit(Cm::Ast::CatchNode& catchNode)
{
    CatchBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), *this);
    catchNode.Accept(binder);
}

void Binder::Visit(Cm::Ast::ExitTryStatementNode& exitTryStatementNode)
{
    ExitTryBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get(), *this);
    exitTryStatementNode.Accept(binder);
}

void Binder::Visit(Cm::Ast::BeginCatchStatementNode& beginCatchStatementNode)
{
    AddBoundStatement(new Cm::BoundTree::BoundBeginCatchStatement(&beginCatchStatementNode));
}

void Binder::Visit(Cm::Ast::AssertStatementNode& assertStatementNode)
{
    if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::unit_test))
    {
        UnitTestAssertBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get());
        assertStatementNode.Accept(binder);
        currentParent->AddStatement(binder.Result());
    }
    else if (Cm::Core::GetGlobalSettings()->Config() == "debug")
    {
        AssertBinder binder(boundCompileUnit, currentContainerScope, boundCompileUnit.GetFileScopes(), boundFunction.get());
        assertStatementNode.Accept(binder);
        currentParent->AddStatement(binder.Result());
    }
}

void Binder::EndVisit(Cm::Ast::CondCompDisjunctionNode& condCompDisjunctionNode)
{
    bool right = PopCondCompValue();
    bool left = PopCondCompValue();
    PushCondCompValue(left || right);
}

void Binder::EndVisit(Cm::Ast::CondCompConjunctionNode& condCompConjunctionNode)
{
    bool right = PopCondCompValue();
    bool left = PopCondCompValue();
    PushCondCompValue(left && right);
}

void Binder::EndVisit(Cm::Ast::CondCompNotNode& condCompNotNode)
{
    bool value = PopCondCompValue();
    PushCondCompValue(!value);
}

void Binder::Visit(Cm::Ast::CondCompPrimaryNode& condCompPrimaryNode)
{
    bool defined = Cm::Sym::IsSymbolDefined(condCompPrimaryNode.Symbol()->Str());
    PushCondCompValue(defined);
}

void Binder::Visit(Cm::Ast::CondCompStatementNode& condCompStatementNode)
{
    if (!condCompilationEvaluationStack.empty())
    {
        throw std::runtime_error("conditional compilation evaluation stack is not empty");
    }
    Cm::Ast::CondCompilationPartNode* ifPartNode = condCompStatementNode.IfPart();
    if (!ifPartNode)
    {
        throw std::runtime_error("conditional compilation statement if part is empty");
    }
    if (!ifPartNode->Expr())
    {
        throw std::runtime_error("conditional compilation statement if part expression is empty");
    }
    ifPartNode->Expr()->Accept(*this);
    bool execute = PopCondCompValue();
    if (execute)
    {
        ifPartNode->Accept(*this);
    }
    else
    {
        Cm::Ast::CondCompilationPartNodeList& elifPartNodes = condCompStatementNode.ElifParts();
        int n = elifPartNodes.Count();
        for (int i = 0; i < n; ++i)
        {
            Cm::Ast::CondCompilationPartNode* elifPartNode = elifPartNodes[i];
            if (!elifPartNode)
            {
                throw std::runtime_error("conditional compilation statement elif part is empty");
            }
            if (!elifPartNode->Expr())
            {
                throw std::runtime_error("conditional compilation statement elif part expression is empty");
            }
            elifPartNode->Expr()->Accept(*this);
            execute = PopCondCompValue();
            if (execute)
            {
                elifPartNode->Accept(*this);
                break;
            }
        }
        if (!execute)
        {
            Cm::Ast::CondCompilationPartNode* elsePartNode = condCompStatementNode.ElsePart();
            if (elsePartNode)
            {
                elsePartNode->Accept(*this);
            }
        }
    }
}

void Binder::PushCondCompValue(bool value)
{
    condCompilationEvaluationStack.push(value);
}

bool Binder::PopCondCompValue()
{
    if (condCompilationEvaluationStack.empty())
    {
        throw std::runtime_error("conditional compilation evaluation stack is empty");
    }
    bool value = condCompilationEvaluationStack.top();
    condCompilationEvaluationStack.pop();
    return value;
}

} } // namespace Cm::Bind
