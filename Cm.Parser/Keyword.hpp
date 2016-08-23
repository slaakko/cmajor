#ifndef Keyword_hpp_20065
#define Keyword_hpp_20065

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>

namespace Cm { namespace Parser {

class KeywordGrammar : public Cm::Parsing::Grammar
{
public:
    static KeywordGrammar* Create();
    static KeywordGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
private:
    std::vector<std::string> keywords0;
    KeywordGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
};

} } // namespace Cm.Parser

#endif // Keyword_hpp_20065
