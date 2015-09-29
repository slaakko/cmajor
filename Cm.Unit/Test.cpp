/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Unit/Test.hpp>
#include <Cm.Build/Build.hpp>
#include <Cm.Bind/ClassTemplateRepository.hpp>
#include <Cm.Bind/InlineFunctionRepository.hpp>
#include <Cm.Bind/SynthesizedClassFun.hpp>
#include <Cm.Bind/DelegateTypeOpRepository.hpp>
#include <Cm.Bind/ClassDelegateTypeOpRepository.hpp>
#include <Cm.Bind/Prebinder.hpp>
#include <Cm.Bind/VirtualBinder.hpp>
#include <Cm.Bind/ArrayTypeOpRepository.hpp>
#include <Cm.Parser/Project.hpp>
#include <Cm.Parser/Solution.hpp>
#include <Cm.Parser/CompileUnit.hpp>
#include <Cm.Parser/FileRegistry.hpp>
#include <Cm.Parser/ToolError.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Core/GlobalSettings.hpp>
#include <Cm.Core/ConceptRepository.hpp>
#include <Cm.Core/InitSymbolTable.hpp>
#include <Cm.Core/CompileUnitMap.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/ExceptionTable.hpp>
#include <Cm.Sym/MutexTable.hpp>
#include <Cm.Sym/ClassCounter.hpp>
#include <Cm.Sym/DeclarationVisitor.hpp>
#include <Cm.Sym/Conditional.hpp>
#include <Cm.Ast/Function.hpp>
#include <Cm.Ast/BasicType.hpp>
#include <Cm.Ast/Literal.hpp>
#include <Cm.Ast/Statement.hpp>
#include <Cm.Ast/Expression.hpp>
#include <Cm.Ast/Clone.hpp>
#include <Cm.Util/MappedInputFile.hpp>
#include <Cm.Util/Path.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <Cm.Util/System.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <stdlib.h>

