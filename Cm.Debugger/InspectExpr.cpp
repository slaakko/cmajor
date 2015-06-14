#include "InspectExpr.hpp"
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

InspectExprGrammar* InspectExprGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

InspectExprGrammar* InspectExprGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    InspectExprGrammar* grammar(new InspectExprGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

InspectExprGrammar::InspectExprGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("InspectExprGrammar", parsingDomain_->GetNamespaceScope("Cm.Debugger"), parsingDomain_)
{
    SetOwner(0);
}

InspectNode* InspectExprGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
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
    InspectNode* result = *static_cast<Cm::Parsing::ValueObject<InspectNode*>*>(value.get());
    stack.pop();
    return result;
}

class InspectExprGrammar::InspectExprRule : public Cm::Parsing::Rule
{
public:
    InspectExprRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("InspectNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<InspectNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InspectExprRule>(this, &InspectExprRule::A0Action));
        Cm::Parsing::NonterminalParser* contentExprNonterminalParser = GetNonterminal("ContentExpr");
        contentExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InspectExprRule>(this, &InspectExprRule::PostContentExpr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromContentExpr;
    }
    void PostContentExpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromContentExpr_value = std::move(stack.top());
            context.fromContentExpr = *static_cast<Cm::Parsing::ValueObject<InspectNode*>*>(fromContentExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromContentExpr() {}
        InspectNode* value;
        InspectNode* fromContentExpr;
    };
    std::stack<Context> contextStack;
    Context context;
};

class InspectExprGrammar::ContentExprRule : public Cm::Parsing::Rule
{
public:
    ContentExprRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("InspectNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<InspectNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ContentExprRule>(this, &ContentExprRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ContentExprRule>(this, &ContentExprRule::A1Action));
        Cm::Parsing::NonterminalParser* contentNonterminalParser = GetNonterminal("content");
        contentNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ContentExprRule>(this, &ContentExprRule::Postcontent));
        Cm::Parsing::NonterminalParser* singleNonterminalParser = GetNonterminal("single");
        singleNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ContentExprRule>(this, &ContentExprRule::Postsingle));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ContentNode(context.fromcontent);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new SingleNode(context.fromsingle);
    }
    void Postcontent(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromcontent_value = std::move(stack.top());
            context.fromcontent = *static_cast<Cm::Parsing::ValueObject<InspectNode*>*>(fromcontent_value.get());
            stack.pop();
        }
    }
    void Postsingle(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromsingle_value = std::move(stack.top());
            context.fromsingle = *static_cast<Cm::Parsing::ValueObject<InspectNode*>*>(fromsingle_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromcontent(), fromsingle() {}
        InspectNode* value;
        InspectNode* fromcontent;
        InspectNode* fromsingle;
    };
    std::stack<Context> contextStack;
    Context context;
};

class InspectExprGrammar::PrefixExprRule : public Cm::Parsing::Rule
{
public:
    PrefixExprRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("InspectNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<InspectNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrefixExprRule>(this, &PrefixExprRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrefixExprRule>(this, &PrefixExprRule::A1Action));
        Cm::Parsing::NonterminalParser* prefixExprNonterminalParser = GetNonterminal("PrefixExpr");
        prefixExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrefixExprRule>(this, &PrefixExprRule::PostPrefixExpr));
        Cm::Parsing::NonterminalParser* postfixExprNonterminalParser = GetNonterminal("PostfixExpr");
        postfixExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrefixExprRule>(this, &PrefixExprRule::PostPostfixExpr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new DerefNode(context.fromPrefixExpr);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromPostfixExpr;
    }
    void PostPrefixExpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromPrefixExpr_value = std::move(stack.top());
            context.fromPrefixExpr = *static_cast<Cm::Parsing::ValueObject<InspectNode*>*>(fromPrefixExpr_value.get());
            stack.pop();
        }
    }
    void PostPostfixExpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromPostfixExpr_value = std::move(stack.top());
            context.fromPostfixExpr = *static_cast<Cm::Parsing::ValueObject<InspectNode*>*>(fromPostfixExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromPrefixExpr(), fromPostfixExpr() {}
        InspectNode* value;
        InspectNode* fromPrefixExpr;
        InspectNode* fromPostfixExpr;
    };
    std::stack<Context> contextStack;
    Context context;
};

