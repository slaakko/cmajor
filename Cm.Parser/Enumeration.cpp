#include "Enumeration.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Parser/Expression.hpp>
#include <Cm.Parser/Identifier.hpp>
#include <Cm.Parser/Specifier.hpp>

namespace Cm { namespace Parser {

using namespace Cm::Ast;
using namespace Cm::Parsing;

EnumerationGrammar* EnumerationGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

EnumerationGrammar* EnumerationGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    EnumerationGrammar* grammar(new EnumerationGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

EnumerationGrammar::EnumerationGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("EnumerationGrammar", parsingDomain_->GetNamespaceScope("Cm.Parser"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Ast::EnumTypeNode* EnumerationGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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
    Cm::Ast::EnumTypeNode* result = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::EnumTypeNode*>*>(value.get());
    stack.pop();
    return result;
}

class EnumerationGrammar::EnumTypeRule : public Cm::Parsing::Rule
{
public:
    EnumTypeRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::EnumTypeNode*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::EnumTypeNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<EnumTypeRule>(this, &EnumTypeRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<EnumTypeRule>(this, &EnumTypeRule::A1Action));
        Cm::Parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal("Specifiers");
        specifiersNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<EnumTypeRule>(this, &EnumTypeRule::PostSpecifiers));
        Cm::Parsing::NonterminalParser* enumTypeIdNonterminalParser = GetNonterminal("enumTypeId");
        enumTypeIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<EnumTypeRule>(this, &EnumTypeRule::PostenumTypeId));
        Cm::Parsing::NonterminalParser* enumConstantsNonterminalParser = GetNonterminal("EnumConstants");
        enumConstantsNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<EnumTypeRule>(this, &EnumTypeRule::PreEnumConstants));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new EnumTypeNode(span, context.fromSpecifiers, context.fromenumTypeId);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->GetSpan().SetEnd(span.End());
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
    void PostenumTypeId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromenumTypeId_value = std::move(stack.top());
            context.fromenumTypeId = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromenumTypeId_value.get());
            stack.pop();
        }
    }
    void PreEnumConstants(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::EnumTypeNode*>(context.value)));
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromSpecifiers(), fromenumTypeId() {}
        ParsingContext* ctx;
        Cm::Ast::EnumTypeNode* value;
        Cm::Ast::Specifiers fromSpecifiers;
        Cm::Ast::IdentifierNode* fromenumTypeId;
    };
    std::stack<Context> contextStack;
    Context context;
};

class EnumerationGrammar::EnumConstantsRule : public Cm::Parsing::Rule
{
public:
    EnumConstantsRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::EnumTypeNode*", "enumType"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> enumType_value = std::move(stack.top());
        context.enumType = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::EnumTypeNode*>*>(enumType_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<EnumConstantsRule>(this, &EnumConstantsRule::A0Action));
        Cm::Parsing::NonterminalParser* enumConstantNonterminalParser = GetNonterminal("EnumConstant");
        enumConstantNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<EnumConstantsRule>(this, &EnumConstantsRule::PreEnumConstant));
        enumConstantNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<EnumConstantsRule>(this, &EnumConstantsRule::PostEnumConstant));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.enumType->AddConstant(context.fromEnumConstant);
    }
    void PreEnumConstant(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::EnumTypeNode*>(context.enumType)));
    }
    void PostEnumConstant(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromEnumConstant_value = std::move(stack.top());
            context.fromEnumConstant = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromEnumConstant_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), enumType(), fromEnumConstant() {}
        ParsingContext* ctx;
        Cm::Ast::EnumTypeNode* enumType;
        Cm::Ast::Node* fromEnumConstant;
    };
    std::stack<Context> contextStack;
    Context context;
};

