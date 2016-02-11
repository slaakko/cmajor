/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/StatementBinder.hpp>
#include <Cm.Bind/LocalVariable.hpp>
#include <Cm.Bind/OverloadResolution.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/Evaluator.hpp>
#include <Cm.Bind/Function.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Bind/Binder.hpp>
#include <Cm.Bind/DelegateTypeOpRepository.hpp>
#include <Cm.Bind/ClassDelegateTypeOpRepository.hpp>
#include <Cm.Bind/Class.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Core/GlobalSettings.hpp>
#include <Cm.Sym/DeclarationVisitor.hpp>
#include <Cm.Sym/ExceptionTable.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.Parser/FileRegistry.hpp>
#include <Cm.Parser/Expression.hpp>
#include <Cm.Parser/TypeExpr.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Expression.hpp>
#include <Cm.Ast/Literal.hpp>
#include <Cm.Ast/BasicType.hpp>
#include <Cm.Ast/Clone.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

namespace Cm { namespace Bind {

StatementBinder::StatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_) :
    ExpressionBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), symbolTable(boundCompileUnit_.SymbolTable()), containerScope(containerScope_), fileScopes(fileScopes_), 
    result(nullptr)
{
}

ConstructionStatementBinder::ConstructionStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_) : 
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), constructionStatement(nullptr)
{
}

void ConstructionStatementBinder::BeginVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
    constructionStatement = new Cm::BoundTree::BoundConstructionStatement(&constructionStatementNode);
    Cm::Sym::LocalVariableSymbol* localVariable = BindLocalVariable(SymbolTable(), ContainerScope(), FileScopes(), BoundCompileUnit().ClassTemplateRepository(), &constructionStatementNode);
    constructionStatement->SetLocalVariable(localVariable);
    CurrentFunction()->AddLocalVariable(localVariable);
}

void ConstructionStatementBinder::EndVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
    if (constructionStatement->LocalVariable()->Used())
    { 
        throw Cm::Core::Exception("local variable '" + constructionStatement->LocalVariable()->Name() + "' is used before it is defined", constructionStatement->LocalVariable()->GetUseSpan(), 
            constructionStatementNode.GetSpan());
    }
    constructionStatement->SetArguments(GetExpressions());
    if (constructionStatement->LocalVariable()->GetType()->IsClassDelegateTypeSymbol())
    {
        if (constructionStatement->Arguments().Count() == 2 && constructionStatement->Arguments()[0]->IsBoundFunctionGroup() &&
            constructionStatement->Arguments()[1]->GetFlag(Cm::BoundTree::BoundNodeFlags::classObjectArg))
        {
            constructionStatement->Arguments().Reverse();
        }
        else if (CurrentFunction()->GetFunctionSymbol()->IsMemberFunctionSymbol() && constructionStatement->Arguments().Count() == 1 &&
            constructionStatement->Arguments()[0]->IsBoundFunctionGroup())
        {
            Cm::Sym::ParameterSymbol* thisParam = CurrentFunction()->GetFunctionSymbol()->Parameters()[0];
            Cm::BoundTree::BoundParameter* thisParamArg = new Cm::BoundTree::BoundParameter(&constructionStatementNode, thisParam);
            thisParamArg->SetType(thisParam->GetType());
            constructionStatement->Arguments().InsertFront(thisParamArg);
        }
    }
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Sym::TypeSymbol* localVariableType = constructionStatement->LocalVariable()->GetType();
    if (localVariableType->IsAbstract())
    {
        throw Cm::Core::Exception("cannot instantiate an abstract class", localVariableType->GetSpan(), constructionStatementNode.GetSpan());
    }
    Cm::Core::Argument variableArgument(Cm::Core::ArgumentCategory::lvalue, SymbolTable().GetTypeRepository().MakePointerType(localVariableType, constructionStatementNode.GetSpan()));
    resolutionArguments.push_back(variableArgument);
    constructionStatement->GetResolutionArguments(localVariableType, resolutionArguments);
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, constructionStatement->LocalVariable()->GetType()->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::ConversionType conversionType = Cm::Sym::ConversionType::implicit;
    Cm::Sym::FunctionSymbol* ctor = ResolveOverload(ContainerScope(), BoundCompileUnit(), "@constructor", resolutionArguments, functionLookups, constructionStatementNode.GetSpan(), conversions, 
        conversionType,  OverloadResolutionFlags::none);
    PrepareFunctionSymbol(ctor, constructionStatementNode.GetSpan());
    constructionStatement->SetConstructor(ctor);
    constructionStatement->InsertLocalVariableToArguments();
    constructionStatement->Arguments()[0]->SetFlag(Cm::BoundTree::BoundNodeFlags::constructVariable);
    int n = int(conversions.size());
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::FunctionSymbol* conversionFun = conversions[i];
        if (conversionFun)
        {
            Cm::BoundTree::BoundExpression* arg = constructionStatement->Arguments()[i].release();
            constructionStatement->Arguments()[i].reset(CreateBoundConversion(ContainerScope(), BoundCompileUnit(), &constructionStatementNode, arg, conversionFun, CurrentFunction()));
        }
    }
    if (!ctor->IsBasicTypeOp())
    {
        constructionStatement->SetTraceCallInfo(CreateTraceCallInfo(BoundCompileUnit(), CurrentFunction()->GetFunctionSymbol(), constructionStatementNode.GetSpan()));
    }
    if (ctor->IsDelegateFromFunCtor())
    {
        DelegateFromFunCtor* delegateFromFunCtor = static_cast<DelegateFromFunCtor*>(ctor);
        Cm::BoundTree::BoundFunctionId* boundFunctionId = new Cm::BoundTree::BoundFunctionId(&constructionStatementNode, delegateFromFunCtor->FunctionSymbol());
        boundFunctionId->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        boundFunctionId->SetType(BoundCompileUnit().SymbolTable().GetTypeRepository().MakePointerType(delegateFromFunCtor->DelegateType(), constructionStatementNode.GetSpan()));
        constructionStatement->Arguments()[1].reset(boundFunctionId);
    }
    else if (ctor->IsClassDelegateFromFunCtor())
    {
        ClassDelegateFromFunCtor* classDelegateFromFunCtor = static_cast<ClassDelegateFromFunCtor*>(ctor);
        Cm::BoundTree::BoundFunctionId* boundFunctionId = new Cm::BoundTree::BoundFunctionId(&constructionStatementNode, classDelegateFromFunCtor->FunctionSymbol());
        boundFunctionId->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        boundFunctionId->SetType(BoundCompileUnit().SymbolTable().GetTypeRepository().MakePointerType(classDelegateFromFunCtor->DelegateType(), constructionStatementNode.GetSpan()));
        constructionStatement->Arguments()[2].reset(boundFunctionId);
    }
    PrepareArguments(ContainerScope(), BoundCompileUnit(), CurrentFunction(), nullptr, ctor->Parameters(), constructionStatement->Arguments(), true, BoundCompileUnit().IrClassTypeRepository(),
        ctor->IsBasicTypeOp());
    if (localVariableType->IsReferenceType())
    {
        constructionStatement->Arguments()[1]->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
    }
    SetResult(constructionStatement);
}

AssignmentStatementBinder::AssignmentStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_) : 
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_)
{
}

void AssignmentStatementBinder::EndVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode)
{
    Cm::BoundTree::BoundExpression* left = nullptr;
    Cm::BoundTree::BoundExpression* right = nullptr;
    std::unique_ptr<Cm::BoundTree::BoundExpression> functionGroup(nullptr);
    if (Stack().ItemCount() == 3)
    {
        right = Pop();
        functionGroup.reset(Pop());
        left = Pop();
    }
    else
    {
        right = Pop();
        left = Pop();
    }
    if (left->GetType()->IsClassDelegateTypeSymbol() && right->GetType()->IsFunctionGroupTypeSymbol())
    {
        if (!functionGroup)
        {
            functionGroup.reset(right);
            Cm::Sym::ParameterSymbol* thisParam = CurrentFunction()->GetFunctionSymbol()->Parameters()[0];
            Cm::BoundTree::BoundParameter* thisParamArg = new Cm::BoundTree::BoundParameter(&assignmentStatementNode, thisParam);
            thisParamArg->SetType(thisParam->GetType());
            right = thisParamArg;
        }
    }
    left->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Sym::TypeSymbol* leftPlainType = SymbolTable().GetTypeRepository().MakePlainType(left->GetType());
    Cm::Core::Argument leftArgument(Cm::Core::ArgumentCategory::lvalue, SymbolTable().GetTypeRepository().MakePointerType(leftPlainType, assignmentStatementNode.GetSpan()));
    resolutionArguments.push_back(leftArgument);
    if (leftPlainType->IsPointerType() && right->IsBoundNullLiteral())
    {
        right->SetType(leftPlainType);
    }
    Cm::Core::Argument rightArgument = Cm::Core::Argument(right->GetArgumentCategory(), right->GetType());
    if (right->GetFlag(Cm::BoundTree::BoundNodeFlags::argIsTemporary))
    {
        rightArgument.SetBindToRvalueRef();
    }
    resolutionArguments.push_back(rightArgument);
    if (functionGroup)
    {
        resolutionArguments.push_back(Cm::Core::Argument(functionGroup->GetArgumentCategory(), functionGroup->GetType()));
    }
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, leftPlainType->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* assignment = ResolveOverload(ContainerScope(), BoundCompileUnit(), "operator=", resolutionArguments, functionLookups, assignmentStatementNode.GetSpan(), conversions);
    PrepareFunctionSymbol(assignment, assignmentStatementNode.GetSpan());
    int conversionCount = 2;
    if (functionGroup)
    {
        conversionCount = 3;
    }
    if (conversions.size() != conversionCount)
    {
        throw std::runtime_error("wrong number of conversions");
    }
    Cm::Sym::FunctionSymbol* leftConversion = conversions[0];
    if (leftConversion)
    {
        left = CreateBoundConversion(ContainerScope(), BoundCompileUnit(), &assignmentStatementNode, left, leftConversion, CurrentFunction());
    }
    Cm::Sym::FunctionSymbol* rightConversion = conversions[1];
    if (rightConversion)
    {
        right = CreateBoundConversion(ContainerScope(), BoundCompileUnit(), &assignmentStatementNode, right, rightConversion, CurrentFunction());
    }
    Cm::BoundTree::BoundExpressionList arguments;
    arguments.Add(left);
    arguments.Add(right);
    if (assignment->IsClassDelegateFromFunAssignment())
    {
        ClassDelegateFromFunAssignment* classDelegateFromFunAssignment = static_cast<ClassDelegateFromFunAssignment*>(assignment);
        Cm::BoundTree::BoundFunctionId* boundFunctionId = new Cm::BoundTree::BoundFunctionId(&assignmentStatementNode, classDelegateFromFunAssignment->FunctionSymbol());
        boundFunctionId->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        boundFunctionId->SetType(BoundCompileUnit().SymbolTable().GetTypeRepository().MakePointerType(classDelegateFromFunAssignment->DelegateType(), assignmentStatementNode.GetSpan()));
        arguments.Add(boundFunctionId);
    }
    PrepareArguments(ContainerScope(), BoundCompileUnit(), CurrentFunction(), assignment->GetReturnType(), assignment->Parameters(), arguments, true, BoundCompileUnit().IrClassTypeRepository(), assignment->IsBasicTypeOp());
    if (assignment->IsClassDelegateFromFunAssignment())
    {
        Cm::BoundTree::BoundFunctionCallStatement* functionCallStatement = new Cm::BoundTree::BoundFunctionCallStatement(assignment, std::move(arguments));
        SetResult(functionCallStatement);
        return;
    }
    left = arguments[0].release();
    right = arguments[1].release();
    Cm::BoundTree::BoundAssignmentStatement* assignmentStatement = new Cm::BoundTree::BoundAssignmentStatement(&assignmentStatementNode, left, right, assignment);
    if (!assignment->IsBasicTypeOp())
    {
        assignmentStatement->SetTraceCallInfo(CreateTraceCallInfo(BoundCompileUnit(), CurrentFunction()->GetFunctionSymbol(), assignmentStatementNode.GetSpan()));
    }
    if (assignment->IsDelegateFromFunAssignment())
    {
        DelegateFromFunAssignment* delegateFromFunAssignment = static_cast<DelegateFromFunAssignment*>(assignment);
        Cm::BoundTree::BoundFunctionId* boundFunctionId = new Cm::BoundTree::BoundFunctionId(&assignmentStatementNode, delegateFromFunAssignment->FunctionSymbol());
        boundFunctionId->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        boundFunctionId->SetType(BoundCompileUnit().SymbolTable().GetTypeRepository().MakePointerType(delegateFromFunAssignment->DelegateType(), assignmentStatementNode.GetSpan()));
        assignmentStatement->RightArgument().reset(boundFunctionId);
    }
    SetResult(assignmentStatement);
}

