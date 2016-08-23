#ifndef Constant_hpp_20065
#define Constant_hpp_20065

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parser/ParsingContext.hpp>
#include <Cm.Ast/Node.hpp>

namespace Cm { namespace Parser {

class ConstantGrammar : public Cm::Parsing::Grammar
{
public:
    static ConstantGrammar* Create();
    static ConstantGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::Node* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    ConstantGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class ConstantRule;
};

} } // namespace Cm.Parser

#endif // Constant_hpp_20065
