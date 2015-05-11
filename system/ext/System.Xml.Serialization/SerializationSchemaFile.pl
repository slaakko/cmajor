namespace System.Xml.Serialization
{
    grammar SerializationSchemaFileGrammar
    {
        SchemaFile: SerializationSchemaFile*;
        Structs(SerializationSchemaFile* file);
        Struct: Struct*;
        Fields(Struct* structure);
        Field: Serializable*;
        ArrayField: Array*;
        ItemType: Serializable*;
        IntField: Serializable*;
        DoubleField: Serializable*;
        BoolField: Serializable*;
        StringField: Serializable*;
        StructField: Serializable*;
        Keyword;
        Identifier: string;
        QualifiedId: string;
    }
}
