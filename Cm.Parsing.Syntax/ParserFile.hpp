#ifndef ParserFile_hpp_6331
#define ParserFile_hpp_6331

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parsing.Syntax/ParserFileContent.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

class ParserFileGrammar : public Cm::Parsing::Grammar
{
public:
    static ParserFileGrammar* Create();
    static ParserFileGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    ParserFileContent* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, int id_, Cm::Parsing::ParsingDomain* parsingDomain_);
private:
    ParserFileGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class ParserFileRule;
    class IncludeDirectivesRule;
    class IncludeDirectiveRule;
    class FileAttributeRule;
    class IncludeFileNameRule;
    class NamespaceContentRule;
    class NamespaceRule;
};

} } } // namespace Cm.Parsing.Syntax

#endif // ParserFile_hpp_6331
