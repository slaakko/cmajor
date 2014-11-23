#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <sstream>

namespace Cm { namespace Parsing {

using namespace Cm::Parsing;

stdlib* stdlib::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

stdlib* stdlib::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    stdlib* grammar(new stdlib(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

stdlib::stdlib(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("stdlib", parsingDomain_->GetNamespaceScope("Cm.Parsing"), parsingDomain_)
{
    SetOwner(0);
}

class stdlib::intRule : public Cm::Parsing::Rule
{
public:
    intRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<intRule>(this, &intRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        std::stringstream s;
        s.str(std::string(matchBegin, matchEnd));
        s >> context.value;
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

class stdlib::uintRule : public Cm::Parsing::Rule
{
public:
    uintRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("unsigned");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<unsigned>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<uintRule>(this, &uintRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        std::stringstream s;
        s.str(std::string(matchBegin, matchEnd));
        s >> context.value;
    }
private:
    struct Context
    {
        Context(): value() {}
        unsigned value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class stdlib::longRule : public Cm::Parsing::Rule
{
public:
    longRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("int64_t");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<int64_t>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<longRule>(this, &longRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        std::stringstream s;
        s.str(std::string(matchBegin, matchEnd));
        s >> context.value;
    }
private:
    struct Context
    {
        Context(): value() {}
        int64_t value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class stdlib::ulongRule : public Cm::Parsing::Rule
{
public:
    ulongRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("uint64_t");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<uint64_t>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ulongRule>(this, &ulongRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        std::stringstream s;
        s.str(std::string(matchBegin, matchEnd));
        s >> context.value;
    }
private:
    struct Context
    {
        Context(): value() {}
        uint64_t value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class stdlib::hexRule : public Cm::Parsing::Rule
{
public:
    hexRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("uint64_t");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<uint64_t>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<hexRule>(this, &hexRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        std::stringstream s;
        s.str(std::string(matchBegin, matchEnd));
        s >> std::hex >> context.value;
    }
private:
    struct Context
    {
        Context(): value() {}
        uint64_t value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class stdlib::hex_literalRule : public Cm::Parsing::Rule
{
public:
    hex_literalRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("uint64_t");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<uint64_t>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<hex_literalRule>(this, &hex_literalRule::A0Action));
        Cm::Parsing::NonterminalParser* hexNonterminalParser = GetNonterminal("hex");
        hexNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<hex_literalRule>(this, &hex_literalRule::Posthex));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromhex;
    }
    void Posthex(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromhex_value = std::move(stack.top());
            context.fromhex = *static_cast<Cm::Parsing::ValueObject<uint64_t>*>(fromhex_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromhex() {}
        uint64_t value;
        uint64_t fromhex;
    };
    std::stack<Context> contextStack;
    Context context;
};

class stdlib::realRule : public Cm::Parsing::Rule
{
public:
    realRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("double");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<double>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<realRule>(this, &realRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        std::stringstream s;
        s.str(std::string(matchBegin, matchEnd));
        s >> context.value;
    }
private:
    struct Context
    {
        Context(): value() {}
        double value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class stdlib::urealRule : public Cm::Parsing::Rule
{
public:
    urealRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("double");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<double>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<urealRule>(this, &urealRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        std::stringstream s;
        s.str(std::string(matchBegin, matchEnd));
        s >> context.value;
    }
private:
    struct Context
    {
        Context(): value() {}
        double value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class stdlib::numberRule : public Cm::Parsing::Rule
{
public:
    numberRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("double");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<double>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<numberRule>(this, &numberRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<numberRule>(this, &numberRule::A1Action));
        Cm::Parsing::NonterminalParser* rNonterminalParser = GetNonterminal("r");
        rNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<numberRule>(this, &numberRule::Postr));
        Cm::Parsing::NonterminalParser* iNonterminalParser = GetNonterminal("i");
        iNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<numberRule>(this, &numberRule::Posti));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromr;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromi;
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
    void Posti(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromi_value = std::move(stack.top());
            context.fromi = *static_cast<Cm::Parsing::ValueObject<int>*>(fromi_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromr(), fromi() {}
        double value;
        double fromr;
        int fromi;
    };
    std::stack<Context> contextStack;
    Context context;
};

class stdlib::boolRule : public Cm::Parsing::Rule
{
public:
    boolRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("bool");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<bool>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<boolRule>(this, &boolRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<boolRule>(this, &boolRule::A1Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = true;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = false;
    }
private:
    struct Context
    {
        Context(): value() {}
        bool value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class stdlib::identifierRule : public Cm::Parsing::Rule
{
public:
    identifierRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<identifierRule>(this, &identifierRule::A0Action));
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

class stdlib::qualified_idRule : public Cm::Parsing::Rule
{
public:
    qualified_idRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<qualified_idRule>(this, &qualified_idRule::A0Action));
        Cm::Parsing::NonterminalParser* firstNonterminalParser = GetNonterminal("first");
        firstNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<qualified_idRule>(this, &qualified_idRule::Postfirst));
        Cm::Parsing::NonterminalParser* restNonterminalParser = GetNonterminal("rest");
        restNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<qualified_idRule>(this, &qualified_idRule::Postrest));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = std::string(matchBegin, matchEnd);
    }
    void Postfirst(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromfirst_value = std::move(stack.top());
            context.fromfirst = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromfirst_value.get());
            stack.pop();
        }
    }
    void Postrest(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromrest_value = std::move(stack.top());
            context.fromrest = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromrest_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromfirst(), fromrest() {}
        std::string value;
        std::string fromfirst;
        std::string fromrest;
    };
    std::stack<Context> contextStack;
    Context context;
};

class stdlib::escapeRule : public Cm::Parsing::Rule
{
public:
    escapeRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<escapeRule>(this, &escapeRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<escapeRule>(this, &escapeRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<escapeRule>(this, &escapeRule::A2Action));
        Cm::Parsing::NonterminalParser* xNonterminalParser = GetNonterminal("x");
        xNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<escapeRule>(this, &escapeRule::Postx));
        Cm::Parsing::NonterminalParser* decimalEscapeNonterminalParser = GetNonterminal("decimalEscape");
        decimalEscapeNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<escapeRule>(this, &escapeRule::PostdecimalEscape));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = char(context.fromx);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = char(context.fromdecimalEscape);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        char c = *matchBegin;
        switch (c)
        {
            case 'a': context.value = '\a';
            break;
            case 'b': context.value = '\b';
            break;
            case 'f': context.value = '\f';
            break;
            case 'n': context.value = '\n';
            break;
            case 'r': context.value = '\r';
            break;
            case 't': context.value = '\t';
            break;
            case 'v': context.value = '\v';
            break;
            case '0': context.value = '\0';
            break;
            default: context.value = c;
            break;
        }
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
    void PostdecimalEscape(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromdecimalEscape_value = std::move(stack.top());
            context.fromdecimalEscape = *static_cast<Cm::Parsing::ValueObject<unsigned>*>(fromdecimalEscape_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromx(), fromdecimalEscape() {}
        char value;
        uint64_t fromx;
        unsigned fromdecimalEscape;
    };
    std::stack<Context> contextStack;
    Context context;
};

class stdlib::charRule : public Cm::Parsing::Rule
{
public:
    charRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<charRule>(this, &charRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<charRule>(this, &charRule::A1Action));
        Cm::Parsing::NonterminalParser* escapeNonterminalParser = GetNonterminal("escape");
        escapeNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<charRule>(this, &charRule::Postescape));
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

class stdlib::stringRule : public Cm::Parsing::Rule
{
public:
    stringRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<stringRule>(this, &stringRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<stringRule>(this, &stringRule::A1Action));
        Cm::Parsing::NonterminalParser* escapeNonterminalParser = GetNonterminal("escape");
        escapeNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<stringRule>(this, &stringRule::Postescape));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value.append(std::string(matchBegin, matchEnd));
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value.append(1, context.fromescape);
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
        std::string value;
        char fromescape;
    };
    std::stack<Context> contextStack;
    Context context;
};

void stdlib::GetReferencedGrammars()
{
}

void stdlib::CreateRules()
{
    AddRule(new Cm::Parsing::Rule("spaces", GetScope(),
        new Cm::Parsing::PositiveParser(
            new Cm::Parsing::SpaceParser())));
    AddRule(new Cm::Parsing::Rule("newline", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::StringParser("\r\n"),
                new Cm::Parsing::StringParser("\n")),
            new Cm::Parsing::StringParser("\r"))));
    AddRule(new Cm::Parsing::Rule("comment", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::NonterminalParser("line_comment", "line_comment", 0),
            new Cm::Parsing::NonterminalParser("block_comment", "block_comment", 0))));
    AddRule(new Cm::Parsing::Rule("line_comment", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::StringParser("//"),
                new Cm::Parsing::KleeneStarParser(
                    new Cm::Parsing::CharSetParser("\r\n", true))),
            new Cm::Parsing::NonterminalParser("newline", "newline", 0))));
    AddRule(new Cm::Parsing::Rule("block_comment", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::StringParser("/*"),
                new Cm::Parsing::KleeneStarParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::NonterminalParser("string", "string", 0),
                            new Cm::Parsing::NonterminalParser("char", "char", 0)),
                        new Cm::Parsing::DifferenceParser(
                            new Cm::Parsing::AnyCharParser(),
                            new Cm::Parsing::StringParser("*/"))))),
            new Cm::Parsing::StringParser("*/"))));
    AddRule(new Cm::Parsing::Rule("spaces_and_comments", GetScope(),
        new Cm::Parsing::PositiveParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::SpaceParser(),
                new Cm::Parsing::NonterminalParser("comment", "comment", 0)))));
    AddRule(new Cm::Parsing::Rule("digit_sequence", GetScope(),
        new Cm::Parsing::TokenParser(
            new Cm::Parsing::PositiveParser(
                new Cm::Parsing::DigitParser()))));
    AddRule(new Cm::Parsing::Rule("sign", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::CharParser('+'),
            new Cm::Parsing::CharParser('-'))));
    AddRule(new intRule("int", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("sign", "sign", 0)),
                    new Cm::Parsing::NonterminalParser("digit_sequence", "digit_sequence", 0))))));
    AddRule(new uintRule("uint", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("digit_sequence", "digit_sequence", 0))));
    AddRule(new longRule("long", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("sign", "sign", 0)),
                    new Cm::Parsing::NonterminalParser("digit_sequence", "digit_sequence", 0))))));
    AddRule(new ulongRule("ulong", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("digit_sequence", "digit_sequence", 0))));
    AddRule(new hexRule("hex", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::PositiveParser(
                    new Cm::Parsing::HexDigitParser())))));
    AddRule(new hex_literalRule("hex_literal", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::StringParser("0x"),
                        new Cm::Parsing::StringParser("0X")),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("hex", "hex", 0)))))));
    AddRule(new realRule("real", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("sign", "sign", 0)),
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::NonterminalParser("fractional_real", "fractional_real", 0),
                        new Cm::Parsing::NonterminalParser("exponent_real", "exponent_real", 0)))))));
    AddRule(new urealRule("ureal", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::NonterminalParser("fractional_real", "fractional_real", 0),
                new Cm::Parsing::NonterminalParser("exponent_real", "exponent_real", 0)))));
    AddRule(new Cm::Parsing::Rule("fractional_real", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::OptionalParser(
                                new Cm::Parsing::NonterminalParser("digit_sequence", "digit_sequence", 0)),
                            new Cm::Parsing::CharParser('.')),
                        new Cm::Parsing::NonterminalParser("digit_sequence", "digit_sequence", 0)),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("exponent_part", "exponent_part", 0)))),
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::NonterminalParser("digit_sequence", "digit_sequence", 0),
                    new Cm::Parsing::CharParser('.'))))));
    AddRule(new Cm::Parsing::Rule("exponent_real", GetScope(),
        new Cm::Parsing::TokenParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::NonterminalParser("digit_sequence", "digit_sequence", 0),
                new Cm::Parsing::NonterminalParser("exponent_part", "exponent_part", 0)))));
    AddRule(new Cm::Parsing::Rule("exponent_part", GetScope(),
        new Cm::Parsing::TokenParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharSetParser("eE"),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("sign", "sign", 0))),
                new Cm::Parsing::NonterminalParser("digit_sequence", "digit_sequence", 0)))));
    AddRule(new numberRule("number", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("r", "real", 0)),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("i", "int", 0)))));
    AddRule(new boolRule("bool", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::KeywordParser("true")),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::KeywordParser("false")))));
    AddRule(new identifierRule("identifier", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::LetterParser(),
                        new Cm::Parsing::CharParser('_')),
                    new Cm::Parsing::KleeneStarParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::AlternativeParser(
                                new Cm::Parsing::LetterParser(),
                                new Cm::Parsing::DigitParser()),
                            new Cm::Parsing::CharParser('_'))))))));
    AddRule(new qualified_idRule("qualified_id", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::NonterminalParser("first", "identifier", 0),
                    new Cm::Parsing::KleeneStarParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::CharParser('.'),
                            new Cm::Parsing::NonterminalParser("rest", "identifier", 0))))))));
    AddRule(new escapeRule("escape", GetScope(),
        new Cm::Parsing::TokenParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::CharParser('\\'),
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::CharSetParser("xX"),
                            new Cm::Parsing::ActionParser("A0",
                                new Cm::Parsing::NonterminalParser("x", "hex", 0))),
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::CharSetParser("dD"),
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::NonterminalParser("decimalEscape", "uint", 0)))),
                    new Cm::Parsing::ActionParser("A2",
                        new Cm::Parsing::CharSetParser("dDxX", true)))))));
    AddRule(new charRule("char", GetScope(),
        new Cm::Parsing::TokenParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('\''),
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::ActionParser("A0",
                            new Cm::Parsing::CharSetParser("\\\r\n", true)),
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::NonterminalParser("escape", "escape", 0)))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser('\''))))));
    AddRule(new stringRule("string", GetScope(),
        new Cm::Parsing::TokenParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('\"'),
                    new Cm::Parsing::KleeneStarParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::ActionParser("A0",
                                new Cm::Parsing::PositiveParser(
                                    new Cm::Parsing::CharSetParser("\"\\\r\n", true))),
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::NonterminalParser("escape", "escape", 0))))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser('\"'))))));
}

} } // namespace Cm.Parsing
