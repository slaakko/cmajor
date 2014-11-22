/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Visitor.hpp>

namespace Cm { namespace Parsing {

class IdentifierCharsRule
{
public:
    static void Init();
    static void Done();
    static IdentifierCharsRule& Instance();
    Rule* GetRule() const{ return rule.get(); }
private:
    IdentifierCharsRule();
    std::unique_ptr<Rule> rule;
    static std::unique_ptr<IdentifierCharsRule> instance;
    static int initCount;
};

IdentifierCharsRule::IdentifierCharsRule(): 
    rule(new Rule("identifier_chars", nullptr,
            new PositiveParser(
                new AlternativeParser(
                    new AlternativeParser(
                        new AlternativeParser(
                            new LetterParser(),
                            new DigitParser()),
                        new CharParser('_')),
                    new CharParser('.')))))
{
}

void IdentifierCharsRule::Init()
{
    if (initCount++ == 0)
    {
        instance.reset(new IdentifierCharsRule());
    }
}

void IdentifierCharsRule::Done()
{
    if (--initCount == 0)
    {
        instance.reset();
    }
}

IdentifierCharsRule& IdentifierCharsRule::Instance()
{
    return *instance;
}

std::unique_ptr<IdentifierCharsRule> IdentifierCharsRule::instance;

int IdentifierCharsRule::initCount = 0;

KeywordParser::KeywordParser(const std::string& keyword_): Parser("keyword", "\"" + keyword_ + "\""), keyword(keyword_), continuationRuleName() 
{
    continuationRule = IdentifierCharsRule::Instance().GetRule();
    keywordStringParser = new StringParser(keyword);
    CreateKeywordRule();
}

KeywordParser::KeywordParser(const std::string& keyword_, const std::string& continuationRuleName_): Parser("keyword", "\"" + keyword_ + "\""), keyword(keyword_), continuationRuleName(continuationRuleName_) 
{
    continuationRule = IdentifierCharsRule::Instance().GetRule();
    keywordStringParser = new StringParser(keyword);
    CreateKeywordRule();
}

void KeywordParser::SetContinuationRule(Rule* continuationRule_)
{
    if (continuationRule_)
    {
        continuationRule = continuationRule_;
        Own(continuationRule);
        CreateKeywordRule();
    }
}

void KeywordParser::CreateKeywordRule()
{
    keywordRule = new Rule(keyword, nullptr,
        new DifferenceParser(
            keywordStringParser,
            new TokenParser(
                new SequenceParser(
                    keywordStringParser,
                    continuationRule))));
}

Match KeywordParser::Parse(Scanner& scanner, ObjectStack& stack)
{
    return keywordRule->Parse(scanner, stack);
}

void KeywordParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

KeywordListParser::KeywordListParser(const std::string& selectorRuleName_, const std::vector<std::string>& keywords_): 
    Parser("keywordList", "keyword_list"), selectorRuleName(selectorRuleName_), keywords(keywords_.begin(), keywords_.end())
{
}

Match KeywordListParser::Parse(Scanner& scanner, ObjectStack& stack)
{
    if (selectorRule)
    {
        Span save = scanner.GetSpan();
        Match match = selectorRule->Parse(scanner, stack);
        if (match.Hit())
        {
            const char* matchBegin = scanner.Start() + save.Start();
            const char* matchEnd = scanner.Start() + scanner.GetSpan().Start();
            std::string keyword(matchBegin, matchEnd);
            if (keywords.find(keyword) != keywords.end())
            {
                return match;
            }
            scanner.SetSpan(save);
        }
    }
    return Match::Nothing();
}

void KeywordListParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void KeywordInit()
{
    IdentifierCharsRule::Init();
}

void KeywordDone()
{
    IdentifierCharsRule::Done();
}



} } // namespace Cm::Parsing
