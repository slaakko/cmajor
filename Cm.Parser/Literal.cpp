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

namespace Cm { namespace Parser {

using namespace Cm::Ast;
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

LiteralGrammar::LiteralGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("LiteralGrammar", parsingDomain_->GetNamespaceScope("Cm.Parser"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Ast::Node* LiteralGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
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
    Cm::Ast::Node* result = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(value.get());
    stack.pop();
    return result;
}

class LiteralGrammar::LiteralRule : public Cm::Parsing::Rule
{
public:
    LiteralRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::Node*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
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
        Cm::Parsing::NonterminalParser* booleanLiteralNonterminalParser = GetNonterminal("BooleanLiteral");
        booleanLiteralNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostBooleanLiteral));
        Cm::Parsing::NonterminalParser* floatingLiteralNonterminalParser = GetNonterminal("FloatingLiteral");
        floatingLiteralNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostFloatingLiteral));
        Cm::Parsing::NonterminalParser* integerLiteralNonterminalParser = GetNonterminal("IntegerLiteral");
        integerLiteralNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostIntegerLiteral));
        Cm::Parsing::NonterminalParser* charLiteralNonterminalParser = GetNonterminal("CharLiteral");
        charLiteralNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostCharLiteral));
        Cm::Parsing::NonterminalParser* stringLiteralNonterminalParser = GetNonterminal("StringLiteral");
        stringLiteralNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostStringLiteral));
        Cm::Parsing::NonterminalParser* nullLiteralNonterminalParser = GetNonterminal("NullLiteral");
        nullLiteralNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostNullLiteral));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromBooleanLiteral;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromFloatingLiteral;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromIntegerLiteral;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromCharLiteral;
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromStringLiteral;
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromNullLiteral;
    }
    void PostBooleanLiteral(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromBooleanLiteral_value = std::move(stack.top());
            context.fromBooleanLiteral = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromBooleanLiteral_value.get());
            stack.pop();
        }
    }
    void PostFloatingLiteral(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFloatingLiteral_value = std::move(stack.top());
            context.fromFloatingLiteral = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromFloatingLiteral_value.get());
            stack.pop();
        }
    }
    void PostIntegerLiteral(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIntegerLiteral_value = std::move(stack.top());
            context.fromIntegerLiteral = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromIntegerLiteral_value.get());
            stack.pop();
        }
    }
    void PostCharLiteral(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCharLiteral_value = std::move(stack.top());
            context.fromCharLiteral = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromCharLiteral_value.get());
            stack.pop();
        }
    }
    void PostStringLiteral(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStringLiteral_value = std::move(stack.top());
            context.fromStringLiteral = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromStringLiteral_value.get());
            stack.pop();
        }
    }
    void PostNullLiteral(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromNullLiteral_value = std::move(stack.top());
            context.fromNullLiteral = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromNullLiteral_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromBooleanLiteral(), fromFloatingLiteral(), fromIntegerLiteral(), fromCharLiteral(), fromStringLiteral(), fromNullLiteral() {}
        Cm::Ast::Node* value;
        Cm::Ast::Node* fromBooleanLiteral;
        Cm::Ast::Node* fromFloatingLiteral;
        Cm::Ast::Node* fromIntegerLiteral;
        Cm::Ast::Node* fromCharLiteral;
        Cm::Ast::Node* fromStringLiteral;
        Cm::Ast::Node* fromNullLiteral;
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
        SetValueTypeName("Cm::Ast::Node*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<BooleanLiteralRule>(this, &BooleanLiteralRule::A0Action));
        Cm::Parsing::NonterminalParser* bNonterminalParser = GetNonterminal("b");
        bNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<BooleanLiteralRule>(this, &BooleanLiteralRule::Postb));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new BooleanLiteralNode(span, context.fromb);
    }
    void Postb(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromb_value = std::move(stack.top());
            context.fromb = *static_cast<Cm::Parsing::ValueObject<bool>*>(fromb_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromb() {}
        Cm::Ast::Node* value;
        bool fromb;
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
        SetValueTypeName("Cm::Ast::Node*");
        AddLocalVariable(AttrOrVariable("int", "start"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
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
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A5Action));
        Cm::Parsing::NonterminalParser* hex_literalNonterminalParser = GetNonterminal("hex_literal");
        hex_literalNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IntegerLiteralRule>(this, &IntegerLiteralRule::Posthex_literal));
        Cm::Parsing::NonterminalParser* ulongNonterminalParser = GetNonterminal("ulong");
        ulongNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IntegerLiteralRule>(this, &IntegerLiteralRule::Postulong));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.start = span.Start();
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CreateIntegerLiteralNode(Span(span.FileIndex(), span.LineNumber(), context.start, span.End()), context.fromhex_literal, true);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CreateIntegerLiteralNode(Span(span.FileIndex(), span.LineNumber(), context.start, span.End()), context.fromhex_literal, false);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.start = span.Start();
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CreateIntegerLiteralNode(Span(span.FileIndex(), span.LineNumber(), context.start, span.End()), context.fromulong, true);
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CreateIntegerLiteralNode(Span(span.FileIndex(), span.LineNumber(), context.start, span.End()), context.fromulong, false);
    }
    void Posthex_literal(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromhex_literal_value = std::move(stack.top());
            context.fromhex_literal = *static_cast<Cm::Parsing::ValueObject<uint64_t>*>(fromhex_literal_value.get());
            stack.pop();
        }
    }
    void Postulong(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromulong_value = std::move(stack.top());
            context.fromulong = *static_cast<Cm::Parsing::ValueObject<uint64_t>*>(fromulong_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), start(), fromhex_literal(), fromulong() {}
        Cm::Ast::Node* value;
        int start;
        uint64_t fromhex_literal;
        uint64_t fromulong;
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
        SetValueTypeName("Cm::Ast::Node*");
        AddLocalVariable(AttrOrVariable("int", "start"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
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
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<FloatingLiteralRule>(this, &FloatingLiteralRule::A2Action));
        Cm::Parsing::NonterminalParser* urealNonterminalParser = GetNonterminal("ureal");
        urealNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<FloatingLiteralRule>(this, &FloatingLiteralRule::Postureal));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.start = span.Start();
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CreateFloatingLiteralNode(Span(span.FileIndex(), span.LineNumber(), context.start, span.End()), context.fromureal, true);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = CreateFloatingLiteralNode(Span(span.FileIndex(), span.LineNumber(), context.start, span.End()), context.fromureal, false);
    }
    void Postureal(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromureal_value = std::move(stack.top());
            context.fromureal = *static_cast<Cm::Parsing::ValueObject<double>*>(fromureal_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), start(), fromureal() {}
        Cm::Ast::Node* value;
        int start;
        double fromureal;
    };
    std::stack<Context> contextStack;
    Context context;
};

