namespace Cm.Debugger
{
    grammar InterpreterGrammar
    {
        Command: CommandPtr;
        SourceFileLine(var std::string filePath): Cm::Core::SourceFileLine;
        FilePath: std::string;
        Drive;
        InspectExpr: std::string;
    }
    grammar BackTraceReplyGrammar
    {
        BackTraceReply: CallStack;
        Frame: Frame;
        SourceFileLine: Cm::Core::SourceFileLine;
        FilePath: std::string;
        Drive;
        Line: int;
    }
    grammar FrameReplyGrammar
    {
        Frame: int;
    }
    grammar ContinueReplyGrammar
    {
        ReplyLine(ContinueReplyData* data): ContinueReplyState;
        ContinuingLine;
        InferiorExit: int;
        octal: int;
        octaldigit: int;
        Breakpoint;
        Signal;
        Prompt;
        ConsoleLine: std::string;
    }
}
