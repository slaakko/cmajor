#ifndef Rule_hpp_10647
#define Rule_hpp_10647

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parsing/Rule.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

class RuleGrammar : public Cm::Parsing::Grammar
{
public:
    static RuleGrammar* Create();
    static RuleGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Parsing::Rule* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, Cm::Parsing::Scope* enclosingScope);
private:
    RuleGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class RuleRule;
    class RuleHeaderRule;
    class RuleBodyRule;
};

} } } // namespace Cm.Parsing.Syntax

#endif // Rule_hpp_10647
