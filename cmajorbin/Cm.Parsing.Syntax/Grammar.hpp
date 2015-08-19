#ifndef Grammar_hpp_26873
#define Grammar_hpp_26873

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parsing/Grammar.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

class GrammarGrammar : public Cm::Parsing::Grammar
{
public:
    static GrammarGrammar* Create();
    static GrammarGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Parsing::Grammar* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, Cm::Parsing::Scope* enclosingScope);
private:
    GrammarGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class GrammarRule;
    class GrammarContentRule;
    class StartClauseRule;
    class SkipClauseRule;
};

} } } // namespace Cm.Parsing.Syntax

#endif // Grammar_hpp_26873
