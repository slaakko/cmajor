#ifndef Identifier_hpp_10102
#define Identifier_hpp_10102

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>

namespace Cm { namespace Parsing { namespace Cpp {

class IdentifierGrammar : public Cm::Parsing::Grammar
{
public:
    static IdentifierGrammar* Create();
    static IdentifierGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    std::string Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    IdentifierGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class IdentifierRule;
    class QualifiedIdRule;
};

} } } // namespace Cm.Parsing.Cpp

#endif // Identifier_hpp_10102