SimpleStatementBinder::SimpleStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_, Binder& binder_) :
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), binder(binder_)
{
}

void SimpleStatementBinder::EndVisit(Cm::Ast::SimpleStatementNode& simpleStatementNode)
{
    Cm::BoundTree::BoundSimpleStatement* simpleStatement = new Cm::BoundTree::BoundSimpleStatement(&simpleStatementNode);
    if (simpleStatementNode.HasExpr())
    {
        Cm::BoundTree::BoundExpression* expression = Pop();
        simpleStatement->SetExpression(expression);
        if (expression->IsBoundFunctionCall())
        {
            Cm::BoundTree::BoundFunctionCall* funCall = static_cast<Cm::BoundTree::BoundFunctionCall*>(expression);
            if (funCall->GetFunction()->FullName() == "System.RethrowException(System.ExceptionPtr&)")
            {
                Cm::Ast::TryStatementNode* currentTry = CurrentFunction()->GetCurrentTry();
                if (currentTry == nullptr && CurrentFunction()->GetFunctionSymbol()->IsNothrow())
                {
                    throw Cm::Core::Exception("nothrow function cannot throw", simpleStatementNode.GetSpan(), CurrentFunction()->GetFunctionSymbol()->GetSpan());
                }
                binder.AddBoundStatement(simpleStatement);
                binder.AddBoundStatement(new Cm::BoundTree::BoundBeginThrowStatement(&simpleStatementNode));
                Cm::Ast::InvokeNode* callThisThreadExpr = new Cm::Ast::InvokeNode(simpleStatementNode.GetSpan(),
                    new Cm::Ast::IdentifierNode(simpleStatementNode.GetSpan(), "this_thread"));
                Cm::Ast::InvokeNode* callGetExceptionTableAddrExpr = new Cm::Ast::InvokeNode(simpleStatementNode.GetSpan(),
                    new Cm::Ast::IdentifierNode(simpleStatementNode.GetSpan(), "get_exception_table_addr"));
                callGetExceptionTableAddrExpr->AddArgument(callThisThreadExpr);
                Cm::Ast::InvokeNode* callSetExceptionAddrExpr = new Cm::Ast::InvokeNode(simpleStatementNode.GetSpan(),
                    new Cm::Ast::IdentifierNode(simpleStatementNode.GetSpan(), "System.Support.SetExceptionAddr"));
                callSetExceptionAddrExpr->AddArgument(callGetExceptionTableAddrExpr);
                callSetExceptionAddrExpr->AddArgument(new Cm::Ast::InvokeNode(simpleStatementNode.GetSpan(), new Cm::Ast::IdentifierNode(simpleStatementNode.GetSpan(), "get_current_exception_id")));
                callSetExceptionAddrExpr->AddArgument(new Cm::Ast::InvokeNode(simpleStatementNode.GetSpan(), new Cm::Ast::IdentifierNode(simpleStatementNode.GetSpan(), "get_current_exception_addr")));
                Cm::Ast::SimpleStatementNode* setExceptionAddrStatement = new Cm::Ast::SimpleStatementNode(simpleStatementNode.GetSpan(), callSetExceptionAddrExpr);
                Cm::Ast::AssignmentStatementNode* setExceptionCodeStatement = new Cm::Ast::AssignmentStatementNode(simpleStatementNode.GetSpan(),
                    new Cm::Ast::IdentifierNode(simpleStatementNode.GetSpan(), Cm::IrIntf::GetExCodeVarName()),
                    new Cm::Ast::InvokeNode(simpleStatementNode.GetSpan(), new Cm::Ast::IdentifierNode(simpleStatementNode.GetSpan(), "get_current_exception_id")));
                Cm::Ast::SimpleStatementNode* resetCurrentExceptionStatement = new Cm::Ast::SimpleStatementNode(simpleStatementNode.GetSpan(), new Cm::Ast::InvokeNode(simpleStatementNode.GetSpan(),
                    new Cm::Ast::IdentifierNode(simpleStatementNode.GetSpan(), "reset_current_exception")));
                Cm::Ast::AssignmentStatementNode* setExceptionCodeParamStatement = nullptr;
                Cm::Ast::StatementNode* endOfThrowStatement = nullptr;
                Cm::Ast::FunctionNode* functionNode = simpleStatementNode.GetFunction();
                Cm::Ast::ExitTryStatementNode* exitTryStatement = nullptr;
                Cm::Ast::TryStatementNode* ownerTry = nullptr;
                if (CurrentFunction()->InHandler())
                {
                    ownerTry = CurrentFunction()->GetParentTry();
                }
                else
                {
                    ownerTry = CurrentFunction()->GetCurrentTry();
                }
                if (ownerTry)
                {
                    int catchId = ownerTry->GetFirstHandler()->CatchId();
                    if (catchId == -1)
                    {
                        catchId = CurrentFunction()->GetNextCatchId();
                        ownerTry->SetFirstCatchId(catchId);
                    }
                    exitTryStatement = new Cm::Ast::ExitTryStatementNode(simpleStatementNode.GetSpan(), ownerTry);
                    std::string continueLabelPrefix;
                    Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
                    if (backend == Cm::IrIntf::BackEnd::llvm)
                    {
                        continueLabelPrefix = "$C";
                    }
                    else if (backend == Cm::IrIntf::BackEnd::c)
                    {
                        continueLabelPrefix = "_C_";
                    }
                    Cm::Ast::GotoStatementNode* gotoCatch = new Cm::Ast::GotoStatementNode(simpleStatementNode.GetSpan(), new Cm::Ast::LabelNode(simpleStatementNode.GetSpan(), continueLabelPrefix + std::to_string(catchId)));
                    gotoCatch->SetExceptionHandlingGoto();
                    endOfThrowStatement = gotoCatch;
                }
                else
                {
                    setExceptionCodeParamStatement = new Cm::Ast::AssignmentStatementNode(simpleStatementNode.GetSpan(),
                        new Cm::Ast::IdentifierNode(simpleStatementNode.GetSpan(), Cm::IrIntf::GetExceptionCodeParamName()),
                        new Cm::Ast::IdentifierNode(simpleStatementNode.GetSpan(), Cm::IrIntf::GetExCodeVarName()));
                    Cm::Ast::Node* returnTypeExpr = functionNode->ReturnTypeExpr();
                    if (returnTypeExpr && returnTypeExpr->GetNodeType() != Cm::Ast::NodeType::voidNode)
                    {
                        Cm::Ast::CloneContext cloneContext;
                        Cm::Ast::InvokeNode* defaultValue = new Cm::Ast::InvokeNode(simpleStatementNode.GetSpan(), returnTypeExpr->Clone(cloneContext));
                        endOfThrowStatement = new Cm::Ast::ReturnStatementNode(simpleStatementNode.GetSpan(), defaultValue);
                    }
                    else
                    {
                        endOfThrowStatement = new Cm::Ast::ReturnStatementNode(simpleStatementNode.GetSpan(), nullptr);
                    }
                }
                Cm::Ast::CompoundStatementNode* throwActions = new Cm::Ast::CompoundStatementNode(simpleStatementNode.GetSpan());
                CurrentFunction()->Own(throwActions);
                throwActions->SetLabelNode(simpleStatementNode.Label());
                throwActions->SetParent(functionNode);
                throwActions->AddStatement(setExceptionAddrStatement);
                throwActions->AddStatement(setExceptionCodeStatement);
                throwActions->AddStatement(resetCurrentExceptionStatement);
                if (setExceptionCodeParamStatement)
                {
                    throwActions->AddStatement(setExceptionCodeParamStatement);
                }
                if (exitTryStatement)
                {
                    throwActions->AddStatement(exitTryStatement);
                }
                throwActions->AddStatement(endOfThrowStatement);
                Cm::Sym::DeclarationVisitor declarationVisitor(SymbolTable());
                throwActions->Accept(declarationVisitor);
                Cm::Sym::ContainerScope* containerScope = SymbolTable().GetContainerScope(throwActions);
                containerScope->SetParent(ContainerScope());
                binder.BeginContainerScope(containerScope);
                throwActions->Accept(binder);
                binder.EndContainerScope();
                binder.AddBoundStatement(new Cm::BoundTree::BoundEndThrowStatement(&simpleStatementNode));
                SetResult(nullptr);
                return;
            }
        }
    }
    SetResult(simpleStatement);
}

ReturnStatementBinder::ReturnStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_) : 
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_)
{
}

