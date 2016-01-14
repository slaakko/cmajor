#ifndef LlvmVersion_hpp_11696
#define LlvmVersion_hpp_11696

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Ast/Project.hpp>

namespace Cm { namespace Parser {

class LlvmVersionParser : public Cm::Parsing::Grammar
{
public:
    static LlvmVersionParser* Create();
    static LlvmVersionParser* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::ProgramVersion Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    LlvmVersionParser(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class LlvmVersionRule;
    class MajorRule;
    class MinorRule;
    class RevisionRule;
    class BuildRule;
};

} } // namespace Cm.Parser

#endif // LlvmVersion_hpp_11696
