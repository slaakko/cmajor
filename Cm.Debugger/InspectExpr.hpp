#ifndef InspectExpr_hpp_20739
#define InspectExpr_hpp_20739

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Debugger/InspectNode.hpp>

namespace Cm { namespace Debugger {

class InspectExprGrammar : public Cm::Parsing::Grammar
{
public:
    static InspectExprGrammar* Create();
    static InspectExprGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    InspectNode* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    InspectExprGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class InspectExprRule;
    class ContentExprRule;
    class PrefixExprRule;
    class PostfixExprRule;
    class MemberIdRule;
    class PrimaryExprRule;
};

} } // namespace Cm.Debugger

#endif // InspectExpr_hpp_20739