void ReturnStatementBinder::EndVisit(Cm::Ast::ReturnStatementNode& returnStatementNode)
{
    Cm::BoundTree::BoundReturnStatement* returnStatement = new Cm::BoundTree::BoundReturnStatement(&returnStatementNode);
    Cm::Ast::FunctionNode* functionNode = returnStatementNode.GetFunction();
    Cm::Ast::Node* returnTypeExpr = functionNode->ReturnTypeExpr();
    if (returnTypeExpr)
    {
        Cm::Sym::TypeSymbol* returnType = ResolveType(SymbolTable(), ContainerScope(), FileScopes(), BoundCompileUnit().ClassTemplateRepository(), returnTypeExpr);
        if (!returnType->IsVoidTypeSymbol())
        {
            if (returnStatementNode.ReturnsValue())
            {
                returnStatement->SetReturnType(returnType);
                Cm::BoundTree::BoundExpression* returnValue = Pop();
                Cm::Sym::MemberVariableSymbol* memberVariableSymbol = returnValue->GetMemberVariableSymbol();
                if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::optimize) && CurrentFunction()->GetFunctionSymbol()->IsInline() && memberVariableSymbol != nullptr && memberVariableSymbol->IsStatic())
                {
                    throw Cm::Core::Exception("inline function cannot return address of a static member variable", returnStatementNode.GetSpan(), memberVariableSymbol->GetSpan());
                }
                std::vector<Cm::Core::Argument> resolutionArguments;
                Cm::Core::Argument targetArgument(Cm::Core::ArgumentCategory::lvalue, SymbolTable().GetTypeRepository().MakePointerType(returnType, returnStatementNode.GetSpan()));
                resolutionArguments.push_back(targetArgument);
                if (returnType->IsNonConstReferenceType() && (returnValue->GetType()->IsNonConstReferenceType() || !returnValue->GetType()->IsReferenceType()))
                {
                    Cm::Core::Argument sourceArgument = Cm::Core::Argument(returnValue->GetArgumentCategory(), SymbolTable().GetTypeRepository().MakeReferenceType(returnValue->GetType(),
                        returnStatementNode.GetSpan()));
                    resolutionArguments.push_back(sourceArgument);
                    if (!returnValue->GetType()->IsReferenceType())
                    {
                        returnValue->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
                    }
                }
				else
                {
                    if (returnValue->GetType()->IsFunctionGroupTypeSymbol())
                    {
                        Cm::Core::Argument sourceArgument = Cm::Core::Argument(returnValue->GetArgumentCategory(), returnValue->GetType());
                        resolutionArguments.push_back(sourceArgument);
                    }
                    else
                    {
                        Cm::Sym::TypeSymbol* sourceType = returnValue->GetType();
                        if (!sourceType->IsPointerType())
                        {
                            sourceType = SymbolTable().GetTypeRepository().MakeConstReferenceType(sourceType, returnStatementNode.GetSpan());
                        }
                        Cm::Core::Argument sourceArgument = Cm::Core::Argument(returnValue->GetArgumentCategory(), sourceType);
                        resolutionArguments.push_back(sourceArgument);
                        if (returnType->IsReferenceType())
                        {
                            if (!returnValue->GetType()->IsReferenceType())
                            {
                                returnValue->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
                            }
                        }
                    }
                }
                if (returnValue->GetFlag(Cm::BoundTree::BoundNodeFlags::argIsTemporary) || returnValue->IsBoundLocalVariable() || 
					returnValue->IsBoundParameter() && !returnValue->GetType()->IsReferenceType())
                {
                    Cm::Core::Argument& sourceArgument = resolutionArguments[1];
                    sourceArgument.SetBindToRvalueRef();
                }
                if (returnValue->IsBoundMemberVariable())
                {
                    Cm::BoundTree::BoundMemberVariable* memberVar = static_cast<Cm::BoundTree::BoundMemberVariable*>(returnValue);
                    if (memberVar->Symbol()->IsStatic() && returnType->IsNonClassReferenceType() && !returnValue->GetType()->IsReferenceType())
                    {
                        Cm::Sym::LocalVariableSymbol* temporary = CurrentFunction()->CreateTempLocalVariable(returnType);
                        temporary->SetSid(BoundCompileUnit().SymbolTable().GetSid());
                        Cm::BoundTree::BoundLocalVariable* boundTemporary = new Cm::BoundTree::BoundLocalVariable(&returnStatementNode, temporary);
                        boundTemporary->SetType(returnType);
                        boundTemporary->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
                        returnStatement->SetBoundTemporary(boundTemporary);
                    }
                }
                Cm::Sym::FunctionLookupSet functionLookups;
                functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, returnType->GetContainerScope()->ClassOrNsScope()));
                std::vector<Cm::Sym::FunctionSymbol*> conversions;
                Cm::Sym::ConversionType conversionType = Cm::Sym::ConversionType::implicit;
                if (returnValue->IsBoundCast())
                {
                    conversionType = Cm::Sym::ConversionType::explicit_;
                }
                Cm::Sym::FunctionSymbol* ctor = ResolveOverload(ContainerScope(), BoundCompileUnit(), "@constructor", resolutionArguments, functionLookups, returnStatementNode.GetSpan(), conversions, 
                    conversionType, OverloadResolutionFlags::none);
                PrepareFunctionSymbol(ctor, returnStatementNode.GetSpan());
                returnStatement->SetConstructor(ctor);
                if (!ctor->IsBasicTypeOp())
                {
                    returnStatement->SetTraceCallInfo(CreateTraceCallInfo(BoundCompileUnit(), CurrentFunction()->GetFunctionSymbol(), returnStatementNode.GetSpan()));
                }
                if (ctor->IsDelegateFromFunCtor())
                {
                    DelegateFromFunCtor* delegateFromFunConstructor = static_cast<DelegateFromFunCtor*>(ctor);
                    Cm::BoundTree::BoundFunctionId* boundFunctionId = new Cm::BoundTree::BoundFunctionId(&returnStatementNode, delegateFromFunConstructor->FunctionSymbol());
                    boundFunctionId->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
                    boundFunctionId->SetType(BoundCompileUnit().SymbolTable().GetTypeRepository().MakePointerType(delegateFromFunConstructor->DelegateType(), returnStatementNode.GetSpan()));
                    returnValue = boundFunctionId;
                }
                if (conversions.size() != 2)
                {
                    throw std::runtime_error("wrong number of conversions");
                }
                Cm::Sym::FunctionSymbol* conversionFun = conversions[1];
                if (conversionFun)
                {
                    returnValue = CreateBoundConversion(ContainerScope(), BoundCompileUnit(), &returnStatementNode, returnValue, conversionFun, CurrentFunction());
                }
                if (returnValue->GetType()->IsClassTypeSymbol() && (returnType->IsReferenceType() || returnType->IsClassTypeSymbol()))
                {
                    returnValue->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
                }
                returnStatement->SetExpression(returnValue);
                returnStatement->SetBoundReturnValue(new Cm::BoundTree::BoundReturnValue(&returnStatementNode, CurrentFunction()->GetFunctionSymbol()->ReturnValue()));
            }
            else
            {
                throw Cm::Core::Exception("must return a value", returnStatementNode.GetSpan(), functionNode->GetSpan());
            }
        }
        else if (returnStatementNode.ReturnsValue())
        {
            throw Cm::Core::Exception("void function cannot return a value", returnStatementNode.GetSpan(), functionNode->GetSpan());
        }
    }
    else if (returnStatementNode.ReturnsValue())
    {
        throw Cm::Core::Exception("cannot return a value", returnStatementNode.GetSpan(), functionNode->GetSpan());
    }
    SetResult(returnStatement);
}

ConditionalStatementBinder::ConditionalStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundConditionalStatement* conditionalStatement_) :
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), conditionalStatement(conditionalStatement_)
{
    PushSkipContent();
}

void ConditionalStatementBinder::EndVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode)
{
    PopSkipContent();
    Cm::BoundTree::BoundExpression* condition = Pop();
    Cm::Sym::TypeSymbol* condType = SymbolTable().GetTypeRepository().MakePlainType(condition->GetType());
    if (!condType->IsBoolTypeSymbol())
    {
        throw Cm::Core::Exception("if statement condition must be Boolean expression (now of type '" + condType->FullName() + "')", conditionalStatementNode.Condition()->GetSpan());
    }
    condition->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
    conditionalStatement->SetCondition(condition);
}

WhileStatementBinder::WhileStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundWhileStatement* whileStatement_) :
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_,  currentFunction_), whileStatement(whileStatement_)
{
    PushSkipContent();
}

void WhileStatementBinder::EndVisit(Cm::Ast::WhileStatementNode& whileStatementNode) 
{
    PopSkipContent();
    Cm::BoundTree::BoundExpression* condition = Pop();
    Cm::Sym::TypeSymbol* condType = SymbolTable().GetTypeRepository().MakePlainType(condition->GetType());
    if (!condType->IsBoolTypeSymbol())
    {
        throw Cm::Core::Exception("while statement condition must be Boolean expression (now of type '" + condType->FullName() + "')", whileStatementNode.Condition()->GetSpan());
    }
    condition->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
    whileStatement->SetCondition(condition);
}

DoStatementBinder::DoStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundDoStatement* doStatement_) :
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), doStatement(doStatement_)
{
    PushSkipContent();
}

void DoStatementBinder::EndVisit(Cm::Ast::DoStatementNode& doStatementNode)
{
    PopSkipContent();
    Cm::BoundTree::BoundExpression* condition = Pop();
    Cm::Sym::TypeSymbol* condType = SymbolTable().GetTypeRepository().MakePlainType(condition->GetType());
    if (!condType->IsBoolTypeSymbol())
    {
        throw Cm::Core::Exception("do statement condition must be Boolean expression (now of type '" + condType->FullName() + "')", doStatementNode.Condition()->GetSpan());
    }
    condition->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
    doStatement->SetCondition(condition);
}

ForStatementBinder::ForStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundForStatement* forStatement_) :
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), forStatement(forStatement_)
{
    PushSkipContent();
}

void ForStatementBinder::EndVisit(Cm::Ast::ForStatementNode& forStatementNode)
{
    PopSkipContent();
    if (!forStatementNode.HasIncrement())
    {
        GenerateTrueExpression(&forStatementNode);
    }
    Cm::BoundTree::BoundExpression* increment = Pop();
    forStatement->SetIncrement(increment);
    if (!forStatementNode.HasCondition())
    {
        GenerateTrueExpression(&forStatementNode);
    }
    Cm::BoundTree::BoundExpression* condition = Pop();
    Cm::Sym::TypeSymbol* condType = SymbolTable().GetTypeRepository().MakePlainType(condition->GetType());
    if (!condType->IsBoolTypeSymbol())
    {
        throw Cm::Core::Exception("for statement condition must be Boolean expression (now of type '" + condType->FullName() + "')", forStatementNode.Condition()->GetSpan());
    }
    condition->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
    forStatement->SetCondition(condition);
}

Cm::Parser::ExpressionGrammar* expressionGrammar = nullptr;

Cm::Ast::Node* MakeExpressionNode(Cm::Sym::TypeSymbol* typeSymbol, const Cm::Parsing::Span& span)
{
    if (!expressionGrammar)
    {
        expressionGrammar = Cm::Parser::ExpressionGrammar::Create();
    }
    std::string typeSymbolText = Cm::Util::Trim(typeSymbol->FullName());
    int n = int(typeSymbolText.size());
    Cm::Parser::ParsingContext parsingContext;
    return expressionGrammar->Parse(&typeSymbolText[0], &typeSymbolText[n], 0, "", &parsingContext);
}

Cm::Parser::TypeExprGrammar* typeExprGrammar = nullptr;

Cm::Ast::Node* MakeTypeIdNode(Cm::Sym::TypeSymbol* typeSymbol, const Cm::Parsing::Span& span)
{
    if (!typeExprGrammar)
    {
        typeExprGrammar = Cm::Parser::TypeExprGrammar::Create();
    }
    std::string typeSymbolText = Cm::Util::Trim(typeSymbol->FullName());
    int n = int(typeSymbolText.size());
    Cm::Parser::ParsingContext parsingContext;
    return typeExprGrammar->Parse(&typeSymbolText[0], &typeSymbolText[n], 0, "", &parsingContext);
}

RangeForStatementBinder::RangeForStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_,
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_, Binder& binder_) :
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), binder(binder_)
{
}

void RangeForStatementBinder::EndVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode) 
{
    rangeForStatementNode.Container()->Accept(*this);
    std::unique_ptr<Cm::BoundTree::BoundExpression> container(Pop());
    Cm::Sym::TypeSymbol* containerType = container->GetType();
    Cm::Sym::TypeSymbol* plainContainerType = SymbolTable().GetTypeRepository().MakePlainType(containerType);
    Cm::Ast::IdentifierNode* beginNode = nullptr;
    Cm::Ast::IdentifierNode* endNode = nullptr;
    if (containerType->IsConstType())
    {
        beginNode = new Cm::Ast::IdentifierNode(rangeForStatementNode.GetSpan(), "CBegin");
        endNode = new Cm::Ast::IdentifierNode(rangeForStatementNode.GetSpan(), "CEnd");
    }
    else
    {
        beginNode = new Cm::Ast::IdentifierNode(rangeForStatementNode.GetSpan(), "Begin");
        endNode = new Cm::Ast::IdentifierNode(rangeForStatementNode.GetSpan(), "End");
    }
    Cm::Ast::CloneContext cloneContext;
    Cm::Ast::DotNode* containerBeginNode(new Cm::Ast::DotNode(rangeForStatementNode.GetSpan(), rangeForStatementNode.Container()->Clone(cloneContext), beginNode));
    Cm::Ast::InvokeNode* invokeContainerBeginNode(new Cm::Ast::InvokeNode(rangeForStatementNode.GetSpan(), containerBeginNode));
    Cm::Ast::DotNode* containerEndNode(new Cm::Ast::DotNode(rangeForStatementNode.GetSpan(), rangeForStatementNode.Container()->Clone(cloneContext), endNode));
    Cm::Ast::InvokeNode* invokeContainerEndNode(new Cm::Ast::InvokeNode(rangeForStatementNode.GetSpan(), containerEndNode));
    Cm::Ast::IdentifierNode* iteratorId(new Cm::Ast::IdentifierNode(rangeForStatementNode.GetSpan(), CurrentFunction()->GetNextTempVariableName()));
    Cm::Ast::IdentifierNode* iteratorTypeId = nullptr; 
    if (containerType->IsConstType())
    {
        iteratorTypeId = new Cm::Ast::IdentifierNode(rangeForStatementNode.GetSpan(), "ConstIterator");
    }
    else
    {
        iteratorTypeId = new Cm::Ast::IdentifierNode(rangeForStatementNode.GetSpan(), "Iterator");
    }
    Cm::Ast::Node* containerTypeId = MakeExpressionNode(plainContainerType, rangeForStatementNode.GetSpan());
    Cm::Ast::DotNode* containerIterator(new Cm::Ast::DotNode(rangeForStatementNode.GetSpan(), containerTypeId, iteratorTypeId));
    Cm::Ast::ConstructionStatementNode* initNode(new Cm::Ast::ConstructionStatementNode(rangeForStatementNode.GetSpan(), containerIterator, iteratorId));
    initNode->AddArgument(invokeContainerBeginNode);
    Cm::Ast::NotEqualNode* iteratorNotEqualToEnd(new Cm::Ast::NotEqualNode(rangeForStatementNode.VariableSpan(), iteratorId->Clone(cloneContext), invokeContainerEndNode));
    Cm::Ast::PrefixIncNode* incIterator(new Cm::Ast::PrefixIncNode(rangeForStatementNode.Container()->GetSpan(), iteratorId->Clone(cloneContext)));
    Cm::Ast::CompoundStatementNode* action(new Cm::Ast::CompoundStatementNode(rangeForStatementNode.GetSpan()));
    action->SetBeginBraceSpan(Span(0, 0, 0, 0));
    action->SetEndBraceSpan(Span(0, 0, 0, 0));
    action->SetParent(rangeForStatementNode.Parent());
    Cm::Ast::DerefNode* derefIterator(new Cm::Ast::DerefNode(rangeForStatementNode.GetSpan(), iteratorId->Clone(cloneContext)));
    Cm::Ast::ConstructionStatementNode* initVariable(new Cm::Ast::ConstructionStatementNode(Span(0, 0, 0, 0), rangeForStatementNode.VarTypeExpr()->Clone(cloneContext), 
        static_cast<Cm::Ast::IdentifierNode*>(rangeForStatementNode.VarId()->Clone(cloneContext))));
    initVariable->AddArgument(derefIterator);
    action->AddStatement(initVariable);
    Cm::Sym::DeclarationVisitor declarationVisitor(SymbolTable());
    action->AddStatement(static_cast<Cm::Ast::StatementNode*>(rangeForStatementNode.Action()->Clone(cloneContext)));
    Cm::Ast::ForStatementNode* forStatementNode = new Cm::Ast::ForStatementNode(rangeForStatementNode.GetSpan(), initNode, iteratorNotEqualToEnd, incIterator, action);
    forStatementNode->SetAsRangeForStatement();
    forStatementNode->SetParent(rangeForStatementNode.Parent());
    CurrentFunction()->Own(forStatementNode);
    forStatementNode->Accept(declarationVisitor);
    Cm::Sym::ContainerScope* containerScope = SymbolTable().GetContainerScope(forStatementNode);
    containerScope->SetParent(ContainerScope());
    binder.BeginContainerScope(containerScope);
    forStatementNode->Accept(binder);
    binder.EndContainerScope();
}

