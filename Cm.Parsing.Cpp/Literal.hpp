#ifndef Literal_hpp_15252
#define Literal_hpp_15252

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parsing.CppObjectModel/Literal.hpp>

namespace Cm { namespace Parsing { namespace Cpp {

class LiteralGrammar : public Cm::Parsing::Grammar
{
public:
    static LiteralGrammar* Create();
    static LiteralGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Parsing::CppObjectModel::Literal* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    LiteralGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class LiteralRule;
    class IntegerLiteralRule;
    class CharacterLiteralRule;
    class CCharSequenceRule;
    class FloatingLiteralRule;
    class StringLiteralRule;
    class BooleanLiteralRule;
    class PointerLiteralRule;
};

} } } // namespace Cm.Parsing.Cpp

#endif // Literal_hpp_15252
