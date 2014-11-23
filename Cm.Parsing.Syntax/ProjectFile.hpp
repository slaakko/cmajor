#ifndef ProjectFile_hpp_15252
#define ProjectFile_hpp_15252

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parsing.Syntax/Project.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

class ProjectFileGrammar : public Cm::Parsing::Grammar
{
public:
    static ProjectFileGrammar* Create();
    static ProjectFileGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Project* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    ProjectFileGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class ProjectFileRule;
    class ProjectFileContentRule;
    class SourceRule;
    class ReferenceRule;
    class FilePathRule;
};

} } } // namespace Cm.Parsing.Syntax

#endif // ProjectFile_hpp_15252