SwitchStatementBinder::SwitchStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundSwitchStatement* switchStatement_): 
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), switchStatement(switchStatement_)
{
}

void SwitchStatementBinder::BeginVisit(Cm::Ast::SwitchStatementNode& switchStatementNode)
{
    PushSkipContent();
    Cm::Ast::Node* conditionNode = switchStatementNode.Condition();
    conditionNode->Accept(*this);
    Cm::BoundTree::BoundExpression* condition = Pop();
    Cm::Sym::TypeSymbol* condType = SymbolTable().GetTypeRepository().MakePlainType(condition->GetType());
    if (condType->IsIntegerTypeSymbol() || condType->IsCharTypeSymbol() || condType->IsBoolTypeSymbol() || condType->IsEnumTypeSymbol())
    {
        switchStatement->SetCondition(condition);
    }
    else
    {
        throw Cm::Core::Exception("switch statement condition must be of integer, character, enumerated or Boolean type", conditionNode->GetSpan());
    }
}

void SwitchStatementBinder::EndVisit(Cm::Ast::SwitchStatementNode& switchStatementNode)
{
    PopSkipContent();
}

CaseStatementBinder::CaseStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundCaseStatement* caseStatement_, Cm::BoundTree::BoundSwitchStatement* switchStatement_) : 
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), caseStatement(caseStatement_), switchStatement(switchStatement_)
{
    PushSkipContent();
}

bool TerminatesCase(Cm::Ast::StatementNode* statementNode)
{
    if (statementNode->IsConditionalStatementNode())
    {
        Cm::Ast::ConditionalStatementNode* conditionalStatement = static_cast<Cm::Ast::ConditionalStatementNode*>(statementNode);
        if (conditionalStatement->HasElseStatement())
        {
            if (TerminatesCase(conditionalStatement->ThenS()) && TerminatesCase(conditionalStatement->ElseS()))
            {
                return true;
            }
        }
    }
    else if (statementNode->IsCompoundStatementNode())
    {
        Cm::Ast::CompoundStatementNode* compoundStatement = static_cast<Cm::Ast::CompoundStatementNode*>(statementNode);
        for (const std::unique_ptr<Cm::Ast::StatementNode>& statement : compoundStatement->Statements())
        {
            if (TerminatesCase(statement.get())) return true;
        }
    }
    else
    {
        return statementNode->IsCaseTerminatingNode();
    }
    return false;
}

void CaseStatementBinder::EndVisit(Cm::Ast::CaseStatementNode& caseStatementNode)
{
    PopSkipContent();
    for (const std::unique_ptr<Cm::Ast::Node>& expr : caseStatementNode.Expressions())
    {
        Cm::Sym::TypeSymbol* condType = switchStatement->Condition()->GetType();
        if (condType->IsEnumTypeSymbol())
        {
            Cm::Sym::EnumTypeSymbol* enumTypeSymbol = static_cast<Cm::Sym::EnumTypeSymbol*>(condType);
            condType = enumTypeSymbol->GetUnderlyingType();
        }
        Cm::Sym::SymbolType symbolType = condType->GetSymbolType();
        Cm::Sym::ValueType valueType = Cm::Sym::GetValueTypeFor(symbolType);
        Cm::Sym::Value* value = Evaluate(valueType, false, expr.get(), SymbolTable(), ContainerScope(), FileScopes(), BoundCompileUnit().ClassTemplateRepository());
        caseStatement->AddValue(value);
    }
    for (const std::unique_ptr<Cm::Ast::StatementNode>& statement : caseStatementNode.Statements())
    {
        if (TerminatesCase(statement.get())) return;
    }
    throw Cm::Core::Exception("case must end in break, continue, return, throw, goto, goto case or goto default statement", caseStatementNode.GetSpan());
}

DefaultStatementBinder::DefaultStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundDefaultStatement* defaultStatement_) : StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_),
    defaultStatement(defaultStatement_)
{
}

void DefaultStatementBinder::EndVisit(Cm::Ast::DefaultStatementNode& defaultStatementNode)
{
    for (const std::unique_ptr<Cm::Ast::StatementNode>& statement : defaultStatementNode.Statements())
    {
        if (TerminatesCase(statement.get())) return;
    }
    throw Cm::Core::Exception("default case must end in break, continue, return, throw goto, goto case or goto default statement", defaultStatementNode.GetSpan());
}

BreakStatementBinder::BreakStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_) : StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_)
{
}

void BreakStatementBinder::Visit(Cm::Ast::BreakStatementNode& breakStatementNode)
{
    Cm::Ast::Node* parent = breakStatementNode.Parent();
    while (parent && !parent->IsBreakEnclosingStatementNode())
    {
        parent = parent->Parent();
    }
    if (!parent)
    {
        throw Cm::Core::Exception("break statement must be enclosed in while, do, for or switch statement", breakStatementNode.GetSpan());
    }
    Cm::BoundTree::BoundBreakStatement* boundBreakStatement = new Cm::BoundTree::BoundBreakStatement(&breakStatementNode);
    SetResult(boundBreakStatement);
}

ContinueStatementBinder::ContinueStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_) : 
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_)
{
}

void ContinueStatementBinder::Visit(Cm::Ast::ContinueStatementNode& continueStatementNode)
{
    Cm::Ast::Node* parent = continueStatementNode.Parent();
    while (parent && !parent->IsContinueEnclosingStatementNode())
    {
        parent = parent->Parent();
    }
    if (!parent)
    {
        throw Cm::Core::Exception("continue statement must be enclosed in while, do, or for statement", continueStatementNode.GetSpan());
    }
    Cm::BoundTree::BoundContinueStatement* boundContinueStatement = new Cm::BoundTree::BoundContinueStatement(&continueStatementNode);
    SetResult(boundContinueStatement);
}

GotoCaseStatementBinder::GotoCaseStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_, Cm::BoundTree::BoundSwitchStatement* switchStatement_) :
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), switchStatement(switchStatement_)
{
}

void GotoCaseStatementBinder::EndVisit(Cm::Ast::GotoCaseStatementNode& gotoCaseStatementNode)
{
    Cm::Ast::Node* parent = gotoCaseStatementNode.Parent();
    while (parent && !parent->IsCaseStatementNode() && !parent->IsDefaultStatementNode())
    {
        parent = parent->Parent();
    }
    if (!parent)
    {
        throw Cm::Core::Exception("goto case statement must be enclosed in case or default statement", gotoCaseStatementNode.GetSpan());
    }
    Cm::Sym::TypeSymbol* condType = switchStatement->Condition()->GetType();
    if (condType->IsEnumTypeSymbol())
    {
        Cm::Sym::EnumTypeSymbol* enumTypeSymbol = static_cast<Cm::Sym::EnumTypeSymbol*>(condType);
        condType = enumTypeSymbol->GetUnderlyingType();
    }
    Cm::Sym::SymbolType symbolType = condType->GetSymbolType();
    Cm::Sym::ValueType valueType = Cm::Sym::GetValueTypeFor(symbolType);
    Cm::Sym::Value* value = Evaluate(valueType, false, gotoCaseStatementNode.TargetCaseExpr(), SymbolTable(), ContainerScope(), FileScopes(), BoundCompileUnit().ClassTemplateRepository());
    Cm::BoundTree::BoundGotoCaseStatement* boundGotoCasetatement = new Cm::BoundTree::BoundGotoCaseStatement(&gotoCaseStatementNode);
    boundGotoCasetatement->SetValue(value);
    SetResult(boundGotoCasetatement);
}

GotoDefaultStatementBinder::GotoDefaultStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_) : 
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_)
{
}

void GotoDefaultStatementBinder::Visit(Cm::Ast::GotoDefaultStatementNode& gotoDefaultStatementNode)
{
    Cm::Ast::Node* parent = gotoDefaultStatementNode.Parent();
    while (parent && !parent->IsCaseStatementNode())
    {
        parent = parent->Parent();
    }
    if (!parent)
    {
        throw Cm::Core::Exception("goto default statement must be enclosed in case statement", gotoDefaultStatementNode.GetSpan());
    }
    Cm::BoundTree::BoundGotoDefaultStatement* boundGotoDefaultStatement = new Cm::BoundTree::BoundGotoDefaultStatement(&gotoDefaultStatementNode);
    SetResult(boundGotoDefaultStatement);
}

DestroyStatementBinder::DestroyStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_,
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_) : StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_)
{
}

void DestroyStatementBinder::EndVisit(Cm::Ast::DestroyStatementNode& destroyStatementNode)
{
    std::unique_ptr<Cm::BoundTree::BoundExpression> ptr(Pop());
    Cm::Sym::TypeSymbol* type = ptr->GetType();
    if (!type->IsPointerType())
    {
        throw Cm::Core::Exception("destroy statement needs pointer type operand", destroyStatementNode.GetSpan());
    }
    if (type->GetPointerCount() == 1)
    {
        Cm::Sym::TypeSymbol* baseType = type->GetBaseType();
        if (baseType->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* classType = static_cast<Cm::Sym::ClassTypeSymbol*>(baseType);
            Cm::Sym::FunctionSymbol* destructor = classType->Destructor();
            if (destructor)
            {
                Cm::BoundTree::BoundExpressionList arguments;
                arguments.Add(ptr.release());
                Cm::BoundTree::BoundFunctionCallStatement* destructionStatement = new Cm::BoundTree::BoundFunctionCallStatement(destructor, std::move(arguments));
                destructionStatement->SetTraceCallInfo(Cm::Bind::CreateTraceCallInfo(BoundCompileUnit(), CurrentFunction()->GetFunctionSymbol(), destroyStatementNode.GetSpan()));
                SetResult(destructionStatement);
                return;
            }
        }
    }
    SetResult(nullptr);
}

DeleteStatementBinder::DeleteStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_,
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_) : 
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), freeStatement(nullptr)
{
}

