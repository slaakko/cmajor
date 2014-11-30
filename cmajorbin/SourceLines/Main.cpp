#include <Cm.Parser/Solution.hpp>
#include <Cm.Parser/Project.hpp>
#include <Cm.Parser/CompileUnit.hpp>
#include <Cm.Parsing/InitDone.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Ast/InitDone.hpp>
#include <Cm.Util/MappedInputFile.hpp>
#include <iostream>

int ParseProject(Cm::Parser::ProjectGrammar* projectGrammar, Cm::Parser::CompileUnitGrammar* compileUnitGrammar, const std::string& projectFilePath);
int ParseCompileUnit(Cm::Parser::CompileUnitGrammar* compileUnitGrammar, const std::string& sourceFilePath);
using Cm::Parsing::SetCountSourceLines;
using Cm::Parsing::GetParsedSourceLines;

void ParseSolution(Cm::Parser::SolutionGrammar* solutionGrammar, Cm::Parser::ProjectGrammar* projectGrammar, Cm::Parser::CompileUnitGrammar* compileUnitGrammar, const std::string& solutionFilePath)
{
    Cm::Util::MappedInputFile solutionFile(solutionFilePath);
    std::unique_ptr<Cm::Ast::Solution> solution(solutionGrammar->Parse(solutionFile.Begin(), solutionFile.End(), 0, solutionFilePath));
    solution->ResolveDeclarations();
    std::cout << solutionFilePath << ": " << solution->Name() << std::endl;
    std::cout << "*********************************************************" << std::endl;
    int numLines = 0;
    for (const std::string& projectFilePath : solution->ProjectFilePaths())
    {
        int numProjectLines = ParseProject(projectGrammar, compileUnitGrammar, projectFilePath);
        numLines += numProjectLines;
    }
    std::cout << "*********************************************************" << std::endl;
    std::cout << numLines << std::endl;
}

int ParseProject(Cm::Parser::ProjectGrammar* projectGrammar, Cm::Parser::CompileUnitGrammar* compileUnitGrammar, const std::string& projectFilePath)
{
    Cm::Util::MappedInputFile projectFile(projectFilePath);
    std::unique_ptr<Cm::Ast::Project> project(projectGrammar->Parse(projectFile.Begin(), projectFile.End(), 0, projectFilePath, "debug", "llvm"));
    project->ResolveDeclarations();
    std::cout << projectFilePath << ": " << project->Name() << std::endl;
    std::cout << "=========================================================" << std::endl;
    int numLines = 0;
    for (const std::string& sourceFilePath : project->SourceFilePaths())
    {
        int numSourceLines = ParseCompileUnit(compileUnitGrammar, sourceFilePath);
        numLines += numSourceLines;
    }
    std::cout << "=========================================================" << std::endl;
    std::cout << numLines << std::endl;
    return numLines;
}

int ParseCompileUnit(Cm::Parser::CompileUnitGrammar* compileUnitGrammar, const std::string& sourceFilePath)
{
    Cm::Util::MappedInputFile sourceFile(sourceFilePath);
    SetCountSourceLines(true);
    Cm::Parser::ParsingContext ctx;
    std::unique_ptr<Cm::Ast::CompileUnit> compileUnit(compileUnitGrammar->Parse(sourceFile.Begin(), sourceFile.End(), 0, sourceFilePath, &ctx));
    SetCountSourceLines(false);
    int numSourceLines = GetParsedSourceLines();
    std::cout << "> " << sourceFilePath << " : " << numSourceLines << std::endl;
    return numSourceLines;
}

class InitDone
{
public:
    InitDone()
    {
        Cm::Parsing::Init();
        Cm::Ast::Init();
    }
    ~InitDone()
    {
        Cm::Ast::Done();
        Cm::Parsing::Done();
    }
};

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        std::cout << "usage: slines [options] {file.cms | file.cmp}" << std::endl;
        std::cout << "options:" << std::endl;
        std::cout << "-debug: debug parsing" << std::endl;
        return 0;
    }
    try
    {
        InitDone initDone;
        Cm::Parser::SolutionGrammar* solutionGrammar = Cm::Parser::SolutionGrammar::Create();
        Cm::Parser::ProjectGrammar* projectGrammar = Cm::Parser::ProjectGrammar::Create();
        Cm::Parser::CompileUnitGrammar* compileUnitGrammar = Cm::Parser::CompileUnitGrammar::Create();
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg == "-debug")
            {
                compileUnitGrammar->SetLog(&std::cout);
            }
            else
            {
                boost::filesystem::path p(arg);
                if (p.extension() == ".cms")
                {
                    ParseSolution(solutionGrammar, projectGrammar, compileUnitGrammar, arg);
                }
                else if (p.extension() == ".cmp")
                {
                    ParseProject(projectGrammar, compileUnitGrammar, arg);
                }
                else
                {
                    throw std::runtime_error("argument '" + arg + " is not Cmajor project or solution file");
                }
            }
        }
    }
    catch (const Cm::Parsing::CombinedParsingError& ex)
    {
        for (const Cm::Parsing::ExpectationFailure& f : ex.Errors())
        {
            std::cerr << f.what() << std::endl;
        }
        return 1;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}