namespace Cm { namespace Unit {

std::string GetOs()
{
#ifdef _WIN32
    return "windows";
#else
    return "linux";
#endif
}

Cm::Ast::FunctionNode* CreateDriverFunction(const std::string& unitTestName)
{
    Cm::Ast::FunctionNode* driverFunction = new Cm::Ast::FunctionNode(Cm::Parsing::Span(), Cm::Ast::Specifiers::public_ | Cm::Ast::Specifiers::nothrow_,
        new Cm::Ast::IntNode(Cm::Parsing::Span()), new Cm::Ast::FunctionGroupIdNode(Cm::Parsing::Span(), "main"));
    Cm::Ast::CompoundStatementNode* body = new Cm::Ast::CompoundStatementNode(Cm::Parsing::Span());

    Cm::Sym::ExceptionTable* exceptionTable = Cm::Sym::GetExceptionTable();
    Cm::Ast::InvokeNode* threadTblInitCall = new Cm::Ast::InvokeNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "threadtbl_init"));
    threadTblInitCall->AddArgument(new Cm::Ast::IntLiteralNode(Cm::Parsing::Span(), exceptionTable->GetNumberOfExceptions()));
    Cm::Ast::SimpleStatementNode* threadTblInitStatement = new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), threadTblInitCall);
    body->AddStatement(threadTblInitStatement);

    Cm::Sym::MutexTable* mutexTable = Cm::Sym::GetMutexTable();
    Cm::Ast::InvokeNode* mutexTblInitCall = new Cm::Ast::InvokeNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "mutextbl_init"));
    mutexTblInitCall->AddArgument(new Cm::Ast::IntLiteralNode(Cm::Parsing::Span(), (mutexTable->GetNumberOfMutexes())));
    Cm::Ast::SimpleStatementNode* mutexTblInitStatement = new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), mutexTblInitCall);
    body->AddStatement(mutexTblInitStatement);

    Cm::Ast::StringLiteralNode* unitTestNameLiteral = new Cm::Ast::StringLiteralNode(Cm::Parsing::Span(), unitTestName);
    Cm::Ast::InvokeNode* beginUnitTestCall = new Cm::Ast::InvokeNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "begin_unit_test"));
    beginUnitTestCall->AddArgument(unitTestNameLiteral);
    Cm::Ast::SimpleStatementNode* beginUnitTestStatement = new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), beginUnitTestCall);
    body->AddStatement(beginUnitTestStatement);

    Cm::Ast::InvokeNode* unitTestCall = new Cm::Ast::InvokeNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), unitTestName));
    Cm::Ast::SimpleStatementNode* callUnitTestStatement = new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), unitTestCall);
    Cm::Ast::CompoundStatementNode* tryBlock = new Cm::Ast::CompoundStatementNode(Cm::Parsing::Span());
    tryBlock->AddStatement(callUnitTestStatement);
    Cm::Ast::TryStatementNode* tryStatement = new Cm::Ast::TryStatementNode(Cm::Parsing::Span(), tryBlock);
    Cm::Ast::CompoundStatementNode* catchBlock = new Cm::Ast::CompoundStatementNode(Cm::Parsing::Span());

    Cm::Ast::InvokeNode* systemConsoleErrorCall = new Cm::Ast::InvokeNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "System.Console.Error"));
    Cm::Ast::ShiftLeftNode* shiftLeft1 = new Cm::Ast::ShiftLeftNode(Cm::Parsing::Span(), systemConsoleErrorCall, new Cm::Ast::StringLiteralNode(Cm::Parsing::Span(), "  caught an exception: "));
    Cm::Ast::InvokeNode* exToStringCall = new Cm::Ast::InvokeNode(Cm::Parsing::Span(), new Cm::Ast::DotNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "ex"),
        new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "ToString")));
    Cm::Ast::ShiftLeftNode* shiftLeft2 = new Cm::Ast::ShiftLeftNode(Cm::Parsing::Span(), shiftLeft1, exToStringCall);
    Cm::Ast::InvokeNode* endlCall = new Cm::Ast::InvokeNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "endl"));
    Cm::Ast::ShiftLeftNode* shiftLeft3 = new Cm::Ast::ShiftLeftNode(Cm::Parsing::Span(), shiftLeft2, endlCall);
    Cm::Ast::SimpleStatementNode* writeExStatement = new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), shiftLeft3);
    Cm::Ast::CompoundStatementNode* tryWriteBlock = new Cm::Ast::CompoundStatementNode(Cm::Parsing::Span());
    tryWriteBlock->AddStatement(writeExStatement);
    Cm::Ast::TryStatementNode* tryWrite = new Cm::Ast::TryStatementNode(Cm::Parsing::Span(), tryWriteBlock);
    Cm::Ast::CompoundStatementNode* catchWriteBlock = new Cm::Ast::CompoundStatementNode(Cm::Parsing::Span());
    Cm::Ast::DerivationList constRefDerivations;
    constRefDerivations.Add(Cm::Ast::Derivation::const_);
    constRefDerivations.Add(Cm::Ast::Derivation::reference);
    Cm::Ast::CatchNode* catchEverything = new Cm::Ast::CatchNode(Cm::Parsing::Span(), new Cm::Ast::DerivedTypeExprNode(Cm::Parsing::Span(), constRefDerivations,
        new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "System.Exception")), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "ex_"), catchWriteBlock);
    tryWrite->AddHandler(catchEverything);
    catchBlock->AddStatement(tryWrite);
    Cm::Ast::ReturnStatementNode* returnTwoStatement = new Cm::Ast::ReturnStatementNode(Cm::Parsing::Span(), new Cm::Ast::IntLiteralNode(Cm::Parsing::Span(), 2));
    catchBlock->AddStatement(returnTwoStatement);
    Cm::Ast::CatchNode* handler = new Cm::Ast::CatchNode(Cm::Parsing::Span(), new Cm::Ast::DerivedTypeExprNode(Cm::Parsing::Span(), constRefDerivations,
        new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "System.Exception")), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "ex"), catchBlock);
    tryStatement->AddHandler(handler);
    body->AddStatement(tryStatement);

    Cm::Ast::InvokeNode* cmExitCall = new Cm::Ast::InvokeNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "cm_exit"));
    Cm::Ast::SimpleStatementNode* cmExitStatement = new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), cmExitCall);
    body->AddStatement(cmExitStatement);

    Cm::Ast::InvokeNode* mutexTblDoneCall = new Cm::Ast::InvokeNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "mutextbl_done"));
    Cm::Ast::SimpleStatementNode* mutexTblDoneStatement = new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), mutexTblDoneCall);
    body->AddStatement(mutexTblDoneStatement);

    Cm::Ast::InvokeNode* threadTblDoneCall = new Cm::Ast::InvokeNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "threadtbl_done"));
    Cm::Ast::SimpleStatementNode* threadTblDoneStatement = new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), threadTblDoneCall);
    body->AddStatement(threadTblDoneStatement);

    Cm::Ast::StringLiteralNode* unitTestNameLiteral2 = new Cm::Ast::StringLiteralNode(Cm::Parsing::Span(), unitTestName);
    Cm::Ast::InvokeNode* endUnitTestCall = new Cm::Ast::InvokeNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "end_unit_test"));
    endUnitTestCall->AddArgument(unitTestNameLiteral2);
    Cm::Ast::SimpleStatementNode* endUnitTestStatement = new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), endUnitTestCall);
    body->AddStatement(endUnitTestStatement);
    Cm::Ast::ReturnStatementNode* returnZeroStatement = new Cm::Ast::ReturnStatementNode(Cm::Parsing::Span(), new Cm::Ast::IntLiteralNode(Cm::Parsing::Span(), 0));
    body->AddStatement(returnZeroStatement);

    driverFunction->SetBody(body);
    return driverFunction;
}

