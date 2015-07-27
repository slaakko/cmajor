namespace Cm.Debugger
{
    grammar BackTraceReplyGrammar
    {
        BackTraceReply: CallStack;
        Frame: Frame;
        SourceFileLine: Cm::Core::SourceFileLine;
        FilePath: std::string;
        Drive;
        Line: int;
    }
    grammar TypeExprGrammar
    {
        TypeExpr: TypeExpr*;
        TypeArguments: std::vector<TypeExpr*>;
        BasicType: TypeExpr*;
        TypeName: std::string;
    }
    grammar FrameReplyGrammar
    {
        Frame: int;
    }
    grammar InspectExprGrammar
    {
        InspectExpr: InspectNode*;
        ContentExpr: InspectNode*;
        PrefixExpr: InspectNode*;
        PostfixExpr(var std::unique_ptr<InspectNode> expr): InspectNode*;
        MemberId: std::string;
        PrimaryExpr: InspectNode*;
    }
    grammar ContinueReplyGrammar
    {
        ReplyLine(ContinueReplyData* data): ContinueReplyState;
        ContinuingLine;
        InferiorExit: int;
        octal: int;
        octaldigit: int;
        Breakpoint;
        Signal: std::string;
        Prompt;
        ConsoleLine: std::string;
    }
    grammar InterpreterGrammar
    {
        Command: CommandPtr;
        SourceFileLine(var std::string filePath): Cm::Core::SourceFileLine;
        FilePath: std::string;
        Drive;
        InspectExpr: std::string;
    }
    grammar ResultGrammar
    {
        Result(std::string expr): Result*;
        Value: Value*;
        PrimitiveValue: Value*;
        StringValue: Value*;
        ParenthesizedExpr;
        AddressValue: Value*;
        OctalDigit: int;
        OctalEscape: char;
        CharValue: Value*;
        BoolValue: Value*;
        FloatingValue: Value*;
        IntegerValue: Value*;
        ArrayValue: ArrayValue*;
        ArrayItem: Value*;
        RepeatedValue: ArrayValue*;
        Repeat;
        Field: Field*;
        StructureValue: StructureValue*;
    }
}
