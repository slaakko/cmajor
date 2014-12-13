#ifndef Class_hpp_20447
#define Class_hpp_20447

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parser/ParsingContext.hpp>
#include <Cm.Ast/Class.hpp>

namespace Cm { namespace Parser {

class ClassGrammar : public Cm::Parsing::Grammar
{
public:
    static ClassGrammar* Create();
    static ClassGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::ClassNode* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    ClassGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class ClassRule;
    class InheritanceRule;
    class BaseClassTypeExprRule;
    class ClassContentRule;
    class ClassMemberRule;
    class InitializerListRule;
    class InitializerRule;
    class MemberInitializerRule;
    class BaseInitializerRule;
    class ThisInitializerRule;
    class StaticConstructorRule;
    class ConstructorRule;
    class DestructorRule;
    class MemberFunctionRule;
    class ConversionFunctionRule;
    class MemberVariableRule;
};

} } // namespace Cm.Parser

#endif // Class_hpp_20447
