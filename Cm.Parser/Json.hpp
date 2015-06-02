#ifndef Json_hpp_12176
#define Json_hpp_12176

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Core/Json.hpp>

namespace Cm { namespace Parser {

class JsonGrammar : public Cm::Parsing::Grammar
{
public:
    static JsonGrammar* Create();
    static JsonGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Core::JsonValue* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    JsonGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class ValueRule;
    class ObjectRule;
    class ArrayRule;
    class StringRule;
    class NumberRule;
};

} } // namespace Cm.Parser

#endif // Json_hpp_12176
