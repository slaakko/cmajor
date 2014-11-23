#include "Primitive.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Parsing.Syntax/Element.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

using namespace Cm::Parsing;

PrimitiveGrammar* PrimitiveGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

PrimitiveGrammar* PrimitiveGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    PrimitiveGrammar* grammar(new PrimitiveGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

PrimitiveGrammar::PrimitiveGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("PrimitiveGrammar", parsingDomain_->GetNamespaceScope("Cm.Parsing.Syntax"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Parsing::Parser* PrimitiveGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
{
    Cm::Parsing::Scanner scanner(start, end, fileName, fileIndex, SkipRule());
    std::unique_ptr<Cm::Parsing::XmlLog> xmlLog;
    if (Log())
    {
        xmlLog.reset(new Cm::Parsing::XmlLog(*Log(), MaxLogLineLength()));
        scanner.SetLog(xmlLog.get());
        xmlLog->WriteBeginRule("parse");
    }
    Cm::Parsing::ObjectStack stack;
    Cm::Parsing::Match match = Cm::Parsing::Grammar::Parse(scanner, stack);
    Cm::Parsing::Span stop = scanner.GetSpan();
    if (Log())
    {
        xmlLog->WriteEndRule("parse");
    }
    if (!match.Hit() || stop.Start() != int(end - start))
    {
        if (StartRule())
        {
            throw Cm::Parsing::ExpectationFailure(StartRule()->Info(), fileName, stop, start, end);
        }
        else
        {
            throw Cm::Parsing::ParsingException("grammar '" + Name() + "' has no start rule", fileName, scanner.GetSpan(), start, end);
        }
    }
    std::unique_ptr<Cm::Parsing::Object> value = std::move(stack.top());
    Cm::Parsing::Parser* result = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(value.get());
    stack.pop();
    return result;
}

class PrimitiveGrammar::PrimitiveRule : public Cm::Parsing::Rule
{
public:
    PrimitiveRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A5Action));
        Cm::Parsing::ActionParser* a6ActionParser = GetAction("A6");
        a6ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A6Action));
        Cm::Parsing::ActionParser* a7ActionParser = GetAction("A7");
        a7ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A7Action));
        Cm::Parsing::ActionParser* a8ActionParser = GetAction("A8");
        a8ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A8Action));
        Cm::Parsing::ActionParser* a9ActionParser = GetAction("A9");
        a9ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A9Action));
        Cm::Parsing::ActionParser* a10ActionParser = GetAction("A10");
        a10ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A10Action));
        Cm::Parsing::ActionParser* a11ActionParser = GetAction("A11");
        a11ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A11Action));
        Cm::Parsing::NonterminalParser* charNonterminalParser = GetNonterminal("Char");
        charNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostChar));
        Cm::Parsing::NonterminalParser* stringNonterminalParser = GetNonterminal("String");
        stringNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostString));
        Cm::Parsing::NonterminalParser* charSetNonterminalParser = GetNonterminal("CharSet");
        charSetNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostCharSet));
        Cm::Parsing::NonterminalParser* keywordNonterminalParser = GetNonterminal("Keyword");
        keywordNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostKeyword));
        Cm::Parsing::NonterminalParser* keywordListNonterminalParser = GetNonterminal("KeywordList");
        keywordListNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostKeywordList));
        Cm::Parsing::NonterminalParser* emptyNonterminalParser = GetNonterminal("Empty");
        emptyNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostEmpty));
        Cm::Parsing::NonterminalParser* spaceNonterminalParser = GetNonterminal("Space");
        spaceNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSpace));
        Cm::Parsing::NonterminalParser* anyCharNonterminalParser = GetNonterminal("AnyChar");
        anyCharNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostAnyChar));
        Cm::Parsing::NonterminalParser* letterNonterminalParser = GetNonterminal("Letter");
        letterNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostLetter));
        Cm::Parsing::NonterminalParser* digitNonterminalParser = GetNonterminal("Digit");
        digitNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostDigit));
        Cm::Parsing::NonterminalParser* hexDigitNonterminalParser = GetNonterminal("HexDigit");
        hexDigitNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostHexDigit));
        Cm::Parsing::NonterminalParser* punctuationNonterminalParser = GetNonterminal("Punctuation");
        punctuationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostPunctuation));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromChar;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromString;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromCharSet;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromKeyword;
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromKeywordList;
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromEmpty;
    }
    void A6Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromSpace;
    }
    void A7Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromAnyChar;
    }
    void A8Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromLetter;
    }
    void A9Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromDigit;
    }
    void A10Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromHexDigit;
    }
    void A11Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromPunctuation;
    }
    void PostChar(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromChar_value = std::move(stack.top());
            context.fromChar = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromChar_value.get());
            stack.pop();
        }
    }
    void PostString(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromString_value = std::move(stack.top());
            context.fromString = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromString_value.get());
            stack.pop();
        }
    }
    void PostCharSet(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCharSet_value = std::move(stack.top());
            context.fromCharSet = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromCharSet_value.get());
            stack.pop();
        }
    }
    void PostKeyword(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromKeyword_value = std::move(stack.top());
            context.fromKeyword = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromKeyword_value.get());
            stack.pop();
        }
    }
    void PostKeywordList(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromKeywordList_value = std::move(stack.top());
            context.fromKeywordList = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromKeywordList_value.get());
            stack.pop();
        }
    }
    void PostEmpty(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromEmpty_value = std::move(stack.top());
            context.fromEmpty = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromEmpty_value.get());
            stack.pop();
        }
    }
    void PostSpace(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromSpace_value = std::move(stack.top());
            context.fromSpace = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromSpace_value.get());
            stack.pop();
        }
    }
    void PostAnyChar(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAnyChar_value = std::move(stack.top());
            context.fromAnyChar = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromAnyChar_value.get());
            stack.pop();
        }
    }
    void PostLetter(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromLetter_value = std::move(stack.top());
            context.fromLetter = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromLetter_value.get());
            stack.pop();
        }
    }
    void PostDigit(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDigit_value = std::move(stack.top());
            context.fromDigit = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromDigit_value.get());
            stack.pop();
        }
    }
    void PostHexDigit(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromHexDigit_value = std::move(stack.top());
            context.fromHexDigit = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromHexDigit_value.get());
            stack.pop();
        }
    }
    void PostPunctuation(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromPunctuation_value = std::move(stack.top());
            context.fromPunctuation = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromPunctuation_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromChar(), fromString(), fromCharSet(), fromKeyword(), fromKeywordList(), fromEmpty(), fromSpace(), fromAnyChar(), fromLetter(), fromDigit(), fromHexDigit(), fromPunctuation() {}
        Cm::Parsing::Parser* value;
        Cm::Parsing::Parser* fromChar;
        Cm::Parsing::Parser* fromString;
        Cm::Parsing::Parser* fromCharSet;
        Cm::Parsing::Parser* fromKeyword;
        Cm::Parsing::Parser* fromKeywordList;
        Cm::Parsing::Parser* fromEmpty;
        Cm::Parsing::Parser* fromSpace;
        Cm::Parsing::Parser* fromAnyChar;
        Cm::Parsing::Parser* fromLetter;
        Cm::Parsing::Parser* fromDigit;
        Cm::Parsing::Parser* fromHexDigit;
        Cm::Parsing::Parser* fromPunctuation;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimitiveGrammar::CharRule : public Cm::Parsing::Rule
{
public:
    CharRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CharRule>(this, &CharRule::A0Action));
        Cm::Parsing::NonterminalParser* chrNonterminalParser = GetNonterminal("chr");
        chrNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CharRule>(this, &CharRule::Postchr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new CharParser(context.fromchr);
    }
    void Postchr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromchr_value = std::move(stack.top());
            context.fromchr = *static_cast<Cm::Parsing::ValueObject<char>*>(fromchr_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromchr() {}
        Cm::Parsing::Parser* value;
        char fromchr;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimitiveGrammar::StringRule : public Cm::Parsing::Rule
{
public:
    StringRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StringRule>(this, &StringRule::A0Action));
        Cm::Parsing::NonterminalParser* strNonterminalParser = GetNonterminal("str");
        strNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StringRule>(this, &StringRule::Poststr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new StringParser(context.fromstr);
    }
    void Poststr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromstr_value = std::move(stack.top());
            context.fromstr = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromstr_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromstr() {}
        Cm::Parsing::Parser* value;
        std::string fromstr;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimitiveGrammar::CharSetRule : public Cm::Parsing::Rule
{
public:
    CharSetRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::Parser*");
        AddLocalVariable(AttrOrVariable("bool", "inverse"));
        AddLocalVariable(AttrOrVariable("std::string", "s"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CharSetRule>(this, &CharSetRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CharSetRule>(this, &CharSetRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CharSetRule>(this, &CharSetRule::A2Action));
        Cm::Parsing::NonterminalParser* charSetRangeNonterminalParser = GetNonterminal("CharSetRange");
        charSetRangeNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CharSetRule>(this, &CharSetRule::PostCharSetRange));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new CharSetParser(context.s, context.inverse);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.inverse = true;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.s.append(context.fromCharSetRange);
    }
    void PostCharSetRange(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCharSetRange_value = std::move(stack.top());
            context.fromCharSetRange = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromCharSetRange_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), inverse(), s(), fromCharSetRange() {}
        Cm::Parsing::Parser* value;
        bool inverse;
        std::string s;
        std::string fromCharSetRange;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimitiveGrammar::CharSetRangeRule : public Cm::Parsing::Rule
{
public:
    CharSetRangeRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("std::string");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<std::string>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CharSetRangeRule>(this, &CharSetRangeRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CharSetRangeRule>(this, &CharSetRangeRule::A1Action));
        Cm::Parsing::NonterminalParser* firstNonterminalParser = GetNonterminal("first");
        firstNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CharSetRangeRule>(this, &CharSetRangeRule::Postfirst));
        Cm::Parsing::NonterminalParser* lastNonterminalParser = GetNonterminal("last");
        lastNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CharSetRangeRule>(this, &CharSetRangeRule::Postlast));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value.append(1, context.fromfirst);
        context.value.append(1, '-');
        context.value.append(1, context.fromlast);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value.append(1, context.fromfirst);
    }
    void Postfirst(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromfirst_value = std::move(stack.top());
            context.fromfirst = *static_cast<Cm::Parsing::ValueObject<char>*>(fromfirst_value.get());
            stack.pop();
        }
    }
    void Postlast(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromlast_value = std::move(stack.top());
            context.fromlast = *static_cast<Cm::Parsing::ValueObject<char>*>(fromlast_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromfirst(), fromlast() {}
        std::string value;
        char fromfirst;
        char fromlast;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimitiveGrammar::CharSetCharRule : public Cm::Parsing::Rule
{
public:
    CharSetCharRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("char");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<char>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CharSetCharRule>(this, &CharSetCharRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CharSetCharRule>(this, &CharSetCharRule::A1Action));
        Cm::Parsing::NonterminalParser* escapeNonterminalParser = GetNonterminal("escape");
        escapeNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CharSetCharRule>(this, &CharSetCharRule::Postescape));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = *matchBegin;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromescape;
    }
    void Postescape(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromescape_value = std::move(stack.top());
            context.fromescape = *static_cast<Cm::Parsing::ValueObject<char>*>(fromescape_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromescape() {}
        char value;
        char fromescape;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimitiveGrammar::KeywordRule : public Cm::Parsing::Rule
{
public:
    KeywordRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<KeywordRule>(this, &KeywordRule::A0Action));
        Cm::Parsing::NonterminalParser* keywordBodyNonterminalParser = GetNonterminal("KeywordBody");
        keywordBodyNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<KeywordRule>(this, &KeywordRule::PostKeywordBody));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromKeywordBody;
    }
    void PostKeywordBody(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromKeywordBody_value = std::move(stack.top());
            context.fromKeywordBody = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromKeywordBody_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromKeywordBody() {}
        Cm::Parsing::Parser* value;
        Cm::Parsing::Parser* fromKeywordBody;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimitiveGrammar::KeywordBodyRule : public Cm::Parsing::Rule
{
public:
    KeywordBodyRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<KeywordBodyRule>(this, &KeywordBodyRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<KeywordBodyRule>(this, &KeywordBodyRule::A1Action));
        Cm::Parsing::NonterminalParser* strNonterminalParser = GetNonterminal("str");
        strNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<KeywordBodyRule>(this, &KeywordBodyRule::Poststr));
        Cm::Parsing::NonterminalParser* continuationNonterminalParser = GetNonterminal("continuation");
        continuationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<KeywordBodyRule>(this, &KeywordBodyRule::Postcontinuation));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new KeywordParser(context.fromstr, context.fromcontinuation);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new KeywordParser(context.fromstr);
    }
    void Poststr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromstr_value = std::move(stack.top());
            context.fromstr = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromstr_value.get());
            stack.pop();
        }
    }
    void Postcontinuation(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromcontinuation_value = std::move(stack.top());
            context.fromcontinuation = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromcontinuation_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromstr(), fromcontinuation() {}
        Cm::Parsing::Parser* value;
        std::string fromstr;
        std::string fromcontinuation;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimitiveGrammar::KeywordListRule : public Cm::Parsing::Rule
{
public:
    KeywordListRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<KeywordListRule>(this, &KeywordListRule::A0Action));
        Cm::Parsing::NonterminalParser* keywordListBodyNonterminalParser = GetNonterminal("KeywordListBody");
        keywordListBodyNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<KeywordListRule>(this, &KeywordListRule::PostKeywordListBody));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromKeywordListBody;
    }
    void PostKeywordListBody(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromKeywordListBody_value = std::move(stack.top());
            context.fromKeywordListBody = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Parser*>*>(fromKeywordListBody_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromKeywordListBody() {}
        Cm::Parsing::Parser* value;
        Cm::Parsing::Parser* fromKeywordListBody;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimitiveGrammar::KeywordListBodyRule : public Cm::Parsing::Rule
{
public:
    KeywordListBodyRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::Parser*");
        AddLocalVariable(AttrOrVariable("std::vector<std::string>", "keywords"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<KeywordListBodyRule>(this, &KeywordListBodyRule::A0Action));
        Cm::Parsing::NonterminalParser* selectorNonterminalParser = GetNonterminal("selector");
        selectorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<KeywordListBodyRule>(this, &KeywordListBodyRule::Postselector));
        Cm::Parsing::NonterminalParser* stringArrayNonterminalParser = GetNonterminal("StringArray");
        stringArrayNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<KeywordListBodyRule>(this, &KeywordListBodyRule::PreStringArray));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new KeywordListParser(context.fromselector, context.keywords);
    }
    void Postselector(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromselector_value = std::move(stack.top());
            context.fromselector = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromselector_value.get());
            stack.pop();
        }
    }
    void PreStringArray(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<std::vector<std::string>*>(&context.keywords)));
    }
