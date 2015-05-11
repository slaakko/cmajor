namespace System.Xml
{
    grammar XmlGrammar
    {
        Document(ContentHandler* handler);
        XmlDecl;
        Element(ContentHandler* handler);
        EmptyElemTag: string;
        STag: string;
        ETag: string;
        S;
        Name: string;
        NameStartChar;
        NameChar;
        Content(ContentHandler* handler);
        CharData(ContentHandler* handler);
        Reference(ContentHandler* handler);
        EntityRef: string;
        CharRef: string;
    }
}
