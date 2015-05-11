namespace System.Net.Http
{
    grammar HttpResponseGrammar
    {
        HttpResponse(var List<HeaderLine> headerLines): HttpResponse;
        StatusLine(var string statusText): HttpStatus;
        HttpVersion: string;
        HeaderLine(var string name, var string val): HeaderLine;
        Content: string;
    }
    grammar TokenSep
    {
        Separators;
    }
}
