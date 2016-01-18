#ifndef Project_hpp_6577
#define Project_hpp_6577

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Ast/Project.hpp>

namespace Cm { namespace Parser {

class ProjectGrammar : public Cm::Parsing::Grammar
{
public:
    static ProjectGrammar* Create();
    static ProjectGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::Project* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, std::string config, std::string backend, std::string os, int bits, Cm::Ast::ProgramVersion llvmVersion);
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
    class CppSourceFileDeclarationRule;
    class TextFileDeclarationRule;
    class ReferenceFileDeclarationRule;
    class AssemblyFileDeclarationRule;
    class ExecutableFileDeclarationRule;
    class AddLibraryPathDeclarationRule;
    class CLibraryDeclarationRule;
    class TargetDeclarationRule;
    class StackSizeDeclarationRule;
    class PropertiesRule;
    class RelOpRule;
    class PropertyValueRule;
    class FilePathRule;
};

} } // namespace Cm.Parser

#endif // Project_hpp_6577
