#include "Literal.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>

namespace Cm { namespace Parsing { namespace Cpp {

using namespace Cm::Parsing::CppObjectModel;
using namespace Cm::Parsing;

LiteralGrammar* LiteralGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

LiteralGrammar* LiteralGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    LiteralGrammar* grammar(new LiteralGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

LiteralGrammar::LiteralGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("LiteralGrammar", parsingDomain_->GetNamespaceScope("Cm.Parsing.Cpp"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Parsing::CppObjectModel::Literal* LiteralGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
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
    Cm::Parsing::CppObjectModel::Literal* result = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Literal*>*>(value.get());
    stack.pop();
    return result;
}

class LiteralGrammar::LiteralRule : public Cm::Parsing::Rule
{
public:
    LiteralRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Literal*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Literal*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A5Action));
        Cm::Parsing::NonterminalParser* integerLiteralNonterminalParser = GetNonterminal("IntegerLiteral");
        integerLiteralNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostIntegerLiteral));
        Cm::Parsing::NonterminalParser* characterLiteralNonterminalParser = GetNonterminal("CharacterLiteral");
        characterLiteralNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostCharacterLiteral));
        Cm::Parsing::NonterminalParser* floatingLiteralNonterminalParser = GetNonterminal("FloatingLiteral");
        floatingLiteralNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostFloatingLiteral));
        Cm::Parsing::NonterminalParser* stringLiteralNonterminalParser = GetNonterminal("StringLiteral");
        stringLiteralNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostStringLiteral));
        Cm::Parsing::NonterminalParser* booleanLiteralNonterminalParser = GetNonterminal("BooleanLiteral");
        booleanLiteralNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostBooleanLiteral));
        Cm::Parsing::NonterminalParser* pointerLiteralNonterminalParser = GetNonterminal("PointerLiteral");
        pointerLiteralNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostPointerLiteral));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromIntegerLiteral;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromCharacterLiteral;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromFloatingLiteral;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromStringLiteral;
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromBooleanLiteral;
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromPointerLiteral;
    }
    void PostIntegerLiteral(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIntegerLiteral_value = std::move(stack.top());
            context.fromIntegerLiteral = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Literal*>*>(fromIntegerLiteral_value.get());
            stack.pop();
        }
    }
    void PostCharacterLiteral(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCharacterLiteral_value = std::move(stack.top());
            context.fromCharacterLiteral = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Literal*>*>(fromCharacterLiteral_value.get());
            stack.pop();
        }
    }
    void PostFloatingLiteral(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFloatingLiteral_value = std::move(stack.top());
            context.fromFloatingLiteral = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Literal*>*>(fromFloatingLiteral_value.get());
            stack.pop();
        }
    }
    void PostStringLiteral(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStringLiteral_value = std::move(stack.top());
            context.fromStringLiteral = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Literal*>*>(fromStringLiteral_value.get());
            stack.pop();
        }
    }
    void PostBooleanLiteral(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromBooleanLiteral_value = std::move(stack.top());
            context.fromBooleanLiteral = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Literal*>*>(fromBooleanLiteral_value.get());
            stack.pop();
        }
    }
    void PostPointerLiteral(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromPointerLiteral_value = std::move(stack.top());
            context.fromPointerLiteral = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Literal*>*>(fromPointerLiteral_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromIntegerLiteral(), fromCharacterLiteral(), fromFloatingLiteral(), fromStringLiteral(), fromBooleanLiteral(), fromPointerLiteral() {}
        Cm::Parsing::CppObjectModel::Literal* value;
        Cm::Parsing::CppObjectModel::Literal* fromIntegerLiteral;
        Cm::Parsing::CppObjectModel::Literal* fromCharacterLiteral;
        Cm::Parsing::CppObjectModel::Literal* fromFloatingLiteral;
        Cm::Parsing::CppObjectModel::Literal* fromStringLiteral;
        Cm::Parsing::CppObjectModel::Literal* fromBooleanLiteral;
        Cm::Parsing::CppObjectModel::Literal* fromPointerLiteral;
    };
    std::stack<Context> contextStack;
    Context context;
};