void DeleteStatementBinder::EndVisit(Cm::Ast::DeleteStatementNode& deleteStatementNode)
{
    bool resultSet = false;
    std::unique_ptr<Cm::BoundTree::BoundExpression> ptr(Pop());
    Cm::Sym::TypeSymbol* type = ptr->GetType();
    if (!type->IsPointerType())
    {
        throw Cm::Core::Exception("delete statement needs pointer type operand", deleteStatementNode.GetSpan());
    }
    if (type->GetPointerCount() == 1)
    {
        Cm::Sym::TypeSymbol* baseType = type->GetBaseType();
        if (baseType->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* classType = static_cast<Cm::Sym::ClassTypeSymbol*>(baseType);
            Cm::Sym::FunctionSymbol* destructor = classType->Destructor();
            if (destructor)
            {
                Cm::BoundTree::BoundExpressionList arguments;
                arguments.Add(ptr.release());
                Cm::BoundTree::BoundFunctionCallStatement* destructionStatement = new Cm::BoundTree::BoundFunctionCallStatement(destructor, std::move(arguments));
                destructionStatement->SetTraceCallInfo(Cm::Bind::CreateTraceCallInfo(BoundCompileUnit(), CurrentFunction()->GetFunctionSymbol(), deleteStatementNode.GetSpan()));
                SetResult(destructionStatement);
                resultSet = true;
            }
        }
    }
    if (!resultSet)
    {
        SetResult(nullptr);
    }
    deleteStatementNode.PointerExpr()->Accept(*this);
    std::unique_ptr<Cm::BoundTree::BoundExpression> mem(Pop());
    std::vector<Cm::Core::Argument> resolutionArguments;
    resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, mem->GetType()));
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_parent, ContainerScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* memFreeFun = ResolveOverload(ContainerScope(), BoundCompileUnit(), "System.Support.MemFree", resolutionArguments, functionLookups, deleteStatementNode.GetSpan(), conversions);
    if (conversions.size() != 1)
    {
        throw std::runtime_error("wrong number of conversions");
    }
    Cm::BoundTree::BoundExpressionList arguments;
    if (conversions[0])
    {
        arguments.Add(CreateBoundConversion(ContainerScope(), BoundCompileUnit(), &deleteStatementNode, mem.release(), conversions[0], CurrentFunction()));
    }
    else
    {
        arguments.Add(mem.release());
    }
    freeStatement = new Cm::BoundTree::BoundFunctionCallStatement(memFreeFun, std::move(arguments));
}

ThrowStatementBinder::ThrowStatementBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_,
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_, Binder& binder_) :
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), binder(binder_)
{
}

void ThrowStatementBinder::EndVisit(Cm::Ast::ThrowStatementNode& throwStatementNode)
{
    Cm::Ast::TryStatementNode* ownerTry = CurrentFunction()->GetCurrentTry();
    if (ownerTry == nullptr && CurrentFunction()->GetFunctionSymbol()->IsNothrow())
    {
        throw Cm::Core::Exception("nothrow function cannot throw", throwStatementNode.GetSpan(), CurrentFunction()->GetFunctionSymbol()->GetSpan());
    }
    std::unique_ptr<Cm::BoundTree::BoundExpression> exceptionExpr(Pop());
    Cm::Sym::TypeSymbol* exceptionType = exceptionExpr->GetType();
    Cm::Sym::TypeSymbol* plainExceptionType = SymbolTable().GetTypeRepository().MakePlainType(exceptionType);
    if (plainExceptionType->IsClassTypeSymbol())
    {
        Cm::Sym::ClassTypeSymbol* exceptionClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(plainExceptionType);
        if (exceptionClassType->IsAbstract())
        {
            throw Cm::Core::Exception("cannot instantiate an abstract class", exceptionClassType->GetSpan(), throwStatementNode.GetSpan());
        }
        Cm::Sym::Symbol* systemExceptionSymbol = SymbolTable().GlobalScope()->Lookup("System.Exception");
        if (systemExceptionSymbol)
        {
            if (systemExceptionSymbol->IsClassTypeSymbol())
            {
                Cm::Sym::ClassTypeSymbol* systemExceptionClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(systemExceptionSymbol);
                if (Cm::Sym::TypesEqual(exceptionClassType, systemExceptionClassType) || exceptionClassType->HasBaseClass(systemExceptionClassType))
                {
                    Cm::Sym::ExceptionTable* exceptionTable = Cm::Sym::GetExceptionTable();
                    exceptionTable->AddProjectException(exceptionClassType);
                    Cm::Sym::WriteExceptionIdToFile(BoundCompileUnit().IrFilePath(), exceptionClassType->FullName());
                    binder.AddBoundStatement(new Cm::BoundTree::BoundBeginThrowStatement(&throwStatementNode));
                    int32_t exceptionId = exceptionTable->GetExceptionId(exceptionClassType);
                    std::string sourceFilePath = Cm::Parser::FileRegistry::Instance()->GetParsedFileName(throwStatementNode.GetSpan().FileIndex());
                    int32_t sourceLineNumber = throwStatementNode.GetSpan().LineNumber();
                    Cm::Ast::NewNode* newEx = new Cm::Ast::NewNode(throwStatementNode.GetSpan(), new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), exceptionClassType->FullName()));
                    Cm::Ast::CloneContext cloneContext;
                    newEx->AddArgument(throwStatementNode.ExceptionExpr()->Clone(cloneContext));
                    std::string exVarName = CurrentFunction()->GetNextTempVariableName();
                    Cm::Ast::DerivationList pointerDerivation;
                    pointerDerivation.Add(Cm::Ast::Derivation::pointer);
                    Cm::Ast::DerivedTypeExprNode* exPtrType = new Cm::Ast::DerivedTypeExprNode(throwStatementNode.GetSpan(), pointerDerivation, 
                        new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), exceptionClassType->FullName()));
                    Cm::Ast::ConstructionStatementNode* constructEx = new Cm::Ast::ConstructionStatementNode(throwStatementNode.GetSpan(), exPtrType, 
                        new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), exVarName));
                    constructEx->AddArgument(newEx);
                    Cm::Ast::InvokeNode* setExceptionTypeExpr = new Cm::Ast::InvokeNode(throwStatementNode.GetSpan(),
                        new Cm::Ast::ArrowNode(throwStatementNode.GetSpan(), new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), exVarName),
                        new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), "SetExceptionType")));
                    Cm::Ast::DerefNode* derefExVar = new Cm::Ast::DerefNode(throwStatementNode.GetSpan(), new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), exVarName));
                    Cm::Ast::TypeNameNode* typeNameNode = new Cm::Ast::TypeNameNode(throwStatementNode.GetSpan(), derefExVar);
                    setExceptionTypeExpr->AddArgument(typeNameNode);
                    Cm::Ast::SimpleStatementNode* setExceptionTypeStatement = new Cm::Ast::SimpleStatementNode(throwStatementNode.GetSpan(), setExceptionTypeExpr);
                    Cm::Ast::InvokeNode* setFileExpr = new Cm::Ast::InvokeNode(throwStatementNode.GetSpan(),
                        new Cm::Ast::ArrowNode(throwStatementNode.GetSpan(), new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), exVarName),
                        new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), "SetFile")));
                    Cm::Ast::StringLiteralNode* sourceFilePathArg = new Cm::Ast::StringLiteralNode(throwStatementNode.GetSpan(), sourceFilePath);
                    setFileExpr->AddArgument(sourceFilePathArg);
                    Cm::Ast::SimpleStatementNode* setFileStatement = new Cm::Ast::SimpleStatementNode(throwStatementNode.GetSpan(), setFileExpr);
                    Cm::Ast::InvokeNode* setLineExpr = new Cm::Ast::InvokeNode(throwStatementNode.GetSpan(),
                        new Cm::Ast::ArrowNode(throwStatementNode.GetSpan(), new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), exVarName),
                        new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), "SetLine")));
                    setLineExpr->AddArgument(new Cm::Ast::IntLiteralNode(throwStatementNode.GetSpan(), sourceLineNumber));
                    Cm::Ast::SimpleStatementNode* setLineStatement = new Cm::Ast::SimpleStatementNode(throwStatementNode.GetSpan(), setLineExpr);

                    Cm::Ast::InvokeNode* beginCaptureCallStackCall = new Cm::Ast::InvokeNode(throwStatementNode.GetSpan(),
                        new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), "begin_capture_call_stack"));
                    Cm::Ast::SimpleStatementNode* beginCaptureCallStackStatement = new Cm::Ast::SimpleStatementNode(throwStatementNode.GetSpan(), beginCaptureCallStackCall);
                    Cm::Ast::InvokeNode* captureCallStackCall = new Cm::Ast::InvokeNode(throwStatementNode.GetSpan(), 
                        new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), "capture_call_stack"));
                    Cm::Ast::InvokeNode* setCallStackExpr = new Cm::Ast::InvokeNode(throwStatementNode.GetSpan(),
                        new Cm::Ast::ArrowNode(throwStatementNode.GetSpan(), new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), exVarName),
                        new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), "SetCallStack")));
                    setCallStackExpr->AddArgument(captureCallStackCall);
                    Cm::Ast::SimpleStatementNode* setCallStackStatement = new Cm::Ast::SimpleStatementNode(throwStatementNode.GetSpan(), setCallStackExpr);
                    Cm::Ast::InvokeNode* endCaptureCallStackCall = new Cm::Ast::InvokeNode(throwStatementNode.GetSpan(),
                        new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), "end_capture_call_stack"));
                    Cm::Ast::SimpleStatementNode* endCaptureCallStackStatement = new Cm::Ast::SimpleStatementNode(throwStatementNode.GetSpan(), endCaptureCallStackCall);

                    Cm::Ast::InvokeNode* callThisThreadExpr = new Cm::Ast::InvokeNode(throwStatementNode.GetSpan(), 
                        new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), "this_thread"));
                    Cm::Ast::InvokeNode* callGetExceptionTableAddrExpr = new Cm::Ast::InvokeNode(throwStatementNode.GetSpan(),
                        new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), "get_exception_table_addr"));
                    callGetExceptionTableAddrExpr->AddArgument(callThisThreadExpr);
                    Cm::Ast::InvokeNode* callSetExceptionAddrExpr = new Cm::Ast::InvokeNode(throwStatementNode.GetSpan(),
                        new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), "System.Support.SetExceptionAddr"));
                    callSetExceptionAddrExpr->AddArgument(callGetExceptionTableAddrExpr);
                    Cm::Ast::IntLiteralNode* exceptionIdArg = new Cm::Ast::IntLiteralNode(throwStatementNode.GetSpan(), exceptionId);
                    callSetExceptionAddrExpr->AddArgument(exceptionIdArg);
                    callSetExceptionAddrExpr->AddArgument(new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), exVarName));
                    Cm::Ast::SimpleStatementNode* setExceptionAddrStatement = new Cm::Ast::SimpleStatementNode(throwStatementNode.GetSpan(), callSetExceptionAddrExpr);

                    Cm::Ast::AssignmentStatementNode* setExceptionCodeStatement = new Cm::Ast::AssignmentStatementNode(throwStatementNode.GetSpan(),
                        new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), Cm::IrIntf::GetExCodeVarName()),
                        new Cm::Ast::IntLiteralNode(throwStatementNode.GetSpan(), exceptionId));
                    Cm::Ast::AssignmentStatementNode* setExceptionCodeParamStatement = nullptr;
                    Cm::Ast::StatementNode* endOfThrowStatement = nullptr;
                    Cm::Ast::FunctionNode* functionNode = throwStatementNode.GetFunction();
                    Cm::Ast::ExitTryStatementNode* exitTryStatement = nullptr;
                    Cm::Ast::TryStatementNode* ownerTry = nullptr; 
                    if (CurrentFunction()->InHandler())
                    {
                        ownerTry = CurrentFunction()->GetParentTry();
                    }
                    else
                    {
                        ownerTry = CurrentFunction()->GetCurrentTry();
                    }
                    if (ownerTry)
                    {
                        int catchId = ownerTry->GetFirstHandler()->CatchId();
                        if (catchId == -1)
                        {
                            catchId = CurrentFunction()->GetNextCatchId();
                            ownerTry->SetFirstCatchId(catchId);
                        }
                        exitTryStatement = new Cm::Ast::ExitTryStatementNode(throwStatementNode.GetSpan(), ownerTry);
                        std::string continueLabelPrefix;
                        Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
                        if (backend == Cm::IrIntf::BackEnd::llvm)
                        {
                            continueLabelPrefix = "$C";
                        }
                        else if (backend == Cm::IrIntf::BackEnd::c)
                        {
                            continueLabelPrefix = "_C_";
                        }
                        Cm::Ast::GotoStatementNode* gotoCatch = new Cm::Ast::GotoStatementNode(throwStatementNode.GetSpan(), new Cm::Ast::LabelNode(throwStatementNode.GetSpan(), continueLabelPrefix + std::to_string(catchId)));
                        gotoCatch->SetExceptionHandlingGoto();
                        endOfThrowStatement = gotoCatch;
                    }
                    else
                    {
                        setExceptionCodeParamStatement = new Cm::Ast::AssignmentStatementNode(throwStatementNode.GetSpan(),
                            new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), Cm::IrIntf::GetExceptionCodeParamName()),
                            new Cm::Ast::IdentifierNode(throwStatementNode.GetSpan(), Cm::IrIntf::GetExCodeVarName()));
                        Cm::Ast::Node* returnTypeExpr = functionNode->ReturnTypeExpr();
                        if (returnTypeExpr && returnTypeExpr->GetNodeType() != Cm::Ast::NodeType::voidNode)
                        {
                            Cm::Ast::CloneContext cloneContext;
                            Cm::Ast::InvokeNode* defaultValue = new Cm::Ast::InvokeNode(throwStatementNode.GetSpan(), returnTypeExpr->Clone(cloneContext));
                            endOfThrowStatement = new Cm::Ast::ReturnStatementNode(throwStatementNode.GetSpan(), defaultValue);
                        }
                        else
                        {
                            endOfThrowStatement = new Cm::Ast::ReturnStatementNode(throwStatementNode.GetSpan(), nullptr);
                        }
                    }

                    Cm::Ast::CompoundStatementNode* throwActions = new Cm::Ast::CompoundStatementNode(throwStatementNode.GetSpan());
                    CurrentFunction()->Own(throwActions);
                    throwActions->SetLabelNode(throwStatementNode.Label());
                    throwActions->SetParent(functionNode);
                    throwActions->AddStatement(constructEx);
                    throwActions->AddStatement(setExceptionTypeStatement);
                    throwActions->AddStatement(setFileStatement);
                    throwActions->AddStatement(setLineStatement);
                    throwActions->AddStatement(beginCaptureCallStackStatement);
                    throwActions->AddStatement(setCallStackStatement);
                    throwActions->AddStatement(endCaptureCallStackStatement);
                    throwActions->AddStatement(setExceptionAddrStatement);
                    throwActions->AddStatement(setExceptionCodeStatement);
                    if (setExceptionCodeParamStatement)
                    {
                        throwActions->AddStatement(setExceptionCodeParamStatement);
                    }
                    if (exitTryStatement)
                    {
                        throwActions->AddStatement(exitTryStatement);
                    }
                    throwActions->AddStatement(endOfThrowStatement);
                    Cm::Sym::DeclarationVisitor declarationVisitor(SymbolTable());
                    throwActions->Accept(declarationVisitor);
                    Cm::Sym::ContainerScope* containerScope = SymbolTable().GetContainerScope(throwActions);
                    containerScope->SetParent(ContainerScope());
                    binder.BeginContainerScope(containerScope);
                    throwActions->Accept(binder);
                    binder.EndContainerScope();
                    binder.AddBoundStatement(new Cm::BoundTree::BoundEndThrowStatement(&throwStatementNode));
                }
                else
                {
                    throw Cm::Core::Exception("exception must be class type equal to or derived from System.Exception class", throwStatementNode.GetSpan());
                }
            }
            else
            {
                throw std::runtime_error("System.Exception symbol not class type");
            }
        }
        else
        {
            throw std::runtime_error("System.Exception symbol not found");
        }
    }
    else
    {
        throw Cm::Core::Exception("type of exception must be class type", throwStatementNode.GetSpan());
    }
}

