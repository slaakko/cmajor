#ifndef Literal_hpp_20065
#define Literal_hpp_20065

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Ast/Literal.hpp>

namespace Cm { namespace Parser {

class LiteralGrammar : public Cm::Parsing::Grammar
{
public:
    static LiteralGrammar* Create();
    static LiteralGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::Node* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    LiteralGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class LiteralRule;
    class BooleanLiteralRule;
    class IntegerLiteralRule;
    class FloatingLiteralRule;
    class CharLiteralRule;
    class StringLiteralRule;
    class NullLiteralRule;
};

} } // namespace Cm.Parser

#endif // Literal_hpp_20065
