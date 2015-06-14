#include "TypeExpr.hpp"
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

TypeExprGrammar* TypeExprGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

TypeExprGrammar* TypeExprGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    TypeExprGrammar* grammar(new TypeExprGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

TypeExprGrammar::TypeExprGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("TypeExprGrammar", parsingDomain_->GetNamespaceScope("Cm.Debugger"), parsingDomain_)
{
    SetOwner(0);
    keywords0.push_back("bool");
    keywords0.push_back("byte");
    keywords0.push_back("char");
    keywords0.push_back("int");
    keywords0.push_back("long");
    keywords0.push_back("sbyte");
    keywords0.push_back("short");
    keywords0.push_back("uint");
    keywords0.push_back("ulong");
    keywords0.push_back("ushort");
    keywords0.push_back("void");
}

TypeExpr* TypeExprGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
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
    TypeExpr* result = *static_cast<Cm::Parsing::ValueObject<TypeExpr*>*>(value.get());
    stack.pop();
    return result;
}

class TypeExprGrammar::TypeExprRule : public Cm::Parsing::Rule
{
public:
    TypeExprRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("TypeExpr*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<TypeExpr*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeExprRule>(this, &TypeExprRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeExprRule>(this, &TypeExprRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeExprRule>(this, &TypeExprRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeExprRule>(this, &TypeExprRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeExprRule>(this, &TypeExprRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeExprRule>(this, &TypeExprRule::A5Action));
        Cm::Parsing::NonterminalParser* basicTypeNonterminalParser = GetNonterminal("BasicType");
        basicTypeNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypeExprRule>(this, &TypeExprRule::PostBasicType));
        Cm::Parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal("TypeName");
        typeNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypeExprRule>(this, &TypeExprRule::PostTypeName));
        Cm::Parsing::NonterminalParser* typeArgumentsNonterminalParser = GetNonterminal("TypeArguments");
        typeArgumentsNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypeExprRule>(this, &TypeExprRule::PostTypeArguments));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromBasicType;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new TypeExpr(context.fromTypeName);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->TypeArguments() = MakeTemplateArgumentList(context.fromTypeArguments);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->Derivations().Add(Cm::Ast::Derivation::rvalueRef);
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->Derivations().Add(Cm::Ast::Derivation::reference);
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->Derivations().Add(Cm::Ast::Derivation::pointer);
    }
    void PostBasicType(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromBasicType_value = std::move(stack.top());
            context.fromBasicType = *static_cast<Cm::Parsing::ValueObject<TypeExpr*>*>(fromBasicType_value.get());
            stack.pop();
        }
    }
    void PostTypeName(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeName_value = std::move(stack.top());
            context.fromTypeName = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromTypeName_value.get());
            stack.pop();
        }
    }
    void PostTypeArguments(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeArguments_value = std::move(stack.top());
            context.fromTypeArguments = *static_cast<Cm::Parsing::ValueObject<std::vector<TypeExpr*>>*>(fromTypeArguments_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromBasicType(), fromTypeName(), fromTypeArguments() {}
        TypeExpr* value;
        TypeExpr* fromBasicType;
        std::string fromTypeName;
        std::vector<TypeExpr*> fromTypeArguments;
    };
    std::stack<Context> contextStack;
    Context context;
};

class TypeExprGrammar::TypeArgumentsRule : public Cm::Parsing::Rule
{
public:
    TypeArgumentsRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("std::vector<TypeExpr*>");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<std::vector<TypeExpr*>>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeArgumentsRule>(this, &TypeArgumentsRule::A0Action));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypeArgumentsRule>(this, &TypeArgumentsRule::PostTypeExpr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value.push_back(context.fromTypeExpr);
    }
    void PostTypeExpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeExpr_value = std::move(stack.top());
            context.fromTypeExpr = *static_cast<Cm::Parsing::ValueObject<TypeExpr*>*>(fromTypeExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromTypeExpr() {}
        std::vector<TypeExpr*> value;
        TypeExpr* fromTypeExpr;
    };
    std::stack<Context> contextStack;
    Context context;
};

class TypeExprGrammar::BasicTypeRule : public Cm::Parsing::Rule
{
public:
    BasicTypeRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("TypeExpr*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<TypeExpr*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<BasicTypeRule>(this, &BasicTypeRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new TypeExpr(std::string(matchBegin, matchEnd));
        context.value->SetBasicTypeExpr();
    }
private:
    struct Context
    {
        Context(): value() {}
        TypeExpr* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class TypeExprGrammar::TypeNameRule : public Cm::Parsing::Rule
{
public:
    TypeNameRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeNameRule>(this, &TypeNameRule::A0Action));
        Cm::Parsing::NonterminalParser* qualified_idNonterminalParser = GetNonterminal("qualified_id");
        qualified_idNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypeNameRule>(this, &TypeNameRule::Postqualified_id));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromqualified_id;
    }
    void Postqualified_id(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromqualified_id_value = std::move(stack.top());
            context.fromqualified_id = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromqualified_id_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromqualified_id() {}
        std::string value;
        std::string fromqualified_id;
    };
    std::stack<Context> contextStack;
    Context context;
};

void TypeExprGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void TypeExprGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("qualified_id", this, "Cm.Parsing.stdlib.qualified_id"));
    AddRuleLink(new Cm::Parsing::RuleLink("identifier", this, "Cm.Parsing.stdlib.identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("spaces", this, "Cm.Parsing.stdlib.spaces"));
    AddRule(new TypeExprRule("TypeExpr", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("BasicType", "BasicType", 0)),
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("TypeName", "TypeName", 0)),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::NonterminalParser("TypeArguments", "TypeArguments", 0))))),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::ActionParser("A3",
                            new Cm::Parsing::StringParser("&&")),
                        new Cm::Parsing::ActionParser("A4",
                            new Cm::Parsing::StringParser("&"))),
                    new Cm::Parsing::ActionParser("A5",
                        new Cm::Parsing::StringParser("*")))))));
    AddRule(new TypeArgumentsRule("TypeArguments", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::CharParser('<'),
                new Cm::Parsing::ListParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 0)),
                    new Cm::Parsing::CharParser(','))),
            new Cm::Parsing::CharParser('>'))));
    AddRule(new BasicTypeRule("BasicType", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::KeywordListParser("identifier", keywords0))));
    AddRule(new TypeNameRule("TypeName", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("qualified_id", "qualified_id", 0))));
    SetSkipRuleName("spaces");
}

} } // namespace Cm.Debugger
