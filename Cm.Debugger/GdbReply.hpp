#ifndef GdbReply_hpp_6309
#define GdbReply_hpp_6309

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Debugger/Util.hpp>
#include <Cm.Debugger/Gdb.hpp>

namespace Cm { namespace Debugger {

class BackTraceReplyGrammar : public Cm::Parsing::Grammar
{
public:
    static BackTraceReplyGrammar* Create();
    static BackTraceReplyGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    CallStack Parse(const char* start, const char* end, int fileIndex, const std::string& fileName);
private:
    BackTraceReplyGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class BackTraceReplyRule;
    class FrameRule;
    class SourceFileLineRule;
    class FilePathRule;
    class LineRule;
};

class ContinueReplyGrammar : public Cm::Parsing::Grammar
{
public:
    static ContinueReplyGrammar* Create();
    static ContinueReplyGrammar* Create(Cm::Parsing::ParsingDomain* parsingDomain);
    ContinueReplyState Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ContinueReplyData* data);
private:
    ContinueReplyGrammar(Cm::Parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class ReplyLineRule;
    class InferiorExitRule;
    class octalRule;
    class octaldigitRule;
    class ConsoleLineRule;
};

} } // namespace Cm.Debugger

#endif // GdbReply_hpp_6309