void BuildSymbolTable(Cm::Sym::SymbolTable& symbolTable, Cm::Core::GlobalConceptData& globalConceptData, Cm::Ast::CompileUnitNode* testUnit, Cm::Ast::Project* project, 
    const std::vector<std::string>& libraryDirs, std::vector<std::string>& assemblyFilePaths, std::vector<std::string>& cLibs, std::vector<std::string>& allReferenceFilePaths, 
    std::vector<std::string>& allDebugInfoFilePaths, std::vector<std::string>& allBcuPaths, const std::string& unitTestName)
{
    std::vector<uint64_t> classHierarchyTable;
    Cm::Core::InitSymbolTable(symbolTable, globalConceptData);
    Cm::Build::ImportModules(symbolTable, project, libraryDirs, assemblyFilePaths, cLibs, allReferenceFilePaths, allDebugInfoFilePaths, allBcuPaths, classHierarchyTable);
    testUnit->GlobalNs()->AddMember(CreateDriverFunction(unitTestName));
    symbolTable.InitVirtualFunctionTables();
    Cm::Sym::DeclarationVisitor declarationVisitor(symbolTable);
    testUnit->Accept(declarationVisitor);
}

Cm::Parser::ToolErrorGrammar* toolErrorGrammar = nullptr;

void Link(const std::vector<std::string>& assemblyFilePaths, const std::vector<std::string>& cLibs, const std::vector<std::string>& objectFilePaths, const std::string& executableFilePath)
{
    std::string ccCommand = "gcc";
    for (const std::string& objectFilePath : objectFilePaths)
    {
        ccCommand.append(" ").append(Cm::Util::QuotedPath(objectFilePath));
    }
    ccCommand.append(" -pthread -Xlinker --allow-multiple-definition");
    ccCommand.append(" -Xlinker --start-group");
    for (const std::string& assemblyFilePath : assemblyFilePaths)
    {
        ccCommand.append(" ").append(Cm::Util::QuotedPath(assemblyFilePath));
    }
    for (const std::string& clib : cLibs)
    {
        ccCommand.append(" -l").append(clib);
    }
    ccCommand.append(" -Xlinker --end-group");
    ccCommand.append(" -o ").append(Cm::Util::QuotedPath(executableFilePath));
    std::string exeErrorFilePath = Cm::Util::GetFullPath(boost::filesystem::path(executableFilePath).replace_extension(".exe.error").generic_string());
    try
    {
        Cm::Util::System(ccCommand, 2, exeErrorFilePath);
    }
    catch (const std::exception&)
    {
        Cm::Util::MappedInputFile file(exeErrorFilePath);
        try
        {
            if (!toolErrorGrammar)
            {
                toolErrorGrammar = Cm::Parser::ToolErrorGrammar::Create();
            }
            Cm::Util::ToolError toolError = toolErrorGrammar->Parse(file.Begin(), file.End(), 0, exeErrorFilePath);
            throw Cm::Core::ToolErrorExcecption(toolError);
        }
        catch (const std::exception&)
        {
            std::string errorText(file.Begin(), file.End());
            throw std::runtime_error(errorText);
        }
    }
    boost::filesystem::remove(exeErrorFilePath);
}

