#include "Interface.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Parser/Specifier.hpp>
#include <Cm.Parser/Identifier.hpp>
#include <Cm.Parser/Keyword.hpp>
#include <Cm.Parser/Parameter.hpp>
#include <Cm.Parser/TypeExpr.hpp>

namespace Cm { namespace Parser {

using namespace Cm::Parsing;

InterfaceGrammar* InterfaceGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

InterfaceGrammar* InterfaceGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    InterfaceGrammar* grammar(new InterfaceGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

InterfaceGrammar::InterfaceGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("InterfaceGrammar", parsingDomain_->GetNamespaceScope("Cm.Parser"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Ast::InterfaceNode* InterfaceGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx, Cm::Ast::CompileUnitNode* compileUnit)
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
    stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<ParsingContext*>(ctx)));
    stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<Cm::Ast::CompileUnitNode*>(compileUnit)));
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
    Cm::Ast::InterfaceNode* result = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::InterfaceNode*>*>(value.get());
    stack.pop();
    return result;
}

class InterfaceGrammar::InterfaceRule : public Cm::Parsing::Rule
{
public:
    InterfaceRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::CompileUnitNode*", "compileUnit"));
        SetValueTypeName("Cm::Ast::InterfaceNode*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> compileUnit_value = std::move(stack.top());
        context.compileUnit = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>*>(compileUnit_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::InterfaceNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InterfaceRule>(this, &InterfaceRule::A0Action));
        Cm::Parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal("Specifiers");
        specifiersNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InterfaceRule>(this, &InterfaceRule::PostSpecifiers));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InterfaceRule>(this, &InterfaceRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* interfaceContentNonterminalParser = GetNonterminal("InterfaceContent");
        interfaceContentNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<InterfaceRule>(this, &InterfaceRule::PreInterfaceContent));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Ast::InterfaceNode(span, context.fromSpecifiers, context.fromIdentifier);
        context.value->SetCompileUnit(context.compileUnit);
    }
    void PostSpecifiers(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromSpecifiers_value = std::move(stack.top());
            context.fromSpecifiers = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Specifiers>*>(fromSpecifiers_value.get());
            stack.pop();
        }
    }
    void PostIdentifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIdentifier_value = std::move(stack.top());
            context.fromIdentifier = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
    void PreInterfaceContent(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::InterfaceNode*>(context.value)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>(context.compileUnit)));
    }
private:
    struct Context
    {
        Context(): ctx(), compileUnit(), value(), fromSpecifiers(), fromIdentifier() {}
        ParsingContext* ctx;
        Cm::Ast::CompileUnitNode* compileUnit;
        Cm::Ast::InterfaceNode* value;
        Cm::Ast::Specifiers fromSpecifiers;
        Cm::Ast::IdentifierNode* fromIdentifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

class InterfaceGrammar::InterfaceContentRule : public Cm::Parsing::Rule
{
public:
    InterfaceContentRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::InterfaceNode*", "intf"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::CompileUnitNode*", "compileUnit"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> compileUnit_value = std::move(stack.top());
        context.compileUnit = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>*>(compileUnit_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> intf_value = std::move(stack.top());
        context.intf = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::InterfaceNode*>*>(intf_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InterfaceContentRule>(this, &InterfaceContentRule::A0Action));
        Cm::Parsing::NonterminalParser* interfaceMemFunNonterminalParser = GetNonterminal("InterfaceMemFun");
        interfaceMemFunNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<InterfaceContentRule>(this, &InterfaceContentRule::PreInterfaceMemFun));
        interfaceMemFunNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InterfaceContentRule>(this, &InterfaceContentRule::PostInterfaceMemFun));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.intf->AddMember(context.fromInterfaceMemFun);
    }
    void PreInterfaceMemFun(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>(context.compileUnit)));
    }
    void PostInterfaceMemFun(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromInterfaceMemFun_value = std::move(stack.top());
            context.fromInterfaceMemFun = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::MemberFunctionNode*>*>(fromInterfaceMemFun_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), intf(), compileUnit(), fromInterfaceMemFun() {}
        ParsingContext* ctx;
        Cm::Ast::InterfaceNode* intf;
        Cm::Ast::CompileUnitNode* compileUnit;
        Cm::Ast::MemberFunctionNode* fromInterfaceMemFun;
    };
    std::stack<Context> contextStack;
    Context context;
};

