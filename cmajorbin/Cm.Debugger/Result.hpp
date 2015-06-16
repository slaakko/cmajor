#ifndef Result_hpp_4380
#define Result_hpp_4380

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Debugger/Value.hpp>

namespace Cm { namespace Debugger {

class ResultGrammar : public Cm::Parsing::Grammar
{
public:
    static ResultGrammar* Create();
    static ResultGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Result* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, std::string expr);
private:
    ResultGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class ResultRule;
    class ValueRule;
    class StringValueRule;
    class AddressValueRule;
    class OctalDigitRule;
    class OctalEscapeRule;
    class CharValueRule;
    class BoolValueRule;
    class FloatingValueRule;
    class IntegerValueRule;
    class FieldRule;
    class StructureValueRule;
};

} } // namespace Cm.Debugger

#endif // Result_hpp_4380