class InspectExprGrammar::PostfixExprRule : public Cm::Parsing::Rule
{
public:
    PostfixExprRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("InspectNode*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<InspectNode>", "expr"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<InspectNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixExprRule>(this, &PostfixExprRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixExprRule>(this, &PostfixExprRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixExprRule>(this, &PostfixExprRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PostfixExprRule>(this, &PostfixExprRule::A3Action));
        Cm::Parsing::NonterminalParser* primaryExprNonterminalParser = GetNonterminal("PrimaryExpr");
        primaryExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostfixExprRule>(this, &PostfixExprRule::PostPrimaryExpr));
        Cm::Parsing::NonterminalParser* dotMemberNonterminalParser = GetNonterminal("dotMember");
        dotMemberNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostfixExprRule>(this, &PostfixExprRule::PostdotMember));
        Cm::Parsing::NonterminalParser* arrowMemberNonterminalParser = GetNonterminal("arrowMember");
        arrowMemberNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PostfixExprRule>(this, &PostfixExprRule::PostarrowMember));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.expr.release();
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.expr.reset(context.fromPrimaryExpr);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.expr.reset(new DotNode(context.expr.release(), context.fromdotMember));
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.expr.reset(new ArrowNode(context.expr.release(), context.fromarrowMember));
    }
    void PostPrimaryExpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromPrimaryExpr_value = std::move(stack.top());
            context.fromPrimaryExpr = *static_cast<Cm::Parsing::ValueObject<InspectNode*>*>(fromPrimaryExpr_value.get());
            stack.pop();
        }
    }
    void PostdotMember(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromdotMember_value = std::move(stack.top());
            context.fromdotMember = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromdotMember_value.get());
            stack.pop();
        }
    }
    void PostarrowMember(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromarrowMember_value = std::move(stack.top());
            context.fromarrowMember = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromarrowMember_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), expr(), fromPrimaryExpr(), fromdotMember(), fromarrowMember() {}
        InspectNode* value;
        std::unique_ptr<InspectNode> expr;
        InspectNode* fromPrimaryExpr;
        std::string fromdotMember;
        std::string fromarrowMember;
    };
    std::stack<Context> contextStack;
    Context context;
};

class InspectExprGrammar::MemberIdRule : public Cm::Parsing::Rule
{
public:
    MemberIdRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MemberIdRule>(this, &MemberIdRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MemberIdRule>(this, &MemberIdRule::A1Action));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MemberIdRule>(this, &MemberIdRule::Postidentifier));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = std::string(matchBegin, matchEnd);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = "base";
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
        Context(): value(), fromidentifier() {}
        std::string value;
        std::string fromidentifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

