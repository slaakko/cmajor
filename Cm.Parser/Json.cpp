#include "Json.hpp"
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

using namespace Cm::Parsing;

JsonGrammar* JsonGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

JsonGrammar* JsonGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    JsonGrammar* grammar(new JsonGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

JsonGrammar::JsonGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("JsonGrammar", parsingDomain_->GetNamespaceScope("Cm.Parser"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Core::JsonValue* JsonGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
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
    if (!match.Hit() || !CC() && stop.Start() != int(end - start))
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
    Cm::Core::JsonValue* result = *static_cast<Cm::Parsing::ValueObject<Cm::Core::JsonValue*>*>(value.get());
    stack.pop();
    return result;
}

class JsonGrammar::ValueRule : public Cm::Parsing::Rule
{
public:
    ValueRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Core::JsonValue*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Core::JsonValue*>(context.value)));
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
        Cm::Parsing::NonterminalParser* stringNonterminalParser = GetNonterminal("String");
        stringNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ValueRule>(this, &ValueRule::PostString));
        Cm::Parsing::NonterminalParser* numberNonterminalParser = GetNonterminal("Number");
        numberNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ValueRule>(this, &ValueRule::PostNumber));
        Cm::Parsing::NonterminalParser* objectNonterminalParser = GetNonterminal("Object");
        objectNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ValueRule>(this, &ValueRule::PostObject));
        Cm::Parsing::NonterminalParser* arrayNonterminalParser = GetNonterminal("Array");
        arrayNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ValueRule>(this, &ValueRule::PostArray));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromString;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromNumber;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromObject;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromArray;
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Core::JsonBool(true);
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Core::JsonBool(false);
    }
    void A6Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Core::JsonNull;
    }
    void PostString(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromString_value = std::move(stack.top());
            context.fromString = *static_cast<Cm::Parsing::ValueObject<Cm::Core::JsonString*>*>(fromString_value.get());
            stack.pop();
        }
    }
    void PostNumber(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromNumber_value = std::move(stack.top());
            context.fromNumber = *static_cast<Cm::Parsing::ValueObject<Cm::Core::JsonNumber*>*>(fromNumber_value.get());
            stack.pop();
        }
    }
    void PostObject(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromObject_value = std::move(stack.top());
            context.fromObject = *static_cast<Cm::Parsing::ValueObject<Cm::Core::JsonObject*>*>(fromObject_value.get());
            stack.pop();
        }
    }
    void PostArray(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromArray_value = std::move(stack.top());
            context.fromArray = *static_cast<Cm::Parsing::ValueObject<Cm::Core::JsonArray*>*>(fromArray_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromString(), fromNumber(), fromObject(), fromArray() {}
        Cm::Core::JsonValue* value;
        Cm::Core::JsonString* fromString;
        Cm::Core::JsonNumber* fromNumber;
        Cm::Core::JsonObject* fromObject;
        Cm::Core::JsonArray* fromArray;
    };
    std::stack<Context> contextStack;
    Context context;
};

class JsonGrammar::ObjectRule : public Cm::Parsing::Rule
{
public:
    ObjectRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Core::JsonObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Core::JsonObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ObjectRule>(this, &ObjectRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ObjectRule>(this, &ObjectRule::A1Action));
        Cm::Parsing::NonterminalParser* nameNonterminalParser = GetNonterminal("name");
        nameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ObjectRule>(this, &ObjectRule::Postname));
        Cm::Parsing::NonterminalParser* valNonterminalParser = GetNonterminal("val");
        valNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ObjectRule>(this, &ObjectRule::Postval));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Core::JsonObject;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->AddField(std::move(*context.fromname), context.fromval);
        delete context.fromname;
    }
    void Postname(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromname_value = std::move(stack.top());
            context.fromname = *static_cast<Cm::Parsing::ValueObject<Cm::Core::JsonString*>*>(fromname_value.get());
            stack.pop();
        }
    }
    void Postval(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromval_value = std::move(stack.top());
            context.fromval = *static_cast<Cm::Parsing::ValueObject<Cm::Core::JsonValue*>*>(fromval_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromname(), fromval() {}
        Cm::Core::JsonObject* value;
        Cm::Core::JsonString* fromname;
        Cm::Core::JsonValue* fromval;
    };
    std::stack<Context> contextStack;
    Context context;
};

