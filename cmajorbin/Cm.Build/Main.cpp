/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Build/Main.hpp>
#include <Cm.Build/Build.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/Binder.hpp>
#include <Cm.Bind/Parameter.hpp>
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Emit/EmittingVisitor.hpp>
#include <Cm.Sym/ExceptionTable.hpp>
#include <Cm.Ast/CompileUnit.hpp>
#include <Cm.Util/Path.hpp>
#include <Cm.Parsing/Scanner.hpp>
#include <boost/filesystem.hpp>

namespace Cm { namespace Build {

void GenerateMainCompileUnit(Cm::Sym::SymbolTable& symbolTable, const std::string& outputBasePath, Cm::Sym::FunctionSymbol* userMainFunction, std::vector<std::string>& objectFilePaths)
{
    if (!userMainFunction)
    {
        throw Cm::Core::Exception("program has no main() function");
    }
    boost::filesystem::path outputBase(outputBasePath);
    Cm::Parsing::Span span;
    Cm::Ast::CompileUnitNode syntaxUnit(span);
    std::string mainCompileUnitIrFilePath = Cm::Util::GetFullPath((outputBase / boost::filesystem::path("__main__.ll")).generic_string());
    Cm::BoundTree::BoundCompileUnit mainCompileUnit(&syntaxUnit, mainCompileUnitIrFilePath, symbolTable);
    std::unique_ptr<Cm::Sym::FunctionSymbol> mainFunctionSymbol(new Cm::Sym::FunctionSymbol(Cm::Parsing::Span(), "main"));
    mainFunctionSymbol->SetCDecl();
    Cm::Sym::TypeSymbol* intType = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId));
    Cm::Sym::ParameterSymbol* argcParam = nullptr;
    Cm::Sym::ParameterSymbol* argvParam = nullptr;
    if (!userMainFunction->Parameters().empty())
    {
        if (userMainFunction->Parameters().size() != 2)
        {
            throw Cm::Core::Exception("main() function must have either zero or two parameters", userMainFunction->GetSpan());
        }
        Cm::Sym::ParameterSymbol* firstParam = userMainFunction->Parameters()[0];
        if (!Cm::Sym::TypesEqual(firstParam->GetType(), intType))
        {
            throw Cm::Core::Exception("type of first parameter of main() function must be int", userMainFunction->GetSpan());
        }
        Cm::Sym::ParameterSymbol* secondParam = userMainFunction->Parameters()[1];
        Cm::Sym::TypeSymbol* constCharPtrPtrType = symbolTable.GetTypeRepository().MakeConstCharPtrPtrType(userMainFunction->GetSpan());
        if (!Cm::Sym::TypesEqual(secondParam->GetType(), constCharPtrPtrType))
        {
            throw Cm::Core::Exception("type of second parameter of main() function must be const char**", userMainFunction->GetSpan());
        }
        argcParam = new Cm::Sym::ParameterSymbol(userMainFunction->GetSpan(), "argc");
        argcParam->SetType(intType);
        mainFunctionSymbol->AddSymbol(argcParam);
        argvParam = new Cm::Sym::ParameterSymbol(userMainFunction->GetSpan(), "argv");
        argvParam->SetType(constCharPtrPtrType);
        mainFunctionSymbol->AddSymbol(argvParam);
    }
    if (!userMainFunction->GetReturnType() || !userMainFunction->GetReturnType()->IsVoidTypeSymbol() && !Cm::Sym::TypesEqual(userMainFunction->GetReturnType(), intType))
    {
        throw Cm::Core::Exception("return type of main() function must be void or int", userMainFunction->GetSpan());
    }