TryBinder::TryBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_, Binder& binder_) : StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), binder(binder_)
{
}

void TryBinder::Visit(Cm::Ast::TryStatementNode& tryStatementNode)
{
    CurrentFunction()->PushTryNode(&tryStatementNode);
    Cm::Ast::CatchNode* firstHandler = tryStatementNode.GetFirstHandler();
    if (firstHandler->CatchId() == -1)
    {
        firstHandler->SetCatchId(CurrentFunction()->GetNextCatchId());
    }
    CurrentFunction()->AddTryCompound(&tryStatementNode, binder.GetCurrentCompound());
    binder.AddBoundStatement(new Cm::BoundTree::BoundBeginTryStatement(&tryStatementNode, firstHandler->CatchId()));
    int continueId = CurrentFunction()->GetNextCatchId();
    std::string continueLabelPrefix;
    Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
    if (backend == Cm::IrIntf::BackEnd::llvm)
    {
        continueLabelPrefix = "$C";
    }
    else if (backend == Cm::IrIntf::BackEnd::c)
    {
        continueLabelPrefix = "_C_";
    }
    std::string continueLabel = continueLabelPrefix + std::to_string(continueId);
    Cm::Sym::DeclarationVisitor declarationVisitor(SymbolTable());
    tryStatementNode.TryBlock()->Accept(declarationVisitor);
    Cm::Sym::ContainerScope* containerScope = SymbolTable().GetContainerScope(tryStatementNode.TryBlock());
    containerScope->SetParent(ContainerScope());
    binder.BeginContainerScope(containerScope);
    tryStatementNode.TryBlock()->Accept(binder);
    binder.EndContainerScope();
    binder.AddBoundStatement(new Cm::BoundTree::BoundEndTryStatement(&tryStatementNode));
    Cm::Ast::GotoStatementNode* gotoOverCatches = new Cm::Ast::GotoStatementNode(Span(0, 0, 0, 0), new Cm::Ast::LabelNode(Span(0, 0, 0, 0), continueLabel));
    gotoOverCatches->SetExceptionHandlingGoto();
    CurrentFunction()->Own(gotoOverCatches);
    gotoOverCatches->Accept(binder);
    CurrentFunction()->PopTryNode();
    tryStatementNode.Handlers().Accept(binder);
    Cm::Ast::SimpleStatementNode* emptyStatement = new Cm::Ast::SimpleStatementNode(Span(0, 0, 0, 0), nullptr);
    CurrentFunction()->Own(emptyStatement);
    emptyStatement->SetLabelNode(new Cm::Ast::LabelNode(tryStatementNode.GetSpan(), continueLabel));
    emptyStatement->Accept(binder);
}

CatchBinder::CatchBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_, Binder& binder_) : StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), binder(binder_)
{
}

