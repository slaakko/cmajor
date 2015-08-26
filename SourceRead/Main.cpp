#include <Cm.Parser/Solution.hpp>
#include <Cm.Parser/Project.hpp>
#include <Cm.Parser/CompileUnit.hpp>
#include <Cm.Parsing/InitDone.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Ast/InitDone.hpp>
#include <Cm.Util/MappedInputFile.hpp>
#include <iostream>

void ReadProject(Cm::Parser::ProjectGrammar* projectGrammar, Cm::Parser::CompileUnitGrammar* compileUnitGrammar, const std::string& projectFilePath);
void ReadCompileUnit(Cm::Parser::CompileUnitGrammar* compileUnitGrammar, const std::string& sourceFilePath);
using Cm::Parsing::SetCountSourceLines;
using Cm::Parsing::GetParsedSourceLines;

void ReadSolution(Cm::Parser::SolutionGrammar* solutionGrammar, Cm::Parser::ProjectGrammar* projectGrammar, Cm::Parser::CompileUnitGrammar* compileUnitGrammar, const std::string& solutionFilePath)
{
    Cm::Util::MappedInputFile solutionFile(solutionFilePath);
    std::unique_ptr<Cm::Ast::Solution> solution(solutionGrammar->Parse(solutionFile.Begin(), solutionFile.End(), 0, solutionFilePath));
    solution->ResolveDeclarations();
    std::cout << solutionFilePath << ": " << solution->Name() << std::endl;
    std::cout << "*********************************************************" << std::endl;
    for (const std::string& projectFilePath : solution->ProjectFilePaths())
    {
        ReadProject(projectGrammar, compileUnitGrammar, projectFilePath);
    }
    std::cout << "*********************************************************" << std::endl;
}

void ReadProject(Cm::Parser::ProjectGrammar* projectGrammar, Cm::Parser::CompileUnitGrammar* compileUnitGrammar, const std::string& projectFilePath)
{
    Cm::Util::MappedInputFile projectFile(projectFilePath);
    std::unique_ptr<Cm::Ast::Project> project(projectGrammar->Parse(projectFile.Begin(), projectFile.End(), 0, projectFilePath, "debug", "llvm", "windows"));
    project->ResolveDeclarations();
    std::cout << projectFilePath << ": " << project->Name() << std::endl;
    std::cout << "=========================================================" << std::endl;
    for (const std::string& sourceFilePath : project->SourceFilePaths())
    {
        ReadCompileUnit(compileUnitGrammar, sourceFilePath);
    }
    std::cout << "=========================================================" << std::endl;
}

void ReadCompileUnit(Cm::Parser::CompileUnitGrammar* compileUnitGrammar, const std::string& sourceFilePath)
{
    Cm::Util::MappedInputFile sourceFile(sourceFilePath);
    Cm::Parser::ParsingContext ctx;
    std::string content(sourceFile.Begin(), sourceFile.End());
    std::cout << "> " << sourceFilePath << std::endl;
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
        std::cout << "usage: sread {file.cms | file.cmp | file.cm}" << std::endl;
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
            boost::filesystem::path p(arg);
            if (p.extension() == ".cms")
            {
                ReadSolution(solutionGrammar, projectGrammar, compileUnitGrammar, arg);
            }
            else if (p.extension() == ".cmp")
            {
                ReadProject(projectGrammar, compileUnitGrammar, arg);
            }
            else if (p.extension() == ".cm")
            {
                ReadCompileUnit(compileUnitGrammar, arg);
            }
            else
            {
                throw std::runtime_error("argument '" + arg + " is not Cmajor solution, project or source file");
            }
        }
    }
    catch (const Cm::Parsing::ExpectationFailure& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
