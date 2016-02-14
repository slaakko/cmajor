/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
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
#include <Cm.Bind/InlineFunctionRepository.hpp>
#include <Cm.Bind/ClassDelegateTypeOpRepository.hpp>
#include <Cm.Bind/ArrayTypeOpRepository.hpp>
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.Core/GlobalSettings.hpp>
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/ExceptionTable.hpp>
#include <Cm.Sym/MutexTable.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.Ast/CompileUnit.hpp>
#include <Cm.Util/Path.hpp>
#include <Cm.Parsing/Scanner.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <boost/filesystem.hpp>

namespace Cm { namespace Build {

bool GenerateMainCompileUnit(Cm::Sym::SymbolTable& symbolTable, const std::string& outputBasePath, const std::string& profDataFilePath, std::vector<std::string>& objectFilePaths, int numClassHierarchyTableEntries, 
    uint64_t stackSize, bool changed)
{
    Cm::Sym::FunctionSymbol* userMainFunction = symbolTable.UserMainFunction();
    if (!userMainFunction)
    {
        throw Cm::Core::Exception("program has no main() function");
    }
    boost::filesystem::path outputBase(outputBasePath);
    Cm::Parsing::Span span;
    Cm::Ast::CompileUnitNode syntaxUnit(span);
    std::string ext;
    Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
    if (backend == Cm::IrIntf::BackEnd::llvm)
    {
        ext = ".ll";
    }
    else if (backend == Cm::IrIntf::BackEnd::c)
    {
        ext = ".c";
    }
    std::string mainCompileUnitIrFilePath = Cm::Util::GetFullPath((outputBase / boost::filesystem::path("__main__" + ext)).generic_string());
    Cm::BoundTree::BoundCompileUnit mainCompileUnit(&syntaxUnit, mainCompileUnitIrFilePath, symbolTable);
    mainCompileUnit.SetMainUnit();
    mainCompileUnit.SetClassTemplateRepository(new Cm::Bind::ClassTemplateRepository(mainCompileUnit));
    mainCompileUnit.SetInlineFunctionRepository(new Cm::Bind::InlineFunctionRepository(mainCompileUnit));
    mainCompileUnit.SetSynthesizedClassFunRepository(new Cm::Bind::SynthesizedClassFunRepository(mainCompileUnit));
    mainCompileUnit.SetDelegateTypeOpRepository(new Cm::Bind::DelegateTypeOpRepository(mainCompileUnit));
    mainCompileUnit.SetClassDelegateTypeOpRepository(new Cm::Bind::ClassDelegateTypeOpRepository(mainCompileUnit));
    mainCompileUnit.SetArrayTypeOpRepository(new Cm::Bind::ArrayTypeOpRepository(mainCompileUnit));
    objectFilePaths.push_back(mainCompileUnit.ObjectFilePath());
    if (!changed)
    {
        boost::filesystem::path ifp = mainCompileUnit.IrFilePath();
        boost::filesystem::path ofp = mainCompileUnit.ObjectFilePath();
        if (!boost::filesystem::exists(ifp))
        {
            changed = true;
        }
        else if (!boost::filesystem::exists(ofp))
        {
            changed = true;
        }
        else if (boost::filesystem::last_write_time(ifp) > boost::filesystem::last_write_time(ofp))
        {
            changed = true;
        }
    }
    if (!changed) return false;
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
        symbolTable.SetSidAndAddSymbol(argcParam);
        mainFunctionSymbol->AddSymbol(argcParam);
        argvParam = new Cm::Sym::ParameterSymbol(userMainFunction->GetSpan(), "argv");
        argvParam->SetType(constCharPtrPtrType);
        symbolTable.SetSidAndAddSymbol(argvParam);
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
        if (stackSize != 0)
        {
            Cm::Sym::FunctionSymbol* setStackSizeFun = symbolTable.GetOverload("set_stack_size");
            if (!setStackSizeFun)
            {
                throw std::runtime_error("set_stack_size function not found");
            }
            Cm::BoundTree::BoundExpressionList setStackSizeArgs;
            Cm::BoundTree::BoundLiteral* stackSizeArg = new Cm::BoundTree::BoundLiteral(nullptr);
            stackSizeArg->SetValue(new Cm::Sym::ULongValue(stackSize));
            Cm::Sym::TypeSymbol* ulongType = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ulongId));
            stackSizeArg->SetType(ulongType);
            setStackSizeArgs.Add(stackSizeArg);
            Cm::BoundTree::BoundFunctionCallStatement* setStackSizeStatement = new Cm::BoundTree::BoundFunctionCallStatement(setStackSizeFun, std::move(setStackSizeArgs));
            mainBody->AddStatement(setStackSizeStatement);
        }

        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::debug_heap))
        {
            Cm::Sym::FunctionSymbol* dbgHeapInit = symbolTable.GetOverload("dbgheap_init");
            Cm::BoundTree::BoundExpressionList dbgHeapInitArgs;
            Cm::BoundTree::BoundFunctionCallStatement* dbgHeapInitStatement = new Cm::BoundTree::BoundFunctionCallStatement(dbgHeapInit, std::move(dbgHeapInitArgs));
            mainBody->AddStatement(dbgHeapInitStatement);
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

        if (Cm::Core::GetGlobalSettings()->Config() != "full")
        {
            Cm::Sym::FunctionSymbol* initClassHierarchy = symbolTable.GetOverload("init_class_hierarchy");
            Cm::BoundTree::BoundExpressionList initClassHierarchyArguments;
            Cm::BoundTree::BoundClassHierarchyTableConstant* classHierarchyTableConstant = new Cm::BoundTree::BoundClassHierarchyTableConstant();
            classHierarchyTableConstant->SetType(symbolTable.GetTypeRepository().MakePointerType(symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ulongId)),
                userMainFunction->GetSpan()));
            initClassHierarchyArguments.Add(classHierarchyTableConstant);
            Cm::BoundTree::BoundLiteral* numEntries = new Cm::BoundTree::BoundLiteral(nullptr);
            numEntries->SetValue(new Cm::Sym::IntValue(numClassHierarchyTableEntries));
            numEntries->SetType(intType);
            initClassHierarchyArguments.Add(numEntries);
            Cm::BoundTree::BoundFunctionCallStatement* callInitClassHierarchyStatement = new Cm::BoundTree::BoundFunctionCallStatement(initClassHierarchy, std::move(initClassHierarchyArguments));
            mainBody->AddStatement(callInitClassHierarchyStatement);
        }

        if (Cm::Core::GetGlobalSettings()->Config() == "profile")
        {
            Cm::Sym::FunctionSymbol* startProfiling = symbolTable.GetOverload("start_profiling");
            Cm::BoundTree::BoundExpressionList startProfilingArguments;
            int profDataFileId = mainCompileUnit.StringRepository().Install(profDataFilePath);
            Cm::BoundTree::BoundStringLiteral* profDataFileArg = new Cm::BoundTree::BoundStringLiteral(nullptr, profDataFileId);
            Cm::Sym::TypeSymbol* constCharPtrType = symbolTable.GetTypeRepository().MakeConstCharPtrType(userMainFunction->GetSpan());
            profDataFileArg->SetType(constCharPtrType);
            startProfilingArguments.Add(profDataFileArg);
            Cm::BoundTree::BoundFunctionCallStatement* startProfilingStatement = new Cm::BoundTree::BoundFunctionCallStatement(startProfiling, std::move(startProfilingArguments));
            mainBody->AddStatement(startProfilingStatement);
        }

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
        Cm::Sym::FunctionSymbol* mainUnhandledException = symbolTable.GetOverload("System.Support.MainUnhandledException");
        Cm::BoundTree::BoundExpressionList mainUnhandledExceptionArguments;
        Cm::BoundTree::BoundFunctionCallStatement* callMainUnhandledExceptionStatement = new Cm::BoundTree::BoundFunctionCallStatement(mainUnhandledException, std::move(mainUnhandledExceptionArguments));
        testExceptionVarStatement->AddStatement(callMainUnhandledExceptionStatement);
        mainBody->AddStatement(testExceptionVarStatement);

        if (Cm::Core::GetGlobalSettings()->Config() == "profile")
        {
            Cm::Sym::FunctionSymbol* endProfiling = symbolTable.GetOverload("end_profiling");
            Cm::BoundTree::BoundExpressionList endProfilingArguments;
            Cm::BoundTree::BoundFunctionCallStatement* endProfilingStatement = new Cm::BoundTree::BoundFunctionCallStatement(endProfiling, std::move(endProfilingArguments));
            mainBody->AddStatement(endProfilingStatement);
        }

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

        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::debug_heap))
        {
            Cm::Sym::FunctionSymbol* dbgHeapReport = symbolTable.GetOverload("dbgheap_report");
            Cm::BoundTree::BoundExpressionList dbgHeapReportArgs;
            Cm::BoundTree::BoundFunctionCallStatement* dbgHeapReportStatement = new Cm::BoundTree::BoundFunctionCallStatement(dbgHeapReport, std::move(dbgHeapReportArgs));
            mainBody->AddStatement(dbgHeapReportStatement);

            Cm::Sym::FunctionSymbol* dbgHeapDone = symbolTable.GetOverload("dbgheap_done");
            Cm::BoundTree::BoundExpressionList dbgHeapDoneArgs;
            Cm::BoundTree::BoundFunctionCallStatement* dbgHeapDoneStatement = new Cm::BoundTree::BoundFunctionCallStatement(dbgHeapDone, std::move(dbgHeapDoneArgs));
            mainBody->AddStatement(dbgHeapDoneStatement);
        }

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

        if (stackSize != 0)
        {
            Cm::Sym::FunctionSymbol* setStackSizeFun = symbolTable.GetOverload("set_stack_size");
            if (!setStackSizeFun)
            {
                throw std::runtime_error("set_stack_size function not found");
            }
            Cm::BoundTree::BoundExpressionList setStackSizeArgs;
            Cm::BoundTree::BoundLiteral* stackSizeArg = new Cm::BoundTree::BoundLiteral(nullptr);
            stackSizeArg->SetValue(new Cm::Sym::ULongValue(stackSize));
            Cm::Sym::TypeSymbol* ulongType = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ulongId));
            stackSizeArg->SetType(ulongType);
            setStackSizeArgs.Add(stackSizeArg);
            Cm::BoundTree::BoundFunctionCallStatement* setStackSizeStatement = new Cm::BoundTree::BoundFunctionCallStatement(setStackSizeFun, std::move(setStackSizeArgs));
            mainBody->AddStatement(setStackSizeStatement);
        }

        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::debug_heap))
        {
            Cm::Sym::FunctionSymbol* dbgHeapInit = symbolTable.GetOverload("dbgheap_init");
            Cm::BoundTree::BoundExpressionList dbgHeapInitArgs;
            Cm::BoundTree::BoundFunctionCallStatement* dbgHeapInitStatement = new Cm::BoundTree::BoundFunctionCallStatement(dbgHeapInit, std::move(dbgHeapInitArgs));
            mainBody->AddStatement(dbgHeapInitStatement);
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

        if (Cm::Core::GetGlobalSettings()->Config() != "full")
        {
            Cm::Sym::FunctionSymbol* initClassHierarchy = symbolTable.GetOverload("init_class_hierarchy");
            Cm::BoundTree::BoundExpressionList initClassHierarchyArguments;
            Cm::BoundTree::BoundClassHierarchyTableConstant* classHierarchyTableConstant = new Cm::BoundTree::BoundClassHierarchyTableConstant();
            classHierarchyTableConstant->SetType(symbolTable.GetTypeRepository().MakePointerType(symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ulongId)),
                userMainFunction->GetSpan()));
            initClassHierarchyArguments.Add(classHierarchyTableConstant);
            Cm::BoundTree::BoundLiteral* numEntries = new Cm::BoundTree::BoundLiteral(nullptr);
            numEntries->SetValue(new Cm::Sym::IntValue(numClassHierarchyTableEntries));
            numEntries->SetType(intType);
            initClassHierarchyArguments.Add(numEntries);
            Cm::BoundTree::BoundFunctionCallStatement* callInitClassHierarchyStatement = new Cm::BoundTree::BoundFunctionCallStatement(initClassHierarchy, std::move(initClassHierarchyArguments));
            mainBody->AddStatement(callInitClassHierarchyStatement);
        }

        if (Cm::Core::GetGlobalSettings()->Config() == "profile")
        {
            Cm::Sym::FunctionSymbol* startProfiling = symbolTable.GetOverload("start_profiling");
            Cm::BoundTree::BoundExpressionList startProfilingArguments;
            int profDataFileId = mainCompileUnit.StringRepository().Install(profDataFilePath);
            Cm::BoundTree::BoundStringLiteral* profDataFileArg = new Cm::BoundTree::BoundStringLiteral(nullptr, profDataFileId);
            Cm::Sym::TypeSymbol* constCharPtrType = symbolTable.GetTypeRepository().MakeConstCharPtrType(userMainFunction->GetSpan());
            profDataFileArg->SetType(constCharPtrType);
            startProfilingArguments.Add(profDataFileArg);
            Cm::BoundTree::BoundFunctionCallStatement* startProfilingStatement = new Cm::BoundTree::BoundFunctionCallStatement(startProfiling, std::move(startProfilingArguments));
            mainBody->AddStatement(startProfilingStatement);
        }

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

        if (Cm::Core::GetGlobalSettings()->Config() == "profile")
        {
            Cm::Sym::FunctionSymbol* endProfiling = symbolTable.GetOverload("end_profiling");
            Cm::BoundTree::BoundExpressionList endProfilingArguments;
            Cm::BoundTree::BoundFunctionCallStatement* endProfilingStatement = new Cm::BoundTree::BoundFunctionCallStatement(endProfiling, std::move(endProfilingArguments));
            mainBody->AddStatement(endProfilingStatement);
        }

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

        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::debug_heap))
        {
            Cm::Sym::FunctionSymbol* dbgHeapReport = symbolTable.GetOverload("dbgheap_report");
            Cm::BoundTree::BoundExpressionList dbgHeapReportArgs;
            Cm::BoundTree::BoundFunctionCallStatement* dbgHeapReportStatement = new Cm::BoundTree::BoundFunctionCallStatement(dbgHeapReport, std::move(dbgHeapReportArgs));
            mainBody->AddStatement(dbgHeapReportStatement);

            Cm::Sym::FunctionSymbol* dbgHeapDone = symbolTable.GetOverload("dbgheap_done");
            Cm::BoundTree::BoundExpressionList dbgHeapDoneArgs;
            Cm::BoundTree::BoundFunctionCallStatement* dbgHeapDoneStatement = new Cm::BoundTree::BoundFunctionCallStatement(dbgHeapDone, std::move(dbgHeapDoneArgs));
            mainBody->AddStatement(dbgHeapDoneStatement);
        }
    }

    Cm::BoundTree::BoundReturnStatement* returnStatement = new Cm::BoundTree::BoundReturnStatement(nullptr);
    std::unique_ptr<Cm::Sym::FunctionSymbol> intCopyCtor(new Cm::Core::CopyCtor(symbolTable.GetTypeRepository(), intType));
    returnStatement->SetConstructor(intCopyCtor.get());
    returnStatement->SetExpression(new Cm::BoundTree::BoundLocalVariable(nullptr, returnValueVariable));
    returnStatement->SetReturnType(intType);
    mainBody->AddStatement(returnStatement);

    mainCompileUnit.AddBoundNode(mainFunction);
    mainFunctionSymbol->SetParent(userMainFunction->Parent());
    Emit(symbolTable.GetTypeRepository(), mainCompileUnit, nullptr);
    GenerateObjectCode(mainCompileUnit);
    return true;
}

} } // namespace Bm::Build
