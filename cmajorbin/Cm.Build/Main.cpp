/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Build/Main.hpp>
#include <Cm.Build/Build.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Core/Argument.hpp>
#include <Cm.Bind/Binder.hpp>
#include <Cm.Bind/Parameter.hpp>
#include <Cm.Bind/OverloadResolution.hpp>
#include <Cm.Bind/ClassTemplateRepository.hpp>
#include <Cm.Bind/SynthesizedClassFun.hpp>
#include <Cm.Bind/DelegateTypeOpRepository.hpp>
#include <Cm.Bind/ClassDelegateTypeOpRepository.hpp>
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Emit/EmittingVisitor.hpp>
#include <Cm.Sym/ExceptionTable.hpp>
#include <Cm.Sym/MutexTable.hpp>
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
    mainCompileUnit.SetClassTemplateRepository(new Cm::Bind::ClassTemplateRepository(mainCompileUnit));
    mainCompileUnit.SetSynthesizedClassFunRepository(new Cm::Bind::SynthesizedClassFunRepository(mainCompileUnit));
    mainCompileUnit.SetDelegateTypeOpRepository(new Cm::Bind::DelegateTypeOpRepository(mainCompileUnit));
    mainCompileUnit.SetClassDelegateTypeOpRepository(new Cm::Bind::ClassDelegateTypeOpRepository(mainCompileUnit));
    std::unique_ptr<Cm::Sym::FunctionSymbol> mainFunctionSymbol(new Cm::Sym::FunctionSymbol(Cm::Parsing::Span(), "main"));
    mainFunctionSymbol->SetCDecl();
    Cm::Sym::TypeSymbol* intType = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId));
    Cm::Sym::ParameterSymbol* argcParam = nullptr;
    Cm::Sym::ParameterSymbol* argvParam = nullptr;
    Cm::Sym::TypeSymbol* boolType = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
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
    mainFunction->SetMainFunction();
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

		Cm::Sym::MutexTable* mutexTable = Cm::Sym::GetMutexTable();
		Cm::Sym::FunctionSymbol* mutexTableInit = symbolTable.GetOverload("mutextbl_init");
		Cm::BoundTree::BoundExpressionList mutexTblInitArguments;
		Cm::BoundTree::BoundLiteral* numMutexes = new Cm::BoundTree::BoundLiteral(nullptr);
		numMutexes->SetValue(new Cm::Sym::IntValue(mutexTable->GetNumberOfMutexes()));
		numMutexes->SetType(intType);
		mutexTblInitArguments.Add(numMutexes);
		Cm::BoundTree::BoundFunctionCallStatement* callMutexTblInitStatement = new Cm::BoundTree::BoundFunctionCallStatement(mutexTableInit, std::move(mutexTblInitArguments));
		mainBody->AddStatement(callMutexTblInitStatement);

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

        Cm::BoundTree::BoundLiteral* zeroExCode = new Cm::BoundTree::BoundLiteral(nullptr);
        zeroExCode->SetValue(new Cm::Sym::IntValue(0));
        zeroExCode->SetType(intType);
        Cm::BoundTree::BoundConditionalStatement* testExceptionVarStatement = new Cm::BoundTree::BoundConditionalStatement(nullptr);
        Cm::BoundTree::BoundExceptionCodeVariable* exceptionCodeVariable = new Cm::BoundTree::BoundExceptionCodeVariable();
        exceptionCodeVariable->SetType(intType);
        Cm::BoundTree::BoundBinaryOp* exCodeEqualToZero = new Cm::BoundTree::BoundBinaryOp(nullptr, exceptionCodeVariable, zeroExCode);
        exCodeEqualToZero->SetType(boolType);
        std::vector<Cm::Core::Argument> equalIntArgs;
        equalIntArgs.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, intType));
        equalIntArgs.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, intType));
        Cm::Sym::FunctionLookupSet equalIntLookups;
        equalIntLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, symbolTable.GlobalScope()));
        std::vector<Cm::Sym::FunctionSymbol*> equalIntConversions;
        Cm::Sym::FunctionSymbol* equalInt = Cm::Bind::ResolveOverload(symbolTable.GlobalScope(), mainCompileUnit, "operator==", equalIntArgs, equalIntLookups, span, equalIntConversions);
        exCodeEqualToZero->SetFunction(equalInt);
        Cm::BoundTree::BoundUnaryOp* exCodeNotEqualToZero = new Cm::BoundTree::BoundUnaryOp(nullptr, exCodeEqualToZero);
        exCodeNotEqualToZero->SetType(boolType);
        std::vector<Cm::Core::Argument> notBoolArgs;
        notBoolArgs.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, boolType)); 
        Cm::Sym::FunctionLookupSet notBoolLookups;
        notBoolLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, symbolTable.GlobalScope()));
        std::vector<Cm::Sym::FunctionSymbol*> notBoolConversions;
        Cm::Sym::FunctionSymbol* notBool = Cm::Bind::ResolveOverload(symbolTable.GlobalScope(), mainCompileUnit, "operator!", notBoolArgs, notBoolLookups, span, notBoolConversions);
        exCodeNotEqualToZero->SetFunction(notBool);
        exCodeNotEqualToZero->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        testExceptionVarStatement->SetCondition(exCodeNotEqualToZero);
        Cm::Sym::FunctionSymbol* mainUnhandlerException = symbolTable.GetOverload("System.Support.MainUnhandledException");
        Cm::BoundTree::BoundExpressionList mainUnhandlerExceptionArguments;
        Cm::BoundTree::BoundFunctionCallStatement* callMainUnhandlerExceptionStatement = new Cm::BoundTree::BoundFunctionCallStatement(mainUnhandlerException, std::move(mainUnhandlerExceptionArguments));
        testExceptionVarStatement->AddStatement(callMainUnhandlerExceptionStatement);
        mainBody->AddStatement(testExceptionVarStatement);

        Cm::Sym::FunctionSymbol* cmExit = symbolTable.GetOverload("cm_exit");
        Cm::BoundTree::BoundExpressionList cmExitArguments;
        Cm::BoundTree::BoundFunctionCallStatement* callCmExitStatement = new Cm::BoundTree::BoundFunctionCallStatement(cmExit, std::move(cmExitArguments));
        mainBody->AddStatement(callCmExitStatement);

		Cm::Sym::FunctionSymbol* mutexTblDone = symbolTable.GetOverload("mutextbl_done");
		Cm::BoundTree::BoundExpressionList mutexTblDoneArguments;
		Cm::BoundTree::BoundFunctionCallStatement* callMutexTblDoneStatement = new Cm::BoundTree::BoundFunctionCallStatement(mutexTblDone, std::move(mutexTblDoneArguments));
		mainBody->AddStatement(callMutexTblDoneStatement);

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

		Cm::Sym::MutexTable* mutexTable = Cm::Sym::GetMutexTable();
		Cm::Sym::FunctionSymbol* mutexTableInit = symbolTable.GetOverload("mutextbl_init");
		Cm::BoundTree::BoundExpressionList mutexTblInitArguments;
		Cm::BoundTree::BoundLiteral* numMutexes = new Cm::BoundTree::BoundLiteral(nullptr);
		numMutexes->SetValue(new Cm::Sym::IntValue(mutexTable->GetNumberOfMutexes()));
		numMutexes->SetType(intType);
		mutexTblInitArguments.Add(numMutexes);
		Cm::BoundTree::BoundFunctionCallStatement* callMutexTblInitStatement = new Cm::BoundTree::BoundFunctionCallStatement(mutexTableInit, std::move(mutexTblInitArguments));
		mainBody->AddStatement(callMutexTblInitStatement);

        Cm::BoundTree::BoundFunctionCall* callUserMainExpr = new Cm::BoundTree::BoundFunctionCall(nullptr, std::move(arguments));
        callUserMainExpr->SetFunction(userMainFunction);
        callUserMainExpr->SetType(userMainFunction->GetReturnType());
        Cm::BoundTree::BoundLocalVariable* returnValue = new Cm::BoundTree::BoundLocalVariable(nullptr, returnValueVariable);
        returnValue->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
        intAssignment.reset(new Cm::Core::CopyAssignment(symbolTable.GetTypeRepository(), intType));
        Cm::BoundTree::BoundAssignmentStatement* assignmentStatement = new Cm::BoundTree::BoundAssignmentStatement(nullptr, returnValue, callUserMainExpr, intAssignment.get());
        mainBody->AddStatement(assignmentStatement);

        Cm::BoundTree::BoundLiteral* zeroExCode = new Cm::BoundTree::BoundLiteral(nullptr);
        zeroExCode->SetValue(new Cm::Sym::IntValue(0));
        zeroExCode->SetType(intType);
        Cm::BoundTree::BoundConditionalStatement* testExceptionVarStatement = new Cm::BoundTree::BoundConditionalStatement(nullptr);
        Cm::BoundTree::BoundExceptionCodeVariable* exceptionCodeVariable = new Cm::BoundTree::BoundExceptionCodeVariable();
        exceptionCodeVariable->SetType(intType);
        Cm::BoundTree::BoundBinaryOp* exCodeEqualToZero = new Cm::BoundTree::BoundBinaryOp(nullptr, exceptionCodeVariable, zeroExCode);
        exCodeEqualToZero->SetType(boolType);
        std::vector<Cm::Core::Argument> equalIntArgs;
        equalIntArgs.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, intType));
        equalIntArgs.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, intType));
        Cm::Sym::FunctionLookupSet equalIntLookups;
        equalIntLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, symbolTable.GlobalScope()));
        std::vector<Cm::Sym::FunctionSymbol*> equalIntConversions;
        Cm::Sym::FunctionSymbol* equalInt = Cm::Bind::ResolveOverload(symbolTable.GlobalScope(), mainCompileUnit, "operator==", equalIntArgs, equalIntLookups, span, equalIntConversions);
        exCodeEqualToZero->SetFunction(equalInt);
        Cm::BoundTree::BoundUnaryOp* exCodeNotEqualToZero = new Cm::BoundTree::BoundUnaryOp(nullptr, exCodeEqualToZero);
        exCodeNotEqualToZero->SetType(boolType);
        std::vector<Cm::Core::Argument> notBoolArgs;
        notBoolArgs.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, boolType));
        Cm::Sym::FunctionLookupSet notBoolLookups;
        notBoolLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, symbolTable.GlobalScope()));
        std::vector<Cm::Sym::FunctionSymbol*> notBoolConversions;
        Cm::Sym::FunctionSymbol* notBool = Cm::Bind::ResolveOverload(symbolTable.GlobalScope(), mainCompileUnit, "operator!", notBoolArgs, notBoolLookups, span, notBoolConversions);
        exCodeNotEqualToZero->SetFunction(notBool);
        exCodeNotEqualToZero->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
        testExceptionVarStatement->SetCondition(exCodeNotEqualToZero);
        Cm::Sym::FunctionSymbol* mainUnhandlerException = symbolTable.GetOverload("System.Support.MainUnhandledException");
        Cm::BoundTree::BoundExpressionList mainUnhandlerExceptionArguments;
        Cm::BoundTree::BoundFunctionCallStatement* callMainUnhandlerExceptionStatement = new Cm::BoundTree::BoundFunctionCallStatement(mainUnhandlerException, std::move(mainUnhandlerExceptionArguments));
        testExceptionVarStatement->AddStatement(callMainUnhandlerExceptionStatement);
        mainBody->AddStatement(testExceptionVarStatement);

        Cm::Sym::FunctionSymbol* cmExit = symbolTable.GetOverload("cm_exit");
        Cm::BoundTree::BoundExpressionList cmExitArguments;
        Cm::BoundTree::BoundFunctionCallStatement* callCmExitStatement = new Cm::BoundTree::BoundFunctionCallStatement(cmExit, std::move(cmExitArguments));
        mainBody->AddStatement(callCmExitStatement);

		Cm::Sym::FunctionSymbol* mutexTblDone = symbolTable.GetOverload("mutextbl_done");
		Cm::BoundTree::BoundExpressionList mutexTblDoneArguments;
		Cm::BoundTree::BoundFunctionCallStatement* callMutexTblDoneStatement = new Cm::BoundTree::BoundFunctionCallStatement(mutexTblDone, std::move(mutexTblDoneArguments));
		mainBody->AddStatement(callMutexTblDoneStatement);

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