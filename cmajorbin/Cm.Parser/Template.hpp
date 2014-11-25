#ifndef Template_hpp_3610
#define Template_hpp_3610

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Ast/Node.hpp>
#include <Cm.Parser/ParsingContext.hpp>

namespace Cm { namespace Parser {

class TemplateGrammar : public Cm::Parsing::Grammar
{
public:
    static TemplateGrammar* Create();
    static TemplateGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::Node* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    TemplateGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class TemplateIdRule;
};

} } // namespace Cm.Parser

#endif // Template_hpp_3610
