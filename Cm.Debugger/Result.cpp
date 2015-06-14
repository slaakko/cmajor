#include "Result.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>

namespace Cm { namespace Debugger {

using namespace Cm::Parsing;

ResultGrammar* ResultGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

ResultGrammar* ResultGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    ResultGrammar* grammar(new ResultGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

ResultGrammar::ResultGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("ResultGrammar", parsingDomain_->GetNamespaceScope("Cm.Debugger"), parsingDomain_)
{
    SetOwner(0);
}

Result* ResultGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, std::string expr)
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
    stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<std::string>(expr)));
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
    Result* result = *static_cast<Cm::Parsing::ValueObject<Result*>*>(value.get());
    stack.pop();
    return result;
}

class ResultGrammar::ResultRule : public Cm::Parsing::Rule
{
public:
    ResultRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("std::string", "expr"));
        SetValueTypeName("Result*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> expr_value = std::move(stack.top());
        context.expr = *static_cast<Cm::Parsing::ValueObject<std::string>*>(expr_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Result*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ResultRule>(this, &ResultRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ResultRule>(this, &ResultRule::A1Action));
        Cm::Parsing::NonterminalParser* handleNonterminalParser = GetNonterminal("handle");
        handleNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ResultRule>(this, &ResultRule::Posthandle));
        Cm::Parsing::NonterminalParser* valueNonterminalParser = GetNonterminal("Value");
        valueNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ResultRule>(this, &ResultRule::PostValue));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->SetValue(context.fromValue);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Result(context.expr, context.fromhandle);
    }
    void Posthandle(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromhandle_value = std::move(stack.top());
            context.fromhandle = *static_cast<Cm::Parsing::ValueObject<int>*>(fromhandle_value.get());
            stack.pop();
        }
    }
    void PostValue(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromValue_value = std::move(stack.top());
            context.fromValue = *static_cast<Cm::Parsing::ValueObject<Value*>*>(fromValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): expr(), value(), fromhandle(), fromValue() {}
        std::string expr;
        Result* value;
        int fromhandle;
        Value* fromValue;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ResultGrammar::ValueRule : public Cm::Parsing::Rule
{
public:
    ValueRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Value*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Value*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ValueRule>(this, &ValueRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ValueRule>(this, &ValueRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ValueRule>(this, &ValueRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ValueRule>(this, &ValueRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ValueRule>(this, &ValueRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ValueRule>(this, &ValueRule::A5Action));
        Cm::Parsing::ActionParser* a6ActionParser = GetAction("A6");
        a6ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ValueRule>(this, &ValueRule::A6Action));
        Cm::Parsing::NonterminalParser* stringValueNonterminalParser = GetNonterminal("StringValue");
        stringValueNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ValueRule>(this, &ValueRule::PostStringValue));
        Cm::Parsing::NonterminalParser* addressValueNonterminalParser = GetNonterminal("AddressValue");
        addressValueNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ValueRule>(this, &ValueRule::PostAddressValue));
        Cm::Parsing::NonterminalParser* charValueNonterminalParser = GetNonterminal("CharValue");
        charValueNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ValueRule>(this, &ValueRule::PostCharValue));
        Cm::Parsing::NonterminalParser* boolValueNonterminalParser = GetNonterminal("BoolValue");
        boolValueNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ValueRule>(this, &ValueRule::PostBoolValue));
        Cm::Parsing::NonterminalParser* floatingValueNonterminalParser = GetNonterminal("FloatingValue");
        floatingValueNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ValueRule>(this, &ValueRule::PostFloatingValue));
        Cm::Parsing::NonterminalParser* integerValueNonterminalParser = GetNonterminal("IntegerValue");
        integerValueNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ValueRule>(this, &ValueRule::PostIntegerValue));
        Cm::Parsing::NonterminalParser* structureValueNonterminalParser = GetNonterminal("StructureValue");
        structureValueNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ValueRule>(this, &ValueRule::PostStructureValue));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromStringValue;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromAddressValue;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromCharValue;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromBoolValue;
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromFloatingValue;
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromIntegerValue;
    }
    void A6Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromStructureValue;
    }
    void PostStringValue(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStringValue_value = std::move(stack.top());
            context.fromStringValue = *static_cast<Cm::Parsing::ValueObject<Value*>*>(fromStringValue_value.get());
            stack.pop();
        }
    }
    void PostAddressValue(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAddressValue_value = std::move(stack.top());
            context.fromAddressValue = *static_cast<Cm::Parsing::ValueObject<Value*>*>(fromAddressValue_value.get());
            stack.pop();
        }
    }
    void PostCharValue(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCharValue_value = std::move(stack.top());
            context.fromCharValue = *static_cast<Cm::Parsing::ValueObject<Value*>*>(fromCharValue_value.get());
            stack.pop();
        }
    }
    void PostBoolValue(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromBoolValue_value = std::move(stack.top());
            context.fromBoolValue = *static_cast<Cm::Parsing::ValueObject<Value*>*>(fromBoolValue_value.get());
            stack.pop();
        }
    }
    void PostFloatingValue(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFloatingValue_value = std::move(stack.top());
            context.fromFloatingValue = *static_cast<Cm::Parsing::ValueObject<Value*>*>(fromFloatingValue_value.get());
            stack.pop();
        }
    }
    void PostIntegerValue(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIntegerValue_value = std::move(stack.top());
            context.fromIntegerValue = *static_cast<Cm::Parsing::ValueObject<Value*>*>(fromIntegerValue_value.get());
            stack.pop();
        }
    }
    void PostStructureValue(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStructureValue_value = std::move(stack.top());
            context.fromStructureValue = *static_cast<Cm::Parsing::ValueObject<StructureValue*>*>(fromStructureValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromStringValue(), fromAddressValue(), fromCharValue(), fromBoolValue(), fromFloatingValue(), fromIntegerValue(), fromStructureValue() {}
        Value* value;
        Value* fromStringValue;
        Value* fromAddressValue;
        Value* fromCharValue;
        Value* fromBoolValue;
        Value* fromFloatingValue;
        Value* fromIntegerValue;
        StructureValue* fromStructureValue;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ResultGrammar::StringValueRule : public Cm::Parsing::Rule
{
public:
    StringValueRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Value*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Value*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StringValueRule>(this, &StringValueRule::A0Action));
        Cm::Parsing::NonterminalParser* xNonterminalParser = GetNonterminal("x");
        xNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StringValueRule>(this, &StringValueRule::Postx));
        Cm::Parsing::NonterminalParser* sNonterminalParser = GetNonterminal("s");
        sNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StringValueRule>(this, &StringValueRule::Posts));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new StringValue(context.froms);
    }
    void Postx(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromx_value = std::move(stack.top());
            context.fromx = *static_cast<Cm::Parsing::ValueObject<uint64_t>*>(fromx_value.get());
            stack.pop();
        }
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
        Context(): value(), fromx(), froms() {}
        Value* value;
        uint64_t fromx;
        std::string froms;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ResultGrammar::AddressValueRule : public Cm::Parsing::Rule
{
public:
    AddressValueRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Value*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Value*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AddressValueRule>(this, &AddressValueRule::A0Action));
        Cm::Parsing::NonterminalParser* xNonterminalParser = GetNonterminal("x");
        xNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AddressValueRule>(this, &AddressValueRule::Postx));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AddressValueRule>(this, &AddressValueRule::Postidentifier));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new AddressValue(context.fromx);
    }
    void Postx(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromx_value = std::move(stack.top());
            context.fromx = *static_cast<Cm::Parsing::ValueObject<uint64_t>*>(fromx_value.get());
            stack.pop();
        }
    }
    void Postidentifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromidentifier_value = std::move(stack.top());
            context.fromidentifier = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromidentifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromx(), fromidentifier() {}
        Value* value;
        uint64_t fromx;
        std::string fromidentifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ResultGrammar::OctalDigitRule : public Cm::Parsing::Rule
{
public:
    OctalDigitRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("int");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<int>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<OctalDigitRule>(this, &OctalDigitRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = *matchBegin - '0';
    }
private:
    struct Context
    {
        Context(): value() {}
        int value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ResultGrammar::OctalEscapeRule : public Cm::Parsing::Rule
{
public:
    OctalEscapeRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<OctalEscapeRule>(this, &OctalEscapeRule::A0Action));
        Cm::Parsing::NonterminalParser* xNonterminalParser = GetNonterminal("x");
        xNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<OctalEscapeRule>(this, &OctalEscapeRule::Postx));
        Cm::Parsing::NonterminalParser* yNonterminalParser = GetNonterminal("y");
        yNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<OctalEscapeRule>(this, &OctalEscapeRule::Posty));
        Cm::Parsing::NonterminalParser* zNonterminalParser = GetNonterminal("z");
        zNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<OctalEscapeRule>(this, &OctalEscapeRule::Postz));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = static_cast<char>(64 * context.fromx + 8 * context.fromy + context.fromz);
    }
    void Postx(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromx_value = std::move(stack.top());
            context.fromx = *static_cast<Cm::Parsing::ValueObject<int>*>(fromx_value.get());
            stack.pop();
        }
    }
    void Posty(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromy_value = std::move(stack.top());
            context.fromy = *static_cast<Cm::Parsing::ValueObject<int>*>(fromy_value.get());
            stack.pop();
        }
    }
    void Postz(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromz_value = std::move(stack.top());
            context.fromz = *static_cast<Cm::Parsing::ValueObject<int>*>(fromz_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromx(), fromy(), fromz() {}
        char value;
        int fromx;
        int fromy;
        int fromz;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ResultGrammar::CharValueRule : public Cm::Parsing::Rule
{
public:
    CharValueRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Value*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Value*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CharValueRule>(this, &CharValueRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CharValueRule>(this, &CharValueRule::A1Action));
        Cm::Parsing::NonterminalParser* charCodeNonterminalParser = GetNonterminal("charCode");
        charCodeNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CharValueRule>(this, &CharValueRule::PostcharCode));
        Cm::Parsing::NonterminalParser* oNonterminalParser = GetNonterminal("o");
        oNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CharValueRule>(this, &CharValueRule::Posto));
        Cm::Parsing::NonterminalParser* chNonterminalParser = GetNonterminal("ch");
        chNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CharValueRule>(this, &CharValueRule::Postch));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new CharValue(context.fromo);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new CharValue(context.fromch);
    }
    void PostcharCode(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromcharCode_value = std::move(stack.top());
            context.fromcharCode = *static_cast<Cm::Parsing::ValueObject<int>*>(fromcharCode_value.get());
            stack.pop();
        }
    }
    void Posto(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromo_value = std::move(stack.top());
            context.fromo = *static_cast<Cm::Parsing::ValueObject<char>*>(fromo_value.get());
            stack.pop();
        }
    }
    void Postch(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromch_value = std::move(stack.top());
            context.fromch = *static_cast<Cm::Parsing::ValueObject<char>*>(fromch_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromcharCode(), fromo(), fromch() {}
        Value* value;
        int fromcharCode;
        char fromo;
        char fromch;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ResultGrammar::BoolValueRule : public Cm::Parsing::Rule
{
public:
    BoolValueRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Value*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Value*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<BoolValueRule>(this, &BoolValueRule::A0Action));
        Cm::Parsing::NonterminalParser* bNonterminalParser = GetNonterminal("b");
        bNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<BoolValueRule>(this, &BoolValueRule::Postb));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new BoolValue(context.fromb);
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
        Value* value;
        bool fromb;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ResultGrammar::FloatingValueRule : public Cm::Parsing::Rule
{
public:
    FloatingValueRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Value*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Value*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<FloatingValueRule>(this, &FloatingValueRule::A0Action));
        Cm::Parsing::NonterminalParser* rNonterminalParser = GetNonterminal("r");
        rNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<FloatingValueRule>(this, &FloatingValueRule::Postr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new FloatingValue(context.fromr);
    }
    void Postr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromr_value = std::move(stack.top());
            context.fromr = *static_cast<Cm::Parsing::ValueObject<double>*>(fromr_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromr() {}
        Value* value;
        double fromr;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ResultGrammar::IntegerValueRule : public Cm::Parsing::Rule
{
public:
    IntegerValueRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Value*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Value*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IntegerValueRule>(this, &IntegerValueRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IntegerValueRule>(this, &IntegerValueRule::A1Action));
        Cm::Parsing::NonterminalParser* uNonterminalParser = GetNonterminal("u");
        uNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IntegerValueRule>(this, &IntegerValueRule::Postu));
        Cm::Parsing::NonterminalParser* lNonterminalParser = GetNonterminal("l");
        lNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IntegerValueRule>(this, &IntegerValueRule::Postl));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new IntegerValue(context.fromu, false);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = MakeIntegerValue(context.froml);
    }
    void Postu(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromu_value = std::move(stack.top());
            context.fromu = *static_cast<Cm::Parsing::ValueObject<uint64_t>*>(fromu_value.get());
            stack.pop();
        }
    }
    void Postl(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> froml_value = std::move(stack.top());
            context.froml = *static_cast<Cm::Parsing::ValueObject<int64_t>*>(froml_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromu(), froml() {}
        Value* value;
        uint64_t fromu;
        int64_t froml;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ResultGrammar::FieldRule : public Cm::Parsing::Rule
{
public:
    FieldRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Field*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Field*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<FieldRule>(this, &FieldRule::A0Action));
        Cm::Parsing::NonterminalParser* fieldNameNonterminalParser = GetNonterminal("fieldName");
        fieldNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<FieldRule>(this, &FieldRule::PostfieldName));
        Cm::Parsing::NonterminalParser* fieldValueNonterminalParser = GetNonterminal("fieldValue");
        fieldValueNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<FieldRule>(this, &FieldRule::PostfieldValue));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Field(context.fromfieldName, context.fromfieldValue);
    }
    void PostfieldName(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromfieldName_value = std::move(stack.top());
            context.fromfieldName = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromfieldName_value.get());
            stack.pop();
        }
    }
    void PostfieldValue(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromfieldValue_value = std::move(stack.top());
            context.fromfieldValue = *static_cast<Cm::Parsing::ValueObject<Value*>*>(fromfieldValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromfieldName(), fromfieldValue() {}
        Field* value;
        std::string fromfieldName;
        Value* fromfieldValue;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ResultGrammar::StructureValueRule : public Cm::Parsing::Rule
{
public:
    StructureValueRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("StructureValue*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<StructureValue*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StructureValueRule>(this, &StructureValueRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StructureValueRule>(this, &StructureValueRule::A1Action));
        Cm::Parsing::NonterminalParser* fieldNonterminalParser = GetNonterminal("Field");
        fieldNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StructureValueRule>(this, &StructureValueRule::PostField));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new StructureValue;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->AddField(std::move(*context.fromField));
        delete context.fromField;
    }
    void PostField(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromField_value = std::move(stack.top());
            context.fromField = *static_cast<Cm::Parsing::ValueObject<Field*>*>(fromField_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromField() {}
        StructureValue* value;
        Field* fromField;
    };
    std::stack<Context> contextStack;
    Context context;
};

void ResultGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void ResultGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("spaces", this, "Cm.Parsing.stdlib.spaces"));
    AddRuleLink(new Cm::Parsing::RuleLink("real", this, "Cm.Parsing.stdlib.real"));
    AddRuleLink(new Cm::Parsing::RuleLink("long", this, "Cm.Parsing.stdlib.long"));
    AddRuleLink(new Cm::Parsing::RuleLink("int", this, "Cm.Parsing.stdlib.int"));
    AddRuleLink(new Cm::Parsing::RuleLink("hex_literal", this, "Cm.Parsing.stdlib.hex_literal"));
    AddRuleLink(new Cm::Parsing::RuleLink("bool", this, "Cm.Parsing.stdlib.bool"));
    AddRuleLink(new Cm::Parsing::RuleLink("char", this, "Cm.Parsing.stdlib.char"));
    AddRuleLink(new Cm::Parsing::RuleLink("ulong", this, "Cm.Parsing.stdlib.ulong"));
    AddRuleLink(new Cm::Parsing::RuleLink("string", this, "Cm.Parsing.stdlib.string"));
    AddRuleLink(new Cm::Parsing::RuleLink("identifier", this, "Cm.Parsing.stdlib.identifier"));
    AddRule(new ResultRule("Result", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::TokenParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::CharParser('$'),
                                new Cm::Parsing::NonterminalParser("handle", "int", 0)))),
                    new Cm::Parsing::CharParser('=')),
                new Cm::Parsing::NonterminalParser("Value", "Value", 0)))));
    AddRule(new ValueRule("Value", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::AlternativeParser(
                                new Cm::Parsing::ActionParser("A0",
                                    new Cm::Parsing::NonterminalParser("StringValue", "StringValue", 0)),
                                new Cm::Parsing::ActionParser("A1",
                                    new Cm::Parsing::NonterminalParser("AddressValue", "AddressValue", 0))),
                            new Cm::Parsing::ActionParser("A2",
                                new Cm::Parsing::NonterminalParser("CharValue", "CharValue", 0))),
                        new Cm::Parsing::ActionParser("A3",
                            new Cm::Parsing::NonterminalParser("BoolValue", "BoolValue", 0))),
                    new Cm::Parsing::ActionParser("A4",
                        new Cm::Parsing::NonterminalParser("FloatingValue", "FloatingValue", 0))),
                new Cm::Parsing::ActionParser("A5",
                    new Cm::Parsing::NonterminalParser("IntegerValue", "IntegerValue", 0))),
            new Cm::Parsing::ActionParser("A6",
                new Cm::Parsing::NonterminalParser("StructureValue", "StructureValue", 0)))));
    AddRule(new StringValueRule("StringValue", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::NonterminalParser("x", "hex_literal", 0),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("s", "string", 0)))));
    AddRule(new Cm::Parsing::Rule("ParenthesizedExpr", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('('),
                    new Cm::Parsing::KleeneStarParser(
                        new Cm::Parsing::DifferenceParser(
                            new Cm::Parsing::AnyCharParser(),
                            new Cm::Parsing::CharSetParser("()")))),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::NonterminalParser("ParenthesizedExpr", "ParenthesizedExpr", 0))),
            new Cm::Parsing::CharParser(')'))));
    AddRule(new AddressValueRule("AddressValue", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::NonterminalParser("ParenthesizedExpr", "ParenthesizedExpr", 0)),
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("x", "hex_literal", 0))),
            new Cm::Parsing::OptionalParser(
                new Cm::Parsing::TokenParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::CharParser('<'),
                            new Cm::Parsing::NonterminalParser("identifier", "identifier", 0)),
                        new Cm::Parsing::CharParser('>')))))));
    AddRule(new OctalDigitRule("OctalDigit", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::CharSetParser("0-7"))));
    AddRule(new OctalEscapeRule("OctalEscape", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::StringParser("\'\\"),
                                new Cm::Parsing::NonterminalParser("x", "OctalDigit", 0)),
                            new Cm::Parsing::NonterminalParser("y", "OctalDigit", 0)),
                        new Cm::Parsing::NonterminalParser("z", "OctalDigit", 0)),
                    new Cm::Parsing::StringParser("\'"))))));
    AddRule(new CharValueRule("CharValue", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::NonterminalParser("charCode", "int", 0),
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("o", "OctalEscape", 0)),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::NonterminalParser("ch", "char", 0))))));
    AddRule(new BoolValueRule("BoolValue", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("b", "bool", 0))));
    AddRule(new FloatingValueRule("FloatingValue", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("r", "real", 0))));
    AddRule(new IntegerValueRule("IntegerValue", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("u", "ulong", 0)),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("l", "long", 0)))));
    AddRule(new FieldRule("Field", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::NonterminalParser("fieldName", "identifier", 0),
                    new Cm::Parsing::CharParser('=')),
                new Cm::Parsing::NonterminalParser("fieldValue", "Value", 0)))));
    AddRule(new StructureValueRule("StructureValue", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::CharParser('{')),
                new Cm::Parsing::ListParser(
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("Field", "Field", 0)),
                    new Cm::Parsing::CharParser(','))),
            new Cm::Parsing::CharParser('}'))));
    SetSkipRuleName("spaces");
}

} } // namespace Cm.Debugger
