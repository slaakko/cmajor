#ifndef Element_hpp_27572
#define Element_hpp_27572

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parsing/Scope.hpp>
#include <Cm.Parsing/Grammar.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

class ElementGrammar : public Cm::Parsing::Grammar
{
public:
    static ElementGrammar* Create();
    static ElementGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    void Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, Cm::Parsing::Grammar* grammar);
private:
    std::vector<std::string> keywords0;
    ElementGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class RuleLinkRule;
    class SignatureRule;
    class ParameterListRule;
    class VariableRule;
    class ParameterRule;
    class ReturnTypeRule;
    class IdentifierRule;
    class QualifiedIdRule;
    class StringArrayRule;
};

} } } // namespace Cm.Parsing.Syntax

#endif // Element_hpp_27572