void CatchBinder::Visit(Cm::Ast::CatchNode& catchNode)
{
    CurrentFunction()->PushHandler();
    if (catchNode.CatchId() == -1)
    {
        catchNode.SetCatchId(CurrentFunction()->GetNextCatchId());
    }
    catchNode.ExceptionTypeExpr()->Accept(*this);
    std::unique_ptr<Cm::BoundTree::BoundExpression> exceptionTypeExpr(Pop());
    if (exceptionTypeExpr->IsBoundTypeExpression())
    {
        Cm::BoundTree::BoundTypeExpression* boundTypeExpression = static_cast<Cm::BoundTree::BoundTypeExpression*>(exceptionTypeExpr.get());
        Cm::Sym::TypeSymbol* catchedExceptionType = boundTypeExpression->Symbol();
        Cm::Sym::TypeSymbol* plainCatchedExceptionType = SymbolTable().GetTypeRepository().MakePlainType(catchedExceptionType);
        if (plainCatchedExceptionType->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* catchedExceptionClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(plainCatchedExceptionType);
            if (catchedExceptionClassType->IsAbstract())
            {
                throw Cm::Core::Exception("cannot instantiate an abstract class", catchedExceptionClassType->GetSpan(), catchNode.GetSpan());
            }
            Cm::Sym::Symbol* systemExceptionSymbol = SymbolTable().GlobalScope()->Lookup("System.Exception");
            if (systemExceptionSymbol)
            {
                if (systemExceptionSymbol->IsClassTypeSymbol())
                {
                    Cm::Sym::ClassTypeSymbol* systemExceptionClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(systemExceptionSymbol);
                    if (Cm::Sym::TypesEqual(catchedExceptionClassType, systemExceptionClassType) || catchedExceptionClassType->HasBaseClass(systemExceptionClassType))
                    {
                        Cm::Sym::ExceptionTable* exceptionTable = Cm::Sym::GetExceptionTable();
                        exceptionTable->AddProjectException(catchedExceptionClassType);
                        Cm::Sym::WriteExceptionIdToFile(BoundCompileUnit().IrFilePath(), catchedExceptionClassType->FullName());
                        binder.AddBoundStatement(new Cm::BoundTree::BoundPushGenDebugInfoStatement(&catchNode, false));
                        int32_t catchedExceptionId = exceptionTable->GetExceptionId(catchedExceptionClassType);
                        Cm::Ast::IdentifierNode* handleVarId = new Cm::Ast::IdentifierNode(catchNode.GetSpan(), CurrentFunction()->GetNextTempVariableName());
                        Cm::Ast::ConstructionStatementNode* handleThisExStatement = new Cm::Ast::ConstructionStatementNode(catchNode.GetSpan(), new Cm::Ast::BoolNode(catchNode.GetSpan()), handleVarId);
                        std::string continueLabelPrefix;
                        Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
                        if (backend == Cm::IrIntf::BackEnd::llvm)
                        {
                            continueLabelPrefix = "$C";
                        }
                        else if (backend == Cm::IrIntf::BackEnd::c)
                        {
                            continueLabelPrefix = "_C_";
                        }
                        handleThisExStatement->SetLabelNode(new Cm::Ast::LabelNode(catchNode.GetSpan(), continueLabelPrefix + std::to_string(catchNode.CatchId())));
                        Cm::Ast::InvokeNode* handleThisExCall = new Cm::Ast::InvokeNode(catchNode.GetSpan(), new Cm::Ast::IdentifierNode(catchNode.GetSpan(), "System.Support.HandleThisEx"));
                        handleThisExCall->AddArgument(new Cm::Ast::IdentifierNode(catchNode.GetSpan(), Cm::IrIntf::GetExceptionBaseIdTableName()));
                        handleThisExCall->AddArgument(new Cm::Ast::IdentifierNode(catchNode.GetSpan(), Cm::IrIntf::GetExCodeVarName()));
                        Cm::Ast::CloneContext cloneContext;
                        handleThisExCall->AddArgument(new Cm::Ast::IntLiteralNode(catchNode.GetSpan(), catchedExceptionId));
                        handleThisExStatement->AddArgument(handleThisExCall);
                        Cm::Ast::CompoundStatementNode* handlerBlock = new Cm::Ast::CompoundStatementNode(catchNode.GetSpan());
                        handlerBlock->SetBeginBraceSpan(Span(0, 0, 0, 0));
                        handlerBlock->SetEndBraceSpan(Span(0, 0, 0, 0));
                        CurrentFunction()->Own(handlerBlock);
                        Cm::Ast::FunctionNode* functionNode = catchNode.GetFunction();
                        handlerBlock->SetParent(functionNode);

                        handlerBlock->AddStatement(handleThisExStatement);
                        Cm::Ast::Node* parent = catchNode.Parent();
                        if (!parent->IsTryStatementNode())
                        {
                            throw std::runtime_error("not try node");
                        }
                        Cm::Ast::TryStatementNode* tryStatementNode = static_cast<Cm::Ast::TryStatementNode*>(parent);
                        if (tryStatementNode->IsLastHandler(&catchNode))
                        {
                            Cm::Ast::CompoundStatementNode* propagateExStatement = new Cm::Ast::CompoundStatementNode(catchNode.GetSpan());
                            Cm::Ast::TryStatementNode* parentTry = CurrentFunction()->GetParentTry();
                            if (parentTry)
                            {
                                Cm::Ast::CatchNode* outerHandler = parentTry->GetFirstHandler();
                                if (outerHandler->CatchId() == -1)
                                {
                                    outerHandler->SetCatchId(CurrentFunction()->GetNextCatchId());
                                }
                                Cm::Ast::ExitTryStatementNode* exitTryStatement = new Cm::Ast::ExitTryStatementNode(catchNode.GetSpan(), parentTry);
                                propagateExStatement->AddStatement(exitTryStatement);
                                std::string continueLabelPrefix;
                                Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
                                if (backend == Cm::IrIntf::BackEnd::llvm)
                                {
                                    continueLabelPrefix = "$C";
                                }
                                else if (backend == Cm::IrIntf::BackEnd::c)
                                {
                                    continueLabelPrefix = "_C_";
                                }
                                Cm::Ast::GotoStatementNode* gotoOuterCatch = new Cm::Ast::GotoStatementNode(catchNode.GetSpan(),
                                    new Cm::Ast::LabelNode(catchNode.GetSpan(), continueLabelPrefix + std::to_string(outerHandler->CatchId())));
                                gotoOuterCatch->SetExceptionHandlingGoto();
                                propagateExStatement->AddStatement(gotoOuterCatch);
                            }
                            else
                            {
                                if (CurrentFunction()->GetFunctionSymbol()->CanThrow())
                                {
                                    Cm::Ast::AssignmentStatementNode* setExceptionCodeParamStatement = new Cm::Ast::AssignmentStatementNode(catchNode.GetSpan(),
                                        new Cm::Ast::IdentifierNode(catchNode.GetSpan(), Cm::IrIntf::GetExceptionCodeParamName()),
                                        new Cm::Ast::IdentifierNode(catchNode.GetSpan(), Cm::IrIntf::GetExCodeVarName()));
                                    propagateExStatement->AddStatement(setExceptionCodeParamStatement);
                                }
                                Cm::Ast::Node* returnTypeExpr = functionNode->ReturnTypeExpr();
                                if (returnTypeExpr && returnTypeExpr->GetNodeType() != Cm::Ast::NodeType::voidNode)
                                {
                                    Cm::Ast::InvokeNode* defaultValue = new Cm::Ast::InvokeNode(catchNode.GetSpan(), returnTypeExpr->Clone(cloneContext));
                                    Cm::Ast::ReturnStatementNode* returnStatement = new Cm::Ast::ReturnStatementNode(catchNode.GetSpan(), defaultValue);
                                    propagateExStatement->AddStatement(returnStatement);
                                }
                                else
                                {
                                    Cm::Ast::ReturnStatementNode* returnStatement = new Cm::Ast::ReturnStatementNode(catchNode.GetSpan(), nullptr);
                                    propagateExStatement->AddStatement(returnStatement);
                                }
                            }
                            Cm::Ast::ConditionalStatementNode* dontHandleTest = new Cm::Ast::ConditionalStatementNode(catchNode.GetSpan(),
                                new Cm::Ast::NotNode(catchNode.GetSpan(), handleVarId->Clone(cloneContext)), propagateExStatement, nullptr);
                            handlerBlock->AddStatement(dontHandleTest);
                        }
                        else
                        {
                            Cm::Ast::CatchNode* nextHandler = tryStatementNode->GetNextHandler(&catchNode);
                            int nextCatchId = nextHandler->CatchId();
                            if (nextHandler->CatchId() == -1)
                            {
                                nextCatchId = CurrentFunction()->GetNextCatchId();
                                nextHandler->SetCatchId(nextCatchId);
                            }
                            std::string continueLabelPrefix;
                            Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
                            if (backend == Cm::IrIntf::BackEnd::llvm)
                            {
                                continueLabelPrefix = "$C";
                            }
                            else if (backend == Cm::IrIntf::BackEnd::c)
                            {
                                continueLabelPrefix = "_C_";
                            }
                            Cm::Ast::GotoStatementNode* gotoNextHandler = new Cm::Ast::GotoStatementNode(catchNode.GetSpan(), new Cm::Ast::LabelNode(catchNode.GetSpan(), continueLabelPrefix + std::to_string(nextCatchId)));
                            gotoNextHandler->SetExceptionHandlingGoto();
                            Cm::Ast::ConditionalStatementNode* dontHandleTest = new Cm::Ast::ConditionalStatementNode(catchNode.GetSpan(),
                                new Cm::Ast::NotNode(catchNode.GetSpan(), handleVarId->Clone(cloneContext)), gotoNextHandler, nullptr);
                            handlerBlock->AddStatement(dontHandleTest);
                        }
                        Cm::Ast::InvokeNode* callThisThreadExpr = new Cm::Ast::InvokeNode(catchNode.GetSpan(),
                            new Cm::Ast::IdentifierNode(catchNode.GetSpan(), "this_thread"));
                        Cm::Ast::InvokeNode* callGetExceptionTableAddrExpr = new Cm::Ast::InvokeNode(catchNode.GetSpan(),
                            new Cm::Ast::IdentifierNode(catchNode.GetSpan(), "get_exception_table_addr"));
                        callGetExceptionTableAddrExpr->AddArgument(callThisThreadExpr);
                        Cm::Ast::InvokeNode* callGetExceptionAddrExpr = new Cm::Ast::InvokeNode(catchNode.GetSpan(),
                            new Cm::Ast::IdentifierNode(catchNode.GetSpan(), "System.Support.GetExceptionAddr"));
                        callGetExceptionAddrExpr->AddArgument(callGetExceptionTableAddrExpr);
                        callGetExceptionAddrExpr->AddArgument(new Cm::Ast::IdentifierNode(catchNode.GetSpan(), Cm::IrIntf::GetExCodeVarName()));
                        Cm::Ast::DerivationList pointerDerivation;
                        pointerDerivation.Add(Cm::Ast::Derivation::pointer);
                        Cm::Ast::DerivedTypeExprNode* exPtrType = new Cm::Ast::DerivedTypeExprNode(catchNode.GetSpan(), pointerDerivation, new Cm::Ast::IdentifierNode(catchNode.GetSpan(),
                            catchedExceptionClassType->FullName()));
                        std::string exPtrVarName = CurrentFunction()->GetNextTempVariableName();
                        Cm::Ast::ConstructionStatementNode* constructExPtrStatement = new Cm::Ast::ConstructionStatementNode(catchNode.GetSpan(), exPtrType,
                            new Cm::Ast::IdentifierNode(catchNode.GetSpan(), exPtrVarName));
                        constructExPtrStatement->AddArgument(new Cm::Ast::CastNode(catchNode.GetSpan(), exPtrType->Clone(cloneContext), callGetExceptionAddrExpr));
                        handlerBlock->AddStatement(constructExPtrStatement);
                        Cm::Ast::InvokeNode* invokeSetCurrentExceptionIdFun = new Cm::Ast::InvokeNode(catchNode.GetSpan(), new Cm::Ast::IdentifierNode(catchNode.GetSpan(), "set_current_exception_id"));
                        invokeSetCurrentExceptionIdFun->AddArgument(new Cm::Ast::IdentifierNode(catchNode.GetSpan(), Cm::IrIntf::GetExCodeVarName()));
                        Cm::Ast::SimpleStatementNode* setCurrentExceptionIdStatement = new Cm::Ast::SimpleStatementNode(catchNode.GetSpan(), invokeSetCurrentExceptionIdFun);
                        handlerBlock->AddStatement(setCurrentExceptionIdStatement);
                        Cm::Ast::InvokeNode* invokeSetCurrentExceptionAddrFun = new Cm::Ast::InvokeNode(catchNode.GetSpan(), new Cm::Ast::IdentifierNode(catchNode.GetSpan(), "set_current_exception_addr"));
                        invokeSetCurrentExceptionAddrFun->AddArgument(callGetExceptionAddrExpr->Clone(cloneContext));
                        Cm::Ast::SimpleStatementNode* setCurrentExceptionAddrStatement = new Cm::Ast::SimpleStatementNode(catchNode.GetSpan(), invokeSetCurrentExceptionAddrFun);
                        handlerBlock->AddStatement(setCurrentExceptionAddrStatement);
                        Cm::Ast::AssignmentStatementNode* resetExCodeStatement = new Cm::Ast::AssignmentStatementNode(catchNode.GetSpan(), 
                            new Cm::Ast::IdentifierNode(catchNode.GetSpan(), Cm::IrIntf::GetExCodeVarName()),
                            new Cm::Ast::IntLiteralNode(catchNode.GetSpan(), 0));
                        handlerBlock->AddStatement(resetExCodeStatement);
                        Cm::Ast::DerivationList constRefDerivation;
                        constRefDerivation.Add(Cm::Ast::Derivation::const_);
                        constRefDerivation.Add(Cm::Ast::Derivation::reference);
                        Cm::Ast::Node* exTypeExpr = catchNode.ExceptionTypeExpr()->Clone(cloneContext);
                        Cm::Ast::IdentifierNode* exId = nullptr;
                        if (catchNode.ExceptionId())
                        {
                            exId = static_cast<Cm::Ast::IdentifierNode*>(catchNode.ExceptionId()->Clone(cloneContext));
                        }
                        else
                        {
                            exId = new Cm::Ast::IdentifierNode(catchNode.GetSpan(), CurrentFunction()->GetNextTempVariableName());
                        }
                        Cm::Ast::ConstructionStatementNode* constructExVarStatement = new Cm::Ast::ConstructionStatementNode(catchNode.GetSpan(), exTypeExpr, exId);
                        constructExVarStatement->AddArgument(new Cm::Ast::DerefNode(catchNode.GetSpan(), new Cm::Ast::IdentifierNode(catchNode.GetSpan(), exPtrVarName)));
                        handlerBlock->AddStatement(constructExVarStatement);
                        Cm::Ast::TemplateIdNode* exDeleterType = new Cm::Ast::TemplateIdNode(catchNode.GetSpan(), new Cm::Ast::IdentifierNode(catchNode.GetSpan(), "System.Support.ExDeleter"));
                        exDeleterType->AddTemplateArgument(new Cm::Ast::IdentifierNode(catchNode.GetSpan(), catchedExceptionClassType->FullName()));
                        Cm::Ast::ConstructionStatementNode* constructExDeleterStatement = new Cm::Ast::ConstructionStatementNode(catchNode.GetSpan(), exDeleterType,
                            new Cm::Ast::IdentifierNode(catchNode.GetSpan(), CurrentFunction()->GetNextTempVariableName()));
                        constructExDeleterStatement->AddArgument(new Cm::Ast::IdentifierNode(catchNode.GetSpan(), exPtrVarName));
                        handlerBlock->AddStatement(constructExDeleterStatement);
                        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::generate_debug_info))
                        {
                            handlerBlock->AddStatement(new Cm::Ast::BeginCatchStatementNode(Span(0, 0, 0, 0)));
                        }
                        handlerBlock->AddStatement(static_cast<Cm::Ast::StatementNode*>(catchNode.CatchBlock()->Clone(cloneContext)));
                        Cm::Sym::DeclarationVisitor declarationVisitor(SymbolTable());
                        handlerBlock->Accept(declarationVisitor);
                        Cm::Sym::ContainerScope* containerScope = SymbolTable().GetContainerScope(handlerBlock);
                        containerScope->SetParent(ContainerScope());
                        binder.BeginContainerScope(containerScope);
                        handlerBlock->Accept(binder);
                        binder.EndContainerScope();
                    }
                    else
                    {
                        throw Cm::Core::Exception("exception must be class type equal to or derived from System.Exception class", catchNode.GetSpan());
                    }
                }
                else
                {
                    throw std::runtime_error("System.Exception symbol not class type");
                }
            }
            else
            {
                throw std::runtime_error("System.Exception symbol not found");
            }
        }
        else
        {
            throw Cm::Core::Exception("type of exception must be class type", catchNode.GetSpan());
        }
    }
    else
    {
        throw Cm::Core::Exception("exception type expression has no type", catchNode.GetSpan());
    }
    CurrentFunction()->PopHandler();
}

ExitTryBinder::ExitTryBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_, Binder& binder_) : StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), binder(binder_)
{
}

void ExitTryBinder::Visit(Cm::Ast::ExitTryStatementNode& exitTryStatementNode)
{
    Cm::Ast::TryStatementNode* tryNode = exitTryStatementNode.TryNode();
    Cm::BoundTree::BoundCompoundStatement* tryCompound = CurrentFunction()->GetTryCompound(tryNode);
    binder.AddBoundStatement(new Cm::BoundTree::BoundExitBlocksStatement(&exitTryStatementNode, tryCompound));
}