std::string Compile(Cm::Ast::CompileUnitNode* testUnit, Cm::Ast::Project* project, const std::string& unitTestName, const std::unordered_set<std::string>& defines)
{
    Cm::Core::CompileUnitMap compileUnitMap;
    Cm::Core::SetCompileUnitMap(&compileUnitMap);
    Cm::Core::GlobalConceptData globalConceptData;
    Cm::Core::SetGlobalConceptData(&globalConceptData);
    Cm::Sym::SymbolTable symbolTable;
    Cm::Sym::SymbolTypeSetCollection symbolTypeSetCollection;
    Cm::Sym::SetSymbolTypeSetCollection(&symbolTypeSetCollection);
    Cm::Sym::ExceptionTable exceptionTable;
    Cm::Sym::SetExceptionTable(&exceptionTable);
    Cm::Sym::MutexTable mutexTable;
    Cm::Sym::SetMutexTable(&mutexTable);
    Cm::Sym::ClassCounter classCounter;
    Cm::Sym::SetClassCounter(&classCounter);
    std::vector<std::string> libraryDirs;
    Cm::Build::GetLibraryDirectories(libraryDirs);
    std::vector<std::string> allReferenceFilePaths;
    std::vector<std::string> allDebugInfoFilePaths;
    std::vector<std::string> allBcuPaths;
    std::vector<std::string> assemblyFilePaths;
    std::vector<std::string> cLibs;
    std::vector<std::string> objectFilePaths;
    std::unordered_set<std::string> allDefines = defines;
    Cm::Build::AddPlatformAndConfigDefines(allDefines);
    Cm::Sym::Define(allDefines);
    BuildSymbolTable(symbolTable, globalConceptData, testUnit, project, libraryDirs, assemblyFilePaths, cLibs, allReferenceFilePaths, allDebugInfoFilePaths, allBcuPaths, unitTestName);
    boost::filesystem::create_directories(project->OutputBasePath());
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
    std::string prebindCompileUnitIrFilePath = Cm::Util::GetFullPath((project->OutputBasePath() / boost::filesystem::path("__prebind__" + ext)).generic_string());
    Cm::BoundTree::BoundCompileUnit prebindCompileUnit(testUnit, prebindCompileUnitIrFilePath, symbolTable);
    prebindCompileUnit.SetPrebindCompileUnit();
    prebindCompileUnit.SetClassTemplateRepository(new Cm::Bind::ClassTemplateRepository(prebindCompileUnit));
    prebindCompileUnit.SetInlineFunctionRepository(new Cm::Bind::InlineFunctionRepository(prebindCompileUnit));
    prebindCompileUnit.SetSynthesizedClassFunRepository(new Cm::Bind::SynthesizedClassFunRepository(prebindCompileUnit));
    prebindCompileUnit.SetDelegateTypeOpRepository(new Cm::Bind::DelegateTypeOpRepository(prebindCompileUnit));
    prebindCompileUnit.SetClassDelegateTypeOpRepository(new Cm::Bind::ClassDelegateTypeOpRepository(prebindCompileUnit));
    prebindCompileUnit.SetArrayTypeOpRepository(new Cm::Bind::ArrayTypeOpRepository(prebindCompileUnit));
    std::vector<std::unique_ptr<Cm::Sym::FileScope>> fileScopes;
    Cm::Bind::Prebinder prebinder(symbolTable, prebindCompileUnit.ClassTemplateRepository());
    testUnit->Accept(prebinder);
    fileScopes.push_back(std::unique_ptr<Cm::Sym::FileScope>(prebinder.ReleaseFileScope()));
    Cm::Bind::VirtualBinder virtualBinder(symbolTable, testUnit, prebindCompileUnit);
    testUnit->Accept(virtualBinder);
    boost::filesystem::path p = boost::filesystem::path(testUnit->FilePath()).parent_path();
    p /= "stage";
    p /= boost::filesystem::path(testUnit->FilePath()).filename().replace_extension();
    p += "." + unitTestName + ext;
    boost::filesystem::create_directories(p.parent_path());
    std::string testUnitIrFilePath = Cm::Util::GetFullPath(p.generic_string());
    Cm::BoundTree::BoundCompileUnit boundCompileUnit(testUnit, testUnitIrFilePath, symbolTable);
    compileUnitMap.MapCompileUnit(testUnit, &boundCompileUnit);
    boundCompileUnit.SetClassTemplateRepository(new Cm::Bind::ClassTemplateRepository(boundCompileUnit));
    boundCompileUnit.SetInlineFunctionRepository(new Cm::Bind::InlineFunctionRepository(boundCompileUnit));
    boundCompileUnit.SetSynthesizedClassFunRepository(new Cm::Bind::SynthesizedClassFunRepository(boundCompileUnit));
    boundCompileUnit.SetDelegateTypeOpRepository(new Cm::Bind::DelegateTypeOpRepository(boundCompileUnit));
    boundCompileUnit.SetClassDelegateTypeOpRepository(new Cm::Bind::ClassDelegateTypeOpRepository(boundCompileUnit));
    boundCompileUnit.SetArrayTypeOpRepository(new Cm::Bind::ArrayTypeOpRepository(boundCompileUnit));
    boundCompileUnit.AddFileScope(fileScopes[0].release());
    Cm::Build::Bind(testUnit, boundCompileUnit);
    if (boundCompileUnit.HasGotos())
    {
        Cm::Build::AnalyzeControlFlow(boundCompileUnit);
    }
    Cm::Build::Emit(symbolTable.GetTypeRepository(), boundCompileUnit);
    Cm::Build::GenerateObjectCode(boundCompileUnit);
    objectFilePaths.push_back(boundCompileUnit.ObjectFilePath());
    Cm::Build::GenerateExceptionTableUnit(symbolTable, project->OutputBasePath().generic_string(), objectFilePaths, true);
    std::string exeExt;
#ifdef _WIN32
    exeExt = ".exe";
#endif
    std::string executableFilePath = Cm::Util::GetFullPath(p.replace_extension(exeExt).generic_string());
    Link(assemblyFilePaths, cLibs, objectFilePaths, executableFilePath);
    return executableFilePath;
}

