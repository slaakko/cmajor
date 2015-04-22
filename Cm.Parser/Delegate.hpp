#ifndef Delegate_hpp_3809
#define Delegate_hpp_3809

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parser/ParsingContext.hpp>
#include <Cm.Ast/Node.hpp>

namespace Cm { namespace Parser {

class DelegateGrammar : public Cm::Parsing::Grammar
{
public:
    static DelegateGrammar* Create();
    static DelegateGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    Cm::Ast::Node* Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    DelegateGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class DelegateRule;
    class ClassDelegateRule;
};

} } // namespace Cm.Parser

#endif // Delegate_hpp_3809
