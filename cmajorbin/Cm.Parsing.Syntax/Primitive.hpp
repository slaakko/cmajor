#ifndef Primitive_hpp_6331
#define Primitive_hpp_6331

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Keyword.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

class PrimitiveGrammar : public Cm::Parsing::Grammar
{
public:
    static PrimitiveGrammar* Create();
    static PrimitiveGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Parsing::Parser* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    PrimitiveGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class PrimitiveRule;
    class CharRule;
    class StringRule;
    class CharSetRule;
    class CharSetRangeRule;
    class CharSetCharRule;
    class KeywordRule;
    class KeywordBodyRule;
    class KeywordListRule;
    class KeywordListBodyRule;
    class EmptyRule;
    class SpaceRule;
    class AnyCharRule;
    class LetterRule;
    class DigitRule;
    class HexDigitRule;
    class PunctuationRule;
};

} } } // namespace Cm.Parsing.Syntax

#endif // Primitive_hpp_6331
