#ifndef Interface_hpp_26941
#define Interface_hpp_26941

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parser/ParsingContext.hpp>
#include <Cm.Ast/Interface.hpp>
#include <Cm.Ast/CompileUnit.hpp>
#include <Cm.Ast/Concept.hpp>
#include <Cm.Ast/Statement.hpp>

namespace Cm { namespace Parser {

class InterfaceGrammar : public Cm::Parsing::Grammar
{
public:
    static InterfaceGrammar* Create();
    static InterfaceGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::InterfaceNode* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx, Cm::Ast::CompileUnitNode* compileUnit);
private:
    InterfaceGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class InterfaceRule;
    class InterfaceContentRule;
    class InterfaceMemFunRule;
    class InterfaceMemFunSpecifiersRule;
    class InterfaceFunctionGroupIdRule;
};

} } // namespace Cm.Parser

#endif // Interface_hpp_26941
