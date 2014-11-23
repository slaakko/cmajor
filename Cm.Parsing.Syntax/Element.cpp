#include "Element.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing.CppObjectModel/Type.hpp>
#include <Cm.Parsing.Cpp/Declarator.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

using namespace Cm::Parsing;

ElementGrammar* ElementGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

ElementGrammar* ElementGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    ElementGrammar* grammar(new ElementGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

ElementGrammar::ElementGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("ElementGrammar", parsingDomain_->GetNamespaceScope("Cm.Parsing.Syntax"), parsingDomain_)
{
    SetOwner(0);
    keywords0.push_back("anychar");
    keywords0.push_back("digit");
    keywords0.push_back("empty");
    keywords0.push_back("grammar");
    keywords0.push_back("hexdigit");
    keywords0.push_back("keyword");
    keywords0.push_back("keyword_list");
    keywords0.push_back("letter");
    keywords0.push_back("punctuation");
    keywords0.push_back("skip");
    keywords0.push_back("space");
    keywords0.push_back("start");
    keywords0.push_back("token");
    keywords0.push_back("using");
    keywords0.push_back("var");
}

void ElementGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, Cm::Parsing::Grammar* grammar)
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
    stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<Cm::Parsing::Grammar*>(grammar)));
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
}

class ElementGrammar::RuleLinkRule : public Cm::Parsing::Rule
{
public:
    RuleLinkRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Grammar*", "grammar"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> grammar_value = std::move(stack.top());
        context.grammar = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Grammar*>*>(grammar_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RuleLinkRule>(this, &RuleLinkRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RuleLinkRule>(this, &RuleLinkRule::A1Action));
        Cm::Parsing::NonterminalParser* aliasNameNonterminalParser = GetNonterminal("aliasName");
        aliasNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RuleLinkRule>(this, &RuleLinkRule::PostaliasName));
        Cm::Parsing::NonterminalParser* ruleNameNonterminalParser = GetNonterminal("ruleName");
        ruleNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RuleLinkRule>(this, &RuleLinkRule::PostruleName));
        Cm::Parsing::NonterminalParser* qualified_idNonterminalParser = GetNonterminal("qualified_id");
        qualified_idNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RuleLinkRule>(this, &RuleLinkRule::Postqualified_id));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        RuleLink * link(new RuleLink(context.fromaliasName, context.grammar, context.fromruleName));
        link->SetSpan(span);
        context.grammar->AddRuleLink(link);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        RuleLink * link(new RuleLink(context.grammar, context.fromqualified_id));
        link->SetSpan(span);
        context.grammar->AddRuleLink(link);
    }
    void PostaliasName(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromaliasName_value = std::move(stack.top());
            context.fromaliasName = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromaliasName_value.get());
            stack.pop();
        }
    }
    void PostruleName(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromruleName_value = std::move(stack.top());
            context.fromruleName = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromruleName_value.get());
            stack.pop();
        }
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
        Context(): grammar(), fromaliasName(), fromruleName(), fromqualified_id() {}
        Cm::Parsing::Grammar* grammar;
        std::string fromaliasName;
        std::string fromruleName;
        std::string fromqualified_id;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ElementGrammar::SignatureRule : public Cm::Parsing::Rule
{
public:
    SignatureRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Rule*", "rule"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> rule_value = std::move(stack.top());
        context.rule = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Rule*>*>(rule_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal("ParameterList");
        parameterListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<SignatureRule>(this, &SignatureRule::PreParameterList));
        Cm::Parsing::NonterminalParser* returnTypeNonterminalParser = GetNonterminal("ReturnType");
        returnTypeNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<SignatureRule>(this, &SignatureRule::PreReturnType));
    }
    void PreParameterList(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Rule*>(context.rule)));
    }
    void PreReturnType(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Rule*>(context.rule)));
    }
private:
    struct Context
    {
        Context(): rule() {}
        Cm::Parsing::Rule* rule;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ElementGrammar::ParameterListRule : public Cm::Parsing::Rule
{
public:
    ParameterListRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Rule*", "rule"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> rule_value = std::move(stack.top());
        context.rule = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Rule*>*>(rule_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::NonterminalParser* variableNonterminalParser = GetNonterminal("Variable");
        variableNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ParameterListRule>(this, &ParameterListRule::PreVariable));
        Cm::Parsing::NonterminalParser* parameterNonterminalParser = GetNonterminal("Parameter");
        parameterNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ParameterListRule>(this, &ParameterListRule::PreParameter));
    }
    void PreVariable(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Rule*>(context.rule)));
    }
    void PreParameter(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Rule*>(context.rule)));
    }
