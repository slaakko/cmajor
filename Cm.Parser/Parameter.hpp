#ifndef Parameter_hpp_11627
#define Parameter_hpp_11627

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parser/ParsingContext.hpp>
#include <Cm.Ast/Parameter.hpp>

namespace Cm { namespace Parser {

class ParameterGrammar : public Cm::Parsing::Grammar
{
public:
    static ParameterGrammar* Create();
    static ParameterGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    void Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx, Cm::Ast::Node* owner);
private:
    ParameterGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class ParameterListRule;
    class ParameterRule;
};

} } // namespace Cm.Parser

#endif // Parameter_hpp_11627
