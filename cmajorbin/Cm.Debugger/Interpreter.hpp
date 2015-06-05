#ifndef Interpreter_hpp_25326
#define Interpreter_hpp_25326

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Debugger/Command.hpp>

namespace Cm { namespace Debugger {

class InterpreterGrammar : public Cm::Parsing::Grammar
{
public:
    static InterpreterGrammar* Create();
    static InterpreterGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    CommandPtr Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    InterpreterGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class CommandRule;
    class SourceFileLineRule;
    class FilePathRule;
    class InspectExprRule;
};

} } // namespace Cm.Debugger

#endif // Interpreter_hpp_25326
