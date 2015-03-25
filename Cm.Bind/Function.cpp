/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Function.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/Parameter.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/Evaluator.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/GlobalFlags.hpp>

namespace Cm { namespace Bind {

Cm::Sym::FunctionSymbol* BindFunction(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Ast::FunctionNode* functionNode, Cm::Sym::ClassTypeSymbol* currentClass)
{
    Cm::Sym::FunctionSymbol* functionSymbol = symbolTable.GetFunctionSymbol(functionNode);
    if (!functionSymbol->Bound())
    {
        Cm::Ast::Specifiers specifiers = functionNode->GetSpecifiers();
        bool isClassMember = currentClass != nullptr;
        SetAccess(functionSymbol, specifiers, isClassMember);
    }
    return functionSymbol;
}

void CompleteBindFunction(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Ast::FunctionNode* functionNode, Cm::Sym::FunctionSymbol* functionSymbol, Cm::Sym::ClassTypeSymbol* currentClass)
{
    if (currentClass && currentClass->IsClassTemplateSymbol())
    {
        functionSymbol->SetMemberOfClassTemplate();
        return;
    }
    bool staticClass = false;
    if (currentClass && currentClass->IsStatic())
    {
        staticClass = true;
    }
    functionSymbol->ComputeName();
    if (functionSymbol->HasConstraint())
    {
        functionSymbol->SetName(functionSymbol->Name() + " " + functionSymbol->Constraint()->ToString());
    }
    else if (functionNode->Constraint())
    {
        functionSymbol->SetName(functionSymbol->Name() + " " + functionNode->Constraint()->ToString());
    }
    Cm::Ast::Specifiers specifiers = functionNode->GetSpecifiers();
    if ((specifiers & Cm::Ast::Specifiers::static_) != Cm::Ast::Specifiers::none)
    {
        if (!functionSymbol->IsMemberFunctionSymbol())
        {
            throw Cm::Core::Exception("free functions cannot be static", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsDestructor())
        {
            throw Cm::Core::Exception("destructor cannot be static", functionSymbol->GetSpan());
        }
        if (!functionNode->HasBody())
        {
            throw Cm::Core::Exception("static functions must have body", functionSymbol->GetSpan());
        }
        functionSymbol->SetStatic();
    }
    if ((specifiers & Cm::Ast::Specifiers::explicit_) != Cm::Ast::Specifiers::none)
    {
        if (!functionSymbol->IsConstructor())
        {
            throw Cm::Core::Exception("only constructors can be explicit", functionSymbol->GetSpan());
        }
        if (!currentClass || !currentClass->IsTemplateTypeSymbol())
        {
            if (!functionNode->HasBody())
            {
                throw Cm::Core::Exception("explicit functions must have body", functionSymbol->GetSpan());
            }
        }
        if (staticClass)
        {
            throw Cm::Core::Exception("static class cannot have explicit constructor", functionSymbol->GetSpan());
        }
        functionSymbol->SetExplicit();
    }
    if ((specifiers & Cm::Ast::Specifiers::external) != Cm::Ast::Specifiers::none)
    {
        if (functionSymbol->IsMemberFunctionSymbol())
        {
            throw Cm::Core::Exception("member functions cannot be external", functionSymbol->GetSpan());
        }
        if (functionNode->HasBody())
        {
            throw Cm::Core::Exception("external functions cannot have body", functionSymbol->GetSpan());
        }
        functionSymbol->SetExternal();
    }
    if ((specifiers & Cm::Ast::Specifiers::suppress) != Cm::Ast::Specifiers::none)
    {
        if (!functionSymbol->IsMemberFunctionSymbol())
        {
            throw Cm::Core::Exception("free functions cannot be suppressed", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsDestructor())
        {
            throw Cm::Core::Exception("destructor cannot be suppressed", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsStaticConstructor())
        {
            throw Cm::Core::Exception("static constructor cannot be suppressed", functionSymbol->GetSpan());
        }
        if (functionNode->HasBody())
        {
            throw Cm::Core::Exception("suppressed functions cannot have body", functionSymbol->GetSpan());
        }
        if (staticClass)
        {
            throw Cm::Core::Exception("static class cannot have suppressed members", functionSymbol->GetSpan());
        }
        functionSymbol->SetSuppressed();
    }
    if ((specifiers & Cm::Ast::Specifiers::default_) != Cm::Ast::Specifiers::none)
    {
        if (!functionSymbol->IsMemberFunctionSymbol())
        {
            throw Cm::Core::Exception("free functions cannot be default", functionSymbol->GetSpan());
        }
        if (functionNode->HasBody())
        {
            throw Cm::Core::Exception("default functions cannot have body", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsStaticConstructor())
        {
            throw Cm::Core::Exception("static constructor cannot be default", functionSymbol->GetSpan());
        }
        if (staticClass)
        {
            throw Cm::Core::Exception("static class cannot have default members", functionSymbol->GetSpan());
        }
        functionSymbol->SetDefault();
        if ((specifiers & Cm::Ast::Specifiers::throw_) != Cm::Ast::Specifiers::none)
        {
            functionSymbol->SetNothrow();
        }
    }
    if ((specifiers & Cm::Ast::Specifiers::inline_) != Cm::Ast::Specifiers::none)
    {
        if (!currentClass || !currentClass->IsTemplateTypeSymbol())
        {
            if (!functionNode->HasBody())
            {
                throw Cm::Core::Exception("inline functions must have body", functionSymbol->GetSpan());
            }
        }
        functionSymbol->SetInline();
        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::optimize))
        {
            functionSymbol->SetReplicated();
        }
    }
    if ((specifiers & Cm::Ast::Specifiers::cdecl_) != Cm::Ast::Specifiers::none)
    {
        if (functionSymbol->IsMemberFunctionSymbol())
        {
            throw Cm::Core::Exception("member functions cannot be cdecl", functionSymbol->GetSpan());
        }
        functionSymbol->SetCDecl();
    }
    if ((specifiers & Cm::Ast::Specifiers::nothrow_) != Cm::Ast::Specifiers::none)
    {
        functionSymbol->SetNothrow();
    }
    if (((specifiers & Cm::Ast::Specifiers::throw_) != Cm::Ast::Specifiers::none))
    {
        functionSymbol->ResetNothrow();
    }
    if ((specifiers & Cm::Ast::Specifiers::abstract_) != Cm::Ast::Specifiers::none)
    {
        if (!functionSymbol->IsMemberFunctionSymbol())
        {
            throw Cm::Core::Exception("free functions cannot be abstract", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsConstructor())
        {
            throw Cm::Core::Exception("constructor cannot be abstract", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsStaticConstructor())
        {
            throw Cm::Core::Exception("static constructor cannot be abstract", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsDestructor())
        {
            throw Cm::Core::Exception("destructor cannot be abstract", functionSymbol->GetSpan());
        }
        if (staticClass)
        {
            throw Cm::Core::Exception("static class cannot have abstract members", functionSymbol->GetSpan());
        }
        if (functionNode->HasBody())
        {
            throw Cm::Core::Exception("abstract functions cannot have body", functionSymbol->GetSpan());
        }
        functionSymbol->SetAbstract();
        currentClass->SetVirtual();
    }
    if ((specifiers & Cm::Ast::Specifiers::virtual_) != Cm::Ast::Specifiers::none)
    {
        if (!functionSymbol->IsMemberFunctionSymbol())
        {
            throw Cm::Core::Exception("free functions cannot be virtual", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsConstructor())
        {
            throw Cm::Core::Exception("constructor cannot be virtual", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsStaticConstructor())
        {
            throw Cm::Core::Exception("static constructor cannot be virtual", functionSymbol->GetSpan());
        }
        if (staticClass)
        {
            throw Cm::Core::Exception("static class cannot have virtual members", functionSymbol->GetSpan());
        }
        functionSymbol->SetVirtual();
        currentClass->SetVirtual();
    }
    if ((specifiers & Cm::Ast::Specifiers::override_) != Cm::Ast::Specifiers::none)
    {
        if (!functionSymbol->IsMemberFunctionSymbol())
        {
            throw Cm::Core::Exception("free functions cannot be override", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsConstructor())
        {
            throw Cm::Core::Exception("constructor cannot be override", functionSymbol->GetSpan());
        }
        if (functionSymbol->IsStaticConstructor())
        {
            throw Cm::Core::Exception("static constructor cannot be override", functionSymbol->GetSpan());
        }
        if (staticClass)
        {
            throw Cm::Core::Exception("static class cannot have override members", functionSymbol->GetSpan());
        }
        functionSymbol->SetOverride();
        currentClass->SetVirtual();
    }
    if (functionNode->ReturnTypeExpr())
    {
        Cm::Sym::TypeSymbol* returnType = ResolveType(symbolTable, containerScope, fileScopes, classTemplateRepository, functionNode->ReturnTypeExpr());
        functionSymbol->SetReturnType(returnType);
    }
    if (currentClass && currentClass->IsTemplateTypeSymbol())
    {
        functionSymbol->SetMemberOfTemplateType();
        functionSymbol->SetReplicated();
    }
    if (functionSymbol->IsMemberFunctionSymbol())
    {
        if (functionSymbol->IsDefaultConstructor())
        {
            if (functionSymbol->IsSuppressed())
            {
                currentClass->SetHasSuppressedDefaultConstructor();
            }
            else if (functionSymbol->IsDefault())
            {
                currentClass->SetGenerateDefaultConstructor();
            }
            else
            {
                currentClass->SetHasUserDefinedConstructor();
                currentClass->SetHasUserDefinedDefaultConstructor();
            }
        }
        else if (functionSymbol->IsCopyConstructor())
        {
            if (functionSymbol->IsSuppressed())
            {
                currentClass->SetHasSuppressedCopyConstructor();
            }
            else if (functionSymbol->IsDefault())
            {
                currentClass->SetGenerateCopyConstructor();
            }
            else
            {
                currentClass->SetHasUserDefinedConstructor();
                currentClass->SetHasUserDefinedCopyConstructor();
            }
        }
        else if (functionSymbol->IsMoveConstructor())
        {
            if (functionSymbol->IsSuppressed())
            {
                currentClass->SetHasSuppressedMoveConstructor();
            }
            else if (functionSymbol->IsDefault())
            {
                currentClass->SetGenerateMoveConstructor();
            }
            else
            {
                currentClass->SetHasUserDefinedConstructor();
                currentClass->SetHasUserDefinedMoveConstructor();
            }
        }
        else if (functionSymbol->IsStaticConstructor())
        {
            if (functionSymbol->IsSuppressed())
            {
                throw Cm::Core::Exception("static constructor cannot be suppressed", functionSymbol->GetSpan());
            }
            else
            {
                currentClass->SetHasUserDefinedStaticConstructor();
            }
        }
        else if (functionSymbol->CheckIfConvertingConstructor())
        {
            functionSymbol->SetConvertingConstructor();
            currentClass->AddConversion(functionSymbol);
        }
        else if (functionSymbol->IsCopyAssignment())
        {
            if (functionSymbol->IsSuppressed())
            {
                currentClass->SetHasSuppressedCopyAssignment();
            }
            else if (functionSymbol->IsDefault())
            {
                currentClass->SetGenerateCopyAssignment();
            }
            else
            {
                currentClass->SetHasUserDefinedCopyAssignment();
            }
        }
        else if (functionSymbol->IsMoveAssignment())
        {
            if (functionSymbol->IsSuppressed())
            {
                currentClass->SetHasSuppressedMoveAssignment();
            }
            else if (functionSymbol->IsDefault())
            {
                currentClass->SetGenerateMoveAssignment();
            }
            else
            {
                currentClass->SetHasUserDefinedMoveAssignment();
            }
        }
        else if (functionSymbol->IsDestructor())
        {
            if (functionSymbol->IsSuppressed())
            {
                throw Cm::Core::Exception("destructor cannot be suppressed", functionSymbol->GetSpan());
            }
            else if (functionSymbol->IsDefault())
            {
                currentClass->SetGenerateDestructor();
            }
            else
            {
                currentClass->SetHasUserDefinedDestructor();
            }
        }
    }
}

bool TerminatesFunction(Cm::Ast::StatementNode* statement, Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, bool inForEverLoop)
{
    switch (statement->GetNodeType())
    {
        case Cm::Ast::NodeType::compoundStatementNode:
        {
            Cm::Ast::CompoundStatementNode* compoundStatement = static_cast<Cm::Ast::CompoundStatementNode*>(statement);
            for (const std::unique_ptr<Cm::Ast::StatementNode>& stat : compoundStatement->Statements())
            {
                if (TerminatesFunction(stat.get(), symbolTable, containerScope, fileScopes, classTemplateRepository, inForEverLoop)) return true;
            }
            break;
        }
        case Cm::Ast::NodeType::conditionalStatementNode:
        {
            Cm::Ast::ConditionalStatementNode* conditionalStatement = static_cast<Cm::Ast::ConditionalStatementNode*>(statement);
            if (inForEverLoop || conditionalStatement->HasElseStatement())
            {
                if (TerminatesFunction(conditionalStatement->ThenS(), symbolTable, containerScope, fileScopes, classTemplateRepository, inForEverLoop) &&
                    (inForEverLoop || (conditionalStatement->HasElseStatement() && TerminatesFunction(conditionalStatement->ElseS(), symbolTable, containerScope, fileScopes, 
                    classTemplateRepository, inForEverLoop))))
                {
                    return true;
                }
            }
            break;
        }
        case Cm::Ast::NodeType::switchStatementNode:
        {
            Cm::Ast::SwitchStatementNode* switchStatement = static_cast<Cm::Ast::SwitchStatementNode*>(statement);
            if (switchStatement->DefaultStatement())
            {
                for (const std::unique_ptr<Cm::Ast::StatementNode>& caseStatement : switchStatement->CaseStatements())
                {
                    if (!TerminatesFunction(caseStatement.get(), symbolTable, containerScope, fileScopes, classTemplateRepository, inForEverLoop)) return false;
                }
                return TerminatesFunction(switchStatement->DefaultStatement(), symbolTable, containerScope, fileScopes, classTemplateRepository, inForEverLoop);
            }
            break;
        }
        case Cm::Ast::NodeType::caseStatementNode:
        {
            Cm::Ast::CaseStatementNode* caseStatement = static_cast<Cm::Ast::CaseStatementNode*>(statement);
            for (const std::unique_ptr<Cm::Ast::StatementNode>& stat : caseStatement->Statements())
            {
                if (TerminatesFunction(stat.get(), symbolTable, containerScope, fileScopes, classTemplateRepository, inForEverLoop)) return true;
            }
            break;
        }
        case Cm::Ast::NodeType::defaultStatementNode:
        {
            Cm::Ast::DefaultStatementNode* defaultStatement = static_cast<Cm::Ast::DefaultStatementNode*>(statement);
            for (const std::unique_ptr<Cm::Ast::StatementNode>& stat : defaultStatement->Statements())
            {
                if (TerminatesFunction(stat.get(), symbolTable, containerScope, fileScopes, classTemplateRepository, inForEverLoop)) return true;
            }
            break;
        }
        case Cm::Ast::NodeType::whileStatementNode:
        {
            Cm::Ast::WhileStatementNode* whileStatement = static_cast<Cm::Ast::WhileStatementNode*>(statement);
            if (IsAlwaysTrue(whileStatement->Condition(), symbolTable, containerScope, fileScopes, classTemplateRepository))
            {
                if (TerminatesFunction(whileStatement->Statement(), symbolTable, containerScope, fileScopes, classTemplateRepository, true)) return true;
            }
            break;
        }
        case Cm::Ast::NodeType::doStatementNode:
        {
            Cm::Ast::DoStatementNode* doStatement = static_cast<Cm::Ast::DoStatementNode*>(statement);
            if (IsAlwaysTrue(doStatement->Condition(), symbolTable, containerScope, fileScopes, classTemplateRepository))
            {
                if (TerminatesFunction(doStatement->Statement(), symbolTable, containerScope, fileScopes, classTemplateRepository, true)) return true;
            }
            break;
        }
        case Cm::Ast::NodeType::forStatementNode:
        {
            Cm::Ast::ForStatementNode* forStatement = static_cast<Cm::Ast::ForStatementNode*>(statement);
            if (!forStatement->Condition() || IsAlwaysTrue(forStatement->Condition(), symbolTable, containerScope, fileScopes, classTemplateRepository))
            {
                if (TerminatesFunction(forStatement->Action(), symbolTable, containerScope, fileScopes, classTemplateRepository, true)) return true;
            }
            break;
        }
        default:
        {
            return statement->IsFunctionTerminatingNode();
        }
    }
    return false;
}

void CheckFunctionReturnPaths(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::Sym::FunctionSymbol* functionSymbol, Cm::Ast::FunctionNode* functionNode)
{
    if (!functionSymbol->GetReturnType() || functionSymbol->GetReturnType()->IsVoidTypeSymbol() || !functionNode->HasBody()) return;
    Cm::Ast::CompoundStatementNode* body = functionNode->Body();
    for (const std::unique_ptr<Cm::Ast::StatementNode>& statement : body->Statements())
    {
        if (TerminatesFunction(statement.get(), symbolTable, containerScope, fileScopes, classTemplateRepository, false)) return;
    }
    throw Cm::Core::Exception("not all control paths terminate in return statement or throw statement", functionNode->GetSpan());
}

void CheckFunctionAccessLevels(Cm::Sym::FunctionSymbol* functionSymbol)
{
    Cm::Sym::TypeSymbol* returnType = functionSymbol->GetReturnType();
    if (returnType)
    {
        if (returnType->Access() < functionSymbol->EffectiveAccess())
        {
            throw Cm::Core::Exception("return type of a function must be at least as accessible as the function itself", returnType->GetSpan(), functionSymbol->GetSpan());
        }
    }
    int n = int(functionSymbol->Parameters().size());
    int start = 0;
    if (functionSymbol->IsMemberFunctionSymbol() && !functionSymbol->IsStatic())
    {
        start = 1;
    }
    for (int i = start; i < n; ++i)
    {
        Cm::Sym::ParameterSymbol* param = functionSymbol->Parameters()[i];
        Cm::Sym::TypeSymbol* parameterType = param->GetType();
        if (parameterType->Access() < functionSymbol->EffectiveAccess())
        {
            std::string accessStr = Cm::Sym::AccessStr(parameterType->Access());
            throw Cm::Core::Exception("parameter type of a function must be at least as accessible as the function itself", parameterType->GetSpan(), functionSymbol->GetSpan());
        }
    }
}

} } // namespace Cm::Bind
