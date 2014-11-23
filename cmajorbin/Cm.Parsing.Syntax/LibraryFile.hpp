#ifndef LibraryFile_hpp_10647
#define LibraryFile_hpp_10647

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

class LibraryFileGrammar : public Cm::Parsing::Grammar
{
public:
    static LibraryFileGrammar* Create();
    static LibraryFileGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    void Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, Cm::Parsing::ParsingDomain* parsingDomain);
private:
    LibraryFileGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class LibraryFileRule;
    class NamespaceContentRule;
    class NamespaceRule;
    class GrammarRule;
    class GrammarContentRule;
    class RuleRule;
};

} } } // namespace Cm.Parsing.Syntax

#endif // LibraryFile_hpp_10647