Cm::BoundTree::BoundConstructionStatement* CreateTracedFunConstructionStatement(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, 
    Cm::BoundTree::BoundFunction* boundFunction, const Cm::Parsing::Span& span)
{
    if (boundFunction->GetFunctionSymbol()->FullName() == "System.TracedFun.@constructor(System.TracedFun*, const char*, const char*, int)")
    {
        return nullptr;
    }
    if (boundFunction->GetFunctionSymbol()->FullName() == "System.TracedFun.@destructor(System.TracedFun*)")
    {
        return nullptr;
    }
    const std::string& currentProjectName = Cm::Core::GetGlobalSettings()->CurrentProjectName();
    if (currentProjectName.empty() || currentProjectName == "os" || currentProjectName == "support")
    {
        return nullptr;
    }
    Cm::BoundTree::BoundConstructionStatement* boundConstructionStatement = new Cm::BoundTree::BoundConstructionStatement(nullptr);
    Cm::Ast::IdentifierNode* tracedFunTypeExprNode = new Cm::Ast::IdentifierNode(span, "System.TracedFun");
    Cm::Sym::TypeSymbol* tracedFunType = ResolveType(boundCompileUnit.SymbolTable(), containerScope, boundCompileUnit.GetFileScopes(), boundCompileUnit.ClassTemplateRepository(),
        tracedFunTypeExprNode);
    boundFunction->Own(tracedFunTypeExprNode);
    if (!tracedFunType->IsClassTypeSymbol())
    {
        throw std::runtime_error("not class type");
    }
    Cm::Sym::ClassTypeSymbol* classTypeSymbol = static_cast<Cm::Sym::ClassTypeSymbol*>(tracedFunType);
    AddClassTypeToIrClassTypeRepository(classTypeSymbol, boundCompileUnit, containerScope);
    Cm::Sym::TypeSymbol* tracedFunPtrType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(tracedFunType, span);
    Cm::Sym::LocalVariableSymbol* tracedFunVar = boundFunction->CreateTempLocalVariable(tracedFunType);
    tracedFunVar->SetSid(boundCompileUnit.SymbolTable().GetSid());
    boundConstructionStatement->SetLocalVariable(tracedFunVar);
    Cm::Sym::TypeSymbol* constCharPtrType = boundCompileUnit.SymbolTable().GetTypeRepository().MakeConstCharPtrType(span);
    Cm::Sym::TypeSymbol* intType = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId));
    std::vector<Cm::Core::Argument> resolutionArguments;
    resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, tracedFunPtrType));
    resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, constCharPtrType));
    resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, constCharPtrType));
    resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, intType));
    Cm::Sym::FunctionLookupSet resolutionLookups;
    resolutionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, tracedFunType->GetContainerScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* tracedFunCtor = ResolveOverload(containerScope, boundCompileUnit, "@constructor", resolutionArguments, resolutionLookups, span, conversions);
    boundConstructionStatement->SetConstructor(tracedFunCtor);
    Cm::BoundTree::BoundExpressionList arguments;
    Cm::Sym::FunctionSymbol* fun = boundFunction->GetFunctionSymbol();
    std::string funFullName = fun->FullName();
    int funId = boundCompileUnit.StringRepository().Install(funFullName);
    Cm::BoundTree::BoundStringLiteral* funLiteral = new Cm::BoundTree::BoundStringLiteral(nullptr, funId);
    funLiteral->SetType(constCharPtrType);
    arguments.Add(funLiteral);
    std::string filePath = Cm::Parser::FileRegistry::Instance()->GetParsedFileName(span.FileIndex());
    int fileId = boundCompileUnit.StringRepository().Install(filePath);
    Cm::BoundTree::BoundStringLiteral* fileLiteral = new Cm::BoundTree::BoundStringLiteral(nullptr, fileId);
    fileLiteral->SetType(constCharPtrType);
    arguments.Add(fileLiteral);
    Cm::Sym::Value* value = new Cm::Sym::IntValue(span.LineNumber());
    Cm::BoundTree::BoundLiteral* lineLiteral = new Cm::BoundTree::BoundLiteral(nullptr);
    lineLiteral->SetValue(value);
    lineLiteral->SetType(intType);
    arguments.Add(lineLiteral);
    boundConstructionStatement->SetArguments(std::move(arguments));
    boundConstructionStatement->InsertLocalVariableToArguments();
    boundConstructionStatement->Arguments()[0]->SetFlag(Cm::BoundTree::BoundNodeFlags::constructVariable);
    PrepareArguments(containerScope, boundCompileUnit, boundFunction, nullptr, tracedFunCtor->Parameters(), boundConstructionStatement->Arguments(), true, 
        boundCompileUnit.IrClassTypeRepository(), false);
    return boundConstructionStatement;
}

AssertBinder::AssertBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::BoundTree::BoundFunction* currentFunction_) : StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_)
{
}

void AssertBinder::Visit(Cm::Ast::AssertStatementNode& assertStatementNode)
{
    Cm::Sym::TypeSymbol* constCharPtrType = BoundCompileUnit().SymbolTable().GetTypeRepository().MakeConstCharPtrType(assertStatementNode.GetSpan());
    assertStatementNode.AssertExpr()->Accept(*this);
    std::unique_ptr<Cm::BoundTree::BoundExpression> expr(Pop());
    if (!expr->GetType()->IsBoolTypeSymbol())
    {
        throw Cm::Core::Exception("#assert expression must be of type bool (now of type '" + expr->GetType()->FullName() + "')", assertStatementNode.GetSpan());
    }
    Cm::BoundTree::BoundExpressionList failAssertionArguments;
    std::string assertExpr = assertStatementNode.AssertExpr()->ToString();
    int assertExprId = BoundCompileUnit().StringRepository().Install(assertExpr);
    Cm::BoundTree::BoundStringLiteral* assertExprLiteral = new Cm::BoundTree::BoundStringLiteral(&assertStatementNode, assertExprId);
    assertExprLiteral->SetType(constCharPtrType);
    failAssertionArguments.Add(assertExprLiteral);
    std::string filePath = Cm::Parser::FileRegistry::Instance()->GetParsedFileName(assertStatementNode.GetSpan().FileIndex());
    int fileId = BoundCompileUnit().StringRepository().Install(filePath);
    Cm::BoundTree::BoundStringLiteral* fileLiteral = new Cm::BoundTree::BoundStringLiteral(&assertStatementNode, fileId);
    fileLiteral->SetType(constCharPtrType);
    failAssertionArguments.Add(fileLiteral);
    std::string line = std::to_string(assertStatementNode.GetSpan().LineNumber());
    int lineId = BoundCompileUnit().StringRepository().Install(line);
    Cm::BoundTree::BoundStringLiteral* lineLiteral = new Cm::BoundTree::BoundStringLiteral(&assertStatementNode, lineId);
    lineLiteral->SetType(constCharPtrType);
    failAssertionArguments.Add(lineLiteral);
    Cm::BoundTree::BoundUnaryOp* notAssertExpr = new Cm::BoundTree::BoundUnaryOp(&assertStatementNode, expr.release());
    Cm::Sym::TypeSymbol* boolType = BoundCompileUnit().SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
    notAssertExpr->SetType(boolType);
    std::vector<Cm::Core::Argument> notAssertExprArgs;
    notAssertExprArgs.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, boolType));
    Cm::Sym::FunctionLookupSet notAssertExprLookups;
    notAssertExprLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, BoundCompileUnit().SymbolTable().GlobalScope()));
    std::vector<Cm::Sym::FunctionSymbol*> notAssertExprConversions;
    Cm::Sym::FunctionSymbol* notAssertFun = Cm::Bind::ResolveOverload(BoundCompileUnit().SymbolTable().GlobalScope(), BoundCompileUnit(), "operator!", notAssertExprArgs, notAssertExprLookups, 
        assertStatementNode.GetSpan(), notAssertExprConversions);
    notAssertExpr->SetFunction(notAssertFun);
    notAssertExpr->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
    Cm::BoundTree::BoundFunctionCall* failAssertionExpr = new Cm::BoundTree::BoundFunctionCall(&assertStatementNode, std::move(failAssertionArguments));
    failAssertionExpr->SetType(BoundCompileUnit().SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)));
    Cm::Sym::FunctionSymbol* failAssertionFun = BoundCompileUnit().SymbolTable().GetOverload("System.Support.FailAssertion");
    failAssertionExpr->SetFunction(failAssertionFun);
    Cm::BoundTree::BoundConditionalStatement* testAssertExprStatement = new Cm::BoundTree::BoundConditionalStatement(&assertStatementNode);
    testAssertExprStatement->SetCondition(notAssertExpr);
    Cm::BoundTree::BoundSimpleStatement* callFailAssertionStatement = new Cm::BoundTree::BoundSimpleStatement(&assertStatementNode);
    callFailAssertionStatement->SetExpression(failAssertionExpr);
    testAssertExprStatement->AddStatement(callFailAssertionStatement);
    SetResult(testAssertExprStatement);
}

UnitTestAssertBinder::UnitTestAssertBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_,
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_) : 
    StatementBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_)
{
}

void UnitTestAssertBinder::Visit(Cm::Ast::AssertStatementNode& assertStatementNode)
{
    Cm::Sym::TypeSymbol* constCharPtrType = BoundCompileUnit().SymbolTable().GetTypeRepository().MakeConstCharPtrType(assertStatementNode.GetSpan());
    assertStatementNode.AssertExpr()->Accept(*this);
    std::unique_ptr<Cm::BoundTree::BoundExpression> expr(Pop());
    Cm::BoundTree::BoundConditionalStatement* testAssertExprStatement = new Cm::BoundTree::BoundConditionalStatement(&assertStatementNode);
    expr->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
    testAssertExprStatement->SetCondition(expr.release());
    Cm::BoundTree::BoundSimpleStatement* callPassAssertionStatement = new Cm::BoundTree::BoundSimpleStatement(&assertStatementNode);
    Cm::BoundTree::BoundExpressionList passAssertionArguments;
    std::string assertExpr = assertStatementNode.AssertExpr()->ToString();
    int assertExprId = BoundCompileUnit().StringRepository().Install(assertExpr);
    Cm::BoundTree::BoundStringLiteral* assertExprLiteral = new Cm::BoundTree::BoundStringLiteral(&assertStatementNode, assertExprId);
    assertExprLiteral->SetType(constCharPtrType);
    passAssertionArguments.Add(assertExprLiteral);
    Cm::BoundTree::BoundFunctionCall* passAssertionCall = new Cm::BoundTree::BoundFunctionCall(&assertStatementNode, std::move(passAssertionArguments));
    passAssertionCall->SetType(BoundCompileUnit().SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)));
    Cm::Sym::FunctionSymbol* passUnitTestAssertionFun = BoundCompileUnit().SymbolTable().GetOverload("pass_unit_test_assertion");
    passAssertionCall->SetFunction(passUnitTestAssertionFun);
    callPassAssertionStatement->SetExpression(passAssertionCall);
    testAssertExprStatement->AddStatement(callPassAssertionStatement);
    Cm::BoundTree::BoundSimpleStatement* callFailAssertionStatement = new Cm::BoundTree::BoundSimpleStatement(&assertStatementNode);
    Cm::BoundTree::BoundExpressionList failAssertionArguments;
    Cm::BoundTree::BoundStringLiteral* assertExprLiteral2 = new Cm::BoundTree::BoundStringLiteral(&assertStatementNode, assertExprId);
    assertExprLiteral2->SetType(constCharPtrType);
    failAssertionArguments.Add(assertExprLiteral2);
    std::string filePath = Cm::Parser::FileRegistry::Instance()->GetParsedFileName(assertStatementNode.GetSpan().FileIndex());
    int fileId = BoundCompileUnit().StringRepository().Install(filePath);
    Cm::BoundTree::BoundStringLiteral* fileLiteral = new Cm::BoundTree::BoundStringLiteral(&assertStatementNode, fileId);
    fileLiteral->SetType(constCharPtrType);
    failAssertionArguments.Add(fileLiteral);
    int line = assertStatementNode.GetSpan().LineNumber();
    Cm::BoundTree::BoundLiteral* lineLiteral = new Cm::BoundTree::BoundLiteral(&assertStatementNode);
    lineLiteral->SetValue(new Cm::Sym::IntValue(line));
    Cm::Sym::TypeSymbol* intType = BoundCompileUnit().SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId));
    lineLiteral->SetType(intType);
    failAssertionArguments.Add(lineLiteral);
    Cm::BoundTree::BoundFunctionCall* failAssertionCall = new Cm::BoundTree::BoundFunctionCall(&assertStatementNode, std::move(failAssertionArguments));
    failAssertionCall->SetType(BoundCompileUnit().SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)));
    Cm::Sym::FunctionSymbol* failUnitTestAssertionFun = BoundCompileUnit().SymbolTable().GetOverload("fail_unit_test_assertion");
    failAssertionCall->SetFunction(failUnitTestAssertionFun);
    callFailAssertionStatement->SetExpression(failAssertionCall);
    testAssertExprStatement->AddStatement(callFailAssertionStatement);
    SetResult(testAssertExprStatement);
}

} } // namespace Cm::Bind
