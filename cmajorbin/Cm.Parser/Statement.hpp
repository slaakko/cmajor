#ifndef Statement_hpp_21497
#define Statement_hpp_21497

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parser/ParsingContext.hpp>
#include <Cm.Ast/Statement.hpp>

namespace Cm { namespace Parser {

class StatementGrammar : public Cm::Parsing::Grammar
{
public:
    static StatementGrammar* Create();
    static StatementGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::CompoundStatementNode* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    StatementGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class StatementRule;
    class LabelIdRule;
    class LabelRule;
    class LabeledStatementRule;
    class SimpleStatementRule;
    class ControlStatementRule;
    class ReturnStatementRule;
    class ConditionalStatementRule;
    class SwitchStatementRule;
    class CaseStatementRule;
    class DefaultStatementRule;
    class CaseListRule;
    class GotoCaseStatementRule;
    class GotoDefaultStatementRule;
    class WhileStatementRule;
    class DoStatementRule;
    class RangeForStatementRule;
    class ForStatementRule;
    class ForInitStatementRule;
    class CompoundStatementRule;
    class BreakStatementRule;
    class ContinueStatementRule;
    class GotoStatementRule;
    class TypedefStatementRule;
    class AssignmentStatementRule;
    class ConstructionStatementRule;
    class DeleteStatementRule;
    class DestroyStatementRule;
    class ThrowStatementRule;
    class TryStatementRule;
    class HandlersRule;
    class HandlerRule;
    class AssertStatementRule;
    class ConditionalCompilationStatementRule;
    class ConditionalCompilationExprRule;
    class ConditionalCompilationDisjunctionRule;
    class ConditionalCompilationConjunctionRule;
    class ConditionalCompilationPrefixRule;
    class ConditionalCompilationPrimaryRule;
    class ConditionalCompilationSymbolRule;
    class SymbolRule;
};

} } // namespace Cm.Parser

#endif // Statement_hpp_21497
