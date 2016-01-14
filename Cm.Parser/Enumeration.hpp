#ifndef Enumeration_hpp_11627
#define Enumeration_hpp_11627

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parser/ParsingContext.hpp>
#include <Cm.Ast/Enumeration.hpp>

namespace Cm { namespace Parser {

class EnumerationGrammar : public Cm::Parsing::Grammar
{
public:
    static EnumerationGrammar* Create();
    static EnumerationGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::EnumTypeNode* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    EnumerationGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class EnumTypeRule;
    class UnderlyingTypeRule;
    class EnumConstantsRule;
    class EnumConstantRule;
};

} } // namespace Cm.Parser

#endif // Enumeration_hpp_11627
