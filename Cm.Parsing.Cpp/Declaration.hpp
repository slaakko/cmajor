#ifndef Declaration_hpp_19440
#define Declaration_hpp_19440

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parsing.CppObjectModel/Declaration.hpp>

namespace Cm { namespace Parsing { namespace Cpp {

class DeclarationGrammar : public Cm::Parsing::Grammar
{
public:
    static DeclarationGrammar* Create();
    static DeclarationGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Parsing::CppObjectModel::CppObject* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    std::vector<std::string> keywords0;
    std::vector<std::string> keywords1;
    DeclarationGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class BlockDeclarationRule;
    class SimpleDeclarationRule;
    class DeclSpecifierSeqRule;
    class DeclSpecifierRule;
    class StorageClassSpecifierRule;
    class TypeSpecifierRule;
    class SimpleTypeSpecifierRule;
    class TypeNameRule;
    class TemplateArgumentListRule;
    class TemplateArgumentRule;
    class TypedefRule;
    class CVQualifierRule;
    class NamespaceAliasDefinitionRule;
    class UsingDeclarationRule;
    class UsingDirectiveRule;
};

} } } // namespace Cm.Parsing.Cpp

#endif // Declaration_hpp_19440