private:
    struct Context
    {
        Context(): rule() {}
        Cm::Parsing::Rule* rule;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ElementGrammar::VariableRule : public Cm::Parsing::Rule
{
public:
    VariableRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Rule*", "rule"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> rule_value = std::move(stack.top());
        context.rule = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Rule*>*>(rule_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<VariableRule>(this, &VariableRule::A0Action));
        Cm::Parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal("TypeId");
        typeIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<VariableRule>(this, &VariableRule::PostTypeId));
        Cm::Parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal("Declarator");
        declaratorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<VariableRule>(this, &VariableRule::PostDeclarator));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.rule->AddLocalVariable(AttrOrVariable(context.fromTypeId->ToString(), context.fromDeclarator));
        delete context.fromTypeId;
    }
    void PostTypeId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeId_value = std::move(stack.top());
            context.fromTypeId = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>*>(fromTypeId_value.get());
            stack.pop();
        }
    }
    void PostDeclarator(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDeclarator_value = std::move(stack.top());
            context.fromDeclarator = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): rule(), fromTypeId(), fromDeclarator() {}
        Cm::Parsing::Rule* rule;
        Cm::Parsing::CppObjectModel::TypeId* fromTypeId;
        std::string fromDeclarator;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ElementGrammar::ParameterRule : public Cm::Parsing::Rule
{
public:
    ParameterRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Rule*", "rule"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> rule_value = std::move(stack.top());
        context.rule = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Rule*>*>(rule_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ParameterRule>(this, &ParameterRule::A0Action));
        Cm::Parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal("TypeId");
        typeIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ParameterRule>(this, &ParameterRule::PostTypeId));
        Cm::Parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal("Declarator");
        declaratorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ParameterRule>(this, &ParameterRule::PostDeclarator));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.rule->AddInheritedAttribute(AttrOrVariable(context.fromTypeId->ToString(), context.fromDeclarator));
        delete context.fromTypeId;
    }
    void PostTypeId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeId_value = std::move(stack.top());
            context.fromTypeId = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>*>(fromTypeId_value.get());
            stack.pop();
        }
    }
    void PostDeclarator(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDeclarator_value = std::move(stack.top());
            context.fromDeclarator = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): rule(), fromTypeId(), fromDeclarator() {}
        Cm::Parsing::Rule* rule;
        Cm::Parsing::CppObjectModel::TypeId* fromTypeId;
        std::string fromDeclarator;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ElementGrammar::ReturnTypeRule : public Cm::Parsing::Rule
{
public:
    ReturnTypeRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::Rule*", "rule"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> rule_value = std::move(stack.top());
        context.rule = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Rule*>*>(rule_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ReturnTypeRule>(this, &ReturnTypeRule::A0Action));
        Cm::Parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal("TypeId");
        typeIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ReturnTypeRule>(this, &ReturnTypeRule::PostTypeId));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.rule->SetValueTypeName(context.fromTypeId->ToString());
        delete context.fromTypeId;
    }
    void PostTypeId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeId_value = std::move(stack.top());
            context.fromTypeId = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>*>(fromTypeId_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): rule(), fromTypeId() {}
        Cm::Parsing::Rule* rule;
        Cm::Parsing::CppObjectModel::TypeId* fromTypeId;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ElementGrammar::IdentifierRule : public Cm::Parsing::Rule
{
public:
    IdentifierRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IdentifierRule>(this, &IdentifierRule::A0Action));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IdentifierRule>(this, &IdentifierRule::Postidentifier));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = std::string(matchBegin, matchEnd);
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

class ElementGrammar::QualifiedIdRule : public Cm::Parsing::Rule
{
public:
    QualifiedIdRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<QualifiedIdRule>(this, &QualifiedIdRule::A0Action));
        Cm::Parsing::NonterminalParser* firstNonterminalParser = GetNonterminal("first");
        firstNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<QualifiedIdRule>(this, &QualifiedIdRule::Postfirst));
        Cm::Parsing::NonterminalParser* restNonterminalParser = GetNonterminal("rest");
        restNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<QualifiedIdRule>(this, &QualifiedIdRule::Postrest));
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

