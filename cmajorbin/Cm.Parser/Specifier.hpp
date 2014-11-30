#ifndef Specifier_hpp_5728
#define Specifier_hpp_5728

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Ast/Specifier.hpp>

namespace Cm { namespace Parser {

class SpecifierGrammar : public Cm::Parsing::Grammar
{
public:
    static SpecifierGrammar* Create();
    static SpecifierGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::Specifiers Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    SpecifierGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class SpecifiersRule;
    class SpecifierRule;
};

} } // namespace Cm.Parser

#endif // Specifier_hpp_5728