class LiteralGrammar::CharLiteralRule : public Cm::Parsing::Rule
{
public:
    CharLiteralRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::Node*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CharLiteralRule>(this, &CharLiteralRule::A0Action));
        Cm::Parsing::NonterminalParser* cNonterminalParser = GetNonterminal("c");
        cNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CharLiteralRule>(this, &CharLiteralRule::Postc));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new CharLiteralNode(span, context.fromc);
    }
    void Postc(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromc_value = std::move(stack.top());
            context.fromc = *static_cast<Cm::Parsing::ValueObject<char>*>(fromc_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromc() {}
        Cm::Ast::Node* value;
        char fromc;
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
        SetValueTypeName("Cm::Ast::Node*");
        AddLocalVariable(AttrOrVariable("std::string", "r"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A2Action));
        Cm::Parsing::NonterminalParser* sNonterminalParser = GetNonterminal("s");
        sNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StringLiteralRule>(this, &StringLiteralRule::Posts));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new StringLiteralNode(span, context.froms);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new StringLiteralNode(span, context.r);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.r = std::string(matchBegin, matchEnd);
    }
    void Posts(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> froms_value = std::move(stack.top());
            context.froms = *static_cast<Cm::Parsing::ValueObject<std::string>*>(froms_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), r(), froms() {}
        Cm::Ast::Node* value;
        std::string r;
        std::string froms;
    };
    std::stack<Context> contextStack;
    Context context;
};