class InspectExprGrammar::PrimaryExprRule : public Cm::Parsing::Rule
{
public:
    PrimaryExprRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("InspectNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<InspectNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryExprRule>(this, &PrimaryExprRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryExprRule>(this, &PrimaryExprRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryExprRule>(this, &PrimaryExprRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PrimaryExprRule>(this, &PrimaryExprRule::A3Action));
        Cm::Parsing::NonterminalParser* localVarNameNonterminalParser = GetNonterminal("localVarName");
        localVarNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryExprRule>(this, &PrimaryExprRule::PostlocalVarName));
        Cm::Parsing::NonterminalParser* handleNonterminalParser = GetNonterminal("handle");
        handleNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryExprRule>(this, &PrimaryExprRule::Posthandle));
        Cm::Parsing::NonterminalParser* typeExprPartNonterminalParser = GetNonterminal("TypeExprPart");
        typeExprPartNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryExprRule>(this, &PrimaryExprRule::PostTypeExprPart));
        Cm::Parsing::NonterminalParser* prefixExprNonterminalParser = GetNonterminal("PrefixExpr");
        prefixExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PrimaryExprRule>(this, &PrimaryExprRule::PostPrefixExpr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new LocalNode("this");
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new LocalNode(context.fromlocalVarName);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new HandleNode(context.fromhandle, context.fromTypeExprPart);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ParenthesesNode(context.fromPrefixExpr);
    }
    void PostlocalVarName(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromlocalVarName_value = std::move(stack.top());
            context.fromlocalVarName = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromlocalVarName_value.get());
            stack.pop();
        }
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
    void PostTypeExprPart(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeExprPart_value = std::move(stack.top());
            context.fromTypeExprPart = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromTypeExprPart_value.get());
            stack.pop();
        }
    }
    void PostPrefixExpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromPrefixExpr_value = std::move(stack.top());
            context.fromPrefixExpr = *static_cast<Cm::Parsing::ValueObject<InspectNode*>*>(fromPrefixExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromlocalVarName(), fromhandle(), fromTypeExprPart(), fromPrefixExpr() {}
        InspectNode* value;
        std::string fromlocalVarName;
        int fromhandle;
        std::string fromTypeExprPart;
        InspectNode* fromPrefixExpr;
    };
    std::stack<Context> contextStack;
    Context context;
};

class InspectExprGrammar::TypeExprPartRule : public Cm::Parsing::Rule
{
public:
    TypeExprPartRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeExprPartRule>(this, &TypeExprPartRule::A0Action));
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

void InspectExprGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void InspectExprGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("spaces", this, "Cm.Parsing.stdlib.spaces"));
    AddRuleLink(new Cm::Parsing::RuleLink("identifier", this, "Cm.Parsing.stdlib.identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("int", this, "Cm.Parsing.stdlib.int"));
    AddRule(new InspectExprRule("InspectExpr", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("ContentExpr", "ContentExpr", 0))));
    AddRule(new ContentExprRule("ContentExpr", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::CharParser('@'),
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("content", "PrefixExpr", 0))),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("single", "PrefixExpr", 0)))));
    AddRule(new PrefixExprRule("PrefixExpr", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::CharParser('*'),
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("PrefixExpr", "PrefixExpr", 0))),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("PostfixExpr", "PostfixExpr", 0)))));
    AddRule(new PostfixExprRule("PostfixExpr", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::NonterminalParser("PrimaryExpr", "PrimaryExpr", 0)),
                new Cm::Parsing::KleeneStarParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::CharParser('.'),
                            new Cm::Parsing::ActionParser("A2",
                                new Cm::Parsing::NonterminalParser("dotMember", "MemberId", 0))),
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::StringParser("->"),
                            new Cm::Parsing::ActionParser("A3",
                                new Cm::Parsing::NonterminalParser("arrowMember", "MemberId", 0)))))))));
    AddRule(new MemberIdRule("MemberId", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("identifier", "identifier", 0)),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::KeywordParser("base")))));
    AddRule(new PrimaryExprRule("PrimaryExpr", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::KeywordParser("this")),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("localVarName", "identifier", 0))),
                new Cm::Parsing::ActionParser("A2",
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::TokenParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::CharParser('$'),
                                new Cm::Parsing::NonterminalParser("handle", "int", 0))),
                        new Cm::Parsing::NonterminalParser("TypeExprPart", "TypeExprPart", 0)))),
            new Cm::Parsing::ActionParser("A3",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::CharParser('('),
                        new Cm::Parsing::NonterminalParser("PrefixExpr", "PrefixExpr", 0)),
                    new Cm::Parsing::CharParser(')'))))));
    AddRule(new TypeExprPartRule("TypeExprPart", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::CharParser('['),
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::TokenParser(
                        new Cm::Parsing::PositiveParser(
                            new Cm::Parsing::DifferenceParser(
                                new Cm::Parsing::AnyCharParser(),
                                new Cm::Parsing::CharParser(']')))))),
            new Cm::Parsing::CharParser(']'))));
    SetSkipRuleName("spaces");
}

} } // namespace Cm.Debugger
