#ifndef Declarator_hpp_20080
#define Declarator_hpp_20080

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parsing.CppObjectModel/Declaration.hpp>

namespace Cm { namespace Parsing { namespace Cpp {

class DeclaratorGrammar : public Cm::Parsing::Grammar
{
public:
    static DeclaratorGrammar* Create();
    static DeclaratorGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Parsing::CppObjectModel::InitDeclaratorList* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    DeclaratorGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class InitDeclaratorListRule;
    class InitDeclaratorRule;
    class DeclaratorRule;
    class DirectDeclaratorRule;
    class DeclaratorIdRule;
    class TypeIdRule;
    class TypeRule;
    class TypeSpecifierSeqRule;
    class AbstractDeclaratorRule;
    class DirectAbstractDeclaratorRule;
    class CVQualifierSeqRule;
    class InitializerRule;
    class InitializerClauseRule;
    class InitializerListRule;
};

} } } // namespace Cm.Parsing.Cpp

#endif // Declarator_hpp_20080