class LiteralGrammar::NullLiteralRule : public Cm::Parsing::Rule
{
public:
    NullLiteralRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::Node*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NullLiteralRule>(this, &NullLiteralRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new NullLiteralNode(span);
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Ast::Node* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

void LiteralGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void LiteralGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("bool", this, "Cm.Parsing.stdlib.bool"));
    AddRuleLink(new Cm::Parsing::RuleLink("string", this, "Cm.Parsing.stdlib.string"));
    AddRuleLink(new Cm::Parsing::RuleLink("ureal", this, "Cm.Parsing.stdlib.ureal"));
    AddRuleLink(new Cm::Parsing::RuleLink("ulong", this, "Cm.Parsing.stdlib.ulong"));
    AddRuleLink(new Cm::Parsing::RuleLink("char", this, "Cm.Parsing.stdlib.char"));
    AddRuleLink(new Cm::Parsing::RuleLink("hex_literal", this, "Cm.Parsing.stdlib.hex_literal"));
    AddRule(new LiteralRule("Literal", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::ActionParser("A0",
                                new Cm::Parsing::NonterminalParser("BooleanLiteral", "BooleanLiteral", 0)),
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::NonterminalParser("FloatingLiteral", "FloatingLiteral", 0))),
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::NonterminalParser("IntegerLiteral", "IntegerLiteral", 0))),
                    new Cm::Parsing::ActionParser("A3",
                        new Cm::Parsing::NonterminalParser("CharLiteral", "CharLiteral", 0))),
                new Cm::Parsing::ActionParser("A4",
                    new Cm::Parsing::NonterminalParser("StringLiteral", "StringLiteral", 0))),
            new Cm::Parsing::ActionParser("A5",
                new Cm::Parsing::NonterminalParser("NullLiteral", "NullLiteral", 0)))));
    AddRule(new BooleanLiteralRule("BooleanLiteral", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("b", "bool", 0))));
    AddRule(new IntegerLiteralRule("IntegerLiteral", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::NonterminalParser("hex_literal", "hex_literal", 0)),
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::AlternativeParser(
                                new Cm::Parsing::CharParser('u'),
                                new Cm::Parsing::CharParser('U'))),
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::EmptyParser())))),
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::ActionParser("A3",
                        new Cm::Parsing::NonterminalParser("ulong", "ulong", 0)),
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::ActionParser("A4",
                            new Cm::Parsing::AlternativeParser(
                                new Cm::Parsing::CharParser('u'),
                                new Cm::Parsing::CharParser('U'))),
                        new Cm::Parsing::ActionParser("A5",
                            new Cm::Parsing::EmptyParser())))))));
    AddRule(new FloatingLiteralRule("FloatingLiteral", GetScope(),
        new Cm::Parsing::TokenParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("ureal", "ureal", 0)),
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::CharParser('f'),
                            new Cm::Parsing::CharParser('F'))),
                    new Cm::Parsing::ActionParser("A2",
                        new Cm::Parsing::EmptyParser()))))));
    AddRule(new CharLiteralRule("CharLiteral", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("c", "char", 0))));
    AddRule(new StringLiteralRule("StringLiteral", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("s", "string", 0)),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('@'),
                    new Cm::Parsing::TokenParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::CharParser('\"'),
                                new Cm::Parsing::ActionParser("A2",
                                    new Cm::Parsing::KleeneStarParser(
                                        new Cm::Parsing::CharSetParser("\"", true)))),
                            new Cm::Parsing::CharParser('\"'))))))));
    AddRule(new NullLiteralRule("NullLiteral", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::KeywordParser("null"))));
    SetStartRuleName("IntegerLiteral");
}

} } // namespace Cm.Parser
