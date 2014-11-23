#ifndef Primary_hpp_10647
#define Primary_hpp_10647

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parsing/Parser.hpp>
#include <Cm.Parsing/Scope.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

class PrimaryGrammar : public Cm::Parsing::Grammar
{
public:
    static PrimaryGrammar* Create();
    static PrimaryGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Parsing::Parser* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, Cm::Parsing::Scope* enclosingScope);
private:
    PrimaryGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class PrimaryRule;
    class RuleCallRule;
    class NonterminalRule;
    class AliasRule;
    class GroupingRule;
    class TokenRule;
    class ExpectationRule;
    class ActionRule;
};

} } } // namespace Cm.Parsing.Syntax

#endif // Primary_hpp_10647
