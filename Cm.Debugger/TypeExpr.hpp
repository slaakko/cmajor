#ifndef TypeExpr_hpp_9767
#define TypeExpr_hpp_9767

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Debugger/Type.hpp>

namespace Cm { namespace Debugger {

class TypeExprGrammar : public Cm::Parsing::Grammar
{
public:
    static TypeExprGrammar* Create();
    static TypeExprGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    TypeExpr* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    std::vector<std::string> keywords0;
    TypeExprGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class TypeExprRule;
    class TypeArgumentsRule;
    class BasicTypeRule;
    class TypeNameRule;
};

} } // namespace Cm.Debugger

#endif // TypeExpr_hpp_9767
