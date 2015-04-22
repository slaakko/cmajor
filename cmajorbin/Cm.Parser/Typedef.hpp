#ifndef Typedef_hpp_3809
#define Typedef_hpp_3809

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Ast/Node.hpp>
#include <Cm.Parser/ParsingContext.hpp>

namespace Cm { namespace Parser {

class TypedefGrammar : public Cm::Parsing::Grammar
{
public:
    static TypedefGrammar* Create();
    static TypedefGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::Node* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    TypedefGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class TypedefRule;
};

} } // namespace Cm.Parser

#endif // Typedef_hpp_3809
