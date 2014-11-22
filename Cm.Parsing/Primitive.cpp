/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Scanner.hpp>
#include <Cm.Parsing/Visitor.hpp>
#include <cctype>

namespace Cm { namespace Parsing {

CharParser::CharParser(char c_): Parser("char", "\"" + std::string(1, c_) + "\""), c(c_) 
{
}

Match CharParser::Parse(Scanner& scanner, ObjectStack& stack)
{
    if (!scanner.AtEnd())
    {
        if (scanner.GetChar() == c)
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void CharParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

StringParser::StringParser(const std::string& s_): Parser("string", "\"" + s_ + "\""), s(s_) 
{
}

Match StringParser::Parse(Scanner& scanner, ObjectStack& stack)
{
    Match match = Match::Empty();
    int i = 0;
    int n = int(s.length());
    while (i < n && !scanner.AtEnd() && scanner.GetChar() == s[i])
    {
        ++scanner;
        ++i;
        match.Concatenate(Match::One());
    }
    if (i == n)
    {
        return match;
    }
    return Match::Nothing();
}

void StringParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

CharSetParser::CharSetParser(const std::string& s_): Parser("charSet", "[" + s_ + "]"), s(s_), inverse(false) 
{ 
    InitBits(); 
}

CharSetParser::CharSetParser(const std::string& s_, bool inverse_): Parser("charSet", "[" + s_ + "]"), s(s_), inverse(inverse_) 
{ 
    InitBits(); 
}

void CharSetParser::InitBits()
{
    int i = 0;
    int n = int(s.length());
    while (i < n)
    {
        int first = s[i];
        int last = first;
        ++i;
        if (i < n)
        {
            if (s[i] == '-')
            {
                ++i;
                if (i < n)
                {
                    last = s[i];
                    ++i;
                }
                else
                {
                    bits.set((unsigned char)first);
                    first = '-';
                    last = '-';
                }
            }
        }
        for (int b = first; b <= last; ++b)
        {
            bits.set((unsigned char)b);
        }
    }
}

Match CharSetParser::Parse(Scanner& scanner, ObjectStack& stack)
{
    if (!scanner.AtEnd())
    {
        if (bits[(unsigned char)scanner.GetChar()] != inverse)
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void CharSetParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

EmptyParser::EmptyParser(): Parser("empty", "") 
{
}

Match EmptyParser::Parse(Scanner& scanner, ObjectStack& stack)
{
    return Match::Empty();
}

void EmptyParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

SpaceParser::SpaceParser(): Parser("space", "space") 
{
}

Match SpaceParser::Parse(Scanner& scanner, ObjectStack& stack)
{
    if (!scanner.AtEnd())
    {
        if (std::isspace(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void SpaceParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

LetterParser::LetterParser(): Parser("letter", "letter") 
{
}

Match LetterParser::Parse(Scanner& scanner, ObjectStack& stack)
{
    if (!scanner.AtEnd())
    {
        if (std::isalpha(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void LetterParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

DigitParser::DigitParser(): Parser("digit", "digit") 
{
}

Match DigitParser::Parse(Scanner& scanner, ObjectStack& stack)
{
    if (!scanner.AtEnd())
    {
        if (std::isdigit(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void DigitParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

HexDigitParser::HexDigitParser(): Parser("hexdigit", "hexdigit") 
{
}

Match HexDigitParser::Parse(Scanner& scanner, ObjectStack& stack)
{
    if (!scanner.AtEnd())
    {
        if (std::isxdigit(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void HexDigitParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

PunctuationParser::PunctuationParser(): Parser("punctuation", "punctuation") 
{
}

Match PunctuationParser::Parse(Scanner& scanner, ObjectStack& stack)
{
    if (!scanner.AtEnd())
    {
        if (std::ispunct(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void PunctuationParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

AnyCharParser::AnyCharParser(): Parser("anychar", "anychar") 
{
}

Match AnyCharParser::Parse(Scanner& scanner, ObjectStack& stack)
{
    if (!scanner.AtEnd())
    {
        ++scanner;
        return Match::One();
    }
    return Match::Nothing();
}

void AnyCharParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}


} } // namespace Cm::Parsing
