namespace System.Text.RegularExpressions
{
    grammar RegularExpressionGrammar
    {
        RegularExpression: PtrNfa;
        AlternativeExpression: PtrNfa;
        SequenceExpression: PtrNfa;
        PostfixExpression: PtrNfa;
        PrimaryExpression: PtrNfa;
        Char: char;
        CharClass(var bool inverse, var string s): System.Text.RegularExpressions.CharClass;
        Class: Class;
        CharRange: string;
        CharClassChar: char;
    }
}
