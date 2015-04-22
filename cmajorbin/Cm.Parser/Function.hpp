#ifndef Function_hpp_3809
#define Function_hpp_3809

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parser/ParsingContext.hpp>
#include <Cm.Ast/Function.hpp>

namespace Cm { namespace Parser {

class FunctionGrammar : public Cm::Parsing::Grammar
{
public:
    static FunctionGrammar* Create();
    static FunctionGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::FunctionNode* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    FunctionGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class FunctionRule;
    class FunctionGroupIdRule;
    class OperatorFunctionGroupIdRule;
};

} } // namespace Cm.Parser

#endif // Function_hpp_3809
