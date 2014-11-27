#ifndef Statement_hpp_19440
#define Statement_hpp_19440

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parsing.CppObjectModel/Statement.hpp>

namespace Cm { namespace Parsing { namespace Cpp {

class StatementGrammar : public Cm::Parsing::Grammar
{
public:
    static StatementGrammar* Create();
    static StatementGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Parsing::CppObjectModel::CompoundStatement* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    StatementGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class StatementRule;
    class LabeledStatementRule;
    class LabelRule;
    class EmptyStatementRule;
    class ExpressionStatementRule;
    class CompoundStatementRule;
    class SelectionStatementRule;
    class IfStatementRule;
    class SwitchStatementRule;
    class IterationStatementRule;
    class WhileStatementRule;
    class DoStatementRule;
    class ForStatementRule;
    class ForInitStatementRule;
    class JumpStatementRule;
    class BreakStatementRule;
    class ContinueStatementRule;
    class ReturnStatementRule;
    class GotoStatementRule;
    class GotoTargetRule;
    class DeclarationStatementRule;
    class ConditionRule;
    class TryStatementRule;
    class HandlerSeqRule;
    class HandlerRule;
    class ExceptionDeclarationRule;
};

} } } // namespace Cm.Parsing.Cpp

#endif // Statement_hpp_19440
