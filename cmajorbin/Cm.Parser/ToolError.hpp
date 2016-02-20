#ifndef ToolError_hpp_1942
#define ToolError_hpp_1942

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Util/ToolError.hpp>

namespace Cm { namespace Parser {

class ToolErrorGrammar : public Cm::Parsing::Grammar
{
public:
    static ToolErrorGrammar* Create();
    static ToolErrorGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Util::ToolError Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    ToolErrorGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class ToolErrorRule;
    class ToolNameRule;
    class FilePathRule;
    class LineRule;
    class ColumnRule;
    class MessageRule;
};

} } // namespace Cm.Parser

#endif // ToolError_hpp_1942
