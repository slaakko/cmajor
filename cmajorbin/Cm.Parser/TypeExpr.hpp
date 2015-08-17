#ifndef TypeExpr_hpp_6309
#define TypeExpr_hpp_6309

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Ast/TypeExpr.hpp>
#include <Cm.Parser/ParsingContext.hpp>

namespace Cm { namespace Parser {

class TypeExprGrammar : public Cm::Parsing::Grammar
{
public:
    static TypeExprGrammar* Create();
    static TypeExprGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::Node* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    TypeExprGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class TypeExprRule;
    class PrefixTypeExprRule;
    class PostfixTypeExprRule;
    class PrimaryTypeExprRule;
};

} } // namespace Cm.Parser

#endif // TypeExpr_hpp_6309
