#ifndef Expression_hpp_19440
#define Expression_hpp_19440

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parsing.CppObjectModel/Expression.hpp>

namespace Cm { namespace Parsing { namespace Cpp {

class ExpressionGrammar : public Cm::Parsing::Grammar
{
public:
    static ExpressionGrammar* Create();
    static ExpressionGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Parsing::CppObjectModel::CppObject* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    ExpressionGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class ExpressionRule;
    class ConstantExpressionRule;
    class AssignmentExpressionRule;
    class AssingmentOpRule;
    class ThrowExpressionRule;
    class ConditionalExpressionRule;
    class LogicalOrExpressionRule;
    class LogicalAndExpressionRule;
    class InclusiveOrExpressionRule;
    class ExclusiveOrExpressionRule;
    class AndExpressionRule;
    class EqualityExpressionRule;
    class EqOpRule;
    class RelationalExpressionRule;
    class RelOpRule;
    class ShiftExpressionRule;
    class ShiftOpRule;
    class AdditiveExpressionRule;
    class AddOpRule;
    class MultiplicativeExpressionRule;
    class MulOpRule;
    class PmExpressionRule;
    class PmOpRule;
    class CastExpressionRule;
    class UnaryExpressionRule;
    class UnaryOperatorRule;
    class NewExpressionRule;
    class NewTypeIdRule;
    class NewDeclaratorRule;
    class DirectNewDeclaratorRule;
    class NewPlacementRule;
    class NewInitializerRule;
    class DeleteExpressionRule;
    class PostfixExpressionRule;
    class TypeSpecifierOrTypeNameRule;
    class PostCastExpressionRule;
    class ExpressionListRule;
    class PrimaryExpressionRule;
    class IdExpressionRule;
};

} } } // namespace Cm.Parsing.Cpp

#endif // Expression_hpp_19440