private:
    struct Context
    {
        Context(): value(), keywords(), fromselector() {}
        Cm::Parsing::Parser* value;
        std::vector<std::string> keywords;
        std::string fromselector;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimitiveGrammar::EmptyRule : public Cm::Parsing::Rule
{
public:
    EmptyRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<EmptyRule>(this, &EmptyRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new EmptyParser;
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::Parser* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimitiveGrammar::SpaceRule : public Cm::Parsing::Rule
{
public:
    SpaceRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SpaceRule>(this, &SpaceRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new SpaceParser;
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::Parser* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimitiveGrammar::AnyCharRule : public Cm::Parsing::Rule
{
public:
    AnyCharRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AnyCharRule>(this, &AnyCharRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new AnyCharParser;
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::Parser* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimitiveGrammar::LetterRule : public Cm::Parsing::Rule
{
public:
    LetterRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LetterRule>(this, &LetterRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new LetterParser;
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::Parser* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimitiveGrammar::DigitRule : public Cm::Parsing::Rule
{
public:
    DigitRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DigitRule>(this, &DigitRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new DigitParser;
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::Parser* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimitiveGrammar::HexDigitRule : public Cm::Parsing::Rule
{
public:
    HexDigitRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<HexDigitRule>(this, &HexDigitRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new HexDigitParser;
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::Parser* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class PrimitiveGrammar::PunctuationRule : public Cm::Parsing::Rule
{
public:
    PunctuationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::Parser*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Parser*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PunctuationRule>(this, &PunctuationRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new PunctuationParser;
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::Parser* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

void PrimitiveGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* parsingDomain = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = parsingDomain->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(parsingDomain);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = parsingDomain->GetGrammar("Cm.Parsing.Syntax.ElementGrammar");
    if (!grammar1)
    {
        grammar1 = Cm::Parsing::Syntax::ElementGrammar::Create(parsingDomain);
    }
    AddGrammarReference(grammar1);
}

void PrimitiveGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("char", this, "Cm.Parsing.stdlib.char"));
    AddRuleLink(new Cm::Parsing::RuleLink("QualifiedId", this, "ElementGrammar.QualifiedId"));
    AddRuleLink(new Cm::Parsing::RuleLink("string", this, "Cm.Parsing.stdlib.string"));
    AddRuleLink(new Cm::Parsing::RuleLink("escape", this, "Cm.Parsing.stdlib.escape"));
    AddRuleLink(new Cm::Parsing::RuleLink("StringArray", this, "ElementGrammar.StringArray"));
    AddRule(new PrimitiveRule("Primitive", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::AlternativeParser(
                                new Cm::Parsing::AlternativeParser(
                                    new Cm::Parsing::AlternativeParser(
                                        new Cm::Parsing::AlternativeParser(
                                            new Cm::Parsing::AlternativeParser(
                                                new Cm::Parsing::AlternativeParser(
                                                    new Cm::Parsing::ActionParser("A0",
                                                        new Cm::Parsing::NonterminalParser("Char", "Char", 0)),
                                                    new Cm::Parsing::ActionParser("A1",
                                                        new Cm::Parsing::NonterminalParser("String", "String", 0))),
                                                new Cm::Parsing::ActionParser("A2",
                                                    new Cm::Parsing::NonterminalParser("CharSet", "CharSet", 0))),
                                            new Cm::Parsing::ActionParser("A3",
                                                new Cm::Parsing::NonterminalParser("Keyword", "Keyword", 0))),
                                        new Cm::Parsing::ActionParser("A4",
                                            new Cm::Parsing::NonterminalParser("KeywordList", "KeywordList", 0))),
                                    new Cm::Parsing::ActionParser("A5",
                                        new Cm::Parsing::NonterminalParser("Empty", "Empty", 0))),
                                new Cm::Parsing::ActionParser("A6",
                                    new Cm::Parsing::NonterminalParser("Space", "Space", 0))),
                            new Cm::Parsing::ActionParser("A7",
                                new Cm::Parsing::NonterminalParser("AnyChar", "AnyChar", 0))),
                        new Cm::Parsing::ActionParser("A8",
                            new Cm::Parsing::NonterminalParser("Letter", "Letter", 0))),
                    new Cm::Parsing::ActionParser("A9",
                        new Cm::Parsing::NonterminalParser("Digit", "Digit", 0))),
                new Cm::Parsing::ActionParser("A10",
                    new Cm::Parsing::NonterminalParser("HexDigit", "HexDigit", 0))),
            new Cm::Parsing::ActionParser("A11",
                new Cm::Parsing::NonterminalParser("Punctuation", "Punctuation", 0)))));
    AddRule(new CharRule("Char", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("chr", "char", 0))));
    AddRule(new StringRule("String", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("str", "string", 0))));
    AddRule(new CharSetRule("CharSet", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::CharParser('['),
                            new Cm::Parsing::OptionalParser(
                                new Cm::Parsing::ActionParser("A1",
                                    new Cm::Parsing::CharParser('^')))),
                        new Cm::Parsing::KleeneStarParser(
                            new Cm::Parsing::ActionParser("A2",
                                new Cm::Parsing::NonterminalParser("CharSetRange", "CharSetRange", 0)))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser(']')))))));
    AddRule(new CharSetRangeRule("CharSetRange", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::NonterminalParser("first", "CharSetChar", 0),
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('-'),
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::NonterminalParser("last", "CharSetChar", 0))),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::EmptyParser())))));
    AddRule(new CharSetCharRule("CharSetChar", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::CharSetParser("\\]", true)),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("escape", "escape", 0)))));
    AddRule(new KeywordRule("Keyword", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("keyword"),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser('('))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("KeywordBody", "KeywordBody", 0))),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(')'))))));
    AddRule(new KeywordBodyRule("KeywordBody", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::NonterminalParser("str", "string", 0),
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser(','),
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("continuation", "QualifiedId", 0)))),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::EmptyParser())))));
    AddRule(new KeywordListRule("KeywordList", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("keyword_list"),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser('('))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("KeywordListBody", "KeywordListBody", 0))),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(')'))))));
    AddRule(new KeywordListBodyRule("KeywordListBody", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("selector", "QualifiedId", 0)),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(','))),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("StringArray", "StringArray", 1))))));
    AddRule(new EmptyRule("Empty", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::KeywordParser("empty"))));
    AddRule(new SpaceRule("Space", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::KeywordParser("space"))));
    AddRule(new AnyCharRule("AnyChar", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::KeywordParser("anychar"))));
    AddRule(new LetterRule("Letter", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::KeywordParser("letter"))));
    AddRule(new DigitRule("Digit", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::KeywordParser("digit"))));
    AddRule(new HexDigitRule("HexDigit", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::KeywordParser("hexdigit"))));
    AddRule(new PunctuationRule("Punctuation", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::KeywordParser("punctuation"))));
}

} } } // namespace Cm.Parsing.Syntax
