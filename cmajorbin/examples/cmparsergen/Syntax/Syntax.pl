namespace Syntax
{
    grammar CompositeGrammar
    {
        Alternative(ScopePtr enclosingScope, ParsingContextPtr ctx): ParserPtr;
        Sequence(ScopePtr enclosingScope, ParsingContextPtr ctx): ParserPtr;
        Difference(ScopePtr enclosingScope, ParsingContextPtr ctx): ParserPtr;
        ExclusiveOr(ScopePtr enclosingScope, ParsingContextPtr ctx): ParserPtr;
        Intersection(ScopePtr enclosingScope, ParsingContextPtr ctx): ParserPtr;
        List(ScopePtr enclosingScope, ParsingContextPtr ctx): ParserPtr;
        Postfix(ScopePtr enclosingScope, ParsingContextPtr ctx): ParserPtr;
    }
    grammar ElementGrammar
    {
        RuleLink(GrammarPtr grammar);
        Signature(RulePtr rule, ParsingContextPtr ctx);
        ParameterList(RulePtr rule, ParsingContextPtr ctx);
        Variable(RulePtr rule, ParsingContextPtr ctx);
        Parameter(RulePtr rule, ParsingContextPtr ctx);
        ReturnType(RulePtr rule, ParsingContextPtr ctx);
        Keyword;
        Identifier: string;
        QualifiedId: string;
        StringArray(List<string>* array);
    }
    grammar GrammarGrammar
    {
        Grammar(ScopePtr enclosingScope, ParsingContextPtr ctx): GrammarPtr;
        GrammarContent(GrammarPtr grammar, ParsingContextPtr ctx);
        StartClause(GrammarPtr grammar);
        SkipClause(GrammarPtr grammar);
    }
    grammar LibraryFileGrammar
    {
        LibraryFile(ParsingDomain* parsingDomain, var ParsingContextPtr ctx);
        NamespaceContent(ParsingDomain* parsingDomain, ParsingContextPtr ctx);
        Namespace(ParsingDomain* parsingDomain, ParsingContextPtr ctx);
        Grammar(ScopePtr enclosingScope, ParsingContextPtr ctx): GrammarPtr;
        GrammarContent(GrammarPtr grammar, ParsingContextPtr ctx);
        Rule(ScopePtr enclosingScope, ParsingContextPtr ctx): RulePtr;
    }
    grammar ParserFileGrammar
    {
        ParserFile(int id_, ParsingDomain* parsingDomain_, var ParsingContextPtr ctx): ParserFileContent*;
        UsingDeclarations(ParserFileContent* parserFileContent);
        UsingDeclaration(ParserFileContent* parserFileContent);
        NamespaceContent(ParserFileContent* parserFileContent, ParsingContextPtr ctx);
        Namespace(ParserFileContent* parserFileContent, ParsingContextPtr ctx);
    }
    grammar PrimaryGrammar
    {
        Primary(ScopePtr enclosingScope, ParsingContextPtr ctx): ParserPtr;
        RuleCall(ParsingContextPtr ctx): ParserPtr;
        Nonterminal: string;
        Alias: string;
        Grouping(ScopePtr enclosingScope, ParsingContextPtr ctx): ParserPtr;
        Token(ScopePtr enclosingScope, ParsingContextPtr ctx): ParserPtr;
        Expectation(ParserPtr child): ParserPtr;
        Action(ParserPtr child, ParsingContextPtr ctx): ParserPtr;
    }
    grammar PrimitiveGrammar
    {
        Primitive: ParserPtr;
        Char: ParserPtr;
        String: ParserPtr;
        CharSet(var bool inverse, var string s): ParserPtr;
        CharSetRange: string;
        CharSetChar: char;
        Keyword: ParserPtr;
        KeywordBody: ParserPtr;
        KeywordList: ParserPtr;
        KeywordListBody(var List<string> keywords): ParserPtr;
        Empty: ParserPtr;
        Space: ParserPtr;
        AnyChar: ParserPtr;
        Letter: ParserPtr;
        Digit: ParserPtr;
        HexDigit: ParserPtr;
        Punctuation: ParserPtr;
    }
    grammar ProjectFileGrammar
    {
        ProjectFile: Project*;
        ProjectFileContent(Project* project);
        Source: string;
        Reference: string;
        FilePath: string;
    }
    grammar RuleGrammar
    {
        Rule(ScopePtr enclosingScope, ParsingContextPtr ctx): RulePtr;
        RuleHeader(ScopePtr enclosingScope, ParsingContextPtr ctx): RulePtr;
        RuleBody(RulePtr rule, ParsingContextPtr ctx);
    }
}
