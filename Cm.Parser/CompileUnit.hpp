#ifndef CompileUnit_hpp_8809
#define CompileUnit_hpp_8809

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parser/ParsingContext.hpp>
#include <Cm.Ast/CompileUnit.hpp>

namespace Cm { namespace Parser {

class CompileUnitGrammar : public Cm::Parsing::Grammar
{
public:
    static CompileUnitGrammar* Create();
    static CompileUnitGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::CompileUnitNode* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    CompileUnitGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class CompileUnitRule;
    class NamespaceContentRule;
    class UsingDirectivesRule;
    class UsingDirectiveRule;
    class UsingAliasDirectiveRule;
    class UsingNamespaceDirectiveRule;
    class DefinitionsRule;
    class DefinitionRule;
    class NamespaceDefinitionRule;
    class FunctionDefinitionRule;
    class ConstantDefinitionRule;
    class EnumerationDefinitionRule;
    class TypedefDefinitionRule;
    class ClassDefinitionRule;
    class DelegateDefinitionRule;
    class ClassDelegateDefinitionRule;
    class ConceptDefinitionRule;
};

} } // namespace Cm.Parser

#endif // CompileUnit_hpp_8809
