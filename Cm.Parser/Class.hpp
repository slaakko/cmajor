#ifndef Class_hpp_24091
#define Class_hpp_24091

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parser/ParsingContext.hpp>
#include <Cm.Ast/Class.hpp>
#include <Cm.Ast/CompileUnit.hpp>

namespace Cm { namespace Parser {

class ClassGrammar : public Cm::Parsing::Grammar
{
public:
    static ClassGrammar* Create();
    static ClassGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::ClassNode* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx, Cm::Ast::CompileUnitNode* compileUnit);
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

#endif // Class_hpp_24091
