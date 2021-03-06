namespace Syntax
{
    grammar CompositeGrammar
    {
        Alternative(Scope* enclosingScope, ParsingContext* ctx): Parser*;
        Sequence(Scope* enclosingScope, ParsingContext* ctx): Parser*;
        Difference(Scope* enclosingScope, ParsingContext* ctx): Parser*;
        ExclusiveOr(Scope* enclosingScope, ParsingContext* ctx): Parser*;
        Intersection(Scope* enclosingScope, ParsingContext* ctx): Parser*;
        List(Scope* enclosingScope, ParsingContext* ctx): Parser*;
        Postfix(Scope* enclosingScope, ParsingContext* ctx): Parser*;
    }
    grammar ElementGrammar
    {
        RuleLink(Grammar* grammar);
        Signature(Rule* rule, ParsingContext* ctx);
        ParameterList(Rule* rule, ParsingContext* ctx);
        Variable(Rule* rule, ParsingContext* ctx);
        Parameter(Rule* rule, ParsingContext* ctx);
        ReturnType(Rule* rule, ParsingContext* ctx);
        Keyword;
        Identifier: string;
        QualifiedId: string;
        StringArray(List<string>* array);
    }
    grammar GrammarGrammar
    {
        Grammar(Scope* enclosingScope, ParsingContext* ctx): Grammar*;
        GrammarContent(Grammar* grammar, ParsingContext* ctx);
        StartClause(Grammar* grammar);
        SkipClause(Grammar* grammar);
    }
    grammar LibraryFileGrammar
    {
        LibraryFile(ParsingDomain* parsingDomain, var ParsingContext* ctx);
        NamespaceContent(ParsingDomain* parsingDomain, ParsingContext* ctx);
        Namespace(ParsingDomain* parsingDomain, ParsingContext* ctx);
        Grammar(Scope* enclosingScope, ParsingContext* ctx): Grammar*;
        GrammarContent(Grammar* grammar, ParsingContext* ctx);
        Rule(Scope* enclosingScope, ParsingContext* ctx): Rule*;
    }
    grammar ParserFileGrammar
    {
        ParserFile(int id_, ParsingDomain* parsingDomain_, var ParsingContext* ctx): ParserFileContent*;
        UsingDeclarations(ParserFileContent* parserFileContent);
        UsingDeclaration(ParserFileContent* parserFileContent);
        NamespaceContent(ParserFileContent* parserFileContent, ParsingContext* ctx);
        Namespace(ParserFileContent* parserFileContent, ParsingContext* ctx);
    }
    grammar PrimaryGrammar
    {
        Primary(Scope* enclosingScope, ParsingContext* ctx): Parser*;
        RuleCall(ParsingContext* ctx): Parser*;
        Nonterminal: string;
        Alias: string;
        Grouping(Scope* enclosingScope, ParsingContext* ctx): Parser*;
        Token(Scope* enclosingScope, ParsingContext* ctx): Parser*;
        Expectation(Parser* child): Parser*;
        Action(Parser* child, ParsingContext* ctx): Parser*;
    }
    grammar PrimitiveGrammar
    {
        Primitive: Parser*;
        Char: Parser*;
        String: Parser*;
        CharSet(var bool inverse, var string s): Parser*;
        CharSetRange: string;
        CharSetChar: char;
        Keyword: Parser*;
        KeywordBody: Parser*;
        KeywordList: Parser*;
        KeywordListBody(var List<string> keywords): Parser*;
        Empty: Parser*;
        Space: Parser*;
        AnyChar: Parser*;
        Letter: Parser*;
        Digit: Parser*;
        HexDigit: Parser*;
        Punctuation: Parser*;
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
        Rule(Scope* enclosingScope, ParsingContext* ctx): Rule*;
        RuleHeader(Scope* enclosingScope, ParsingContext* ctx): Rule*;
        RuleBody(Rule* rule, ParsingContext* ctx);
    }
}
