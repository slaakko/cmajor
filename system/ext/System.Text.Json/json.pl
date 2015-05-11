namespace System.Text.Json.Data
{
    grammar JsonGrammar
    {
        Value: JsonValue*;
        Object: JsonObject*;
        Array: JsonArray*;
        String: JsonString*;
        Number: JsonNumber*;
    }
}
namespace System.Text.Json.Schema
{
    grammar SchemaFileGrammar
    {
        SchemaFile: SchemaFileContent*;
        JsonContainer(SchemaFileContent* content);
        Sequence(SchemaFileContent* content, var Sequence* sequence);
        Struct(SchemaFileContent* content, var Struct* structure);
        Field(SchemaFileContent* content): Field*;
        StringField: Field*;
        NumberField: Field*;
        BooleanField: Field*;
        ContainerField(SchemaFileContent* content): Field*;
        ArrayField: Field*;
        FieldType: string;
        Identifier: string;
        Keyword;
    }
}