int numSolutionTests = 0;
int solutionPassed = 0;
int solutionFailed = 0;
int solutionNotCompiled = 0;
int solutionExceptions = 0;
int solutionCrashed = 0;
int solutionUnknown = 0;
int numProjectTests = 0;
int projectPassed = 0;
int projectFailed = 0;
int projectNotCompiled = 0;
int projectExceptions = 0;
int projectCrashed = 0;
int projectUnknown = 0;
int numFileTests;
int filePassed = 0;
int fileFailed = 0;
int fileNotCompiled = 0;
int fileExceptions = 0;
int fileCrashed = 0;
int fileUnknown = 0;

void TestUnit(Cm::Ast::CompileUnitNode* testUnit, Cm::Ast::Project* project, const std::string& unitTestName, const std::unordered_set<std::string>& defines)
{
    ++numFileTests;
    std::string executableFilePath;
    try
    {
        executableFilePath = Compile(testUnit, project, unitTestName, defines);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "  could not compile unit test '" << unitTestName << "': " << ex.what() << std::endl;
        ++fileFailed;
        ++fileNotCompiled;
        return;
    }
    catch (...)
    {
        std::cerr << "  could not compile unit test '" << unitTestName << "'" << std::endl;
        ++fileFailed;
        ++fileNotCompiled;
        return;
    }
    int result = system(executableFilePath.c_str());
    switch (result)
    {
        case 0: 
        {
            ++filePassed;
            break;
        }
        case 1:
        {
            ++fileFailed;
            break;
        }
        case 2:
        {
            ++fileFailed;
            ++fileExceptions;
            break;
        }
        case 255:
        {
            ++fileFailed;
            ++fileCrashed;
            break;
        }
        default:
        {
            std::cerr << "  got unknown result '" << result << "' from unit test '" << unitTestName << "'" << std::endl;
            ++fileFailed;
            ++fileUnknown;
            break;
        }
    }
}

