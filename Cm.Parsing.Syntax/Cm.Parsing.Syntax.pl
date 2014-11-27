namespace Cm.Parsing.Syntax
{
    grammar CompositeGrammar
    {
        Alternative(Cm::Parsing::Scope* enclosingScope): Cm::Parsing::Parser*;
        Sequence(Cm::Parsing::Scope* enclosingScope): Cm::Parsing::Parser*;
        Difference(Cm::Parsing::Scope* enclosingScope): Cm::Parsing::Parser*;
        ExclusiveOr(Cm::Parsing::Scope* enclosingScope): Cm::Parsing::Parser*;
        Intersection(Cm::Parsing::Scope* enclosingScope): Cm::Parsing::Parser*;
        List(Cm::Parsing::Scope* enclosingScope): Cm::Parsing::Parser*;
        Postfix(Cm::Parsing::Scope* enclosingScope): Cm::Parsing::Parser*;
    }
    grammar ElementGrammar
    {
        RuleLink(Cm::Parsing::Grammar* grammar);
        Signature(Cm::Parsing::Rule* rule);
        ParameterList(Cm::Parsing::Rule* rule);
        Variable(Cm::Parsing::Rule* rule);
        Parameter(Cm::Parsing::Rule* rule);
        ReturnType(Cm::Parsing::Rule* rule);
        Keyword;
        Identifier: std::string;
        QualifiedId: std::string;
        StringArray(std::vector<std::string>* array);
    }
    grammar ParserFileGrammar
    {
        ParserFile(int id_, Cm::Parsing::ParsingDomain* parsingDomain_): ParserFileContent*;
        IncludeDirectives(ParserFileContent* parserFileContent);
        IncludeDirective(ParserFileContent* parserFileContent);
        FileAttribute: std::string;
        IncludeFileName: std::string;
        NamespaceContent(ParserFileContent* parserFileContent);
        Namespace(ParserFileContent* parserFileContent);
    }
    grammar LibraryFileGrammar
    {
        LibraryFile(Cm::Parsing::ParsingDomain* parsingDomain);
        NamespaceContent(Cm::Parsing::ParsingDomain* parsingDomain);
        Namespace(Cm::Parsing::ParsingDomain* parsingDomain);
        Grammar(Cm::Parsing::Scope* enclosingScope): Cm::Parsing::Grammar*;
        GrammarContent(Cm::Parsing::Grammar* grammar);
        Rule(Cm::Parsing::Scope* enclosingScope): Cm::Parsing::Rule*;
    }
    grammar GrammarGrammar
    {
        Grammar(Cm::Parsing::Scope* enclosingScope): Cm::Parsing::Grammar*;
        GrammarContent(Cm::Parsing::Grammar* grammar);
        StartClause(Cm::Parsing::Grammar* grammar);
        SkipClause(Cm::Parsing::Grammar* grammar);
        RecoverClause(Cm::Parsing::Grammar* grammar);
    }
    grammar RuleGrammar
    {
        Rule(Cm::Parsing::Scope* enclosingScope): Cm::Parsing::Rule*;
        RuleHeader(Cm::Parsing::Scope* enclosingScope): Cm::Parsing::Rule*;
        SynchronizeSpecification(Cm::Parsing::Rule* rule);
        RuleBody(Cm::Parsing::Rule* rule);
    }
    grammar PrimaryGrammar
    {
        Primary(Cm::Parsing::Scope* enclosingScope): Cm::Parsing::Parser*;
        RuleCall: Cm::Parsing::Parser*;
        Nonterminal: std::string;
        Alias: std::string;
        Grouping(Cm::Parsing::Scope* enclosingScope): Cm::Parsing::Parser*;
        Token(Cm::Parsing::Scope* enclosingScope): Cm::Parsing::Parser*;
        Expectation(Cm::Parsing::Parser* child): Cm::Parsing::Parser*;
        Action(Cm::Parsing::Parser* child): Cm::Parsing::Parser*;
    }
    grammar PrimitiveGrammar
    {
        Primitive: Cm::Parsing::Parser*;
        Char: Cm::Parsing::Parser*;
        String: Cm::Parsing::Parser*;
        CharSet(var bool inverse, var std::string s): Cm::Parsing::Parser*;
        CharSetRange: std::string;
        CharSetChar: char;
        Keyword: Cm::Parsing::Parser*;
        KeywordBody: Cm::Parsing::Parser*;
        KeywordList: Cm::Parsing::Parser*;
        KeywordListBody(var std::vector<std::string> keywords): Cm::Parsing::Parser*;
        Empty: Cm::Parsing::Parser*;
        Space: Cm::Parsing::Parser*;
        AnyChar: Cm::Parsing::Parser*;
        Letter: Cm::Parsing::Parser*;
        Digit: Cm::Parsing::Parser*;
        HexDigit: Cm::Parsing::Parser*;
        Punctuation: Cm::Parsing::Parser*;
    }
    grammar ProjectFileGrammar
    {
        ProjectFile: Project*;
        ProjectFileContent(Project* project);
        Source: std::string;
        Reference: std::string;
        FilePath: std::string;
    }
}
