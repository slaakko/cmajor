namespace System.Text.Parsing
{
    grammar stdlib
    {
        spaces;
        newline;
        comment;
        line_comment;
        block_comment;
        spaces_and_comments;
        digit_sequence;
        sign;
        int: int;
        uint: uint;
        long: long;
        ulong: ulong;
        hex: ulong;
        hex_literal: ulong;
        octaldigit: int;
        real: double;
        ureal: double;
        fractional_real;
        exponent_part;
        number: double;
        bool: bool;
        identifier: string;
        qualified_id: string;
        escape: char;
        char: char;
        string: string;
    }
}