std::vector<std::pair<std::unique_ptr<Cm::Ast::CompileUnitNode>, std::string>> SplitIntoTestUnits(Cm::Ast::CompileUnitNode* compileUnit)
{
    std::vector<std::pair<std::unique_ptr<Cm::Ast::CompileUnitNode>, std::string>> testUnits;
    Cm::Ast::CloneContext makeUnitTestUnitContext;
    makeUnitTestUnitContext.SetMakeTestUnits();
    std::unique_ptr<Cm::Ast::CompileUnitNode> environmentNode(static_cast<Cm::Ast::CompileUnitNode*>(compileUnit->Clone(makeUnitTestUnitContext)));
    for (std::unique_ptr<Cm::Ast::FunctionNode>& unitTestFunction : makeUnitTestUnitContext.UnitTestFunctions())
    {
        std::string unitTestName = unitTestFunction->FullGroupName();
        Cm::Ast::CloneContext testUnitContext;
        std::pair<std::unique_ptr<Cm::Ast::CompileUnitNode>, std::string> testUnit = std::make_pair(
            std::unique_ptr<Cm::Ast::CompileUnitNode>(static_cast<Cm::Ast::CompileUnitNode*>(environmentNode->Clone(testUnitContext))), unitTestName);
        testUnit.first->GlobalNs()->AddMember(new Cm::Ast::NamespaceImportNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "System")));
        Cm::Ast::NamespaceNode* ns = testUnit.first->GetNamespace(unitTestFunction->Parent()->FullName());
        Cm::Ast::FunctionNode* unitTestFun = unitTestFunction.release();
        ns->AddMember(unitTestFun);
        testUnits.push_back(std::move(testUnit));
    }
    return testUnits;
}

Cm::Parser::CompileUnitGrammar* compileUnitGrammar = nullptr;

bool TestNameEquals(const std::string& testName, const std::string& testUnitName)
{
    std::vector<std::string> testNameComponents = Cm::Util::Split(testName, '.');
    std::vector<std::string> testUnitNameComponents = Cm::Util::Split(testUnitName, '.');
    int n = std::min(int(testNameComponents.size()), int(testUnitNameComponents.size()));
    for (int i = 0; i < n; ++i)
    {
        int k = int(testNameComponents.size()) - i - 1;
        int l = int(testUnitNameComponents.size()) - i - 1;
        if (testNameComponents[k] != testUnitNameComponents[l]) return false;
    }
    return true;
}

