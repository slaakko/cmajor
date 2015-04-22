#ifndef Project_hpp_3809
#define Project_hpp_3809

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Ast/Project.hpp>

namespace Cm { namespace Parser {

class ProjectGrammar : public Cm::Parsing::Grammar
{
public:
    static ProjectGrammar* Create();
    static ProjectGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::Project* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, std::string config, std::string backend, std::string os);
private:
    ProjectGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class ProjectRule;
    class DeclarationsRule;
    class DeclarationRule;
    class SourceFileDeclarationRule;
    class AsmSourceFileDeclarationRule;
    class CSourceFileDeclarationRule;
    class TextFileDeclarationRule;
    class ReferenceFileDeclarationRule;
    class AssemblyFileDeclarationRule;
    class ExecutableFileDeclarationRule;
    class CLibraryDeclarationRule;
    class TargetDeclarationRule;
    class PropertiesRule;
    class FilePathRule;
};

} } // namespace Cm.Parser

#endif // Project_hpp_3809
