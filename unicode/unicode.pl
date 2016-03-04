namespace UnicodeBin
{
    grammar UnicodeDataGrammar
    {
        file(var CategoryMap categoryMap);
        characterData(CategoryMap* categoryMap);
        codepoint: uint;
        category: string;
        name: string;
        ignore;
    }
}