void TestSourceFile(const std::string& sourceFilePath, Cm::Ast::Project* project, const std::string& testName, const std::unordered_set<std::string>& defines)
{
    numFileTests = 0;
    filePassed = 0;
    fileFailed = 0;
    fileNotCompiled = 0;
    fileExceptions = 0;
    fileCrashed = 0;
    fileUnknown = 0;
    std::cerr << sourceFilePath << std::endl;
    try
    {
        if (!compileUnitGrammar)
        {
            compileUnitGrammar = Cm::Parser::CompileUnitGrammar::Create();
        }
        Cm::Util::MappedInputFile sourceFile(sourceFilePath);
        Cm::Parser::FileRegistry::Init();
        int sourceFileIndex = Cm::Parser::FileRegistry::Instance()->RegisterParsedFile(sourceFilePath);
        Cm::Parser::ParsingContext ctx;
        std::unique_ptr<Cm::Ast::CompileUnitNode> compileUnit(compileUnitGrammar->Parse(sourceFile.Begin(), sourceFile.End(), sourceFileIndex, sourceFilePath, &ctx));
        std::vector<std::pair<std::unique_ptr<Cm::Ast::CompileUnitNode>, std::string>> testUnits = SplitIntoTestUnits(compileUnit.get());
        for (const std::pair<std::unique_ptr<Cm::Ast::CompileUnitNode>, std::string>& testUnit : testUnits)
        {
            if (!testName.empty())
            {
                if (!TestNameEquals(testName, testUnit.second)) continue;
            }
            TestUnit(testUnit.first.get(), project, testUnit.second, defines);
        }
    }
    catch (const Cm::Parsing::ExpectationFailure& ex)
    {
        std::cerr << "  could not compile file '" << sourceFilePath << "': " << ex.what() << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "  could not compile file '" << sourceFilePath << "': " << ex.what() << std::endl;
        ++fileFailed;
        ++fileNotCompiled;
    }
    catch (...)
    {
        std::cerr << "  could not compile file '" << sourceFilePath << "'" << std::endl;
        ++fileFailed;
        ++fileNotCompiled;
    }
    std::cerr << sourceFilePath << " (ran " << numFileTests << " tests): " << filePassed << " passed, " << fileFailed << " failed (" << fileNotCompiled << " did not compile, " << 
        fileExceptions << " exceptions, " << fileCrashed << " crashed, " << fileUnknown << " unknown results)" << std::endl;
    numProjectTests += numFileTests;
    projectPassed += filePassed;
    projectFailed += fileFailed;
    projectNotCompiled += fileNotCompiled;
    projectExceptions += fileExceptions;
    projectCrashed += fileCrashed;
    projectUnknown += fileUnknown;
    Cm::Parser::FileRegistry::Done();
}

bool SourceFileNameEquals(const std::string& fileName, const std::string& sourceFilePath)
{
    boost::filesystem::path filePath = fileName;
    std::vector<std::string> filePathComponents = Cm::Util::Split(filePath.generic_string(), '/');
    std::vector<std::string> sourceFilePathComponents = Cm::Util::Split(sourceFilePath, '/');
    int n = std::min(int(filePathComponents.size()), int(sourceFilePathComponents.size()));
    for (int i = 0; i < n; ++i)
    {
        int k = int(filePathComponents.size()) - i - 1;
        int l = int(sourceFilePathComponents.size()) - i - 1;
        if (filePathComponents[k] != sourceFilePathComponents[l]) return false;
    }
    return true;

}

