#ifndef Expression_hpp_92
#define Expression_hpp_92

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parser/ParsingContext.hpp>
#include <Cm.Ast/Node.hpp>

namespace Cm { namespace Parser {

class ExpressionGrammar : public Cm::Parsing::Grammar
{
public:
    static ExpressionGrammar* Create();
    static ExpressionGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::Node* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    ExpressionGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class ExpressionRule;
    class EquivalenceRule;
    class ImplicationRule;
    class DisjunctionRule;
    class ConjunctionRule;
    class BitOrRule;
    class BitXorRule;
    class BitAndRule;
    class EqualityRule;
    class RelationalRule;
    class ShiftRule;
    class AdditiveRule;
    class MultiplicativeRule;
    class PrefixRule;
    class PostfixRule;
    class PrimaryRule;
    class SizeOfExprRule;
    class CastExprRule;
    class ConstructExprRule;
    class NewExprRule;
    class ArgumentListRule;
    class ExpressionListRule;
    class InvokeExprRule;
};

} } // namespace Cm.Parser

#endif // Expression_hpp_92