class LiteralGrammar::IntegerLiteralRule : public Cm::Parsing::Rule
{
public:
    IntegerLiteralRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Literal*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Literal*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A2Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::Literal(std::string(matchBegin, matchEnd));
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::Literal(std::string(matchBegin, matchEnd));
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::Literal(std::string(matchBegin, matchEnd));
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::CppObjectModel::Literal* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class LiteralGrammar::CharacterLiteralRule : public Cm::Parsing::Rule
{
public:
    CharacterLiteralRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Literal*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Literal*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CharacterLiteralRule>(this, &CharacterLiteralRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CharacterLiteralRule>(this, &CharacterLiteralRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CharacterLiteralRule>(this, &CharacterLiteralRule::A2Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::Literal(std::string(matchBegin, matchEnd));
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::Literal(std::string(matchBegin, matchEnd));
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::Literal(std::string(matchBegin, matchEnd));
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::CppObjectModel::Literal* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class LiteralGrammar::CCharSequenceRule : public Cm::Parsing::Rule
{
public:
    CCharSequenceRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CCharSequenceRule>(this, &CCharSequenceRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = std::string(matchBegin, matchEnd);
    }
private:
    struct Context
    {
        Context(): value() {}
        std::string value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class LiteralGrammar::FloatingLiteralRule : public Cm::Parsing::Rule
{
public:
    FloatingLiteralRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Literal*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Literal*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<FloatingLiteralRule>(this, &FloatingLiteralRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<FloatingLiteralRule>(this, &FloatingLiteralRule::A1Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::Literal(std::string(matchBegin, matchEnd));
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::Literal(std::string(matchBegin, matchEnd));
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::CppObjectModel::Literal* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class LiteralGrammar::StringLiteralRule : public Cm::Parsing::Rule
{
public:
    StringLiteralRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Literal*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Literal*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::Literal(std::string(matchBegin, matchEnd));
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::CppObjectModel::Literal* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class LiteralGrammar::BooleanLiteralRule : public Cm::Parsing::Rule
{
public:
    BooleanLiteralRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Literal*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Literal*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<BooleanLiteralRule>(this, &BooleanLiteralRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<BooleanLiteralRule>(this, &BooleanLiteralRule::A1Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::Literal("true");
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::Literal("false");
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::CppObjectModel::Literal* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class LiteralGrammar::PointerLiteralRule : public Cm::Parsing::Rule
{
public:
    PointerLiteralRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Literal*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Literal*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PointerLiteralRule>(this, &PointerLiteralRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::Literal("nullptr");
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::CppObjectModel::Literal* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

void LiteralGrammar::GetReferencedGrammars()
{
}

void LiteralGrammar::CreateRules()
{
    AddRule(new LiteralRule("Literal", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::ActionParser("A0",
                                new Cm::Parsing::NonterminalParser("IntegerLiteral", "IntegerLiteral", 0)),
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::NonterminalParser("CharacterLiteral", "CharacterLiteral", 0))),
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::NonterminalParser("FloatingLiteral", "FloatingLiteral", 0))),
                    new Cm::Parsing::ActionParser("A3",
                        new Cm::Parsing::NonterminalParser("StringLiteral", "StringLiteral", 0))),
                new Cm::Parsing::ActionParser("A4",
                    new Cm::Parsing::NonterminalParser("BooleanLiteral", "BooleanLiteral", 0))),
            new Cm::Parsing::ActionParser("A5",
                new Cm::Parsing::NonterminalParser("PointerLiteral", "PointerLiteral", 0)))));
    AddRule(new IntegerLiteralRule("IntegerLiteral", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::TokenParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::NonterminalParser("dl", "DecimalLiteral", 0),
                            new Cm::Parsing::OptionalParser(
                                new Cm::Parsing::NonterminalParser("is1", "IntegerSuffix", 0))))),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::TokenParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::NonterminalParser("ol", "OctalLiteral", 0),
                            new Cm::Parsing::OptionalParser(
                                new Cm::Parsing::NonterminalParser("is2", "IntegerSuffix", 0)))))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::TokenParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::NonterminalParser("hl", "HexadecimalLiteral", 0),
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::NonterminalParser("is3", "IntegerSuffix", 0))))))));
    AddRule(new Cm::Parsing::Rule("DecimalLiteral", GetScope(),
        new Cm::Parsing::TokenParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::CharSetParser("1-9"),
                new Cm::Parsing::KleeneStarParser(
                    new Cm::Parsing::DigitParser())))));
    AddRule(new Cm::Parsing::Rule("OctalLiteral", GetScope(),
        new Cm::Parsing::TokenParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::CharParser('0'),
                new Cm::Parsing::KleeneStarParser(
                    new Cm::Parsing::NonterminalParser("OctalDigit", "OctalDigit", 0))))));
    AddRule(new Cm::Parsing::Rule("OctalDigit", GetScope(),
        new Cm::Parsing::CharSetParser("0-7")));
    AddRule(new Cm::Parsing::Rule("HexadecimalLiteral", GetScope(),
        new Cm::Parsing::TokenParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::StringParser("0x"),
                    new Cm::Parsing::StringParser("0X")),
                new Cm::Parsing::PositiveParser(
                    new Cm::Parsing::HexDigitParser())))));
    AddRule(new Cm::Parsing::Rule("IntegerSuffix", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::TokenParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::NonterminalParser("UnsignedSuffix", "UnsignedSuffix", 0),
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::AlternativeParser(
                                new Cm::Parsing::NonterminalParser("LongLongSuffix", "LongLongSuffix", 0),
                                new Cm::Parsing::NonterminalParser("LongSuffix", "LongSuffix", 0))))),
                new Cm::Parsing::TokenParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::NonterminalParser("LongLongSuffix", "LongLongSuffix", 0),
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::NonterminalParser("UnsignedSuffix", "UnsignedSuffix", 0))))),
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::NonterminalParser("LongSuffix", "LongSuffix", 0),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("UnsignedSuffix", "UnsignedSuffix", 0)))))));
    AddRule(new Cm::Parsing::Rule("UnsignedSuffix", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::CharParser('u'),
            new Cm::Parsing::CharParser('U'))));
    AddRule(new Cm::Parsing::Rule("LongLongSuffix", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::StringParser("ll"),
            new Cm::Parsing::StringParser("LL"))));
    AddRule(new Cm::Parsing::Rule("LongSuffix", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::CharParser('l'),
            new Cm::Parsing::CharParser('L'))));
    AddRule(new CharacterLiteralRule("CharacterLiteral", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("NarrowCharacterLiteral", "NarrowCharacterLiteral", 0)),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::NonterminalParser("UniversalCharacterLiteral", "UniversalCharacterLiteral", 0))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::NonterminalParser("WideCharacterLiteral", "WideCharacterLiteral", 0)))));
    AddRule(new Cm::Parsing::Rule("NarrowCharacterLiteral", GetScope(),
        new Cm::Parsing::TokenParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('\''),
                    new Cm::Parsing::NonterminalParser("CCharSequence", "CCharSequence", 0)),
                new Cm::Parsing::CharParser('\'')))));
    AddRule(new Cm::Parsing::Rule("UniversalCharacterLiteral", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::CharParser('u'),
                            new Cm::Parsing::CharParser('\'')),
                        new Cm::Parsing::NonterminalParser("cs1", "CCharSequence", 0)),
                    new Cm::Parsing::CharParser('\''))),
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::CharParser('U'),
                            new Cm::Parsing::CharParser('\'')),
                        new Cm::Parsing::NonterminalParser("cs2", "CCharSequence", 0)),
                    new Cm::Parsing::CharParser('\''))))));
    AddRule(new Cm::Parsing::Rule("WideCharacterLiteral", GetScope(),
        new Cm::Parsing::TokenParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::CharParser('L'),
                        new Cm::Parsing::CharParser('\'')),
                    new Cm::Parsing::NonterminalParser("CCharSequence", "CCharSequence", 0)),
                new Cm::Parsing::CharParser('\'')))));
    AddRule(new CCharSequenceRule("CCharSequence", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::PositiveParser(
                    new Cm::Parsing::NonterminalParser("CChar", "CChar", 0))))));
    AddRule(new Cm::Parsing::Rule("CChar", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::CharSetParser("\'\\\n", true),
            new Cm::Parsing::NonterminalParser("EscapeSequence", "EscapeSequence", 0))));
    AddRule(new Cm::Parsing::Rule("EscapeSequence", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::NonterminalParser("SimpleEscapeSequence", "SimpleEscapeSequence", 0),
                new Cm::Parsing::NonterminalParser("OctalEscapeSequence", "OctalEscapeSequence", 0)),
            new Cm::Parsing::NonterminalParser("HexadecimalEscapeSequence", "HexadecimalEscapeSequence", 0))));
    AddRule(new Cm::Parsing::Rule("SimpleEscapeSequence", GetScope(),
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
                                                new Cm::Parsing::StringParser("\\\'"),
                                                new Cm::Parsing::StringParser("\\\"")),
                                            new Cm::Parsing::StringParser("\\?")),
                                        new Cm::Parsing::StringParser("\\\\")),
                                    new Cm::Parsing::StringParser("\\a")),
                                new Cm::Parsing::StringParser("\\b")),
                            new Cm::Parsing::StringParser("\\f")),
                        new Cm::Parsing::StringParser("\\n")),
                    new Cm::Parsing::StringParser("\\r")),
                new Cm::Parsing::StringParser("\\t")),
            new Cm::Parsing::StringParser("\\v"))));
    AddRule(new Cm::Parsing::Rule("OctalEscapeSequence", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('\\'),
                    new Cm::Parsing::NonterminalParser("OctalDigit", "OctalDigit", 0)),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::NonterminalParser("OctalDigit", "OctalDigit", 0))),
            new Cm::Parsing::OptionalParser(
                new Cm::Parsing::NonterminalParser("OctalDigit", "OctalDigit", 0)))));
    AddRule(new Cm::Parsing::Rule("HexadecimalEscapeSequence", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::StringParser("\\x"),
            new Cm::Parsing::PositiveParser(
                new Cm::Parsing::HexDigitParser()))));
    AddRule(new FloatingLiteralRule("FloatingLiteral", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::TokenParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::NonterminalParser("FractionalConstant", "FractionalConstant", 0),
                            new Cm::Parsing::OptionalParser(
                                new Cm::Parsing::NonterminalParser("ExponentPart", "ExponentPart", 0))),
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::NonterminalParser("FloatingSuffix", "FloatingSuffix", 0))))),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::TokenParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::NonterminalParser("DigitSequence", "DigitSequence", 0),
                            new Cm::Parsing::NonterminalParser("ExponentPart", "ExponentPart", 0)),
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::NonterminalParser("FloatingSuffix", "FloatingSuffix", 0))))))));
    AddRule(new Cm::Parsing::Rule("FractionalConstant", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("DigitSequence", "DigitSequence", 0)),
                    new Cm::Parsing::CharParser('.')),
                new Cm::Parsing::NonterminalParser("DigitSequence", "DigitSequence", 0)),
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::NonterminalParser("DigitSequence", "DigitSequence", 0),
                new Cm::Parsing::CharParser('.')))));
    AddRule(new Cm::Parsing::Rule("DigitSequence", GetScope(),
        new Cm::Parsing::PositiveParser(
            new Cm::Parsing::DigitParser())));
    AddRule(new Cm::Parsing::Rule("ExponentPart", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::CharParser('e'),
                    new Cm::Parsing::CharParser('E')),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::NonterminalParser("Sign", "Sign", 0))),
            new Cm::Parsing::NonterminalParser("DigitSequence", "DigitSequence", 0))));
    AddRule(new Cm::Parsing::Rule("Sign", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::CharParser('+'),
            new Cm::Parsing::CharParser('-'))));
    AddRule(new Cm::Parsing::Rule("FloatingSuffix", GetScope(),
        new Cm::Parsing::CharSetParser("fFlL")));
    AddRule(new StringLiteralRule("StringLiteral", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::OptionalParser(
                                new Cm::Parsing::NonterminalParser("EncodingPrefix", "EncodingPrefix", 0)),
                            new Cm::Parsing::CharParser('\"')),
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::NonterminalParser("SCharSequence", "SCharSequence", 0))),
                    new Cm::Parsing::CharParser('\"'))))));
    AddRule(new Cm::Parsing::Rule("EncodingPrefix", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::StringParser("u8"),
                    new Cm::Parsing::CharParser('u')),
                new Cm::Parsing::CharParser('U')),
            new Cm::Parsing::CharParser('L'))));
    AddRule(new Cm::Parsing::Rule("SCharSequence", GetScope(),
        new Cm::Parsing::PositiveParser(
            new Cm::Parsing::NonterminalParser("SChar", "SChar", 0))));
    AddRule(new Cm::Parsing::Rule("SChar", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::CharSetParser("\"\\\n", true),
            new Cm::Parsing::NonterminalParser("EscapeSequence", "EscapeSequence", 0))));
    AddRule(new BooleanLiteralRule("BooleanLiteral", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::KeywordParser("true")),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::KeywordParser("false")))));
    AddRule(new PointerLiteralRule("PointerLiteral", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::KeywordParser("nullptr"))));
}

} } } // namespace Cm.Parsing.Cpp