bool TestProject(Cm::Ast::Project* project, const std::string& fileName, const std::string& testName, const std::unordered_set<std::string>& defines)
{
    numProjectTests = 0;
    projectPassed = 0;
    projectFailed = 0;
    projectNotCompiled = 0;
    projectExceptions = 0;
    projectCrashed = 0;
    projectUnknown = 0;
    std::cerr << "testing project '" << project->Name() << "' using " + Cm::IrIntf::GetBackEndStr() + " backend and " + Cm::Core::GetGlobalSettings()->Config()  + " configuration..." << std::endl;
    for (const std::string& sourceFilePath : project->SourceFilePaths())
    {
        if (!fileName.empty())
        {
            if (!SourceFileNameEquals(fileName, sourceFilePath)) continue;
        }
        TestSourceFile(sourceFilePath, project, testName, defines);
    }
    std::cerr << "test results for project '" << project->Name() << "' (ran " << numProjectTests << " tests):\n" <<
        "passed             : " << projectPassed << "\n" <<
        "failed:            : " << projectFailed << "\n" <<
        "  did not compile  : " << projectNotCompiled << "\n" <<
        "  exceptions       : " << projectExceptions << "\n" <<
        "  crashed          : " << projectCrashed << "\n" <<
        "  unknown result   : " << projectUnknown << "\n" <<
        std::endl;
    numSolutionTests += numProjectTests;
    solutionPassed += projectPassed;
    solutionFailed += projectFailed;
    solutionNotCompiled += projectNotCompiled;
    solutionExceptions += projectExceptions;
    solutionCrashed += projectExceptions;
    solutionUnknown += projectUnknown;
    return projectFailed == 0;
}

Cm::Parser::ProjectGrammar* projectGrammar = nullptr;

bool TestProject(const std::string& projectFilePath, const std::string& fileName, const std::string& testName, const std::unordered_set<std::string>& defines)
{
    if (!boost::filesystem::exists(projectFilePath))
    {
        throw std::runtime_error("project file '" + projectFilePath + "' not found");
    }
    Cm::Util::MappedInputFile projectFile(projectFilePath);
    if (!projectGrammar)
    {
        projectGrammar = Cm::Parser::ProjectGrammar::Create();
    }
    std::unique_ptr<Cm::Ast::Project> project(projectGrammar->Parse(projectFile.Begin(), projectFile.End(), 0, projectFilePath, Cm::Core::GetGlobalSettings()->Config(),
        Cm::IrIntf::GetBackEndStr(), GetOs()));
    project->ResolveDeclarations();
    return TestProject(project.get(), fileName, testName, defines);
}

Cm::Parser::SolutionGrammar* solutionGrammar = nullptr;

bool TestSolution(const std::string& solutionFilePath, const std::string& fileName, const std::string& testName, const std::unordered_set<std::string>& defines)
{
    boost::filesystem::path currentPath = boost::filesystem::current_path();
    numSolutionTests = 0;
    solutionPassed = 0;
    solutionFailed = 0;
    solutionNotCompiled = 0;
    solutionExceptions = 0;
    solutionCrashed = 0;
    solutionUnknown = 0;
    if (!boost::filesystem::exists(solutionFilePath))
    {
        throw std::runtime_error("solution file '" + solutionFilePath + "' not found");
    }
    if (!solutionGrammar)
    {
        solutionGrammar = Cm::Parser::SolutionGrammar::Create();
    }
    Cm::Util::MappedInputFile solutionFile(solutionFilePath);
    std::unique_ptr<Cm::Ast::Solution> solution(solutionGrammar->Parse(solutionFile.Begin(), solutionFile.End(), 0, solutionFilePath));
    solution->ResolveDeclarations();
    std::cerr << "testing solution '" << solution->Name() << "' using " + Cm::IrIntf::GetBackEndStr() + " backend and " + Cm::Core::GetGlobalSettings()->Config() + " configuration..." << std::endl;
    for (const std::string& projectFilePath : solution->ProjectFilePaths())
    {
        boost::filesystem::path pfp = projectFilePath;
        boost::filesystem::current_path(pfp.parent_path());
        TestProject(projectFilePath, fileName, testName, defines);
    }
    std::cerr << "test results for solution '" << solution->Name() << "' (ran " << numSolutionTests << " tests):\n" <<
        "passed             : " << solutionPassed << "\n" <<
        "failed:            : " << solutionFailed << "\n" <<
        "  did not compile  : " << solutionNotCompiled << "\n" << 
        "  exceptions       : " << solutionExceptions << "\n" <<
        "  crashed          : " << solutionCrashed << "\n" <<
        "  unknown result   : " << solutionUnknown << "\n" << 
        std::endl;
    boost::filesystem::current_path(currentPath);
    return solutionFailed == 0;
}

} } // Cm::Unit