class EnumerationGrammar::EnumConstantRule : public Cm::Parsing::Rule
{
public:
    EnumConstantRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::EnumTypeNode*", "enumType"));
        SetValueTypeName("Cm::Ast::Node*");
        AddLocalVariable(AttrOrVariable("Span", "s"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> enumType_value = std::move(stack.top());
        context.enumType = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::EnumTypeNode*>*>(enumType_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<EnumConstantRule>(this, &EnumConstantRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<EnumConstantRule>(this, &EnumConstantRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<EnumConstantRule>(this, &EnumConstantRule::A2Action));
        Cm::Parsing::NonterminalParser* constantIdNonterminalParser = GetNonterminal("constantId");
        constantIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<EnumConstantRule>(this, &EnumConstantRule::PostconstantId));
        Cm::Parsing::NonterminalParser* constantValueNonterminalParser = GetNonterminal("constantValue");
        constantValueNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<EnumConstantRule>(this, &EnumConstantRule::PreconstantValue));
        constantValueNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<EnumConstantRule>(this, &EnumConstantRule::PostconstantValue));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.s = span;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.s.SetEnd(span.End());
        context.value = new EnumConstantNode(context.s, context.fromconstantId, context.fromconstantValue);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.s.SetEnd(span.End());
        context.value = new EnumConstantNode(context.s, context.fromconstantId, MakeNextEnumConstantValue(context.s, context.enumType));
    }
    void PostconstantId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromconstantId_value = std::move(stack.top());
            context.fromconstantId = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromconstantId_value.get());
            stack.pop();
        }
    }
    void PreconstantValue(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostconstantValue(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromconstantValue_value = std::move(stack.top());
            context.fromconstantValue = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromconstantValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), enumType(), value(), s(), fromconstantId(), fromconstantValue() {}
        ParsingContext* ctx;
        Cm::Ast::EnumTypeNode* enumType;
        Cm::Ast::Node* value;
        Span s;
        Cm::Ast::IdentifierNode* fromconstantId;
        Cm::Ast::Node* fromconstantValue;
    };
    std::stack<Context> contextStack;
    Context context;
};

void EnumerationGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parser.IdentifierGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parser::IdentifierGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar1)
    {
        grammar1 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar1);
    Cm::Parsing::Grammar* grammar2 = pd->GetGrammar("Cm.Parser.ExpressionGrammar");
    if (!grammar2)
    {
        grammar2 = Cm::Parser::ExpressionGrammar::Create(pd);
    }
    AddGrammarReference(grammar2);
    Cm::Parsing::Grammar* grammar3 = pd->GetGrammar("Cm.Parser.SpecifierGrammar");
    if (!grammar3)
    {
        grammar3 = Cm::Parser::SpecifierGrammar::Create(pd);
    }
    AddGrammarReference(grammar3);
}

void EnumerationGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("Identifier", this, "IdentifierGrammar.Identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("spaces_and_comments", this, "Cm.Parsing.stdlib.spaces_and_comments"));
    AddRuleLink(new Cm::Parsing::RuleLink("Expression", this, "ExpressionGrammar.Expression"));
    AddRuleLink(new Cm::Parsing::RuleLink("Specifiers", this, "SpecifierGrammar.Specifiers"));
    AddRule(new EnumTypeRule("EnumType", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::NonterminalParser("Specifiers", "Specifiers", 0),
                                new Cm::Parsing::KeywordParser("enum")),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::NonterminalParser("enumTypeId", "Identifier", 0)))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser('{'))),
                new Cm::Parsing::NonterminalParser("EnumConstants", "EnumConstants", 2)),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser('}'))))));
    AddRule(new EnumConstantsRule("EnumConstants", GetScope(),
        new Cm::Parsing::ListParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("EnumConstant", "EnumConstant", 2))),
            new Cm::Parsing::CharParser(','))));
    AddRule(new EnumConstantRule("EnumConstant", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("constantId", "Identifier", 0)),
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('='),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("constantValue", "Expression", 1))),
                new Cm::Parsing::ActionParser("A2",
                    new Cm::Parsing::EmptyParser())))));
    SetSkipRuleName("spaces_and_comments");
}

} } // namespace Cm.Parser
