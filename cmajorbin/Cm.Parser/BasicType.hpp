#ifndef BasicType_hpp_3809
#define BasicType_hpp_3809

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Ast/BasicType.hpp>

namespace Cm { namespace Parser {

class BasicTypeGrammar : public Cm::Parsing::Grammar
{
public:
    static BasicTypeGrammar* Create();
    static BasicTypeGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::Node* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    BasicTypeGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class BasicTypeRule;
};

} } // namespace Cm.Parser

#endif // BasicType_hpp_3809
