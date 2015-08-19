#ifndef Concept_hpp_27291
#define Concept_hpp_27291

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parser/ParsingContext.hpp>
#include <Cm.Ast/Concept.hpp>

namespace Cm { namespace Parser {

class ConceptGrammar : public Cm::Parsing::Grammar
{
public:
    static ConceptGrammar* Create();
    static ConceptGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::ConceptNode* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    ConceptGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class ConceptRule;
    class RefinementRule;
    class ConceptBodyRule;
    class ConceptBodyConstraintRule;
    class AxiomRule;
    class AxiomBodyRule;
    class AxiomStatementRule;
    class TypenameConstraintRule;
    class SignatureConstraintRule;
    class ConstructorConstraintRule;
    class DestructorConstraintRule;
    class MemberFunctionConstraintRule;
    class FunctionConstraintRule;
    class EmbeddedConstraintRule;
    class WhereConstraintRule;
    class ConstraintExprRule;
    class DisjunctiveConstraintExprRule;
    class ConjunctiveConstraintExprRule;
    class PrimaryConstraintExprRule;
    class AtomicConstraintExprRule;
    class IsConstraintRule;
    class ConceptOrTypeNameRule;
    class MultiParamConstraintRule;
};

} } // namespace Cm.Parser

#endif // Concept_hpp_27291