class InterfaceGrammar::InterfaceMemFunRule : public Cm::Parsing::Rule
{
public:
    InterfaceMemFunRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::CompileUnitNode*", "compileUnit"));
        SetValueTypeName("Cm::Ast::MemberFunctionNode*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<Cm::Ast::MemberFunctionNode>", "memFun"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> compileUnit_value = std::move(stack.top());
        context.compileUnit = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>*>(compileUnit_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::MemberFunctionNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InterfaceMemFunRule>(this, &InterfaceMemFunRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InterfaceMemFunRule>(this, &InterfaceMemFunRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InterfaceMemFunRule>(this, &InterfaceMemFunRule::A2Action));
        Cm::Parsing::NonterminalParser* interfaceMemFunSpecifiersNonterminalParser = GetNonterminal("InterfaceMemFunSpecifiers");
        interfaceMemFunSpecifiersNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InterfaceMemFunRule>(this, &InterfaceMemFunRule::PostInterfaceMemFunSpecifiers));
        Cm::Parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal("TypeExpr");
        typeExprNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<InterfaceMemFunRule>(this, &InterfaceMemFunRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InterfaceMemFunRule>(this, &InterfaceMemFunRule::PostTypeExpr));
        Cm::Parsing::NonterminalParser* interfaceFunctionGroupIdNonterminalParser = GetNonterminal("InterfaceFunctionGroupId");
        interfaceFunctionGroupIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InterfaceMemFunRule>(this, &InterfaceMemFunRule::PostInterfaceFunctionGroupId));
        Cm::Parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal("ParameterList");
        parameterListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<InterfaceMemFunRule>(this, &InterfaceMemFunRule::PreParameterList));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.memFun.release();
        context.value->SetCompileUnit(context.compileUnit);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.memFun.reset(new Cm::Ast::MemberFunctionNode(span, context.fromInterfaceMemFunSpecifiers, context.fromTypeExpr, context.fromInterfaceFunctionGroupId));
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.memFun->SetConst();
    }
    void PostInterfaceMemFunSpecifiers(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromInterfaceMemFunSpecifiers_value = std::move(stack.top());
            context.fromInterfaceMemFunSpecifiers = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Specifiers>*>(fromInterfaceMemFunSpecifiers_value.get());
            stack.pop();
        }
    }
    void PreTypeExpr(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostTypeExpr(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeExpr_value = std::move(stack.top());
            context.fromTypeExpr = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromTypeExpr_value.get());
            stack.pop();
        }
    }
    void PostInterfaceFunctionGroupId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromInterfaceFunctionGroupId_value = std::move(stack.top());
            context.fromInterfaceFunctionGroupId = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::FunctionGroupIdNode*>*>(fromInterfaceFunctionGroupId_value.get());
            stack.pop();
        }
    }
    void PreParameterList(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.memFun.get())));
    }
private:
    struct Context
    {
        Context(): ctx(), compileUnit(), value(), memFun(), fromInterfaceMemFunSpecifiers(), fromTypeExpr(), fromInterfaceFunctionGroupId() {}
        ParsingContext* ctx;
        Cm::Ast::CompileUnitNode* compileUnit;
        Cm::Ast::MemberFunctionNode* value;
        std::unique_ptr<Cm::Ast::MemberFunctionNode> memFun;
        Cm::Ast::Specifiers fromInterfaceMemFunSpecifiers;
        Cm::Ast::Node* fromTypeExpr;
        Cm::Ast::FunctionGroupIdNode* fromInterfaceFunctionGroupId;
    };
    std::stack<Context> contextStack;
    Context context;
};

class InterfaceGrammar::InterfaceMemFunSpecifiersRule : public Cm::Parsing::Rule
{
public:
    InterfaceMemFunSpecifiersRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::Specifiers");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Specifiers>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InterfaceMemFunSpecifiersRule>(this, &InterfaceMemFunSpecifiersRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InterfaceMemFunSpecifiersRule>(this, &InterfaceMemFunSpecifiersRule::A1Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = Cm::Ast::Specifiers::throw_;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = Cm::Ast::Specifiers::nothrow_;
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Ast::Specifiers value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class InterfaceGrammar::InterfaceFunctionGroupIdRule : public Cm::Parsing::Rule
{
public:
    InterfaceFunctionGroupIdRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::FunctionGroupIdNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::FunctionGroupIdNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InterfaceFunctionGroupIdRule>(this, &InterfaceFunctionGroupIdRule::A0Action));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InterfaceFunctionGroupIdRule>(this, &InterfaceFunctionGroupIdRule::Postidentifier));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Ast::FunctionGroupIdNode(span, std::string(matchBegin, matchEnd));
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
        Cm::Ast::FunctionGroupIdNode* value;
        std::string fromidentifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

void InterfaceGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parser.KeywordGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parser::KeywordGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parser.ParameterGrammar");
    if (!grammar1)
    {
        grammar1 = Cm::Parser::ParameterGrammar::Create(pd);
    }
    AddGrammarReference(grammar1);
    Cm::Parsing::Grammar* grammar2 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar2)
    {
        grammar2 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar2);
    Cm::Parsing::Grammar* grammar3 = pd->GetGrammar("Cm.Parser.SpecifierGrammar");
    if (!grammar3)
    {
        grammar3 = Cm::Parser::SpecifierGrammar::Create(pd);
    }
    AddGrammarReference(grammar3);
    Cm::Parsing::Grammar* grammar4 = pd->GetGrammar("Cm.Parser.IdentifierGrammar");
    if (!grammar4)
    {
        grammar4 = Cm::Parser::IdentifierGrammar::Create(pd);
    }
    AddGrammarReference(grammar4);
    Cm::Parsing::Grammar* grammar5 = pd->GetGrammar("Cm.Parser.TypeExprGrammar");
    if (!grammar5)
    {
        grammar5 = Cm::Parser::TypeExprGrammar::Create(pd);
    }
    AddGrammarReference(grammar5);
}

void InterfaceGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("spaces_and_comments", this, "Cm.Parsing.stdlib.spaces_and_comments"));
    AddRuleLink(new Cm::Parsing::RuleLink("Identifier", this, "IdentifierGrammar.Identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("Specifiers", this, "SpecifierGrammar.Specifiers"));
    AddRuleLink(new Cm::Parsing::RuleLink("ParameterList", this, "ParameterGrammar.ParameterList"));
    AddRuleLink(new Cm::Parsing::RuleLink("Keyword", this, "KeywordGrammar.Keyword"));
    AddRuleLink(new Cm::Parsing::RuleLink("TypeExpr", this, "TypeExprGrammar.TypeExpr"));
    AddRuleLink(new Cm::Parsing::RuleLink("identifier", this, "Cm.Parsing.stdlib.identifier"));
    AddRule(new InterfaceRule("Interface", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::NonterminalParser("Specifiers", "Specifiers", 0),
                                new Cm::Parsing::KeywordParser("interface")),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0)))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser('{'))),
                new Cm::Parsing::NonterminalParser("InterfaceContent", "InterfaceContent", 3)),
            new Cm::Parsing::ExpectationParser(
                new Cm::Parsing::CharParser('}')))));
    AddRule(new InterfaceContentRule("InterfaceContent", GetScope(),
        new Cm::Parsing::KleeneStarParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("InterfaceMemFun", "InterfaceMemFun", 2)))));
    AddRule(new InterfaceMemFunRule("InterfaceMemFun", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::OptionalParser(
                                        new Cm::Parsing::NonterminalParser("InterfaceMemFunSpecifiers", "InterfaceMemFunSpecifiers", 0)),
                                    new Cm::Parsing::NonterminalParser("TypeExpr", "TypeExpr", 1)),
                                new Cm::Parsing::NonterminalParser("InterfaceFunctionGroupId", "InterfaceFunctionGroupId", 0))),
                        new Cm::Parsing::NonterminalParser("ParameterList", "ParameterList", 2)),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::KeywordParser("const")))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new InterfaceMemFunSpecifiersRule("InterfaceMemFunSpecifiers", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::KeywordParser("throw")),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::KeywordParser("nothrow")))));
    AddRule(new InterfaceFunctionGroupIdRule("InterfaceFunctionGroupId", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::DifferenceParser(
                    new Cm::Parsing::NonterminalParser("identifier", "identifier", 0),
                    new Cm::Parsing::NonterminalParser("Keyword", "Keyword", 0))))));
    SetSkipRuleName("spaces_and_comments");
}

} } // namespace Cm.Parser
