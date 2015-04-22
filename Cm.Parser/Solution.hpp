#ifndef Solution_hpp_3809
#define Solution_hpp_3809

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Ast/Solution.hpp>

namespace Cm { namespace Parser {

class SolutionGrammar : public Cm::Parsing::Grammar
{
public:
    static SolutionGrammar* Create();
    static SolutionGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::Solution* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    SolutionGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class SolutionRule;
    class DeclarationsRule;
    class DeclarationRule;
    class ProjectFileDeclarationRule;
    class ActiveProjectDeclarationRule;
    class ProjectDependencyDeclarationRule;
    class FilePathRule;
};

} } // namespace Cm.Parser

#endif // Solution_hpp_3809
