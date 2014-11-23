#ifndef Keyword_hpp_15252
#define Keyword_hpp_15252

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>

namespace Cm { namespace Parsing { namespace Cpp {

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

} } } // namespace Cm.Parsing.Cpp

#endif // Keyword_hpp_15252