class ElementGrammar::StringArrayRule : public Cm::Parsing::Rule
{
public:
    StringArrayRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("std::vector<std::string>*", "array"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> array_value = std::move(stack.top());
        context.array = *static_cast<Cm::Parsing::ValueObject<std::vector<std::string>*>*>(array_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StringArrayRule>(this, &StringArrayRule::A0Action));
        Cm::Parsing::NonterminalParser* strNonterminalParser = GetNonterminal("str");
        strNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StringArrayRule>(this, &StringArrayRule::Poststr));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.array->push_back(context.fromstr);
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
        Context(): array(), fromstr() {}
        std::vector<std::string>* array;
        std::string fromstr;
    };
    std::stack<Context> contextStack;
    Context context;
};

void ElementGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.Cpp.DeclaratorGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::Cpp::DeclaratorGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar1)
    {
        grammar1 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void ElementGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("TypeId", this, "Cm.Parsing.Cpp.DeclaratorGrammar.TypeId"));
    AddRuleLink(new Cm::Parsing::RuleLink("qualified_id", this, "Cm.Parsing.stdlib.qualified_id"));
    AddRuleLink(new Cm::Parsing::RuleLink("identifier", this, "Cm.Parsing.stdlib.identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("Declarator", this, "Cm.Parsing.Cpp.DeclaratorGrammar.Declarator"));
    AddRuleLink(new Cm::Parsing::RuleLink("string", this, "Cm.Parsing.stdlib.string"));
    AddRule(new RuleLinkRule("RuleLink", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::KeywordParser("using"),
                                new Cm::Parsing::NonterminalParser("aliasName", "identifier", 0)),
                            new Cm::Parsing::CharParser('=')),
                        new Cm::Parsing::NonterminalParser("ruleName", "qualified_id", 0)),
                    new Cm::Parsing::CharParser(';'))),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("using"),
                        new Cm::Parsing::NonterminalParser("qualified_id", "qualified_id", 0)),
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new SignatureRule("Signature", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::OptionalParser(
                new Cm::Parsing::NonterminalParser("ParameterList", "ParameterList", 1)),
            new Cm::Parsing::OptionalParser(
                new Cm::Parsing::NonterminalParser("ReturnType", "ReturnType", 1)))));
    AddRule(new ParameterListRule("ParameterList", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::CharParser('('),
                new Cm::Parsing::ListParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::NonterminalParser("Variable", "Variable", 1),
                        new Cm::Parsing::NonterminalParser("Parameter", "Parameter", 1)),
                    new Cm::Parsing::CharParser(','))),
            new Cm::Parsing::ExpectationParser(
                new Cm::Parsing::CharParser(')')))));
    AddRule(new VariableRule("Variable", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::KeywordParser("var"),
                new Cm::Parsing::NonterminalParser("TypeId", "TypeId", 0)),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("Declarator", "Declarator", 0)))));
    AddRule(new ParameterRule("Parameter", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::NonterminalParser("TypeId", "TypeId", 0),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("Declarator", "Declarator", 0)))));
    AddRule(new ReturnTypeRule("ReturnType", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::DifferenceParser(
                new Cm::Parsing::CharParser(':'),
                new Cm::Parsing::StringParser("::")),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("TypeId", "TypeId", 0)))));
    AddRule(new Cm::Parsing::Rule("Keyword", GetScope(),
        new Cm::Parsing::KeywordListParser("identifier", keywords0)));
    AddRule(new IdentifierRule("Identifier", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::DifferenceParser(
                new Cm::Parsing::NonterminalParser("identifier", "identifier", 0),
                new Cm::Parsing::NonterminalParser("Keyword", "Keyword", 0)))));
    AddRule(new QualifiedIdRule("QualifiedId", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::NonterminalParser("first", "Identifier", 0),
                    new Cm::Parsing::KleeneStarParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::CharParser('.'),
                            new Cm::Parsing::NonterminalParser("rest", "Identifier", 0))))))));
    AddRule(new StringArrayRule("StringArray", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::CharParser('['),
                new Cm::Parsing::ListParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::NonterminalParser("str", "string", 0)),
                    new Cm::Parsing::CharParser(','))),
            new Cm::Parsing::ExpectationParser(
                new Cm::Parsing::CharParser(']')))));
}

} } } // namespace Cm.Parsing.Syntax