    mainFunctionSymbol->SetReturnType(intType);
    Cm::BoundTree::BoundFunction* mainFunction = new Cm::BoundTree::BoundFunction(nullptr, mainFunctionSymbol.get());
    Cm::BoundTree::BoundCompoundStatement* mainBody = new Cm::BoundTree::BoundCompoundStatement(nullptr);
    mainFunction->SetBody(mainBody);
    Cm::Bind::GenerateReceives(nullptr, mainCompileUnit, mainFunction);
    Cm::Sym::LocalVariableSymbol* returnValueVariable = new Cm::Sym::LocalVariableSymbol(userMainFunction->GetSpan(), "returnValue");
    returnValueVariable->SetType(intType);
    mainFunctionSymbol->AddSymbol(returnValueVariable);
    mainFunction->AddLocalVariable(returnValueVariable);
    std::unique_ptr<Cm::Sym::FunctionSymbol> intAssignment;

    if (userMainFunction->GetReturnType()->IsVoidTypeSymbol())
    {
        Cm::Sym::ExceptionTable* exceptionTable = Cm::Sym::GetExceptionTable();
        Cm::Sym::FunctionSymbol* threadTblInit = symbolTable.GetOverload("threadtbl_init");
        Cm::BoundTree::BoundExpressionList threadTblInitArguments;
        Cm::BoundTree::BoundLiteral* numExceptions = new Cm::BoundTree::BoundLiteral(nullptr);
        numExceptions->SetValue(new Cm::Sym::IntValue(exceptionTable->GetNumberOfExceptions()));
        numExceptions->SetType(intType);
        threadTblInitArguments.Add(numExceptions);
        Cm::BoundTree::BoundFunctionCallStatement* callThreadTblInitStatement = new Cm::BoundTree::BoundFunctionCallStatement(threadTblInit, std::move(threadTblInitArguments));
        mainBody->AddStatement(callThreadTblInitStatement);

        Cm::BoundTree::BoundExpressionList arguments;
        if (argcParam && argvParam)
        {
            arguments.Add(new Cm::BoundTree::BoundParameter(nullptr, argcParam));
            arguments.Add(new Cm::BoundTree::BoundParameter(nullptr, argvParam));
        }
        Cm::BoundTree::BoundFunctionCall* callUserMainExpr = new Cm::BoundTree::BoundFunctionCall(nullptr, std::move(arguments));
        callUserMainExpr->SetFunction(userMainFunction);
        callUserMainExpr->SetType(userMainFunction->GetReturnType());
        Cm::BoundTree::BoundSimpleStatement* callUserMainStatement = new Cm::BoundTree::BoundSimpleStatement(nullptr);
        callUserMainStatement->SetExpression(callUserMainExpr);
        mainBody->AddStatement(callUserMainStatement);

        Cm::Sym::FunctionSymbol* cmExit = symbolTable.GetOverload("cm_exit");
        Cm::BoundTree::BoundExpressionList cmExitArguments;
        Cm::BoundTree::BoundFunctionCallStatement* callCmExitStatement = new Cm::BoundTree::BoundFunctionCallStatement(cmExit, std::move(cmExitArguments));
        mainBody->AddStatement(callCmExitStatement);

        Cm::Sym::FunctionSymbol* threadTblDone = symbolTable.GetOverload("threadtbl_done");
        Cm::BoundTree::BoundExpressionList threadTblDoneArguments;
        Cm::BoundTree::BoundFunctionCallStatement* callThreadTblDoneStatement = new Cm::BoundTree::BoundFunctionCallStatement(threadTblDone, std::move(threadTblDoneArguments));
        mainBody->AddStatement(callThreadTblDoneStatement);

        Cm::BoundTree::BoundLiteral* zero = new Cm::BoundTree::BoundLiteral(nullptr);
        zero->SetValue(new Cm::Sym::IntValue(0));
        zero->SetType(intType);
        Cm::BoundTree::BoundLocalVariable* returnValue = new Cm::BoundTree::BoundLocalVariable(nullptr, returnValueVariable);
        returnValue->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
        intAssignment.reset(new Cm::Core::CopyAssignment(symbolTable.GetTypeRepository(), intType));
        Cm::BoundTree::BoundAssignmentStatement* assignmentStatement = new Cm::BoundTree::BoundAssignmentStatement(nullptr, returnValue, zero, intAssignment.get());
        mainBody->AddStatement(assignmentStatement);
    }
    else
    {
        Cm::BoundTree::BoundExpressionList arguments;
        if (argcParam && argvParam)
        {
            arguments.Add(new Cm::BoundTree::BoundParameter(nullptr, argcParam));
            arguments.Add(new Cm::BoundTree::BoundParameter(nullptr, argvParam));
        }

        Cm::Sym::ExceptionTable* exceptionTable = Cm::Sym::GetExceptionTable();
        Cm::Sym::FunctionSymbol* threadTblInit = symbolTable.GetOverload("threadtbl_init");
        Cm::BoundTree::BoundExpressionList threadTblInitArguments;
        Cm::BoundTree::BoundLiteral* numExceptions = new Cm::BoundTree::BoundLiteral(nullptr);
        numExceptions->SetValue(new Cm::Sym::IntValue(exceptionTable->GetNumberOfExceptions()));
        numExceptions->SetType(intType);
        threadTblInitArguments.Add(numExceptions);
        Cm::BoundTree::BoundFunctionCallStatement* callThreadTblInitStatement = new Cm::BoundTree::BoundFunctionCallStatement(threadTblInit, std::move(threadTblInitArguments));
        mainBody->AddStatement(callThreadTblInitStatement);

        Cm::BoundTree::BoundFunctionCall* callUserMainExpr = new Cm::BoundTree::BoundFunctionCall(nullptr, std::move(arguments));
        callUserMainExpr->SetFunction(userMainFunction);
        callUserMainExpr->SetType(userMainFunction->GetReturnType());
        Cm::BoundTree::BoundLocalVariable* returnValue = new Cm::BoundTree::BoundLocalVariable(nullptr, returnValueVariable);
        returnValue->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
        intAssignment.reset(new Cm::Core::CopyAssignment(symbolTable.GetTypeRepository(), intType));
        Cm::BoundTree::BoundAssignmentStatement* assignmentStatement = new Cm::BoundTree::BoundAssignmentStatement(nullptr, returnValue, callUserMainExpr, intAssignment.get());
        mainBody->AddStatement(assignmentStatement);

        Cm::Sym::FunctionSymbol* cmExit = symbolTable.GetOverload("cm_exit");
        Cm::BoundTree::BoundExpressionList cmExitArguments;
        Cm::BoundTree::BoundFunctionCallStatement* callCmExitStatement = new Cm::BoundTree::BoundFunctionCallStatement(cmExit, std::move(cmExitArguments));
        mainBody->AddStatement(callCmExitStatement);

        Cm::Sym::FunctionSymbol* threadTblDone = symbolTable.GetOverload("threadtbl_done");
        Cm::BoundTree::BoundExpressionList threadTblDoneArguments;
        Cm::BoundTree::BoundFunctionCallStatement* callThreadTblDoneStatement = new Cm::BoundTree::BoundFunctionCallStatement(threadTblDone, std::move(threadTblDoneArguments));
        mainBody->AddStatement(callThreadTblDoneStatement);
    }

    Cm::BoundTree::BoundReturnStatement* returnStatement = new Cm::BoundTree::BoundReturnStatement(nullptr);
    std::unique_ptr<Cm::Sym::FunctionSymbol> intCopyCtor(new Cm::Core::CopyCtor(symbolTable.GetTypeRepository(), intType));
    returnStatement->SetConstructor(intCopyCtor.get());
    returnStatement->SetExpression(new Cm::BoundTree::BoundLocalVariable(nullptr, returnValueVariable));
    returnStatement->SetReturnType(intType);
    mainBody->AddStatement(returnStatement);

    mainCompileUnit.AddBoundNode(mainFunction);
    mainFunctionSymbol->SetParent(userMainFunction->Parent());
    Emit(symbolTable.GetTypeRepository(), mainCompileUnit);
    GenerateObjectCode(mainCompileUnit);
    objectFilePaths.push_back(mainCompileUnit.ObjectFilePath());
}

} } // namespace Bm::Build
