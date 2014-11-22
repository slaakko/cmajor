#ifndef StdLib_hpp_14206
#define StdLib_hpp_14206

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <stdint.h>

namespace Soul { namespace Parsing {

class stdlib : public Cm::Parsing::Grammar
{
public:
    static stdlib* Create();
    static stdlib* Create(Cm::Parsing::ParsingDomain* parsingDomain);
private:
    stdlib(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class intRule;
    class uintRule;
    class longRule;
    class ulongRule;
    class hexRule;
    class hex_literalRule;
    class realRule;
    class urealRule;
    class numberRule;
    class boolRule;
    class identifierRule;
    class qualified_idRule;
    class escapeRule;
    class charRule;
    class stringRule;
};

} } // namespace Soul.Parsing

#endif // StdLib_hpp_14206
