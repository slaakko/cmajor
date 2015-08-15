#ifndef Identifier_hpp_24386
#define Identifier_hpp_24386

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Parser {

class IdentifierGrammar : public Cm::Parsing::Grammar
{
public:
    static IdentifierGrammar* Create();
    static IdentifierGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::IdentifierNode* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    IdentifierGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class IdentifierRule;
    class QualifiedIdRule;
};

} } // namespace Cm.Parser

#endif // Identifier_hpp_24386