class JsonGrammar::ArrayRule : public Cm::Parsing::Rule
{
public:
    ArrayRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Core::JsonArray*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Core::JsonArray*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ArrayRule>(this, &ArrayRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ArrayRule>(this, &ArrayRule::A1Action));
        Cm::Parsing::NonterminalParser* itemNonterminalParser = GetNonterminal("item");
        itemNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ArrayRule>(this, &ArrayRule::Postitem));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Core::JsonArray;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->AddItem(context.fromitem);
    }
    void Postitem(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromitem_value = std::move(stack.top());
            context.fromitem = *static_cast<Cm::Parsing::ValueObject<Cm::Core::JsonValue*>*>(fromitem_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromitem() {}
        Cm::Core::JsonArray* value;
        Cm::Core::JsonValue* fromitem;
    };
    std::stack<Context> contextStack;
    Context context;
};

class JsonGrammar::StringRule : public Cm::Parsing::Rule
{
public:
    StringRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Core::JsonString*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Core::JsonString*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StringRule>(this, &StringRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StringRule>(this, &StringRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StringRule>(this, &StringRule::A2Action));
        Cm::Parsing::NonterminalParser* escapeNonterminalParser = GetNonterminal("escape");
        escapeNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StringRule>(this, &StringRule::Postescape));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Core::JsonString;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->Append(*matchBegin);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->Append(context.fromescape);
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
        Cm::Core::JsonString* value;
        char fromescape;
    };
    std::stack<Context> contextStack;
    Context context;
};

class JsonGrammar::NumberRule : public Cm::Parsing::Rule
{
public:
    NumberRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Core::JsonNumber*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Core::JsonNumber*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NumberRule>(this, &NumberRule::A0Action));
        Cm::Parsing::NonterminalParser* numberNonterminalParser = GetNonterminal("number");
        numberNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NumberRule>(this, &NumberRule::Postnumber));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Core::JsonNumber(context.fromnumber);
    }
    void Postnumber(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromnumber_value = std::move(stack.top());
            context.fromnumber = *static_cast<Cm::Parsing::ValueObject<double>*>(fromnumber_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromnumber() {}
        Cm::Core::JsonNumber* value;
        double fromnumber;
    };
    std::stack<Context> contextStack;
    Context context;
};

void JsonGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void JsonGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("spaces", this, "Cm.Parsing.stdlib.spaces"));
    AddRuleLink(new Cm::Parsing::RuleLink("escape", this, "Cm.Parsing.stdlib.escape"));
    AddRuleLink(new Cm::Parsing::RuleLink("number", this, "Cm.Parsing.stdlib.number"));
    AddRule(new ValueRule("Value", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::AlternativeParser(
                                new Cm::Parsing::ActionParser("A0",
                                    new Cm::Parsing::NonterminalParser("String", "String", 0)),
                                new Cm::Parsing::ActionParser("A1",
                                    new Cm::Parsing::NonterminalParser("Number", "Number", 0))),
                            new Cm::Parsing::ActionParser("A2",
                                new Cm::Parsing::NonterminalParser("Object", "Object", 0))),
                        new Cm::Parsing::ActionParser("A3",
                            new Cm::Parsing::NonterminalParser("Array", "Array", 0))),
                    new Cm::Parsing::ActionParser("A4",
                        new Cm::Parsing::KeywordParser("true"))),
                new Cm::Parsing::ActionParser("A5",
                    new Cm::Parsing::KeywordParser("false"))),
            new Cm::Parsing::ActionParser("A6",
                new Cm::Parsing::KeywordParser("null")))));
    AddRule(new ObjectRule("Object", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::CharParser('{')),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::ListParser(
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::NonterminalParser("name", "String", 0),
                                    new Cm::Parsing::CharParser(':')),
                                new Cm::Parsing::NonterminalParser("val", "Value", 0))),
                        new Cm::Parsing::CharParser(',')))),
            new Cm::Parsing::CharParser('}'))));
    AddRule(new ArrayRule("Array", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::CharParser('[')),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::ListParser(
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::NonterminalParser("item", "Value", 0)),
                        new Cm::Parsing::CharParser(',')))),
            new Cm::Parsing::CharParser(']'))));
    AddRule(new StringRule("String", GetScope(),
        new Cm::Parsing::TokenParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::CharParser('\"')),
                    new Cm::Parsing::KleeneStarParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::CharSetParser("\"\\", true)),
                            new Cm::Parsing::ActionParser("A2",
                                new Cm::Parsing::NonterminalParser("escape", "escape", 0))))),
                new Cm::Parsing::CharParser('\"')))));
    AddRule(new NumberRule("Number", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("number", "number", 0))));
    SetSkipRuleName("spaces");
}

} } // namespace Cm.Parser
