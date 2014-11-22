#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <sstream>

namespace Soul { namespace Parsing {

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

stdlib::stdlib(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("stdlib", parsingDomain_->GetNamespaceScope("Soul.Parsing"), parsingDomain_)
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
    virtual void Enter(ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<int>(context.value)));
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
    virtual void Enter(ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<unsigned>(context.value)));
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
    virtual void Enter(ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<int64_t>(context.value)));
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
    virtual void Enter(ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<uint64_t>(context.value)));
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
    virtual void Enter(ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<uint64_t>(context.value)));
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
    virtual void Enter(ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<uint64_t>(context.value)));
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
    void Posthex(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromhex_value = std::move(stack.top());
            context.fromhex = *static_cast<ValueObject<uint64_t>*>(fromhex_value.get());
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
    virtual void Enter(ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<double>(context.value)));
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
    virtual void Enter(ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<double>(context.value)));
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
    virtual void Enter(ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<double>(context.value)));
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
    void Postr(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromr_value = std::move(stack.top());
            context.fromr = *static_cast<ValueObject<double>*>(fromr_value.get());
            stack.pop();
        }
    }
    void Posti(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromi_value = std::move(stack.top());
            context.fromi = *static_cast<ValueObject<int>*>(fromi_value.get());
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
    virtual void Enter(ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<bool>(context.value)));
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
    virtual void Enter(ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<std::string>(context.value)));
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
    virtual void Enter(ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<std::string>(context.value)));
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
    void Postfirst(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromfirst_value = std::move(stack.top());
            context.fromfirst = *static_cast<ValueObject<std::string>*>(fromfirst_value.get());
            stack.pop();
        }
    }
    void Postrest(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromrest_value = std::move(stack.top());
            context.fromrest = *static_cast<ValueObject<std::string>*>(fromrest_value.get());
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
    virtual void Enter(ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<char>(context.value)));
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
    void Postx(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromx_value = std::move(stack.top());
            context.fromx = *static_cast<ValueObject<uint64_t>*>(fromx_value.get());
            stack.pop();
        }
    }
    void PostdecimalEscape(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromdecimalEscape_value = std::move(stack.top());
            context.fromdecimalEscape = *static_cast<ValueObject<unsigned>*>(fromdecimalEscape_value.get());
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
    virtual void Enter(ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<char>(context.value)));
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
    void Postescape(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromescape_value = std::move(stack.top());
            context.fromescape = *static_cast<ValueObject<char>*>(fromescape_value.get());
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
    virtual void Enter(ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<std::string>(context.value)));
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
    void Postescape(ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromescape_value = std::move(stack.top());
            context.fromescape = *static_cast<ValueObject<char>*>(fromescape_value.get());
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
    AddRule(new Rule("spaces", GetScope(),
        new PositiveParser(
            new SpaceParser())));
    AddRule(new Rule("newline", GetScope(),
        new AlternativeParser(
            new AlternativeParser(
                new StringParser("\r\n"),
                new StringParser("\n")),
            new StringParser("\r"))));
    AddRule(new Rule("comment", GetScope(),
        new AlternativeParser(
            new NonterminalParser("line_comment", "line_comment", 0),
            new NonterminalParser("block_comment", "block_comment", 0))));
    AddRule(new Rule("line_comment", GetScope(),
        new SequenceParser(
            new SequenceParser(
                new StringParser("//"),
                new KleeneStarParser(
                    new CharSetParser("\r\n", true))),
            new NonterminalParser("newline", "newline", 0))));
    AddRule(new Rule("block_comment", GetScope(),
        new SequenceParser(
            new SequenceParser(
                new StringParser("/*"),
                new KleeneStarParser(
                    new AlternativeParser(
                        new AlternativeParser(
                            new NonterminalParser("string", "string", 0),
                            new NonterminalParser("char", "char", 0)),
                        new DifferenceParser(
                            new AnyCharParser(),
                            new StringParser("*/"))))),
            new StringParser("*/"))));
    AddRule(new Rule("spaces_and_comments", GetScope(),
        new PositiveParser(
            new AlternativeParser(
                new SpaceParser(),
                new NonterminalParser("comment", "comment", 0)))));
    AddRule(new Rule("digit_sequence", GetScope(),
        new TokenParser(
            new PositiveParser(
                new DigitParser()))));
    AddRule(new Rule("sign", GetScope(),
        new AlternativeParser(
            new CharParser('+'),
            new CharParser('-'))));
    AddRule(new intRule("int", GetScope(),
        new ActionParser("A0",
            new TokenParser(
                new SequenceParser(
                    new OptionalParser(
                        new NonterminalParser("sign", "sign", 0)),
                    new NonterminalParser("digit_sequence", "digit_sequence", 0))))));
    AddRule(new uintRule("uint", GetScope(),
        new ActionParser("A0",
            new NonterminalParser("digit_sequence", "digit_sequence", 0))));
    AddRule(new longRule("long", GetScope(),
        new ActionParser("A0",
            new TokenParser(
                new SequenceParser(
                    new OptionalParser(
                        new NonterminalParser("sign", "sign", 0)),
                    new NonterminalParser("digit_sequence", "digit_sequence", 0))))));
    AddRule(new ulongRule("ulong", GetScope(),
        new ActionParser("A0",
            new NonterminalParser("digit_sequence", "digit_sequence", 0))));
    AddRule(new hexRule("hex", GetScope(),
        new ActionParser("A0",
            new TokenParser(
                new PositiveParser(
                    new HexDigitParser())))));
    AddRule(new hex_literalRule("hex_literal", GetScope(),
        new ActionParser("A0",
            new TokenParser(
                new SequenceParser(
                    new AlternativeParser(
                        new StringParser("0x"),
                        new StringParser("0X")),
                    new ExpectationParser(
                        new NonterminalParser("hex", "hex", 0)))))));
    AddRule(new realRule("real", GetScope(),
        new ActionParser("A0",
            new TokenParser(
                new SequenceParser(
                    new OptionalParser(
                        new NonterminalParser("sign", "sign", 0)),
                    new AlternativeParser(
                        new NonterminalParser("fractional_real", "fractional_real", 0),
                        new NonterminalParser("exponent_real", "exponent_real", 0)))))));
    AddRule(new urealRule("ureal", GetScope(),
        new ActionParser("A0",
            new AlternativeParser(
                new NonterminalParser("fractional_real", "fractional_real", 0),
                new NonterminalParser("exponent_real", "exponent_real", 0)))));
    AddRule(new Rule("fractional_real", GetScope(),
        new AlternativeParser(
            new TokenParser(
                new SequenceParser(
                    new SequenceParser(
                        new SequenceParser(
                            new OptionalParser(
                                new NonterminalParser("digit_sequence", "digit_sequence", 0)),
                            new CharParser('.')),
                        new NonterminalParser("digit_sequence", "digit_sequence", 0)),
                    new OptionalParser(
                        new NonterminalParser("exponent_part", "exponent_part", 0)))),
            new TokenParser(
                new SequenceParser(
                    new NonterminalParser("digit_sequence", "digit_sequence", 0),
                    new CharParser('.'))))));
    AddRule(new Rule("exponent_real", GetScope(),
        new TokenParser(
            new SequenceParser(
                new NonterminalParser("digit_sequence", "digit_sequence", 0),
                new NonterminalParser("exponent_part", "exponent_part", 0)))));
    AddRule(new Rule("exponent_part", GetScope(),
        new TokenParser(
            new SequenceParser(
                new SequenceParser(
                    new CharSetParser("eE"),
                    new OptionalParser(
                        new NonterminalParser("sign", "sign", 0))),
                new NonterminalParser("digit_sequence", "digit_sequence", 0)))));
    AddRule(new numberRule("number", GetScope(),
        new AlternativeParser(
            new ActionParser("A0",
                new NonterminalParser("r", "real", 0)),
            new ActionParser("A1",
                new NonterminalParser("i", "int", 0)))));
    AddRule(new boolRule("bool", GetScope(),
        new AlternativeParser(
            new ActionParser("A0",
                new KeywordParser("true")),
            new ActionParser("A1",
                new KeywordParser("false")))));
    AddRule(new identifierRule("identifier", GetScope(),
        new ActionParser("A0",
            new TokenParser(
                new SequenceParser(
                    new AlternativeParser(
                        new LetterParser(),
                        new CharParser('_')),
                    new KleeneStarParser(
                        new AlternativeParser(
                            new AlternativeParser(
                                new LetterParser(),
                                new DigitParser()),
                            new CharParser('_'))))))));
    AddRule(new qualified_idRule("qualified_id", GetScope(),
        new ActionParser("A0",
            new TokenParser(
                new SequenceParser(
                    new NonterminalParser("first", "identifier", 0),
                    new KleeneStarParser(
                        new SequenceParser(
                            new CharParser('.'),
                            new NonterminalParser("rest", "identifier", 0))))))));
    AddRule(new escapeRule("escape", GetScope(),
        new TokenParser(
            new SequenceParser(
                new CharParser('\\'),
                new AlternativeParser(
                    new AlternativeParser(
                        new SequenceParser(
                            new CharSetParser("xX"),
                            new ActionParser("A0",
                                new NonterminalParser("x", "hex", 0))),
                        new SequenceParser(
                            new CharSetParser("dD"),
                            new ActionParser("A1",
                                new NonterminalParser("decimalEscape", "uint", 0)))),
                    new ActionParser("A2",
                        new CharSetParser("dDxX", true)))))));
    AddRule(new charRule("char", GetScope(),
        new TokenParser(
            new SequenceParser(
                new SequenceParser(
                    new CharParser('\''),
                    new AlternativeParser(
                        new ActionParser("A0",
                            new CharSetParser("\\\r\n", true)),
                        new ActionParser("A1",
                            new NonterminalParser("escape", "escape", 0)))),
                new ExpectationParser(
                    new CharParser('\''))))));
    AddRule(new stringRule("string", GetScope(),
        new TokenParser(
            new SequenceParser(
                new SequenceParser(
                    new CharParser('\"'),
                    new KleeneStarParser(
                        new AlternativeParser(
                            new ActionParser("A0",
                                new PositiveParser(
                                    new CharSetParser("\"\\\r\n", true))),
                            new ActionParser("A1",
                                new NonterminalParser("escape", "escape", 0))))),
                new ExpectationParser(
                    new CharParser('\"'))))));
}

} } // namespace Soul.Parsing
