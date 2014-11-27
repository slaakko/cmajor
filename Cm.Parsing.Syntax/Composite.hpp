#ifndef Composite_hpp_19440
#define Composite_hpp_19440

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parsing/Scope.hpp>
#include <Cm.Parsing/Parser.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

class CompositeGrammar : public Cm::Parsing::Grammar
{
public:
    static CompositeGrammar* Create();
    static CompositeGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Parsing::Parser* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, Cm::Parsing::Scope* enclosingScope);
private:
    CompositeGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class AlternativeRule;
    class SequenceRule;
    class DifferenceRule;
    class ExclusiveOrRule;
    class IntersectionRule;
    class ListRule;
    class PostfixRule;
};

} } } // namespace Cm.Parsing.Syntax

#endif // Composite_hpp_19440
